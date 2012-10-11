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


#ifndef GRAPHICSDISPLAYWIN_H
#define GRAPHICSDISPLAYWIN_H

#include "Gfx_Interface.h"
#include "GLTimer.h"
#include "GLDeviceObjects.h"
#include "GLRenderStates.h"

#include <d2d1.h>
#include <d2d1helper.h>
#include <dwrite.h>
#include "Wincodec.h"


namespace nux
{

  class Event;
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

    // WIN32 system variables
    HGLRC       opengl_rendering_context_;  //!< OpenGL Rendering Context.
    HDC         device_context_;            //!< Device Context.
    HWND        wnd_handle_;                //!< Window Handle.
    HWND        parent_window_;             //!< Parent window Handle.

    GLuint      pixel_format_;              //!< Holds The Results After Searching For A Match
    DWORD       window_extended_style_;     //!< Window Extended Style
    DWORD       window_style_;              //!< Window Style
    std::string window_title_;

    static HGLRC sMainGLRC;         // Used as the first argument of wglShareLists to make all successive OpenGL  context share their objects
    static HDC   sMainDC;           // Used as the first argument of wglShareLists to make all successive OpenGL  context share their objects

    ID2D1Factory*       d2d_factory_;
    IDWriteFactory*     dw_factory_;
    IWICImagingFactory* wic_factory_;

    Size viewport_size_;            //!< Viewport size.
    Size window_size_;              //!< Window size.

    //! Full screen mode.
    bool fullscreen_;
    //! Screen bit depth.
    unsigned int screen_bit_depth_;

    //! State of the graphics interface.
    bool gfx_interface_created_;

    // Device information
    void GetDisplayInfo();

    bool window_minimized_;

    HCURSOR cursor_;               //!< Windows Cursor.

    static int Win32KeySymToINL(int Keysym);
    static int Win32VKToNuxKey(int vk);
  public:

    //! Last Win32 event.
    Event event_;

    // Creation
    bool IsGfxInterfaceCreated();

    //! Create a window with and OpenGL context.
    /*!
        @param WindowTitle      The window title.
        @param WindowWidth      Initial window width.
        @param WindowHeight     Initial window height.
        @param Style            The window style.
        @param ParentWindow     The parent window.
        @param fullscreen_flag  Full screen flag.
    */
    bool CreateOpenGLWindow(
      const char *WindowTitle,
      unsigned int WindowWidth,
      unsigned int WindowHeight,
      WindowStyle Style,
      const GraphicsDisplay *Parent,
      bool fullscreen_flag = false,
      bool create_rendering_data = true);

    //! Create a GLWindow from a window and device context.
    /*!
        @param WindowHandle     Provided window.
        @param WindowDCHandle   Provided device context.
        @param OpenGLRenderingContext   And OpenGL rendering context.
    */
    bool CreateFromOpenGLWindow(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext);

    void DestroyOpenGLWindow();

    void SetWindowTitle(const char *Title);
    void SetWindowSize(int width, int height);
    void SetViewPort(int x, int y, int width, int height);
    Point GetMouseScreenCoord();
    Point GetMouseWindowCoord();
    Point GetWindowCoord();
    Rect GetWindowGeometry();
    Rect GetNCWindowGeometry();
    void MakeGLContextCurrent(bool b = true);
    void SwapBuffer(bool glswap = true);

    // Event methods
    /*!
      Returns true if there was a pending event to be fetched and false otherwise
     */
    bool GetSystemEvent(Event *evt);
    const Event& GetCurrentEvent() const;

    bool isWindowMinimized() const
    {
      return window_minimized_;
    }

    void ShowWindow();
    void HideWindow();
    bool IsWindowVisible();

    void EnterMaximizeWindow();
    void ExitMaximizeWindow();

    HWND GetWindowHandle() const
    {
      return wnd_handle_;
    }
    HWND GetParentWindowHandle() const
    {
      return parent_window_;
    }
    HDC GetWindowHDC() const
    {
      return device_context_;
    }
    bool IsChildWindow() const
    {
      return parent_window_ != 0;
    }

