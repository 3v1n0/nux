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


#include "NuxCore/NuxCore.h"
#include "GLWindowManager.h"

namespace nux
{

#if defined(NUX_OS_WINDOWS)
  const TCHAR *WINDOW_CLASS_NAME = TEXT ("InalogicWindowClass");
  HINSTANCE gInstance = 0;
#endif

  DisplayAccessController *DisplayAccessController::m_pInstance = 0;

  DisplayAccessController::DisplayAccessController()
  {
    inlRegisterThreadLocalIndex (1, ThreadLocal_GLWindowImpl, NULL);
    inlRegisterThreadLocalIndex (2, ThreadLocal_GLDeviceFactory, NULL);

#if defined(NUX_OS_WINDOWS)
    // Register Windows Class

    // Get hInstance of current application.
    if (gInstance == 0)
      hInstance = GetModuleHandle (NULL);
    else
      hInstance = gInstance;

    WinClass.cbSize         = sizeof (WNDCLASSEX);
    //  CS_CLASSDC      Specifies that one device context is shared between all windows created with this class.
    //  CS_DBLCLKS      This is needed if you want to be able to detect double mouse clicks made on the window.
    //  CS_HREDRAW      The window is redrawn if there is a change in the window's width or if the window is moved horizontally.
    //  CS_NOCLOSE      Disables the close option on the window menu.
    //  CS_OWNDC        A unique device context is created for each window created. This is the opposite to CS_CLASSDC.
    //  CS_PARENTDC     This sets the clipping rectangle of the child window to that of the parent window. This allows the child window to be able to draw on the parent window.
    //  CS_VREDRAW      The window is redrawn if there is a change in the window's height or if the window is moved vertically.
    WinClass.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;	// Redraw On Size, And Own DC For Window.
    WinClass.lpfnWndProc    = (WNDPROC) WndProcManager;				// WndProc Handles Messages
    WinClass.cbClsExtra     = 0;									// No Extra Window Data
    WinClass.cbWndExtra     = 0;									// No Extra Window Data
    WinClass.hInstance      = hInstance;							// Set The Instance
    WinClass.hIcon          = LoadIcon (hInstance, "IDI_INALOGIC"); //LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
    WinClass.hCursor        = LoadCursor (NULL, IDC_ARROW);			// Class cursor: Load The Arrow Pointer
    WinClass.hbrBackground  = NULL; //(HBRUSH)GetStockObject(BLACK_BRUSH);  // No Background Required For GL
    WinClass.lpszMenuName   = NULL;									// We Don't Want A Menu
    WinClass.lpszClassName  = WINDOW_CLASS_NAME; //gClassName;							// Set The Class Name
    WinClass.hIconSm        = LoadIcon (hInstance, "IDI_INALOGIC");

    if (!RegisterClassEx (&WinClass) )									// Attempt To Register The Window Class
    {
      nuxDebugMsg (TEXT ("[DisplayAccessController::~DisplayAccessController] Failed to register window class name: %s."), WINDOW_CLASS_NAME);
      inlWin32MessageBox (NULL, TEXT ("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal,
                          TEXT ("Failed to register window class name.\nThe program will exit.") );
      //MessageBox(NULL, TEXT("Failed To Register The Window Class."), TEXT(""ERROR"), MB_OK | MB_ICONERROR);
      exit (1);
    }

#endif

  }

  DisplayAccessController::~DisplayAccessController()
  {
#if defined(NUX_OS_WINDOWS)

    if (!UnregisterClass (WINDOW_CLASS_NAME, hInstance) )			// Are We Able To Unregister Class
    {
      nuxDebugMsg (TEXT ("[DisplayAccessController::~DisplayAccessController] Failed to unregister window class name: %s."), WINDOW_CLASS_NAME);
      hInstance = NULL;									      // Set hInstance To NULL
    }

#endif
  }

  DisplayAccessController &DisplayAccessController::Instance()
  {
    if (m_pInstance == 0)
    {
      m_pInstance = new DisplayAccessController();
    }

    return *m_pInstance;
  }

  GLWindowImpl *DisplayAccessController::CreateGLWindow (const TCHAR *WindowTitle, unsigned int WindowWidth, unsigned int WindowHeight, WindowStyle Style, GLWindowImpl *GLWindow, bool FullscreenFlag/* = FALSE*/)
  {
    if (GetThreadGLWindow() )
    {
      // A GlWindow already exist for this thread.
      nuxAssertMsg (0, TEXT ("Only one GLWindow per thread is allowed") );
      return 0;
    }

    GLWindowImpl *glwindow = new GLWindowImpl();
    glwindow->CreateOpenGLWindow (WindowTitle, WindowWidth, WindowHeight, Style, GLWindow, FullscreenFlag);

    return glwindow;
  }

#if defined(NUX_OS_WINDOWS)
  GLWindowImpl *DisplayAccessController::CreateFromForeignWindow (HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext)
  {
    if (GetThreadGLWindow() )
    {
      // A GlWindow already exist for this thread.
      nuxAssertMsg (0, TEXT ("Only one GLWindow per thread is allowed") );
      return 0;
    }

    GLWindowImpl *glwindow = new GLWindowImpl();
    glwindow->CreateFromOpenGLWindow (WindowHandle, WindowDCHandle, OpenGLRenderingContext);

    return glwindow;
  }
#elif defined(NUX_OS_LINUX)
  GLWindowImpl *DisplayAccessController::CreateFromForeignWindow (Display *X11Display, Window X11Window, GLXContext OpenGLContext)
  {
    if (GetThreadGLWindow() )
    {
      // A GlWindow already exist for this thread.
      nuxAssertMsg (0, TEXT ("Only one GLWindow per thread is allowed") );
      return 0;
    }

    GLWindowImpl *glwindow = new GLWindowImpl();
    glwindow->CreateFromOpenGLWindow (X11Display, X11Window, OpenGLContext);

    return glwindow;
  }
#endif

  GLWindowImpl *GetThreadGLWindow()
  {
    return (GLWindowImpl *) inlGetThreadLocalStorage (ThreadLocal_GLWindowImpl);
  }

  GLDeviceFactory *GetThreadGLDeviceFactory()
  {
    return (GLDeviceFactory *) inlGetThreadLocalStorage (ThreadLocal_GLDeviceFactory);
  }

  GraphicsContext *GetThreadGraphicsContext()
  {
    GLWindowImpl *glwindow = (GLWindowImpl *) inlGetThreadLocalStorage (ThreadLocal_GLWindowImpl);
    return glwindow->GetGraphicsContext();
  }


}

GLEWContext *glewGetContext()
{
  return nux::GetThreadGLWindow()->GetGLEWContext();
}

#if defined(NUX_OS_WINDOWS)
WGLEWContext *wglewGetContext()
{
  return nux::GetThreadGLWindow()->GetWGLEWContext();
}
#elif defined(NUX_OS_LINUX)
GLXEWContext *glxewGetContext()
{
  return nux::GetThreadGLWindow()->GetGLXEWContext();
}
#endif