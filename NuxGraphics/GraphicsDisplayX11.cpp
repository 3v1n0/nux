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


#include "GLResource.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GraphicsEngine.h"
#include "GLWindowManager.h"
#include "Events.h"
#include "IniFile.h"

#include "GraphicsDisplay.h"

namespace nux
{

  unsigned int gVirtualKeycodeState[NUX_MAX_VK];

  // Compute the frame rate every FRAME_RATE_PERIODE;
#define FRAME_RATE_PERIODE    10

#define NUX_MISSING_GL_EXTENSION_MESSAGE_BOX(message) {MessageBox(NULL, TEXT("Missing extension: " #message), TEXT("ERROR"), MB_OK|MB_ICONERROR); exit(-1);}
#define NUX_ERROR_EXIT_MESSAGE(message) inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, #message " The program will exit.")); exit(-1);

  EventToNameStruct EventToName[] =
  {
    {NUX_NO_EVENT,               TEXT ("NUX_NO_EVENT") },
    {NUX_MOUSE_PRESSED,          TEXT ("NUX_MOUSE_PRESSED") },
    {NUX_MOUSE_RELEASED,         TEXT ("NUX_MOUSE_RELEASED") },
    {NUX_KEYDOWN,                TEXT ("NUX_KEYDOWN") },
    {NUX_KEYUP,                  TEXT ("NUX_KEYUP") },
    {NUX_MOUSE_MOVE,             TEXT ("NUX_MOUSE_MOVE") },
    {NUX_SIZE_CONFIGURATION,     TEXT ("NUX_SIZE_CONFIGURATION") },
    {NUX_WINDOW_CONFIGURATION,   TEXT ("NUX_WINDOW_CONFIGURATION") },
    {NUX_WINDOW_ENTER_FOCUS,     TEXT ("NUX_WINDOW_ENTER_FOCUS") },
    {NUX_WINDOW_EXIT_FOCUS,      TEXT ("NUX_WINDOW_EXIT_FOCUS") },
    {NUX_WINDOW_DIRTY,           TEXT ("NUX_WINDOW_DIRTY") },
    {NUX_WINDOW_MOUSELEAVE,      TEXT ("NUX_WINDOW_MOUSELEAVE") },
    {NUX_TERMINATE_APP,          TEXT ("NUX_TERMINATE_APP") }
  };

//---------------------------------------------------------------------------------------------------------

  GraphicsDisplay::GraphicsDisplay()
  {
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

    inlSetThreadLocalStorage (ThreadLocal_GLWindowImpl, this);

    m_X11LastEvent.type = -1;
    m_X11RepeatKey = true;

    m_GfxInterfaceCreated = false;
    m_pEvent = new IEvent();

    m_WindowSize.SetWidth (0);
    m_WindowSize.SetHeight (0);

    // A window never starts in a minimized state.
    m_is_window_minimized = false;
  }

//---------------------------------------------------------------------------------------------------------
  GraphicsDisplay::~GraphicsDisplay()
  {
    NUX_SAFE_DELETE ( m_GraphicsContext );
    NUX_SAFE_DELETE ( m_DeviceFactory );

    DestroyOpenGLWindow();
    NUX_SAFE_DELETE ( m_pEvent );

    inlSetThreadLocalStorage (ThreadLocal_GLWindowImpl, 0);
  }

//---------------------------------------------------------------------------------------------------------
  NString GraphicsDisplay::FindResourceLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    NString path = m_ResourcePathLocation.GetFile (ResourceFileName);

    if (path == TEXT ("") && ErrorOnFail)
    {
      nuxDebugMsg (TEXT ("[GraphicsDisplay::FindResourceLocation] Failed to locate resource file: %s."), ResourceFileName);
      /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal,
          TEXT("Failed to locate resource file %s.\nThe program will exit."), ResourceFileName);*/
      exit (1);
    }

    return path;
  }

  NString GraphicsDisplay::FindUITextureLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath (m_UITextureSearchPath);
    NString path = searchpath.GetFile (ResourceFileName);

    if ( (path == TEXT ("") ) && ErrorOnFail)
    {
      nuxDebugMsg (TEXT ("[GraphicsDisplay::FindResourceLocation] Failed to locate ui texture file: %s."), ResourceFileName);
      /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal,
          TEXT("Failed to locate ui texture file %s.\nThe program will exit."), ResourceFileName);*/
      exit (1);
    }

    return path;
  }

  NString GraphicsDisplay::FindShaderLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath (m_ShaderSearchPath);
    NString path = searchpath.GetFile (ResourceFileName);

    if ( (path == TEXT ("") ) && ErrorOnFail)
    {
      nuxDebugMsg (TEXT ("[GraphicsDisplay::FindResourceLocation] Failed to locate shader file: %s."), ResourceFileName);
      /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal,
          TEXT("Failed to locate shader file %s.\nThe program will exit."), ResourceFileName);*/
      exit (1);
    }

    return path;
  }

  NString GraphicsDisplay::FindFontLocation (const TCHAR *ResourceFileName, bool ErrorOnFail)
  {
    FilePath searchpath;
    searchpath.AddSearchPath (m_FontSearchPath);
    NString path = searchpath.GetFile (ResourceFileName);

    if ( (path == TEXT ("") ) && ErrorOnFail)
    {
      nuxDebugMsg (TEXT ("[GraphicsDisplay::FindResourceLocation] Failed to locate font file file: %s."), ResourceFileName);
      /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal,
          TEXT("Failed to locate font file %s.\nThe program will exit."), ResourceFileName);*/
      exit (1);
    }

    return path;
  }


//---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::IsGfxInterfaceCreated()
  {
    return m_GfxInterfaceCreated;
  }

