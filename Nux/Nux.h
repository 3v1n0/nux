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


#ifndef NUX_H
#define NUX_H

#include "NuxCore/NuxCore.h"
#include "NuxCore/Error.h"
#include "NuxCore/FilePath.h"
#include "NuxCore/Color.h"
#include "NuxCore/Rect.h"
#include "NuxCore/Point.h"
#include "NuxCore/Size.h"

#include "NuxCore/ObjectPtr.h"

#include "NuxCore/Math/Constants.h"
#include "NuxCore/Math/Vector3.h"
#include "NuxCore/Math/Vector4.h"
#include "NuxCore/Math/Matrix3.h"
#include "NuxCore/Math/Matrix4.h"
#include "NuxCore/Math/Spline.h"

#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/slot.h>

#include <glib.h>

#if defined(NUX_OS_LINUX)
#include "Nux/Features.h"
#endif

#include "Utils.h"
#include "WidgetMetrics.h"
#include "Area.h"
#include "InputArea.h"
#include "Theme.h"
#include "Painter.h"
#include "View.h"
#include "BasicView.h"
#include "AbstractThread.h"
#include "WindowThread.h"
#include "WindowCompositor.h"
#if !defined(NUX_MINIMAL)
#  include "Timeline.h"
#endif
#include "SystemThread.h"



namespace nux
{
  class WindowThread;
  class SystemThread;
  class WindowCompositor;
  class TimerHandler;

  void NuxInitialize(const char *CommandLine);

  //! Create a Nux window.
  /*!
      Create a Nux window. The window 

      @param window_title The window title.
      @param width The window width.
      @param height The window height.
      @param parent The window height parent. Should be NULL.
      @param user_init_func Initialization function to be called before starting the window loop.
      @param data Parameter to the initialization function.
  */
  WindowThread *CreateGUIThread(const char *window_title,
                                int width,
                                int height,
                                WindowThread *parent = NULL,
                                ThreadUserInitFunc user_init_func = NULL,
                                void *data = NULL);

  //! Create the only Nux window for the current system thread.
  /*!
      There can be only one NuxWindow per system thread. This function creates the only Nux window allowed in the current thread. \n
      The first time this function is called in the current system thread, it returns a valid pointer
      to a NuxWindow object. The next time it is called, the system detects that a NuxWindow has already
      been created in the thread and it returns NULL. \n
      When CreateNuxWindow successfully returns, the NuxWindow internals (GraphicsDisplay, Compositor, Painter, TimerManager)
      have been initialized. Calling nux::GetThreadNuxWindow() returns a pointer to the NuxWindow that has been successfully created
      in the current thread. \n
      To start the main loop of a NuxWindow created with CreateNuxWindow, call NuxWindow::Run(); \n
      The modal parameter specifies if the window is blocking while it is active. This is valid only if the parent thread is a WindowThread.

      @param window_title The window title.
      @param width The window width.
      @param height The window height.
      @param window_border_style The window border style.
      @param parent The window parent.
      @param modal Modality of the window.
      @param user_init_func Initialization function to be called before starting the window loop.
      @param data Parameter to the initialization function.

      @return A valid pointer to a NuxWindow if the window has been successfully created. NULL otherwise.
  */
  WindowThread *CreateNuxWindow(const char *window_title,
    int width,
    int height,
    WindowStyle window_border_style = WINDOWSTYLE_NORMAL,
    AbstractThread *parent = NULL,
    bool modal = false,
    ThreadUserInitFunc user_init_func = NULL,
    void *data = NULL);

