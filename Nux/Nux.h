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


#ifndef NUX_H
#define NUX_H

#include "NuxCore/NKernel.h"

#include "NuxCore/NNamespace.h"
#include "NuxCore/NSystemTypes.h"
#include "NuxCore/Error.h"
#include "NuxCore/FilePath.h"
#include "NuxCore/Color.h"
#include "NuxCore/Rect.h"
#include "NuxCore/Point.h"
#include "NuxCore/Size.h"

#include "NuxCore/SmartPtr/NRefCount.h"
#include "NuxCore/SmartPtr/NSmartPtr.h"

#include "NuxCore/Math/Constants.h"
#include "NuxCore/Math/Vector3.h"
#include "NuxCore/Math/Vector4.h"
#include "NuxCore/Math/Matrix3.h"
#include "NuxCore/Math/Matrix4.h"
#include "NuxCore/Math/Spline.h"

#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>
#include <sigc++/adaptors/bind.h>
#include <sigc++/functors/slot.h>

#include <glib.h>

#define NAMESPACE_BEGIN_GUI  namespace nux {
#define NAMESPACE_END_GUI    };

#define NUX_USE_GLIB_LOOP_ON_WINDOWS
//#define NUX_DISABLE_GLIB_LOOP

#include "Utils.h"
#include "WidgetMetrics.h"
#include "BaseObject.h"
#include "BaseArea.h"
#include "Theme.h"
#include "Painter.h"
#include "InterfaceControl.h"
#include "AbstractThread.h"
#include "WindowThread.h"
#include "SystemThread.h"



NAMESPACE_BEGIN_GUI

class WindowThread;
class SystemThread;
//class BasePainter;
class WindowCompositor;
class TimerHandler;
//class Theme;

void NuxInitialize(const TCHAR* CommandLine);

//! Create a main graphics thread. This thread has a window and no parent window (The Parent parameter should always be null).
WindowThread* CreateGUIThread(const TCHAR* WindowTitle,
    UINT width,
    UINT height,
    WindowThread *Parent = NULL,
    ThreadUserInitFunc UserInitFunc = NULL,
    void* InitData = NULL);

#if defined(NUX_OS_WINDOWS)
    //! Create a main graphics thread. This thread has a window and no parent window.
    WindowThread* CreateFromForeignWindow(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext,
    ThreadUserInitFunc UserInitFunc,
    void* InitData);
#elif defined(NUX_OS_LINUX)
    //! Create a main graphics thread. This thread has a window and no parent window.
    WindowThread* CreateFromForeignWindow(Display *X11Display, Window X11Window, GLXContext OpenGLContext,
    ThreadUserInitFunc UserInitFunc,
    void* InitData);
#endif

// Create a window thread that is a child of the Parent. This thread has a window.
WindowThread* CreateWindowThread(WindowStyle WndStyle,
    const TCHAR* WindowTitle,
    UINT width,
    UINT height,
    WindowThread *Parent,
    ThreadUserInitFunc UserInitFunc = NULL,
    void* InitData = NULL);

// Create a Modal window thread that is a child of the Parent. This thread has a window.
WindowThread* CreateModalWindowThread(WindowStyle WndStyle,
    const TCHAR* WindowTitle,
    UINT width,
    UINT height,
    WindowThread *Parent,
    ThreadUserInitFunc UserInitFunc = NULL,
    void* InitData = NULL);

// Create a simple thread
SystemThread* CreateSimpleThread(AbstractThread *Parent = NULL,
    ThreadUserInitFunc UserInitFunc = NULL, void* InitData = NULL);

ThreadState GetThreadState(unsigned int ThreadID);


GLWindowImpl& GetWindow();
GraphicsContext& GetGraphicsContext();

NThread* GetThreadApplication();
WindowThread* GetGraphicsThread();
WindowCompositor& GetThreadWindowCompositor();
BasePainter& GetThreadPainter();
#define  gPainter nux::GetThreadPainter()
UXTheme& GetThreadTheme();
#define  gTheme nux::GetThreadTheme()

TimerHandler& GetThreadTimer();

inlDeclareThreadLocalStorage(NThread*, 0, ThreadLocal_InalogicAppImpl);

NAMESPACE_END_GUI

#endif // NUX_H
