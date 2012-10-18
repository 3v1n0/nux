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

  void NuxInitialize(const char * /* CommandLine */)
  {
    nux::NuxCoreInitialize(0);
    nux::NuxGraphicsInitialize();

    // Register a thread local store for the WindowThreads. Each window thread will be able to access its own WindowThread pointer.
    inlRegisterThreadLocalIndex(0, ThreadLocal_InalogicAppImpl, NULL);
  }

  static WindowThread *_CreateModalWindowThread(const char *window_title,
      int width,
      int height,
      WindowThread *parent,
      ThreadUserInitFunc /* user_init_func */,
      void * /* data */,
      bool Modal)
  {
    // check that Parent exist
    WindowThread *w = new WindowThread(window_title, width, height, parent, Modal);

    if (w == 0)
    {
      nuxDebugMsg("[_CreateModalWindowThread] WindowThread creation failed.");
      return NULL;
    }

    return w;
  }

  WindowThread *CreateGUIThread(const char *window_title,
                                int width,
                                int height,
                                WindowThread *parent,
                                ThreadUserInitFunc user_init_func,
                                void *data)
  {
    return CreateNuxWindow(window_title, width, height, WINDOWSTYLE_NORMAL, parent, false, user_init_func, data);
  }

  WindowThread *CreateNuxWindow(const char *window_title,
    int width,
    int height,
    WindowStyle window_border_style,
    AbstractThread *parent,
    bool modal,
    ThreadUserInitFunc user_init_func,
    void *data)
  {
    if (GetWindowThread())
    {
      // An WindowThread already exist for this thread.
      nuxDebugMsg("[CreateGUIThread] You may have only one Nux window per system thread.");
      return NULL;
    }

    modal = (modal && parent && parent->Type().IsObjectType(WindowThread::StaticObjectType));

    if (width <= WindowThread::MINIMUM_WINDOW_WIDTH)
    {
      width = WindowThread::MINIMUM_WINDOW_WIDTH;
    }
    
    if (height <= WindowThread::MINIMUM_WINDOW_HEIGHT)
    {
      height = WindowThread::MINIMUM_WINDOW_HEIGHT;
    }

    WindowThread *w = new WindowThread(window_title, width, height, parent, modal);

    if (w == NULL)
    {
      nuxDebugMsg("[CreateGUIThread] WindowThread creation failed.");
      return NULL;
    }

    w->user_init_func_ = user_init_func;
    w->user_exit_func_ = 0;
    w->initialization_data_ = data;
    w->window_style_ = window_border_style;
    w->ThreadCtor();
    return w;
  }

  WindowThread *CreateNuxWindowNewThread(const char *window_title,
    int width,
    int height,
    WindowStyle window_border_style,
    AbstractThread *parent,
    bool modal,
    ThreadUserInitFunc user_init_func,
    void *data)
  {
    modal = (modal && parent && parent->Type().IsObjectType(WindowThread::StaticObjectType));

    if (width <= WindowThread::MINIMUM_WINDOW_WIDTH)
    {
      width = WindowThread::MINIMUM_WINDOW_WIDTH;
    }

    if (height <= WindowThread::MINIMUM_WINDOW_HEIGHT)
    {
      height = WindowThread::MINIMUM_WINDOW_HEIGHT;
    }

    WindowThread *w = new WindowThread(window_title, width, height, parent, modal);
    if (w == NULL)
    {
      nuxDebugMsg("[CreateNuxWindowNewThread] WindowThread creation failed.");
      return NULL;
    }

    w->user_init_func_ = user_init_func;
    w->user_exit_func_ = 0;
    w->initialization_data_ = data;
    w->window_style_ = window_border_style;
    // Do not call WindowThread::ThreadCtor();
    return w;
  }

#if defined(NUX_OS_WINDOWS)
  WindowThread* CreateFromForeignWindow(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext,
                                        ThreadUserInitFunc user_init_func,
                                        void *data)
  {
    if (GetWindowThread())
    {
      // An WindowThread already exist for this thread.
      nuxDebugMsg("[CreateGUIThread] You may have only one Nux window per system thread.");
      return NULL;
    }

    WindowThread *w = new WindowThread("WindowTitle", 400, 300, 0, true);

    if (w == 0)
    {
      nuxDebugMsg("[CreateGUIThread] WindowThread creation failed.");
      return NULL;
    }

    w->user_init_func_ = user_init_func;
    w->user_exit_func_ = 0;
    w->initialization_data_ = data;
    w->window_style_ = WINDOWSTYLE_NORMAL;
    w->embedded_window_ = true;
    w->ThreadCtor(WindowHandle, WindowDCHandle, OpenGLRenderingContext);
    return w;
  }

