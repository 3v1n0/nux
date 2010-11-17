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
#include "NuxGraphics/NuxGraphics.h"

namespace nux
{

  static NCriticalSection ThreadArrayLock;
  std::vector<NThread *> ThreadArray;

  void NuxInitialize (const TCHAR *CommandLine)
  {
    nux::NuxCoreInitialize (0);
    nux::NuxGraphicsInitialize();

    // Register a thread local store for the WindowThreads. Each window thread will be able to access its own WindowThread pointer.
    inlRegisterThreadLocalIndex (0, ThreadLocal_InalogicAppImpl, NULL);
  }

  static WindowThread *_CreateModalWindowThread (const TCHAR *WindowTitle,
      UINT width,
      UINT height,
      WindowThread *Parent,
      ThreadUserInitFunc UserInitFunc,
      void *InitData,
      bool Modal)
  {
    // check that Parent exist
    WindowThread *w = new WindowThread (WindowTitle, width, height, Parent, Modal);

    if (w == 0)
    {
      nuxAssertMsg (0, TEXT ("[_CreateModalWindowThread] WindowThread creation failed.") );
      return 0;
    }

    return w;
  }

  WindowThread *CreateGUIThread (const TCHAR *WindowTitle,
                                 UINT width,
                                 UINT height,
                                 WindowThread *Parent,
                                 ThreadUserInitFunc UserInitFunc,
                                 void *InitData)
  {
    if (GetThreadApplication() )
    {
      // An WindowThread already exist for this thread.
      nuxAssertMsg (0, "[CreateGUIThread] Only one WindowThread per thread is allowed");
      return 0;
    }

    inlSetThreadLocalStorage (ThreadLocal_InalogicAppImpl, 0);

    WindowThread *w = new WindowThread (WindowTitle, width, height, 0, true);

    if (w == 0)
    {
      nuxAssertMsg (0, TEXT ("[CreateGUIThread] WindowThread creation failed.") );
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle (WINDOWSTYLE_NORMAL);
    w->ThreadCtor();
    return w;
  }

#if defined(NUX_OS_WINDOWS)
  WindowThread *CreateFromForeignWindow (HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext,
                                         ThreadUserInitFunc UserInitFunc,
                                         void *InitData
                                        )
  {
    if (GetThreadApplication() )
    {
      // An WindowThread already exist for this thread.
      nuxAssertMsg (0, "[CreateGUIThread] Only one WindowThread per thread is allowed");
      return 0;
    }

    inlSetThreadLocalStorage (ThreadLocal_InalogicAppImpl, 0);

    WindowThread *w = new WindowThread ("WindowTitle", 400, 300, 0, true);

    if (w == 0)
    {
      nuxAssertMsg (0, TEXT ("[CreateGUIThread] WindowThread creation failed.") );
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle (WINDOWSTYLE_NORMAL);
    w->ThreadCtor (WindowHandle, WindowDCHandle, OpenGLRenderingContext);
    return w;
  }

#elif defined(NUX_OS_LINUX)
  WindowThread *CreateFromForeignWindow (Window X11Window, GLXContext OpenGLContext,
                                         ThreadUserInitFunc UserInitFunc,
                                         void *InitData)
  {
    if (GetThreadApplication() )
    {
      // An WindowThread already exist for this thread.
      nuxAssertMsg (0, "[CreateGUIThread] Only one WindowThread per thread is allowed");
      return 0;
    }

    inlSetThreadLocalStorage (ThreadLocal_InalogicAppImpl, 0);

    WindowThread *w = new WindowThread ("WindowTitle", 400, 300, 0, true);

    if (w == 0)
    {
      nuxAssertMsg (0, TEXT ("[CreateGUIThread] WindowThread creation failed.") );
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle (WINDOWSTYLE_NORMAL);
    w->m_embedded_window = true;
    w->ThreadCtor (XOpenDisplay (NULL), X11Window, OpenGLContext);
    return w;
  }
#endif

// Create a window thread that is a child of the Parent. This thread has a window.
  WindowThread *CreateWindowThread (WindowStyle WndStyle,
                                    const TCHAR *WindowTitle,
                                    UINT width,
                                    UINT height,
                                    WindowThread *Parent,
                                    ThreadUserInitFunc UserInitFunc,
                                    void *InitData)
  {
    WindowThread *w = _CreateModalWindowThread (WindowTitle, width, height, Parent, UserInitFunc, InitData, false);

    if (w == 0)
    {
      nuxAssertMsg (0, TEXT ("[CreateWindowThread] WindowThread creation failed.") );
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle (WndStyle);

    return w;
  }

// Create modal graphics thread that is a child of the Parent. This thread has a window.
  WindowThread *CreateModalWindowThread (WindowStyle WndStyle,
                                         const TCHAR *WindowTitle,
                                         UINT width,
                                         UINT height,
                                         WindowThread *Parent,
                                         ThreadUserInitFunc UserInitFunc,
                                         void *InitData)
  {
    WindowThread *w = _CreateModalWindowThread (WindowTitle, width, height, Parent, UserInitFunc, InitData, true);

    if (w == 0)
    {
      nuxAssertMsg (0, TEXT ("[CreateWindowThread] WindowThread creation failed.") );
      return 0;
    }

    w->m_UserInitFunc = UserInitFunc;
    w->m_UserExitFunc = 0;
    w->m_InitData = InitData;
    w->m_ExitData = 0;
    w->SetWindowStyle (WndStyle);

    return w;
  }

  SystemThread *CreateSimpleThread (AbstractThread *Parent, ThreadUserInitFunc UserInitFunc, void *InitData)
  {
    SystemThread *simpleThread = new SystemThread (Parent);
    return static_cast<SystemThread *> (simpleThread);
  }

  bool RegisterNuxThread (NThread *ThreadPtr)
  {
    nuxAssert (ThreadPtr);
    NUX_RETURN_VALUE_IF_NULL (ThreadPtr, false);

    NScopeLock Scope (&ThreadArrayLock);
    std::vector<NThread *>::iterator it = find (ThreadArray.begin(), ThreadArray.end(), ThreadPtr);

    if (it == ThreadArray.end() )
    {
      ThreadArray.push_back (ThreadPtr);
    }

    return true;
  }

  void UnregisterNuxThread (NThread *ThreadPtr)
  {
    nuxAssert (ThreadPtr);
    NUX_RETURN_IF_NULL (ThreadPtr);

    NScopeLock Scope (&ThreadArrayLock);
    std::vector<NThread *>::iterator it = find (ThreadArray.begin(), ThreadArray.end(), ThreadPtr);

    if (it != ThreadArray.end() )
    {
      ThreadArray.erase (it);
    }
  }

  ThreadState GetThreadState (unsigned int ThreadID)
  {
    NScopeLock Scope (&ThreadArrayLock);
    std::vector<NThread *>::iterator it;

    for (it = ThreadArray.begin(); it != ThreadArray.end(); it++)
    {
      if ( (*it)->GetThreadId() == ThreadID)
      {
        return (*it)->GetThreadState();
        break;
      }
    }

    return THREADSTOP;
  }


  IntrusiveSP<FontTexture> GetSysFont ()
  {
    return GetGraphicsEngine ().GetFont ();
  }

  IntrusiveSP<FontTexture> GetSysBoldFont ()
  {
    return GetGraphicsEngine ().GetBoldFont ();
  }

  WindowCompositor &GetWindowCompositor ()
  {
    NThread *thread = GetThreadApplication ();

    if (!thread->Type().IsObjectType (WindowThread::StaticObjectType) )
    {
      nuxAssertMsg (0, TEXT ("[GetWindowCompositor] You can't call GetWindowCompositor on this type of thread: s"), thread->Type().GetName() );
      PrintOutputDebugString (TEXT ("[GetWindowCompositor] You can't call GetWindowCompositor on this type of thread: s"), thread->Type().GetName() );
      NUX_HARDWARE_BREAK;
    }

    return (static_cast<WindowThread *> (thread) )->GetWindowCompositor();
  }

  NThread *GetThreadApplication()
  {
    NThread *thread = static_cast<NThread *> (inlGetThreadLocalStorage (ThreadLocal_InalogicAppImpl) );
    return thread;
  }

  WindowThread* GetGraphicsThread()
  {
    return NUX_STATIC_CAST (WindowThread *, GetThreadApplication ());
  }

  BasePainter& GetPainter()
  {
    NThread *thread = GetThreadApplication();
    return NUX_STATIC_CAST (WindowThread *, thread)->GetPainter();
  }

  UXTheme& GetTheme() 
  {
    NThread *thread = GetThreadApplication();
    return NUX_STATIC_CAST (WindowThread *, thread)->GetTheme();
  }

  TimerHandler& GetTimer()
  {
    NThread *thread = GetThreadApplication();
    return NUX_STATIC_CAST (WindowThread *, thread)->GetTimerHandler();
  }

  GLWindowImpl& GetWindow()
  {
    NThread *thread = GetThreadApplication();
    return NUX_STATIC_CAST (WindowThread *, thread)->GetWindow();
  }

  GraphicsEngine& GetGraphicsEngine ()
  {
    NThread *thread = GetThreadApplication ();
    return NUX_STATIC_CAST (WindowThread *, thread)->GetGraphicsEngine ();
  }

}
