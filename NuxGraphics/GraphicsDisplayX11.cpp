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
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GraphicsEngine.h"
#include "GLWindowManager.h"
#include "Events.h"

#include "GraphicsDisplay.h"

#include <X11/extensions/shape.h>

namespace nux
{
  // Compute the frame rate every FRAME_RATE_PERIODE;
  #define FRAME_RATE_PERIODE    10

  EventToNameStruct EventToName[] =
  {
    {NUX_NO_EVENT,               "NUX_NO_EVENT" },
    {NUX_MOUSE_PRESSED,          "NUX_MOUSE_PRESSED" },
    {NUX_MOUSE_RELEASED,         "NUX_MOUSE_RELEASED" },
    {NUX_KEYDOWN,                "NUX_KEYDOWN" },
    {NUX_KEYUP,                  "NUX_KEYUP" },
    {NUX_MOUSE_MOVE,             "NUX_MOUSE_MOVE" },
    {NUX_SIZE_CONFIGURATION,     "NUX_SIZE_CONFIGURATION" },
    {NUX_WINDOW_CONFIGURATION,   "NUX_WINDOW_CONFIGURATION" },
    {NUX_WINDOW_MAP,             "NUX_WINDOW_MAP" },
    {NUX_WINDOW_UNMAP,           "NUX_WINDOW_UNMAP" },
    {NUX_WINDOW_ENTER_FOCUS,     "NUX_WINDOW_ENTER_FOCUS" },
    {NUX_WINDOW_EXIT_FOCUS,      "NUX_WINDOW_EXIT_FOCUS" },
    {NUX_WINDOW_DIRTY,           "NUX_WINDOW_DIRTY" },
    {NUX_WINDOW_MOUSELEAVE,      "NUX_WINDOW_MOUSELEAVE" },
    {NUX_TERMINATE_APP,          "NUX_TERMINATE_APP" },
    {NUX_TAKE_FOCUS,             "NUX_TAKE_FOCUS" }
  };

  GraphicsDisplay::GraphicsDisplay()
  {
    m_CreatedFromForeignWindow      = false;
    m_ParentWindow                  = 0;
    m_GLCtx                         = 0;
    m_Fullscreen                    = false;
    m_GfxInterfaceCreated           = false;
    m_pEvent                        = NULL;
    m_ScreenBitDepth                = 32;
    m_BestMode                      = -1;
    m_num_device_modes              = 0;
    m_DeviceFactory                 = 0;
    m_GraphicsContext               = 0;
    m_Style                         = WINDOWSTYLE_NORMAL;
    m_PauseGraphicsRendering        = false;

    inlSetThreadLocalStorage(_TLS_GraphicsDisplay, this);

    m_X11LastEvent.type = -1;
    m_X11RepeatKey = true;

    m_GfxInterfaceCreated = false;
    m_pEvent = new Event();

    m_WindowSize = Size(0,0);

    _has_glx_13 = false;
    _glx_major = 0;
    _glx_minor = 0;
    
    _dnd_is_drag_source = false;
    _dnd_source_grab_active = false;
    _dnd_source_funcs.get_drag_image = 0;
    _dnd_source_funcs.get_drag_types = 0;
    _dnd_source_funcs.get_data_for_type = 0;
    _dnd_source_funcs.drag_finished = 0;
    
    _global_keyboard_grab_data = 0;
    _global_keyboard_grab_callback = 0;
    _global_pointer_grab_data = 0;
    _global_pointer_grab_callback = 0;

    // DND
    _last_dnd_position = Point(0, 0);
  }

  GraphicsDisplay::~GraphicsDisplay()
  {
    NUX_SAFE_DELETE( m_GraphicsContext );
    NUX_SAFE_DELETE( m_DeviceFactory );

    if (m_CreatedFromForeignWindow == false)
      DestroyOpenGLWindow();

    NUX_SAFE_DELETE( m_pEvent );

    inlSetThreadLocalStorage(_TLS_GraphicsDisplay, 0);
  }

  NString GraphicsDisplay::FindResourceLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    NString path = m_ResourcePathLocation.GetFile(ResourceFileName);

    if (path == "" && ErrorOnFail)
    {
      nuxCriticalMsg("[GraphicsDisplay::FindResourceLocation] Failed to locate resource file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }

  NString GraphicsDisplay::FindUITextureLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_UITextureSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[GraphicsDisplay::FindResourceLocation] Failed to locate ui texture file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }

  NString GraphicsDisplay::FindShaderLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_ShaderSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[GraphicsDisplay::FindResourceLocation] Failed to locate shader file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }

