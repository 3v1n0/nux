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

  extern const char *WINDOW_CLASS_NAME;

  class DisplayAccessController
  {
  public:
    //! Create a graphics window capable of doing OpenGL rendering.
    /*!
      @param WindowTitle The title name of the window.
      @param WindowWidth Window width.
      @param WindowHeight Window height.
      @param Style Window style.
      @param parent The parent window.
      @param fullscreen_flag True to create a full screen window.
      @param create_rendering_data If true, then in GraphicsEngine, the system creates the OpenGL shaders and the font textures for the rendering.
    */
    GraphicsDisplay *CreateGLWindow(const char *WindowTitle, unsigned int WindowWidth, unsigned int WindowHeight, WindowStyle Style,
      GraphicsDisplay *parent,
      bool fullscreen_flag = false,
      bool create_rendering_data = true);

#if defined(NUX_OS_WINDOWS)
    HINSTANCE GetInstance()
    {
      return hInstance;
    }
#endif

#if defined(NUX_OS_WINDOWS)
    //! Create a GraphicsDisplay from a foreign window and display.
    GraphicsDisplay *CreateFromForeignWindow(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext);
#elif defined(USE_X11)
    //! Create a GraphicsDisplay from a foreign window and display.
#  ifdef NUX_OPENGLES_20
    GraphicsDisplay *CreateFromForeignWindow(Display *X11Display, Window X11Window, EGLContext OpenGLContext);
#  else
    GraphicsDisplay *CreateFromForeignWindow(Display *X11Display, Window X11Window, GLXContext OpenGLContext);
#  endif
#elif defined(NO_X11)
    GraphicsDisplay *CreateFromForeignWindow(EGLDisplay *disp, EGLContext OpenGLContext);
#endif

    static DisplayAccessController &Instance();
  private:
    static DisplayAccessController *m_pInstance;
    DisplayAccessController();
    DisplayAccessController(const DisplayAccessController &);
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