  //! Create a Nux window to be run in a new thread.
  /*!
      Create a Nux window that will run in a new thread. 
      Unlike CreateNuxWindow, CreateNuxWindowNewThread should always successfully return a valid NuxWindow pointer.
      However, the internal objects of the NuxWindow (GraphicsDisplay, Compositor, Painter, TimerManager) have not been initialized yet.
      So, calling NuxWindow::GetWindowCompositor(), NuxWindow::GetGraphicsDisplay() will fail. \n
      To start the main loop of a NuxWindow created with CreateNuxWindow, call NuxWindow::Run(); \n
      The modal parameter specifies if the window is blocking its parent window while it is active. This is valid only if the parent thread is a WindowThread.


      @param window_title The window title.
      @param width The window width.
      @param height The window height.
      @param window_border_style The window border style.
      @param parent The window parent.
      @param modal Modality of the window.
      @param user_init_func Initialization function to be called before starting the window loop.
      @param data Parameter to the initialization function.

      @return A valid pointer to a NuxWindow if the window has been successfully created. NULL otherwise.
  */
  WindowThread *CreateNuxWindowNewThread(const char *window_title,
    int width,
    int height,
    WindowStyle window_border_style = WINDOWSTYLE_NORMAL,
    AbstractThread *parent = NULL,
    bool modal = false,
    ThreadUserInitFunc user_init_func = NULL,
    void *data = NULL);

#if defined(NUX_OS_WINDOWS)
  //! Create a main graphics thread. This thread has a window and no parent window.
  WindowThread *CreateFromForeignWindow(HWND WindowHandle, HDC WindowDCHandle,
                                        HGLRC OpenGLRenderingContext,
                                        ThreadUserInitFunc user_init_func,
                                        void *data);
#elif defined(NO_X11)
#elif defined(NUX_OS_LINUX)
  //! Create a main graphics thread. This thread has a window and no parent window.
#ifdef NUX_OPENGLES_20
  WindowThread *CreateFromForeignWindow (Window X11Window, EGLContext OpenGLContext,
                                         ThreadUserInitFunc user_init_func,
                                         void *data);
#else
  WindowThread *CreateFromForeignWindow (Window X11Window, GLXContext OpenGLContext,
                                         ThreadUserInitFunc user_init_func,
                                         void *data);
#endif
#endif

  // Create a window thread that is a child of the Parent. This thread has a window.
  /*!
      @param window_style The window style.
      @param window_title The window title.
      @param width The window width.
      @param height The window height.
      @param parent The window parent.
      @param user_init_func Initialization function to be called before starting the window loop.
      @param data Parameter to the initialization function.
  */
  WindowThread *CreateWindowThread(WindowStyle window_style,
                                   const char *window_title,
                                   int width,
                                   int height,
                                   WindowThread *parent,
                                   ThreadUserInitFunc user_init_func = NULL,
                                   void *data = NULL);

  // Create a Modal window thread that is a child of the Parent. This thread has a window.
  /*!
      @param window_style The window style.
      @param window_title The window title.
      @param width The window width.
      @param height The window height.
      @param parent The window height parent. Should be NULL.
      @param user_init_func Initialization function to be called before starting the window loop.
      @param data Parameter to the initialization function.
  */
  WindowThread *CreateModalWindowThread(WindowStyle window_style,
                                        const char *window_title,
                                        int width,
                                        int height,
                                        WindowThread *parent,
                                        ThreadUserInitFunc user_init_func = NULL,
                                        void *data = NULL);

// Create a simple thread
  SystemThread *CreateSystemThread(AbstractThread *parent = NULL,
                                   ThreadUserInitFunc user_init_func = NULL,
                                   void *data = NULL);

/*  ThreadState GetThreadState(unsigned int ThreadID);*/


  ObjectPtr<FontTexture> GetSysFont();
  ObjectPtr<FontTexture> GetSysBoldFont();

  WindowThread      *GetWindowThread();
  WindowThread      *GetThreadNuxWindow();

  WindowCompositor  &GetWindowCompositor();
  BasePainter       &GetPainter();
  UXTheme           &GetTheme();
  TimerHandler      &GetTimer();

#define  gPainter nux::GetPainter() // deprecated
#define  gTheme   nux::GetTheme()   // deprecated

  inlDeclareThreadLocalStorage(NThread *, 0, ThreadLocal_InalogicAppImpl);

}

#endif // NUX_H
