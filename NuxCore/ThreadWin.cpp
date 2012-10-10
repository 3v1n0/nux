/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore.h"
#include "TextString.h"
#include "ThreadWin.h"


namespace nux
{

#define NUX_ATOMOP_ITERLOCKED_INCREMENT      InterlockedIncrement
#define NUX_ATOMOP_ITERLOCKED_DECREMENT      InterlockedDecrement
#define NUX_ATOMOP_ITERLOCKED_EXCHANGED      InterlockedExchange
#define NUX_ATOMOP_ITERLOCKED_VALUE

  long NThreadSafeCounter::Increment()
  {
    return NUX_ATOMOP_ITERLOCKED_INCREMENT ( &m_Counter );
  }
  long NThreadSafeCounter::Decrement()
  {
    return NUX_ATOMOP_ITERLOCKED_DECREMENT ( &m_Counter );
  }
  long NThreadSafeCounter::Set (long i)
  {
    return NUX_ATOMOP_ITERLOCKED_EXCHANGED ( &m_Counter, i );
  }

  long NThreadSafeCounter::GetValue() const
  {
    return m_Counter;
  }

  long NThreadSafeCounter::operator ++ ()
  {
    return Increment();
  }

  long NThreadSafeCounter::operator -- ()
  {
    return Decrement();
  }

  bool NThreadSafeCounter::operator == (long i)
  {
    return (m_Counter == i);
  }

  unsigned int  NThreadLocalStorage::m_TLSIndex[NThreadLocalStorage::NbTLS];
  BOOL NThreadLocalStorage::m_TLSUsed[NThreadLocalStorage::NbTLS];
  NThreadLocalStorage::TLS_ShutdownCallback  NThreadLocalStorage::m_TLSCallbacks[NThreadLocalStorage::NbTLS];


// http://msdn2.microsoft.com/en-us/library/xcb2z8hs(VS.80).aspx
//
// Usage: SetWin32ThreadName (-1, "MainThread");
//
#define MS_VC_EXCEPTION 0x406D1388

  typedef struct tagTHREADNAME_INFO
  {
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
  } THREADNAME_INFO;

  void SetWin32ThreadName (DWORD dwThreadID, LPCSTR szThreadName)
  {
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = szThreadName;
    info.dwThreadID = dwThreadID;
    info.dwFlags = 0;

    __try
    {
      RaiseException ( MS_VC_EXCEPTION, 0, sizeof (info) / sizeof (DWORD), (ULONG_PTR *) &info );
    }
    __except (EXCEPTION_CONTINUE_EXECUTION)
    {
    }
  }