  NString GraphicsDisplay::FindFontLocation(const char *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath(m_FontSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if ((path == "") && ErrorOnFail)
    {
      nuxCriticalMsg("[GraphicsDisplay::FindResourceLocation] Failed to locate font file file: %s.", ResourceFileName);
      return NString("");
    }

    return path;
  }



  bool GraphicsDisplay::IsGfxInterfaceCreated()
  {
    return m_GfxInterfaceCreated;
  }

  static Bool WaitForNotify( Display *dpy, XEvent *event, XPointer arg )
  {
    return(event->type == MapNotify) && (event->xmap.window == (Window) arg);
  }

// TODO: change windowWidth, windowHeight, to window_size;
  static NCriticalSection CreateOpenGLWindow_CriticalSection;
  bool GraphicsDisplay::CreateOpenGLWindow(const char *WindowTitle,
                                         unsigned int WindowWidth,
                                         unsigned int WindowHeight,
                                         WindowStyle Style,
                                         const GraphicsDisplay *Parent,
                                         bool FullscreenFlag,
                                         bool create_rendering_data)
  {
    int xinerama_event, xinerama_error;
    int xinerama_major, xinerama_minor;
    NScopeLock Scope(&CreateOpenGLWindow_CriticalSection);

    m_GfxInterfaceCreated = false;

    // FIXME : put at the end
    Size new_size(WindowWidth, WindowHeight);
    m_ViewportSize = new_size;
    m_WindowSize = new_size;
    // end of fixme

    m_Fullscreen = FullscreenFlag;  // Set The Global Fullscreen Flag
    m_BestMode = -1;                // assume -1 if the mode is not fullscreen

    // Open The display.
    m_X11Display = XOpenDisplay(0);

    if (m_X11Display == 0)
    {
      nuxDebugMsg("[GraphicsDisplay::CreateOpenGLWindow] XOpenDisplay has failed. The window cannot be created.");
      return false;
    }

    m_X11Screen = DefaultScreen(m_X11Display);
    XF86VidModeQueryVersion(m_X11Display, &_x11_major, &_x11_minor);
    XineramaQueryVersion(m_X11Display, &xinerama_major, &xinerama_minor);
    XineramaQueryExtension(m_X11Display, &xinerama_event, &xinerama_error);

    XF86VidModeGetAllModeLines(m_X11Display, m_X11Screen, &m_NumVideoModes, &m_X11VideoModes);
    m_X11OriginalVideoMode = *m_X11VideoModes[0];

    if (m_Fullscreen)               // Attempt Fullscreen Mode?
    {
      // check if resolution is supported
      bool mode_supported = false;

      for (int num_modes = 0 ; num_modes < m_NumVideoModes; num_modes++)
      {
        if ((m_X11VideoModes[num_modes]->hdisplay == m_ViewportSize.width )
          && (m_X11VideoModes[num_modes]->vdisplay == m_ViewportSize.height ))
        {
          mode_supported = true;
          m_BestMode = num_modes;
          break;
        }
      }

      if (mode_supported == false)
      {
        m_Fullscreen = false;
      }
    }

#ifndef NUX_OPENGLES_20
    // Check support for GLX
    int dummy0, dummy1;
    if (!glXQueryExtension(m_X11Display, &dummy0, &dummy1))
    {
      nuxCriticalMsg("[GraphicsDisplay::CreateOpenGLWindow] GLX is not supported.");
      return false;
    }

    // Check GLX version
    glXQueryVersion(m_X11Display, &_glx_major, &_glx_minor);

    // FBConfigs support added in GLX version 1.3
    if (((_glx_major == 1) && (_glx_minor < 3)) || (_glx_major < 1))
    {
      _has_glx_13 = false;
    }
    else
    {
      _has_glx_13 = true;
    }

    _has_glx_13 = false; // force old way. this is temporary...
 
    if (_has_glx_13 == false)
    {
      // Find an OpenGL capable visual.
      static int g_DoubleBufferVisual[] =
      {
        GLX_RGBA,
        GLX_DOUBLEBUFFER,
        GLX_RED_SIZE,       8,
        GLX_GREEN_SIZE,     8,
        GLX_BLUE_SIZE,      8,
        GLX_ALPHA_SIZE,     8,
        GLX_DEPTH_SIZE,     24,
        GLX_STENCIL_SIZE,   8,
        None
      };

      m_X11VisualInfo = glXChooseVisual(m_X11Display, m_X11Screen, g_DoubleBufferVisual);

      if (m_X11VisualInfo == NULL)
      {
        nuxDebugMsg("[GraphicsDisplay::CreateOpenGLWindow] Cannot get appropriate visual.");
        return false;
      }

      // Create OpenGL Context.
      m_GLCtx = glXCreateContext(m_X11Display, m_X11VisualInfo, 0, GL_TRUE);

      m_X11Colormap = XCreateColormap(m_X11Display,
                                       RootWindow(m_X11Display, m_X11VisualInfo->screen),
                                       m_X11VisualInfo->visual,
                                       AllocNone);
    }
    else
    {
        static int DoubleBufferAttributes[] =
        {
          //GLX_X_RENDERABLE, True,
          GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
          GLX_RENDER_TYPE,   GLX_RGBA_BIT,
          //GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
          GLX_DOUBLEBUFFER,  True,
          GLX_RED_SIZE,      8,     /* the maximum number of bits per component    */
          GLX_GREEN_SIZE,    8, 
          GLX_BLUE_SIZE,     8,
          //GLX_ALPHA_SIZE,    8,
          //GLX_DEPTH_SIZE,    24,
          //GLX_STENCIL_SIZE,  8,
          None
        };

        //XSetWindowAttributes  swa;
        GLXFBConfig           *fbconfigs;
        //GLXContext            context;
        //GLXWindow             glxWin;
        int                   fbcount;

        // Request a double buffer configuration
        fbconfigs = glXChooseFBConfig(m_X11Display, DefaultScreen(m_X11Display), DoubleBufferAttributes, &fbcount );

        if (fbconfigs == NULL)
        {
          nuxCriticalMsg("[GraphicsDisplay::CreateOpenGLWindow] glXChooseFBConfig cannot get a supported configuration.");
          return false;
        }

        // Select the best config
        int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
        for (int i = 0; i < fbcount; i++)
        {
          XVisualInfo *vi = glXGetVisualFromFBConfig(m_X11Display, fbconfigs[i]);
          if (vi)
          {
            int samp_buf, samples;
            glXGetFBConfigAttrib(m_X11Display, fbconfigs[i], GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(m_X11Display, fbconfigs[i], GLX_SAMPLES       , &samples);

            nuxDebugMsg("Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d SAMPLES = %d\n", i, vi->visualid, samp_buf, samples);

            if (((best_fbc < 0) || samp_buf) && (samples > best_num_samp))
              best_fbc = i, best_num_samp = samples;
            if ((worst_fbc < 0) || (!samp_buf) || (samples < worst_num_samp))
              worst_fbc = i, worst_num_samp = samples;
          }
          XFree(vi);
        }

        _fb_config = fbconfigs[best_fbc];

        XFree(fbconfigs);

        m_X11VisualInfo = glXGetVisualFromFBConfig(m_X11Display, _fb_config);

        m_X11Colormap = XCreateColormap(m_X11Display, RootWindow(m_X11Display, m_X11VisualInfo->screen),
          m_X11VisualInfo->visual,
          AllocNone);
    }
#else
    EGLDisplay dpy = eglGetDisplay((EGLNativeDisplayType)m_X11Display);
    if (dpy == EGL_NO_DISPLAY)
    {
      nuxDebugMsg("[GraphicsDisplay::CreateOpenGLWindow] Cannot get EGL display.");
      return false;
    }
    EGLint            major, minor;
    if (!eglInitialize(dpy, &major, &minor))
    {
      nuxDebugMsg("[GraphicsDisplay::CreateOpenGLWindow] Cannot initialize EGL.");
      return false;
    }

    eglBindAPI(EGL_OPENGL_ES_API);

    const EGLint config_attribs[] =
    {
      EGL_SURFACE_TYPE,         EGL_WINDOW_BIT,
      EGL_RED_SIZE,             1,
      EGL_GREEN_SIZE,           1,
      EGL_BLUE_SIZE,            1,
      EGL_ALPHA_SIZE,           1,
      EGL_DEPTH_SIZE,           1,
      EGL_RENDERABLE_TYPE,      EGL_OPENGL_ES2_BIT,
      EGL_CONFIG_CAVEAT,        EGL_NONE,
      EGL_NONE,
    };
    EGLConfig         configs[1024];
    EGLint            count;
    if (!eglChooseConfig(dpy, config_attribs, configs, 1024, &count))
    {
      nuxDebugMsg("[GraphicsDisplay::CreateOpenGLWindow] Cannot get EGL config.");
      return false;
    }

    EGLConfig config = configs[0];
    EGLint visualid = 0;
    if (!eglGetConfigAttrib(dpy, config, EGL_NATIVE_VISUAL_ID, &visualid))
    {
      nuxDebugMsg("[GraphicsDisplay::CreateOpenGLWindow] Cannot get native visual ID from EGL config.");
      return false;
    }

    XVisualInfo       visual_info = {0};
    visual_info.visualid = visualid;
    m_X11VisualInfo = XGetVisualInfo(m_X11Display, VisualIDMask, &visual_info, &count);
    if (!m_X11VisualInfo)
    {
      nuxCriticalMsg("[GraphicsDisplay::CreateOpenGLWindow] Cannot get appropriate visual.");
      return false;
    }

    m_X11Colormap = XCreateColormap(m_X11Display,
                                     RootWindow(m_X11Display, m_X11VisualInfo->screen),
                                     m_X11VisualInfo->visual,
                                     AllocNone);
#endif

    m_X11Attr.background_pixmap = 0;
    m_X11Attr.border_pixel      = 0;
    m_X11Attr.colormap          = m_X11Colormap;
    m_X11Attr.override_redirect = m_Fullscreen;
    m_X11Attr.event_mask =
      // Mouse
      /*Button1MotionMask |
      Button2MotionMask |
      Button3MotionMask |
      Button4MotionMask |
      Button5MotionMask |
      ButtonMotionMask |*/
      ButtonPressMask |
      ButtonReleaseMask |
      // Mouse motion
      //-OwnerGrabButtonMask |
      //PointerMotionHintMask |
      PointerMotionMask |
      // Keyboard
      //--KeymapStateMask |
      KeyPressMask    |
      KeyReleaseMask  |
      // Window enter/exit
      LeaveWindowMask |
      EnterWindowMask |
      // Exposure Focus
      ExposureMask |
      FocusChangeMask |
      // Structure notify
      //--ResizeRedirectMask |
      StructureNotifyMask;// |
    //--SubstructureNotifyMask |
    //--SubstructureRedirectMask |
    // Visibility
    //--VisibilityChangeMask |
    // Property
    //--PropertyChangeMask |
    // Colormap
    //--ColormapChangeMask |
    // No event
    //--NoEventMask;


    if (m_Fullscreen)
    {
      XF86VidModeSwitchToMode(m_X11Display, m_X11Screen, m_X11VideoModes[m_BestMode]);
      XF86VidModeSetViewPort(m_X11Display, m_X11Screen, 0, 0);
      //Width = m_X11VideoModes[m_BestMode]->hdisplay;
      //Height = m_X11VideoModes[m_BestMode]->vdisplay;
      XFree(m_X11VideoModes);

      /* create a fullscreen window */

      m_X11Window = XCreateWindow(m_X11Display,
                                   RootWindow(m_X11Display, m_X11VisualInfo->screen),
                                   0, 0,                           // X, Y
                                   m_WindowSize.width, m_WindowSize.height,
                                   0,                              // Border
                                   m_X11VisualInfo->depth,         // Depth
                                   InputOutput,                    // Class
                                   m_X11VisualInfo->visual,        // Visual
                                   CWBorderPixel |
                                   CWColormap |
                                   CWEventMask |
                                   CWOverrideRedirect,
                                   &m_X11Attr);

      XWarpPointer(m_X11Display, None, m_X11Window, 0, 0, 0, 0, 0, 0);
      //XMapRaised(m_X11Display, m_X11Window);
      XGrabKeyboard(m_X11Display, m_X11Window, True,
                     GrabModeAsync,
                     GrabModeAsync,
                     CurrentTime);
      XGrabPointer(m_X11Display, m_X11Window, True,
                    ButtonPressMask,
                    GrabModeAsync, GrabModeAsync, m_X11Window, None, CurrentTime);
    }
    else
    {
      m_X11Window = XCreateWindow(m_X11Display,
                                   RootWindow(m_X11Display, m_X11VisualInfo->screen),
                                   0, 0,
                                   m_WindowSize.width, m_WindowSize.height,
                                   0,
                                   m_X11VisualInfo->depth,
                                   InputOutput,
                                   m_X11VisualInfo->visual,
                                   CWBorderPixel |
                                   CWColormap |
                                   CWEventMask |
                                   CWOverrideRedirect,
                                   &m_X11Attr);

      /* only set window title and handle wm_delete_events if in windowed mode */
      m_WMDeleteWindow = XInternAtom(m_X11Display, "WM_DELETE_WINDOW", True);
      XSetWMProtocols(m_X11Display, m_X11Window, &m_WMDeleteWindow, 1);

      XSetStandardProperties(m_X11Display, m_X11Window, WindowTitle, WindowTitle, None, NULL, 0, NULL);
      //XMapRaised(m_X11Display, m_X11Window);
    }

#ifndef NUX_OPENGLES_20
    if (0 /*_has_glx_13*/)
    {
      XFree(m_X11VisualInfo);
      m_X11VisualInfo = 0;

      /* Create a GLX context for OpenGL rendering */
      m_GLCtx = glXCreateNewContext(m_X11Display, _fb_config, GLX_RGBA_TYPE, NULL, True);

      /* Create a GLX window to associate the frame buffer configuration
      ** with the created X window */
      GLXWindow glxWin = glXCreateWindow(m_X11Display, _fb_config, m_X11Window, NULL );
      
      // Map the window to the screen, and wait for it to appear */
      XMapWindow(m_X11Display, m_X11Window);
      XEvent event;
      XIfEvent(m_X11Display, &event, WaitForNotify, (XPointer) m_X11Window);

      /* Bind the GLX context to the Window */
      glXMakeContextCurrent(m_X11Display, glxWin, glxWin, m_GLCtx);
    }
#else
    m_GLSurface = eglCreateWindowSurface(dpy, config, (EGLNativeWindowType)m_X11Window, 0);
    if (!m_GLSurface)
    {
      nuxCriticalMsg("[GraphicsDisplay::CreateOpenGLWindow] Failed to create surface.");
      return false;
    }

    const EGLint context_attribs[] =
    {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
    };
    m_GLCtx = eglCreateContext(dpy, config, EGL_NO_CONTEXT, context_attribs);
    if (m_GLCtx == EGL_NO_CONTEXT)
    {
      nuxCriticalMsg("[GraphicsDisplay::CreateOpenGLWindow] Failed to create EGL context.");
      return false;
    }
#endif

    MakeGLContextCurrent();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SwapBuffer();

    m_GfxInterfaceCreated = true;

    m_DeviceFactory = new GpuDevice(m_ViewportSize.width, m_ViewportSize.height, BITFMT_R8G8B8A8,
        m_X11Display,
        m_X11Window,
        0,
        _fb_config,
        m_GLCtx,
        1, 0, false);

    m_GraphicsContext = new GraphicsEngine(*this);

    //EnableVSyncSwapControl();
    DisableVSyncSwapControl();
    
    InitGlobalGrabWindow();

    return TRUE;
  }

#ifdef NUX_OPENGLES_20
  bool GraphicsDisplay::CreateFromOpenGLWindow(Display *X11Display, Window X11Window, EGLContext OpenGLContext)
#else
  bool GraphicsDisplay::CreateFromOpenGLWindow(Display *X11Display, Window X11Window, GLXContext OpenGLContext)
#endif
  {
    // Do not make the opengl context current
    // Do not swap the framebuffer
    // Do not clear the depth or color buffer
    // Do not enable/disbale VSync

    m_X11Display = X11Display;
    m_X11Window = X11Window;
    m_GLCtx = OpenGLContext;

    m_X11Screen = DefaultScreen(m_X11Display);

    Window root_return;
    int x_return, y_return;
    unsigned int width_return, height_return;
    unsigned int border_width_return;
    unsigned int depth_return;

    XGetGeometry(X11Display, X11Window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);
    m_WindowSize = Size(width_return, height_return);
    m_WindowPosition = Point(x_return, y_return);

    m_ViewportSize = Size(width_return, height_return);

    m_GfxInterfaceCreated = true;

    // m_DeviceFactory = new GpuDevice(m_ViewportSize.GetWidth(), m_ViewportSize.GetHeight(), BITFMT_R8G8B8A8);
    m_DeviceFactory = new GpuDevice(m_ViewportSize.width, m_ViewportSize.height, BITFMT_R8G8B8A8,
        m_X11Display,
        m_X11Window,
        false,
        _fb_config,
        m_GLCtx,
        1, 0, false);
    m_GraphicsContext = new GraphicsEngine(*this);

    InitGlobalGrabWindow();

    m_CreatedFromForeignWindow = true;

    return true;
  }

// bool GraphicsDisplay::CreateVisual(unsigned int WindowWidth, unsigned int WindowHeight, XVisualInfo& ChosenVisual, XVisualInfo& Template, unsigned long Mask)
// {
//     // Get all the visuals matching the template
//     Template.screen = m_X11Screen;
//     int NunberOfVisuals = 0;
//     XVisualInfo* VisualsArray = XGetVisualInfo(m_X11Display, Mask | VisualScreenMask, &Template, &NunberOfVisuals);
//
//     if (!VisualsArray || (NunberOfVisuals == 0))
//     {
//         if (VisualsArray)
//             XFree(VisualsArray);
//         nuxDebugMsg("[GraphicsDisplay::CreateVisual] There is no matching visuals.");
//         return false;
//     }
//
//     // Find the best visual
//     int          BestScore  = 0xFFFF;
//     XVisualInfo* BestVisual = NULL;
//     while (!BestVisual)
//     {
//         for (int i = 0; i < NunberOfVisuals; ++i)
//         {
//             // Get the current visual attributes
//             int RGBA, DoubleBuffer, Red, Green, Blue, Alpha, Depth, Stencil, MultiSampling, Samples;
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_RGBA,               &RGBA);
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_DOUBLEBUFFER,       &DoubleBuffer);
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_RED_SIZE,           &Red);
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_GREEN_SIZE,         &Green);
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_BLUE_SIZE,          &Blue);
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_ALPHA_SIZE,         &Alpha);
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_DEPTH_SIZE,         &Depth);
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_STENCIL_SIZE,       &Stencil);
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_SAMPLE_BUFFERS_ARB, &MultiSampling);
//             glXGetConfig(ourDisplay, &Visuals[i], GLX_SAMPLES_ARB,        &Samples);
//
//             // First check the mandatory parameters
//             if ((RGBA == 0) || (DoubleBuffer == 0))
//                 continue;
//
//             // Evaluate the current configuration
//             int Color = Red + Green + Blue + Alpha;
//             int Score = EvaluateConfig(Mode, Params, Color, Depth, Stencil, MultiSampling ? Samples : 0);
//
//             // Keep it if it's better than the current best
//             if (Score < BestScore)
//             {
//                 BestScore  = Score;
//                 BestVisual = &Visuals[i];
//             }
//         }
//
//         // If no visual has been found, try a lower level of antialiasing
//         if (!BestVisual)
//         {
//             if (Params.AntialiasingLevel > 2)
//             {
//                 std::cerr << "Failed to find a pixel format supporting "
//                     << Params.AntialiasingLevel << " antialiasing levels ; trying with 2 levels" << std::endl;
//                 Params.AntialiasingLevel = 2;
//             }
//             else if (Params.AntialiasingLevel > 0)
//             {
//                 std::cerr << "Failed to find a pixel format supporting antialiasing ; antialiasing will be disabled" << std::endl;
//                 Params.AntialiasingLevel = 0;
//             }
//             else
//             {
//                 std::cerr << "Failed to find a suitable pixel format for the window -- cannot create OpenGL context" << std::endl;
//                 return false;
//             }
//         }
//     }
//
//     // Create the OpenGL context
//     myGLContext = glXCreateContext(ourDisplay, BestVisual, glXGetCurrentContext(), true);
//     if (myGLContext == NULL)
//     {
//         std::cerr << "Failed to create an OpenGL context for this window" << std::endl;
//         return false;
//     }
//
//     // Update the creation settings from the chosen format
//     int Depth, Stencil;
//     glXGetConfig(ourDisplay, BestVisual, GLX_DEPTH_SIZE,   &Depth);
//     glXGetConfig(ourDisplay, BestVisual, GLX_STENCIL_SIZE, &Stencil);
//     Params.DepthBits   = static_cast<unsigned int>(Depth);
//     Params.StencilBits = static_cast<unsigned int>(Stencil);
//
//     // Assign the chosen visual, and free the temporary visuals array
//     ChosenVisual = *BestVisual;
//     XFree(Visuals);
//
//     // Activate the context
//     SetActive(true);
//
//     // Enable multisampling if needed
//     if (Params.AntialiasingLevel > 0)
//         glEnable(GL_MULTISAMPLE_ARB);
//
//     return true;
// }

  GraphicsEngine* GraphicsDisplay::GetGraphicsEngine() const
  {
    return m_GraphicsContext;
  }

  GpuDevice* GraphicsDisplay::GetGpuDevice() const
  {
    return m_DeviceFactory;
  }

  int GraphicsDisplay::GetGlXMajor() const
  {
    return _glx_major;
  }

  int GraphicsDisplay::GetGlXMinor() const
  {
    return _glx_minor;
  }

  bool GraphicsDisplay::HasFrameBufferSupport()
  {
    return m_DeviceFactory->GetGpuInfo().Support_EXT_Framebuffer_Object();
  }

