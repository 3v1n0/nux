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


#ifndef GFXSERVER_H
#define GFXSERVER_H

#if defined(INL_OS_WINDOWS)
    #include "NuxGraphics/Gfx_OpenGL.h"
#elif defined(INL_OS_LINUX)
    #include "NuxGraphics/GfxSetupX11.h"
#endif

#include "AbstractThread.h"

NAMESPACE_BEGIN_GUI

class WindowThread;
class SystemThread;
//class BasePainter;
class StackManagerImpl;
class TimerHandler;
//class Theme;

class GfxServerImpl
{
public:
    // The main function to start the program
    int Run();
	
    bool RegisterRunningApplication(NThread* app);
    
    // Create a main graphics thread. This thread has a window and no parent window (The Parent parameter should always be null).
    WindowThread* CreateGUIThread(const TCHAR* WindowTitle,
        UINT width,
        UINT height,
        WindowThread *Parent = NULL,
        ThreadUserInitFunc UserInitFunc = NULL,
        void* InitData = NULL);

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

    void SetCurrentApplication(WindowThread* App);
    ThreadState GetThreadState(unsigned int ThreadID);

    static GfxServerImpl& Instance();
private:
    WindowThread* _CreateModalWindowThread(WindowStyle WndStyle,
        const TCHAR* WindowTitle,
        UINT width,
        UINT height,
        WindowThread *Parent,
        ThreadUserInitFunc UserInitFunc,
        void* InitData, bool Modal);

    static GfxServerImpl* m_pInstance;
    std::vector<NThread*> ApplicationArray;

    GfxServerImpl();
    GfxServerImpl(const GfxServerImpl&);
    // Does not make sense for a singleton. This is a self assignment.
    GfxServerImpl& operator=(const GfxServerImpl&);
    // Declare operator adress-of as private 
    GfxServerImpl* operator &();
    ~GfxServerImpl();
};

#define gGfxServer nux::GfxServerImpl::Instance()

inlDeclareThreadLocalStorage(NThread*, 0, ThreadLocal_InalogicAppImpl);

GLWindowImpl& GetWindow();
GraphicsContext& GetGraphicsContext();

NThread* GetThreadApplication();
WindowThread* GetGraphicsThread();
StackManagerImpl& GetThreadStackManager();
BasePainter& GetThreadPainter();
#define  gPainter nux::GetThreadPainter()
UXTheme& GetThreadTheme();
#define  gTheme nux::GetThreadTheme()

TimerHandler& GetThreadTimer();


NAMESPACE_END_GUI

#endif // GFXSERVER_H