//---------------------------------------------------------------------------------------------------------
  static NCriticalSection CreateOpenGLWindow_CriticalSection;
  bool GraphicsDisplay::CreateOpenGLWindow (const TCHAR *WindowTitle,
                                         unsigned int WindowWidth,
                                         unsigned int WindowHeight,
                                         WindowStyle Style,
                                         const GraphicsDisplay *Parent,
                                         bool FullscreenFlag)
  {
    NScopeLock Scope (&CreateOpenGLWindow_CriticalSection);

    m_GfxInterfaceCreated = false;

    // FIXME : put at the end
    m_ViewportSize.SetWidth (WindowWidth);
    m_ViewportSize.SetHeight (WindowHeight);
    m_WindowSize.SetWidth (WindowWidth);
    m_WindowSize.SetHeight (WindowHeight);
    // end of fixme

    m_Fullscreen = FullscreenFlag;  // Set The Global Fullscreen Flag
    m_BestMode = -1;                // assume -1 if the mode is not fullscreen

    // Open The display.
    m_X11Display = XOpenDisplay (0);

    if (m_X11Display == 0)
    {
      nuxDebugMsg (TEXT ("[GraphicsDisplay::CreateOpenGLWindow] XOpenDisplay has failed. The window cannot be created.") );
      return false;
    }

    // Check support for GLX
    int dummy0, dummy1;
    if (!glXQueryExtension(m_X11Display, &dummy0, &dummy1))
    {
      nuxCriticalMsg (TEXT ("[GraphicsDisplay::CreateOpenGLWindow] GLX is not supported."));
      exit (-1);
    }

    // Check GLX version
    glXQueryVersion (m_X11Display, &m_GLXVerMajor, &m_GLXVerMinor);


    m_X11Screen = DefaultScreen (m_X11Display);
    XF86VidModeQueryVersion (m_X11Display, &m_X11VerMajor, &m_X11VerMinor);

    XF86VidModeGetAllModeLines (m_X11Display, m_X11Screen, &m_NumVideoModes, &m_X11VideoModes);
    m_X11OriginalVideoMode = *m_X11VideoModes[0];

    if (m_Fullscreen)               // Attempt Fullscreen Mode?
    {
      // check if resolution is supported
      bool mode_supported = false;

      for (int num_modes = 0 ; num_modes < m_NumVideoModes; num_modes++)
      {
        if ( (m_X11VideoModes[num_modes]->hdisplay == m_ViewportSize.GetWidth() )
             && (m_X11VideoModes[num_modes]->vdisplay == m_ViewportSize.GetHeight() ) )
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
    m_X11VisualInfo = glXChooseVisual (m_X11Display, m_X11Screen, g_DoubleBufferVisual);

    if (m_X11VisualInfo == NULL)
    {
      nuxDebugMsg (TEXT ("[GraphicsDisplay::CreateOpenGLWindow] Cannot get appropriate visual.") );
      return false;
    }

    // Create OpenGL Context.
    m_GLCtx = glXCreateContext (m_X11Display, m_X11VisualInfo, 0, GL_TRUE);

    m_X11Colormap = XCreateColormap (m_X11Display,
                                     RootWindow (m_X11Display, m_X11VisualInfo->screen),
                                     m_X11VisualInfo->visual,
                                     AllocNone);

//     {
//       int DoubleBufferAttributes[] =
//       {
//         GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
//         GLX_RENDER_TYPE,   GLX_RGBA_BIT,
//         GLX_DOUBLEBUFFER,  True,
//         GLX_RED_SIZE,      8,     /* the maximum number of bits per component    */
//         GLX_GREEN_SIZE,    8, 
//         GLX_BLUE_SIZE,     8,
//         GLX_ALPHA_SIZE,    8,
//         GLX_DEPTH_SIZE,    24,
//         GLX_STENCIL_SIZE,  8,
//         None
//       };
// 
//       XSetWindowAttributes  swa;
//       GLXFBConfig           *fbConfigs;
//       GLXContext            context;
//       GLXWindow             glxWin;
//       int                   numReturned;
// 
//       /* Request a suitable framebuffer configuration - try for a double 
//       ** buffered configuration first */
//       fbConfigs = glXChooseFBConfig (m_X11Display, DefaultScreen(m_X11Display), DoubleBufferAttributes, &numReturned );
// 
//       /* Create an X colormap and window with a visual matching the first
//       ** returned framebuffer config */
//       m_X11VisualInfo = glXGetVisualFromFBConfig (m_X11Display, fbConfigs[0]);
// 
//       m_X11Colormap = XCreateColormap (m_X11Display, RootWindow (m_X11Display, m_X11VisualInfo->screen),
//         m_X11VisualInfo->visual,
//         AllocNone);
// 
//     }

    m_X11Attr.border_pixel = 0;
    m_X11Attr.colormap = m_X11Colormap;
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
      XF86VidModeSwitchToMode (m_X11Display, m_X11Screen, m_X11VideoModes[m_BestMode]);
      XF86VidModeSetViewPort (m_X11Display, m_X11Screen, 0, 0);
      //Width = m_X11VideoModes[m_BestMode]->hdisplay;
      //Height = m_X11VideoModes[m_BestMode]->vdisplay;
      XFree (m_X11VideoModes);

      /* create a fullscreen window */

      m_X11Window = XCreateWindow (m_X11Display,
                                   RootWindow (m_X11Display, m_X11VisualInfo->screen),
                                   0, 0,                           // X, Y
                                   m_WindowSize.GetWidth(), m_WindowSize.GetHeight(),
                                   0,                              // Border
                                   m_X11VisualInfo->depth,         // Depth
                                   InputOutput,                    // Class
                                   m_X11VisualInfo->visual,        // Visual
                                   CWBorderPixel |
                                   CWColormap |
                                   CWEventMask |
                                   CWOverrideRedirect,
                                   &m_X11Attr);

      XWarpPointer (m_X11Display, None, m_X11Window, 0, 0, 0, 0, 0, 0);
      XMapRaised (m_X11Display, m_X11Window);
      XGrabKeyboard (m_X11Display, m_X11Window, True,
                     GrabModeAsync,
                     GrabModeAsync,
                     CurrentTime);
      XGrabPointer (m_X11Display, m_X11Window, True,
                    ButtonPressMask,
                    GrabModeAsync, GrabModeAsync, m_X11Window, None, CurrentTime);
    }
    else
    {
      m_X11Window = XCreateWindow (m_X11Display,
                                   RootWindow (m_X11Display, m_X11VisualInfo->screen),
                                   0, 0,
                                   m_WindowSize.GetWidth(), m_WindowSize.GetHeight(),
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
      m_WMDeleteWindow = XInternAtom (m_X11Display, "WM_DELETE_WINDOW", True);
      XSetWMProtocols (m_X11Display, m_X11Window, &m_WMDeleteWindow, 1);

      XSetStandardProperties (m_X11Display, m_X11Window, WindowTitle, WindowTitle, None, NULL, 0, NULL);
      XMapRaised (m_X11Display, m_X11Window);
    }

//     {
//       /* Create a GLX context for OpenGL rendering */
//       GLXContext context = glXCreateNewContext (m_X11Display, fbConfigs[0], GLX_RGBA_TYPE, NULL, True);
// 
//       /* Create a GLX window to associate the frame buffer configuration
//       ** with the created X window */
//       GLXWindow = glXCreateWindow (m_X11Display, fbConfigs[0], m_X11Window, NULL );
//       
//       /* Map the window to the screen, and wait for it to appear */
//       XMapWindow( m_X11Display, m_X11Window );
//       XIfEvent( m_X11Display, &event, WaitForNotify, (XPointer) m_X11Window );
// 
//       /* Bind the GLX context to the Window */
//       glXMakeContextCurrent( dpy, glxWin, glxWin, context );
//     }

    MakeGLContextCurrent();
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SwapBuffer();

    m_GfxInterfaceCreated = true;

    m_DeviceFactory = new GpuDevice (m_ViewportSize.GetWidth(), m_ViewportSize.GetHeight(), BITFMT_R8G8B8A8);
    m_GraphicsContext = new GraphicsEngine (*this);

    EnableVSyncSwapControl();
    //DisableVSyncSwapControl();

    return TRUE;
  }

  bool GraphicsDisplay::CreateFromOpenGLWindow (Display *X11Display, Window X11Window, GLXContext OpenGLContext)
  {
    // Do not make the opengl context current
    // Do not swap the framebuffer
    // Do not clear the depth or color buffer
    // Do not enable/disbale VSync

    m_X11Display = X11Display;
    m_X11Window = X11Window;
    m_GLCtx = OpenGLContext;

    Window root_return;
    int x_return, y_return;
    unsigned int width_return, height_return;
    unsigned int border_width_return;
    unsigned int depth_return;

    XGetGeometry (X11Display, X11Window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);
    m_WindowSize = Size (width_return, height_return);
    m_WindowPosition = Point (x_return, y_return);

    m_ViewportSize = Size (width_return, height_return);

    m_GfxInterfaceCreated = true;

    m_DeviceFactory = new GpuDevice (m_ViewportSize.GetWidth(), m_ViewportSize.GetHeight(), BITFMT_R8G8B8A8);
    m_GraphicsContext = new GraphicsEngine (*this);

    return true;
  }

// bool GraphicsDisplay::CreateVisual(unsigned int WindowWidth, unsigned int WindowHeight, XVisualInfo& ChosenVisual, XVisualInfo& Template, unsigned long Mask)
// {
//     // Get all the visuals matching the template
//     Template.screen = m_X11Screen;
//     int NunberOfVisuals = 0;
//     XVisualInfo* VisualsArray = XGetVisualInfo(m_X11Display, Mask | VisualScreenMask, &Template, &NunberOfVisuals);
//
//     if(!VisualsArray || (NunberOfVisuals == 0))
//     {
//         if(VisualsArray)
//             XFree(VisualsArray);
//         nuxDebugMsg(TEXT("[GraphicsDisplay::CreateVisual] There is no matching visuals."));
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

//---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::HasFrameBufferSupport()
  {
    return m_DeviceFactory->GetGpuInfo().Support_EXT_Framebuffer_Object ();
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::GetWindowSize (int &w, int &h)
  {
    w = m_WindowSize.GetWidth();
    h = m_WindowSize.GetHeight();
  }

  void GraphicsDisplay::GetDesktopSize (int &w, int &h)
  {
    Window root;
    int x, y;
    unsigned int width, height, depth, border_width;
    bool ret = XGetGeometry (m_X11Display, RootWindow (m_X11Display, m_X11Screen),
                             &root,
                             &x, &y,
                             &width, &height, &border_width, &depth);

    if (ret == false)
    {
      nuxAssert (TEXT ("[GetDesktopSize] Failed to get the desktop size") );
      w = 0;
      h = 0;
    }
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::SetWindowSize (int width, int height)
  {
    nuxDebugMsg (TEXT ("[GraphicsDisplay::SetWindowSize] Setting window size to %dx%d"), width, height);
    // Resize window client area
    XResizeWindow (m_X11Display, m_X11Window, width, height);
    XFlush (m_X11Display);
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::SetWindowPosition (int x, int y)
  {
    nuxDebugMsg (TEXT ("[GraphicsDisplay::SetWindowPosition] Setting window position to %dx%d"), x, y);
    // Resize window client area
    XMoveWindow (m_X11Display, m_X11Window, x, y);
    XFlush (m_X11Display);
  }

//---------------------------------------------------------------------------------------------------------
  unsigned int GraphicsDisplay::GetWindowWidth()
  {
    return m_WindowSize.GetWidth();
  }

//---------------------------------------------------------------------------------------------------------
  unsigned int GraphicsDisplay::GetWindowHeight()
  {
    return m_WindowSize.GetHeight();
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::SetViewPort (int x, int y, int width, int height)
  {

    if (IsGfxInterfaceCreated() )
    {
      //do not rely on m_ViewportSize: glViewport can be called directly
      m_ViewportSize.SetWidth (width);
      m_ViewportSize.SetHeight (height);

      m_GraphicsContext->SetViewport (x, y, m_ViewportSize.GetWidth(), m_ViewportSize.GetHeight() );
      m_GraphicsContext->SetScissor (0, 0, width, height);
    }
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


    XQueryPointer (m_X11Display,
                   RootWindow (m_X11Display, m_X11Screen),
                   &root_return,
                   &child_return,
                   &root_x_return,
                   &root_y_return,
                   &win_x_return,
                   &win_y_return,
                   &mask_return);
    XFlush (m_X11Display);

    return Point (root_x_return, root_y_return);
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

    XQueryPointer (m_X11Display,
                   RootWindow (m_X11Display, m_X11Screen),
                   &root_return,
                   &child_return,
                   &root_x_return,
                   &root_y_return,
                   &win_x_return,
                   &win_y_return,
                   &mask_return);
    XFlush (m_X11Display);

    return Point (win_x_return, win_y_return);
  }

  Point GraphicsDisplay::GetWindowCoord()
  {
    XWindowAttributes attrib;
    int status = XGetWindowAttributes (m_X11Display, m_X11Window, &attrib);

    if (status == 0)
    {
      nuxAssert (TEXT ("[GraphicsDisplay::GetWindowCoord] Failed to get the window attributes.") );
      return Point (0, 0);
    }

    return Point (attrib.x, attrib.y);
  }

  Rect GraphicsDisplay::GetWindowGeometry()
  {
    XWindowAttributes attrib;
    int status = XGetWindowAttributes (m_X11Display, m_X11Window, &attrib);

    if (status == 0)
    {
      nuxAssert (TEXT ("[GraphicsDisplay::GetWindowGeometry] Failed to get the window attributes.") );
      return Rect (0, 0, 0, 0);
    }

    return Rect (attrib.x, attrib.y, attrib.width, attrib.height);
  }

  Rect GraphicsDisplay::GetNCWindowGeometry()
  {
    XWindowAttributes attrib;
    int status = XGetWindowAttributes (m_X11Display, m_X11Window, &attrib);

    if (status == 0)
    {
      nuxAssert (TEXT ("[GraphicsDisplay::GetWindowGeometry] Failed to get the window attributes.") );
      return Rect (0, 0, 0, 0);
    }

    return Rect (attrib.x, attrib.y, attrib.width, attrib.height);
  }

  void GraphicsDisplay::MakeGLContextCurrent()
  {
    if (!glXMakeCurrent (m_X11Display, m_X11Window, m_GLCtx) )
    {
      DestroyOpenGLWindow();
    }
  }

  void GraphicsDisplay::SwapBuffer (bool glswap)
  {
    // There are a lot of mouse motion events coming from X11. The system processes one event at a time and sleeps
    // if necessary to cap the frame rate to 60 frames per seconds. But while the thread sleeping, there are accumulated
    // motion events waiting to be processed. This creates an increasing backlog of motion events. It translate into a slow
    // motion of elements that moves in response to the mouse.
    // Solution: if the the current event is a motion event, changes are, it is followed many more motion events.
    // In this case, don't sleep the thread... Swap the framebuffer to see the result of the current single motion event.
    // It maybe worth investigating how to properly balance event processing and drawing in order to keep the
    // frame rate and the responsiveness at acceptable levels.
    // As a consequence, when the mouse is moving, the frame rate goes beyong 60fps.

    /*bool bsleep = true;
    if(XPending(m_X11Display) > 0)
    {
        XEvent xevent;
        XPeekEvent(m_X11Display, &xevent);
        if(xevent.type == MotionNotify)
        {
            //nuxDebugMsg(TEXT("[GraphicsDisplay::SwapBuffer]: MotionNotify event."));
            bsleep = false;
        }
    }*/

    if (IsPauseThreadGraphicsRendering() )
      return;

    if (glswap)
    {
      glXSwapBuffers (m_X11Display, m_X11Window);
    }

    m_FrameTime = m_Timer.PassedMilliseconds();

//     if(16.6f - m_FrameTime > 0)
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
//     if(m_FramePeriodeCounter >= FRAME_RATE_PERIODE)
//     {
//         //nuxDebugMsg(TEXT("[GraphicsDisplay::SwapBuffer] Frametime: %f"), m_FrameTime);
//         m_FrameRate = m_FramePeriodeCounter / (m_PeriodeTime / 1000.0f);
//         m_PeriodeTime = 0.0f;
//         m_FramePeriodeCounter = 0;
//     }
  }
//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::DestroyOpenGLWindow()
  {
    if (m_GfxInterfaceCreated == true)
    {
      if (m_GLCtx)
      {
        if (!glXMakeCurrent (m_X11Display, None, NULL) )
        {
          nuxAssert (TEXT ("[GraphicsDisplay::DestroyOpenGLWindow] glXMakeCurrent failed.") );
        }

        glXDestroyContext (m_X11Display, m_GLCtx);
        m_GLCtx = NULL;
      }

      /* switch back to original desktop resolution if we were in fs */
      if (m_Fullscreen)
      {
        XF86VidModeSwitchToMode (m_X11Display, m_X11Screen, &m_X11OriginalVideoMode);
        XF86VidModeSetViewPort (m_X11Display, m_X11Screen, 0, 0);
      }

      XCloseDisplay (m_X11Display);
    }

    m_GfxInterfaceCreated = false;
  }

// //---------------------------------------------------------------------------------------------------------
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
// //---------------------------------------------------------------------------------------------------------
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
//---------------------------------------------------------------------------------------------------------
  static int mouse_move (XEvent xevent, IEvent *m_pEvent)
  {
//     m_pEvent->e_x = xevent.xmotion.x;
//     m_pEvent->e_y = xevent.xmotion.y;
//     m_pEvent->e_x_root = 0;
//     m_pEvent->e_y_root = 0;

    // Erase mouse event and mouse doubleclick events. Keep the mouse states.
    t_uint32 _mouse_state = m_pEvent->e_mouse_state & 0x0F000000;

    m_pEvent->e_event = NUX_MOUSE_MOVE;

    _mouse_state |= (xevent.xmotion.state & Button1Mask) ? NUX_STATE_BUTTON1_DOWN : 0;
    _mouse_state |= (xevent.xmotion.state & Button2Mask) ? NUX_STATE_BUTTON2_DOWN : 0;
    _mouse_state |= (xevent.xmotion.state & Button3Mask) ? NUX_STATE_BUTTON3_DOWN : 0;

    m_pEvent->e_mouse_state = _mouse_state;

    return 0;
  }

  static int mouse_press (XEvent xevent, IEvent *m_pEvent)
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
        m_pEvent->e_event = NUX_MOUSEWHEEL;
        m_pEvent->e_wheeldelta = -NUX_MOUSEWHEEL_DELTA;
        return 1;
      }

      if (xevent.xbutton.button == Button5)
      {
        _mouse_state |= NUX_EVENT_MOUSEWHEEL;
        m_pEvent->e_event = NUX_MOUSEWHEEL;
        m_pEvent->e_wheeldelta = +NUX_MOUSEWHEEL_DELTA;
        return 1;
      }

    }

    m_pEvent->e_mouse_state = _mouse_state;

    return 0;
  }

  static int mouse_release (XEvent xevent, IEvent *m_pEvent)
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

  unsigned int GetModifierKeyState (unsigned int modifier_key_state)
  {
    unsigned int state = 0;

    // For CapsLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the CapsLock: on (keyboard light is on) or off?
    if (modifier_key_state & LockMask)
      state |= NUX_STATE_CAPS_LOCK;

    // For NumLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the NumLock: on (keyboard light is on) or off?
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

  void GraphicsDisplay::GetSystemEvent (IEvent *evt)
  {
    m_pEvent->Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    m_pEvent->e_mouse_state &= 0x0F000000;
    bool bProcessEvent = true;

    // Process event matching this window
    XEvent xevent;

    if (XPending (m_X11Display))
    {
      XNextEvent (m_X11Display, &xevent);
      // Detect auto repeat keys. X11 sends a combination of KeyRelease/KeyPress (at the same time) when a key auto repeats.
      // Here, we make sure we process only the keyRelease when the key is effectively released.
      if ( (xevent.type == KeyPress) || (xevent.type == KeyRelease) )
      {
        if (xevent.xkey.keycode < 256)
        {
          // Detect if a key is repeated
          char Keys[32];
          // The XQueryKeymap function returns a bit vector for the logical state of the keyboard, where each bit set
          // to 1 indicates that the corresponding key is currently pressed down. The vector is represented as 32 bytes.
          // Byte N (from 0) contains the bits for keys 8N to 8N + 7 with the least significant bit in the byte representing
          // key 8N.
          // Note that the logical state of a device (as seen by client applications) may lag the physical state if device
          // event processing is frozen.

          XQueryKeymap (m_X11Display, Keys);

          if (Keys[xevent.xkey.keycode >> 3] & (1 << (xevent.xkey.keycode % 8) ) )
          {
            // KeyRelease event + KeyDown = discard repeated event
            if (xevent.type == KeyRelease)
            {
              m_X11LastEvent = xevent;
              bProcessEvent = false;
            }

            // KeyPress event + key repeat disabled + matching KeyRelease event = discard repeated event
            if ( (xevent.type == KeyPress) && (!m_X11RepeatKey) &&
                 (m_X11LastEvent.xkey.keycode == xevent.xkey.keycode) &&
                 (m_X11LastEvent.xkey.time == xevent.xkey.time) )
            {
              bProcessEvent = false;;
            }
          }
        }
      }

      if (xevent.type == MotionNotify)
      {
        while (XCheckTypedEvent (m_X11Display, MotionNotify, &xevent) );
      }

      /*if(previous_event_motion == true)
      {
          if(xevent.type == MotionNotify)
          {

              if((motion_x == xevent.xmotion.x) && (motion_y == xevent.xmotion.y))
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
      else if(xevent.type == MotionNotify)
      {
          //printf("motion\n");
          previous_event_motion = true;
          motion_x = xevent.xmotion.x;
          motion_y = xevent.xmotion.y;
      }*/

      if (bProcessEvent)
        ProcessXEvent (xevent, false);

      memcpy (evt, m_pEvent, sizeof (IEvent) );

    }
    else
    {
      memcpy (evt, m_pEvent, sizeof (IEvent) );
    }
  }
  
#if defined (NUX_OS_LINUX)
  void GraphicsDisplay::InjectXEvent (IEvent *evt, XEvent xevent)
  {
    m_pEvent->Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    m_pEvent->e_mouse_state &= 0x0F000000;
    
    // We could do some checks here to make sure the xevent is really what it pretends to be.
    ProcessXEvent (xevent, false);
    memcpy (evt, m_pEvent, sizeof (IEvent));
  }
#endif

  void GraphicsDisplay::ProcessForeignX11Event (XEvent *xevent, IEvent *nux_event)
  {
    m_pEvent->Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    m_pEvent->e_mouse_state &= 0x0F000000;
    bool bProcessEvent = true;

    // Process event matching this window
    if (true /*(NUX_REINTERPRET_CAST(XAnyEvent*, xevent))->window == m_X11Window*/)
    {
      // Detect auto repeat keys. X11 sends a combination of KeyRelease/KeyPress (at the same time) when a key auto repeats.
      // Here, we make sure we process only the keyRelease when the key is effectively released.
      if ( (xevent->type == KeyPress) || (xevent->type == KeyRelease) )
      {
        if (xevent->xkey.keycode < 256)
        {
          // Detect if a key is repeated
          char Keys[32];
          // The XQueryKeymap function returns a bit vector for the logical state of the keyboard, where each bit set
          // to 1 indicates that the corresponding key is currently pressed down. The vector is represented as 32 bytes.
          // Byte N (from 0) contains the bits for keys 8N to 8N + 7 with the least significant bit in the byte representing
          // key 8N.
          // Note that the logical state of a device (as seen by client applications) may lag the physical state if device
          // event processing is frozen.

          XQueryKeymap (m_X11Display, Keys);

          if (Keys[xevent->xkey.keycode >> 3] & (1 << (xevent->xkey.keycode % 8) ) )
          {
            // KeyRelease event + KeyDown = discard repeated event
            if (xevent->type == KeyRelease)
            {
              m_X11LastEvent = *xevent;
              bProcessEvent = false;
            }

            // KeyPress event + key repeat disabled + matching KeyRelease event = discard repeated event
            if ( (xevent->type == KeyPress) && (!m_X11RepeatKey) &&
                 (m_X11LastEvent.xkey.keycode == xevent->xkey.keycode) &&
                 (m_X11LastEvent.xkey.time == xevent->xkey.time) )
            {
              bProcessEvent = false;;
            }
          }
        }
      }

      if (xevent->type == MotionNotify)
      {
        while (XCheckTypedEvent (m_X11Display, MotionNotify, xevent) );
      }

      if (bProcessEvent)
        ProcessXEvent (*xevent, true);

      memcpy (nux_event, m_pEvent, sizeof (IEvent) );
    }
    else
    {
      memcpy (nux_event, m_pEvent, sizeof (IEvent) );
    }
  }

  IEvent &GraphicsDisplay::GetCurrentEvent()
  {
    return *m_pEvent;
  }

  bool GraphicsDisplay::HasXPendingEvent() const
  {
    return XPending (m_X11Display) ? true : false;
  }

  void GraphicsDisplay::RecalcXYPosition (Window TheMainWindow, XEvent xevent, int &x_recalc, int &y_recalc)
  {
    int main_window_x = m_WindowPosition.x;
    int main_window_y = m_WindowPosition.y;
    bool same = (TheMainWindow == xevent.xany.window);
    
    switch (xevent.type)
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

  void GraphicsDisplay::ProcessXEvent (XEvent xevent, bool foreign)
  {
    int x_recalc = 0;
    int y_recalc = 0;
    
    RecalcXYPosition (m_X11Window, xevent, x_recalc, y_recalc);
    
    foreign = foreign || xevent.xany.window != m_X11Window;

    m_pEvent->e_event = NUX_NO_EVENT;

    switch (xevent.type)
    {
      case DestroyNotify:
      {
        if (foreign)
          break;
          
        m_pEvent->e_event = NUX_DESTROY_WINDOW;
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: DestroyNotify event."));
        break;
      }

      case Expose:
      {
        if (foreign)
          break;
        
        m_pEvent->e_event = NUX_WINDOW_DIRTY;
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: Expose event."));
        break;
      }


      case ConfigureNotify:
      {
        if (foreign)
          break;
        
        m_pEvent->e_event = NUX_SIZE_CONFIGURATION;
        m_pEvent->width =  xevent.xconfigure.width;
        m_pEvent->height = xevent.xconfigure.height;
        m_WindowSize.SetWidth (xevent.xconfigure.width);
        m_WindowSize.SetHeight (xevent.xconfigure.height);
        m_WindowPosition.Set (xevent.xconfigure.x, xevent.xconfigure.y);

        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: ConfigureNotify event."));
        break;
      }

      case FocusIn:
      {
        if (foreign)
          break;
        
        m_pEvent->e_event = NUX_WINDOW_ENTER_FOCUS;
        m_pEvent->e_mouse_state = 0;

        // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process().
        // Because WM_SETFOCUS can happen with the mouse outside of the client area, we set e_x and e_y so that the mouse will be
        // outside of all widgets. A subsequent mouse down or mouse move event will set the correct values for e_x and e_y.
        m_pEvent->e_x = 0xFFFFFFFF;
        m_pEvent->e_y = 0xFFFFFFFF;
        m_pEvent->e_dx = 0;
        m_pEvent->e_dy = 0;
        m_pEvent->virtual_code = 0;
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: FocusIn event."));
        break;
      }

      case FocusOut:
      {
        if (foreign)
          break;
        
        m_pEvent->e_event = NUX_WINDOW_EXIT_FOCUS;
        m_pEvent->e_mouse_state = 0;

        // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process()
        m_pEvent->e_x = 0xFFFFFFFF;
        m_pEvent->e_y = 0xFFFFFFFF;
        m_pEvent->e_dx = 0;
        m_pEvent->e_dy = 0;
        m_pEvent->virtual_code = 0;
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: FocusOut event."));
        break;
      }

      case KeyPress:
      {
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: KeyPress event."));
        KeyCode keycode = xevent.xkey.keycode;
        KeySym keysym = NoSymbol;
        keysym = XKeycodeToKeysym (m_X11Display, keycode, 0);
        unsigned int inlKeysym = GraphicsDisplay::X11KeySymToINL (keysym);
        m_pEvent->VirtualKeycodeState[inlKeysym] = 1;

        m_pEvent->e_key_modifiers = GetModifierKeyState (xevent.xkey.state);

        m_pEvent->e_key_repeat_count = 0;
        m_pEvent->e_keysym = inlKeysym;
        m_pEvent->e_event = NUX_KEYDOWN;

        static XComposeStatus ComposeStatus;
        static char buffer[16];
        m_pEvent->e_text[0] = 0;

        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: Keysym: %d - %x."), keysym, keysym);
        if (XLookupString (&xevent.xkey, buffer, sizeof (buffer), NULL, &ComposeStatus) )
        {
          m_pEvent->e_text[0] = buffer[0];
        }

        break;
      }

      case KeyRelease:
      {
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: KeyRelease event."));
        KeyCode keycode = xevent.xkey.keycode;
        KeySym keysym = NoSymbol;
        keysym = XKeycodeToKeysym (m_X11Display, keycode, 0);
        unsigned int inlKeysym = GraphicsDisplay::X11KeySymToINL (keysym);
        m_pEvent->VirtualKeycodeState[inlKeysym] = 0;

        m_pEvent->e_key_modifiers = GetModifierKeyState (xevent.xkey.state);

        m_pEvent->e_key_repeat_count = 0;
        m_pEvent->e_keysym = inlKeysym;
        m_pEvent->e_event = NUX_KEYUP;
        break;
      }

      case ButtonPress:
      {
        m_pEvent->e_x = x_recalc;
        m_pEvent->e_y = y_recalc;
        m_pEvent->e_x_root = 0;
        m_pEvent->e_y_root = 0;
        mouse_press (xevent, m_pEvent);
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: ButtonPress event."));
        break;
      }

      case ButtonRelease:
      {
        m_pEvent->e_x = x_recalc;
        m_pEvent->e_y = y_recalc;
        m_pEvent->e_x_root = 0;
        m_pEvent->e_y_root = 0;
        mouse_release (xevent, m_pEvent);
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: ButtonRelease event."));
        break;
      }

      case MotionNotify:
      {
        m_pEvent->e_x = x_recalc;
        m_pEvent->e_y = y_recalc;
        m_pEvent->e_x_root = 0;
        m_pEvent->e_y_root = 0;
        mouse_move (xevent, m_pEvent);
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: MotionNotify event."));
        break;
      }

      // Note: there is no WM_MOUSEENTER. WM_MOUSEENTER is equivalent to WM_MOUSEMOVE after a WM_MOUSELEAVE.
      case LeaveNotify:
      {
        m_pEvent->e_event = NUX_WINDOW_MOUSELEAVE;
        m_pEvent->e_x = x_recalc;
        m_pEvent->e_y = y_recalc;
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: LeaveNotify event."));
        break;
      }

      case EnterNotify:
      {
        m_pEvent->e_event = NUX_WINDOW_MOUSELEAVE;
        m_pEvent->e_x = x_recalc;
        m_pEvent->e_y = y_recalc;
        //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: EnterNotify event."));
        break;
      }

      case ClientMessage:
      {
        if (foreign)
          break;
        
        if ( (xevent.xclient.format == 32) && ( (xevent.xclient.data.l[0]) == static_cast<long> (m_WMDeleteWindow) ) )
        {
          m_pEvent->e_event = NUX_TERMINATE_APP;
          //nuxDebugMsg(TEXT("[GraphicsDisplay::ProcessXEvents]: ClientMessage event: Close Application."));
        }

        break;
      }
    }
  }

  int GraphicsDisplay::X11KeySymToINL (int Keysym)
  {
    switch (Keysym)
    {
      case XK_Cancel:
        return NUX_VK_CANCEL;
      case XK_BackSpace:
        return NUX_VK_BACKSPACE;
      case XK_Tab:
        return NUX_VK_TAB;
      case XK_Clear:
        return NUX_VK_CLEAR;
      case XK_Return:
        return NUX_VK_ENTER;
      case XK_Shift_L:
        return NUX_VK_SHIFT;
      case XK_Control_L:
        return NUX_VK_CONTROL;
      case XK_Alt_L:
        return NUX_VK_MENU;
      case XK_Pause:
        return NUX_VK_PAUSE;
      case XK_Caps_Lock:
        return NUX_VK_CAPITAL;
      case XK_Escape:
        return NUX_VK_ESCAPE;
      case XK_space:
        return NUX_VK_SPACE;
      case XK_Page_Up:
        return NUX_VK_PAGE_UP;
      case XK_Page_Down:
        return NUX_VK_PAGE_DOWN;
      case XK_End:
        return NUX_VK_END;
      case XK_Home:
        return NUX_VK_HOME;
      case XK_Left:
        return NUX_VK_LEFT;
      case XK_Up:
        return NUX_VK_UP;
      case XK_Right:
        return NUX_VK_RIGHT;
      case XK_Down:
        return NUX_VK_DOWN;
      case XK_Select:
        return NUX_VK_SELECT;
      case XK_Print:
        return NUX_VK_PRINT;
      case XK_Execute:
        return NUX_VK_EXECUTE;
      case XK_Insert:
        return NUX_VK_INSERT;
      case XK_Delete:
        return NUX_VK_DELETE;
      case XK_Help:
        return NUX_VK_HELP;
      case XK_0:
        return NUX_VK_0;
      case XK_1:
        return NUX_VK_1;
      case XK_2:
        return NUX_VK_2;
      case XK_3:
        return NUX_VK_3;
      case XK_4:
        return NUX_VK_4;
      case XK_5:
        return NUX_VK_5;
      case XK_6:
        return NUX_VK_6;
      case XK_7:
        return NUX_VK_7;
      case XK_8:
        return NUX_VK_8;
      case XK_9:
        return NUX_VK_9;
      case XK_A:
        return NUX_VK_A;
      case XK_B:
        return NUX_VK_B;
      case XK_C:
        return NUX_VK_C;
      case XK_D:
        return NUX_VK_D;
      case XK_E:
        return NUX_VK_E;
      case XK_F:
        return NUX_VK_F;
      case XK_G:
        return NUX_VK_G;
      case XK_H:
        return NUX_VK_H;
      case XK_I:
        return NUX_VK_I;
      case XK_J:
        return NUX_VK_J;
      case XK_K:
        return NUX_VK_K;
      case XK_L:
        return NUX_VK_L;
      case XK_M:
        return NUX_VK_M;
      case XK_N:
        return NUX_VK_N;
      case XK_O:
        return NUX_VK_O;
      case XK_P:
        return NUX_VK_P;
      case XK_Q:
        return NUX_VK_Q;
      case XK_R:
        return NUX_VK_R;
      case XK_S:
        return NUX_VK_S;
      case XK_T:
        return NUX_VK_T;
      case XK_U:
        return NUX_VK_U;
      case XK_V:
        return NUX_VK_V;
      case XK_W:
        return NUX_VK_W;
      case XK_X:
        return NUX_VK_X;
      case XK_Y:
        return NUX_VK_Y;
      case XK_Z:
        return NUX_VK_Z;
      case XK_Super_L:
        return NUX_VK_LWIN;
      case XK_Super_R:
        return NUX_VK_RWIN;
      case XK_KP_0:
        return NUX_VK_NUMPAD0;
      case XK_KP_1:
        return NUX_VK_NUMPAD1;
      case XK_KP_2:
        return NUX_VK_NUMPAD2;
      case XK_KP_3:
        return NUX_VK_NUMPAD3;
      case XK_KP_4:
        return NUX_VK_NUMPAD4;
      case XK_KP_5:
        return NUX_VK_NUMPAD5;
      case XK_KP_6:
        return NUX_VK_NUMPAD6;
      case XK_KP_7:
        return NUX_VK_NUMPAD7;
      case XK_KP_8:
        return NUX_VK_NUMPAD8;
      case XK_KP_9:
        return NUX_VK_NUMPAD9;
      case XK_KP_Multiply:
        return NUX_VK_MULTIPLY;
      case XK_KP_Add:
        return NUX_VK_ADD;
      case XK_KP_Separator:
        return NUX_VK_SEPARATOR;
      case XK_KP_Subtract:
        return NUX_VK_SUBTRACT;
      case XK_KP_Decimal:
        return NUX_VK_DECIMAL;
      case XK_KP_Divide:
        return NUX_VK_DIVIDE;
      case XK_F1:
        return NUX_VK_F1;
      case XK_F2:
        return NUX_VK_F2;
      case XK_F3:
        return NUX_VK_F3;
      case XK_F4:
        return NUX_VK_F4;
      case XK_F5:
        return NUX_VK_F5;
      case XK_F6:
        return NUX_VK_F6;
      case XK_F7:
        return NUX_VK_F7;
      case XK_F8:
        return NUX_VK_F8;
      case XK_F9:
        return NUX_VK_F9;
      case XK_F10:
        return NUX_VK_F10;
      case XK_F11:
        return NUX_VK_F11;
      case XK_F12:
        return NUX_VK_F12;
      case XK_F13:
        return NUX_VK_F13;
      case XK_F14:
        return NUX_VK_F14;
      case XK_F15:
        return NUX_VK_F15;
      case XK_F16:
        return NUX_VK_F16;
      case XK_F17:
        return NUX_VK_F17;
      case XK_F18:
        return NUX_VK_F18;
      case XK_F19:
        return NUX_VK_F19;
      case XK_F20:
        return NUX_VK_F20;
      case XK_F21:
        return NUX_VK_F21;
      case XK_F22:
        return NUX_VK_F22;
      case XK_F23:
        return NUX_VK_F23;
      case XK_F24:
        return NUX_VK_F24;
      case XK_Num_Lock:
        return NUX_VK_NUMLOCK;
      case XK_Scroll_Lock:
        return NUX_VK_SCROLL;
      case XK_Shift_R:
        return NUX_VK_RSHIFT;
      case XK_Control_R:
        return NUX_VK_RCONTROL;
      case XK_Alt_R:
        return NUX_VK_RMENU;
      default:
        return 0x0;
    }
  };

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::ShowWindow()
  {
#if defined(_WIN32)
    ::ShowWindow (hWnd, SW_RESTORE);
#endif
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::HideWindow()
  {
#if defined(_WIN32)
    ::ShowWindow (hWnd, SW_MINIMIZE);
#endif
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::EnterMaximizeWindow()
  {
#if defined(_WIN32)
    ::ShowWindow (hWnd, SW_MAXIMIZE);
#endif
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::ExitMaximizeWindow()
  {
#if defined(_WIN32)
    ::ShowWindow (hWnd, SW_RESTORE);
#endif
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::SetWindowTitle (const TCHAR *Title)
  {
#if defined(_WIN32)
    SetWindowText (hWnd, Title);
#endif
  }

//---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::HasVSyncSwapControl() const
  {
    return GetThreadGLDeviceFactory()->GetGpuInfo().Support_EXT_Swap_Control();
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::EnableVSyncSwapControl()
  {
#if _WIN32

    if (HasVSyncSwapControl() )
    {
      wglSwapIntervalEXT (1);
    }

#endif
  }

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::DisableVSyncSwapControl()
  {
#if _WIN32

    if (HasVSyncSwapControl() )
    {
      wglSwapIntervalEXT (0);
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
  //---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::StartOpenFileDialog(FileDialogOption& fdo)
  {
      return Win32OpenFileDialog(GetWindowHandle(), fdo);
  }

  //---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::StartSaveFileDialog(FileDialogOption& fdo)
  {
      return Win32SaveFileDialog(GetWindowHandle(), fdo);
  }

  //---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::StartColorDialog(ColorDialogOption& cdo)
  {
      return Win32ColorDialog(GetWindowHandle(), cdo);
  }
  */
//---------------------------------------------------------------------------------------------------------
  /*void GraphicsDisplay::SetWindowCursor(HCURSOR cursor)
  {
      m_Cursor = cursor;
  }

  //---------------------------------------------------------------------------------------------------------
  HCURSOR GraphicsDisplay::GetWindowCursor() const
  {
      return m_Cursor;
  }*/

//---------------------------------------------------------------------------------------------------------
  void GraphicsDisplay::PauseThreadGraphicsRendering()
  {
    m_PauseGraphicsRendering = true;
    MakeGLContextCurrent();
  }

//---------------------------------------------------------------------------------------------------------
  bool GraphicsDisplay::IsPauseThreadGraphicsRendering() const
  {
    return m_PauseGraphicsRendering;
  }

}
