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
#include "Gfx_Events.h"
#include "IniFile.h"

#include "Gfx_OpenGL.h"

NAMESPACE_BEGIN_OGL

// Compute the frame rate every FRAME_RATE_PERIODE;
#define FRAME_RATE_PERIODE    10

#define INL_MISSING_GL_EXTENSION_MESSAGE_BOX(message) {MessageBox(NULL, TEXT("Missing extension: " #message), TEXT("ERROR"), MB_OK|MB_ICONERROR); exit(-1);}
#define INL_ERROR_EXIT_MESSAGE(message) inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, #message " The program will exit.")); exit(-1);

void ClipOrCenterRectToMonitor(LPRECT prc, t_u32 flags)
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

    //if (flags & MONITOR_WORKAREA)
        rc = mi.rcWork;
//    else
//        rc = mi.rcMonitor;

    //
    // center or clip the passed rect to the monitor rect
    //
    //if (flags & MONITOR_CENTER)
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
HGLRC GLWindowImpl::sMainGLRC = 0;
HDC   GLWindowImpl::sMainDC = 0;

GLWindowImpl::GLWindowImpl()
:   m_pEvent(NULL)
,   m_GfxInterfaceCreated(false)
,   m_fullscreen(false)
,   m_ScreenBitDepth(32)
,   m_num_device_modes(0)
,   m_index_of_current_mode(-1)
,   m_BackupFrameBuffer(true)
,   m_DeviceFactory(0)
,   m_GraphicsContext(0)
// ,   m_GLEWContext(0)
// ,   m_WGLEWContext(0)
,   m_Style(WINDOWSTYLE_NORMAL)
,   m_Cursor(0)
,   m_PauseGraphicsRendering(false)
,   m_ParentWindow(0)
,   m_dwExStyle(0)
,   m_dwStyle(0)
{
    inlSetThreadLocalStorage(ThreadLocal_GLWindowImpl, this);

	m_GfxInterfaceCreated = false;
	m_pEvent = new IEvent();
	GetDisplayInfo();

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
        inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Can't find .ini file.\nThe program will exit."));
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
            inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
                TEXT("Failed to read font search path from .ini file.\nThe program will exit."));
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
            inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
                TEXT("Failed to read texture search path from .ini file.\nThe program will exit."));
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
            inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
                TEXT("Failed to read shader search path from .ini file.\nThe program will exit."));
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

