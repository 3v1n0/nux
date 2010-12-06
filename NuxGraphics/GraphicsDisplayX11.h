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


#ifndef GFXSETUPX11_H
#define GFXSETUPX11_H

#include "Gfx_Interface.h"
#include "GLTimer.h"
#include "GLDeviceObjects.h"
#include "GLRenderStates.h"

/* Xlib.h is the default header that is included and has the core functionallity */
#include <X11/Xlib.h>
/* Xatom.h includes functionallity for creating new protocol messages */
#include <X11/Xatom.h>
/* keysym.h contains keysymbols which we use to resolv what keys that are being pressed */
#include <X11/keysym.h>

#include <X11/extensions/xf86vmode.h>

namespace nux
{

  struct IEvent;
  class MainFBO;
  class GpuDevice;
  class GraphicsEngine;
  class IOpenGLFrameBufferObject;

  enum WindowStyle
  {
    WINDOWSTYLE_NORMAL,
    WINDOWSTYLE_PANEL,
    WINDOWSTYLE_DIALOG,
    WINDOWSTYLE_TOOL,
    WINDOWSTYLE_NOBORDER,
  };

#define NUX_THREADMSG                           (WM_APP+0)
#define NUX_THREADMSG_START_RENDERING           (WM_APP+1)  // Connection established // start at WM_APP
#define NUX_THREADMSG_CHILD_WINDOW_TERMINATED   (WM_APP+2)  // General failure - Wait Connection failed
#define NUX_THREADMSG_THREAD_TERMINATED         (WM_APP+3)  // Set wParam = Thread ID, lParam = 0

// This will become GLWindow
  class GraphicsDisplay : public GraphicSystem
  {
    friend class GraphicsEngine;

  private:
    Display     *m_X11Display;
    int         m_X11Screen;
    Window      m_X11Window;
    XVisualInfo *m_X11VisualInfo;

    int         m_ParentWindow;
    GLXContext  m_GLCtx;
    XSetWindowAttributes m_X11Attr;

    int m_NumVideoModes;
    XF86VidModeModeInfo **m_X11VideoModes;
    /* original desktop mode which we save so we can restore it later */
    XF86VidModeModeInfo m_X11OriginalVideoMode;

    Atom            m_WMDeleteWindow;
    Colormap        m_X11Colormap;
    int             m_BorderPixel;

    int m_X11VerMajor;
    int m_X11VerMinor;
    int m_GLXVerMajor;
    int m_GLXVerMinor;

    XEvent m_X11LastEvent;
    Bool m_X11RepeatKey;

    TCHAR m_WindowClassName[256];
    GLuint      m_PixelFormat;      // Holds The Results After Searching For A Match
    NString     m_WindowTitle;

    // size, position
    Size m_ViewportSize;
    Size m_WindowSize;
    Point m_WindowPosition;

    // surface attibute;
    bool m_Fullscreen;
    unsigned int m_ScreenBitDepth;

    // verifiy that the interface is properly created
    bool m_GfxInterfaceCreated;

    // Device information
    void GetDisplayInfo();
    int m_BestMode;

    bool m_is_window_minimized;

    static int X11KeySymToINL (int Keysym);

  public:
    bool HasXPendingEvent() const;
    Display *GetX11Display()
    {
      return m_X11Display;
    }
    // Device
    int m_num_device_modes;

    // Event object
    IEvent *m_pEvent;

    bool IsGfxInterfaceCreated();

    //! Create a window with and OpenGL context.
    /*!
        @param WindowTitle      The window title.
        @param WindowWidth      Initial window width.
        @param WindowHeight     Initial window height.
        @param Style            The window style.
        @param ParentWindow     The parent window.
        @param FullscreenFlag   Full screen flag.
    */
    bool CreateOpenGLWindow (
      const TCHAR *WindowTitle,
      unsigned int WindowWidth,
      unsigned int WindowHeight,
      WindowStyle Style,
      const GraphicsDisplay *Parent,
      bool FullscreenFlag = false);

    //! Create a GLWindow from a display and window created externally.
    /*!
        @param X11Display   Provided display.
        @param X11Window    Provided window.
    */
    bool CreateFromOpenGLWindow (Display *X11Display, Window X11Window, GLXContext OpenGLContext);

    void DestroyOpenGLWindow();

