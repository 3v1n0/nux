/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "Nux.h"

#include "StackManager.h"
#include "TimerProc.h"
#include "WindowThread.h"
#include "SystemThread.h"

#include "GfxServer.h"

NAMESPACE_BEGIN_GUI

extern long gNumArea;

static NCriticalSection ApplicationArrayLock;
GfxServerImpl* GfxServerImpl::m_pInstance = 0;

GfxServerImpl::GfxServerImpl(/* Pass the command line here */)
{
    inlRegisterThreadLocalIndex(0, ThreadLocal_InalogicAppImpl, NULL);

    // Initialize threading for OpenGL Windows.
    gGLWindowManager;

};

GfxServerImpl::~GfxServerImpl()
{
}

GfxServerImpl& GfxServerImpl::Instance()
{
    if(m_pInstance == 0)
    {
        m_pInstance = new GfxServerImpl();
    }
    return *m_pInstance;
}

//---------------------------------------------------------------------------------------------------------
// This is the main thread. it supervises all other threads and maintain them in a structure.
// When there are no more threads running, the main thread exits and the programs terminates.
#if defined(INL_OS_WINDOWS)
int GfxServerImpl::Run()
{
    std::vector<NThread*>::iterator it;
    bool EndProgram = false;
    int ExitCode = 0;

    // Threads must have been created before this function is called. If not then the main thread terminates and the program
    // will exit if nothing else is done.
    if(ApplicationArray.size() == 0)
        EndProgram = true;

    MSG		msg;
    while(!EndProgram && GetMessage(&msg, NULL, 0, 0))	// Is There A Message Waiting? If yes, remove it.
    {
        if(msg.message == INL_THREADMSG_THREAD_TERMINATED)
        {
            NScopeLock Scope(&ApplicationArrayLock);
            it = ApplicationArray.begin();
            while(it != ApplicationArray.end())
            {
                if((WaitForSingleObject((*it)->GetThreadHandle(), 0) == WAIT_OBJECT_0) || (msg.wParam == (*it)->GetThreadId() ))
                {
                    // A thread object's state is signaled when the thread terminates.
                    //(*it)->Stop();
                    // Erasing while traversing:
                    //      Save the current iterator in temp.
                    //      Let it go back to the preceding iterator.
                    //      Erase temp from the array

                    std::vector<NThread*>::iterator temp = it;
                    delete (*temp);
                    it = ApplicationArray.erase(temp);
                }
                else
                {
                    it++;
                }
            }
        }
        if(ApplicationArray.size() == 0)
            EndProgram = true;
    }

    return ExitCode;
}
#elif defined(INL_OS_LINUX)
int GfxServerImpl::Run()
{
    std::vector<NThread*>::iterator it;
    bool EndProgram = false;
    int ExitCode = 0;

    // Threads must have been created before this function is called. If not then the main thread terminates and the program
    // will exit if nothing else is done.
    if(ApplicationArray.size() == 0)
        EndProgram = true;

    while(1)
    {
        SleepForMilliseconds(30);
    }

    return ExitCode;
}
#else
#error GfxServerImpl::Run() is not implemented for this platform.
#endif
//---------------------------------------------------------------------------------------------------------
WindowThread* GfxServerImpl::CreateGUIThread(const TCHAR* WindowTitle,
                                             UINT width,
                                             UINT height,
                                             WindowThread *Parent,
                                             ThreadUserInitFunc UserInitFunc,
                                             void* InitData)
{
    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, 0);
    if(GetThreadApplication())
    {   
        // An WindowThread already exist for this thread.
        inlAssertMsg(0, "[GfxServerImpl::CreateGUIThread] Only one WindowThread per thread is allowed");
        return 0;
    }
    
    WindowThread* myApplication = new WindowThread(WindowTitle, width, height, 0, true);

    if(myApplication == 0)
    {
        inlAssertMsg(0, TEXT("[GfxServerImpl::CreateGUIThread] WindowThread creation failed."));
    }

    myApplication->m_UserInitFunc = UserInitFunc;
    myApplication->m_UserExitFunc = 0;
    myApplication->m_InitData = InitData;
    myApplication->m_ExitData = 0;
    myApplication->SetWindowStyle(WINDOWSTYLE_NORMAL);
    //ApplicationArray.push_back(myApplication);

    myApplication->Start(0);

    int wait_time = 0;
    while(myApplication->GetThreadState() == THREADINIT)
    {
        // Sleep for 1 milliseconds. Exit when the thread is running
        SleepForMilliseconds(1);
        wait_time++;
        if(wait_time > 1000)
        {
            // A second has passed and the thread is still not running. There could be a problem
            // Prepare to exit gracefully.
        }
    }
    return myApplication;
}