// TODO(thumper): Size const& GraphicsDisplay::GetWindowSize();
  void GraphicsDisplay::GetWindowSize(int &w, int &h)
  {
    w = m_WindowSize.width;
    h = m_WindowSize.height;
  }

  void GraphicsDisplay::GetDesktopSize(int &w, int &h)
  {
    Window root;
    int x, y;
    unsigned int width, height, depth, border_width;
    bool ret = XGetGeometry(m_X11Display, RootWindow(m_X11Display, m_X11Screen),
                             &root,
                             &x, &y,
                             &width, &height, &border_width, &depth);

    if (ret == false)
    {
      nuxAssert("[GetDesktopSize] Failed to get the desktop size");
      w = 0;
      h = 0;
    }
  }

  void GraphicsDisplay::SetWindowSize(int width, int height)
  {
    nuxDebugMsg("[GraphicsDisplay::SetWindowSize] Setting window size to %dx%d", width, height);
    // Resize window client area
    XResizeWindow(m_X11Display, m_X11Window, width, height);
    XFlush(m_X11Display);
  }

  void GraphicsDisplay::SetWindowPosition(int x, int y)
  {
    nuxDebugMsg("[GraphicsDisplay::SetWindowPosition] Setting window position to %dx%d", x, y);
    // Resize window client area
    XMoveWindow(m_X11Display, m_X11Window, x, y);
    XFlush(m_X11Display);
  }

  int GraphicsDisplay::GetWindowWidth()
  {
    return m_WindowSize.width;
  }

  int GraphicsDisplay::GetWindowHeight()
  {
    return m_WindowSize.height;
  }

  void GraphicsDisplay::SetViewPort(int x, int y, int width, int height)
  {
    if (IsGfxInterfaceCreated())
    {
      //do not rely on m_ViewportSize: glViewport can be called directly
      m_ViewportSize = Size(width, height);
      m_GraphicsContext->SetViewport(x, y, width, height);
      m_GraphicsContext->SetScissor(0, 0, width, height);
    }
  }

  void GraphicsDisplay::ResetWindowSize()
  {
    Window root_return;
    int x_return, y_return;
    unsigned int width_return, height_return;
    unsigned int border_width_return;
    unsigned int depth_return;

    XGetGeometry(m_X11Display,
      m_X11Window,
      &root_return,
      &x_return,
      &y_return,
      &width_return,
      &height_return,
      &border_width_return,
      &depth_return);

    m_WindowSize = Size(width_return, height_return);
    m_WindowPosition = Point(x_return, y_return);
  }

  Point GraphicsDisplay::GetMouseScreenCoord()
  {
    Window root_return;
    Window child_return;
    int root_x_return;
    int root_y_return;
    int win_x_return;
    int win_y_return;
    unsigned int mask_return;


    XQueryPointer(m_X11Display,
                   RootWindow(m_X11Display, m_X11Screen),
                   &root_return,
                   &child_return,
                   &root_x_return,
                   &root_y_return,
                   &win_x_return,
                   &win_y_return,
                   &mask_return);
    XFlush(m_X11Display);

    return Point(root_x_return, root_y_return);
  }

  Point GraphicsDisplay::GetMouseWindowCoord()
  {
    Window root_return;
    Window child_return;
    int root_x_return;
    int root_y_return;
    int win_x_return;
    int win_y_return;
    unsigned int mask_return;

    XQueryPointer(m_X11Display,
                   RootWindow(m_X11Display, m_X11Screen),
                   &root_return,
                   &child_return,
                   &root_x_return,
                   &root_y_return,
                   &win_x_return,
                   &win_y_return,
                   &mask_return);
    XFlush(m_X11Display);

    return Point(win_x_return, win_y_return);
  }

  Point GraphicsDisplay::GetWindowCoord()
  {
    XWindowAttributes attrib;
    int status = XGetWindowAttributes(m_X11Display, m_X11Window, &attrib);

    if (status == 0)
    {
      nuxAssert("[GraphicsDisplay::GetWindowCoord] Failed to get the window attributes.");
      return Point(0, 0);
    }

    return Point(attrib.x, attrib.y);
  }

  Rect GraphicsDisplay::GetWindowGeometry()
  {
    XWindowAttributes attrib;
    int status = XGetWindowAttributes(m_X11Display, m_X11Window, &attrib);

    if (status == 0)
    {
      nuxAssert("[GraphicsDisplay::GetWindowGeometry] Failed to get the window attributes.");
      return Rect(0, 0, 0, 0);
    }

    return Rect(attrib.x, attrib.y, attrib.width, attrib.height);
  }

  Rect GraphicsDisplay::GetNCWindowGeometry()
  {
    XWindowAttributes attrib;
    int status = XGetWindowAttributes(m_X11Display, m_X11Window, &attrib);

    if (status == 0)
    {
      nuxAssert("[GraphicsDisplay::GetWindowGeometry] Failed to get the window attributes.");
      return Rect(0, 0, 0, 0);
    }

    return Rect(attrib.x, attrib.y, attrib.width, attrib.height);
  }

  void GraphicsDisplay::MakeGLContextCurrent()
  {
#ifndef NUX_OPENGLES_20
    if (!glXMakeCurrent(m_X11Display, m_X11Window, m_GLCtx))
    {
      DestroyOpenGLWindow();
    }
#else
    EGLDisplay dpy = eglGetDisplay((EGLNativeDisplayType)m_X11Display);

    if (!eglMakeCurrent(dpy, m_GLSurface, m_GLSurface, m_GLCtx))
    {
      DestroyOpenGLWindow();
    }
#endif
  }

  void GraphicsDisplay::SwapBuffer(bool glswap)
  {
    // There are a lot of mouse motion events coming from X11. The system processes one event at a time and sleeps
    // if necessary to cap the frame rate to 60 frames per seconds. But while the thread sleeping, there are accumulated
    // motion events waiting to be processed. This creates an increasing backlog of motion events. It translate into a slow
    // motion of elements that moves in response to the mouse.
    // Solution: if the the current event is a motion event, changes are, it is followed many more motion events.
    // In this case, don't sleep the thread... Swap the framebuffer to see the result of the current single motion event.
    // It maybe worth investigating how to properly balance event processing and drawing in order to keep the
    // frame rate and the responsiveness at acceptable levels.
    // As a consequence, when the mouse is moving, the frame rate goes beyond 60fps.

    /*bool bsleep = true;
    if (XPending(m_X11Display) > 0)
    {
        XEvent xevent;
        XPeekEvent(m_X11Display, &xevent);
        if (xevent.type == MotionNotify)
        {
            //nuxDebugMsg("[GraphicsDisplay::SwapBuffer]: MotionNotify event.");
            bsleep = false;
        }
    }*/

    if (IsPauseThreadGraphicsRendering())
      return;

    if (glswap)
    {
#ifndef NUX_OPENGLES_20
      glXSwapBuffers(m_X11Display, m_X11Window);
#else
      eglSwapBuffers(eglGetDisplay((EGLNativeDisplayType)m_X11Display), m_GLSurface);
#endif
    }

    m_FrameTime = m_Timer.PassedMilliseconds();

//     if (16.6f - m_FrameTime > 0)
//     {
//         SleepForMilliseconds(16.6f - m_FrameTime);
//         m_FrameTime = m_Timer.PassedMilliseconds();
//     }
//
//     m_Timer.Reset();
//     m_PeriodeTime += m_FrameTime;
//
//     m_FrameCounter++;
//     m_FramePeriodeCounter++;
//     if (m_FramePeriodeCounter >= FRAME_RATE_PERIODE)
//     {
//         //nuxDebugMsg("[GraphicsDisplay::SwapBuffer] Frametime: %f", m_FrameTime);
//         m_FrameRate = m_FramePeriodeCounter / (m_PeriodeTime / 1000.0f);
//         m_PeriodeTime = 0.0f;
//         m_FramePeriodeCounter = 0;
//     }
  }

  void GraphicsDisplay::DestroyOpenGLWindow()
  {
    if (m_GfxInterfaceCreated == true)
    {
      if (m_GLCtx)
      {
#ifndef NUX_OPENGLES_20
        if (!glXMakeCurrent(m_X11Display, None, NULL))
        {
          nuxAssert("[GraphicsDisplay::DestroyOpenGLWindow] glXMakeCurrent failed.");
        }

        glXDestroyContext(m_X11Display, m_GLCtx);
#else
        EGLDisplay dpy = eglGetDisplay((EGLNativeDisplayType)m_X11Display);

        if (!eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT))
        {
          nuxAssert("[GraphicsDisplay::DestroyOpenGLWindow] eglMakeCurrent failed.");
        }

        eglDestroyContext(dpy, m_GLCtx);
        eglDestroySurface(dpy, m_GLSurface);
        eglTerminate(dpy);
        eglReleaseThread();
#endif
        m_GLCtx = NULL;
      }

      /* switch back to original desktop resolution if we were in fs */
      if (m_Fullscreen)
      {
        XF86VidModeSwitchToMode(m_X11Display, m_X11Screen, &m_X11OriginalVideoMode);
        XF86VidModeSetViewPort(m_X11Display, m_X11Screen, 0, 0);
      }

      XCloseDisplay(m_X11Display);
    }

    m_GfxInterfaceCreated = false;
  }