//     INL_SAFE_DELETE( m_WGLEWContext );
//     INL_SAFE_DELETE( m_GLEWContext );

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
        inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Failed to locate resource file %s.\nThe program will exit."), ResourceFileName);
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
        inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Failed to locate ui texture file %s.\nThe program will exit."), ResourceFileName);
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
        inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Failed to locate shader file %s.\nThe program will exit."), ResourceFileName);
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
        inlWin32MessageBox(NULL, TEXT("Error"), MBTYPE_Ok, MBICON_Error, MBMODAL_ApplicationModal, 
            TEXT("Failed to locate font file %s.\nThe program will exit."), ResourceFileName);
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

	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values

	m_GfxInterfaceCreated = false;

	// FIXME : put at the end
	m_ViewportSize.SetWidth(WindowWidth);
	m_ViewportSize.SetHeight(WindowHeight);
	m_WindowSize.SetWidth(WindowWidth);
	m_WindowSize.SetHeight(WindowHeight);

	// end of fixme

	WindowRect.left     = (long)0;
	WindowRect.right    = (long)m_ViewportSize.GetWidth();
	WindowRect.top      = (long)0;	
	WindowRect.bottom   = (long)m_ViewportSize.GetHeight();

	m_fullscreen = FullscreenFlag;								// Set The Global Fullscreen Flag
	m_index_of_current_mode = -1;								// assume -1 if the mode is not fullscreen


	if (m_fullscreen)									        // Attempt Fullscreen Mode?
	{
		// check if resolution is supported
		bool mode_supported = false;
		for(int num_modes = 0 ; num_modes < m_num_gfx_device_modes; num_modes++)
		{
			if((m_gfx_device_modes[num_modes].width == m_ViewportSize.GetWidth()) 
				&& (m_gfx_device_modes[num_modes].height == m_ViewportSize.GetHeight())
				&& (m_gfx_device_modes[num_modes].format == m_ScreenBitDepth))
			{
				mode_supported = true;
				m_index_of_current_mode = num_modes;
				break;
			}
		}

		if(mode_supported == false)
		{
            if(inlWin32MessageBox(NULL, TEXT("Info"), MBTYPE_Ok, MBICON_Information, MBMODAL_ApplicationModal,
                TEXT("The requested fullscreen mode is not supported by your monitor.\nUsing windowed mode instead.")) == MBRES_Yes)
			{
				m_fullscreen = FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
		}

		DEVMODE dmScreenSettings;                                               // Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	                // Makes Sure Memory's Cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);                     // Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= m_ViewportSize.GetWidth();                 // Selected Screen Width
		dmScreenSettings.dmPelsHeight	= m_ViewportSize.GetHeight();                // Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= m_ScreenBitDepth;                              // Selected Bits Per Pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
            if(inlWin32MessageBox(NULL, TEXT("Info"), MBTYPE_Ok, MBICON_Information, MBMODAL_ApplicationModal,
                TEXT("The requested fullscreen mode is not supported by your monitor.\nUsing windowed mode instead.")) == MBRES_Yes)
			{
				m_fullscreen = FALSE;                   // Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR", MB_OK|MB_ICONSTOP);
				exit(1);
			}
		}
	}

    m_dwExStyle = 0;
    m_dwStyle = 0;
	if(m_fullscreen)                                    // Are We Still In Fullscreen Mode?
	{
		m_dwExStyle = WS_EX_APPWINDOW;                    // Window Extended Style
		m_dwStyle = WS_POPUP;                             // Windows Style
		ShowCursor(TRUE);                               // Hide Mouse Pointer
	}
	else
	{
        // Window Extended Style
		m_dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        // Windows Style
        m_dwStyle = WS_OVERLAPPED;    // Creates an overlapped window. An overlapped window has a title bar and a border
	}

    // See Win32 Window Hierarchy and Styles: http://msdn.microsoft.com/en-us/library/ms997562.aspx

    //WS_EX_APPWINDOW       // Forces a top-level window onto the taskbar when the window is visible
    //WS_EX_WINDOWEDGE      // Specifies that a window has a border with a raised edge

    //WS_POPUP      // Creates a pop-up window. This style cannot be used with the WS_CHILD style.
    //WS_SYSMENU    // Creates a window that has a window menu on its title bar. The WS_CAPTION style must also be specified.
    //WS_SIZEBOX    // Creates a window that has a sizing border. Same as the WS_THICKFRAME style.
    //WS_CAPTION    // Creates a window that has a title bar (includes the WS_BORDER style).

    m_Style = Style;
    if(Style == WINDOWSTYLE_TOOL)
    {
        m_dwExStyle = WS_EX_TOOLWINDOW;
        m_dwStyle = WS_CAPTION | WS_SYSMENU;
    }
    else if(Style == WINDOWSTYLE_DIALOG)
    {
        m_dwExStyle = WS_EX_DLGMODALFRAME;
        m_dwStyle = WS_CAPTION | WS_SYSMENU;
    }
    else if(Style == WINDOWSTYLE_NOBORDER)
    {
        m_dwExStyle = WS_EX_TOPMOST | WS_EX_TOOLWINDOW;
        m_dwStyle = WS_POPUP;
    }
    else if(Style == WINDOWSTYLE_PANEL)
    {
        m_dwExStyle = 0;           // Specifies that a window has a border with a raised edge
        m_dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
    }
    else
    {
        // Normal Window: NormalWindow
        m_dwExStyle = WS_EX_APPWINDOW |   // Forces a top-level window onto the taskbar when the window is visible
            WS_EX_WINDOWEDGE;           // Specifies that a window has a border with a raised edge

        m_dwStyle |= WS_CAPTION | // Creates a window that has a title bar.
            WS_SYSMENU |        // Creates a window that has a window menu on its title bar. The WS_CAPTION style must also be specified.
            WS_THICKFRAME |     // Creates a window that has a sizing border.
            WS_MINIMIZEBOX |    // Creates a window that has a minimize button.
            WS_MAXIMIZEBOX |    // Creates a window that has a maximize button.
            WS_BORDER;          // Creates a window that has a thin-line border.
    }

    // The AdjustWindowRectEx function calculates the required size of the window rectangle,
    // based on the desired size of the client rectangle. The window rectangle can then be passed to
    // the CreateWindowEx function to create a window whose client area is the desired size.
	AdjustWindowRectEx(&WindowRect, m_dwStyle, FALSE, m_dwExStyle);     // Adjust Window To True Requested Size

    RECT rect;
    rect.top = 0;
    rect.bottom = WindowRect.bottom-WindowRect.top;
    rect.left = 0;
    rect.right = WindowRect.right-WindowRect.left;
    int WindowX = 0;
    int WindowY = 0;
    if(Parent)
    {
        m_ParentWindow = Parent->GetWindowHandle();
        GetWindowRect(m_ParentWindow, &rect);

        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        WindowX = rect.left + (width - (WindowRect.right-WindowRect.left))/2;
        WindowY = rect.top + (height - (WindowRect.bottom-WindowRect.top))/2;
    }
    else
    {
        ClipOrCenterRectToMonitor(&rect, 0);
        WindowX = rect.left;
        WindowY = rect.top;
    }
    
    m_WindowTitle = WindowTitle;
	// Create The Window
    if (!(m_hWnd = ::CreateWindowEx(m_dwExStyle,                        // Extended Style For The Window
                                WINDOW_CLASS_NAME,                  // Class Name
                                m_WindowTitle.GetTCharPtr(),        // Window Title
                                m_dwStyle |                           // Defined Window Style
                                WS_CLIPSIBLINGS |                   // Required Window Style
                                WS_CLIPCHILDREN,                    // Required Window Style
                                WindowX, WindowY,                   // Window Position
                                WindowRect.right-WindowRect.left,   // Calculate Window Width
                                WindowRect.bottom-WindowRect.top,   // Calculate Window Height
                                m_ParentWindow,                     // No Parent Window
                                NULL,                               // No Menu
                                gGLWindowManager.GetInstance(),     // Instance
                                NULL)))                             // Dont Pass Anything To WM_CREATE
	{
		DestroyOpenGLWindow();
		MessageBox(NULL, TEXT("Window Creation Error."), TEXT("ERROR"), MB_OK|MB_ICONERROR);
		exit(1);                                                    // Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,		                    // Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
        24,	//        cColorBits
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
           //        Specifies the depth of the depth (z-axis) buffer.
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

	if (!(m_hDC = GetDC(m_hWnd)))   // Did We Get A Device Context?
	{
		DestroyOpenGLWindow();
		MessageBox(NULL, TEXT("Can't Create A GL Device Context."), TEXT("ERROR"), MB_OK|MB_ICONERROR);
		return FALSE;
	}

	if (!(m_PixelFormat = ChoosePixelFormat(m_hDC, &pfd)))    // Did Windows Find A Matching Pixel Format?
	{
		DestroyOpenGLWindow();
		MessageBox(NULL, TEXT("Can't Find A Suitable PixelFormat."), TEXT("ERROR"), MB_OK|MB_ICONERROR);
		return FALSE;
	}

	if(!SetPixelFormat(m_hDC, m_PixelFormat, &pfd))           // Are We Able To Set The Pixel Format?
	{
		DestroyOpenGLWindow();
		MessageBox(NULL, TEXT("Can't Set The PixelFormat."), TEXT("ERROR"), MB_OK|MB_ICONERROR);
		return FALSE;
	}

	if (!(m_GLRC = wglCreateContext(m_hDC)))                   // Are We Able To Get A Rendering Context?
	{
		DestroyOpenGLWindow();
		MessageBox(NULL, TEXT("Can't Create A GL Rendering Context."), TEXT("ERROR"), MB_OK|MB_ICONERROR);
		return FALSE;
	}

    if(sMainGLRC == 0)
    {
        sMainGLRC = m_GLRC;
        sMainDC = m_hDC;
    }
    else
    {
//         wglMakeCurrent(m_hDC, 0);
//         // Make the newly created context share it resources with all the other OpenGL context
//         if(wglShareLists(sMainGLRC, m_GLRC) == FALSE)
//         {
//             DWORD err = GetLastError();
//             DestroyOpenGLWindow();
//             MessageBox(NULL, TEXT("Can't share GL context."), TEXT("ERROR"), MB_OK|MB_ICONERROR);
//             return FALSE;
//         }
    }

    // This creates a  warning. It is a 64-bits compatibility issue. 
    // When not in 64-bit you can disable the warning:
    // Project Properties --> C/C++ tab --> General --> Select "NO" for - Detect 64-bit Portability Issues.
    // See also SetWindowLongPtr
    SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (INL_PTRSIZE_LONG) this); 

    //::ShowWindow(m_hWnd,SW_SHOW);						// Show The Window
    ::SetForegroundWindow(m_hWnd);						// Slightly Higher Priority
    ::SetFocus(m_hWnd);									// Sets Keyboard Focus To The Window

	MakeGLContextCurrent();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    SwapBuffer();

	m_GfxInterfaceCreated = true;

    //m_GLEWContext = new GLEWContext();
    //m_WGLEWContext = new WGLEWContext();

    m_DeviceFactory = new GLDeviceFactory(m_ViewportSize.GetWidth(), m_ViewportSize.GetHeight(), BITFMT_R8G8B8A8);
    m_GraphicsContext = new GraphicsContext(*this);

    //EnableVSyncSwapControl();
    //DisableVSyncSwapControl();

	return true;
}

