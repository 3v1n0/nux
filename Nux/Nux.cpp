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


#include "Nux.h"
#include "NuxGraphics/NuxGraphics.h"

namespace nux
{

  static NCriticalSection ThreadArrayLock;
  std::vector<NThread *> ThreadArray;

  void NuxInitialize(const char *CommandLine)
  {
    nux::NuxCoreInitialize(0);
    nux::NuxGraphicsInitialize();

    // Register a thread local store for the WindowThreads. Each window thread will be able to access its own WindowThread pointer.
    inlRegisterThreadLocalIndex(0, ThreadLocal_InalogicAppImpl, NULL);
  }

  static WindowThread *_CreateModalWindowThread(const char *WindowTitle,
      unsigned int width,
      unsigned int height,
      WindowThread *Parent,
      ThreadUserInitFunc UserInitFunc,
      void *InitData,
      bool Modal)
  {
    // check that Parent exist
    WindowThread *w = new WindowThread(WindowTitle, width, height, Parent, Modal);

    if (w == 0)
    {
      nuxAssertMsg(0, "[_CreateModalWindowThread] WindowThread creation failed.");
      return 0;
    }

    return w;
  }

  WindowThread *CreateGUIThread(const char *WindowTitle,
                                 unsigned int width,
                                 unsigned int height,
                                 WindowThread *Parent,
                                 ThreadUserInitFunc UserInitFunc,
                                 void *InitData)
  {
    if (GetWindowThread())
    {
      // An WindowThread already exist for this thread.
      nuxAssertMsg(0, "[CreateGUIThread] Only one WindowThread per thread is allowed");
      return 0;
    }

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, 0);

    WindowThread *w = new WindowThread(WindowTitle, width, height, 0, true);

    if (w == 0)
    {
      nuxAssertMsg(0, "[CreateGUIThread] WindowThread creation failed.");
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle(WINDOWSTYLE_NORMAL);
    w->ThreadCtor();
    return w;
  }

#if defined(NUX_OS_WINDOWS)
  WindowThread *CreateFromForeignWindow(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext,
                                         ThreadUserInitFunc UserInitFunc,
                                         void *InitData
                                        )
  {
    if (GetWindowThread())
    {
      // An WindowThread already exist for this thread.
      nuxAssertMsg(0, "[CreateGUIThread] Only one WindowThread per thread is allowed");
      return 0;
    }

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, 0);

    WindowThread *w = new WindowThread("WindowTitle", 400, 300, 0, true);

    if (w == 0)
    {
      nuxAssertMsg(0, "[CreateGUIThread] WindowThread creation failed.");
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle(WINDOWSTYLE_NORMAL);
    w->ThreadCtor(WindowHandle, WindowDCHandle, OpenGLRenderingContext);
    return w;
  }

#elif defined(NUX_OS_LINUX)
#ifdef NUX_OPENGLES_20
  WindowThread *CreateFromForeignWindow (Window X11Window, EGLContext OpenGLContext,
                                         ThreadUserInitFunc UserInitFunc,
                                         void *InitData)
#else
  WindowThread *CreateFromForeignWindow (Window X11Window, GLXContext OpenGLContext,
                                         ThreadUserInitFunc UserInitFunc,
                                         void *InitData)
#endif
  {
    if (GetWindowThread())
    {
      // An WindowThread already exist for this thread.
      nuxAssertMsg(0, "[CreateGUIThread] Only one WindowThread per thread is allowed");
      return 0;
    }

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, 0);

    WindowThread *w = new WindowThread("WindowTitle", 400, 300, 0, true);

