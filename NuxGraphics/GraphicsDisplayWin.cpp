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


#include "GLResource.h"
#include "NuxGraphics.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GraphicsEngine.h"
#include "GLWindowManager.h"
#include "Events.h"

#include "GraphicsDisplay.h"

namespace nux
{

  // Compute the frame rate every FRAME_RATE_PERIODE;
  #define FRAME_RATE_PERIODE    10

  void ClipOrCenterRectToMonitor(LPRECT prc, unsigned int flags)
  {
    HMONITOR hMonitor;
    MONITORINFO mi;
    RECT        rc;
    int         w = prc->right  - prc->left;
    int         h = prc->bottom - prc->top;

    //
    // get the nearest monitor to the passed rect.
    //
    hMonitor = MonitorFromRect(prc, MONITOR_DEFAULTTONEAREST);

    //
    // get the work area or entire monitor rect.
    //
    mi.cbSize = sizeof(mi);
    GetMonitorInfo(hMonitor, &mi);

    //if(flags & MONITOR_WORKAREA)
    rc = mi.rcWork;
//    else
//        rc = mi.rcMonitor;

    //
    // center or clip the passed rect to the monitor rect
    //
    //if(flags & MONITOR_CENTER)
    {
      prc->left   = rc.left + (rc.right  - rc.left - w) / 2;
      prc->top    = rc.top  + (rc.bottom - rc.top  - h) / 2;
      prc->right  = prc->left + w;
      prc->bottom = prc->top  + h;
    }
//    else
//    {
//        prc->left   = Max(rc.left, Min(rc.right-w,  prc->left));
//        prc->top    = Max(rc.top,  Min(rc.bottom-h, prc->top));
//        prc->right  = prc->left + w;
//        prc->bottom = prc->top  + h;
//    }
  }

  EventToNameStruct EventToName[] =
  {
    {NUX_NO_EVENT,               "NUX_NO_EVENT" },
    {NUX_MOUSE_PRESSED,          "NUX_MOUSE_PRESSED" },
    {NUX_MOUSE_RELEASED,         "NUX_MOUSE_RELEASED" },
    {NUX_KEYDOWN,                "NUX_KEYDOWN" },
    {NUX_KEYUP,                  "NUX_KEYUP" },
    {NUX_MOUSE_MOVE,             "NUX_MOUSE_MOVE" },
    {NUX_SIZE_CONFIGURATION,     "NUX_SIZE_CONFIGURATION" },
    {NUX_NC_WINDOW_CONFIGURATION,   "NUX_NC_WINDOW_CONFIGURATION" },
    {NUX_WINDOW_ENTER_FOCUS,     "NUX_WINDOW_ENTER_FOCUS" },
    {NUX_WINDOW_EXIT_FOCUS,      "NUX_WINDOW_EXIT_FOCUS" },
    {NUX_WINDOW_DIRTY,           "NUX_WINDOW_DIRTY" },
    {NUX_WINDOW_MOUSELEAVE,      "NUX_WINDOW_MOUSELEAVE" },
    {NUX_TERMINATE_APP,          "NUX_TERMINATE_APP" }
  };

//---------------------------------------------------------------------------------------------------------
  HGLRC GraphicsDisplay::sMainGLRC = 0;
  HDC   GraphicsDisplay::sMainDC = 0;

  GraphicsDisplay::GraphicsDisplay()
    : gfx_interface_created_(false)
    , fullscreen_(false)
    , screen_bit_depth_(32)
    , m_DeviceFactory(0)
    , m_GraphicsContext(0)
    , m_Style(WINDOWSTYLE_NORMAL)
    , cursor_(0)
    , m_PauseGraphicsRendering(false)
    , parent_window_(0)
    , window_extended_style_(0)
    , window_style_(0)
    , device_context_(NULL)
    , wnd_handle_(NULL)
  {
    // Initialize Direct2D and DirectWrite
    d2d_factory_ = NULL;
    dw_factory_ = NULL;
    wic_factory_ = NULL;

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2d_factory_);

    if (hr == S_OK)
    {
      hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&dw_factory_));
    }

    // Initialize COM
    CoInitialize(NULL);

    hr = CoCreateInstance(
      CLSID_WICImagingFactory,
      NULL,
      CLSCTX_INPROC_SERVER,
      IID_IWICImagingFactory,
      (LPVOID*)&wic_factory_);

    inlSetThreadLocalStorage(_TLS_GraphicsDisplay, this);

    gfx_interface_created_ = false;
    GetDisplayInfo();

    window_size_.width = 0;
    window_size_.height = 0;

    // A window never starts in a minimized state.
    window_minimized_ = false;

    //_dnd_source_grab_active = false;
    _global_keyboard_grab_data = 0;
    _global_pointer_grab_data = 0;

  }

//---------------------------------------------------------------------------------------------------------
  GraphicsDisplay::~GraphicsDisplay()
  {
    NUX_SAFE_DELETE( m_GraphicsContext );
    NUX_SAFE_DELETE( m_DeviceFactory );

    DestroyOpenGLWindow();

    inlSetThreadLocalStorage(_TLS_GraphicsDisplay, 0);

    if (dw_factory_)
    {
      dw_factory_->Release();
      dw_factory_ = NULL;
    }

    if (d2d_factory_)
    {
      d2d_factory_->Release();
      d2d_factory_ = NULL;
    }

    if (wic_factory_)
    {
      wic_factory_->Release();
      wic_factory_ = NULL;
    }
  }

  ID2D1Factory* GraphicsDisplay::GetDirect2DFactory()
  {
    return d2d_factory_;
  }

  IDWriteFactory* GraphicsDisplay::GetDirectWriteFactory()
  {
    return dw_factory_;
  }

  IWICImagingFactory* GraphicsDisplay::GetWICFactory()
  {
    return wic_factory_;
  }

//---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::IsGfxInterfaceCreated()
  {
    return gfx_interface_created_;
  }

