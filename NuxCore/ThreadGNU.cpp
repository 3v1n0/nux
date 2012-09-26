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


namespace nux
{

  int NThreadSafeCounter::Increment()
  {
    return __sync_add_and_fetch (&m_Counter, 1);
  }

  int NThreadSafeCounter::Decrement()
  {
    return __sync_add_and_fetch (&m_Counter, -1);
  }

  int NThreadSafeCounter::Set (int i)
  {
    return __sync_lock_test_and_set (&m_Counter, i);
  }

  int NThreadSafeCounter::GetValue() const
  {
    return m_Counter;
  }

  int NThreadSafeCounter::operator ++ ()
  {
    return Increment();
  }

  int NThreadSafeCounter::operator -- ()
  {
    return Decrement();
  }

  bool NThreadSafeCounter::operator == (int i)
  {
    return (m_Counter == i);
  }

  __thread void *NThreadLocalStorage::m_TLSIndex[NThreadLocalStorage::NbTLS];
  BOOL NThreadLocalStorage::m_TLSUsed[NThreadLocalStorage::NbTLS];
  NThreadLocalStorage::TLS_ShutdownCallback  NThreadLocalStorage::m_TLSCallbacks[NThreadLocalStorage::NbTLS];

  BOOL NThreadLocalStorage::RegisterTLS(unsigned int index, NThreadLocalStorage::TLS_ShutdownCallback shutdownCallback)
  {
    NUX_RETURN_VALUE_IF_FALSE(index < NThreadLocalStorage::NbTLS, FALSE);
    NUX_RETURN_VALUE_IF_TRUE(m_TLSUsed[index], TRUE); // already registered

    m_TLSUsed[index]  = TRUE;
    m_TLSCallbacks[index] =  shutdownCallback;
    return TRUE;
  }

  BOOL NThreadLocalStorage::UnRegisterTLS (unsigned int index)
  {
    NUX_RETURN_VALUE_IF_FALSE(index < NThreadLocalStorage::NbTLS, FALSE);
    NUX_RETURN_VALUE_IF_FALSE(m_TLSUsed[index], FALSE);

    m_TLSUsed[index]  = FALSE;
    m_TLSCallbacks[index] =  NULL;
    m_TLSIndex[index]  = NULL;

    return TRUE;
  }

  void NThreadLocalStorage::Initialize()
  {
    for (unsigned int i = 0; i < NThreadLocalStorage::NbTLS; i++)
    {
      m_TLSUsed[i] = FALSE;
      // Fill the array with invalid values
      m_TLSIndex[i] = 0;
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

  NThread::NThread (ThreadRoutineFunc lpExternalRoutine)
  {
    Attach (lpExternalRoutine);
  }

  NThread::~NThread()
  {
    if (m_ThreadCtx.m_dwTID)
      pthread_detach (m_ThreadCtx.m_dwTID);
  }

  ThreadState NThread::Start ( void *arg )
  {
    m_ThreadCtx.m_pUserData = arg;
    int ret = pthread_create (&m_ThreadCtx.m_dwTID,
                              NULL,
                              m_pThreadFunc,
                              this);

    if (ret != 0)
    {
      nuxDebugMsg (TEXT ("[NThread::Start] Cannot start thread.") );
      m_ThreadState = THREAD_START_ERROR;
      return m_ThreadState;
    }

    return m_ThreadState;
  }

  ThreadState NThread::Stop ( bool /* bForceKill */ )
  {
    int ret = pthread_detach (m_ThreadCtx.m_dwTID);

    if (ret != 0)
    {
      nuxDebugMsg (TEXT ("[NThread::Stop] Cannot detach thread.") );
      m_ThreadState = THREAD_STOP_ERROR;
      return m_ThreadState;
    }

    m_ThreadState = THREADSTOP;
    return m_ThreadState;
  }

  ThreadState NThread::Suspend()
  {
    m_ThreadState = THREADSUSPENDED;
    return m_ThreadState;
  }

  ThreadState NThread::Resume()
  {
    return m_ThreadState;
  }

// go from suspended to thread start
  ThreadState NThread::ResumeStart()
  {
    m_ThreadState = THREADINIT;
    return m_ThreadState;
  }

// go from suspended to thread exit
  ThreadState NThread::ResumeExit()
  {
    m_ThreadState = THREADSTOP;
    return m_ThreadState;
  }

  void *NThread::EntryPoint (void *pArg)
  {
    NThread *pParent = reinterpret_cast<NThread *> (pArg);

    if (pParent == 0)
    {
      nuxDebugMsg (TEXT ("[NThread::EntryPoint] Invalid pointer. The thread will exit.") );
      return 0;
    }

    if (!pParent->ThreadCtor() )
    {
      // return another message saying the thread could not execute due to error in ThreadCtor;
    }

    pParent->Run ( pParent->m_ThreadCtx.m_pUserData );

    pParent->ThreadDtor();
    return 0;
  }

  unsigned int NThread::GetExitCode() const
  {
    return m_ThreadCtx.m_dwExitCode;
  }

  pthread_t NThread::GetPThread()
  {
    return m_ThreadCtx.m_dwTID;
  }

  ThreadState NThread::GetThreadState() const
  {
    return m_ThreadState;
  }

  void NThread::SetThreadState (ThreadState state)
  {
    m_ThreadState = state;
  }

  ThreadWaitResult NThread::JoinThread(NThread *thread, unsigned int /* milliseconds */)
  {
    if (thread == NULL)
    {
      return THREAD_WAIT_RESULT_FAILED;
    }

    void *value_ptr = NULL;
    int result = pthread_join(thread->GetPThread(), &value_ptr);

    switch(result)
    {
    case 0:
      return THREAD_WAIT_RESULT_COMPLETED;
    default:
      return THREAD_WAIT_RESULT_FAILED;
    }
  }
}