// Create a window thread that is a child of the Parent. This thread has a window.
WindowThread* GfxServerImpl::CreateWindowThread(WindowStyle WndStyle,
                                                   const TCHAR* WindowTitle,
                                    UINT width,
                                    UINT height,
                                    WindowThread *Parent,
                                    ThreadUserInitFunc UserInitFunc,
                                    void* InitData)
{
    return _CreateModalWindowThread(WndStyle, WindowTitle, width, height, Parent, UserInitFunc, InitData, false);
}

// Create modal graphics thread that is a child of the Parent. This thread has a window.
WindowThread* GfxServerImpl::CreateModalWindowThread(WindowStyle WndStyle,
                                                        const TCHAR* WindowTitle,
                                                        UINT width,
                                                        UINT height,
                                                        WindowThread *Parent,
                                                        ThreadUserInitFunc UserInitFunc,
                                                        void* InitData)
{
    return _CreateModalWindowThread(WndStyle, WindowTitle, width, height, Parent, UserInitFunc, InitData, true);
}

WindowThread* GfxServerImpl::_CreateModalWindowThread(WindowStyle WndStyle,
                                                      const TCHAR* WindowTitle,
                                                      UINT width,
                                                      UINT height,
                                                      WindowThread *Parent,
                                                      ThreadUserInitFunc UserInitFunc,
                                                      void* InitData,
                                                      bool Modal)
{
    // check that Parent exist
    WindowThread* pWindow = new WindowThread(WindowTitle, width, height, Parent, Modal);
    pWindow->m_UserInitFunc = UserInitFunc;
    pWindow->m_UserExitFunc = 0;
    pWindow->m_InitData = InitData;
    pWindow->m_ExitData = 0;

    pWindow->SetWindowStyle(WndStyle);
    return pWindow;
}

SystemThread* GfxServerImpl::CreateSimpleThread(AbstractThread *Parent, ThreadUserInitFunc UserInitFunc, void* InitData)
{
    SystemThread* simpleThread = new SystemThread(Parent);
    simpleThread->m_UserInitFunc = UserInitFunc;
    simpleThread->m_UserExitFunc = 0;
    simpleThread->m_InitData = InitData;
    simpleThread->m_ExitData = 0;
    return static_cast<SystemThread*>(simpleThread);
}

bool GfxServerImpl::RegisterRunningApplication(NThread* app)
{
    NScopeLock Scope(&ApplicationArrayLock);
    inlAssert(app);
    if(app == 0)
        return FALSE;

    std::vector<NThread*>::iterator it = find(ApplicationArray.begin(), ApplicationArray.end(), app);
    if(it == ApplicationArray.end())
    {
        //app->Start(0);
        ApplicationArray.push_back(app);
    }
    return TRUE;
}

// WARNING: This function is not reliable if the thread has just been created. The thread may not have called 
// RegisterRunningApplication yet and then GetThreadState will return an incorrect result.
// You may call NThread->GetThreadState() for a better reliability.
ThreadState GfxServerImpl::GetThreadState(unsigned int ThreadID)
{
    std::vector<NThread*>::iterator it;
    for(it = ApplicationArray.begin(); it != ApplicationArray.end(); it++)
    {
        if((*it)->GetThreadId() == ThreadID)
        {
            return (*it)->GetThreadState();
            break;
        }
    }
    return THREADSTOP;
}

