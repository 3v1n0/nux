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


#ifndef OPENGL_GFX_H
#define OPENGL_GFX_H

#include "Gfx_Interface.h"
#include "GLTimer.h"
#include "GLDeviceObjects.h"
#include "GLRenderStates.h"

namespace nux { //NUX_NAMESPACE_BEGIN

struct IEvent;
class MainFBO;
class GLDeviceFactory;
class GraphicsContext;
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
class GLWindowImpl : public GraphicSystem
{
    friend class GraphicsContext;

private:
#ifdef WIN32
    // WIN32 system variables
    HGLRC       m_GLRC;     //!< OpenGL Rendering Context.
    HDC         m_hDC;      //!< Device Context.
    HWND        m_hWnd;       //!< Window Handle.
    HWND        m_ParentWindow;

    TCHAR m_WindowClassName[256];
    GLuint      m_PixelFormat;      // Holds The Results After Searching For A Match
    DWORD       m_dwExStyle;        // Window Extended Style
    DWORD       m_dwStyle;          // Window Style
    NString     m_WindowTitle;
#endif

    static HGLRC sMainGLRC;         // Used as the first argument of wglShareLists to make all successive OpenGL  context share their objects
    static HDC   sMainDC;           // Used as the first argument of wglShareLists to make all successive OpenGL  context share their objects

    // size, position
    Size  m_ViewportSize;
    Size m_WindowSize;

    // surface attibute;
    bool m_fullscreen;
    unsigned int m_ScreenBitDepth;

    // verifiy that the interface is properly created
    bool m_GfxInterfaceCreated;

    // Device information
    void GetDisplayInfo();
    int m_index_of_current_mode;

    bool m_is_window_minimized;

    HCURSOR m_Cursor;

    static int Win32KeySymToINL(int Keysym);
public:

    // Device
    int m_num_device_modes;

    // Event object
    IEvent* m_pEvent;

    // Creation
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
    bool CreateOpenGLWindow(
        const TCHAR* WindowTitle,
        unsigned int WindowWidth,
        unsigned int WindowHeight,
        WindowStyle Style,
        const GLWindowImpl* Parent,
        bool FullscreenFlag = false);

    //! Create a GLWindow from a window and device context.
    /*!
        @param WindowHandle     Provided window.
        @param WindowDCHandle   Provided device context.
        @param OpenGLRenderingContext   And OpenGL rendering context.
    */
    bool CreateFromOpenGLWindow(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext);

    void DestroyOpenGLWindow();
    
    void SetWindowTitle(const TCHAR* Title);
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
    void GetSystemEvent(IEvent *evt);
    IEvent& GetCurrentEvent();
 
    bool isWindowMinimized() const {return m_is_window_minimized;}

    void ShowWindow();
    void HideWindow();
    void EnterMaximizeWindow();
    void ExitMaximizeWindow();

    HWND GetWindowHandle() const {return m_hWnd;}
    HWND GetParentWindowHandle() const {return m_ParentWindow;}
    HDC GetWindowHDC() const { return m_hDC; }
    bool IsChildWindow() const {return m_ParentWindow != 0;}

    // Return true if VSync swap control is available
    bool HasVSyncSwapControl() const;
    void EnableVSyncSwapControl();
    void DisableVSyncSwapControl();

    // m_FrameRate
    float GetFrameTime() const;
    void ResetFrameTime();

    GraphicsContext* GetGraphicsContext() const
    {
        return m_GraphicsContext;
    }
    GLDeviceFactory* GetDeviceFactory() const
    {
        return m_DeviceFactory;
    }
    // Dialog
    bool StartOpenFileDialog(FileDialogOption& fdo);
    bool StartSaveFileDialog(FileDialogOption& fdo);
    bool StartColorDialog(ColorDialogOption& cdo);


    void GetWindowSize(int &w, int &h);
    unsigned int GetWindowWidth();
    unsigned int GetWindowHeight();

    bool HasFrameBufferSupport();
    void SetWindowCursor(HCURSOR cursor);
    HCURSOR GetWindowCursor() const;

    void ProcessForeignWin32Event(HWND hWnd, MSG msg, WPARAM wParam, LPARAM lParam, IEvent* event);
    LRESULT ProcessWin32Event(HWND hWnd, t_u32 uMsg, WPARAM wParam, LPARAM lParam);

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

private:
    bool m_PauseGraphicsRendering;
    GLTimer m_Timer;
    float m_FrameTime;
    GLDeviceFactory* m_DeviceFactory;
    GraphicsContext* m_GraphicsContext;
    WindowStyle m_Style;

public:
    ~GLWindowImpl();
    GLEWContext* GetGLEWContext() { return &m_GLEWContext; }
    WGLEWContext* GetWGLEWContext() { return &m_WGLEWContext; }

private:
    GLWindowImpl();
    GLWindowImpl(const GLWindowImpl&);
    // Does not make sense for a singleton. This is a self assignment.
    GLWindowImpl& operator=(const GLWindowImpl&);


    GLEWContext m_GLEWContext;
    WGLEWContext m_WGLEWContext;
    friend class DisplayAccessController;
};

LRESULT CALLBACK WndProcManager(HWND    hWnd,           // Handle For This Window
                                t_u32   uMsg,           // Message For This Window
                                WPARAM  wParam,         // Additional Message Information
                                LPARAM  lParam);        // Additional Message Information

} //NUX_NAMESPACE_END

#endif //OPENGL_GFX_H