// // convert a MSWindows VK_x to an INL keysym or and extended INL keysym:
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
// static int ms2fltk(int vk, int extended)
// {
//     static unsigned short vklut[256];
//     static unsigned short extendedlut[256];
//     if (!vklut[1])
//     {
//         // init the table
//         unsigned int i;
//         for (i = 0; i < 256; i++)
//         {
//             vklut[i] = i; //tolower(i);
//         }
// //        for (i=VK_F1; i<=VK_F16; i++)
// //        {
// //            vklut[i] = i+(FL_F-(VK_F1-1));   // (FL_F + 1 -> VK_F1) ... (FL_F + 16 -> VK_F16)
// //        }
// //        for (i=VK_NUMPAD0; i<=VK_NUMPAD9; i++)
// //        {
// //            vklut[i] = i+(FL_KP+'0'-VK_NUMPAD0);    // (FL_KP + '0' -> VK_NUMPAD0) ... (FL_KP + '9' = VK_NUMPAD9)
// //        }
//         for (i = 0; i < sizeof(vktab)/sizeof(*vktab); i++)
//         {
//             vklut[vktab[i].vk] = vktab[i].fltk;
//             extendedlut[vktab[i].vk] = vktab[i].extended;
//         }
//         for (i = 0; i < 256; i++)
//         {
//             if (!extendedlut[i])
//                 extendedlut[i] = vklut[i];
//         }
//     }
//
//     return extended ? extendedlut[vk] : vklut[vk];
// }

  static int mouse_move(XEvent xevent, Event *m_pEvent)
  {
//     m_pEvent->e_x = xevent.xmotion.x;
//     m_pEvent->e_y = xevent.xmotion.y;
//     m_pEvent->e_x_root = 0;
//     m_pEvent->e_y_root = 0;

    // Erase mouse event and mouse doubleclick events. Keep the mouse states.
    t_uint32 _mouse_state = m_pEvent->e_mouse_state & 0x0F000000;

    m_pEvent->e_event = NUX_MOUSE_MOVE;

    if (xevent.type == MotionNotify)
    {
      _mouse_state |= (xevent.xmotion.state & Button1Mask) ? NUX_STATE_BUTTON1_DOWN : 0;
      _mouse_state |= (xevent.xmotion.state & Button2Mask) ? NUX_STATE_BUTTON2_DOWN : 0;
      _mouse_state |= (xevent.xmotion.state & Button3Mask) ? NUX_STATE_BUTTON3_DOWN : 0;
    }
    else if (xevent.type == LeaveNotify || xevent.type == EnterNotify)
    {
      _mouse_state |= (xevent.xcrossing.state & Button1Mask) ? NUX_STATE_BUTTON1_DOWN : 0;
      _mouse_state |= (xevent.xcrossing.state & Button2Mask) ? NUX_STATE_BUTTON2_DOWN : 0;
      _mouse_state |= (xevent.xcrossing.state & Button3Mask) ? NUX_STATE_BUTTON3_DOWN : 0;
    }

    m_pEvent->e_mouse_state = _mouse_state;

    return 0;
  }

  static int mouse_press(XEvent xevent, Event *m_pEvent)
  {
//     m_pEvent->e_x = xevent.xbutton.x;
//     m_pEvent->e_y = xevent.xbutton.y;
//     m_pEvent->e_x_root = 0;
//     m_pEvent->e_y_root = 0;

    // Erase mouse event and mouse double-click events. Keep the mouse states.
    ulong _mouse_state = m_pEvent->e_mouse_state & 0x0F000000;

    m_pEvent->e_event = NUX_MOUSE_PRESSED;

    // State of the button before the event
    _mouse_state |= (xevent.xbutton.state & Button1Mask) ? NUX_STATE_BUTTON1_DOWN : 0;
    _mouse_state |= (xevent.xbutton.state & Button2Mask) ? NUX_STATE_BUTTON2_DOWN : 0;
    _mouse_state |= (xevent.xbutton.state & Button3Mask) ? NUX_STATE_BUTTON3_DOWN : 0;

    if (xevent.xbutton.type == ButtonPress)
    {
      if (xevent.xbutton.button == Button1)
      {
        _mouse_state |= NUX_EVENT_BUTTON1_DOWN;
        _mouse_state |= NUX_STATE_BUTTON1_DOWN;
      }

      if (xevent.xbutton.button == Button2)
      {
        _mouse_state |= NUX_EVENT_BUTTON2_DOWN;
        _mouse_state |= NUX_STATE_BUTTON2_DOWN;
      }

      if (xevent.xbutton.button == Button3)
      {
        _mouse_state |= NUX_EVENT_BUTTON3_DOWN;
        _mouse_state |= NUX_STATE_BUTTON3_DOWN;
      }

      if (xevent.xbutton.button == Button4)
      {
        _mouse_state |= NUX_EVENT_MOUSEWHEEL;
        m_pEvent->e_event = NUX_MOUSE_WHEEL;
        m_pEvent->e_wheeldelta = NUX_MOUSEWHEEL_DELTA;
        return 1;
      }

      if (xevent.xbutton.button == Button5)
      {
        _mouse_state |= NUX_EVENT_MOUSEWHEEL;
        m_pEvent->e_event = NUX_MOUSE_WHEEL;
        m_pEvent->e_wheeldelta = -NUX_MOUSEWHEEL_DELTA;
        return 1;
      }

    }

    m_pEvent->e_mouse_state = _mouse_state;

    return 0;
  }

  static int mouse_release(XEvent xevent, Event *m_pEvent)
  {
//     m_pEvent->e_x = xevent.xbutton.x;
//     m_pEvent->e_y = xevent.xbutton.y;
//     m_pEvent->e_x_root = 0;
//     m_pEvent->e_y_root = 0;

    // Erase mouse event and mouse double-click events. Keep the mouse states.
    ulong _mouse_state = m_pEvent->e_mouse_state & 0x0F000000;

    m_pEvent->e_event = NUX_MOUSE_RELEASED;

    // State of the button before the event
    _mouse_state |= (xevent.xbutton.state & Button1Mask) ? NUX_STATE_BUTTON1_DOWN : 0;
    _mouse_state |= (xevent.xbutton.state & Button2Mask) ? NUX_STATE_BUTTON2_DOWN : 0;
    _mouse_state |= (xevent.xbutton.state & Button3Mask) ? NUX_STATE_BUTTON3_DOWN : 0;

    if (xevent.xbutton.type == ButtonRelease)
    {
      if (xevent.xbutton.button == Button1)
      {
        _mouse_state |= NUX_EVENT_BUTTON1_UP;
        _mouse_state &= ~NUX_STATE_BUTTON1_DOWN;
      }

      if (xevent.xbutton.button == Button2)
      {
        _mouse_state |= NUX_EVENT_BUTTON2_UP;
        _mouse_state &= ~NUX_STATE_BUTTON2_DOWN;
      }

      if (xevent.xbutton.button == Button3)
      {
        _mouse_state |= NUX_EVENT_BUTTON3_UP;
        _mouse_state &= ~NUX_STATE_BUTTON3_DOWN;
      }
    }

    m_pEvent->e_mouse_state = _mouse_state;

    return 0;
  }

  unsigned int GetModifierKeyState(unsigned int modifier_key_state)
  {
    unsigned int state = 0;

    // For CapsLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the CapsLock: on(keyboard light is on) or off?
    if (modifier_key_state & LockMask)
      state |= NUX_STATE_CAPS_LOCK;

    // For NumLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the NumLock: on(keyboard light is on) or off?
    if (modifier_key_state & Mod5Mask)
      state |= NUX_STATE_NUMLOCK;

//     if (modifier_key_state & 0x8000)
//         state |= NUX_STATE_SCROLLLOCK;

    if (modifier_key_state & ControlMask)
      state |= NUX_STATE_CTRL;

    if (modifier_key_state & ShiftMask)
      state |= NUX_STATE_SHIFT;

    if (modifier_key_state & Mod1Mask)
      state |= NUX_STATE_ALT;

    return state;
  }

  void GraphicsDisplay::GetSystemEvent(Event *evt)
  {
    m_pEvent->Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    m_pEvent->e_mouse_state &= 0x0F000000;
    bool bProcessEvent = true;

    // Process event matching this window
    XEvent xevent;

    if (XPending(m_X11Display))
    {
      XNextEvent(m_X11Display, &xevent);
      // Detect auto repeat keys. X11 sends a combination of KeyRelease/KeyPress(at the same time) when a key auto repeats.
      // Here, we make sure we process only the keyRelease when the key is effectively released.
      if ((xevent.type == KeyPress) || (xevent.type == KeyRelease))
      {
        if (xevent.xkey.keycode < 256)
        {
          // Detect if a key is repeated
          char Keys[32];
          // The XQueryKeymap function returns a bit vector for the logical state of the keyboard, where each bit set
          // to 1 indicates that the corresponding key is currently pressed down. The vector is represented as 32 bytes.
          // Byte N(from 0) contains the bits for keys 8N to 8N + 7 with the least significant bit in the byte representing
          // key 8N.
          // Note that the logical state of a device(as seen by client applications) may lag the physical state if device
          // event processing is frozen.

          XQueryKeymap(m_X11Display, Keys);

          if (Keys[xevent.xkey.keycode >> 3] & (1 << (xevent.xkey.keycode % 8)))
          {
            // KeyRelease event + KeyDown = discard repeated event
            if (xevent.type == KeyRelease)
            {
              m_X11LastEvent = xevent;
              bProcessEvent = false;
            }

            // KeyPress event + key repeat disabled + matching KeyRelease event = discard repeated event
            if ((xevent.type == KeyPress) && (!m_X11RepeatKey) &&
                 (m_X11LastEvent.xkey.keycode == xevent.xkey.keycode) &&
                 (m_X11LastEvent.xkey.time == xevent.xkey.time))
            {
              bProcessEvent = false;;
            }
          }
        }
      }

      if (xevent.type == MotionNotify)
      {
        while (XCheckTypedEvent(m_X11Display, MotionNotify, &xevent));
      }

      /*if(previous_event_motion == true)
      {
          if (xevent.type == MotionNotify)
          {

              if ((motion_x == xevent.xmotion.x) && (motion_y == xevent.xmotion.y))
              {
                  //printf("skipmotion\n");
                  bProcessEvent = false;
              }
              else
              {
                  motion_x = xevent.xmotion.x;
                  motion_y = xevent.xmotion.y;
              }
          }
          else
          {
              previous_event_motion = false;
          }
      }
      else if (xevent.type == MotionNotify)
      {
          //printf("motion\n");
          previous_event_motion = true;
          motion_x = xevent.xmotion.x;
          motion_y = xevent.xmotion.y;
      }*/

      if (bProcessEvent)
        ProcessXEvent(xevent, false);

      memcpy(evt, m_pEvent, sizeof(Event));

    }
    else
    {
      memcpy(evt, m_pEvent, sizeof(Event));
    }
  }
  