GLWindowImpl& GetWindow()
{
    NThread* thread = GetThreadApplication();
    if(!thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        inlAssertMsg(0, TEXT("[GfxServerImpl::GetWindow] You can't call GetWindow on this type of thread: s"), thread->Type().GetName());
    }
    return (static_cast<WindowThread*> (thread))->GetWindow();
}

GraphicsContext& GetGraphicsContext()
{
    NThread* thread = GetThreadApplication();
    if(!thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        inlAssertMsg(0, TEXT("[GfxServerImpl::GetGraphicsContext] You can't call GetGraphicsContext on this type of thread: s"), thread->Type().GetName());
    }
    return (static_cast<WindowThread*> (thread))->GetGraphicsContext();
}

StackManagerImpl& GetThreadStackManager()
{
    NThread* thread = GetThreadApplication();
    if(!thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        inlAssertMsg(0, TEXT("[GetThreadStackManager] You can't call GetThreadStackManager on this type of thread: s"), thread->Type().GetName());
        inlOutputDebugString(TEXT("[GetThreadStackManager] You can't call GetThreadStackManager on this type of thread: s"), thread->Type().GetName());
        INL_HARDWARE_BREAK;
    }
    return (static_cast<WindowThread*> (thread))->GetStackManager();
}

NThread* GetThreadApplication()
{
    NThread* thread = static_cast<NThread*>(inlGetThreadLocalStorage(ThreadLocal_InalogicAppImpl));
    if(thread == 0)
    {
    }
    return thread;
}

WindowThread* GetGraphicsThread()
{
    NThread* thread = GetThreadApplication();
    inlAssertMsg(thread, TEXT("[GetGraphicsThread] The current thread is not of the type WindowThread."));
    if(!thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        inlAssertMsg(0, TEXT("[GetGraphicsThread] You can't call GetGraphicsThread on this type of thread: s"), thread->Type().GetName());
        return 0;
    }
    return static_cast<WindowThread*>(inlGetThreadLocalStorage(ThreadLocal_InalogicAppImpl));
}

BasePainter& GetThreadPainter()
{
    NThread* thread = GetThreadApplication();
    inlAssertMsg(thread, TEXT("[GetGraphicsThread] The current thread is not of the type WindowThread."));
    if(!thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        inlAssertMsg(0, TEXT("[GetThreadPainter] You can't call GetThreadPainter on this type of thread: s"), thread->Type().GetName());
    }
    return (static_cast<WindowThread*> (thread))->GetPainter();
}

UXTheme& GetThreadTheme()
{
    NThread* thread = GetThreadApplication();
    inlAssertMsg(thread, TEXT("[GetThreadTheme] The current thread is not of the type WindowThread."));
    if(!thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        inlAssertMsg(0, TEXT("[GetThreadTheme] You can't call GetThreadTheme on this type of thread: s"), thread->Type().GetName());
    }
    return (static_cast<WindowThread*> (thread))->GetTheme();
}

TimerHandler& GetThreadTimer()
{
    NThread* thread = GetThreadApplication();
    inlAssertMsg(thread, TEXT("[GetGraphicsThread] The current thread is not of the type WindowThread."));
    if(!thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        inlAssertMsg(0, TEXT("[GetThreadTimer] You can't call GetThreadTimer on this type of thread: s"), thread->Type().GetName());
        inlOutputDebugString(TEXT("[GetThreadTimer] You can't call GetThreadTimer on this type of thread: s"), thread->Type().GetName());
        INL_HARDWARE_BREAK;
    }
    return (static_cast<WindowThread*> (thread))->GetTimerHandler();
}

NAMESPACE_END_GUI
