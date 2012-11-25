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


#ifndef THREADWIN_H
#define THREADWIN_H

#include "ObjectType.h"

namespace nux
{

  class NThreadSafeCounter
  {
  public:
    NThreadSafeCounter()
    {
      m_Counter = 0;
    }
    NThreadSafeCounter (long i)
    {
      m_Counter = i;
    }
    long Increment();
    long Decrement();
    long Set (long i);
    long GetValue() const;
    long operator ++ ();
    long operator -- ();
    bool operator == (long i);
  private:
    long m_Counter;
  };

  class NCriticalSection
  {
  public:
    //! Initialize critical section.
    /*!
        Initialize critical section.
    */
    NCriticalSection()
    {
      InitializeCriticalSection (&m_lock);
    }

    //! Destroy critical section.
    /*!
        Destroy critical section.
    */
    ~NCriticalSection()
    {
      DeleteCriticalSection (&m_lock);
    }

    //! Enter critical section.
    /*!
        Enter critical section. This function is made const so it can be used without restriction.
        For that matter, m_lock is made mutable.
    */
    void Lock() const
    {
      EnterCriticalSection (&m_lock);
    }

    //! Leave critical section.
    /*!
        Leave critical section. This function is made const so it can be used without restriction.
        For that matter, m_lock is made mutable.
    */
    void Unlock() const
    {
      LeaveCriticalSection (&m_lock);
    }

  private:
    //! Prohibit copy constructor.
    /*!
        Prohibit copy constructor.
    */
    NCriticalSection (const NCriticalSection &);
    //! Prohibit assignment operator.
    /*!
        Prohibit assignment operator.
    */
    NCriticalSection &operator= (const NCriticalSection &);

    mutable CRITICAL_SECTION m_lock;
  };

//! Scope Lock class
  /*!
      Takes a critical section object as parameter of the constructor.
      The constructor locks the critical section.
      The destructor unlocks the critical section.
  */
  class NScopeLock
  {
  public:
    //! The constructor locks the critical section object.
    /*!
        The constructor locks the critical section object.
        @param  LockObject      Critical section object.
    */
    NScopeLock (NCriticalSection *CriticalSectionObject)
      : m_CriticalSectionObject (CriticalSectionObject)
    {
      nuxAssert (m_CriticalSectionObject);
      m_CriticalSectionObject->Lock();
    }

    //! The destructor unlocks the critical section object.
    /*!
        The destructor unlocks the critical section object.
    */
    ~NScopeLock (void)
    {
      nuxAssert (m_CriticalSectionObject);
      m_CriticalSectionObject->Unlock();
    }

  private:
    //! Prohibit default constructor.
    /*!
        Prohibit default constructor.
    */
    NScopeLock (void);

    //! Prohibit copy constructor.
    /*!
        Prohibit copy constructor.
    */
    NScopeLock (const NScopeLock &ScopeLockObject);

    //! Prohibit assignment operator.
    /*!
        Prohibit assignment operator.
    */
    NScopeLock &operator= (const NScopeLock &ScopeLockObject)
    {
      return *this;
    }

    //! Critical section Object.
    /*!
        Critical section Object.
    */
    NCriticalSection *m_CriticalSectionObject;
  };

  class NThreadLocalStorage
  {
  public:
    enum
    {
      NbTLS = 128,
      InvalidTLS = 0xFFFFFFFF
    };

    typedef void (*TLS_ShutdownCallback) ();

    static BOOL                     m_TLSUsed[NbTLS];
    static unsigned int                     m_TLSIndex[NbTLS];
    static TLS_ShutdownCallback     m_TLSCallbacks[NbTLS];

    static void Initialize();
    static void Shutdown();
    static BOOL RegisterTLS (unsigned int index, TLS_ShutdownCallback shutdownCallback);
    static void ThreadInit();
    static void ThreadShutdown();

  public:

    template<class T> static inline T GetData (unsigned int index)
    {
      nuxAssert (sizeof (T) <= sizeof (size_t) );
      nuxAssert (index < NbTLS);
      nuxAssert (m_TLSUsed[index]);

      // T and (unsigned long) can be of different sizes
      // but this limits the use of GetData to classes without copy constructors
      union
      {
        T               t;
        void           *v;
      } temp;
      temp.v = TlsGetValue (m_TLSIndex[index]);
      return temp.t;
    }