#elif defined(NO_X11)
#elif defined(NUX_OS_LINUX)
#ifdef NUX_OPENGLES_20
  WindowThread *CreateFromForeignWindow (Window X11Window, EGLContext OpenGLContext,
                                         ThreadUserInitFunc user_init_func,
                                         void *data)
#else
  WindowThread *CreateFromForeignWindow (Window X11Window, GLXContext OpenGLContext,
                                         ThreadUserInitFunc user_init_func,
                                         void *data)
#endif
  {
    if (GetWindowThread())
    {
      // An WindowThread already exist for this thread.
      nuxDebugMsg("[CreateGUIThread] You may have only one Nux window per system thread.");
      return 0;
    }

    WindowThread *w = new WindowThread("WindowTitle", 400, 300, 0, true);

    if (w == 0)
    {
      nuxDebugMsg("[CreateGUIThread] WindowThread creation failed.");
      return NULL;
    }

    w->user_init_func_ = user_init_func;
    w->user_exit_func_ = 0;
    w->initialization_data_ = data;
    w->window_style_ = WINDOWSTYLE_NORMAL;
    w->embedded_window_ = true;
    w->ThreadCtor(NULL, X11Window, OpenGLContext);
    return w;
  }
#endif

// Create a window thread that is a child of the Parent. This thread has a window.
  WindowThread *CreateWindowThread(WindowStyle WndStyle,
                                   const char *window_title,
                                   int width,
                                   int height,
                                   WindowThread *parent,
                                   ThreadUserInitFunc user_init_func,
                                   void *data)
  {
    if (width <= WindowThread::MINIMUM_WINDOW_WIDTH)
    {
      width = WindowThread::MINIMUM_WINDOW_WIDTH;
    }

    if (height <= WindowThread::MINIMUM_WINDOW_HEIGHT)
    {
      height = WindowThread::MINIMUM_WINDOW_HEIGHT;
    }

    WindowThread *w = _CreateModalWindowThread(window_title, width, height, parent, user_init_func, data, false);

    if (w == 0)
    {
      nuxDebugMsg("[CreateWindowThread] WindowThread creation failed.");
      return NULL;
    }

    w->user_init_func_ = user_init_func;
    w->user_exit_func_ = 0;
    w->initialization_data_ = data;
    w->window_style_ = WndStyle;

    return w;
  }

// Create modal graphics thread that is a child of the Parent. This thread has a window.
  WindowThread *CreateModalWindowThread(WindowStyle WndStyle,
                                        const char *window_title,
                                        int width,
                                        int height,
                                        WindowThread *parent,
                                        ThreadUserInitFunc user_init_func,
                                        void *data)
  {
    if (width <= WindowThread::MINIMUM_WINDOW_WIDTH)
    {
      width = WindowThread::MINIMUM_WINDOW_WIDTH;
    }

    if (height <= WindowThread::MINIMUM_WINDOW_HEIGHT)
    {
      height = WindowThread::MINIMUM_WINDOW_HEIGHT;
    }

    WindowThread *w = _CreateModalWindowThread(window_title, width, height, parent, user_init_func, data, true);

    if (w == 0)
    {
      nuxDebugMsg("[CreateWindowThread] WindowThread creation failed.");
      return NULL;
    }

    w->user_init_func_ = user_init_func;
    w->user_exit_func_ = 0;
    w->initialization_data_ = data;
    w->window_style_ = WndStyle;

    return w;
  }

  SystemThread *CreateSystemThread(AbstractThread *parent, ThreadUserInitFunc user_init_func, void *data)
  {
    SystemThread *system_thread = new SystemThread(parent);
    
    if (system_thread == 0)
    {
      nuxDebugMsg("[CreateSimpleThread] SystemThread creation failed.");
      return NULL;
    }
    system_thread->user_init_func_ = user_init_func;
    system_thread->user_exit_func_ = 0;
    system_thread->initialization_data_ = data;
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

  ObjectPtr<FontTexture> GetSysFont()
  {
    return GetWindowThread()->GetGraphicsEngine().GetFont();
  }

  ObjectPtr<FontTexture> GetSysBoldFont()
  {
    return GetWindowThread()->GetGraphicsEngine().GetBoldFont();
  }

  WindowCompositor &GetWindowCompositor()
  {
    NThread *thread = GetWindowThread();

    if (thread && !thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
      nuxDebugMsg("[GetWindowCompositor] You can't call GetWindowCompositor on this type of thread: s", thread->Type().name );
      PrintOutputDebugString("[GetWindowCompositor] You can't call GetWindowCompositor on this type of thread: s", thread->Type().name );
      NUX_HARDWARE_BREAK;
    }

    return (static_cast<WindowThread *> (thread))->GetWindowCompositor();
  }

  WindowThread *GetWindowThread()
  {
    return GetThreadNuxWindow();
  }

  WindowThread *GetThreadNuxWindow()
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
}