//---------------------------------------------------------------------------------------------------------
  static NCriticalSection CreateOpenGLWindow_CriticalSection;
  bool GraphicsDisplay::CreateOpenGLWindow(const char *WindowTitle,
                                         unsigned int WindowWidth,
                                         unsigned int WindowHeight,
                                         WindowStyle Style,
                                         const GraphicsDisplay *Parent,
                                         bool fullscreen_flag,
                                         bool create_rendering_data)
  {
    NScopeLock Scope(&CreateOpenGLWindow_CriticalSection);

    RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values

    gfx_interface_created_ = false;

    // FIXME : put at the end
    viewport_size_.width = WindowWidth;
    viewport_size_.height = WindowHeight;
    window_size_.width = WindowWidth;
    window_size_.height = WindowHeight;

    // end of fixme

    WindowRect.left     = (long) 0;
    WindowRect.right    = (long) viewport_size_.width;
    WindowRect.top      = (long) 0;
    WindowRect.bottom   = (long) viewport_size_.height;

    fullscreen_ = fullscreen_flag;              // Set The Global Fullscreen Flag

    if (fullscreen_)                           // Attempt Fullscreen Mode?
    {
      // check if resolution is supported
      bool mode_supported = false;

      for (int num_modes = 0 ; num_modes < m_num_gfx_device_modes; num_modes++)
      {
        if ((m_gfx_device_modes[num_modes].width == viewport_size_.width)
             && (m_gfx_device_modes[num_modes].height == viewport_size_.height)
             && (m_gfx_device_modes[num_modes].format == screen_bit_depth_))
        {
          mode_supported = true;
          break;
        }
      }

      if (mode_supported == false)
      {
        if (inlWin32MessageBox(NULL, "Info", MBTYPE_Ok, MBICON_Information, MBMODAL_ApplicationModal,
                                "The requested fullscreen mode is not supported by your monitor.\nUsing windowed mode instead.") == MBRES_Yes)
        {
          fullscreen_ = FALSE;   // Windowed Mode Selected.  Fullscreen = FALSE
        }
      }

      DEVMODE dmScreenSettings;                                               // Device Mode
      memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));	              // Makes Sure Memory's Cleared
      dmScreenSettings.dmSize = sizeof(dmScreenSettings);                    // Size Of The Devmode Structure
      dmScreenSettings.dmPelsWidth	= viewport_size_.width;                   // Selected Screen Width
      dmScreenSettings.dmPelsHeight	= viewport_size_.height;                  // Selected Screen Height
      dmScreenSettings.dmBitsPerPel	= screen_bit_depth_;                              // Selected Bits Per Pixel
      dmScreenSettings.dmDisplayFrequency = 60;
      dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

      // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
      if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
      {
        if (inlWin32MessageBox(NULL, "Info", MBTYPE_Ok, MBICON_Information, MBMODAL_ApplicationModal,
                                "The requested fullscreen mode is not supported by your monitor.\nUsing windowed mode instead.") == MBRES_Yes)
        {
          fullscreen_ = FALSE;                   // Windowed Mode Selected.  Fullscreen = FALSE
        }
        else
        {
          // Pop Up A Message Box Letting User Know The Program Is Closing.
          MessageBox(NULL, "Program Will Now Close.", "ERROR", MB_OK | MB_ICONSTOP);
        }
      }
    }

    window_extended_style_ = 0;
    window_style_ = 0;

    if (fullscreen_)                                   // Are We Still In Fullscreen Mode?
    {
      window_extended_style_ = WS_EX_APPWINDOW;                    // Window Extended Style
      window_style_ = WS_POPUP;                             // Windows Style
      ShowCursor(FALSE);                              // Hide Mouse Pointer
    }
    else
    {
      // Window Extended Style
      window_extended_style_ = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
      // Windows Style
      window_style_ = WS_OVERLAPPED;    // Creates an overlapped window. An overlapped window has a title bar and a border

      // See Win32 Window Hierarchy and Styles: http://msdn.microsoft.com/en-us/library/ms997562.aspx

      //WS_EX_APPWINDOW       // Forces a top-level window onto the taskbar when the window is visible
      //WS_EX_WINDOWEDGE      // Specifies that a window has a border with a raised edge

      //WS_POPUP      // Creates a pop-up window. This style cannot be used with the WS_CHILD style.
      //WS_SYSMENU    // Creates a window that has a window menu on its title bar. The WS_CAPTION style must also be specified.
      //WS_SIZEBOX    // Creates a window that has a sizing border. Same as the WS_THICKFRAME style.
      //WS_CAPTION    // Creates a window that has a title bar(includes the WS_BORDER style).

      m_Style = Style;

      if (Style == WINDOWSTYLE_TOOL)
      {
        window_extended_style_ = WS_EX_TOOLWINDOW;
        window_style_ = WS_CAPTION | WS_SYSMENU;
      }
      else if (Style == WINDOWSTYLE_DIALOG)
      {
        window_extended_style_ = WS_EX_DLGMODALFRAME;
        window_style_ = WS_CAPTION | WS_SYSMENU;
      }
      else if (Style == WINDOWSTYLE_NOBORDER)
      {
        window_extended_style_ = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
        window_style_ = WS_POPUP;
      }
      else if (Style == WINDOWSTYLE_PANEL)
      {
        window_extended_style_ = 0;           // Specifies that a window has a border with a raised edge
        window_style_ = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
      }
      else
      {
        // Normal Window: NormalWindow
        window_extended_style_ = WS_EX_APPWINDOW |   // Forces a top-level window onto the taskbar when the window is visible
                      WS_EX_WINDOWEDGE;   // Specifies that a window has a border with a raised edge

        window_style_ |= WS_CAPTION |         // Creates a window that has a title bar.
                     WS_SYSMENU |         // Creates a window that has a window menu on its title bar. The WS_CAPTION style must also be specified.
                     WS_THICKFRAME |      // Creates a window that has a sizing border.
                     WS_MINIMIZEBOX |     // Creates a window that has a minimize button.
                     WS_MAXIMIZEBOX |     // Creates a window that has a maximize button.
                     WS_BORDER;           // Creates a window that has a thin-line border.
      }
    }
    // The AdjustWindowRectEx function calculates the required size of the window rectangle,
    // based on the desired size of the client rectangle. The window rectangle can then be passed to
    // the CreateWindowEx function to create a window whose client area is the desired size.
    AdjustWindowRectEx(&WindowRect, window_style_, FALSE, window_extended_style_);    // Adjust Window To True Requested Size

    RECT rect;
    rect.top = 0;
    rect.bottom = WindowRect.bottom - WindowRect.top;
    rect.left = 0;
    rect.right = WindowRect.right - WindowRect.left;
    int WindowX = 0;
    int WindowY = 0;

    if (Parent)
    {
      parent_window_ = Parent->GetWindowHandle();
      GetWindowRect(parent_window_, &rect);

      int width = rect.right - rect.left;
      int height = rect.bottom - rect.top;

      WindowX = rect.left + (width - (WindowRect.right - WindowRect.left)) / 2;
      WindowY = rect.top + (height - (WindowRect.bottom - WindowRect.top)) / 2;
    }
    else if (!fullscreen_)
    {
      ClipOrCenterRectToMonitor(&rect, 0);
      WindowX = rect.left;
      WindowY = rect.top;
    }

    window_title_ = WindowTitle;

    // Create The Window
    if (! (wnd_handle_ = ::CreateWindowEx(window_extended_style_,         // Extended Style For The Window
                                      WINDOW_CLASS_NAME,                  // Class Name
                                      window_title_.c_str(),              // Window Title
                                      window_style_ |                     // Defined Window Style
                                      WS_CLIPSIBLINGS |                   // Required Window Style
                                      WS_CLIPCHILDREN,                    // Required Window Style
                                      WindowX, WindowY,                   // Window Position
                                      WindowRect.right - WindowRect.left, // Calculate Window Width
                                      WindowRect.bottom - WindowRect.top, // Calculate Window Height
                                      parent_window_,                     // No Parent Window
                                      NULL,                               // No Menu
                                      gGLWindowManager.GetInstance(),     // Instance
                                      NULL)))                             // Don't Pass Anything To WM_CREATE
    {
      DestroyOpenGLWindow();
      MessageBox(NULL, "Window Creation Error.", "ERROR", MB_OK | MB_ICONERROR);
                                                 // Return FALSE
    }

    static	PIXELFORMATDESCRIPTOR pfd =   // pfd Tells Windows How We Want Things To Be
    {
      sizeof(PIXELFORMATDESCRIPTOR),     // Size Of This Pixel Format Descriptor
      1,                                  // Version Number
      PFD_DRAW_TO_WINDOW |                // Format Must Support Window
      PFD_SUPPORT_OPENGL |                // Format Must Support OpenGL
      PFD_DOUBLEBUFFER,                   // Must Support Double Buffering
      PFD_TYPE_RGBA,                      // Request An RGBA Format
      24,                                 //        cColorBits
      //        Specifies the number of color bitplanes in each color buffer.
      //        For RGBA pixel types, it is the size of the color buffer, excluding the alpha bitplanes.
      //        For color-index pixels, it is the size of the color-index buffer.


      0, //        cRedBits
      //        Specifies the number of red bitplanes in each RGBA color buffer.
      0, //        cRedShift
      //        Specifies the shift count for red bitplanes in each RGBA color buffer.
      0, //        cGreenBits
      //        Specifies the number of green bitplanes in each RGBA color buffer.
      0, //        cGreenShift
      //        Specifies the shift count for green bitplanes in each RGBA color buffer.
      0, //        cBlueBits
      //        Specifies the number of blue bitplanes in each RGBA color buffer.
      0, //        cBlueShift
      //        Specifies the shift count for blue bitplanes in each RGBA color buffer.

      0, //        cAlphaBits
      //        Specifies the number of alpha bitplanes in each RGBA color buffer. Alpha bitplanes are not supported.
      0, //        cAlphaShift
      //        Specifies the shift count for alpha bitplanes in each RGBA color buffer. Alpha bitplanes are not supported.

      0, //        cAccumBits
      //        Specifies the total number of bitplanes in the accumulation buffer.
      0, //        cAccumRedBits
      //        Specifies the number of red bitplanes in the accumulation buffer.
      0, //        cAccumGreenBits
      //        Specifies the number of green bitplanes in the accumulation buffer.
      0, //        cAccumBlueBits
      //        Specifies the number of blue bitplanes in the accumulation buffer.
      0, //        cAccumAlphaBits
      //        Specifies the number of alpha bitplanes in the accumulation buffer.
      24,//        cDepthBits
      //        Specifies the depth of the depth(z-axis) buffer.
      8, //        cStencilBits
      //        Specifies the depth of the stencil buffer.
      0, //        cAuxBuffers
      //        Specifies the number of auxiliary buffers. Auxiliary buffers are not supported.
      PFD_MAIN_PLANE,   //        iLayerType
      //        Ignored. Earlier implementations of OpenGL used this member, but it is no longer used.
      0, //        bReserved
      //        Specifies the number of overlay and underlay planes. Bits 0 through 3 specify up to 15 overlay planes and bits 4 through 7 specify up to 15 underlay planes.
      0, //        dwLayerMask
      //        Ignored. Earlier implementations of OpenGL used this member, but it is no longer used.
      0, //        dwVisibleMask
      //        Specifies the transparent color or index of an underlay plane. When the pixel type is RGBA, dwVisibleMask is a transparent RGB color value. When the pixel type is color index, it is a transparent index value.
      0, //        dwDamageMask
      //        Ignored. Earlier implementations of OpenGL used this member, but it is no longer used.
    };

    device_context_ = GetDC(wnd_handle_);
    if (device_context_ == NULL) // Did We Get A Device Context?
    {
      DestroyOpenGLWindow();
      MessageBox(NULL, "Can't Create A GL Device Context.", "ERROR", MB_OK | MB_ICONERROR);
      return FALSE;
    }

    if (! (pixel_format_ = ChoosePixelFormat(device_context_, &pfd))) // Did Windows Find A Matching Pixel Format?
    {
      DestroyOpenGLWindow();
      MessageBox(NULL, "Can't Find A Suitable PixelFormat.", "ERROR", MB_OK | MB_ICONERROR);
      return FALSE;
    }

    if (!SetPixelFormat(device_context_, pixel_format_, &pfd))        // Are We Able To Set The Pixel Format?
    {
      DestroyOpenGLWindow();
      MessageBox(NULL, "Can't Set The PixelFormat.", "ERROR", MB_OK | MB_ICONERROR);
      return FALSE;
    }

    if (!(opengl_rendering_context_ = wglCreateContext(device_context_)))               // Are We Able To Get A Rendering Context?
    {
      DestroyOpenGLWindow();
      MessageBox(NULL, "Can't Create A GL Rendering Context.", "ERROR", MB_OK | MB_ICONERROR);
      return FALSE;
    }

    if (sMainGLRC == 0)
    {
      sMainGLRC = opengl_rendering_context_;
      sMainDC = device_context_;
    }
    else
    {
//         wglMakeCurrent(device_context_, 0);
//         // Make the newly created context share it resources with all the other OpenGL context
//         if (wglShareLists(sMainGLRC, opengl_rendering_context_) == FALSE)
//         {
//             DWORD err = GetLastError();
//             DestroyOpenGLWindow();
//             MessageBox(NULL, "Can't share GL context.", "ERROR", MB_OK|MB_ICONERROR);
//             return FALSE;
//         }
    }

    // This creates a  warning. It is a 64-bits compatibility issue.
    // When not in 64-bit you can disable the warning:
    // Project Properties --> C/C++ tab --> General --> Select "NO" for - Detect 64-bit Portability Issues.
    // See also SetWindowLongPtr
    SetWindowLongPtr(wnd_handle_, GWLP_USERDATA, (long) this);

    //::ShowWindow(wnd_handle_,SW_SHOW);           // Show The Window
    ::SetForegroundWindow(wnd_handle_);           // Slightly Higher Priority
    ::SetFocus(wnd_handle_);                      // Sets Keyboard Focus To The Window

    MakeGLContextCurrent();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SwapBuffer();

    gfx_interface_created_ = true;

    //m_GLEWContext = new GLEWContext();
    //m_WGLEWContext = new WGLEWContext();

    HGLRC new_opengl_rendering_context = opengl_rendering_context_;
    m_DeviceFactory = new GpuDevice(viewport_size_.width, viewport_size_.height, BITFMT_R8G8B8A8,
      device_context_,
      new_opengl_rendering_context,
      1, 0, false);

    if (m_DeviceFactory->GetGpuInfo().Support_EXT_Framebuffer_Object())
      m_DeviceFactory->GetFrameBufferObject()->SetupFrameBufferObject();

    if (new_opengl_rendering_context != 0)
    {
      opengl_rendering_context_ = new_opengl_rendering_context;
    }

    m_GraphicsContext = new GraphicsEngine(*this, create_rendering_data);

    //EnableVSyncSwapControl();
    //DisableVSyncSwapControl();

    InitGlobalGrabWindow();

    return true;
  }

  bool GraphicsDisplay::CreateFromOpenGLWindow(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext)
  {
    // Do not make the opengl context current
    // Do not swap the framebuffer
    // Do not clear the depth or color buffer
    // Do not enable/disable VSync

    wnd_handle_ = WindowHandle;
    device_context_ = WindowDCHandle;
    opengl_rendering_context_ = OpenGLRenderingContext;

    RECT rect;
    ::GetClientRect(wnd_handle_, &rect);
    window_size_ = Size(rect.right - rect.left, rect.bottom - rect.top);
    viewport_size_ = Size(rect.right - rect.left, rect.bottom - rect.top);

    // The opengl context should be made current by an external entity.

    gfx_interface_created_ = true;
    m_DeviceFactory = new GpuDevice(viewport_size_.width, viewport_size_.height, BITFMT_R8G8B8A8,
      device_context_,
      opengl_rendering_context_);

    if (m_DeviceFactory->GetGpuInfo().Support_EXT_Framebuffer_Object())
      m_DeviceFactory->GetFrameBufferObject()->SetupFrameBufferObject();

    m_GraphicsContext = new GraphicsEngine(*this);

    InitGlobalGrabWindow();

    return true;
  }

  GraphicsEngine* GraphicsDisplay::GetGraphicsEngine() const
  {
    return m_GraphicsContext;
  }
  
  GpuDevice* GraphicsDisplay::GetGpuDevice() const
  {
    return m_DeviceFactory;
  }