    if (w == 0)
    {
      nuxAssertMsg(0, "[CreateGUIThread] WindowThread creation failed.");
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle(WINDOWSTYLE_NORMAL);
    w->m_embedded_window = true;
    w->ThreadCtor(NULL, X11Window, OpenGLContext);
    return w;
  }
#endif

// Create a window thread that is a child of the Parent. This thread has a window.
  WindowThread *CreateWindowThread(WindowStyle WndStyle,
                                    const char *WindowTitle,
                                    unsigned int width,
                                    unsigned int height,
                                    WindowThread *Parent,
                                    ThreadUserInitFunc UserInitFunc,
                                    void *InitData)
  {
    WindowThread *w = _CreateModalWindowThread(WindowTitle, width, height, Parent, UserInitFunc, InitData, false);

    if (w == 0)
    {
      nuxAssertMsg(0, "[CreateWindowThread] WindowThread creation failed.");
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle(WndStyle);

    return w;
  }

// Create modal graphics thread that is a child of the Parent. This thread has a window.
  WindowThread *CreateModalWindowThread(WindowStyle WndStyle,
                                         const char *WindowTitle,
                                         unsigned int width,
                                         unsigned int height,
                                         WindowThread *Parent,
                                         ThreadUserInitFunc UserInitFunc,
                                         void *InitData)
  {
    WindowThread *w = _CreateModalWindowThread(WindowTitle, width, height, Parent, UserInitFunc, InitData, true);

    if (w == 0)
    {
      nuxAssertMsg(0, "[CreateWindowThread] WindowThread creation failed.");
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle(WndStyle);

    return w;
  }

  SystemThread *CreateSystemThread(AbstractThread *Parent, ThreadUserInitFunc UserInitFunc, void *InitData)
  {
    SystemThread *system_thread = new SystemThread(Parent);
    
    if (system_thread == 0)
    {
      nuxAssertMsg(0, "[CreateSimpleThread] SystemThread creation failed.");
      return 0;
    }
    system_thread->m_UserInitFunc = UserInitFunc;
    system_thread->m_UserExitFunc = 0;
    system_thread->m_InitData = InitData;
    system_thread->m_ExitData = 0;
    return system_thread;
  }

  bool RegisterNuxThread(NThread *ThreadPtr)
  {
    nuxAssert(ThreadPtr);
    NUX_RETURN_VALUE_IF_NULL(ThreadPtr, false);

    NScopeLock Scope(&ThreadArrayLock);
    std::vector<NThread *>::iterator it = find(ThreadArray.begin(), ThreadArray.end(), ThreadPtr);

    if (it == ThreadArray.end())
    {
      ThreadArray.push_back(ThreadPtr);
    }

    return true;
  }

  void UnregisterNuxThread(NThread *ThreadPtr)
  {
    nuxAssert(ThreadPtr);
    NUX_RETURN_IF_NULL(ThreadPtr);

    NScopeLock Scope(&ThreadArrayLock);
    std::vector<NThread *>::iterator it = find(ThreadArray.begin(), ThreadArray.end(), ThreadPtr);

    if (it != ThreadArray.end())
    {
      ThreadArray.erase(it);
    }
  }

  ThreadState GetThreadState(unsigned int ThreadID)
  {
    NScopeLock Scope(&ThreadArrayLock);
    std::vector<NThread *>::iterator it;

    for (it = ThreadArray.begin(); it != ThreadArray.end(); it++)
    {
      if ((*it)->GetThreadId() == ThreadID)
      {
        return (*it)->GetThreadState();
        break;
      }
    }

    return THREADSTOP;
  }


  ObjectPtr<FontTexture> GetSysFont()
  {
    return GetGraphicsEngine().GetFont();
  }

  ObjectPtr<FontTexture> GetSysBoldFont()
  {
    return GetGraphicsEngine().GetBoldFont();
  }

  WindowCompositor &GetWindowCompositor()
  {
    NThread *thread = GetWindowThread();

    if (!thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
      nuxAssertMsg(0, "[GetWindowCompositor] You can't call GetWindowCompositor on this type of thread: s", thread->Type().name );
      PrintOutputDebugString("[GetWindowCompositor] You can't call GetWindowCompositor on this type of thread: s", thread->Type().name );
      NUX_HARDWARE_BREAK;
    }

    return (static_cast<WindowThread *> (thread))->GetWindowCompositor();
  }

  NThread *GetThreadApplication()
  {
    NThread *thread = static_cast<NThread *> (inlGetThreadLocalStorage(ThreadLocal_InalogicAppImpl));
    return thread;
  }

  WindowThread *GetGraphicsThread()
  {
    return NUX_STATIC_CAST(WindowThread *, GetWindowThread());
  }

  WindowThread *GetWindowThread()
  {
    NThread *thread = static_cast<NThread *> (inlGetThreadLocalStorage(ThreadLocal_InalogicAppImpl));
    return NUX_STATIC_CAST(WindowThread *, thread);
  }

  BasePainter& GetPainter()
  {
    NThread *thread = GetWindowThread();
    return NUX_STATIC_CAST(WindowThread *, thread)->GetPainter();
  }

  UXTheme& GetTheme() 
  {
    NThread *thread = GetWindowThread();
    return NUX_STATIC_CAST(WindowThread *, thread)->GetTheme();
  }

  TimerHandler& GetTimer()
  {
    NThread *thread = GetWindowThread();
    return NUX_STATIC_CAST(WindowThread *, thread)->GetTimerHandler();
  }

  GraphicsDisplay& GetWindow()
  {
    NThread *thread = GetWindowThread();
    return NUX_STATIC_CAST(WindowThread *, thread)->GetWindow();
  }

  GraphicsEngine& GetGraphicsEngine()
  {
    NThread *thread = GetWindowThread();
    return NUX_STATIC_CAST(WindowThread *, thread)->GetGraphicsEngine();
  }

}