#if defined(NUX_OS_LINUX)
  void GraphicsDisplay::InjectXEvent(Event *evt, XEvent xevent)
  {
    m_pEvent->Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    m_pEvent->e_mouse_state &= 0x0F000000;
    
    // We could do some checks here to make sure the xevent is really what it pretends to be.
    ProcessXEvent(xevent, false);
    memcpy(evt, m_pEvent, sizeof(Event));
  }
#endif

  void GraphicsDisplay::ProcessForeignX11Event(XEvent *xevent, Event *nux_event)
  {
    m_pEvent->Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    m_pEvent->e_mouse_state &= 0x0F000000;
    bool bProcessEvent = true;

    // Process event matching this window
    if (true /*(NUX_REINTERPRET_CAST(XAnyEvent*, xevent))->window == m_X11Window*/)
    {
      // Detect auto repeat keys. X11 sends a combination of KeyRelease/KeyPress(at the same time) when a key auto repeats.
      // Here, we make sure we process only the keyRelease when the key is effectively released.
      if ((xevent->type == KeyPress) || (xevent->type == KeyRelease))
      {
        if (xevent->xkey.keycode < 256)
        {
          // Detect if a key is repeated
          char Keys[32];
          // The XQueryKeymap function returns a bit vector for the logical state of the keyboard, where each bit set
          // to 1 indicates that the corresponding key is currently pressed down. The vector is represented as 32 bytes.
          // Byte N(from 0) contains the bits for keys 8N to 8N + 7 with the least significant bit in the byte representing
          // key 8N.
          // Note that the logical state of a device(as seen by client applications) may lag the physical state if device
          // event processing is frozen.

          XQueryKeymap(xevent->xany.display, Keys);

          if (Keys[xevent->xkey.keycode >> 3] & (1 << (xevent->xkey.keycode % 8)))
          {
            // KeyRelease event + KeyDown = discard repeated event
            if (xevent->type == KeyRelease)
            {
              m_X11LastEvent = *xevent;
              bProcessEvent = false;
            }

            // KeyPress event + key repeat disabled + matching KeyRelease event = discard repeated event
            if ((xevent->type == KeyPress) && (!m_X11RepeatKey) &&
                 (m_X11LastEvent.xkey.keycode == xevent->xkey.keycode) &&
                 (m_X11LastEvent.xkey.time == xevent->xkey.time))
            {
              bProcessEvent = false;;
            }
          }
        }
      }

      if (xevent->type == MotionNotify)
      {
        while (XCheckTypedEvent(m_X11Display, MotionNotify, xevent));
      }

      if (bProcessEvent)
        ProcessXEvent(*xevent, true);

      memcpy(nux_event, m_pEvent, sizeof(Event));
    }
    else
    {
      memcpy(nux_event, m_pEvent, sizeof(Event));
    }
  }

  Event &GraphicsDisplay::GetCurrentEvent()
  {
    return *m_pEvent;
  }

  bool GraphicsDisplay::HasXPendingEvent() const
  {
    return XPending(m_X11Display) ? true : false;
  }
  
  void GraphicsDisplay::RecalcXYPosition(int x_root, int y_root, int &x_recalc, int &y_recalc)
  {
    int main_window_x = m_WindowPosition.x;
    int main_window_y = m_WindowPosition.y;
  
    x_recalc = x_root - main_window_x;
    y_recalc = y_root - main_window_y;
  }

  void GraphicsDisplay::RecalcXYPosition(Window TheMainWindow, XEvent xevent, int &x_recalc, int &y_recalc)
  {
    int main_window_x = m_WindowPosition.x;
    int main_window_y = m_WindowPosition.y;
    bool same = (TheMainWindow == xevent.xany.window);
    
    switch(xevent.type)
    {
      case ButtonPress:
      case ButtonRelease:
      {
        if (same)
        {
          x_recalc = xevent.xbutton.x;
          y_recalc = xevent.xbutton.y;
        }
        else
        {
          x_recalc = xevent.xbutton.x_root - main_window_x;
          y_recalc = xevent.xbutton.y_root - main_window_y;
        }
        break;
      }

      case MotionNotify:
      {
        if (same)
        {
          x_recalc = xevent.xmotion.x;
          y_recalc = xevent.xmotion.y;
        }
        else
        {
          x_recalc = xevent.xmotion.x_root - main_window_x;
          y_recalc = xevent.xmotion.y_root - main_window_y;
        }
        break;
      }

      case LeaveNotify:
      case EnterNotify:
      {
        if (same)
        {
          x_recalc = xevent.xcrossing.x;
          y_recalc = xevent.xcrossing.y;
        }
        else
        {
          x_recalc = xevent.xcrossing.x_root - main_window_x;
          y_recalc = xevent.xcrossing.y_root - main_window_y;
        }
        break;
      }
      
      default:
      {
        x_recalc = y_recalc = 0;
      }
    }
  }

  void GraphicsDisplay::ProcessXEvent(XEvent xevent, bool foreign)
  {
    int x_recalc = 0;
    int y_recalc = 0;

    RecalcXYPosition(m_X11Window, xevent, x_recalc, y_recalc);
    
    bool local_from_server = !foreign;
    foreign = foreign || xevent.xany.window != m_X11Window;

    m_pEvent->e_event = NUX_NO_EVENT;
    m_pEvent->e_x11_window = xevent.xany.window;


    switch(xevent.type)
    {
      case DestroyNotify:
      {
        if (foreign)
          break;
          
        m_pEvent->e_event = NUX_DESTROY_WINDOW;
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: DestroyNotify event.");
        break;
      }

      case Expose:
      {
        if (foreign)
          break;
        
        m_pEvent->e_event = NUX_WINDOW_DIRTY;
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: Expose event.");
        break;
      }


      case ConfigureNotify:
      {
        if (foreign)
          break;
        
        m_pEvent->e_event = NUX_SIZE_CONFIGURATION;
        m_pEvent->width =  xevent.xconfigure.width;
        m_pEvent->height = xevent.xconfigure.height;
        m_WindowSize = Size(xevent.xconfigure.width, xevent.xconfigure.height);
        m_WindowPosition = Point(xevent.xconfigure.x, xevent.xconfigure.y);

        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: ConfigureNotify event.");
        break;
      }

      case FocusIn:
      {
        if (!local_from_server)
          break;
          
        m_pEvent->e_event = NUX_WINDOW_ENTER_FOCUS;
        m_pEvent->e_mouse_state = 0;

        m_pEvent->e_dx = 0;
        m_pEvent->e_dy = 0;
        m_pEvent->virtual_code = 0;
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: FocusIn event.");
        break;
      }

      case FocusOut:
      {
        if (!local_from_server)
          break;
          
        m_pEvent->e_event = NUX_WINDOW_EXIT_FOCUS;
        m_pEvent->e_mouse_state = 0;

        m_pEvent->e_dx = 0;
        m_pEvent->e_dy = 0;
        m_pEvent->virtual_code = 0;
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: FocusOut event.");
        break;
      }

      case KeyPress:
      {
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: KeyPress event.");
        KeyCode keycode = xevent.xkey.keycode;
        KeySym keysym = NoSymbol;
        keysym = XKeycodeToKeysym(xevent.xany.display, keycode, 0);

        m_pEvent->e_key_modifiers = GetModifierKeyState(xevent.xkey.state);
        m_pEvent->e_key_repeat_count = 0;
        m_pEvent->e_keysym = keysym;
        m_pEvent->e_x11_keycode = xevent.xkey.keycode;
        m_pEvent->e_event = NUX_KEYDOWN;
        m_pEvent->e_x11_timestamp = xevent.xkey.time;
        m_pEvent->e_x11_state = xevent.xkey.state;

        char buffer[NUX_EVENT_TEXT_BUFFER_SIZE];
        Memset(m_pEvent->e_text, 0, NUX_EVENT_TEXT_BUFFER_SIZE);

        bool skip = false;
        if ((keysym == NUX_VK_BACKSPACE) ||
            (keysym == NUX_VK_DELETE) ||
            (keysym == NUX_VK_ESCAPE))
        {
          //temporary fix for TextEntry widget: filter some keys
         skip = true; 
        }
        
        int num_char_stored = XLookupString(&xevent.xkey, buffer, NUX_EVENT_TEXT_BUFFER_SIZE, (KeySym*) &m_pEvent->e_keysym, NULL);
        if (num_char_stored && (!skip))
        {
          Memcpy(m_pEvent->e_text, buffer, num_char_stored);
        }

        break;
      }

      case KeyRelease:
      {
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: KeyRelease event.");
        KeyCode keycode = xevent.xkey.keycode;
        KeySym keysym = NoSymbol;
        keysym = XKeycodeToKeysym(xevent.xany.display, keycode, 0);

        m_pEvent->e_key_modifiers = GetModifierKeyState(xevent.xkey.state);
        m_pEvent->e_key_repeat_count = 0;
        m_pEvent->e_keysym = keysym;
        m_pEvent->e_x11_keycode = xevent.xkey.keycode;
        m_pEvent->e_event = NUX_KEYUP;
        m_pEvent->e_x11_timestamp = xevent.xkey.time;
        m_pEvent->e_x11_state = xevent.xkey.state;
        break;
      }

      case ButtonPress:
      {
        if (_dnd_is_drag_source)
        {
          HandleDndDragSourceEvent(xevent);
          break;
        }
        
        m_pEvent->e_x = x_recalc;
        m_pEvent->e_y = y_recalc;
        m_pEvent->e_x_root = 0;
        m_pEvent->e_y_root = 0;
        m_pEvent->e_key_modifiers = GetModifierKeyState(xevent.xkey.state);
        mouse_press(xevent, m_pEvent);
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: ButtonPress event.");
        break;
      }

      case ButtonRelease:
      {
        if (_dnd_is_drag_source)
        {
          HandleDndDragSourceEvent(xevent);
          // fall through on purpose
        }
      
        m_pEvent->e_x = x_recalc;
        m_pEvent->e_y = y_recalc;
        m_pEvent->e_x_root = 0;
        m_pEvent->e_y_root = 0;
        m_pEvent->e_key_modifiers = GetModifierKeyState(xevent.xkey.state);
        mouse_release(xevent, m_pEvent);
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: ButtonRelease event.");
        break;
      }

      case MotionNotify:
      {
        if (_dnd_is_drag_source)
        {
          HandleDndDragSourceEvent(xevent);
          break;
        }
      
        m_pEvent->e_x = x_recalc;
        m_pEvent->e_y = y_recalc;
        m_pEvent->e_x_root = 0;
        m_pEvent->e_y_root = 0;
        m_pEvent->e_key_modifiers = GetModifierKeyState(xevent.xkey.state);
        mouse_move(xevent, m_pEvent);
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: MotionNotify event.");
        break;
      }

      // Note: there is no WM_MOUSEENTER. WM_MOUSEENTER is equivalent to WM_MOUSEMOVE after a WM_MOUSELEAVE.
      case LeaveNotify:
      {
        if (xevent.xcrossing.mode != NotifyNormal || !local_from_server)
          break;
          
        m_pEvent->e_x = -1;
        m_pEvent->e_y = -1;
        m_pEvent->e_x_root = 0;
        m_pEvent->e_y_root = 0;
        m_pEvent->e_key_modifiers = GetModifierKeyState(xevent.xkey.state);
        m_pEvent->e_event = NUX_WINDOW_MOUSELEAVE;
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: LeaveNotify event.");
        break;
      }

      case EnterNotify:
      {
        if (xevent.xcrossing.mode != NotifyNormal || !local_from_server)
          break;
          
        m_pEvent->e_x = x_recalc;
        m_pEvent->e_y = y_recalc;
        m_pEvent->e_x_root = 0;
        m_pEvent->e_y_root = 0;
        m_pEvent->e_key_modifiers = GetModifierKeyState(xevent.xkey.state);
        mouse_move(xevent, m_pEvent);
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: EnterNotify event.");
        break;
      }
      
      case SelectionRequest:
      {
        if (xevent.xselectionrequest.selection == XInternAtom(xevent.xany.display, "XdndSelection", false))
           HandleDndSelectionRequest(xevent);
        break;
      }
      
      case MapNotify:
      {
        if (xevent.xmap.window == _dnd_source_window)
        {
          DrawDndSourceWindow();
        } 
        else
        {
          //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: MapNotify event.");
          m_pEvent->e_event = NUX_WINDOW_MAP;

          XSetInputFocus(xevent.xany.display,
                          xevent.xany.window,
                          RevertToParent,
                          CurrentTime);

        }
        
        break;
      }

      case UnmapNotify:
      {
        //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: UnmapNotify event.");
        m_pEvent->e_event = NUX_WINDOW_UNMAP;
        break;
      }

      case ClientMessage:
      {
        //if(foreign)
        //  break;

        if ((xevent.xclient.format == 32) && ((xevent.xclient.data.l[0]) == static_cast<long> (m_WMDeleteWindow)))
        {
          m_pEvent->e_event = NUX_TERMINATE_APP;
          //nuxDebugMsg("[GraphicsDisplay::ProcessXEvents]: ClientMessage event: Close Application.");
        }
        
        if (xevent.xclient.message_type == XInternAtom(xevent.xany.display, "XdndPosition", false))
        {
          HandleXDndPosition(xevent, m_pEvent);
        }
        else if (xevent.xclient.message_type == XInternAtom(xevent.xany.display, "XdndEnter", false))
        {
          HandleXDndEnter(xevent);
          m_pEvent->e_event = NUX_DND_ENTER_WINDOW;
        }
        else if (xevent.xclient.message_type == XInternAtom(xevent.xany.display, "XdndStatus", false))
        {
          HandleXDndStatus(xevent);
          m_pEvent->e_event = NUX_NO_EVENT;
        }
        else if (xevent.xclient.message_type == XInternAtom(xevent.xany.display, "XdndLeave", false))
        {
          HandleXDndLeave(xevent);
          m_pEvent->e_event = NUX_DND_LEAVE_WINDOW;
        }
        else if (xevent.xclient.message_type == XInternAtom(xevent.xany.display, "XdndDrop", false))
        {
          HandleXDndDrop(xevent, m_pEvent);
        }
        else if (xevent.xclient.message_type == XInternAtom(xevent.xany.display, "XdndFinished", false))
        {
          HandleXDndFinished(xevent);
          m_pEvent->e_event = NUX_NO_EVENT;
        }
        
        break;
      }
    }
  }
  
  void GraphicsDisplay::HandleDndSelectionRequest(XEvent xevent)
  {
    XEvent result;
    
    if (!_dnd_source_funcs.get_data_for_type)
      return;

    result.xselection.type = SelectionNotify;
    result.xselection.display = xevent.xany.display;
    result.xselection.requestor = xevent.xselectionrequest.requestor;
    result.xselection.selection = xevent.xselectionrequest.selection;
    result.xselection.target = xevent.xselectionrequest.target;
    result.xselection.property = xevent.xselectionrequest.property;
    result.xselection.time = xevent.xselectionrequest.time;

    int format, size;
    char *type = XGetAtomName(xevent.xany.display, xevent.xselectionrequest.target);
    const unsigned char *data = (const unsigned char *) (*(_dnd_source_funcs.get_data_for_type)) (type, &size, &format, _dnd_source_data);
    
    XFree(type);
    
    XChangeProperty(xevent.xany.display,  
                     xevent.xselectionrequest.requestor, 
                     xevent.xselectionrequest.property,
                     xevent.xselectionrequest.target, 
                     format, 
                     PropModeReplace, 
                     data,
                     size);
    XSendEvent(xevent.xany.display, xevent.xselectionrequest.requestor, False, 0, &result);
  }
  
  gboolean
  GraphicsDisplay::OnDragEndTimeout(gpointer data)
  {
    static_cast<GraphicsDisplay*> (data)->EndDndDrag(DNDACTION_NONE);
    
    return false;
  }
  
  void GraphicsDisplay::HandleDndDragSourceEvent(XEvent xevent)
  {
    if (_dnd_source_drop_sent)
      return;

    switch(xevent.type)
    {
      case ButtonPress:
        break;

      case ButtonRelease:
      
        if (!_dnd_source_target_window || !_dnd_source_target_accepts_drop)
        {
          SetDndSourceTargetWindow(None);
          EndDndDrag(DNDACTION_NONE);
        }
        else
        {
          SendDndSourceDrop(_dnd_source_target_window, xevent.xbutton.time);
          _dnd_source_drop_sent = true;

          UngrabPointer(this);
          _dnd_source_grab_active = false;

          g_timeout_add(1000, &GraphicsDisplay::OnDragEndTimeout, this);
        }
        break;

      case MotionNotify:
        Window target = GetDndTargetWindowForPos(xevent.xmotion.x_root, xevent.xmotion.y_root);
        
        if (_dnd_source_window)
        {
          Window rw;
          int x, y;
          unsigned int w, h, b, d;
          XGetGeometry(GetX11Display(), _dnd_source_window, &rw, &x, &y, &w, &h, &b, &d);
          XMoveWindow(GetX11Display(), _dnd_source_window, xevent.xmotion.x_root - (w / 2), xevent.xmotion.y_root - (h / 2));
        }
        
        if (target != _dnd_source_target_window)
          SetDndSourceTargetWindow(target);
        
        if (_dnd_source_target_window)
          SendDndSourcePosition(_dnd_source_target_window, xevent.xmotion.x_root, xevent.xmotion.y_root, xevent.xmotion.time);
        
        break;
    }
  }
  
  void GraphicsDisplay::SendDndSourceDrop(Window target, Time time)
  {
    XClientMessageEvent drop_message;
    drop_message.window = target;
    drop_message.format = 32;
    drop_message.type = ClientMessage;

    drop_message.message_type = XInternAtom(GetX11Display(), "XdndDrop", false);
    drop_message.data.l[0] = _dnd_source_window;
    drop_message.data.l[1] = 0;
    drop_message.data.l[2] = time;
    
    XSendEvent(GetX11Display(), target, False, NoEventMask, (XEvent *) &drop_message);
  }
  
  void GraphicsDisplay::SendDndSourcePosition(Window target, int x, int y, Time time)
  {
    XClientMessageEvent position_message;
    position_message.window = target;
    position_message.format = 32;
    position_message.type = ClientMessage;

    position_message.message_type = XInternAtom(GetX11Display(), "XdndPosition", false);
    position_message.data.l[0] = _dnd_source_window;
    position_message.data.l[1] = 0;
    position_message.data.l[2] = (x << 16) + y;
    position_message.data.l[3] = time;
    position_message.data.l[4] = XInternAtom(GetX11Display(), "XdndActionCopy", false); //fixme
    
    XSendEvent(GetX11Display(), target, False, NoEventMask, (XEvent *) &position_message);
  }
  
  void GraphicsDisplay::SendDndSourceEnter(Window target)
  {
    XClientMessageEvent enter_message;
    enter_message.window = target;
    enter_message.format = 32;
    enter_message.type = ClientMessage;

    enter_message.message_type = XInternAtom(GetX11Display(), "XdndEnter", false);
    enter_message.data.l[0] = _dnd_source_window;
    enter_message.data.l[1] = (((unsigned long) xdnd_version) << 24) + 1; // mark that we have set the atom list
    enter_message.data.l[2] = None; // fixme, these should contain the first 3 atoms
    enter_message.data.l[3] = None;
    enter_message.data.l[4] = None;
    
    XSendEvent(GetX11Display(), target, False, NoEventMask, (XEvent *) &enter_message);
  }
  
  void GraphicsDisplay::SendDndSourceLeave(Window target)
  {
    XClientMessageEvent leave_message;
    leave_message.window = target;
    leave_message.format = 32;
    leave_message.type = ClientMessage;

    leave_message.message_type = XInternAtom(GetX11Display(), "XdndLeave", false);
    leave_message.data.l[0] = _dnd_source_window;
    leave_message.data.l[1] = 0; // flags
    
    XSendEvent(GetX11Display(), target, False, NoEventMask, (XEvent *) &leave_message);
  }
  
  void GraphicsDisplay::SetDndSourceTargetWindow(Window target)
  {
    if (target == _dnd_source_target_window || !_dnd_source_grab_active)
      return;
    
    if (_dnd_source_target_window)
      SendDndSourceLeave(_dnd_source_target_window);
    
    if (target)
      SendDndSourceEnter(target);
    
    _dnd_source_target_accepts_drop = false;
    _dnd_source_target_window = target;
  }
  
  // This function hilariously inefficient
  Window GraphicsDisplay::GetDndTargetWindowForPos(int pos_x, int pos_y)
  {
    Window result = 0;
    
    Window root_window = DefaultRootWindow(GetX11Display());
    
    int cur_x, cur_y;
    XTranslateCoordinates(GetX11Display(), root_window, root_window, pos_x, pos_y, &cur_x, &cur_y, &result);
    
    if (!result)
      return result;
      
    Window src = root_window;
    while (true)
    {
      // translate into result space
      Window child;
      int new_x, new_y;
      XTranslateCoordinates(GetX11Display(), src, result, cur_x, cur_y, &new_x, &new_y, &child);
      
      cur_x = new_x;
      cur_y = new_y;
    
      // Check if our current window is XdndAware
      Atom type = 0;
      int format;
      unsigned long n, a;
      unsigned char *data = 0;
      if (XGetWindowProperty(GetX11Display(), result, XInternAtom(GetX11Display(), "XdndAware", false), 0, 1, False,
                             XA_ATOM, &type, &format, &n, &a, &data) == Success) 
      {
        long dnd_version = 0;
        if (data)
        {
          dnd_version = ((Atom *)data)[0];

          if (dnd_version < 5)
            result = 0; // dont have v5? go away until I implement this :)

          XFree(data);
          break; // result is the winner
        }
      }
      
      // Find child window if any and ignore translation
      XTranslateCoordinates(GetX11Display(), result, result, cur_x, cur_y, &new_x, &new_y, &child);
      
      // there is no child window, stop
      if (!child)
      {
        result = 0;
        break;
      }
      
      src = result;
      result = child;
    }
    
    return result;
  }
  
  void GraphicsDisplay::EndDndDrag(DndAction action)
  {
    Display *display = GetX11Display();
    
    if (_dnd_source_funcs.drag_finished)
      (*(_dnd_source_funcs.drag_finished)) (action, _dnd_source_data);
    _dnd_is_drag_source = false;
    
    if (_dnd_source_window)
      XDestroyWindow(display, _dnd_source_window);
    _dnd_source_window = 0;
    
    GrabDndSelection(display, None, CurrentTime);
    UngrabPointer(this);
    _dnd_source_grab_active = false;
    
    _dnd_source_funcs.get_drag_image = 0;
    _dnd_source_funcs.get_drag_types = 0;
    _dnd_source_funcs.get_data_for_type = 0;
    _dnd_source_funcs.drag_finished = 0;
    
    _dnd_source_data = 0;
  }
  
  void GraphicsDisplay::DrawDndSourceWindow()
  {
    if (!_dnd_source_funcs.get_drag_image || !_dnd_source_data || !_dnd_source_window)
      return;
    
    Display *display = GetX11Display();
    NBitmapData *data = (*(_dnd_source_funcs.get_drag_image)) (_dnd_source_data);
    XImage *image;
    
    image = XGetImage(display, _dnd_source_window, 0, 0, data->GetWidth(), data->GetHeight(), AllPlanes, ZPixmap);
    GC gc = XCreateGC(display, _dnd_source_window, 0, NULL);
    
    BitmapFormat format = data->GetFormat();
    
    /* draw some shit */
    if (data->IsTextureData())
    {
      ImageSurface surface = data->GetSurface(0);
      
      int x, y;
      for (y = 0; y < data->GetHeight(); y++)
      {
        for (x = 0; x < data->GetWidth(); x++)
        {
          long pixel = (long) surface.Read(x, y);
	  long a;
	  
	  if (format  == BITFMT_R8G8B8)
	    a = 255;
	  else
	    a = ((pixel >> 24) & 0xff);
          long r = (((pixel >> 16) & 0xff) * a) / 255;
          long g = (((pixel >> 8)  & 0xff) * a) / 255;
          long b = (((pixel >> 0)  & 0xff) * a) / 255;
          
          long result_pixel = (a << 24) | (b << 16) | (g << 8) | (r << 0);
          
          XPutPixel(image, x, y, result_pixel);
        }
      }
    }
    
    /* upload */
    XPutImage(display, _dnd_source_window, gc, image, 0, 0, 0, 0, data->GetWidth(), data->GetHeight());
    
    XDestroyImage(image);
  }
  
  void GraphicsDisplay::StartDndDrag(const DndSourceFuncs &funcs, void *user_data)
  {
    Display *display = GetX11Display();
    
    if (!display || !GrabPointer(NULL, this, true))
    {
      if (funcs.drag_finished)
        (*(funcs.drag_finished)) (DNDACTION_NONE, user_data);
      return;
    }
  
    _dnd_source_funcs = funcs;
    _dnd_source_data = user_data;
    _dnd_source_grab_active = true;
    _dnd_source_drop_sent = false;
    
    int width = 100, height = 100;
    if (_dnd_source_funcs.get_drag_image)
    {
      NBitmapData *data = (*(_dnd_source_funcs.get_drag_image)) (_dnd_source_data);
      width = data->GetWidth();
      height = data->GetHeight();
      
      delete data;
    }
    
    Window root = DefaultRootWindow(display);
    XVisualInfo vinfo;
    if (!XMatchVisualInfo(display, XDefaultScreen(display), 32, TrueColor, &vinfo))
    {
      printf("Could not match visual info\n");
      EndDndDrag(DNDACTION_NONE);
      return;
    }
    
    XSetWindowAttributes attribs;
    attribs.override_redirect = true;
    attribs.background_pixel = 0;
    attribs.border_pixel = 0;
    attribs.colormap = XCreateColormap(display, root, vinfo.visual, AllocNone);
    
    unsigned long attrib_mask = CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWColormap;
    // make a window which will serve two purposes:
    // First this window will be used to display feedback to the user
    // Second this window will grab and own the XdndSelection Selection
    _dnd_source_window = XCreateWindow(display, 
                                        root, 
                                        100, 100, 
                                        width, height, 
                                        0,
                                        vinfo.depth,
                                        InputOutput,
                                        vinfo.visual, 
                                        attrib_mask,
                                        &attribs);
                                        
    XSelectInput(display, _dnd_source_window, StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask);
    XMapRaised(display, _dnd_source_window);
    
    Atom atom_type[1];
    atom_type[0] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_DND", false);
    XChangeProperty(display, _dnd_source_window, XInternAtom(display, "_NET_WM_WINDOW_TYPE", false), 
                     XA_ATOM, 32, PropModeReplace, (unsigned char*) atom_type, 1);

    Atom data[32];
    int     i = 0;
    data[i++] = XInternAtom(display, "_NET_WM_STATE_STICKY", false);
    data[i++] = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", false);
    data[i++] = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", false);
    data[i++] = XInternAtom(display, "_NET_WM_STATE_ABOVE", false);

    XChangeProperty(display, _dnd_source_window, XInternAtom(display, "_NET_WM_STATE", 0),
                 XA_ATOM, 32, PropModeReplace,
                 (unsigned char *) data, i);

    Region region = XCreateRegion();
    if (region)
    {
      XShapeCombineRegion(display, _dnd_source_window, ShapeInput, 0, 0, region, ShapeSet);
      XDestroyRegion(region);
    }
    
    XFlush(display);
    
    _dnd_is_drag_source = true;
    _dnd_source_target_window = 0;
    
    
    std::list<const char *> types = _dnd_source_funcs.get_drag_types(_dnd_source_data);
    std::list<const char *>::iterator it;
    
    Atom type_atoms[types.size()];
    
    i = 0;
    for (it = types.begin(); it != types.end(); it++)
    {
      type_atoms[i] = XInternAtom(display, *it, false);
      i++;
    }
    
    XChangeProperty(display, _dnd_source_window, XInternAtom(display, "XdndTypeList", false),
                    XA_ATOM, 32, PropModeReplace, (unsigned char *)type_atoms, i);
    
    GrabDndSelection(display, _dnd_source_window, CurrentTime);
  }
  
  bool GraphicsDisplay::GrabDndSelection(Display *display, Window window, Time time)
  {
    XSetSelectionOwner(GetX11Display(), XInternAtom(display, "XdndSelection", false), window, time);
    Window owner = XGetSelectionOwner(display, XInternAtom(display, "XdndSelection", false));
    return owner == window;
  }
  
  void GraphicsDisplay::SendDndStatus(bool accept, DndAction action, Rect region)
  {
    if (!_drag_window || !_drag_display || !_drag_source)
      return;
  
    Atom a;
    switch(action)
    {
      case DNDACTION_MOVE:
        a = XInternAtom(_drag_display, "XdndActionMove", false);
        break;
      case DNDACTION_COPY:
        a = XInternAtom(_drag_display, "XdndActionCopy", false);
        break;
      case DNDACTION_PRIVATE:
        a = XInternAtom(_drag_display, "XdndActionPrivate", false);
        break;
      case DNDACTION_LINK:
        a = XInternAtom(_drag_display, "XdndActionLink", false);
        break;
      case DNDACTION_ASK:
        a = XInternAtom(_drag_display, "XdndActionAsk", false);
        break;
      default:
        a = None;
        break;
    }
    SendXDndStatus(_drag_display, _drag_window, _drag_source, accept, a, region);
  }
  
  void GraphicsDisplay::SendDndFinished(bool accepted, DndAction performed_action)
  {
    if (!_drag_window || !_drag_display || !_drag_source)
      return;
    
    Atom a;
    switch(performed_action)
    {
      case DNDACTION_MOVE:
        a = XInternAtom(_drag_display, "XdndActionMove", false);
        break;
      case DNDACTION_COPY:
        a = XInternAtom(_drag_display, "XdndActionCopy", false);
        break;
      case DNDACTION_PRIVATE:
        a = XInternAtom(_drag_display, "XdndActionPrivate", false);
        break;
      case DNDACTION_LINK:
        a = XInternAtom(_drag_display, "XdndActionLink", false);
        break;
      case DNDACTION_ASK:
        a = XInternAtom(_drag_display, "XdndActionAsk", false);
        break;
      default:
        a = None;
        break;
    }
    SendXDndFinished(_drag_display, _drag_window, _drag_source, accepted, a);
  }
  
  std::list<char *> GraphicsDisplay::GetDndMimeTypes()
  {
    std::list<char *> result;
    
    if (!_drag_display)
      return result;
    
    Atom a;
    int i;
    for (i = 0; i <= _xdnd_max_type; i++)
    {
      a = _xdnd_types[i];
      
      if (!a)
        break;
      
      char *name = XGetAtomName(_drag_display, a);
      result.push_back(g_strdup(name));
      XFree(name);
    }
    return result;
  }
  
  char * GraphicsDisplay::GetDndData(char *property)
  {
    if (_dnd_is_drag_source)
    {
      int size, format;
      return g_strdup((*(_dnd_source_funcs.get_data_for_type)) (property, &size, &format, _dnd_source_data));
    }
    else
    {
      Atom a = XInternAtom(_drag_display, property, false);
      return GetXDndData(_drag_display, _drag_window, a, _drag_drop_timestamp);
    }
  }
  
  void GraphicsDisplay::SendXDndStatus(Display *display, Window source, Window target, bool accept, Atom action, Rect box)
  {
    XClientMessageEvent response;
    response.window = target;
    response.format = 32;
    response.type = ClientMessage;

    response.message_type = XInternAtom(display, "XdndStatus", false);
    response.data.l[0] = source;
    response.data.l[1] = 0; // flags
    response.data.l[2] = (box.x << 16) | box.y; // x, y
    response.data.l[3] = (box.width << 16) | box.height; // w, h
    
    if (accept)
    {
      response.data.l[4] = action;
      response.data.l[1] |= 1 << 0;
    }
    else
    {
      response.data.l[4] = None;
    }
    
    XSendEvent(display, target, False, NoEventMask, (XEvent *) &response);
  }
  
  void GraphicsDisplay::HandleXDndPosition(XEvent event, Event* nux_event)
  {
    const unsigned long *l = (const unsigned long *)event.xclient.data.l;
  
    int x = (l[2] & 0xffff0000) >> 16;
    int y = l[2] & 0x0000ffff;
    
    int x_recalc = 0;
    int y_recalc = 0;

    RecalcXYPosition(x, y, x_recalc, y_recalc);

    nux_event->e_event = NUX_DND_MOVE;
    nux_event->e_x = x_recalc;
    nux_event->e_y = y_recalc;

    // Store the last DND position;
    _last_dnd_position = Point(x_recalc, y_recalc);
  }
  
  void GraphicsDisplay::HandleXDndEnter(XEvent event)
  {
    const long *l = event.xclient.data.l;
    int version = (int)(((unsigned long)(l[1])) >> 24);
    
    if (version > xdnd_version)
      return;
    
    _drag_source = l[0];
    _drag_window = event.xany.window;
    _drag_display = event.xany.display;
    
    int j = 0;
    if (l[1] & 1) 
    {
      unsigned char *retval = 0;
      unsigned long n, a;
      int f;
      Atom type = None;

      XGetWindowProperty(_drag_display, _drag_source, XInternAtom(_drag_display, "XdndTypeList", false), 0,
                         _xdnd_max_type, False, XA_ATOM, &type, &f, &n, &a, &retval);

      if (retval) 
      {
        Atom *data = (Atom *)retval;
        for (; j < _xdnd_max_type && j < (int)n; j++)
          _xdnd_types[j] = data[j];
        
        XFree((uchar*)data);
      }
    } 
    else 
    {
      // xdnd supports up to 3 types without using XdndTypelist
      int i;
      for (i = 2; i < 5; i++) 
        _xdnd_types[j++] = l[i];
    }
    
    _xdnd_types[j] = 0;
  }
  
  void GraphicsDisplay::HandleXDndStatus(XEvent event)
  {
    const unsigned long *l = (const unsigned long *)event.xclient.data.l;
    
    // should protect against stray messages
    if (l[1] & 1)
      _dnd_source_target_accepts_drop = true;
    else
      _dnd_source_target_accepts_drop = false;
  }
  
  void GraphicsDisplay::HandleXDndLeave(XEvent event)
  {
    // reset the key things
    _xdnd_types[0] = 0;
    _drag_source = 0;
    _drag_window = 0;
    _drag_drop_timestamp = 0;
  }
  
  bool GraphicsDisplay::GetXDndSelectionEvent(Display *display, Window target, Atom property, long time, XEvent *result, int attempts)
  {
    // request the selection
    XConvertSelection(display,
                       XInternAtom(display, "XdndSelection", false),
                       property,
                       XInternAtom(display, "XdndSelection", false),
                       target,
                       time);
    XFlush(display);
    
    int i;
    for (i = 0; i < attempts; i++)
    {
      if (XCheckTypedWindowEvent(display, target, SelectionNotify, result))
      {
        return true;
      }
      
      XFlush(display);
      
      struct timeval usleep_tv;
      usleep_tv.tv_sec = 0;
      usleep_tv.tv_usec = 50000;
      select(0, 0, 0, 0, &usleep_tv);
    }
    
    return false;
  }
  
  void GraphicsDisplay::SendXDndFinished(Display *display, Window source, Window target, bool result, Atom action)
  {
    XClientMessageEvent response;
    response.window = target;
    response.format = 32;
    response.type = ClientMessage;

    response.message_type = XInternAtom(display, "XdndFinished", false);
    response.data.l[0] = source;
    response.data.l[1] = result ? 1 : 0; // flags
    response.data.l[2] = action; // action
    
    XSendEvent(display, target, False, NoEventMask, (XEvent *) &response);
  }
  
  char * GraphicsDisplay::GetXDndData(Display *display, Window requestor, Atom property, long time)
  {
    char *result = 0;
    XEvent xevent;
    if (GetXDndSelectionEvent(display, requestor, property, time, &xevent, 50))
    {
      unsigned char *buffer = NULL;
      Atom type;

      unsigned long  bytes_left; // bytes_after
      unsigned long  length;     // nitems
      int   format;
      
      if (XGetWindowProperty(display, 
                             requestor, 
                             XInternAtom(display, "XdndSelection", false), 
                             0, 
                             10000,
                             False,
                             AnyPropertyType, 
                             &type, 
                             &format, 
                             &length, 
                             &bytes_left, 
                             &buffer) == Success)
      {
        result = g_strdup((char *) buffer);
        XFree(buffer);
      }
    }
    
    return result;
  }
  
  void GraphicsDisplay::HandleXDndDrop(XEvent event, Event *nux_event)
  {
    const long *l = event.xclient.data.l;
    _drag_drop_timestamp = l[2];
    
    nux_event->e_event = NUX_DND_DROP;

    // The drop does not provide(x, y) coordinates of the location of the drop. Use the last DND position.
    nux_event->e_x = _last_dnd_position.x;
    nux_event->e_y = _last_dnd_position.y;
  }
  
  void GraphicsDisplay::HandleXDndFinished(XEvent event)
  {
    const unsigned long *l = (const unsigned long *)event.xclient.data.l;
    
    if (l[0] != _dnd_source_target_window)
      return;
    
    bool accepted = l[1] & 1;
    DndAction result = DNDACTION_NONE;

    if (accepted)
    {
      if (l[2] == XInternAtom(GetX11Display(), "XdndActionCopy", false))
        result = DNDACTION_COPY;
      else if (l[2] == XInternAtom(GetX11Display(), "XdndActionAsk", false))
        result = DNDACTION_ASK;
      else if (l[2] == XInternAtom(GetX11Display(), "XdndActionLink", false))
        result = DNDACTION_LINK;
      else if (l[2] == XInternAtom(GetX11Display(), "XdndActionMove", false))
        result = DNDACTION_MOVE;
      else if (l[2] == XInternAtom(GetX11Display(), "XdndActionPrivate", false))
        result = DNDACTION_PRIVATE;  
    }
    
    EndDndDrag(result);
  }
  
  void GraphicsDisplay::InitGlobalGrabWindow()
  {
    Display *display = GetX11Display();

    XSetWindowAttributes attribs;
    attribs.override_redirect = True;
    _global_grab_window = XCreateWindow(display,
                                         DefaultRootWindow(display),
                                         -100, -100,                     // X, Y
                                         1, 1,                           // Width, Height
                                         0,                              // Border
                                         0,                              // Depth
                                         InputOnly,                      // Class
                                         CopyFromParent,                 // Visual
                                         CWOverrideRedirect,
                                         &attribs);
    
    XSelectInput(display, _global_grab_window, StructureNotifyMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask);
    XMapRaised(display, _global_grab_window);
    
    Atom atom_type[1];
    atom_type[0] = XInternAtom(display, "_NET_WM_WINDOW_TYPE_UTILITY", false);
    XChangeProperty(display, _global_grab_window, XInternAtom(display, "_NET_WM_WINDOW_TYPE", false), 
                     XA_ATOM, 32, PropModeReplace, (unsigned char*) atom_type, 1);

    Atom data[32];
    int     i = 0;
    data[i++] = XInternAtom(display, "_NET_WM_STATE_STICKY", false);
    data[i++] = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", false);
    data[i++] = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", false);
    data[i++] = XInternAtom(display, "_NET_WM_STATE_ABOVE", false);

    XChangeProperty(display, _global_grab_window, XInternAtom(display, "_NET_WM_STATE", 0),
                 XA_ATOM, 32, PropModeReplace,
                 (unsigned char *) data, i);
  }

  bool GraphicsDisplay::GrabPointer(GrabReleaseCallback callback, void *data, bool replace_existing)
  {
    if (_global_pointer_grab_active)
    {
      if (!replace_existing || _dnd_source_grab_active) // prevent grabbing over DND grabs
        return false;
      
      if (_global_pointer_grab_callback)
        (*_global_pointer_grab_callback) (true, _global_pointer_grab_data);
    }
    
    if (!_global_pointer_grab_active)
    {
      int result = XGrabPointer(GetX11Display(), 
                                _global_grab_window, 
                                True, 
                                   ButtonPressMask | 
                                   ButtonReleaseMask | 
                                   PointerMotionMask | 
                                   ButtonMotionMask , 
                                GrabModeAsync,
                                GrabModeAsync, 
                                None,
                                None, 
                                CurrentTime);
                                      
      if (result == GrabSuccess)
        _global_pointer_grab_active = true;
    }
    
    if (_global_pointer_grab_active)
    {
      _global_pointer_grab_callback = callback;
      _global_pointer_grab_data = data;
    }
    
    return _global_pointer_grab_active;
  }
  
  bool GraphicsDisplay::UngrabPointer(void *data)
  {
    if (data != _global_pointer_grab_data || !_global_pointer_grab_active)
      return false;
    
    _global_pointer_grab_active = false;
    XUngrabPointer(GetX11Display(), CurrentTime);
    
    if (_global_pointer_grab_callback)
      (*_global_pointer_grab_callback) (false, data);
    
    _global_pointer_grab_data = false;
    _global_pointer_grab_callback = 0;
    
    return true;
  }
  
  bool GraphicsDisplay::PointerIsGrabbed()
  {
    return _global_pointer_grab_active;  
  }

  bool GraphicsDisplay::GrabKeyboard(GrabReleaseCallback callback, void *data, bool replace_existing)
  {
    if (_global_keyboard_grab_active)
    {
      if (!replace_existing)
        return false; // fail case
      
      if (_global_keyboard_grab_callback)
        (*_global_keyboard_grab_callback) (true, _global_keyboard_grab_data);
    }
    
    if (!_global_keyboard_grab_active)
    {
      int result = XGrabKeyboard(GetX11Display(), 
                                _global_grab_window, 
                                True, 
                                GrabModeAsync,
                                GrabModeAsync, 
                                CurrentTime);
                                      
      if (result == GrabSuccess)
        _global_keyboard_grab_active = true;
    }
    
    if (_global_keyboard_grab_active)
    {
      _global_keyboard_grab_callback = callback;
      _global_keyboard_grab_data = data;
    }
    
    return _global_keyboard_grab_active;
  }
  
  bool GraphicsDisplay::UngrabKeyboard(void *data)
  {
    if (data != _global_keyboard_grab_data || !_global_keyboard_grab_active)
      return false;
    
    _global_keyboard_grab_active = false;
    XUngrabKeyboard(GetX11Display(), CurrentTime);
    
    if (_global_keyboard_grab_callback)
      (*_global_keyboard_grab_callback) (false, data);
    
    _global_keyboard_grab_data = false;
    _global_keyboard_grab_callback = 0;
    
    return true;
  }
  
  bool GraphicsDisplay::KeyboardIsGrabbed()
  {
    return _global_keyboard_grab_active;  
  }

  void GraphicsDisplay::ShowWindow()
  {
    XMapRaised(m_X11Display, m_X11Window);
  }

  void GraphicsDisplay::HideWindow()
  {
    XUnmapWindow(m_X11Display, m_X11Window);
  }

  bool GraphicsDisplay::IsWindowVisible()
  {
    XWindowAttributes window_attributes_return;
    XGetWindowAttributes(m_X11Display, m_X11Window, &window_attributes_return);

    if (window_attributes_return.map_state == IsViewable)
    {
      return true;
    }
    return false;
  }

  void GraphicsDisplay::EnterMaximizeWindow()
  {

  }

  void GraphicsDisplay::ExitMaximizeWindow()
  {

  }

  void GraphicsDisplay::SetWindowTitle(const char *Title)
  {
    XStoreName(m_X11Display, m_X11Window, TCHAR_TO_ANSI(Title));
  }

  bool GraphicsDisplay::HasVSyncSwapControl() const
  {
    return GetGpuDevice()->GetGpuInfo().Support_EXT_Swap_Control();
  }

  void GraphicsDisplay::EnableVSyncSwapControl()
  {
#ifndef NUX_OPENGLES_20
    if (GetGpuDevice()->GetGpuInfo().Support_EXT_Swap_Control())
    {
      GLXDrawable drawable = glXGetCurrentDrawable();
      glXSwapIntervalEXT(m_X11Display, drawable, 1);
    }
#endif
  }

  void GraphicsDisplay::DisableVSyncSwapControl()
  {
#ifndef NUX_OPENGLES_20
    if (GetGpuDevice()->GetGpuInfo().Support_EXT_Swap_Control())
    {
      GLXDrawable drawable = glXGetCurrentDrawable();
      glXSwapIntervalEXT(m_X11Display, drawable, 0);
    }
#endif
  }

  float GraphicsDisplay::GetFrameTime() const
  {
    return m_FrameTime;
  }

  void GraphicsDisplay::ResetFrameTime()
  {
    m_Timer.Reset();
  }

  /*
  bool GraphicsDisplay::StartOpenFileDialog(FileDialogOption& fdo)
  {
      return Win32OpenFileDialog(GetWindowHandle(), fdo);
  }

  bool GraphicsDisplay::StartSaveFileDialog(FileDialogOption& fdo)
  {
      return Win32SaveFileDialog(GetWindowHandle(), fdo);
  }

  bool GraphicsDisplay::StartColorDialog(ColorDialogOption& cdo)
  {
      return Win32ColorDialog(GetWindowHandle(), cdo);
  }
  */
  /*void GraphicsDisplay::SetWindowCursor(HCURSOR cursor)
  {
      m_Cursor = cursor;
  }

  HCURSOR GraphicsDisplay::GetWindowCursor() const
  {
      return m_Cursor;
  }*/

  void GraphicsDisplay::PauseThreadGraphicsRendering()
  {
    m_PauseGraphicsRendering = true;
    MakeGLContextCurrent();
  }

  bool GraphicsDisplay::IsPauseThreadGraphicsRendering() const
  {
    return m_PauseGraphicsRendering;
  }

}