    ID2D1Factory* GetDirect2DFactory();
    IDWriteFactory* GetDirectWriteFactory();
    IWICImagingFactory* GetWICFactory();

    // Return true if VSync swap control is available
    bool HasVSyncSwapControl() const;
    void EnableVSyncSwapControl();
    void DisableVSyncSwapControl();

    // m_FrameRate
    float GetFrameTime() const;
    void ResetFrameTime();

    GraphicsEngine *GetGraphicsEngine() const;
    
    GpuDevice *GetGpuDevice() const;

    // Dialog
    bool StartOpenFileDialog(FileDialogOption &fdo);
    bool StartSaveFileDialog(FileDialogOption &fdo);
    bool StartColorDialog(ColorDialogOption &cdo);


    void GetWindowSize(int &w, int &h);
    int GetWindowWidth();
    int GetWindowHeight();

    //! Get the window size and reset the GraphicsEngine and GpuDevice accordingly.
    /*!
        This is a passive way to set the window size through out the NuxGraphics system. This call gets the 
        current window size and sets its accordingly to all sub-system.
    */
    void ResetWindowSize();

    bool HasFrameBufferSupport();
    void SetWindowCursor(HCURSOR cursor);
    HCURSOR GetWindowCursor() const;

    void ProcessForeignWin32Event(HWND hWnd, MSG msg, WPARAM wParam, LPARAM lParam, Event *event);
    LRESULT ProcessWin32Event(HWND hWnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam);

    //! Pause graphics rendering.
    /*!
        Pause graphics rendering.
        This function also sets the current openGL context to 0 for this window.
        This is useful while a child window is being created and is sharing openGL objects with this context.
        For wglShareLists to work, both OpenGL context must be set to 0 in their respective thread.
        Send NUX_THREADMSG_START_RENDERING(PostThreadMessage) to this window to reactivate rendering.

        Never call this function while doing rendering. Call it only when processing events.
    */
    void PauseThreadGraphicsRendering();
    bool IsPauseThreadGraphicsRendering() const;

    // Pointer and keyboard grab API
    typedef void(*GrabReleaseCallback) (bool replaced, void *user_data);

    bool GrabPointer   (GrabReleaseCallback callback, void *data, bool replace_existing);
    bool UngrabPointer(void *data);
    bool PointerIsGrabbed();

    bool GrabKeyboard   (GrabReleaseCallback callback, void *data, bool replace_existing);
    bool UngrabKeyboard(void *data);
    bool KeyboardIsGrabbed();

    void * KeyboardGrabData() { return _global_keyboard_grab_data; }
    void * PointerGrabData() { return _global_pointer_grab_data; }

  private:
    void InitGlobalGrabWindow();

    bool m_PauseGraphicsRendering;
    GLTimer m_Timer;
    float m_FrameTime;
    GpuDevice *m_DeviceFactory;
    GraphicsEngine *m_GraphicsContext;
    WindowStyle m_Style;

    HWND                _global_grab_window;

    void               *_global_pointer_grab_data;
    bool                _global_pointer_grab_active;
    GrabReleaseCallback _global_pointer_grab_callback;

    void               *_global_keyboard_grab_data;
    bool                _global_keyboard_grab_active;
    GrabReleaseCallback _global_keyboard_grab_callback;


  public:
    ~GraphicsDisplay();
    GLEWContext *GetGLEWContext()
    {
      return &m_GLEWContext;
    }
    WGLEWContext *GetWGLEWContext()
    {
      return &m_WGLEWContext;
    }

  private:
    GraphicsDisplay();
    GraphicsDisplay(const GraphicsDisplay &);
    // Does not make sense for a singleton. This is a self assignment.
    GraphicsDisplay &operator= (const GraphicsDisplay &);


    GLEWContext m_GLEWContext;
    WGLEWContext m_WGLEWContext;
    friend class DisplayAccessController;
  };

  LRESULT CALLBACK WndProcManager(HWND    hWnd,          // Handle For This Window
                                   unsigned int   uMsg,           // Message For This Window
                                   WPARAM  wParam,         // Additional Message Information
                                   LPARAM  lParam);        // Additional Message Information

}

#endif //GRAPHICSDISPLAYWIN_H