  BOOL NThreadLocalStorage::RegisterTLS(unsigned int index, NThreadLocalStorage::TLS_ShutdownCallback shutdownCallback)
  {
    if (m_TLSUsed[index])
    {
      nuxDebugMsg("[NThreadLocalStorage::RegisterTLS] TLS has already been registered.");
      return TRUE;
    }

    if (!m_TLSUsed[index])
    {
      m_TLSIndex[index] = TlsAlloc();

      if (m_TLSIndex[index] == TLS_OUT_OF_INDEXES)
      {
        nuxAssertMsg (0, TEXT ("[NThreadLocalStorage::RegisterTLS] Out of TLS index.") );
      }

      m_TLSUsed[index]  = TRUE;
      m_TLSCallbacks[index] =  shutdownCallback;
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }

  void NThreadLocalStorage::Initialize()
  {
    for (unsigned int i = 0; i < NThreadLocalStorage::NbTLS; i++)
    {
      m_TLSUsed[i] = FALSE;
      // Fill the array with invalid values
      m_TLSIndex[i] = NThreadLocalStorage::InvalidTLS; // invalid index
    }
  }

  void NThreadLocalStorage::Shutdown()
  {
    ThreadShutdown();
  }

  void NThreadLocalStorage::ThreadInit()
  {
  }

  void NThreadLocalStorage::ThreadShutdown()
  {
    TLS_ShutdownCallback *callback = m_TLSCallbacks;

    for (unsigned int i = 0; i < NThreadLocalStorage::NbTLS; ++i, ++callback)
    {
      if (*callback)
      {
        (**callback) ();
      }
    }
  }

  NUX_IMPLEMENT_ROOT_OBJECT_TYPE (NThread);

  NThread::NThread()
    :   m_ThreadState (THREADINIT)
  {
    m_pThreadFunc = NThread::EntryPoint; // Can call Detach() also.
  }

  NThread::NThread (LPTHREAD_START_ROUTINE lpExternalRoutine)
  {
    Attach (lpExternalRoutine);
  }

  NThread::~NThread()
  {
    CloseHandle (m_ThreadCtx.m_hThread);
  }

  ThreadState NThread::Start ( void *arg )
  {
    if ( m_ThreadCtx.m_hThread == 0)
    {
      m_ThreadCtx.m_pUserData = arg;
      m_ThreadCtx.m_hThread = CreateThread (NULL,
                                            0,
                                            m_pThreadFunc,
                                            this,
                                            0 /*CREATE_SUSPENDED*/,
                                            (LPDWORD) &m_ThreadCtx.m_dwTID);

      if (m_ThreadCtx.m_hThread != 0)
      {
        //ResumeStart();
        m_ThreadState = THREADINIT;
        m_ThreadCtx.m_dwExitCode = (unsigned int) - 1;
        return m_ThreadState;
      }
      else
      {
        nuxDebugMsg (TEXT ("[NThread::Start] Cannot start thread: %s"), inlGetSystemErrorMessage() );
        m_ThreadState = THREAD_START_ERROR;
        return m_ThreadState;
      }

    }

    return m_ThreadState;
  }

  ThreadState NThread::Stop(bool bForceKill)
  {
// From MSDN
//    TerminateThread is used to cause a thread to exit. When this occurs, the target thread has no chance to execute any user-mode code and its initial stack is not deallocated. DLLs attached to the thread are not notified that the thread is terminating.
//        TerminateThread is a dangerous function that should only be used in the most extreme cases. You should call TerminateThread only if you know exactly what the target thread is doing, and you control all of the code that the target thread could possibly be running at the time of the termination. For example, TerminateThread can result in the following problems:
//    If the target thread owns a critical section, the critical section will not be released.
//        If the target thread is allocating memory from the heap, the heap lock will not be released.
//        If the target thread is executing certain kernel32 calls when it is terminated, the kernel32 state for the thread's process could be inconsistent.
//        If the target thread is manipulating the global state of a shared DLL, the state of the DLL could be destroyed, affecting other users of the DLL.

    // Attention: Calling Stop from another thread is not going to free the stack of this thread.
    // the stack is freed only if the thread exits by itself.
    if (m_ThreadCtx.m_hThread)
    {
      BOOL success = GetExitCodeThread(m_ThreadCtx.m_hThread, (LPDWORD) &m_ThreadCtx.m_dwExitCode);

      if ((m_ThreadCtx.m_dwExitCode == STILL_ACTIVE) && bForceKill)
      {
        // This will forcibly kill the thread! Read the doc on TerminateThread to find out about the consequences.
        TerminateThread(m_ThreadCtx.m_hThread, unsigned int (-1));
        CloseHandle(m_ThreadCtx.m_hThread);
      }

      m_ThreadCtx.m_hThread = NULL;
    }

    m_ThreadState = THREADSTOP;
    return m_ThreadState;
  }

  ThreadState NThread::Suspend()
  {
    unsigned int ret = SuspendThread(m_ThreadCtx.m_hThread);

    if (ret == 0xFFFFFFFF)
    {
      nuxDebugMsg(TEXT("[NThread::Suspend] Cannot suspend thread: %s"), inlGetSystemErrorMessage());
      return THREAD_SUSPEND_ERROR;
    }

    m_ThreadState = THREADSUSPENDED;
    return m_ThreadState;
  }

  ThreadState NThread::Resume()
  {
    unsigned int ret = ResumeThread(m_ThreadCtx.m_hThread);

    if (ret == 0xFFFFFFFF)
    {
      nuxDebugMsg(TEXT("[NThread::Suspend] Cannot resume thread: %s"), inlGetSystemErrorMessage());
      return THREAD_RESUME_ERROR;
    }

    if (ret == 1)
    {
      // If the return value is 1, the specified thread was suspended but was restarted.
      m_ThreadState = THREADRUNNING;
    }

    if (ret > 1)
    {
      // If the return value is greater than 1, the specified thread is still suspended.
      m_ThreadState = THREADSUSPENDED;
    }

    return m_ThreadState;
  }

// go from suspended to thread start
  ThreadState NThread::ResumeStart()
  {
    m_ThreadState = THREADINIT;
    unsigned int ret = ResumeThread(m_ThreadCtx.m_hThread);

    if (ret == 0xFFFFFFFF)
    {
      nuxDebugMsg(TEXT("[NThread::ResumeExit] Cannot resume thread: %s"), inlGetSystemErrorMessage());
      return THREAD_RESUME_ERROR;
    }

    if (ret == 1)
    {
      // If the return value is 1, the specified thread was suspended but was restarted.
      m_ThreadState = THREADINIT;
    }

    if (ret > 1)
    {
      nuxAssert(0); // should not happen
      // If the return value is greater than 1, the specified thread is still suspended.
      m_ThreadState = THREADINIT;
    }

    return m_ThreadState;
  }

// go from suspended to thread exit
  ThreadState NThread::ResumeExit()
  {
    m_ThreadState = THREADSTOP;
    unsigned int ret = ResumeThread(m_ThreadCtx.m_hThread);

    if (ret == 0xFFFFFFFF)
    {
      nuxDebugMsg(TEXT("[NThread::ResumeExit] Cannot resume thread: %s"), inlGetSystemErrorMessage());
      return THREAD_RESUME_ERROR;
    }

    if (ret == 1)
    {
      // If the return value is 1, the specified thread was suspended but was restarted.
      m_ThreadState = THREADSTOP;
    }

    if (ret > 1)
    {
      nuxAssert(0); // should not happen
      // If the return value is greater than 1, the specified thread is still suspended.
      m_ThreadState = THREADSTOP;
    }

    return m_ThreadState;
  }

  DWORD WINAPI NThread::EntryPoint(void *pArg)
  {
    NThread *pParent = reinterpret_cast<NThread *> (pArg);

    if (pParent == 0)
    {
      nuxDebugMsg(TEXT("[NThread::EntryPoint] Invalid pointer. The thread will exit."));
      return 0;
    }

    if (!pParent->ThreadCtor())
    {
      // return another message saying the thread could not execute due to error in ThreadCtor;
      return 0;
    }

    pParent->Run(pParent->m_ThreadCtx.m_pUserData);
    pParent->ThreadDtor();

    return 0;
  }

  unsigned int NThread::GetExitCode() const
  {
    if ( m_ThreadCtx.m_hThread )
      GetExitCodeThread (m_ThreadCtx.m_hThread, (LPDWORD) &m_ThreadCtx.m_dwExitCode);

    return m_ThreadCtx.m_dwExitCode;
  }

  HANDLE NThread::GetThreadHandle()
  {
    return m_ThreadCtx.m_hThread;
  }

  unsigned int NThread::GetThreadId()
  {
    return (unsigned int)m_ThreadCtx.m_dwTID;
  }

  ThreadState NThread::GetThreadState() const
  {
    return m_ThreadState;
  }

  void NThread::SetThreadState(ThreadState state)
  {
    m_ThreadState = state;
  }

  void NThread::SetThreadName(const TCHAR *ThreadName)
  {
    SetWin32ThreadName (GetThreadId(), TCHAR_TO_ANSI (ThreadName) );
    m_ThreadName = ThreadName;
  }

  const std::string& NThread::GetThreadName() const
  {
    return m_ThreadName;
  }

  ThreadWaitResult NThread::JoinThread(NThread *thread, unsigned int milliseconds)
  {
    if (thread == NULL)
    {
      return THREAD_WAIT_RESULT_FAILED;
    }

    unsigned int result = WaitForSingleObject(thread->GetThreadHandle(), milliseconds);

    switch(result)
    {
    case WAIT_OBJECT_0:
      return THREAD_WAIT_RESULT_COMPLETED;
    case WAIT_ABANDONED:
      return THREAD_WAIT_RESULT_ABANDONED;
    case WAIT_TIMEOUT:
      return THREAD_WAIT_RESULT_TIMEOUT;
    case WAIT_FAILED:
    default:
      return THREAD_WAIT_RESULT_FAILED;
    }
  }
}