bool GLWindowImpl::CreateFromOpenGLWindow(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext)
{
    // Do not make the opengl context current
    // Do not swap the framebuffer
    // Do not clear the depth or color buffer
    // Do not enable/disable VSync

    m_hWnd = WindowHandle;
    m_hDC = WindowDCHandle;
    m_GLRC = OpenGLRenderingContext;

    RECT rect;
    ::GetClientRect(m_hWnd, &rect);
    m_WindowSize = Size(rect.right - rect.left, rect.bottom - rect.top);
    m_ViewportSize = Size(rect.right - rect.left, rect.bottom - rect.top);

    // The opengl context should be made current by an external entity.

    m_GfxInterfaceCreated = true;
    m_DeviceFactory = new GLDeviceFactory(m_ViewportSize.GetWidth(), m_ViewportSize.GetHeight(), BITFMT_R8G8B8A8);
    m_GraphicsContext = new GraphicsContext(*this);

    return true;
}

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
void GLWindowImpl::SetWindowSize(int width, int height)
{
    RECT window_rect;
    RECT new_rect;
    ::GetWindowRect(m_hWnd, &window_rect);

    new_rect.left = 0; new_rect.right = width;
    new_rect.top = 0; new_rect.bottom = height;
    BOOL b = ::AdjustWindowRectEx(&new_rect, m_dwStyle, FALSE, m_dwExStyle);     // Adjust Window To True Requested Size

    ::MoveWindow(m_hWnd,
        window_rect.left,
        window_rect.top,
        (new_rect.right - new_rect.left),
        (new_rect.bottom - new_rect.top),
        TRUE);
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
    POINT pt;
    ::GetCursorPos(&pt);
    ScreenToClient(m_hWnd, &pt);
    Point point(pt.x, pt.y);
    return point;
}

Point GLWindowImpl::GetMouseWindowCoord()
{
    POINT pt;
    ::GetCursorPos(&pt);
    ::ScreenToClient(m_hWnd, &pt);
    Point point(pt.x, pt.y);
    return point;
}

Point GLWindowImpl::GetWindowCoord()
{
    RECT rect;
    ::GetWindowRect(m_hWnd, &rect);
    Point point(rect.left, rect.top);
    return point;
}