    template<class T> static inline void SetData (unsigned int index, T value)
    {
      nuxAssert (sizeof (T) <= sizeof (size_t) );
      nuxAssert (index < NbTLS);
      nuxAssert (m_TLSUsed[index]);

      // T and (unsigned long) can be of different sizes
      // but this limits the use of GetData to classes without copy constructors
      union
      {
        T               t;
        void           *v;
      } temp;
      temp.t = value;
      BOOL b = TlsSetValue (m_TLSIndex[index], temp.v);
      nuxAssertMsg (b, TEXT ("[NThreadLocalStorage::SetData] TlsSetValue returned FALSE.") );
    }
  };

#define inlDeclareThreadLocalStorage(type, index, name)  \
struct    ThreadLocalStorageDef##name { enum Const { Index = index}; };\
inline    type GetTLS_##name() { return nux::NThreadLocalStorage::GetData<type>(ThreadLocalStorageDef##name::Index); }\
inline    void SetTLS_##name(type value) { nux::NThreadLocalStorage::SetData<type>(ThreadLocalStorageDef##name::Index, value); }

#define inlRegisterThreadLocalIndex(index, name, shutdownCallback) \
    nuxVerifyExpr(index == ThreadLocalStorageDef##name::Index); \
    nuxVerifyExpr(nux::NThreadLocalStorage::RegisterTLS(index, shutdownCallback))

#define inlGetThreadLocalStorage(name)      GetTLS_##name()
#define inlSetThreadLocalStorage(name, value)  SetTLS_##name(value)

#ifdef POP_CHECK_THREADS
#define  nuxAssertInsideThread(threadtype)               nuxAssert( inlGetThreadLocalStorage(ThreadType) == threadtype)
#define  nuxAssertInsideThread2(threadtype1, threadtype2) nuxAssert( inlGetThreadLocalStorage(ThreadType) == threadtype1 || popGetThreadLocalData(ThreadType) == threadtype2)
#define nuxAssertNotInsideThread(threadtype)             nuxAssert( inlGetThreadLocalStorage(ThreadType) != threadtype)
#else
#define  nuxAssertInsideThread(threadtype)  ((void) 0)
#define  nuxAssertInsideThread2(threadtype1, threadtype2)  ((void) 0)
#define nuxAssertNotInsideThread(threadtype) ((void) 0)
#endif

  void SetWin32ThreadName (DWORD dwThreadID, LPCSTR szThreadName);


  enum ThreadState
  {
    THREADINIT,
    THREADRUNNING,
    THREADSUSPENDED,
    THREADSTOP,
    THREAD_START_ERROR,
    THREAD_STOP_ERROR,
    THREAD_SUSPEND_ERROR,
    THREAD_RESUME_ERROR,
  };

  enum ThreadWaitTimeout
  {
    THREAD_WAIT_TIMEOUT_NONE    = 0,
    THREAD_WAIT_TIMEOUT_FOREVER = INFINITE,
  };

  enum ThreadWaitResult
  {
    THREAD_WAIT_RESULT_COMPLETED  = 0,
    THREAD_WAIT_RESULT_ABANDONED  = 1,
    THREAD_WAIT_RESULT_TIMEOUT    = 2,
    THREAD_WAIT_RESULT_FAILED     = 3,
  };

// http://www.codeguru.com/cpp/misc/misc/threadsprocesses/article.php/c3793/
  class NThread
  {
    NUX_DECLARE_ROOT_OBJECT_TYPE (NThread);
  public:
    /*!
      Info: Default Constructor
    */
    NThread();

    /*!
        Info: Plug Constructor

        Use this to migrate/port existing worker threads to objects immediately
        Although you lose the benefits of ThreadCTOR and ThreadDTOR.
    */
    NThread (LPTHREAD_START_ROUTINE lpExternalRoutine);

    /*!
        Info: Default Destructor

        I think it is wise to destroy the thread even if it is running,
        when the main thread reaches here.
    */
    virtual ~NThread();

    /*!
        Info: Starts the thread.

        This function starts the thread pointed by m_pThreadFunc with default attributes
    */
    virtual ThreadState Start ( void *arg = NULL );

    /*!
        Info: Stops the thread.

        This function stops the current thread.
        We can force kill a thread which results in a TerminateThread.
    */
    virtual ThreadState Stop ( bool bForceKill = false );

    ThreadState Suspend();
    ThreadState Resume();
    ThreadState ResumeStart();
    ThreadState ResumeExit();

    /*!
        Info: Starts the thread.

        This function starts the thread pointed by m_pThreadFunc with default attributes
    */
    unsigned int GetExitCode() const;

    /*!
        Info: Attaches a Thread Function

        Used primarily for porting but can serve in developing generic thread objects
    */
    void Attach ( LPTHREAD_START_ROUTINE lpThreadFunc )
    {
      m_pThreadFunc = lpThreadFunc;
    }

    /*!
        Info: Detaches the Attached Thread Function

        Detaches the Attached Thread Function, If any.
        by resetting the thread function pointer to EntryPoint1
    */
    void  Detach ( void )
    {
      m_pThreadFunc = NThread::EntryPoint;
    }

    HANDLE GetThreadHandle();
    unsigned int GetThreadId();


    ThreadState GetThreadState() const;
    void SetThreadState (ThreadState state);

    void SetThreadName (const TCHAR *ThreadName);
    const std::string& GetThreadName() const;

    /*!
        Wait for a thread to complete.
        The second parameters to this call specifies a how long to wait for the thread to complete.
        The following options are available:
          ThreadWaitTimeout::THREAD_WAIT_TIMEOUT_NONE: The function returns immediately if the thread exits.
          ThreadWaitTimeout::THREAD_WAIT_TIMEOUT_FOREVER: The function waits until the thread exits.

        @param thread Pointer to a valid NThread.
        @param milliseconds Time to wait for the thread to complete.
    */
    static ThreadWaitResult JoinThread(NThread *thread, unsigned int milliseconds);

  protected:
    std::string m_ThreadName;

    volatile ThreadState m_ThreadState;

    /*!
        Info: DONT override this method.

        This function is like a standard template.
        Override if you are sure of what you are doing.

        In C++ the entry function of a thread cannot be a normal member function of a class.
        However, it can be a static member function of a class. This is what we will use as the entry point.
        There is a gotcha here though. Static member functions do not have access to the this pointer of a C++ object.
        They can only access static data. Fortunately, there is way to do it. Thread entry point functions take a void * as
        a parameter so that the caller can typecast any data and pass in to the thread. We will use this to pass this to
        the static function. The static function will then typecast the void * and use it to call a non static member function
    */
    static DWORD WINAPI EntryPoint (void *pArg);

    /*!
        Info: Override this method.

        This function should contain the body/code of your thread.
        Notice the signature is similar to that of any worker thread function
        except for the calling convention.
    */
    virtual int Run (void* /* arg */ )
    {
      return m_ThreadCtx.m_dwExitCode;
    }

    /*!
        Info: Constructor-like function.

        Will be called by EntryPoint before executing the thread body.
        Override this function to provide your extra initialization.

        NOTE: do not confuse it with the classes constructor
        @return TRUE if the thread can continue running the program. If FALSE is returned, the thread won't execute the main body Run() and will exit without calling ThreadDtor.
    */
    virtual bool ThreadCtor()
    {
      return true;
    }

    /*!
        Info: Destructor-like function.

        Will be called by EntryPoint after executing the thread body.
        Override this function to provide your extra destruction.

        NOTE: do not confuse it with the classes constructor
        @return TRUE if this function executed without problems.
    */
    virtual bool ThreadDtor()
    {
      return true;
    }

  private:
    /*!
        Info: Thread Context Inner Class

        Every thread object needs to be associated with a set of values.
        like UserData Pointer, Handle, Thread ID etc.

        NOTE: This class can be enhanced to varying functionalities
        eg.,
            * Members to hold StackSize
            * SECURITY_ATTRIBUTES member.
    */
    class NThreadContext
    {
    public:
      NThreadContext()
      {
        memset (this, 0, sizeof (this) );
      }

      /*
      *  Attributes Section
      */
    public:
      HANDLE m_hThread;                 //!< The Thread Handle.
      volatile unsigned int  m_dwTID;   //!< The Thread ID.
      void *m_pUserData;                //!< The user data pointer.
      void *m_pParent;                  //!< The this pointer of the parent NThread object.
      unsigned int  m_dwExitCode;       //!< The Exit Code of the thread.
    };

    /*!
        Attributes Section
    */
  protected:
    /*!
        Info: Members of NThread
    */
    NThreadContext      m_ThreadCtx;  //  The Thread Context member
    LPTHREAD_START_ROUTINE  m_pThreadFunc;  //  The Worker Thread Function Pointer
  };

//  USAGE:
//    DWORD WINAPI Threaded(void* lpData);
//
//    class CDemoThread : public CThread
//    {
//        virtual unsigned int Run( void* /* arg */ )
//        {
//            for(;;)
//            {
//                printf("Threaded Object Code \n");
//                Sleep(1000);
//            }
//        }
//    };
//
//    void main( void )
//    {
//        CDemoThread dmt;
//        dmt.Start(NULL);
//        SleepEx(15 * 1000, FALSE);
//        dmt.Stop(true);
//
//        //  A Sample Code for porting existent code of Threaded function
//        CThread t1(Threaded), t2;
//        t2.Attach(Threaded);
//        t1.Start();
//        t2.Start();
//        SleepEx(15 * 1000, FALSE);
//        t2.Stop();
//        t1.Stop();
//    }
//
//    DWORD WINAPI Threaded( void* /* lpData */ )
//    {
//        for(;;)
//        {
//            printf("worker threaded code");
//            Sleep(1000);
//        }
//    }

}

#endif // THREADWIN_H

