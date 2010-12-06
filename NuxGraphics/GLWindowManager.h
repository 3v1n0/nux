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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef DISPLAYACCESSCONTROLLER_H
#define DISPLAYACCESSCONTROLLER_H

#include "NuxCore/NuxCore.h"

#if defined(NUX_OS_WINDOWS)
  #include "GraphicsDisplay.h"
#elif defined(NUX_OS_LINUX)
  #include "GraphicsDisplay.h"
#endif

namespace nux
{
  class GraphicsDisplay;
  class GpuDevice;
  class GraphicsEngine;

  extern const TCHAR *WINDOW_CLASS_NAME;

  class DisplayAccessController
  {
  public:
    GraphicsDisplay *CreateGLWindow (const TCHAR *WindowTitle, unsigned int WindowWidth, unsigned int WindowHeight, WindowStyle Style, GraphicsDisplay *GLWindow, bool FullscreenFlag = FALSE);

#if defined(NUX_OS_WINDOWS)
    HINSTANCE GetInstance()
    {
      return hInstance;
    }
#endif

#if defined(NUX_OS_WINDOWS)
    //! Create a GraphicsDisplay from a foreign window and display.
    GraphicsDisplay *CreateFromForeignWindow (HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext);
#elif defined(NUX_OS_LINUX)
    //! Create a GraphicsDisplay from a foreign window and display.
    GraphicsDisplay *CreateFromForeignWindow (Display *X11Display, Window X11Window, GLXContext OpenGLContext);
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

}

#endif // DISPLAYACCESSCONTROLLER_H