//---------------------------------------------------------------------------------------------------------
  // NUXTODO: remove this call. Make a direct access to GpuInfo via GpuDevice.
  bool GraphicsDisplay::HasFrameBufferSupport()
  {
    return m_DeviceFactory->GetGpuInfo().Support_EXT_Framebuffer_Object();
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::GetWindowSize(int &w, int &h)
  {
    w = window_size_.width;
    h = window_size_.height;
  }

//---------------------------------------------------------------------------------------------------------
  int GraphicsDisplay::GetWindowWidth()
  {
    return window_size_.width;
  }

//---------------------------------------------------------------------------------------------------------
  int GraphicsDisplay::GetWindowHeight()
  {
    return window_size_.height;
  }

  void GraphicsDisplay::ResetWindowSize()
  {
    RECT rect;
    ::GetClientRect(wnd_handle_, &rect);
    window_size_ = Size(rect.right - rect.left, rect.bottom - rect.top);
    viewport_size_ = Size(rect.right - rect.left, rect.bottom - rect.top);
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::SetWindowSize(int width, int height)
  {
    RECT window_rect;
    RECT new_rect;
    ::GetWindowRect(wnd_handle_, &window_rect);

    new_rect.left = 0;
    new_rect.right = width;
    new_rect.top = 0;
    new_rect.bottom = height;
    BOOL b = ::AdjustWindowRectEx(&new_rect, window_style_, FALSE, window_extended_style_);    // Adjust Window To True Requested Size

    ::MoveWindow(wnd_handle_,
                  window_rect.left,
                  window_rect.top,
                  (new_rect.right - new_rect.left),
                  (new_rect.bottom - new_rect.top),
                  TRUE);
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::SetViewPort(int x, int y, int width, int height)
  {
    if (IsGfxInterfaceCreated())
    {
      //do not rely on viewport_size_: glViewport can be called directly
      viewport_size_.width = width;
      viewport_size_.height = height;

      m_GraphicsContext->SetViewport(x, y, viewport_size_.width, viewport_size_.height);
      m_GraphicsContext->SetScissor(0, 0, width, height);
    }
  }

  Point GraphicsDisplay::GetMouseScreenCoord()
  {
    POINT pt;
    ::GetCursorPos(&pt);
    ScreenToClient(wnd_handle_, &pt);
    Point point(pt.x, pt.y);
    return point;
  }

  Point GraphicsDisplay::GetMouseWindowCoord()
  {
    POINT pt;
    ::GetCursorPos(&pt);
    ::ScreenToClient(wnd_handle_, &pt);
    Point point(pt.x, pt.y);
    return point;
  }

  Point GraphicsDisplay::GetWindowCoord()
  {
    RECT rect;
    ::GetWindowRect(wnd_handle_, &rect);
    Point point(rect.left, rect.top);
    return point;
  }

  Rect GraphicsDisplay::GetWindowGeometry()
  {
    RECT rect;
    ::GetClientRect(wnd_handle_, &rect);
    Rect geo(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    return geo;
  }

  Rect GraphicsDisplay::GetNCWindowGeometry()
  {
    RECT rect;
    ::GetWindowRect(wnd_handle_, &rect);
    Rect geo(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    return geo;
  }

  void GraphicsDisplay::MakeGLContextCurrent(bool b)
  {
    HGLRC glrc = opengl_rendering_context_;

    if (b == false)
    {
      glrc = 0;
    }

    if (!wglMakeCurrent(device_context_, glrc))
    {
      std::string error = inlGetSystemErrorMessage();
      DestroyOpenGLWindow();
      MessageBox(NULL, "Can't Activate The GL Rendering Context.", "ERROR", MB_OK | MB_ICONERROR);
    }
  }

  void GraphicsDisplay::SwapBuffer(bool glswap)
  {
    if (IsPauseThreadGraphicsRendering())
      return;

    if (glswap)
    {
      SwapBuffers(device_context_);
    }

    m_FrameTime = m_Timer.PassedMilliseconds();

//     if (16.6f - m_FrameTime > 0)
//     {
//         SleepForMilliseconds(16.6f - m_FrameTime);
//     }
//
//     m_FrameTime = m_Timer.PassedMilliseconds();
//     m_Timer.Reset();
//     m_PeriodeTime += m_FrameTime;
//
//     m_FrameCounter++;
//     m_FramePeriodeCounter++;
//     if (m_FramePeriodeCounter >= FRAME_RATE_PERIODE)
//     {
//         m_FrameRate = m_FramePeriodeCounter * 1000.0f / m_PeriodeTime;
//         m_PeriodeTime = 0.0f;
//         m_FramePeriodeCounter = 0;
//     }
  }
//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::DestroyOpenGLWindow()
  {
    if (gfx_interface_created_ == true)
    {
      if (fullscreen_)                                   // Are We In Fullscreen Mode?
      {
        ChangeDisplaySettings(NULL, 0);                  // If So Switch Back To The Desktop
        ShowCursor(TRUE);                                // Show Mouse Pointer
      }

      if (opengl_rendering_context_)                      // Do We Have A Rendering Context?
      {
        if (!wglMakeCurrent(device_context_, NULL))     // Are We Able To Release The DC And RC Contexts?
        {
          MessageBox(NULL, "Release Of DC And RC Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        }

        if (!wglDeleteContext(opengl_rendering_context_))           // Are We Able To Delete The RC?
        {
          MessageBox(NULL, "Release Rendering Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
        }

        opengl_rendering_context_ = NULL;                             // Set RC To NULL
      }

      if (device_context_ && (ReleaseDC(wnd_handle_, device_context_) == 0))   // Are We Able To Release The DC
      {
        MessageBox(NULL, "Release Device Context Failed.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
      }
      device_context_ = NULL;                                       // Set DC To NULL

      if (wnd_handle_ && (::DestroyWindow(wnd_handle_) == 0))                       // Are We Able To Destroy The Window?
      {
        MessageBox(NULL, "Could Not Release window handle.", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
      }
      wnd_handle_ = NULL;                                                // Set Window Handle To NULL
    }

    gfx_interface_created_ = false;
  }


// //---------------------------------------------------------------------------------------------------------
// // convert a MSWindows VK_x to an Fltk(X) Keysym:
// // See also the inverse converter in Fl_get_key_win32.cxx
// // This table is in numeric order by VK:
// static const struct {unsigned short vk, fltk, extended;} vktab[] = {
//     {NUX_VK_BACK,	    NUX_BackSpace},
//     {NUX_VK_TAB,	    NUX_Tab},
//     {NUX_VK_CLEAR,	    NUX_Clear,	    0xff0b/*XK_Clear*/},
//     {NUX_VK_ENTER,	    NUX_Enter,	    NUX_KP_ENTER},
//     {NUX_VK_SHIFT,	    NUX_Shift_L,	NUX_EXT_Shift_R},
//     {NUX_VK_CONTROL,	NUX_Control_L,	NUX_EXT_Control_R},
//     {NUX_VK_MENU,	    NUX_Alt_L,	    NUX_EXT_Alt_R},
//     {NUX_VK_PAUSE,	    NUX_Pause},
//     {NUX_VK_CAPITAL,	NUX_Caps_Lock},
//     {NUX_VK_ESCAPE,	    NUX_Escape},
//     {NUX_VK_SPACE,	    ' '},
//     {NUX_VK_PAGE_UP,	NUX_Page_Up     /*KP+'9'*/,	    NUX_KP_PAGE_UP},
//     {NUX_VK_PAGE_DOWN,  NUX_Page_Down   /*KP+'3'*/,	    NUX_KP_PAGE_DOWN},
//     {NUX_VK_END,	    NUX_End         /*KP+'1'*/,	    NUX_KP_END},
//     {NUX_VK_HOME,	    NUX_Home        /*KP+'7'*/,	    NUX_KP_HOME},
//     {NUX_VK_LEFT,	    NUX_Left        /*KP+'4'*/,	    NUX_KP_LEFT},
//     {NUX_VK_UP,	        NUX_Up          /*KP+'8'*/,	    NUX_KP_UP},
//     {NUX_VK_RIGHT,	    NUX_Right       /*KP+'6'*/,	    NUX_KP_RIGHT},
//     {NUX_VK_DOWN,	    NUX_Down        /*KP+'2'*/,	    NUX_KP_DOWN},
//     {NUX_VK_SNAPSHOT,	NUX_Print},	    // does not work on NT
//     {NUX_VK_INSERT,	    NUX_Insert      /*KP+'0'*/,	    NUX_KP_INSERT},
//     {NUX_VK_DELETE,	    NUX_Delete      /*KP+'.'*/,	    NUX_KP_DELETE},
//     {NUX_VK_LWIN,	    NUX_LWin        /*Meta_L*/},
//     {NUX_VK_RWIN,	    NUX_RWin        /*Meta_R*/},
//     {NUX_VK_APPS,	    NUX_VK_APPS     /*Menu*/},
//     {NUX_VK_MULTIPLY,	NUX_Multiply    /*KP+'*'*/},
//     {NUX_VK_ADD,	    NUX_Add         /*KP+'+'*/},
//     {NUX_VK_SUBTRACT,	NUX_Subtract    /*KP+'-'*/},
//     {NUX_VK_DECIMAL,	NUX_Decimal     /*KP+'.'*/},
//     {NUX_VK_DIVIDE,	    NUX_Divide      /*KP+'/'*/},
//     {NUX_VK_NUMLOCK,	NUX_Numlock     /*Num_Lock*/},
//     {NUX_VK_SCROLL,	    NUX_Scroll      /*Scroll_Lock*/},
//     {0xba,	';'},
//     {0xbb,	'='},
//     {0xbc,	','},
//     {0xbd,	'-'},
//     {0xbe,	'.'},
//     {0xbf,	'/'},
//     {0xc0,	'`'},
//     {0xdb,	'['},
//     {0xdc,	'\\'},
//     {0xdd,	']'},
//     {0xde,	'\''}
// };

  /*
      The extended-key flag indicates whether the keystroke message originated
      from one of the additional keys on the enhanced keyboard. The extended keys
      consist of the ALT and CTRL keys on the right-hand side of the keyboard;
      the INS, DEL, HOME, END, PAGE UP, PAGE DOWN, and arrow keys in the clusters
      to the left of the numeric keypad; the NUM LOCK key; the BREAK(CTRL+PAUSE) key;
      the PRINT SCRN key; and the divide(/) and ENTER keys in the numeric keypad.
      The extended-key flag is set if the key is an extended key.
  */
//---------------------------------------------------------------------------------------------------------
  /*static int ms2fltk(int vk, int extended)
  {
      static unsigned short vklut[256];
      static unsigned short extendedlut[256];
      if (!vklut[1])
      {
          // init the table
          unsigned int i;
          for (i = 0; i < 256; i++)
          {
              vklut[i] = i; //tolower(i);
          }
  //        for (i=VK_F1; i<=VK_F16; i++)
  //        {
  //            vklut[i] = i+(FL_F-(VK_F1-1));   // (FL_F + 1 -> VK_F1) ... (FL_F + 16 -> VK_F16)
  //        }
  //        for (i=VK_NUMPAD0; i<=VK_NUMPAD9; i++)
  //        {
  //            vklut[i] = i+(FL_KP+'0'-VK_NUMPAD0);    // (FL_KP + '0' -> VK_NUMPAD0) ... (FL_KP + '9' = VK_NUMPAD9)
  //        }
          for (i = 0; i < sizeof(vktab)/sizeof(*vktab); i++)
          {
              vklut[vktab[i].vk] = vktab[i].fltk;
              extendedlut[vktab[i].vk] = vktab[i].extended;
          }
          for (i = 0; i < 256; i++)
          {
              if (!extendedlut[i])
                  extendedlut[i] = vklut[i];
          }
      }

      return extended ? extendedlut[vk] : vklut[vk];
  }*/
//---------------------------------------------------------------------------------------------------------
  static int InspectMouseEvent(HWND window, Event& event, int what, int button,
                          WPARAM wParam, LPARAM lParam)
  {
    static int pmx, pmy;
    event.x = (signed short) LOWORD(lParam);
    event.y = (signed short) HIWORD(lParam);
    event.x_root = 0;
    event.y_root = 0;

    POINT EventScreenPosition;

    ClientToScreen(window, &EventScreenPosition);
    EventScreenPosition.x = event.x;
    EventScreenPosition.y = event.y;
    POINT WindowScreenPosition;
    WindowScreenPosition.x = WindowScreenPosition.y = 0;
    ClientToScreen(window, &WindowScreenPosition);

    // Erase mouse event and mouse doubleclick events. Keep the mouse states.
    ulong _mouse_state = event.mouse_state & 0x0F000000;

    // establish cause of the event
//     if (button == 1)
//         _mouse_state |= NUX_EVENT_BUTTON1;
//     else if (button == 2)
//         _mouse_state |= NUX_EVENT_BUTTON2;
//     else if (button == 3)
//         _mouse_state |= NUX_EVENT_BUTTON3;
//     else
    if (button == 4)
    {
      event.mouse_state |= NUX_EVENT_MOUSEWHEEL;
      event.type = NUX_MOUSE_WHEEL;

      int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
      int xPos = (int) (short) LOWORD(lParam) - WindowScreenPosition.x;
      int yPos = (int) (short) HIWORD(lParam) - WindowScreenPosition.y;
      event.x = xPos;
      event.y = yPos;

      event.wheel_delta = zDelta;
      return 1;
    }

    // set the cause of the event
    switch(what)
    {
      case 0:
      {
        // mouse down
        if (button == 1)
        {
          _mouse_state |= NUX_STATE_BUTTON1_DOWN;
          _mouse_state |= NUX_EVENT_BUTTON1_DOWN;
        }
        else if (button == 2)
        {
          _mouse_state |= NUX_STATE_BUTTON2_DOWN;
          _mouse_state |= NUX_EVENT_BUTTON2_DOWN;
        }
        else if (button == 3)
        {
          _mouse_state |= NUX_STATE_BUTTON3_DOWN;
          _mouse_state |= NUX_EVENT_BUTTON3_DOWN;
        }
      }
      break;

      case 1:
      {
        // double click
        if (button == 1)
        {
          _mouse_state |= NUX_EVENT_BUTTON1_DBLCLICK;
          _mouse_state |= NUX_STATE_BUTTON1_DOWN;
          _mouse_state |= NUX_EVENT_BUTTON1_DOWN;
        }
        else if (button == 2)
        {
          _mouse_state |= NUX_EVENT_BUTTON2_DBLCLICK;
          _mouse_state |= NUX_STATE_BUTTON2_DOWN;
          _mouse_state |= NUX_EVENT_BUTTON2_DOWN;
        }
        else if (button == 3)
        {
          _mouse_state |= NUX_EVENT_BUTTON3_DBLCLICK;
          _mouse_state |= NUX_STATE_BUTTON3_DOWN;
          _mouse_state |= NUX_EVENT_BUTTON3_DOWN;
        }
      }
      break;

      case 2:
      {
        // button up
        if (button == 1)
        {
          _mouse_state &= ~NUX_STATE_BUTTON1_DOWN;
          _mouse_state |= NUX_EVENT_BUTTON1_UP;
        }
        else if (button == 2)
        {
          _mouse_state &= ~NUX_STATE_BUTTON2_DOWN;
          _mouse_state |= NUX_EVENT_BUTTON2_UP;
        }
        else if (button == 3)
        {
          _mouse_state &= ~NUX_STATE_BUTTON3_DOWN;
          _mouse_state |= NUX_EVENT_BUTTON3_UP;
        }
      }
      break;
    }

    event.mouse_state = _mouse_state;

    switch(what)
    {
      static int px, py;
      case 1: // double-click

        if (event.is_click)
        {
          event.clicks++;
          // The SetCapture function sets the mouse capture to the specified window belonging to
          // the current thread. SetCapture captures mouse input either when the mouse is over the
          // capturing window, or when the mouse button was pressed while the mouse was over the
          // capturing window and the button is still down. Only one window at a time can capture the mouse.
          SetCapture(window);
          event.is_click = 1;
          px = pmx = event.x;
          py = pmy = event.y;
          event.type = NUX_MOUSE_DOUBLECLICK;
          return 1;
        }

      case 0: // single-click
        event.clicks = 0;
        // The SetCapture function sets the mouse capture to the specified window belonging to
        // the current thread. SetCapture captures mouse input either when the mouse is over the
        // capturing window, or when the mouse button was pressed while the mouse was over the
        // capturing window and the button is still down. Only one window at a time can capture the mouse.
        SetCapture(window);
        event.is_click = 1;
        px = pmx = event.x;
        py = pmy = event.y;
        event.type = NUX_MOUSE_PRESSED;
        return 1;

      case 2: // release:
        // The ReleaseCapture function releases the mouse capture from a window in the current thread
        // and restores normal mouse input processing. A window that has captured the mouse receives all
        // mouse input, regardless of the position of the cursor, except when a mouse button is clicked
        // while the cursor hot spot is in the window of another thread.
        ReleaseCapture();
        event.type = NUX_MOUSE_RELEASED;
        return 1;

      case 3: // move:
      default: // avoid compiler warning
        // MSWindows produces extra events even if mouse does not move, ignore them.
        // http://blogs.msdn.com/oldnewthing/archive/2003/10/01/55108.aspx: Why do I get spurious WM_MOUSEMOVE messages?
        if (event.x == pmx && event.y == pmy)
          return 1;

        pmx = event.x;
        pmy = event.y;
//        if (abs(event.x - px)>5 || abs(event.y - py)>5)
//            event.is_click = 0;
        event.type = NUX_MOUSE_MOVE;
        return 1;
    }

    return 0;
  }

  unsigned int GetModifierKeyState()
  {
    unsigned int state = 0;
    unsigned short r = 0;

    // For CapsLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the CapsLock: on(keyboard light is on) or off?
    r = GetKeyState(VK_CAPITAL);

    if (r & 1)
      state |= NUX_STATE_CAPS_LOCK;

    // For NumLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the NumLock: on(keyboard light is on) or off?
    r = GetKeyState(VK_NUMLOCK);

    if (r & 1)
      state |= NUX_STATE_NUMLOCK;

    r = GetKeyState(VK_SCROLL);

    if (r & 0x8000)
      state |= NUX_STATE_SCROLLLOCK;

    r = GetKeyState(VK_CONTROL);

    if (r & 0x8000)
      state |= NUX_STATE_CTRL;

    r = GetKeyState(VK_SHIFT);

    if (r & 0x8000)
      state |= NUX_STATE_SHIFT;

    r = GetKeyState(VK_MENU);

    if (r & 0x8000)
      state |= NUX_STATE_ALT;



    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) &~1)
    {
      // WIN32 bug?  GetKeyState returns garbage if the user hit the
      // meta key to pop up start menu.  Sigh.
      if ((GetAsyncKeyState(VK_LWIN) | GetAsyncKeyState(VK_RWIN)) &~1)
        state |= NUX_STATE_SUPER;
    }

    return state;
  }

//---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::GetSystemEvent(Event *event)
  {
    MSG		msg;
    event_.Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    event_.mouse_state &= 0x0F000000;
    bool got_event;

    // Always set the second parameter of PeekMessage to NULL. Indeed, many services creates
    // windows on the program behalf. If pass the main window as filter, we will miss all the
    // messages from the other windows.
    // Same with GetMessage.
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting? If yes, remove it.
    {
      if (msg.message == NUX_THREADMSG_START_RENDERING)
      {
        m_PauseGraphicsRendering = false;
        MakeGLContextCurrent(true);
      }

      TranslateMessage(&msg);
      DispatchMessage(&msg);

      memcpy(event, &event_, sizeof(Event));
      got_event = true;
    }
    else
    {
      memcpy(event, &event_, sizeof(Event));
      got_event = false;
    }

    if (msg.message == WM_QUIT)
    {
      // Re-post the message that we retrieved so other modal loops will catch it.
      // See [Modality, part 3: The WM_QUIT message] http://blogs.msdn.com/oldnewthing/archive/2005/02/22/378018.aspx
      PostQuitMessage(msg.wParam);

      event_.type = NUX_TERMINATE_APP;
      memcpy(event, &event_, sizeof(Event));
    }

    if (msg.message == -1) // error
    {
      event_.type = NUX_NO_EVENT;
      memcpy(event, &event_, sizeof(Event));
    }

    return got_event;
  }

  void GraphicsDisplay::ProcessForeignWin32Event(HWND hWnd, MSG msg, WPARAM wParam, LPARAM lParam, Event *event)
  {
    event_.Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    event_.mouse_state &= 0x0F000000;

    // Always set the second parameter of PeekMessage to NULL. Indeed, many services creates
    // windows on the program behalf. If pass the main window as filter, we will miss all the
    // messages from the other windows.
    // Same with GetMessage.
    ProcessWin32Event(hWnd, msg.message, wParam, lParam);
    memcpy(event, &event_, sizeof(Event));

    if (msg.message != WM_QUIT)
    {
      // Re-post the message that we retrieved so other modal loops will catch it.
      // See [Modality, part 3: The WM_QUIT message] http://blogs.msdn.com/oldnewthing/archive/2005/02/22/378018.aspx
      PostQuitMessage(msg.wParam);

      event_.type = NUX_TERMINATE_APP;
      memcpy(event, &event_, sizeof(Event));
    }

    if (msg.message == -1) // error
    {
      event_.type = NUX_NO_EVENT;
      memcpy(event, &event_, sizeof(Event));
    }
  }

  const Event& GraphicsDisplay::GetCurrentEvent() const
  {
    return event_;
  }

//---------------------------------------------------------------------------------------------------------
  LRESULT CALLBACK WndProcManager(HWND	hWnd,			// Handle For This Window
                                   UINT	uMsg,			// Message For This Window
                                   WPARAM	wParam,			// Additional Message Information
                                   LPARAM	lParam)			// Additional Message Information
  {
    // This creates a  warning. It is a 64-bits compatibility issue.
    // When not in 64-bit you can disable the warning:
    // Project Properties --> C/C++ tab --> General --> Select "NO" for - Detect 64-bit Portability Issues.
    // See also GetWindowLongPtr
    GraphicsDisplay *GLWindow = reinterpret_cast<GraphicsDisplay *> (::GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if (GLWindow == 0)
    {
      switch(uMsg)
      {
          // These cases happens before GLWindow is set. So we must catch them here.

        case WM_NCCREATE:
        {
          // Non Client Area

          // WARNING: MUST RETURN TRUE TO CONTINUE WITH THE CREATION OF THE WINDOW
          // return TRUE;

          // WARNING: Continue with DefWindowProc if you want the title of the window to appears
          // or you have to set it yourself before returning true
          break;
        }
        case WM_CREATE:
        {
          // WARNING: MUST RETURN 0 TO CONTINUE WITH THE CREATION OF THE WINDOW
          return 0;
        }
      }

      return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return GLWindow->ProcessWin32Event(hWnd, uMsg, wParam, lParam);
  }

  LRESULT GraphicsDisplay::ProcessWin32Event(HWND	hWnd,          // Handle For This Window
      UINT	uMsg,           // Message For This Window
      WPARAM	wParam,		// Additional Message Information
      LPARAM	lParam)		// Additional Message Information
  {
    switch(uMsg)
    {
      case WM_DESTROY:
      {
        nuxDebugMsg("[GraphicsDisplay::WndProc]: Window \"%s\" received WM_DESTROY message.", window_title_.c_str());
        break;
      }

      case WM_CLOSE:
      {
        nuxDebugMsg("[GraphicsDisplay::WndProc]: Window \"%s\" received WM_CLOSE message.", window_title_.c_str());
        // close? yes or no?
        PostQuitMessage(0);
        return 0;
      }

      case WM_PAINT:
      {
        ValidateRect(hWnd, NULL); //  validate the surface to avoid receiving WM_PAINT continuously
        event_.type = NUX_WINDOW_DIRTY;
        break;
      }

      case WM_CAPTURECHANGED:
      {
        // The WM_CAPTURECHANGED message is sent to the window that is losing the mouse capture
        if ((HWND) lParam == hWnd)
        {
          // Cancel everything about the mouse state and send a NUX_WINDOW_EXIT_FOCUS message.
          event_.mouse_state = 0;
          //nuxDebugMsg("Windows Msg: WM_CAPTURECHANGED/NUX_WINDOW_EXIT_FOCUS");
          return 0;
        }

        break;
      }

      case WM_ENTERSIZEMOVE:
      {
        return 0;
      }

      case WM_EXITSIZEMOVE:
      {
        RECT clientrect;
        GetClientRect( hWnd, &clientrect);

        event_.type = NUX_SIZE_CONFIGURATION;
        event_.width =  clientrect.right - clientrect.left;
        event_.height =  clientrect.bottom - clientrect.top;
        return 0;
      }

      case WM_SIZE:
      {
        RECT clientrect;
        GetClientRect( hWnd, &clientrect);

        event_.type = NUX_NO_EVENT; //NUX_SIZE_CONFIGURATION;
        event_.width =  clientrect.right - clientrect.left;
        event_.height =  clientrect.bottom - clientrect.top;

        //setViewPort(0, 0, clientrect.right - clientrect.left, clientrect.bottom - clientrect.top);
        window_size_.width = clientrect.right - clientrect.left;
        window_size_.height = clientrect.bottom - clientrect.top;

        if ((wParam == SIZE_MAXHIDE) || (wParam == SIZE_MINIMIZED))
        {
          window_minimized_ = true;
        }
        else
        {
          window_minimized_ = false;
        }

        if ((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED))
        {
          event_.type = NUX_SIZE_CONFIGURATION;
        }

        return 0;
      }

      case WM_SETFOCUS:
      {
        event_.type = NUX_WINDOW_ENTER_FOCUS;
        event_.mouse_state = 0;

        // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process().
        // Because WM_SETFOCUS can happen with the mouse outside of the client area, we set x and y so that the mouse will be
        // outside of all widgets. A subsequent mouse down or mouse move event will set the correct values for x and y.
        event_.x = 0xFFFFFFFF;
        event_.y = 0xFFFFFFFF;
        event_.dx = 0;
        event_.dy = 0;
        event_.virtual_code = 0;
        //nuxDebugMsg("Windows Msg: WM_SETFOCUS/NUX_WINDOW_ENTER_FOCUS");
        break;
      }

      case WM_KILLFOCUS:
      {
        event_.type = NUX_WINDOW_EXIT_FOCUS;
        event_.mouse_state = 0;

        // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process()
        event_.x = 0xFFFFFFFF;
        event_.y = 0xFFFFFFFF;
        event_.dx = 0;
        event_.dy = 0;
        event_.virtual_code = 0;
        //nuxDebugMsg("Windows Msg: WM_KILLFOCUS/NUX_WINDOW_EXIT_FOCUS");
        break;
      }

      case WM_NCHITTEST:
      {
        break;
      }

      case WM_ACTIVATE:
      {
        if (LOWORD(wParam) != WA_INACTIVE)
        {
          event_.type = NUX_WINDOW_ENTER_FOCUS;
        }
        else
        {
          event_.type = NUX_WINDOW_EXIT_FOCUS;
        }
        event_.mouse_state = 0;

        // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process().
        // Because WM_SETFOCUS can happen with the mouse outside of the client area, we set x and y so that the mouse will be
        // outside of all widgets. A subsequent mouse down or mouse move event will set the correct values for x and y.
        event_.x = 0xFFFFFFFF;
        event_.y = 0xFFFFFFFF;
        event_.dx = 0;
        event_.dy = 0;
        event_.virtual_code = 0;

        event_.key_modifiers = GetModifierKeyState();
        return 0;
      }

      case WM_ACTIVATEAPP:
        {
          if (wParam)
          {
            event_.type = NUX_WINDOW_ENTER_FOCUS;
          }
          else
          {
            event_.type = NUX_WINDOW_EXIT_FOCUS;
          }
          event_.mouse_state = 0;

          // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process().
          // Because WM_SETFOCUS can happen with the mouse outside of the client area, we set x and y so that the mouse will be
          // outside of all widgets. A subsequent mouse down or mouse move event will set the correct values for x and y.
          event_.x = 0xFFFFFFFF;
          event_.y = 0xFFFFFFFF;
          event_.dx = 0;
          event_.dy = 0;
          event_.virtual_code = 0;

          event_.key_modifiers = GetModifierKeyState();
          return 0;
        }

      case WM_SYSKEYDOWN:
      case WM_KEYDOWN:
      {
        event_.type = NUX_KEYDOWN;
        event_.key_modifiers = GetModifierKeyState();
        event_.win32_keysym = wParam;

        if ((uMsg == WM_KEYDOWN) || (uMsg == WM_SYSKEYDOWN))
        {
          event_.VirtualKeycodeState[GraphicsDisplay::Win32KeySymToINL(wParam) ] = 1;
        }

        if (wParam == VK_CONTROL)
        {
          if (lParam & (1 << 24))
          {
            event_.win32_keysym = NUX_VK_RCONTROL;
          }
          else
          {
            event_.win32_keysym = NUX_VK_LCONTROL;
          }
        }

        if (wParam == VK_MENU)
        {
          if (lParam & (1 << 24))
          {
            event_.win32_keysym = NUX_VK_RALT;
          }
          else
          {
            event_.win32_keysym = NUX_VK_LALT;
          }
        }

        if (wParam == VK_SHIFT)
        {
          if (HIWORD(GetAsyncKeyState(VK_LSHIFT)))
          {
            event_.win32_keysym = NUX_VK_LSHIFT;
          }
          else if (HIWORD(GetAsyncKeyState(VK_RSHIFT)))
          {
            event_.win32_keysym = NUX_VK_RSHIFT;
          }
        }

        break;
      }

      case WM_SYSKEYUP:
      case WM_KEYUP:
      {
        event_.type = NUX_KEYUP;
        event_.key_modifiers = GetModifierKeyState();
        event_.win32_keysym = wParam;

        if ((uMsg == WM_KEYUP) || (uMsg == WM_SYSKEYUP))
        {
          event_.VirtualKeycodeState[GraphicsDisplay::Win32KeySymToINL(wParam) ] = 0;
        }

        break;
      }

      case WM_DEADCHAR:
      case WM_SYSDEADCHAR:
      case WM_CHAR:
      case WM_SYSCHAR:
      {
        event_.key_modifiers = GetModifierKeyState();

        // reset key repeat count to 0.
        event_.key_repeat_count = 0;

        if (lParam & (1 << 31))
        {
          // key up events.
          event_.type = NUX_KEYUP;
          return 0;
        }
        else
        {
          // key down events.
          event_.type = NUX_KEYDOWN;
          event_.key_repeat_count = (int) (lParam & 0xff);
        }


        if ((wParam == '\b') || // backspace
          (wParam == '\t') || // tab
          (wParam == '\n') || // linefeed
          (wParam == '\r')) // carriage return
        {
          return 0;
        }

        wchar_t *utf16_str = new wchar_t [4];
        std::memset(utf16_str, 0, sizeof(wchar_t) * 4);
        std::memcpy(utf16_str, (int*) &wParam, sizeof(wParam));
        wchar_t *temp0 = utf16_str;

        unsigned char *utf8_str = new unsigned char [NUX_EVENT_TEXT_BUFFER_SIZE];
        std::memset(utf8_str, 0, sizeof(unsigned char) * NUX_EVENT_TEXT_BUFFER_SIZE);
        unsigned char *temp1 = utf8_str;


        ConversionResult res = ConvertUTF16toUTF8((const wchar_t **) &temp0,
          utf16_str + sizeof(wchar_t) * 4,
          &temp1,
          utf8_str + NUX_EVENT_TEXT_BUFFER_SIZE,
          lenientConversion);

        if (res == conversionOK)
        {
          std::memcpy(event_.text, utf8_str, NUX_EVENT_TEXT_BUFFER_SIZE);
        }
        delete utf8_str;
        delete utf16_str;

        return 0;
      }

      case WM_UNICHAR:
      {
        if (wParam == UNICODE_NOCHAR)
          return 1;

        event_.key_modifiers = GetModifierKeyState();

        // reset key repeat count to 0.
        event_.key_repeat_count = 0;

        if (lParam & (1 << 31))
        {
          // key up events.
          event_.type = NUX_KEYUP;
          return 0;
        }
        else
        {
          // key down events.
          event_.type = NUX_KEYDOWN;
          event_.key_repeat_count = (int) (lParam & 0xff);
        }

        unsigned int *utf32_str = new unsigned int [4];
        std::memset(utf32_str, 0, sizeof(unsigned int) * 4);
        std::memcpy(utf32_str, (int*) &wParam, sizeof(wParam));
        unsigned int *temp0 = utf32_str;

        unsigned char *utf8_str = new unsigned char [NUX_EVENT_TEXT_BUFFER_SIZE];
        std::memset(utf8_str, 0, sizeof(unsigned char) * NUX_EVENT_TEXT_BUFFER_SIZE);
        unsigned char *temp1 = utf8_str;


        ConversionResult res = ConvertUTF32toUTF8((const unsigned int**) &temp0,
          utf32_str + sizeof(unsigned int) * 4,
          &temp1,
          utf8_str + NUX_EVENT_TEXT_BUFFER_SIZE,
          lenientConversion);

        if (res == conversionOK)
        {
          std::memcpy(event_.text, utf8_str, NUX_EVENT_TEXT_BUFFER_SIZE);
        }
        delete utf8_str;
        delete utf32_str;
      }

      case WM_LBUTTONDOWN:
      {
        InspectMouseEvent(hWnd, event_, 0, 1, wParam, lParam);
        //nuxDebugMsg("Windows Msg: WM_LBUTTONDOWN");
        return 0;
      }
      case WM_LBUTTONDBLCLK:
      {
        InspectMouseEvent(hWnd, event_, 1, 1, wParam, lParam);
        //nuxDebugMsg("Windows Msg: WM_LBUTTONDBLCLK");
        return 0;
      }
      case WM_LBUTTONUP:
      {
        InspectMouseEvent(hWnd, event_, 2, 1, wParam, lParam);
        //nuxDebugMsg("Windows Msg: WM_LBUTTONUP");
        return 0;
      }
      case WM_MBUTTONDOWN:
      {
        InspectMouseEvent(hWnd, event_, 0, 2, wParam, lParam);
        break;
      }
      case WM_MBUTTONDBLCLK:
      {
        InspectMouseEvent(hWnd, event_, 1, 2, wParam, lParam);
        break;
      }
      case WM_MBUTTONUP:
      {
        InspectMouseEvent(hWnd, event_, 2, 2, wParam, lParam);
        break;
      }
      case WM_RBUTTONDOWN:
      {
        InspectMouseEvent(hWnd, event_, 0, 3, wParam, lParam);
        break;
      }
      case WM_RBUTTONDBLCLK:
      {
        InspectMouseEvent(hWnd, event_, 1, 3, wParam, lParam);
        break;
      }
      case WM_RBUTTONUP:
      {
        InspectMouseEvent(hWnd, event_, 2, 3, wParam, lParam);
        break;
      }
      case WM_MOUSEWHEEL:
      {
        InspectMouseEvent(hWnd, event_, 0, 4, wParam, lParam);
        break;
      }

      case WM_NCLBUTTONDBLCLK:
      {
        event_.type = NUX_NC_WINDOW_CONFIGURATION;
        break;
      }
      case WM_NCLBUTTONDOWN:
      {
        event_.type = NUX_NC_WINDOW_CONFIGURATION;
        break;
      }
      case WM_NCLBUTTONUP:
      {
        event_.type = NUX_NC_WINDOW_CONFIGURATION;
        break;
      }
      case WM_NCMBUTTONDBLCLK:
      {
        event_.type = NUX_NC_WINDOW_CONFIGURATION;
        break;
      }
      case WM_NCMBUTTONDOWN:
      {
        event_.type = NUX_NC_WINDOW_CONFIGURATION;
        break;
      }
      case WM_NCMBUTTONUP:
      {
        event_.type = NUX_NC_WINDOW_CONFIGURATION;
        break;
      }
      case WM_NCRBUTTONDBLCLK:
      {
        event_.type = NUX_NC_WINDOW_CONFIGURATION;
        break;
      }
      case WM_NCRBUTTONDOWN:
      {
        event_.type = NUX_NC_WINDOW_CONFIGURATION;
        break;
      }
      case WM_NCRBUTTONUP:
      {
        event_.type = NUX_NC_WINDOW_CONFIGURATION;
        break;
      }

      case WM_MOUSEMOVE:
      {
        InspectMouseEvent(hWnd, event_, 3, 0, wParam, lParam);
        //nuxDebugMsg("Windows Msg: WM_MOUSEMOVE");

        TRACKMOUSEEVENT tme = { sizeof(tme) };
        // Enable NUX_WINDOW_MOUSELEAVE event.
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hWnd;
        TrackMouseEvent(&tme);
        break;
      }

      // Note: there is no WM_MOUSEENTER. WM_MOUSEENTER is equivalent to WM_MOUSEMOVE after a WM_MOUSELEAVE.
      case WM_MOUSELEAVE:
      {
        // All tracking requested by TrackMouseEvent is canceled when this message is generated.
        // The application must call TrackMouseEvent when the mouse reenters its window if
        // it requires further tracking of mouse hover behavior.
        event_.type = NUX_WINDOW_MOUSELEAVE;
        // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process()
        event_.x = 0xFFFFFFFF;
        event_.y = 0xFFFFFFFF;
        //nuxDebugMsg("Windows Msg: WM_MOUSELEAVE/NUX_WINDOW_MOUSELEAVE");
        break;
      }

      case WM_SETCURSOR:

        if ((LOWORD(lParam) == HTCLIENT) && cursor_)
        {
          SetCursor(cursor_);
          return TRUE; //return FALSE;
        }
        else
          break;

      case WM_COMMAND:
      {
        nuxDebugMsg("[GraphicsDisplay::WndProc]: Window \"%s\" received WM_COMMAND message.", window_title_.c_str());
        break;;
      }

      case WM_SYSCOMMAND:                             // Intercept System Commands
      {
        switch(wParam)                         // Check System Calls
        {
          case SC_CLOSE:
            break;
          case SC_CONTEXTHELP:
            break;
          case SC_DEFAULT:
            break;
          case SC_HOTKEY:
            break;
          case SC_HSCROLL:
            break;
          case SC_KEYMENU:
            break;
          case SC_MAXIMIZE:
            break;
          case SC_MINIMIZE:
            break;
          case SC_MONITORPOWER:
            break;
          case SC_MOUSEMENU:
            break;
          case SC_MOVE:
            break;
          case SC_NEXTWINDOW:
            break;
          case SC_PREVWINDOW:
            break;
          case SC_RESTORE:
            break;
          case SC_SCREENSAVE:
            break;
          case SC_SIZE:
            break;
          case SC_TASKLIST:
            break;
          case SC_VSCROLL:
            break;
        }

        break;									// Exit
      }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }

  int GraphicsDisplay::Win32VKToNuxKey(int vk)
  {
    switch(vk)
    {
      case VK_CANCEL:
        return NUX_VK_CANCEL;
      case VK_BACK:
        return NUX_VK_BACKSPACE;
      case VK_TAB:
        return NUX_VK_TAB;
      case VK_CLEAR:
        return NUX_VK_CLEAR;
      case VK_RETURN:
        return NUX_VK_ENTER;
      case VK_SHIFT:
        return NUX_VK_SHIFT;
      
      case VK_CONTROL:
        return NUX_VK_LCONTROL;
      case VK_LCONTROL:
        return NUX_VK_LCONTROL;
      case VK_RCONTROL:
        return NUX_VK_RCONTROL;

      case VK_MENU:
        return NUX_VK_LALT;
      case VK_LMENU:
        return NUX_VK_LALT;
      case VK_RMENU:
        return NUX_VK_RALT;

      case VK_PAUSE:
        return NUX_VK_PAUSE;
      case VK_CAPITAL:
        return NUX_VK_CAPITAL;
      case VK_ESCAPE:
        return NUX_VK_ESCAPE;

      case VK_PRIOR:
        return NUX_VK_PAGE_UP;
      case VK_NEXT:
        return NUX_VK_PAGE_DOWN;
      case VK_END:
        return NUX_VK_END;
      case VK_HOME:
        return NUX_VK_HOME;
      case VK_LEFT:
        return NUX_VK_LEFT;
      case VK_UP:
        return NUX_VK_UP;
      case VK_RIGHT:
        return NUX_VK_RIGHT;
      case VK_DOWN:
        return NUX_VK_DOWN;

      case VK_INSERT:
        return NUX_VK_INSERT;
      case VK_DELETE:
        return NUX_VK_DELETE;

      case 0x30:
        return NUX_VK_0;
      case 0x31:
        return NUX_VK_1;
      case 0x32:
        return NUX_VK_2;
      case 0x33:
        return NUX_VK_3;
      case 0x34:
        return NUX_VK_4;
      case 0x35:
        return NUX_VK_5;
      case 0x36:
        return NUX_VK_6;
      case 0x37:
        return NUX_VK_7;
      case 0x38:
        return NUX_VK_8;
      case 0x39:
        return NUX_VK_9;
      case 0x41:
        return NUX_VK_A;
      case 0x42:
        return NUX_VK_B;
      case 0x43:
        return NUX_VK_C;
      case 0x44:
        return NUX_VK_D;
      case 0x45:
        return NUX_VK_E;
      case 0x46:
        return NUX_VK_F;
      case 0x47:
        return NUX_VK_G;
      case 0x48:
        return NUX_VK_H;
      case 0x49:
        return NUX_VK_I;
      case 0x4A:
        return NUX_VK_J;
      case 0x4B:
        return NUX_VK_K;
      case 0x4C:
        return NUX_VK_L;
      case 0x4D:
        return NUX_VK_M;
      case 0x4E:
        return NUX_VK_N;
      case 0x4F:
        return NUX_VK_O;
      case 0x50:
        return NUX_VK_P;
      case 0x51:
        return NUX_VK_Q;
      case 0x52:
        return NUX_VK_R;
      case 0x53:
        return NUX_VK_S;
      case 0x54:
        return NUX_VK_T;
      case 0x55:
        return NUX_VK_U;
      case 0x56:
        return NUX_VK_V;
      case 0x57:
        return NUX_VK_W;
      case 0x58:
        return NUX_VK_X;
      case 0x59:
        return NUX_VK_Y;
      case 0x5A:
        return NUX_VK_Z;

      case VK_LWIN:
        return NUX_VK_LSUPER; // Windows key left
      case VK_RWIN:
        return NUX_VK_RSUPER; // Windows key right

      case VK_NUMLOCK:
        return NUX_VK_NUMLOCK;
      case VK_SCROLL:
        return NUX_VK_SCROLL;
      case VK_LSHIFT:
        return NUX_VK_LSHIFT;
      case VK_RSHIFT:
        return NUX_VK_RSHIFT;

      case VK_NUMPAD0:
        return NUX_KP_0;
      case VK_NUMPAD1:
        return NUX_KP_1;
      case VK_NUMPAD2:
        return NUX_KP_2;
      case VK_NUMPAD3:
        return NUX_KP_3;
      case VK_NUMPAD4:
        return NUX_KP_4;
      case VK_NUMPAD5:
        return NUX_KP_5;
      case VK_NUMPAD6:
        return NUX_KP_6;
      case VK_NUMPAD7:
        return NUX_KP_7;
      case VK_NUMPAD8:
        return NUX_KP_8;
      case VK_NUMPAD9:
        return NUX_KP_9;

      case VK_MULTIPLY:
        return NUX_KP_MULTIPLY;
      case VK_ADD:
        return NUX_KP_ADD;
      case VK_SEPARATOR:
        return NUX_KP_SEPARATOR;
      case VK_SUBTRACT:
        return NUX_VK_SUBTRACT;
      case VK_DECIMAL:
        return NUX_VK_DECIMAL;
      case VK_DIVIDE:
        return NUX_VK_SLASH;

      case VK_F1:
        return NUX_VK_F1;
      case VK_F2:
        return NUX_VK_F2;
      case VK_F3:
        return NUX_VK_F3;
      case VK_F4:
        return NUX_VK_F4;
      case VK_F5:
        return NUX_VK_F5;
      case VK_F6:
        return NUX_VK_F6;
      case VK_F7:
        return NUX_VK_F7;
      case VK_F8:
        return NUX_VK_F8;
      case VK_F9:
        return NUX_VK_F9;
      case VK_F10:
        return NUX_VK_F10;
      case VK_F11:
        return NUX_VK_F11;
      case VK_F12:
        return NUX_VK_F12;
      case VK_F13:
        return NUX_VK_F13;
      case VK_F14:
        return NUX_VK_F14;
      case VK_F15:
        return NUX_VK_F15;
      case VK_F16:
        return NUX_VK_F16;
      case VK_F17:
        return NUX_VK_F17;
      case VK_F18:
        return NUX_VK_F18;
      case VK_F19:
        return NUX_VK_F19;
      case VK_F20:
        return NUX_VK_F20;
      case VK_F21:
        return NUX_VK_F21;
      case VK_F22:
        return NUX_VK_F22;
      case VK_F23:
        return NUX_VK_F23;
      case VK_F24:
        return NUX_VK_F24;

      default:
        return 0x0;
    }
  }

  int GraphicsDisplay::Win32KeySymToINL(int Keysym)
  {
    return Keysym;

//     switch(Keysym)
//     {
//       case VK_CANCEL:
//         return NUX_VK_CANCEL;
//       case VK_BACK:
//         return NUX_VK_BACKSPACE;
//       case VK_TAB:
//         return NUX_VK_TAB;
//       case VK_CLEAR:
//         return NUX_VK_CLEAR;
//       case VK_RETURN:
//         return NUX_VK_ENTER;
//       case VK_SHIFT:
//         return NUX_VK_SHIFT;
//       case VK_CONTROL:
//         return NUX_VK_CONTROL;
//       case VK_MENU:
//         return NUX_VK_MENU; // ALT key
//       case VK_PAUSE:
//         return NUX_VK_PAUSE;
//       case VK_CAPITAL:
//         return NUX_VK_CAPITAL;
//       case VK_ESCAPE:
//         return NUX_VK_ESCAPE;
//       case VK_SPACE:
//         return NUX_VK_SPACE;
//       case VK_PRIOR:
//         return NUX_VK_PAGE_UP;
//       case VK_NEXT:
//         return NUX_VK_PAGE_DOWN;
//       case VK_END:
//         return NUX_VK_END;
//       case VK_HOME:
//         return NUX_VK_HOME;
//       case VK_LEFT:
//         return NUX_VK_LEFT;
//       case VK_UP:
//         return NUX_VK_UP;
//       case VK_RIGHT:
//         return NUX_VK_RIGHT;
//       case VK_DOWN:
//         return NUX_VK_DOWN;
//       case VK_SELECT:
//         return NUX_VK_SELECT;
//       case VK_PRINT:
//         return NUX_VK_PRINT;
//       case VK_EXECUTE:
//         return NUX_VK_EXECUTE;
//       case VK_INSERT:
//         return NUX_VK_INSERT;
//       case VK_DELETE:
//         return NUX_VK_DELETE;
//       case VK_HELP:
//         return NUX_VK_HELP;
//       case 0x30:
//         return NUX_VK_0;
//       case 0x31:
//         return NUX_VK_1;
//       case 0x32:
//         return NUX_VK_2;
//       case 0x33:
//         return NUX_VK_3;
//       case 0x34:
//         return NUX_VK_4;
//       case 0x35:
//         return NUX_VK_5;
//       case 0x36:
//         return NUX_VK_6;
//       case 0x37:
//         return NUX_VK_7;
//       case 0x38:
//         return NUX_VK_8;
//       case 0x39:
//         return NUX_VK_9;
//       case 0x41:
//         return NUX_VK_A;
//       case 0x42:
//         return NUX_VK_B;
//       case 0x43:
//         return NUX_VK_C;
//       case 0x44:
//         return NUX_VK_D;
//       case 0x45:
//         return NUX_VK_E;
//       case 0x46:
//         return NUX_VK_F;
//       case 0x47:
//         return NUX_VK_G;
//       case 0x48:
//         return NUX_VK_H;
//       case 0x49:
//         return NUX_VK_I;
//       case 0x4A:
//         return NUX_VK_J;
//       case 0x4B:
//         return NUX_VK_K;
//       case 0x4C:
//         return NUX_VK_L;
//       case 0x4D:
//         return NUX_VK_M;
//       case 0x4E:
//         return NUX_VK_N;
//       case 0x4F:
//         return NUX_VK_O;
//       case 0x50:
//         return NUX_VK_P;
//       case 0x51:
//         return NUX_VK_Q;
//       case 0x52:
//         return NUX_VK_R;
//       case 0x53:
//         return NUX_VK_S;
//       case 0x54:
//         return NUX_VK_T;
//       case 0x55:
//         return NUX_VK_U;
//       case 0x56:
//         return NUX_VK_V;
//       case 0x57:
//         return NUX_VK_W;
//       case 0x58:
//         return NUX_VK_X;
//       case 0x59:
//         return NUX_VK_Y;
//       case 0x5A:
//         return NUX_VK_Z;
//       case VK_LWIN:
//         return NUX_VK_LWIN; // Windows key left
//       case VK_RWIN:
//         return NUX_VK_RWIN; // Windows key right
//       case VK_NUMPAD0:
//         return NUX_VK_NUMPAD0;
//       case VK_NUMPAD1:
//         return NUX_VK_NUMPAD1;
//       case VK_NUMPAD2:
//         return NUX_VK_NUMPAD2;
//       case VK_NUMPAD3:
//         return NUX_VK_NUMPAD3;
//       case VK_NUMPAD4:
//         return NUX_VK_NUMPAD4;
//       case VK_NUMPAD5:
//         return NUX_VK_NUMPAD5;
//       case VK_NUMPAD6:
//         return NUX_VK_NUMPAD6;
//       case VK_NUMPAD7:
//         return NUX_VK_NUMPAD7;
//       case VK_NUMPAD8:
//         return NUX_VK_NUMPAD8;
//       case VK_NUMPAD9:
//         return NUX_VK_NUMPAD9;
//       case VK_MULTIPLY:
//         return NUX_VK_MULTIPLY;
//       case VK_ADD:
//         return NUX_VK_ADD;
//       case VK_SEPARATOR:
//         return NUX_VK_SEPARATOR;
//       case VK_SUBTRACT:
//         return NUX_VK_SUBTRACT;
//       case VK_DECIMAL:
//         return NUX_VK_DECIMAL;
//       case VK_DIVIDE:
//         return NUX_VK_DIVIDE;
//       case VK_F1:
//         return NUX_VK_F1;
//       case VK_F2:
//         return NUX_VK_F2;
//       case VK_F3:
//         return NUX_VK_F3;
//       case VK_F4:
//         return NUX_VK_F4;
//       case VK_F5:
//         return NUX_VK_F5;
//       case VK_F6:
//         return NUX_VK_F6;
//       case VK_F7:
//         return NUX_VK_F7;
//       case VK_F8:
//         return NUX_VK_F8;
//       case VK_F9:
//         return NUX_VK_F9;
//       case VK_F10:
//         return NUX_VK_F10;
//       case VK_F11:
//         return NUX_VK_F11;
//       case VK_F12:
//         return NUX_VK_F12;
//       case VK_F13:
//         return NUX_VK_F13;
//       case VK_F14:
//         return NUX_VK_F14;
//       case VK_F15:
//         return NUX_VK_F15;
//       case VK_F16:
//         return NUX_VK_F16;
//       case VK_F17:
//         return NUX_VK_F17;
//       case VK_F18:
//         return NUX_VK_F18;
//       case VK_F19:
//         return NUX_VK_F19;
//       case VK_F20:
//         return NUX_VK_F20;
//       case VK_F21:
//         return NUX_VK_F21;
//       case VK_F22:
//         return NUX_VK_F22;
//       case VK_F23:
//         return NUX_VK_F23;
//       case VK_F24:
//         return NUX_VK_F24;
//       case VK_NUMLOCK:
//         return NUX_VK_NUMLOCK;
//       case VK_SCROLL:
//         return NUX_VK_SCROLL;
//       case VK_LSHIFT:
//         return NUX_VK_LSHIFT;
//       case VK_RSHIFT:
//         return NUX_VK_RSHIFT;
//       case VK_LCONTROL:
//         return NUX_VK_LCONTROL;
//       case VK_RCONTROL:
//         return NUX_VK_RCONTROL;
//       case VK_LMENU:
//         return NUX_VK_LMENU;
//       case VK_RMENU:
//         return NUX_VK_RMENU;
//       default:
//         return 0x0;
//     }
  }

  void GraphicsDisplay::InitGlobalGrabWindow()
  {

  }

  bool GraphicsDisplay::GrabPointer(GrabReleaseCallback callback, void *data, bool replace_existing)
  {
    return false;
  }

  bool GraphicsDisplay::UngrabPointer(void *data)
  {
    return true;
  }

  bool GraphicsDisplay::PointerIsGrabbed()
  {
    return false;  
  }

  bool GraphicsDisplay::GrabKeyboard(GrabReleaseCallback callback, void *data, bool replace_existing)
  {
    return _global_keyboard_grab_active;
  }

  bool GraphicsDisplay::UngrabKeyboard(void *data)
  {
    return true;
  }

  bool GraphicsDisplay::KeyboardIsGrabbed()
  {
    return _global_keyboard_grab_active;  
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::GetDisplayInfo()
  {
    DEVMODE   devmode;
    INT         iMode = 0;
    BOOL		bRetVal;
    DeviceModes dm;
    m_num_gfx_device_modes = 0;

    do
    {
      bRetVal = ::EnumDisplaySettings(NULL, iMode, &devmode);
      iMode++;

      if (bRetVal)
      {
        dm.width        = devmode.dmPelsWidth;
        dm.height       = devmode.dmPelsHeight;
        dm.format       = devmode.dmBitsPerPel;
        dm.refresh_rate = devmode.dmDisplayFrequency;
        m_gfx_device_modes.push_back(dm);
        m_num_gfx_device_modes++;
      }
    }
    while (bRetVal);
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::ShowWindow()
  {
    ::ShowWindow(wnd_handle_, SW_SHOW);
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::HideWindow()
  {
    ::ShowWindow(wnd_handle_, SW_HIDE);
  }

  bool GraphicsDisplay::IsWindowVisible()
  {
    return ::IsWindowVisible(wnd_handle_);
  }
//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::EnterMaximizeWindow()
  {
    ::ShowWindow(wnd_handle_, SW_MAXIMIZE);
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::ExitMaximizeWindow()
  {
    ::ShowWindow(wnd_handle_, SW_RESTORE);
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::SetWindowTitle(const char *Title)
  {
    SetWindowText(wnd_handle_, Title);
  }

//---------------------------------------------------------------------------------------------------------
  // NUXTODO: remove this call. Make a direct access to GpuInfo via GpuDevice.
  bool GraphicsDisplay::HasVSyncSwapControl() const
  {
    return GetGpuDevice()->GetGpuInfo().Support_EXT_Swap_Control();
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::EnableVSyncSwapControl()
  {
    if (HasVSyncSwapControl())
    {
      wglSwapIntervalEXT(1);
    }
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::DisableVSyncSwapControl()
  {
    if (HasVSyncSwapControl())
    {
      wglSwapIntervalEXT(0);
    }
  }

  float GraphicsDisplay::GetFrameTime() const
  {
    return m_FrameTime;
  }

  void GraphicsDisplay::ResetFrameTime()
  {
    m_Timer.Reset();
  }

  bool GraphicsDisplay::StartOpenFileDialog(FileDialogOption &fdo)
  {
    return Win32OpenFileDialog(GetWindowHandle(), fdo);
  }

  bool GraphicsDisplay::StartSaveFileDialog(FileDialogOption &fdo)
  {
    return Win32SaveFileDialog(GetWindowHandle(), fdo);
  }

  bool GraphicsDisplay::StartColorDialog(ColorDialogOption &cdo)
  {
    return Win32ColorDialog(GetWindowHandle(), cdo);
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::SetWindowCursor(HCURSOR cursor)
  {
    cursor_ = cursor;
  }

//---------------------------------------------------------------------------------------------------------
  HCURSOR GraphicsDisplay::GetWindowCursor() const
  {
    return cursor_;
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::PauseThreadGraphicsRendering()
  {
    m_PauseGraphicsRendering = true;
    MakeGLContextCurrent(false);
  }

//---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::IsPauseThreadGraphicsRendering() const
  {
    return m_PauseGraphicsRendering;
  }

}
