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
#include "GLDeviceFactory.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "OpenGLEngine.h"
#include "GLWindowManager.h"
#include "GfxEventsX11.h"
#include "IniFile.h"

#include "GfxSetupX11.h"

NAMESPACE_BEGIN_OGL

unsigned int gVirtualKeycodeState[INL_MAX_VK];

// Attributes for a single buffered visual in RGBA format
static int g_DoubleBufferVisual[] = {
    GLX_RGBA,
    GLX_DOUBLEBUFFER,
    GLX_RED_SIZE,       8,
    GLX_GREEN_SIZE,     8,
    GLX_BLUE_SIZE,      8,
    GLX_ALPHA_SIZE,     8,
    GLX_DEPTH_SIZE,     24,
    GLX_STENCIL_SIZE,   8,
    None};

// Attributes for a single buffered visual in RGBA format
static int g_SingleBufferVisual[] = {
    GLX_RGBA, 
    GLX_RED_SIZE,       8,
    GLX_GREEN_SIZE,     8,
    GLX_BLUE_SIZE,      8,
    GLX_ALPHA_SIZE,     8,
    GLX_DEPTH_SIZE,     24,
    GLX_STENCIL_SIZE,   8,
    None };

// Compute the frame rate every FRAME_RATE_PERIODE;
#define FRAME_RATE_PERIODE    10

#define INL_MISSING_GL_EXTENSION_MESSAGE_BOX(message) {MessageBox(NULL, TEXT("Missing extension: " #message), TEXT("ERROR"), MB_OK|MB_ICONERROR); exit(-1);}
#define INL_ERROR_EXIT_MESSAGE(message) inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, #message " The program will exit.")); exit(-1);

EventToNameStruct EventToName[] = 
{
    {INL_NO_EVENT,               TEXT("INL_NO_EVENT")},
    {INL_MOUSE_PRESSED,          TEXT("INL_MOUSE_PRESSED")},
    {INL_MOUSE_RELEASED,         TEXT("INL_MOUSE_RELEASED")},
    {INL_KEYDOWN,                TEXT("INL_KEYDOWN")},
    {INL_KEYUP,                  TEXT("INL_KEYUP")},
    {INL_MOUSE_MOVE,             TEXT("INL_MOUSE_MOVE")},
    {INL_SIZE_CONFIGURATION,     TEXT("INL_SIZE_CONFIGURATION")},
    {INL_WINDOW_CONFIGURATION,   TEXT("INL_WINDOW_CONFIGURATION")},
    {INL_WINDOW_ENTER_FOCUS,     TEXT("INL_WINDOW_ENTER_FOCUS")},
    {INL_WINDOW_EXIT_FOCUS,      TEXT("INL_WINDOW_EXIT_FOCUS")},
    {INL_WINDOW_DIRTY,           TEXT("INL_WINDOW_DIRTY")},
    {INL_WINDOW_MOUSELEAVE,      TEXT("INL_WINDOW_MOUSELEAVE")},
    {INL_TERMINATE_APP,          TEXT("INL_TERMINATE_APP")}
};

//---------------------------------------------------------------------------------------------------------

GLWindowImpl::GLWindowImpl()
:   m_pEvent(NULL)
,   m_GLCtx(0)
,   m_GfxInterfaceCreated(false)
,   m_Fullscreen(false)
,   m_ScreenBitDepth(32)
,   m_num_device_modes(0)
,   m_BestMode(-1)
,   m_BackupFrameBuffer(true)
,   m_DeviceFactory(0)
,   m_GraphicsContext(0)
,   m_Style(WINDOWSTYLE_NORMAL)
,   m_PauseGraphicsRendering(false)
,   m_ParentWindow(0)
{
    inlSetThreadLocalStorage(ThreadLocal_GLWindowImpl, this);
    
    m_X11LastEvent.type = -1;
    m_X11RepeatKey = true;

	m_GfxInterfaceCreated = false;
	m_pEvent = new IEvent();

    m_WindowSize.SetWidth(0);
	m_WindowSize.SetHeight(0);

    // A window never starts in a minimized state.
    m_is_window_minimized = false;


    FilePath m_FilePath;
    m_FilePath.AddSearchPath(TEXT("")); // for case where fully qualified path is given
    m_FilePath.AddSearchPath(TEXT("./ini"));
    m_FilePath.AddSearchPath(TEXT("../ini"));
    m_FilePath.AddSearchPath(TEXT("../../ini"));
    m_FilePath.AddSearchPath(TEXT("../../../ini"));


    NString file_search = TEXT("inalogic.ini");
    NString FileName = m_FilePath.GetFile(file_search.GetTCharPtr());
    if (FileName == TEXT(""))
    {
        nuxDebugMsg(TEXT("[GLWindowImpl::GLWindowImpl] Can't find inalogic.ini file."));
        /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Can't find .ini file.\nThe program will exit."));*/
        exit(-1);
    }

    if(FileName != TEXT(""))
    {
        NString key_name = TEXT("UseBackupFBO");
        NString section_name = TEXT("OpenGLSystem");

        if(CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()) == false)
        {
            nuxDebugMsg(TEXT("[GLWindowImpl::GLWindowImpl] Key [%s] does not exit in .ini file."));
        }
        else
        {
            NString value = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
            if((value == TEXT("1")) || (value == TEXT("true")))
            {
                m_BackupFrameBuffer = true;
            }
            else
            {
                m_BackupFrameBuffer = false;
            }
        }

        section_name = TEXT("SearchPath");
        key_name = TEXT("FontPath");
        if(CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()))
        {
            NString FontPath = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
            FontPath.ParseToArray(m_FontSearchPath, TEXT(";"));
        }
        else
        {
            nuxDebugMsg(TEXT("[GLWindowImpl::GLWindowImpl] Failed to read font search path from .ini file."));
            /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
                TEXT("Failed to read font search path from .ini file.\nThe program will exit."));*/
            exit(-1);

        }

        key_name = TEXT("UITexturePath");
        if(CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()))
        {
            NString UITexturePath = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
            UITexturePath.ParseToArray(m_UITextureSearchPath, TEXT(";"));
        }
        else
        {
            nuxDebugMsg(TEXT("[GLWindowImpl::GLWindowImpl] Failed to read texture search path from .ini file."));
            /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
                TEXT("Failed to read texture search path from .ini file.\nThe program will exit."));*/
            exit(-1);
        }

        key_name = TEXT("ShaderPath");
        if(CIniFile::RecordExists(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr()))
        {
            NString ShaderPath = CIniFile::GetValue(key_name.GetTCharPtr(), section_name.GetTCharPtr(), FileName.GetTCharPtr());
            ShaderPath.ParseToArray(m_ShaderSearchPath, TEXT(";"));
        }
        else
        {
            nuxDebugMsg(TEXT("[GLWindowImpl::GLWindowImpl] Failed to read shader search path from .ini file."));
            /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
                TEXT("Failed to read shader search path from .ini file.\nThe program will exit."));*/
            exit(-1);
        }
    }

    m_ResourcePathLocation.AddSearchPath(TEXT(""));
    m_ResourcePathLocation.AddSearchPath(TEXT("./"));
    m_ResourcePathLocation.AddSearchPath(TEXT("../"));
    m_ResourcePathLocation.AddSearchPath(TEXT("../../"));
    m_ResourcePathLocation.AddSearchPath(TEXT("./Data"));
    m_ResourcePathLocation.AddSearchPath(TEXT("../Data"));
    m_ResourcePathLocation.AddSearchPath(TEXT("../../Data"));
    m_ResourcePathLocation.AddSearchPath(TEXT("../../../Data"));

    m_ResourcePathLocation.AddSearchPath(m_FontSearchPath);
    m_ResourcePathLocation.AddSearchPath(m_ShaderSearchPath);
    m_ResourcePathLocation.AddSearchPath(m_UITextureSearchPath);
}

