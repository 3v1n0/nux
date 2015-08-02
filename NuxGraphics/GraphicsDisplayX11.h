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


#ifndef GRAPHICSDISPLAYX11_H
#define GRAPHICSDISPLAYX11_H

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
#include <X11/extensions/Xinerama.h>

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

  enum DndAction
  {
    DNDACTION_COPY,
    DNDACTION_MOVE,
    DNDACTION_LINK,
    DNDACTION_ASK,
    DNDACTION_PRIVATE,

    DNDACTION_NONE,
  };

#define NUX_THREADMSG                           (WM_APP+0)
#define NUX_THREADMSG_START_RENDERING           (WM_APP+1)  // Connection established // start at WM_APP
#define NUX_THREADMSG_CHILD_WINDOW_TERMINATED   (WM_APP+2)  // General failure - Wait Connection failed
#define NUX_THREADMSG_THREAD_TERMINATED         (WM_APP+3)  // Set wParam = Thread ID, lParam = 0

#define _xdnd_max_type 100
#define xdnd_version 5

// This will become GLWindow
  class GraphicsDisplay : public GraphicSystem
  {
  private:
    Display     *m_X11Display;
    int         m_X11Screen;
    Window      m_X11Window;
    XVisualInfo *m_X11VisualInfo;

    XIC m_current_xic;

    int         parent_window_;
#ifndef NUX_OPENGLES_20
    GLXContext  m_GLCtx;
    GLXFBConfig _fb_config;
    GLXWindow   glx_window_;
#else
    EGLContext  m_GLCtx;
    EGLSurface  m_GLSurface;
    EGLConfig   _fb_config;
#endif

    XSetWindowAttributes m_X11Attr;

    int m_NumVideoModes;
    XF86VidModeModeInfo **m_X11VideoModes;
    /* original desktop mode which we save so we can restore it later */
    XF86VidModeModeInfo m_X11OriginalVideoMode;

    Atom            m_WMDeleteWindow;
    Colormap        m_X11Colormap;
    int             m_BorderPixel;

    int _x11_major;
    int _x11_minor;
    int _glx_major;
    int _glx_minor;
    bool _has_glx_13;

    XEvent m_X11LastEvent;
    Bool m_X11RepeatKey;

    char m_WindowClassName[256];
    GLuint      m_PixelFormat;      // Holds The Results After Searching For A Match
    std::string window_title_;

    // size, position
    Size viewport_size_;
    Size window_size_;
    Point m_WindowPosition;

    //! Full screen mode.
    bool fullscreen_;
    //! Screen bit depth
    unsigned int screen_bit_depth_;

    //! State of the graphics interface.
    bool gfx_interface_created_;

    // Device information
    void GetDisplayInfo();

    bool m_CreatedFromForeignWindow;
    Time last_click_time_;
    /*!
        Maximum time allowed between the end of the last click (mouse up) and the next mouse down
        to be considered as a double click event.
    */
    static Time double_click_time_delay;
    int double_click_counter_;

  public:
    typedef void(*GrabReleaseCallback) (bool replaced, void *user_data);

    typedef struct _DndSourceFuncs
    {
      nux::NBitmapData *       (*get_drag_image)    (void * data);
      std::list<const char *>  (*get_drag_types)    (void * data);
      const char *             (*get_data_for_type) (const char * type, int *size, int *format, void * data);
      void                     (*drag_finished)     (DndAction result, void * data);
    } DndSourceFuncs;

    bool HasXPendingEvent() const;
    Display *GetX11Display()
    {
      return m_X11Display;
    }

    // Event object
    Event *m_pEvent;

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

    //! Create a GLWindow from a display and window created externally.
    /*!
        @param X11Display   Provided display.
        @param X11Window    Provided window.
    */
#ifdef NUX_OPENGLES_20
    bool CreateFromOpenGLWindow(Display *X11Display, Window X11Window, EGLContext OpenGLContext);
#else
    bool CreateFromOpenGLWindow(Display *X11Display, Window X11Window, GLXContext OpenGLContext);
#endif

    void DestroyOpenGLWindow();

    void SetWindowTitle(const char *Title);

    //! Set the window size.
    /*!
        Actively set the window size to the provided parameters.
        \sa ResetWindowSize
    */
    void SetWindowSize(int width, int height);

    //! Set the window position.
    void SetWindowPosition(int width, int height);

    //! Set the OpenGL Viewport.
    void SetViewPort(int x, int y, int width, int height);

    Point GetMouseScreenCoord();
    Point GetMouseWindowCoord();
    Point GetWindowCoord();
    Rect GetWindowGeometry();
    Rect GetNCWindowGeometry();
    void MakeGLContextCurrent();
    void SwapBuffer(bool glswap = true);

    // Event methods
    /*!
      Returns true if there was a pending event to be fetched and false otherwise
     */
    bool GetSystemEvent(Event *evt);

    // Os specific
    int GetGlXMajor() const;
    int GetGlXMinor() const;

#if defined(NUX_OS_LINUX)
    void InjectXEvent(Event *evt, XEvent xevent);

    typedef struct _EventFilterArg
    {
      bool   (*filter)    (XEvent event, void * data);
      void * data;
    } EventFilterArg;

    void AddEventFilter (EventFilterArg arg);
    void RemoveEventFilter (void *owner);

    std::list<EventFilterArg> _event_filters;
#endif

    Event &GetCurrentEvent();

    // That method is deprecated, it always returns false and still here in
    // order to maintain API compatibility.
    bool isWindowMinimized() const
    {
      return false;
    }

    void ShowWindow();
    void HideWindow();
    bool IsWindowVisible();

    void EnterMaximizeWindow();
    void ExitMaximizeWindow();

    Window GetWindowHandle() const
    {
      return m_X11Window;
    }
    bool IsChildWindow() const
    {
      return parent_window_ != 0;
    }

    // Return true if VSync swap control is available
    bool HasVSyncSwapControl() const;
    void EnableVSyncSwapControl();
    void DisableVSyncSwapControl();

    // m_FrameRate
    float GetFrameTime() const;
    void ResetFrameTime();

    GraphicsEngine* GetGraphicsEngine() const;

    GpuDevice* GetGpuDevice() const;

    void SetCurrentXIC(XIC xic);

    // Dialog
    /*bool StartOpenFileDialog(FileDialogOption& fdo);
    bool StartSaveFileDialog(FileDialogOption& fdo);
    bool StartColorDialog(ColorDialogOption& cdo);*/


    void GetDesktopSize(int &w, int &h);
    void GetWindowSize(int &w, int &h);
    int GetWindowWidth();
    int GetWindowHeight();

    //! Get the window size and reset the GraphicsEngine and GpuDevice accordingly.
    /*!
        This is a passive way to set the window size through out the NuxGraphics system. This call gets the
        current window size and sets its accordingly to all sub-system.
        \sa SetWindowSize
    */
    void ResetWindowSize();

    bool HasFrameBufferSupport();
    /*void SetWindowCursor(HCURSOR cursor);
    HCURSOR GetWindowCursor() const;*/


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

    void ProcessForeignX11Event(XEvent *xevent, Event *nux_event);
    void ProcessXEvent(XEvent xevent, bool foreign);
    void RecalcXYPosition(Window TheMainWindow, XEvent xevent, int &x, int &y);
    void RecalcXYPosition(int x_root, int y_root, int &x_recalc, int &y_recalc);

    void              SendDndStatus   (bool accept, DndAction action, Rect region);
    void              SendDndFinished(bool accepted, DndAction performed_action);
    std::list<char *> GetDndMimeTypes();
    char *            GetDndData      (char *property);

    void StartDndDrag(const DndSourceFuncs &funcs, void *user_data);

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

    void HandleXDndPosition(XEvent event, Event* nux_event);
    void HandleXDndEnter    (XEvent event);
    void HandleXDndStatus   (XEvent event);
    void HandleXDndLeave    (XEvent event);
    void HandleXDndDrop     (XEvent event, Event* nux_event);
    void HandleXDndFinished(XEvent event);

    void SendXDndStatus(Display *display, Window source, Window target, bool accept, Atom action, Rect box);
    bool GetXDndSelectionEvent(Display *display, Window target, Atom property, long time, XEvent *result, int attempts);
    void SendXDndFinished(Display *display, Window source, Window target, bool result, Atom action);
    char * GetXDndData(Display *display, Window requestor, Atom property, long time);

    void EndDndDrag(DndAction action);
    bool GrabDndSelection(Display *display, Window window, Time time);
    void HandleDndDragSourceEvent(XEvent event);
    void HandleDndSelectionRequest(XEvent event);
    Window GetDndTargetWindowForPos(int x, int y);

    void DrawDndSourceWindow();

    void SendDndSourcePosition(Window target, int x, int y, Time time);
    void SendDndSourceEnter(Window target);
    void SendDndSourceLeave(Window target);
    void SendDndSourceDrop(Window target, Time time);
    void SetDndSourceTargetWindow(Window target);

    static gboolean OnDragEndTimeout(gpointer data);

    Point _last_dnd_position;

    bool m_PauseGraphicsRendering;
    GLTimer m_Timer;
    float m_FrameTime;
    GpuDevice *m_DeviceFactory;
    GraphicsEngine *m_GraphicsContext;
    WindowStyle m_Style;

    Atom _xdnd_types[_xdnd_max_type + 1];
    Display *_drag_display;
    Window _drag_window;
    Window _drag_source;
    long _drag_drop_timestamp;

    void * _dnd_source_data;
    DndSourceFuncs _dnd_source_funcs;

    Window _dnd_source_window;
    Window _dnd_source_target_window;

    Window              _global_grab_window;

    void               *_global_pointer_grab_data;
    bool                _global_pointer_grab_active;
    GrabReleaseCallback _global_pointer_grab_callback;

    void               *_global_keyboard_grab_data;
    bool                _global_keyboard_grab_active;
    GrabReleaseCallback _global_keyboard_grab_callback;

    bool _dnd_is_drag_source;
    bool _dnd_source_target_accepts_drop;
    bool _dnd_source_grab_active;
    bool _dnd_source_drop_sent;
  public:
    ~GraphicsDisplay();
#ifndef NUX_OPENGLES_20
    GLEWContext *GetGLEWContext()
    {
      return &m_GLEWContext;
    }
    GLXEWContext *GetGLXEWContext()
    {
      return &m_GLXEWContext;
    }
#endif

    std::string FindResourceLocation(const char *ResourceFileName, bool ErrorOnFail = false);
    std::string FindUITextureLocation(const char *ResourceFileName, bool ErrorOnFail = false);
    std::string FindShaderLocation(const char *ResourceFileName, bool ErrorOnFail = false);
    std::string FindFontLocation(const char *ResourceFileName, bool ErrorOnFail = false);

    const std::vector<std::string>& GetFontSearchPath() const
    {
      return m_FontSearchPath;
    }
    const std::vector<std::string>& GetShaderSearchPath() const
    {
      return m_ShaderSearchPath;
    }
    const std::vector<std::string>& GetUITextureSearchPath() const
    {
      return m_UITextureSearchPath;
    }

  private:
    std::vector<std::string> m_FontSearchPath;
    std::vector<std::string> m_ShaderSearchPath;
    std::vector<std::string> m_UITextureSearchPath;
    FilePath m_ResourcePathLocation;

    GraphicsDisplay();
    GraphicsDisplay(const GraphicsDisplay &);
    // Does not make sense for a singleton. This is a self assignment.
    GraphicsDisplay &operator= (const GraphicsDisplay &);

#ifndef NUX_OPENGLES_20
    GLEWContext m_GLEWContext;
    GLXEWContext m_GLXEWContext;
#endif

    int MouseMove(XEvent xevent, Event *event);
    int MousePress(XEvent xevent, Event *event);
    int MouseRelease(XEvent xevent, Event *event);

    friend class DisplayAccessController;
    friend class GraphicsEngine;
  };

}

#endif //GRAPHICSDISPLAYX11_H