    void SetWindowTitle (const TCHAR *Title);
    void SetWindowSize (int width, int height);
    void SetWindowPosition (int width, int height);
    void SetViewPort (int x, int y, int width, int height);
    Point GetMouseScreenCoord();
    Point GetMouseWindowCoord();
    Point GetWindowCoord();
    Rect GetWindowGeometry();
    Rect GetNCWindowGeometry();
    void MakeGLContextCurrent();
    void SwapBuffer (bool glswap = true);

    // Event methods
    void GetSystemEvent (IEvent *evt);

#if defined (NUX_OS_LINUX)
    void InjectXEvent (IEvent *evt, XEvent xevent);
#endif
    
    IEvent &GetCurrentEvent();

    bool isWindowMinimized() const
    {
      return m_is_window_minimized;
    }

    void ShowWindow();
    void HideWindow();
    void EnterMaximizeWindow();
    void ExitMaximizeWindow();

    Window GetWindowHandle() const
    {
      return m_X11Window;
    }
    bool IsChildWindow() const
    {
      return m_ParentWindow != 0;
    }

    // Return true if VSync swap control is available
    bool HasVSyncSwapControl() const;
    void EnableVSyncSwapControl();
    void DisableVSyncSwapControl();

    // m_FrameRate
    float GetFrameTime() const;
    void ResetFrameTime();

    GraphicsEngine *GetGraphicsEngine() const
    {
      return m_GraphicsContext;
    }
    GpuDevice *GetGpuDevice () const
    {
      return m_DeviceFactory;
    }
    // Dialog
    /*bool StartOpenFileDialog(FileDialogOption& fdo);
    bool StartSaveFileDialog(FileDialogOption& fdo);
    bool StartColorDialog(ColorDialogOption& cdo);*/


    void GetDesktopSize (int &w, int &h);
    void GetWindowSize (int &w, int &h);
    unsigned int GetWindowWidth();
    unsigned int GetWindowHeight();

    bool HasFrameBufferSupport();
    /*void SetWindowCursor(HCURSOR cursor);
    HCURSOR GetWindowCursor() const;*/


    //! Pause graphics rendering.
    /*!
        Pause graphics rendering.
        This function also sets the current openGL context to 0 for this window.
        This is useful while a child window is being created and is sharing openGL objects with this context.
        For wglShareLists to work, both OpenGL context must be set to 0 in their respective thread.
        Send NUX_THREADMSG_START_RENDERING (PostThreadMessage) to this window to reactivate rendering.

        Never call this function while doing rendering. Call it only when processing events.
    */
    void PauseThreadGraphicsRendering();
    bool IsPauseThreadGraphicsRendering() const;

    void ProcessForeignX11Event (XEvent *xevent, IEvent *nux_event);
    void ProcessXEvent (XEvent xevent, bool foreign);
    void RecalcXYPosition (Window TheMainWindow, XEvent xevent, int &x, int &y);

  private:
    bool m_PauseGraphicsRendering;
    GLTimer m_Timer;
    float m_FrameTime;
    GpuDevice *m_DeviceFactory;
    GraphicsEngine *m_GraphicsContext;
    WindowStyle m_Style;

  public:
    ~GraphicsDisplay();
    GLEWContext *GetGLEWContext()
    {
      return &m_GLEWContext;
    }
    GLXEWContext *GetGLXEWContext()
    {
      return &m_GLXEWContext;
    }

    NString FindResourceLocation (const TCHAR *ResourceFileName, bool ErrorOnFail = false);
    NString FindUITextureLocation (const TCHAR *ResourceFileName, bool ErrorOnFail = false);
    NString FindShaderLocation (const TCHAR *ResourceFileName, bool ErrorOnFail = false);
    NString FindFontLocation (const TCHAR *ResourceFileName, bool ErrorOnFail = false);

    const std::vector<NString>& GetFontSearchPath() const
    {
      return m_FontSearchPath;
    }
    const std::vector<NString>& GetShaderSearchPath() const
    {
      return m_ShaderSearchPath;
    }
    const std::vector<NString>& GetUITextureSearchPath() const
    {
      return m_UITextureSearchPath;
    }

  private:
    std::vector<NString> m_FontSearchPath;
    std::vector<NString> m_ShaderSearchPath;
    std::vector<NString> m_UITextureSearchPath;
    FilePath m_ResourcePathLocation;

    GraphicsDisplay();
    GraphicsDisplay (const GraphicsDisplay &);
    // Does not make sense for a singleton. This is a self assignment.
    GraphicsDisplay &operator= (const GraphicsDisplay &);

    GLEWContext m_GLEWContext;
    GLXEWContext m_GLXEWContext;
    friend class DisplayAccessController;
  };

}

#endif //GFXSETUPX11_H