Rect GLWindowImpl::GetWindowGeometry()
{
    RECT rect;
    ::GetClientRect(m_hWnd, &rect);
    Rect geo(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    return geo;
}

Rect GLWindowImpl::GetNCWindowGeometry()
{
    RECT rect;
    ::GetWindowRect(m_hWnd, &rect);
    Rect geo(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
    return geo;
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
void GLWindowImpl::MakeGLContextCurrent(bool b)
{
    HGLRC glrc = m_GLRC;
    if(b == false)
    {
        glrc = 0;
    }

    if(!wglMakeCurrent(m_hDC, glrc))
    {
        NString error = inlGetSystemErrorMessage();
        DestroyOpenGLWindow();
        MessageBox(NULL, TEXT("Can't Activate The GL Rendering Context."), TEXT("ERROR"), MB_OK|MB_ICONERROR);
    }
}
//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::SwapBuffer(bool glswap)
{
    if(IsPauseThreadGraphicsRendering())
        return;

    if(glswap)
    {
	    SwapBuffers(m_hDC);
    }

    m_FrameTime = m_Timer.PassedMilliseconds();

//     if(16.6f - m_FrameTime > 0)
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
//     if(m_FramePeriodeCounter >= FRAME_RATE_PERIODE)
//     {
//         m_FrameRate = m_FramePeriodeCounter * 1000.0f / m_PeriodeTime;
//         m_PeriodeTime = 0.0f;
//         m_FramePeriodeCounter = 0;
//     }
}
//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::DestroyOpenGLWindow()
{
    if(m_GfxInterfaceCreated == true)
    {
	    if (m_fullscreen)										// Are We In Fullscreen Mode?
	    {
		    ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		    ShowCursor(TRUE);								// Show Mouse Pointer
	    }

	    if (m_GLRC)											// Do We Have A Rendering Context?
	    {
		    if (!wglMakeCurrent(m_hDC, NULL))					// Are We Able To Release The DC And RC Contexts?
		    {
			    MessageBox(NULL, TEXT("Release Of DC And RC Failed."), TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
		    }

		    if (!wglDeleteContext(m_GLRC))						// Are We Able To Delete The RC?
		    {
			    MessageBox(NULL, TEXT("Release Rendering Context Failed."), TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
		    }
		    m_GLRC = NULL;										// Set RC To NULL
	    }

	    if (m_hDC && !ReleaseDC(m_hWnd, m_hDC))					// Are We Able To Release The DC
	    {
		    MessageBox(NULL, TEXT("Release Device Context Failed."), TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
		    m_hDC = NULL;										// Set DC To NULL
	    }

        if (m_hWnd && !(::DestroyWindow(m_hWnd)))					// Are We Able To Destroy The Window?
	    {
		    MessageBox(NULL, TEXT("Could Not Release window handle."), TEXT("SHUTDOWN ERROR"), MB_OK | MB_ICONINFORMATION);
		    m_hWnd = NULL;										// Set Window Handle To NULL
	    }
    }
    m_GfxInterfaceCreated = false;
}


// //---------------------------------------------------------------------------------------------------------
// // convert a MSWindows VK_x to an Fltk (X) Keysym:
// // See also the inverse converter in Fl_get_key_win32.cxx
// // This table is in numeric order by VK:
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

 /*
     The extended-key flag indicates whether the keystroke message originated
     from one of the additional keys on the enhanced keyboard. The extended keys
     consist of the ALT and CTRL keys on the right-hand side of the keyboard;
     the INS, DEL, HOME, END, PAGE UP, PAGE DOWN, and arrow keys in the clusters
     to the left of the numeric keypad; the NUM LOCK key; the BREAK (CTRL+PAUSE) key;
     the PRINT SCRN key; and the divide (/) and ENTER keys in the numeric keypad.
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
static int mouse_event(HWND window, IEvent* m_pEvent, int what, int button,
                       WPARAM wParam, LPARAM lParam)
{
    static int px, py, pmx, pmy;
    m_pEvent->e_x = (signed short)LOWORD(lParam);
    m_pEvent->e_y = (signed short)HIWORD(lParam);
    m_pEvent->e_x_root = 0;
    m_pEvent->e_y_root = 0;

    POINT EventScreenPosition;

    ClientToScreen(window, &EventScreenPosition);
    EventScreenPosition.x = m_pEvent->e_x;
    EventScreenPosition.y = m_pEvent->e_y;
    POINT WindowScreenPosition;
    WindowScreenPosition.x = WindowScreenPosition.y = 0;
    ClientToScreen(window, &WindowScreenPosition);

    // Erase mouse event and mouse doubleclick events. Keep the mouse states.
    ulong _mouse_state = m_pEvent->e_mouse_state & 0x0F000000;

    // establish cause of the event
//     if(button == 1)
//         _mouse_state |= INL_EVENT_BUTTON1;
//     else if(button == 2)
//         _mouse_state |= INL_EVENT_BUTTON2;
//     else if(button == 3)
//         _mouse_state |= INL_EVENT_BUTTON3;
//     else 
        if(button == 4)
    {
        m_pEvent->e_mouse_state |= INL_EVENT_MOUSEWHEEL;
        m_pEvent->e_event = INL_MOUSEWHEEL;

        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        int xPos = (int)(short)LOWORD(lParam) - WindowScreenPosition.x; 
        int yPos = (int)(short)HIWORD(lParam) - WindowScreenPosition.y;
        m_pEvent->e_x = xPos;
        m_pEvent->e_y = yPos;

        m_pEvent->e_wheeldelta = zDelta;
        return 1;
    }

    // set the cause of the event
    switch(what)
    {
    case 0:
        {
            // mouse down
            if(button == 1)
            {
                _mouse_state |= INL_STATE_BUTTON1_DOWN;
                _mouse_state |= INL_EVENT_BUTTON1_DOWN;
            }
            else if(button == 2)
            {
                _mouse_state |= INL_STATE_BUTTON2_DOWN;
                _mouse_state |= INL_EVENT_BUTTON2_DOWN;
            }
            else if(button == 3)
            {
                _mouse_state |= INL_STATE_BUTTON3_DOWN;
                _mouse_state |= INL_EVENT_BUTTON3_DOWN;
            }
        }
        break;

    case 1:
        {
            // double click
            if(button == 1)
                _mouse_state |= INL_EVENT_BUTTON1_DBLCLICK;
            else if(button == 2)
                _mouse_state |= INL_EVENT_BUTTON2_DBLCLICK;
            else if(button == 3)
                _mouse_state |= INL_EVENT_BUTTON3_DBLCLICK;
        }
        break;

    case 2:
        {
            // button up
            if(button == 1)
            {
                _mouse_state &= ~INL_STATE_BUTTON1_DOWN;
                _mouse_state |= INL_EVENT_BUTTON1_UP;
            }
            else if(button == 2)
            {
                _mouse_state &= ~INL_STATE_BUTTON2_DOWN;
                _mouse_state |= INL_EVENT_BUTTON2_UP;
            }
            else if(button == 3)
            {
                _mouse_state &= ~INL_STATE_BUTTON3_DOWN;
                _mouse_state |= INL_EVENT_BUTTON3_UP;
            }
        }
        break;
    }

    m_pEvent->e_mouse_state = _mouse_state;

    switch (what)
    {
    case 1: // double-click
        if(m_pEvent->e_is_click)
        {
            m_pEvent->e_clicks++;
            goto J1;
        }
    case 0: // single-click
        m_pEvent->e_clicks = 0;
    J1:
        // The SetCapture function sets the mouse capture to the specified window belonging to
        // the current thread. SetCapture captures mouse input either when the mouse is over the
        // capturing window, or when the mouse button was pressed while the mouse was over the
        // capturing window and the button is still down. Only one window at a time can capture the mouse.
        SetCapture(window);
        m_pEvent->e_is_click = 1;
        px = pmx = m_pEvent->e_x;
        py = pmy = m_pEvent->e_y;
        m_pEvent->e_event = INL_MOUSE_PRESSED;
        return 1;

    case 2: // release:
        // The ReleaseCapture function releases the mouse capture from a window in the current thread
        // and restores normal mouse input processing. A window that has captured the mouse receives all
        // mouse input, regardless of the position of the cursor, except when a mouse button is clicked
        // while the cursor hot spot is in the window of another thread. 
        ReleaseCapture();
        m_pEvent->e_event = INL_MOUSE_RELEASED;
        return 1;

    case 3: // move:
    default: // avoid compiler warning
        // MSWindows produces extra events even if mouse does not move, ignore them.
        // http://blogs.msdn.com/oldnewthing/archive/2003/10/01/55108.aspx: Why do I get spurious WM_MOUSEMOVE messages?
        if (m_pEvent->e_x == pmx && m_pEvent->e_y == pmy)
            return 1;
        pmx = m_pEvent->e_x; 
        pmy = m_pEvent->e_y;
//        if(abs(m_pEvent->e_x - px)>5 || abs(m_pEvent->e_y - py)>5)
//            m_pEvent->e_is_click = 0;
        m_pEvent->e_event = INL_MOUSE_MOVE;
        return 1;
    }
    return 0;
}

unsigned int GetModifierKeyState()
{
    unsigned int state = 0;
    unsigned short r = 0;
    
    // For CapsLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the CapsLock: on (keyboard light is on) or off?
    r = GetKeyState(VK_CAPITAL);
    if (r & 1)
        state |= INL_STATE_CAPS_LOCK;

    // For NumLock, we don't want to know if the key is pressed Down or Up.
    // We really want to know the state of the the NumLock: on (keyboard light is on) or off?
    r = GetKeyState(VK_NUMLOCK);
    if (r & 1)
        state |= INL_STATE_NUMLOCK;

    r = GetKeyState(VK_SCROLL);
    if (r & 0x8000) 
        state |= INL_STATE_SCROLLLOCK;

    r = GetKeyState(VK_CONTROL);
    if (r & 0x8000) 
        state |= INL_STATE_CTRL;

    r = GetKeyState(VK_SHIFT);
    if (r & 0x8000) 
        state |= INL_STATE_SHIFT;

    r = GetKeyState(VK_MENU);
    if (r & 0x8000)
        state |= INL_STATE_ALT;



    if ((GetKeyState(VK_LWIN)|GetKeyState(VK_RWIN))&~1) 
    {
        // WIN32 bug?  GetKeyState returns garbage if the user hit the
        // meta key to pop up start menu.  Sigh.
        if ((GetAsyncKeyState(VK_LWIN)|GetAsyncKeyState(VK_RWIN))&~1)
            state |= INL_STATE_META;
    }

    return state;
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::GetSystemEvent(IEvent *evt)
{
    MSG		msg;
    m_pEvent->Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    m_pEvent->e_mouse_state &= 0x0F000000;

    // Always set the second parameter of PeekMessage to NULL. Indeed, many services creates
    // windows on the program behalf. If pass the main window as filter, we will miss all the
    // messages from the other windows.
    // Same with GetMessage.
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	// Is There A Message Waiting? If yes, remove it.
    {
        if(msg.message == INL_THREADMSG_START_RENDERING)
        {
            m_PauseGraphicsRendering = false;
            MakeGLContextCurrent(true);
        }

        TranslateMessage(&msg);			            // Translate The Message
        DispatchMessage(&msg);	

        memcpy(evt, m_pEvent, sizeof(IEvent));
    }
    else
    {
        memcpy(evt, m_pEvent, sizeof(IEvent));
    }

    if(msg.message == WM_QUIT)
    {
        // Re-post the message that we retrieved so other modal loops will catch it.
        // See [Modality, part 3: The WM_QUIT message] http://blogs.msdn.com/oldnewthing/archive/2005/02/22/378018.aspx
        PostQuitMessage(msg.wParam);

        m_pEvent->e_event = INL_TERMINATE_APP;
        memcpy(evt, m_pEvent, sizeof(IEvent));
    }

    if(msg.message == -1)  // error
    {
        m_pEvent->e_event = INL_NO_EVENT;
        memcpy(evt, m_pEvent, sizeof(IEvent));
    }
} 

void GLWindowImpl::ProcessForeignWin32Event(HWND hWnd, MSG msg, WPARAM wParam, LPARAM lParam, IEvent* event)
{
    m_pEvent->Reset();
    // Erase mouse event and mouse doubleclick states. Keep the mouse states.
    m_pEvent->e_mouse_state &= 0x0F000000;

    // Always set the second parameter of PeekMessage to NULL. Indeed, many services creates
    // windows on the program behalf. If pass the main window as filter, we will miss all the
    // messages from the other windows.
    // Same with GetMessage.
    ProcessWin32Event(hWnd, msg.message, wParam, lParam);
    memcpy(event, m_pEvent, sizeof(IEvent));

    if(msg.message != WM_QUIT)
    {
        // Re-post the message that we retrieved so other modal loops will catch it.
        // See [Modality, part 3: The WM_QUIT message] http://blogs.msdn.com/oldnewthing/archive/2005/02/22/378018.aspx
        PostQuitMessage(msg.wParam);

        m_pEvent->e_event = INL_TERMINATE_APP;
        memcpy(event, m_pEvent, sizeof(IEvent));
    }

    if(msg.message == -1)  // error
    {
        m_pEvent->e_event = INL_NO_EVENT;
        memcpy(event, m_pEvent, sizeof(IEvent));
    }
}

IEvent& GLWindowImpl::GetCurrentEvent()
{
    return *m_pEvent;
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
    GLWindowImpl* GLWindow = reinterpret_cast<GLWindowImpl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

    if(GLWindow == 0)
    {
        switch (uMsg)
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

LRESULT GLWindowImpl::ProcessWin32Event(HWND	hWnd,           // Handle For This Window
                              UINT	uMsg,           // Message For This Window
                              WPARAM	wParam,		// Additional Message Information
                              LPARAM	lParam)		// Additional Message Information
{
	switch (uMsg)
	{
        case WM_DESTROY:
        {
            nuxDebugMsg(TEXT("[GLWindowImpl::WndProc]: Window \"%s\" received WM_DESTROY message."), m_WindowTitle.GetTCharPtr());
            break;
        }

        case WM_CLOSE:
        {
            nuxDebugMsg(TEXT("[GLWindowImpl::WndProc]: Window \"%s\" received WM_CLOSE message."), m_WindowTitle.GetTCharPtr());
            // close? yes or no?
            PostQuitMessage(0);
            return 0;
        }

		case WM_PAINT:
		{
			ValidateRect(hWnd, NULL); //  validate the surface to avoid receiving WM_PAINT continuously
            m_pEvent->e_event = INL_WINDOW_DIRTY;
            break;
		}

		case WM_CAPTURECHANGED:
		{
            // The WM_CAPTURECHANGED message is sent to the window that is losing the mouse capture
            if((HWND)lParam != hWnd)
            {
                // Cancel everything about the mouse state and send a INL_WINDOW_EXIT_FOCUS message.
                m_pEvent->e_mouse_state = 0;
                //nuxDebugMsg(TEXT("Windows Msg: WM_CAPTURECHANGED/INL_WINDOW_EXIT_FOCUS"));
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

            m_pEvent->e_event = INL_SIZE_CONFIGURATION;
            m_pEvent->width =  clientrect.right - clientrect.left;
            m_pEvent->height =  clientrect.bottom - clientrect.top;
            return 0;
        }

        case WM_SIZE:
		{		
			RECT clientrect;
			GetClientRect( hWnd, &clientrect);

			m_pEvent->e_event = INL_NO_EVENT; //INL_SIZE_CONFIGURATION;
			m_pEvent->width =  clientrect.right - clientrect.left;
			m_pEvent->height =  clientrect.bottom - clientrect.top;

			//setViewPort(0, 0, clientrect.right - clientrect.left, clientrect.bottom - clientrect.top);
            m_WindowSize.SetWidth(clientrect.right - clientrect.left);
            m_WindowSize.SetHeight(clientrect.bottom - clientrect.top);

            if((wParam == SIZE_MAXHIDE) || (wParam == SIZE_MINIMIZED))
            {
                m_is_window_minimized = true;
            }
            else
            {
                m_is_window_minimized = false;
            }

            if((wParam == SIZE_MAXIMIZED) || (wParam == SIZE_RESTORED))
            {
                m_pEvent->e_event = INL_SIZE_CONFIGURATION;
            }

			return 0;
		}

        case WM_SETFOCUS:
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
                //nuxDebugMsg(TEXT("Windows Msg: WM_SETFOCUS/INL_WINDOW_ENTER_FOCUS"));
                break;
            }

        case WM_KILLFOCUS:
            {
                m_pEvent->e_event = INL_WINDOW_EXIT_FOCUS;
                m_pEvent->e_mouse_state = 0;

                // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process()
                m_pEvent->e_x = 0xFFFFFFFF;
                m_pEvent->e_y = 0xFFFFFFFF;
                m_pEvent->e_dx = 0;
                m_pEvent->e_dy = 0;
                m_pEvent->virtual_code = 0;
                //nuxDebugMsg(TEXT("Windows Msg: WM_KILLFOCUS/INL_WINDOW_EXIT_FOCUS"));
                break;
            }

        case WM_NCHITTEST:
            {
                break;
            }

        case WM_ACTIVATE:
            {
                if(LOWORD(wParam)!=WA_INACTIVE)
                {
                    m_pEvent->e_key_modifiers = GetModifierKeyState();
                    return 0;
                }
            }

        case WM_ACTIVATEAPP:
            if (wParam)
            {
                m_pEvent->e_key_modifiers = GetModifierKeyState();
                return 0;
            }

        case WM_SYSKEYDOWN:
        case WM_KEYDOWN:
            {
                m_pEvent->e_event = INL_KEYDOWN;
                m_pEvent->e_key_modifiers = GetModifierKeyState();
                m_pEvent->e_keysym = wParam;

                if((uMsg == WM_KEYDOWN) || (uMsg == WM_SYSKEYDOWN))
                {
                    m_pEvent->VirtualKeycodeState[GLWindowImpl::Win32KeySymToINL(wParam)] = 1;
                }
                
                if(wParam == VK_CONTROL)
                {
                    if(lParam & (1 << 24))
                    {
                        m_pEvent->e_keysym = INL_VK_RCONTROL;
                    }
                    else
                    {
                        m_pEvent->e_keysym = INL_VK_LCONTROL;
                    }
                }

                if(wParam == VK_MENU)
                {
                    if(lParam & (1 << 24))
                    {
                        m_pEvent->e_keysym = INL_VK_RMENU;
                    }
                    else
                    {
                        m_pEvent->e_keysym = INL_VK_LMENU;
                    }
                }

                if(wParam == VK_SHIFT)
                {
                    if(HIWORD(GetAsyncKeyState(VK_LSHIFT)))
                    {
                        m_pEvent->e_keysym = INL_VK_LSHIFT;
                    }
                    else if(HIWORD(GetAsyncKeyState(VK_RSHIFT)))
                    {
                        m_pEvent->e_keysym = INL_VK_RSHIFT;
                    }
                }

                break;
            }
            
        case WM_SYSKEYUP:
        case WM_KEYUP:
            {
                m_pEvent->e_event = INL_KEYUP;
                m_pEvent->e_key_modifiers = GetModifierKeyState();
                m_pEvent->e_keysym = wParam;

                if((uMsg == WM_KEYUP) || (uMsg == WM_SYSKEYUP))
                {
                    m_pEvent->VirtualKeycodeState[GLWindowImpl::Win32KeySymToINL(wParam)] = 0;
                }
                break;
            }            

        case WM_DEADCHAR:
        case WM_SYSDEADCHAR:
        case WM_CHAR:
        case WM_SYSCHAR: 
            {
                m_pEvent->e_key_modifiers = GetModifierKeyState();

                // reset key repeat count to 0.
                m_pEvent->e_key_repeat_count = 0;
                if (lParam & (1<<31)) 
                { 
                    // key up events.
                    m_pEvent->e_event = INL_KEYUP;
                    return 0;
                }
                else
                {
                    // key down events.
                    m_pEvent->e_event = INL_KEYDOWN;
                    m_pEvent->e_key_repeat_count = (int)(lParam & 0xff);
                }

                static char buffer[2];
                buffer[0] = 0; buffer[1] = 0;
                if (uMsg == WM_CHAR || uMsg == WM_SYSCHAR) 
                {
                    buffer[0] = char(wParam);
                    m_pEvent->e_length = 1;
                }
                else
                {
                    buffer[0] = 0;
                    m_pEvent->e_length = 0;
                }

                m_pEvent->e_text = buffer;
                return 0;
            }

        case WM_LBUTTONDOWN:  
            {
                mouse_event(hWnd, m_pEvent, 0, 1, wParam, lParam);
                //nuxDebugMsg(TEXT("Windows Msg: WM_LBUTTONDOWN"));
                return 0;
            }
        case WM_LBUTTONDBLCLK:
            {
                mouse_event(hWnd, m_pEvent, 1, 1, wParam, lParam);
                //nuxDebugMsg(TEXT("Windows Msg: WM_LBUTTONDBLCLK"));
                return 0;
            }
        case WM_LBUTTONUP:    
            {
                mouse_event(hWnd, m_pEvent, 2, 1, wParam, lParam);
                //nuxDebugMsg(TEXT("Windows Msg: WM_LBUTTONUP"));
                return 0;
            }
        case WM_MBUTTONDOWN:  
            {mouse_event(hWnd, m_pEvent, 0, 2, wParam, lParam);  break;}
        case WM_MBUTTONDBLCLK:
            {mouse_event(hWnd, m_pEvent, 1, 2, wParam, lParam);  break;}
        case WM_MBUTTONUP:    
            {mouse_event(hWnd, m_pEvent, 2, 2, wParam, lParam);  break;}
        case WM_RBUTTONDOWN:  
            {mouse_event(hWnd, m_pEvent, 0, 3, wParam, lParam);  break;}
        case WM_RBUTTONDBLCLK:
            {mouse_event(hWnd, m_pEvent, 1, 3, wParam, lParam);  break;}
        case WM_RBUTTONUP:    
            {mouse_event(hWnd, m_pEvent, 2, 3, wParam, lParam);  break;}
        case WM_MOUSEWHEEL:    
            {
                mouse_event(hWnd, m_pEvent, 0, 4, wParam, lParam);  break;
            }

        case WM_NCLBUTTONDBLCLK:
            {m_pEvent->e_event = INL_WINDOW_CONFIGURATION; break;}
        case WM_NCLBUTTONDOWN:
            {m_pEvent->e_event = INL_WINDOW_CONFIGURATION; break;}
        case WM_NCLBUTTONUP:
            {m_pEvent->e_event = INL_WINDOW_CONFIGURATION; break;}
        case WM_NCMBUTTONDBLCLK:
            {m_pEvent->e_event = INL_WINDOW_CONFIGURATION; break;}
        case WM_NCMBUTTONDOWN:
            {m_pEvent->e_event = INL_WINDOW_CONFIGURATION; break;}
        case WM_NCMBUTTONUP:
            {m_pEvent->e_event = INL_WINDOW_CONFIGURATION; break;}
        case WM_NCRBUTTONDBLCLK:
            {m_pEvent->e_event = INL_WINDOW_CONFIGURATION; break;}
        case WM_NCRBUTTONDOWN:
            {m_pEvent->e_event = INL_WINDOW_CONFIGURATION; break;}
        case WM_NCRBUTTONUP:
            {m_pEvent->e_event = INL_WINDOW_CONFIGURATION; break;}

        case WM_MOUSEMOVE:
            {
                mouse_event(hWnd, m_pEvent, 3, 0, wParam, lParam);
                //nuxDebugMsg(TEXT("Windows Msg: WM_MOUSEMOVE"));

                TRACKMOUSEEVENT tme = { sizeof(tme) };
                // Enable INL_WINDOW_MOUSELEAVE event.
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
                m_pEvent->e_event = INL_WINDOW_MOUSELEAVE;
                // This causes the mouse to be outside of all widgets when it is tested in m_EventHandler.Process()
                m_pEvent->e_x = 0xFFFFFFFF;
                m_pEvent->e_y = 0xFFFFFFFF;
                //nuxDebugMsg(TEXT("Windows Msg: WM_MOUSELEAVE/INL_WINDOW_MOUSELEAVE"));
                break;
            }

        case WM_SETCURSOR:
            if((LOWORD(lParam) == HTCLIENT) && m_Cursor)
            {
                SetCursor(m_Cursor);
                return TRUE; //return FALSE;
            }
            else
                break;

		case WM_COMMAND:
		{
            nuxDebugMsg(TEXT("[GLWindowImpl::WndProc]: Window \"%s\" received WM_COMMAND message."), m_WindowTitle.GetTCharPtr());
			break;;
		}

        case WM_SYSCOMMAND:                             // Intercept System Commands
            {
                switch (wParam)                         // Check System Calls
                {
                case SC_CLOSE: break;
                case SC_CONTEXTHELP: break;
                case SC_DEFAULT: break;
                case SC_HOTKEY: break;
                case SC_HSCROLL: break;
                case SC_KEYMENU: break;
                case SC_MAXIMIZE: break;
                case SC_MINIMIZE: break;
                case SC_MONITORPOWER: break;
                case SC_MOUSEMENU: break;
                case SC_MOVE: break;
                case SC_NEXTWINDOW: break;
                case SC_PREVWINDOW: break;
                case SC_RESTORE: break;
                case SC_SCREENSAVE: break;
                case SC_SIZE: break;
                case SC_TASKLIST: break;
                case SC_VSCROLL: break;
                }
                break;									// Exit
            }
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int GLWindowImpl::Win32KeySymToINL(int Keysym)
{
    switch(Keysym)
    {
    case VK_CANCEL:         return INL_VK_CANCEL;
    case VK_BACK:           return INL_VK_BACKSPACE;
    case VK_TAB:            return INL_VK_TAB;
    case VK_CLEAR:          return INL_VK_CLEAR;
    case VK_RETURN:         return INL_VK_ENTER;
    case VK_SHIFT:          return INL_VK_SHIFT;
    case VK_CONTROL:        return INL_VK_CONTROL;
    case VK_MENU:           return INL_VK_MENU; // ALT key
    case VK_PAUSE:          return INL_VK_PAUSE;
    case VK_CAPITAL:        return INL_VK_CAPITAL;
    case VK_ESCAPE:         return INL_VK_ESCAPE;
    case VK_SPACE:          return INL_VK_SPACE;
    case VK_PRIOR:          return INL_VK_PAGE_UP;
    case VK_NEXT:           return INL_VK_PAGE_DOWN;
    case VK_END:            return INL_VK_END;
    case VK_HOME:           return INL_VK_HOME;
    case VK_LEFT:           return INL_VK_LEFT;
    case VK_UP:             return INL_VK_UP;
    case VK_RIGHT:          return INL_VK_RIGHT;
    case VK_DOWN:           return INL_VK_DOWN;
    case VK_SELECT:         return INL_VK_SELECT;
    case VK_PRINT:          return INL_VK_PRINT;
    case VK_EXECUTE:        return INL_VK_EXECUTE;
    case VK_INSERT:         return INL_VK_INSERT;
    case VK_DELETE:         return INL_VK_DELETE;
    case VK_HELP:           return INL_VK_HELP;
    case 0x30:              return INL_VK_0;
    case 0x31:              return INL_VK_1;
    case 0x32:              return INL_VK_2;
    case 0x33:              return INL_VK_3;
    case 0x34:              return INL_VK_4;
    case 0x35:              return INL_VK_5;
    case 0x36:              return INL_VK_6;
    case 0x37:              return INL_VK_7;
    case 0x38:              return INL_VK_8;
    case 0x39:              return INL_VK_9;
    case 0x41:              return INL_VK_A;
    case 0x42:              return INL_VK_B;
    case 0x43:              return INL_VK_C;
    case 0x44:              return INL_VK_D;
    case 0x45:              return INL_VK_E;
    case 0x46:              return INL_VK_F;
    case 0x47:              return INL_VK_G;
    case 0x48:              return INL_VK_H;
    case 0x49:              return INL_VK_I;
    case 0x4A:              return INL_VK_J;
    case 0x4B:              return INL_VK_K;
    case 0x4C:              return INL_VK_L;
    case 0x4D:              return INL_VK_M;
    case 0x4E:              return INL_VK_N;
    case 0x4F:              return INL_VK_O;
    case 0x50:              return INL_VK_P;
    case 0x51:              return INL_VK_Q;
    case 0x52:              return INL_VK_R;
    case 0x53:              return INL_VK_S;
    case 0x54:              return INL_VK_T;
    case 0x55:              return INL_VK_U;
    case 0x56:              return INL_VK_V;
    case 0x57:              return INL_VK_W;
    case 0x58:              return INL_VK_X;
    case 0x59:              return INL_VK_Y;
    case 0x5A:              return INL_VK_Z;
    case VK_LWIN:           return INL_VK_LWIN; // Windows key left
    case VK_RWIN:           return INL_VK_RWIN; // Windows key right
    case VK_NUMPAD0:        return INL_VK_NUMPAD0;
    case VK_NUMPAD1:        return INL_VK_NUMPAD1;
    case VK_NUMPAD2:        return INL_VK_NUMPAD2;
    case VK_NUMPAD3:        return INL_VK_NUMPAD3;
    case VK_NUMPAD4:        return INL_VK_NUMPAD4;
    case VK_NUMPAD5:        return INL_VK_NUMPAD5;
    case VK_NUMPAD6:        return INL_VK_NUMPAD6;
    case VK_NUMPAD7:        return INL_VK_NUMPAD7;
    case VK_NUMPAD8:        return INL_VK_NUMPAD8;
    case VK_NUMPAD9:        return INL_VK_NUMPAD9;
    case VK_MULTIPLY:       return INL_VK_MULTIPLY;
    case VK_ADD:            return INL_VK_ADD;
    case VK_SEPARATOR:      return INL_VK_SEPARATOR;
    case VK_SUBTRACT:       return INL_VK_SUBTRACT;
    case VK_DECIMAL:        return INL_VK_DECIMAL;
    case VK_DIVIDE:         return INL_VK_DIVIDE;
    case VK_F1:             return INL_VK_F1;
    case VK_F2:             return INL_VK_F2;
    case VK_F3:             return INL_VK_F3;
    case VK_F4:             return INL_VK_F4;
    case VK_F5:             return INL_VK_F5;
    case VK_F6:             return INL_VK_F6;
    case VK_F7:             return INL_VK_F7;
    case VK_F8:             return INL_VK_F8;
    case VK_F9:             return INL_VK_F9;
    case VK_F10:            return INL_VK_F10;
    case VK_F11:            return INL_VK_F11;
    case VK_F12:            return INL_VK_F12;
    case VK_F13:            return INL_VK_F13;
    case VK_F14:            return INL_VK_F14;
    case VK_F15:            return INL_VK_F15;
    case VK_F16:            return INL_VK_F16;
    case VK_F17:            return INL_VK_F17;
    case VK_F18:            return INL_VK_F18;
    case VK_F19:            return INL_VK_F19;
    case VK_F20:            return INL_VK_F20;
    case VK_F21:            return INL_VK_F21;
    case VK_F22:            return INL_VK_F22;
    case VK_F23:            return INL_VK_F23;
    case VK_F24:            return INL_VK_F24;
    case VK_NUMLOCK:        return INL_VK_NUMLOCK;
    case VK_SCROLL:         return INL_VK_SCROLL;
    case VK_LSHIFT:         return INL_VK_LSHIFT;
    case VK_RSHIFT:         return INL_VK_RSHIFT;
    case VK_LCONTROL:       return INL_VK_LCONTROL;
    case VK_RCONTROL:       return INL_VK_RCONTROL;
    case VK_LMENU:          return INL_VK_LMENU;
    case VK_RMENU:          return INL_VK_RMENU;
    default:                return 0x0;
    }
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::GetDisplayInfo()
{
    DEVMODE   devmode;
    INT         iMode = 0;
    BOOL		bRetVal;
    DeviceModes dm;
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
        }
    }
    while (bRetVal);
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::ShowWindow()
{
    ::ShowWindow(m_hWnd, SW_RESTORE);
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::HideWindow()
{
    ::ShowWindow(m_hWnd, SW_MINIMIZE);
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::EnterMaximizeWindow()
{
    ::ShowWindow(m_hWnd, SW_MAXIMIZE);
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::ExitMaximizeWindow()
{
    ::ShowWindow(m_hWnd, SW_RESTORE);
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::SetWindowTitle(const TCHAR* Title)
{
    SetWindowText(m_hWnd, Title);
}

//---------------------------------------------------------------------------------------------------------
bool GLWindowImpl::HasVSyncSwapControl() const
{ 
    return GetThreadGLDeviceFactory()->SUPPORT_WGL_EXT_SWAP_CONTROL();
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::EnableVSyncSwapControl()
{
#if WIN32
    if(HasVSyncSwapControl())
    {
        wglSwapIntervalEXT(1);
    }
#endif
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::DisableVSyncSwapControl()
{
#if WIN32
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

bool GLWindowImpl::StartOpenFileDialog(FileDialogOption& fdo)
{
    return Win32OpenFileDialog(GetWindowHandle(), fdo);
}

bool GLWindowImpl::StartSaveFileDialog(FileDialogOption& fdo)
{
    return Win32SaveFileDialog(GetWindowHandle(), fdo);
}

bool GLWindowImpl::StartColorDialog(ColorDialogOption& cdo)
{
    return Win32ColorDialog(GetWindowHandle(), cdo);
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::SetWindowCursor(HCURSOR cursor)
{
    m_Cursor = cursor;
}

//---------------------------------------------------------------------------------------------------------
HCURSOR GLWindowImpl::GetWindowCursor() const
{
    return m_Cursor;
}

//---------------------------------------------------------------------------------------------------------
void GLWindowImpl::PauseThreadGraphicsRendering()
{
    m_PauseGraphicsRendering = true;
    MakeGLContextCurrent(false);
}

//---------------------------------------------------------------------------------------------------------
bool GLWindowImpl::IsPauseThreadGraphicsRendering() const
{
    return m_PauseGraphicsRendering;
}

NAMESPACE_END_OGL
