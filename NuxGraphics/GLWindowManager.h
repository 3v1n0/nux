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


#ifndef DISPLAYACCESSCONTROLLER_H
#define DISPLAYACCESSCONTROLLER_H

#include "NuxCore/NKernel.h"

#if defined(NUX_OS_WINDOWS)
#include "Gfx_OpenGL.h"
#elif defined(NUX_OS_LINUX)
#include "GfxSetupX11.h"
#endif

#include "GLThread.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{
  class GLWindowImpl;
  class GLDeviceFactory;
  class GraphicsContext;

  extern const TCHAR *WINDOW_CLASS_NAME;

  class DisplayAccessController
  {
  public:
    GLWindowImpl *CreateGLWindow (const TCHAR *WindowTitle, unsigned int WindowWidth, unsigned int WindowHeight, WindowStyle Style, GLWindowImpl *GLWindow, bool FullscreenFlag = FALSE);

#if defined(NUX_OS_WINDOWS)
    HINSTANCE GetInstance()
    {
      return hInstance;
    }
#endif

#if defined(NUX_OS_WINDOWS)
    //! Create a GLWindowImpl from a foreign window and display.
    GLWindowImpl *CreateFromForeignWindow (HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext);
#elif defined(NUX_OS_LINUX)
    //! Create a GLWindowImpl from a foreign window and display.
    GLWindowImpl *CreateFromForeignWindow (Display *X11Display, Window X11Window, GLXContext OpenGLContext);
#endif

    static DisplayAccessController &Instance();
  private:
    static DisplayAccessController *m_pInstance;
    DisplayAccessController();
    DisplayAccessController (const DisplayAccessController &);
    // Does not make sense for a singleton. This is a self assignment.
    DisplayAccessController &operator= (const DisplayAccessController &);
    // Declare operator address-of as private
    DisplayAccessController *operator &();
    ~DisplayAccessController();

#if defined(NUX_OS_WINDOWS)
    HINSTANCE	hInstance;
    WNDCLASSEX	WinClass;
#endif
  };

#define gGLWindowManager nux::DisplayAccessController::Instance()

  inlDeclareThreadLocalStorage (GLWindowImpl *, 1, ThreadLocal_GLWindowImpl);
  inlDeclareThreadLocalStorage (GLDeviceFactory *, 2, ThreadLocal_GLDeviceFactory);

} //NUX_NAMESPACE_END

#endif // DISPLAYACCESSCONTROLLER_H