//---------------------------------------------------------------------------------------------------------
GLWindowImpl::~GLWindowImpl()
{
    INL_SAFE_DELETE( m_GraphicsContext );
    INL_SAFE_DELETE( m_DeviceFactory );

    DestroyOpenGLWindow();
    INL_SAFE_DELETE( m_pEvent );

    inlSetThreadLocalStorage(ThreadLocal_GLWindowImpl, 0);
}

//---------------------------------------------------------------------------------------------------------
NString GLWindowImpl::FindResourceLocation(const TCHAR* ResourceFileName, bool ErrorOnFail)
{
    NString path = m_ResourcePathLocation.GetFile(ResourceFileName);

    if(path == TEXT("") && ErrorOnFail)
    {
        nuxDebugMsg(TEXT("[GLWindowImpl::FindResourceLocation] Failed to locate resource file: %s."), ResourceFileName);
        /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Failed to locate resource file %s.\nThe program will exit."), ResourceFileName);*/
        exit(1);
    }
    return path;
}

NString GLWindowImpl::FindUITextureLocation(const TCHAR* ResourceFileName, bool ErrorOnFail)
{
    FilePath searchpath;
    searchpath.AddSearchPath(m_UITextureSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if((path == TEXT("")) && ErrorOnFail)
    {
        nuxDebugMsg(TEXT("[GLWindowImpl::FindResourceLocation] Failed to locate ui texture file: %s."), ResourceFileName);
        /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Failed to locate ui texture file %s.\nThe program will exit."), ResourceFileName);*/
        exit(1);
    }
    return path;
}

NString GLWindowImpl::FindShaderLocation(const TCHAR* ResourceFileName, bool ErrorOnFail)
{
    FilePath searchpath;
    searchpath.AddSearchPath(m_ShaderSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if((path == TEXT("")) && ErrorOnFail)
    {
        nuxDebugMsg(TEXT("[GLWindowImpl::FindResourceLocation] Failed to locate shader file: %s."), ResourceFileName);
        /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Failed to locate shader file %s.\nThe program will exit."), ResourceFileName);*/
        exit(1);
    }
    return path;
}

NString GLWindowImpl::FindFontLocation(const TCHAR* ResourceFileName, bool ErrorOnFail)
{
    FilePath searchpath;
    searchpath.AddSearchPath(m_FontSearchPath);
    NString path = searchpath.GetFile(ResourceFileName);

    if((path == TEXT("")) && ErrorOnFail)
    {
        nuxDebugMsg(TEXT("[GLWindowImpl::FindResourceLocation] Failed to locate font file file: %s."), ResourceFileName);
        /*inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Failed to locate font file %s.\nThe program will exit."), ResourceFileName);*/
        exit(1);
    }
    return path;
}


//---------------------------------------------------------------------------------------------------------
bool GLWindowImpl::IsGfxInterfaceCreated()
{
	return m_GfxInterfaceCreated;
}

//---------------------------------------------------------------------------------------------------------
static NCriticalSection CreateOpenGLWindow_CriticalSection;
bool GLWindowImpl::CreateOpenGLWindow(const TCHAR* WindowTitle,
                                      unsigned int WindowWidth,
                                      unsigned int WindowHeight,
                                      WindowStyle Style,
                                      const GLWindowImpl* Parent,
                                      bool FullscreenFlag)
{
    NScopeLock Scope(&CreateOpenGLWindow_CriticalSection);

	m_GfxInterfaceCreated = false;

	// FIXME : put at the end
	m_ViewportSize.SetWidth(WindowWidth);
	m_ViewportSize.SetHeight(WindowHeight);
	m_WindowSize.SetWidth(WindowWidth);
	m_WindowSize.SetHeight(WindowHeight);
	// end of fixme

	m_Fullscreen = FullscreenFlag;								// Set The Global Fullscreen Flag
	m_BestMode = -1;								// assume -1 if the mode is not fullscreen

    m_X11Display = XOpenDisplay(0);
    if(m_X11Display == 0)
    {
        nuxDebugMsg(TEXT("[GLWindowImpl::CreateOpenGLWindow] XOpenDisplay has failed. The window cannot be created."));
        return false;
    }
    m_X11Screen = DefaultScreen(m_X11Display);
    XF86VidModeQueryVersion(m_X11Display, &m_X11VerMajor, &m_X11VerMinor);

    XF86VidModeGetAllModeLines(m_X11Display, m_X11Screen, &m_NumVideoModes, &m_X11VideoModes);
    m_X11OriginalVideoMode = *m_X11VideoModes[0];

	if(m_Fullscreen)									        // Attempt Fullscreen Mode?
	{
		// check if resolution is supported
		bool mode_supported = false;
		for(int num_modes = 0 ; num_modes < m_NumVideoModes; num_modes++)
		{
			if((m_X11VideoModes[num_modes]->hdisplay == m_ViewportSize.GetWidth()) 
				&& (m_X11VideoModes[num_modes]->vdisplay == m_ViewportSize.GetHeight()))
			{
				mode_supported = true;
				m_BestMode = num_modes;
				break;
			}
		}

		if(mode_supported == false)
		{
            m_Fullscreen = false;
		}
	}

    glXQueryVersion(m_X11Display, &m_GLXVerMajor, &m_GLXVerMinor);

    // Get an appropriate visual
    m_X11VisualInfo = glXChooseVisual(m_X11Display, m_X11Screen, g_DoubleBufferVisual);

    if (m_X11VisualInfo == NULL)
    {
        nuxDebugMsg(TEXT("[GLWindowImpl::CreateOpenGLWindow] Cannot get appropriate visual."));
        return false;
    }

    m_GLCtx = glXCreateContext(m_X11Display, m_X11VisualInfo, 0, GL_TRUE);

    m_X11Colormap = XCreateColormap(m_X11Display,
        RootWindow(m_X11Display, m_X11VisualInfo->screen),
        m_X11VisualInfo->visual,
        AllocNone);

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
        XF86VidModeSwitchToMode(m_X11Display, m_X11Screen, m_X11VideoModes[m_BestMode]);
        XF86VidModeSetViewPort(m_X11Display, m_X11Screen, 0, 0);
        //Width = m_X11VideoModes[m_BestMode]->hdisplay;
        //Height = m_X11VideoModes[m_BestMode]->vdisplay;
        XFree(m_X11VideoModes);

        /* create a fullscreen window */

        m_X11Window = XCreateWindow(m_X11Display,
            RootWindow(m_X11Display, m_X11VisualInfo->screen),
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
        
        XWarpPointer(m_X11Display, None, m_X11Window, 0, 0, 0, 0, 0, 0);
        XMapRaised(m_X11Display, m_X11Window);
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
        m_WMDeleteWindow = XInternAtom(m_X11Display, "WM_DELETE_WINDOW", True);
        XSetWMProtocols(m_X11Display, m_X11Window, &m_WMDeleteWindow, 1);

        XSetStandardProperties(m_X11Display, m_X11Window, WindowTitle, WindowTitle, None, NULL, 0, NULL);
        XMapRaised(m_X11Display, m_X11Window);
    } 
    
	MakeGLContextCurrent();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SwapBuffer();

	m_GfxInterfaceCreated = true;

    m_DeviceFactory = new GLDeviceFactory(m_ViewportSize.GetWidth(), m_ViewportSize.GetHeight(), BITFMT_R8G8B8A8);
    m_GraphicsContext = new GraphicsContext(*this);

    EnableVSyncSwapControl();
    //DisableVSyncSwapControl();

	return TRUE;
}

bool GLWindowImpl::CreateFromOpenGLWindow(Display *X11Display, Window X11Window, GLXContext OpenGLContext)
{
    m_X11Display = X11Display;
    m_X11Window = X11Window;
    m_GLCtx = OpenGLContext;

    Window root_return;
    int x_return, y_return;
    unsigned int width_return, height_return;
    unsigned int border_width_return;
    unsigned int depth_return;

    XGetGeometry(X11Display, X11Window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return);
    m_WindowSize = Size(width_return, height_return);
    m_ViewportSize = Size(width_return, height_return);

    MakeGLContextCurrent();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SwapBuffer();

    m_GfxInterfaceCreated = true;

    m_DeviceFactory = new GLDeviceFactory(m_ViewportSize.GetWidth(), m_ViewportSize.GetHeight(), BITFMT_R8G8B8A8);
    m_GraphicsContext = new GraphicsContext(*this);

    EnableVSyncSwapControl();
}

// bool GLWindowImpl::CreateVisual(unsigned int WindowWidth, unsigned int WindowHeight, XVisualInfo& ChosenVisual, XVisualInfo& Template, unsigned long Mask)
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
//         nuxDebugMsg(TEXT("[GLWindowImpl::CreateVisual] There is no matching visuals."));
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
bool GLWindowImpl::HasFrameBufferSupport()
{
    return m_DeviceFactory->SUPPORT_GL_EXT_FRAMEBUFFER_OBJECT() && m_BackupFrameBuffer;
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::GetWindowSize(int &w, int &h)
{
	w = m_WindowSize.GetWidth();
	h = m_WindowSize.GetHeight();
}

void GLWindowImpl::GetDesktopSize(int &w, int &h)
{
    Window root;
    int x, y;
    unsigned int width, height, depth, border_width;
    bool ret = XGetGeometry(m_X11Display, RootWindow(m_X11Display, m_X11Screen),
                        &root,
                        &x, &y,
                        &width, &height, &border_width, &depth);

    if(ret == false)
    {
        nuxAssert(TEXT("[GetDesktopSize] Failed to get the desktop size"));
        w = 0; h = 0;
    }
}


//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::SetWindowSize(int width, int height)
{
    nuxDebugMsg(TEXT("[GLWindowImpl::SetWindowSize] Setting window size to %dx%d"), width, height);
    // Resize window client area
    XResizeWindow(m_X11Display, m_X11Window, width, height);
    XFlush(m_X11Display);
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::SetWindowPosition(int x, int y)
{
    nuxDebugMsg(TEXT("[GLWindowImpl::SetWindowPosition] Setting window position to %dx%d"), x, y);
    // Resize window client area
    XMoveWindow(m_X11Display, m_X11Window, x, y);
    XFlush(m_X11Display);
}

//---------------------------------------------------------------------------------------------------------
unsigned int GLWindowImpl::GetWindowWidth()
{
	return m_WindowSize.GetWidth();
}

//---------------------------------------------------------------------------------------------------------
unsigned int GLWindowImpl::GetWindowHeight()
{
	return m_WindowSize.GetHeight();
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::SetViewPort(int x, int y, int width, int height)
{
    
    if(IsGfxInterfaceCreated())
    {
        //do not rely on m_ViewportSize: glViewport can be called directly
        m_ViewportSize.SetWidth(width);
        m_ViewportSize.SetHeight(height);

        m_GraphicsContext->SetViewport(x, y, m_ViewportSize.GetWidth(), m_ViewportSize.GetHeight());
        m_GraphicsContext->SetScissor(0, 0, width, height);
    }
}

Point GLWindowImpl::GetMouseScreenCoord()
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

Point GLWindowImpl::GetMouseWindowCoord()
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

Point GLWindowImpl::GetWindowCoord()
{
    XWindowAttributes attrib;
    int status = XGetWindowAttributes(m_X11Display, m_X11Window, &attrib);
    if(status == 0)
    {
        nuxAssert(TEXT("[GLWindowImpl::GetWindowCoord] Failed to get the window attributes."));
        return Point(0, 0);
    }
    return Point(attrib.x, attrib.y);
}

Rect GLWindowImpl::GetWindowGeometry()
{
    XWindowAttributes attrib;
    int status = XGetWindowAttributes(m_X11Display, m_X11Window, &attrib);
    if(status == 0)
    {
        nuxAssert(TEXT("[GLWindowImpl::GetWindowGeometry] Failed to get the window attributes."));
        return Rect(0, 0, 0, 0);
    }
    return Rect(attrib.x, attrib.y, attrib.width, attrib.height);
}

Rect GLWindowImpl::GetNCWindowGeometry()
{
    XWindowAttributes attrib;
    int status = XGetWindowAttributes(m_X11Display, m_X11Window, &attrib);
    if(status == 0)
    {
        nuxAssert(TEXT("[GLWindowImpl::GetWindowGeometry] Failed to get the window attributes."));
        return Rect(0, 0, 0, 0);
    }
    return Rect(attrib.x, attrib.y, attrib.width, attrib.height);
}

//---------------------------------------------------------------------------------------------------------
//void GLWindowImpl::EnableBackupFrameBuffer()
//{
//    if(m_BackupFrameBuffer)
//    {
//        m_MainFBO->Enable();
//    }
//}
//
//void GLWindowImpl::DisableBackupFrameBuffer()
//{
//    if(m_BackupFrameBuffer)
//    {
//        m_MainFBO->Disable();
//    }
//}
//
//void GLWindowImpl::BackupFrameBufferBind()
//{
//    if(m_BackupFrameBuffer)
//    {
//        m_MainFBO->BindToTexUnit0();
//    }
//}
//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::MakeGLContextCurrent()
{
    if(!glXMakeCurrent(m_X11Display, m_X11Window, m_GLCtx))
    {
        DestroyOpenGLWindow();
    }
}
//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::SwapBuffer(bool glswap)
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
            //nuxDebugMsg(TEXT("[GLWindowImpl::SwapBuffer]: MotionNotify event."));
            bsleep = false;
        }
    }*/

    if(IsPauseThreadGraphicsRendering())
        return;

    if(glswap)
    {
	    glXSwapBuffers(m_X11Display, m_X11Window);
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
//         //nuxDebugMsg(TEXT("[GLWindowImpl::SwapBuffer] Frametime: %f"), m_FrameTime);
//         m_FrameRate = m_FramePeriodeCounter / (m_PeriodeTime / 1000.0f);
//         m_PeriodeTime = 0.0f;
//         m_FramePeriodeCounter = 0;
//     }
}
//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::DestroyOpenGLWindow()
{
    if(m_GfxInterfaceCreated == true)
    {
        if (m_GLCtx)
        {
            if (!glXMakeCurrent(m_X11Display, None, NULL))
            {
                nuxAssert(TEXT("[GLWindowImpl::DestroyOpenGLWindow] glXMakeCurrent failed."));
            }
            glXDestroyContext(m_X11Display, m_GLCtx);
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

// //---------------------------------------------------------------------------------------------------------
// // convert a MSWindows VK_x to an INL keysym or and extended INL keysym:
// static const struct {unsigned short vk, fltk, extended;} vktab[] = {
//     {INL_VK_BACK,	    INL_BackSpace},
//     {INL_VK_TAB,	    INL_Tab},
//     {INL_VK_CLEAR,	    INL_Clear,	    0xff0b/*XK_Clear*/},
//     {INL_VK_ENTER,	    INL_Enter,	    INL_KP_ENTER},
//     {INL_VK_SHIFT,	    INL_Shift_L,	INL_EXT_Shift_R},
//     {INL_VK_CONTROL,	INL_Control_L,	INL_EXT_Control_R},
//     {INL_VK_MENU,	    INL_Alt_L,	    INL_EXT_Alt_R},
//     {INL_VK_PAUSE,	    INL_Pause},
//     {INL_VK_CAPITAL,	INL_Caps_Lock},
//     {INL_VK_ESCAPE,	    INL_Escape},
//     {INL_VK_SPACE,	    ' '},
//     {INL_VK_PAGE_UP,	INL_Page_Up     /*KP+'9'*/,	    INL_KP_PAGE_UP},
//     {INL_VK_PAGE_DOWN,  INL_Page_Down   /*KP+'3'*/,	    INL_KP_PAGE_DOWN},
//     {INL_VK_END,	    INL_End         /*KP+'1'*/,	    INL_KP_END},
//     {INL_VK_HOME,	    INL_Home        /*KP+'7'*/,	    INL_KP_HOME},
//     {INL_VK_LEFT,	    INL_Left        /*KP+'4'*/,	    INL_KP_LEFT},
//     {INL_VK_UP,	        INL_Up          /*KP+'8'*/,	    INL_KP_UP},
//     {INL_VK_RIGHT,	    INL_Right       /*KP+'6'*/,	    INL_KP_RIGHT},
//     {INL_VK_DOWN,	    INL_Down        /*KP+'2'*/,	    INL_KP_DOWN},
//     {INL_VK_SNAPSHOT,	INL_Print},	    // does not work on NT
//     {INL_VK_INSERT,	    INL_Insert      /*KP+'0'*/,	    INL_KP_INSERT},
//     {INL_VK_DELETE,	    INL_Delete      /*KP+'.'*/,	    INL_KP_DELETE},
//     {INL_VK_LWIN,	    INL_LWin        /*Meta_L*/},
//     {INL_VK_RWIN,	    INL_RWin        /*Meta_R*/},
//     {INL_VK_APPS,	    INL_VK_APPS     /*Menu*/},
//     {INL_VK_MULTIPLY,	INL_Multiply    /*KP+'*'*/},
//     {INL_VK_ADD,	    INL_Add         /*KP+'+'*/},
//     {INL_VK_SUBTRACT,	INL_Subtract    /*KP+'-'*/},
//     {INL_VK_DECIMAL,	INL_Decimal     /*KP+'.'*/},
//     {INL_VK_DIVIDE,	    INL_Divide      /*KP+'/'*/},
//     {INL_VK_NUMLOCK,	INL_Numlock     /*Num_Lock*/},
//     {INL_VK_SCROLL,	    INL_Scroll      /*Scroll_Lock*/},
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
static int mouse_move(XEvent xevent, IEvent* m_pEvent)
{
    static int px, py, pmx, pmy;
    m_pEvent->e_x = xevent.xmotion.x;
    m_pEvent->e_y = xevent.xmotion.y;
    m_pEvent->e_x_root = 0;
    m_pEvent->e_y_root = 0;

    // Erase mouse event and mouse doubleclick events. Keep the mouse states.
    t_uint32 _mouse_state = m_pEvent->e_mouse_state & 0x0F000000;

    m_pEvent->e_event = INL_MOUSE_MOVE;

    _mouse_state |= (xevent.xmotion.state & Button1Mask) ? INL_STATE_BUTTON1_DOWN : 0;
    _mouse_state |= (xevent.xmotion.state & Button2Mask) ? INL_STATE_BUTTON2_DOWN : 0;
    _mouse_state |= (xevent.xmotion.state & Button3Mask) ? INL_STATE_BUTTON3_DOWN : 0;

    m_pEvent->e_mouse_state = _mouse_state;
}

static int mouse_press(XEvent xevent, IEvent* m_pEvent)
{
    static int px, py, pmx, pmy;
    m_pEvent->e_x = xevent.xbutton.x;
    m_pEvent->e_y = xevent.xbutton.y;
    m_pEvent->e_x_root = 0;
    m_pEvent->e_y_root = 0;

    // Erase mouse event and mouse double-click events. Keep the mouse states.
    ulong _mouse_state = m_pEvent->e_mouse_state & 0x0F000000;

    m_pEvent->e_event = INL_MOUSE_PRESSED;

    // State of the button before the event
    _mouse_state |= (xevent.xbutton.state & Button1Mask) ? INL_STATE_BUTTON1_DOWN : 0;
    _mouse_state |= (xevent.xbutton.state & Button2Mask) ? INL_STATE_BUTTON2_DOWN : 0;
    _mouse_state |= (xevent.xbutton.state & Button3Mask) ? INL_STATE_BUTTON3_DOWN : 0;

    if(xevent.xbutton.type == ButtonPress)
    {
        if(xevent.xbutton.button == Button1)
        {
            _mouse_state |= INL_EVENT_BUTTON1_DOWN;
            _mouse_state |= INL_STATE_BUTTON1_DOWN;
        }
        if(xevent.xbutton.button == Button2)
        {
            _mouse_state |= INL_EVENT_BUTTON2_DOWN;
            _mouse_state |= INL_STATE_BUTTON2_DOWN;
        }
        if(xevent.xbutton.button == Button3)
        {
            _mouse_state |= INL_EVENT_BUTTON3_DOWN;
            _mouse_state |= INL_STATE_BUTTON3_DOWN;
        }
    }
    m_pEvent->e_mouse_state = _mouse_state;
}

static int mouse_release(XEvent xevent, IEvent* m_pEvent)
{
    static int px, py, pmx, pmy;
    m_pEvent->e_x = xevent.xbutton.x;
    m_pEvent->e_y = xevent.xbutton.y;
    m_pEvent->e_x_root = 0;
    m_pEvent->e_y_root = 0;

    // Erase mouse event and mouse double-click events. Keep the mouse states.
    ulong _mouse_state = m_pEvent->e_mouse_state & 0x0F000000;

    m_pEvent->e_event = INL_MOUSE_RELEASED;

    // State of the button before the event
    _mouse_state |= (xevent.xbutton.state & Button1Mask) ? INL_STATE_BUTTON1_DOWN : 0;
    _mouse_state |= (xevent.xbutton.state & Button2Mask) ? INL_STATE_BUTTON2_DOWN : 0;
    _mouse_state |= (xevent.xbutton.state & Button3Mask) ? INL_STATE_BUTTON3_DOWN : 0;

    if(xevent.xbutton.type == ButtonPress)
    {
        if(xevent.xbutton.button == Button1)
        {
            _mouse_state |= INL_EVENT_BUTTON1_DOWN;
            _mouse_state &= ~INL_STATE_BUTTON1_DOWN;
        }
        if(xevent.xbutton.button == Button2)
        {
            _mouse_state |= INL_EVENT_BUTTON2_DOWN;
            _mouse_state &= ~INL_STATE_BUTTON2_DOWN;
        }
        if(xevent.xbutton.button == Button3)
        {
            _mouse_state |= INL_EVENT_BUTTON3_DOWN;
            _mouse_state &= ~INL_STATE_BUTTON3_DOWN;
        }
    }
    m_pEvent->e_mouse_state = _mouse_state;
}

unsigned int GetModifierKeyState(unsigned int modifier_key_state)
{
    unsigned int state = 0;
    
    // For CapsLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the CapsLock: on (keyboard light is on) or off?
    if (modifier_key_state & LockMask)
        state |= INL_STATE_CAPS_LOCK;

    // For NumLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the NumLock: on (keyboard light is on) or off?
    if (modifier_key_state & Mod5Mask)
        state |= INL_STATE_NUMLOCK;

//     if (modifier_key_state & 0x8000) 
//         state |= INL_STATE_SCROLLLOCK;

    if (modifier_key_state & ControlMask)
        state |= INL_STATE_CTRL;

    if (modifier_key_state & ShiftMask)
        state |= INL_STATE_SHIFT;

    if (modifier_key_state & Mod1Mask)
        state |= INL_STATE_ALT;

    return state;
}

//---------------------------------------------------------------------------------------------------------
Bool CheckEventWindow(Display *display, XEvent *xevent, XPointer arg)
{
    return xevent->xany.window == INL_REINTERPRET_CAST(Window, arg);
}

void GLWindowImpl::GetSystemEvent(IEvent *evt)
{
    static bool previous_event_motion = false;
    static int motion_x = 0;
    static int motion_y = 0;

    m_pEvent->Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    m_pEvent->e_mouse_state &= 0x0F000000;
    bool bProcessEvent = true;

    // Process event matching this window
    XEvent xevent;
    if(XCheckIfEvent(m_X11Display, &xevent, &CheckEventWindow, INL_REINTERPRET_CAST(XPointer, m_X11Window)))
    {
        // Detect auto repeat keys. X11 sends a combination of KeyRelease/KeyPress (at the same time) when a key auto repeats.
        // Here, we make sure we process only the keyRelease when the key is effectively released.
        if ((xevent.type == KeyPress) || (xevent.type == KeyRelease))
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
        
	if(xevent.type == MotionNotify)
	{
	  while(XCheckTypedEvent(m_X11Display, MotionNotify, &xevent));
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

        if(bProcessEvent)
            ProcessXEvents(xevent);

        memcpy(evt, m_pEvent, sizeof(IEvent));

    }
    else
    {
        memcpy(evt, m_pEvent, sizeof(IEvent));
    }
} 

IEvent& GLWindowImpl::GetCurrentEvent()
{
    return *m_pEvent;
}

bool GLWindowImpl::HasXPendingEvent() const
{
    return XPending(m_X11Display) ? true : false;
}

void GLWindowImpl::ProcessXEvents(XEvent xevent)
{
    KeySym key;
	switch (xevent.type)
	{
        case DestroyNotify:
            {
                m_pEvent->e_event = INL_DESTROY_WINDOW;
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: DestroyNotify event."));
                break;
            }

		case Expose:
            {
                m_pEvent->e_event = INL_WINDOW_DIRTY;
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: Expose event."));
                break;
		    }


		case ConfigureNotify:
            {
                m_pEvent->e_event = INL_SIZE_CONFIGURATION;
                m_pEvent->width =  xevent.xconfigure.width;
                m_pEvent->height = xevent.xconfigure.height;
                m_WindowSize.SetWidth(xevent.xconfigure.width);
                m_WindowSize.SetHeight(xevent.xconfigure.height);

                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: ConfigureNotify event."));
                break;
            }

        case FocusIn:
            {
                m_pEvent->e_event = INL_WINDOW_ENTER_FOCUS;
                m_pEvent->e_mouse_state = 0;

                // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process().
                // Because WM_SETFOCUS can happen with the mouse outside of the client area, we set e_x and e_y so that the mouse will be 
                // outside of all widgets. A subsequent mouse down or mouse move event will set the correct values for e_x and e_y.
                m_pEvent->e_x = 0xFFFFFFFF;
                m_pEvent->e_y = 0xFFFFFFFF;
                m_pEvent->e_dx = 0;
                m_pEvent->e_dy = 0;
                m_pEvent->virtual_code = 0;
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: FocusIn event."));
                break;
            }

        case FocusOut:
            {
                m_pEvent->e_event = INL_WINDOW_EXIT_FOCUS;
                m_pEvent->e_mouse_state = 0;

                // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process()
                m_pEvent->e_x = 0xFFFFFFFF;
                m_pEvent->e_y = 0xFFFFFFFF;
                m_pEvent->e_dx = 0;
                m_pEvent->e_dy = 0;
                m_pEvent->virtual_code = 0;
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: FocusOut event."));
                break;
            }

        case KeyPress:
            {
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: KeyPress event."));
                KeyCode keycode = xevent.xkey.keycode;
                KeySym keysym = NoSymbol;
                keysym = XKeycodeToKeysym(m_X11Display, keycode, 0);
                unsigned int inlKeysym = GLWindowImpl::X11KeySymToINL(keysym);
                m_pEvent->VirtualKeycodeState[inlKeysym] = 1;

                m_pEvent->e_key_modifiers = GetModifierKeyState(xevent.xkey.state);

                m_pEvent->e_key_repeat_count = 0;
                m_pEvent->e_keysym = inlKeysym;
                m_pEvent->e_event = INL_KEYDOWN;

                static XComposeStatus ComposeStatus;
                static char buffer[16];
                m_pEvent->e_text = "";
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: Keysym: %d - %x."), keysym, keysym);
                if (XLookupString(&xevent.xkey, buffer, sizeof(buffer), NULL, &ComposeStatus))
                {
                    m_pEvent->e_text = buffer;
                }
                break;
            }

        case KeyRelease:
            {
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: KeyRelease event."));
                KeyCode keycode = xevent.xkey.keycode;
                KeySym keysym = NoSymbol;
                keysym = XKeycodeToKeysym(m_X11Display, keycode, 0);
                unsigned int inlKeysym = GLWindowImpl::X11KeySymToINL(keysym);
                m_pEvent->VirtualKeycodeState[inlKeysym] = 0;

                m_pEvent->e_key_modifiers = GetModifierKeyState(xevent.xkey.state);

                m_pEvent->e_key_repeat_count = 0;
                m_pEvent->e_keysym = inlKeysym;
                m_pEvent->e_event = INL_KEYUP;
                break;
            }

        case ButtonPress:
            {
                mouse_press(xevent, m_pEvent);
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: ButtonPress event."));
                break;
            }

        case ButtonRelease:
            {
                mouse_release(xevent, m_pEvent);
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: ButtonRelease event."));
                break;
            }            

        case MotionNotify:
            {
                mouse_move(xevent, m_pEvent);
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: MotionNotify event."));
                break;
            }

        // Note: there is no WM_MOUSEENTER. WM_MOUSEENTER is equivalent to WM_MOUSEMOVE after a WM_MOUSELEAVE.
        case LeaveNotify:
            {
                m_pEvent->e_event = INL_WINDOW_MOUSELEAVE;
                m_pEvent->e_x = xevent.xcrossing.x;
                m_pEvent->e_y = xevent.xcrossing.y;
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: LeaveNotify event."));
                break;
            }

        case EnterNotify:
            {
                m_pEvent->e_event = INL_WINDOW_MOUSELEAVE;
                m_pEvent->e_x = xevent.xcrossing.x;
                m_pEvent->e_y = xevent.xcrossing.y;
                //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: EnterNotify event."));
                break;
            }

        case ClientMessage :
            {
                if((xevent.xclient.format == 32) && ((xevent.xclient.data.l[0]) == static_cast<long>(m_WMDeleteWindow)))
                {
                    m_pEvent->e_event = INL_TERMINATE_APP;
                    //nuxDebugMsg(TEXT("[GLWindowImpl::ProcessXEvents]: ClientMessage event: Close Application."));
                }
                break;
            }

        default:
            {
                m_pEvent->e_event = INL_NO_EVENT;
                break;
            }
	}
}

int GLWindowImpl::X11KeySymToINL(int Keysym)
{
    switch(Keysym)
    {
    case XK_Cancel:         return INL_VK_CANCEL;
    case XK_BackSpace:      return INL_VK_BACKSPACE;
    case XK_Tab:            return INL_VK_TAB;
    case XK_Clear:          return INL_VK_CLEAR;
    case XK_Return:         return INL_VK_ENTER;
    case XK_Shift_L:        return INL_VK_SHIFT;
    case XK_Control_L:      return INL_VK_CONTROL;
    case XK_Alt_L:          return INL_VK_MENU;
    case XK_Pause:          return INL_VK_PAUSE;
    case XK_Caps_Lock:      return INL_VK_CAPITAL;
    case XK_Escape:         return INL_VK_ESCAPE;
    case XK_space:          return INL_VK_SPACE;
    case XK_Page_Up:        return INL_VK_PAGE_UP;
    case XK_Page_Down:      return INL_VK_PAGE_DOWN;
    case XK_End:            return INL_VK_END;
    case XK_Home:           return INL_VK_HOME;
    case XK_Left:           return INL_VK_LEFT;
    case XK_Up:             return INL_VK_UP;
    case XK_Right:          return INL_VK_RIGHT;
    case XK_Down:           return INL_VK_DOWN;
    case XK_Select:         return INL_VK_SELECT;
    case XK_Print:          return INL_VK_PRINT;
    case XK_Execute:        return INL_VK_EXECUTE;
    case XK_Insert:         return INL_VK_INSERT;
    case XK_Delete:         return INL_VK_DELETE;
    case XK_Help:           return INL_VK_HELP;
    case XK_0:              return INL_VK_0;
    case XK_1:              return INL_VK_1;
    case XK_2:              return INL_VK_2;
    case XK_3:              return INL_VK_3;
    case XK_4:              return INL_VK_4;
    case XK_5:              return INL_VK_5;
    case XK_6:              return INL_VK_6;
    case XK_7:              return INL_VK_7;
    case XK_8:              return INL_VK_8;
    case XK_9:              return INL_VK_9;
    case XK_A:              return INL_VK_A;
    case XK_B:              return INL_VK_B;
    case XK_C:              return INL_VK_C;
    case XK_D:              return INL_VK_D;
    case XK_E:              return INL_VK_E;
    case XK_F:              return INL_VK_F;
    case XK_G:              return INL_VK_G;
    case XK_H:              return INL_VK_H;
    case XK_I:              return INL_VK_I;
    case XK_J:              return INL_VK_J;
    case XK_K:              return INL_VK_K;
    case XK_L:              return INL_VK_L;
    case XK_M:              return INL_VK_M;
    case XK_N:              return INL_VK_N;
    case XK_O:              return INL_VK_O;
    case XK_P:              return INL_VK_P;
    case XK_Q:              return INL_VK_Q;
    case XK_R:              return INL_VK_R;
    case XK_S:              return INL_VK_S;
    case XK_T:              return INL_VK_T;
    case XK_U:              return INL_VK_U;
    case XK_V:              return INL_VK_V;
    case XK_W:              return INL_VK_W;
    case XK_X:              return INL_VK_X;
    case XK_Y:              return INL_VK_Y;
    case XK_Z:              return INL_VK_Z;
    case XK_Super_L:        return INL_VK_LWIN;
    case XK_Super_R:        return INL_VK_RWIN;
    case XK_KP_0:           return INL_VK_NUMPAD0;
    case XK_KP_1:           return INL_VK_NUMPAD1;
    case XK_KP_2:           return INL_VK_NUMPAD2;
    case XK_KP_3:           return INL_VK_NUMPAD3;
    case XK_KP_4:           return INL_VK_NUMPAD4;
    case XK_KP_5:           return INL_VK_NUMPAD5;
    case XK_KP_6:           return INL_VK_NUMPAD6;
    case XK_KP_7:           return INL_VK_NUMPAD7;
    case XK_KP_8:           return INL_VK_NUMPAD8;
    case XK_KP_9:           return INL_VK_NUMPAD9;
    case XK_KP_Multiply:    return INL_VK_MULTIPLY;
    case XK_KP_Add:         return INL_VK_ADD;
    case XK_KP_Separator:   return INL_VK_SEPARATOR;
    case XK_KP_Subtract:    return INL_VK_SUBTRACT;
    case XK_KP_Decimal:     return INL_VK_DECIMAL;
    case XK_KP_Divide:      return INL_VK_DIVIDE;
    case XK_F1:             return INL_VK_F1;
    case XK_F2:             return INL_VK_F2;
    case XK_F3:             return INL_VK_F3;
    case XK_F4:             return INL_VK_F4;
    case XK_F5:             return INL_VK_F5;
    case XK_F6:             return INL_VK_F6;
    case XK_F7:             return INL_VK_F7;
    case XK_F8:             return INL_VK_F8;
    case XK_F9:             return INL_VK_F9;
    case XK_F10:            return INL_VK_F10;
    case XK_F11:            return INL_VK_F11;
    case XK_F12:            return INL_VK_F12;
    case XK_F13:            return INL_VK_F13;
    case XK_F14:            return INL_VK_F14;
    case XK_F15:            return INL_VK_F15;
    case XK_F16:            return INL_VK_F16;
    case XK_F17:            return INL_VK_F17;
    case XK_F18:            return INL_VK_F18;
    case XK_F19:            return INL_VK_F19;
    case XK_F20:            return INL_VK_F20;
    case XK_F21:            return INL_VK_F21;
    case XK_F22:            return INL_VK_F22;
    case XK_F23:            return INL_VK_F23;
    case XK_F24:            return INL_VK_F24;
    case XK_Num_Lock:       return INL_VK_NUMLOCK;
    case XK_Scroll_Lock:    return INL_VK_SCROLL;
    case XK_Shift_R:        return INL_VK_RSHIFT;
    case XK_Control_R:      return INL_VK_RCONTROL;
    case XK_Alt_R:          return INL_VK_RMENU;
    default:                return 0x0;
        }
};

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::ShowWindow()
{
#if defined(_WIN32)
    ::ShowWindow(hWnd, SW_RESTORE);
#endif
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::HideWindow()
{
#if defined(_WIN32)
    ::ShowWindow(hWnd, SW_MINIMIZE);
#endif
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::EnterMaximizeWindow()
{
#if defined(_WIN32)
    ::ShowWindow(hWnd, SW_MAXIMIZE);
#endif
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::ExitMaximizeWindow()
{
#if defined(_WIN32)
    ::ShowWindow(hWnd, SW_RESTORE);
#endif
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::SetWindowTitle(const TCHAR* Title)
{
#if defined(_WIN32)
    SetWindowText(hWnd, Title);
#endif
}

//---------------------------------------------------------------------------------------------------------
bool GLWindowImpl::HasVSyncSwapControl() const
{ 
    return GetThreadGLDeviceFactory()->SUPPORT_WGL_EXT_SWAP_CONTROL();
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::EnableVSyncSwapControl()
{
#if _WIN32
    if(HasVSyncSwapControl())
    {
        wglSwapIntervalEXT(1);
    }
#endif
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::DisableVSyncSwapControl()
{
#if _WIN32
    if(HasVSyncSwapControl())
    {
        wglSwapIntervalEXT(0);
    }
#endif
}

float GLWindowImpl::GetFrameTime() const
{
    return m_FrameTime;
}

void GLWindowImpl::ResetFrameTime()
{
    m_Timer.Reset();
}

/*
//---------------------------------------------------------------------------------------------------------
bool GLWindowImpl::StartOpenFileDialog(FileDialogOption& fdo)
{
    return Win32OpenFileDialog(GetWindowHandle(), fdo);
}

//---------------------------------------------------------------------------------------------------------
bool GLWindowImpl::StartSaveFileDialog(FileDialogOption& fdo)
{
    return Win32SaveFileDialog(GetWindowHandle(), fdo);
}

//---------------------------------------------------------------------------------------------------------
bool GLWindowImpl::StartColorDialog(ColorDialogOption& cdo)
{
    return Win32ColorDialog(GetWindowHandle(), cdo);
}
*/
//---------------------------------------------------------------------------------------------------------
/*void GLWindowImpl::SetWindowCursor(HCURSOR cursor)
{
    m_Cursor = cursor;
}

//---------------------------------------------------------------------------------------------------------
HCURSOR GLWindowImpl::GetWindowCursor() const
{
    return m_Cursor;
}*/

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::PauseThreadGraphicsRendering()
{
    m_PauseGraphicsRendering = true;
    MakeGLContextCurrent();
}

//---------------------------------------------------------------------------------------------------------
bool GLWindowImpl::IsPauseThreadGraphicsRendering() const
{
    return m_PauseGraphicsRendering;
}

NAMESPACE_END_OGL
