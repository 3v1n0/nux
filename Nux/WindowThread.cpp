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


#include "Nux.h"
#include "Layout.h"
#include "NuxGraphics/OpenGLEngine.h"
#include "ClientArea.h"
#include "WindowCompositor.h"
#include "TimerProc.h"
#include "WindowCompositor.h"
#include "SystemThread.h"
#include "FloatingWindow.h"

#include "WindowThread.h"

NAMESPACE_BEGIN_GUI

// Thread registration call. Hidden from the users. Implemented in Nux.cpp
bool RegisterNuxThread(NThread* ThreadPtr);
void UnregisterNuxThread(NThread* ThreadPtr);

#if (defined(INL_OS_LINUX) || defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(INL_DISABLE_GLIB_LOOP))

static GMutex *gLibEventMutex = 0;
static void
nux_glib_threads_lock (void)
{
    if (gLibEventMutex)
        g_mutex_lock (gLibEventMutex);
}

static void
nux_glib_threads_unlock (void)
{
    if (gLibEventMutex)
        g_mutex_unlock (gLibEventMutex);
}

struct NuxEventSource
{
    GSource source;
    GPollFD event_poll_fd;
};

static gboolean
nux_event_prepare (GSource *source,
                       gint    *timeout)
{
    nux_glib_threads_lock();

    gboolean retval;
    *timeout = -1;
#if defined(INL_OS_WINDOWS)
    MSG msg;
    retval = PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE) ? TRUE : FALSE;
#elif defined(INL_OS_LINUX)
    retval = GetThreadGLWindow()->HasXPendingEvent () ? TRUE : FALSE;
#else
    #error Not implemented.
#endif
  
    nux_glib_threads_unlock();
    return retval;
}

static gboolean
nux_event_check (GSource *source)
{
    nux_glib_threads_lock();
    
    gboolean retval;
    NuxEventSource *event_source = (NuxEventSource *) source;

    if ((event_source->event_poll_fd.revents & G_IO_IN))
    {
#if defined(INL_OS_WINDOWS)
        MSG msg;
        retval = PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE) ? TRUE : FALSE;
#elif defined(INL_OS_LINUX)
        retval = GetThreadGLWindow()->HasXPendingEvent () ? TRUE : FALSE;
#else
        #error Not implemented.
#endif
    }
    else
    retval = FALSE;

    nux_glib_threads_unlock();
    return retval;
}

static gboolean
nux_event_dispatch (GSource     *source,
                    GSourceFunc  callback,
                    gpointer     user_data)
{
    nux_glib_threads_lock();
    WindowThread* window_thread = INL_STATIC_CAST(WindowThread*, user_data);
    t_u32 return_code = window_thread->ExecutionLoop(0);

    if(return_code == 0)
    {
        //g_source_destroy (source);
        //g_source_unref (source);
        g_main_loop_quit(window_thread->m_GLibLoop);
    }
    
    nux_glib_threads_unlock();
    return (return_code != 0) ? TRUE : FALSE;
}

static GSourceFuncs event_funcs = {
    nux_event_prepare,
    nux_event_check,
    nux_event_dispatch,
    NULL
};

void WindowThread::InitGlibLoop()
{
    static bool gthread_initialized = false;
    if(!gthread_initialized)
        g_thread_init(NULL);
    gthread_initialized = true;

    if((m_GLibContext == 0) || (m_GLibLoop == 0))
    {
        //create a context
        m_GLibContext = g_main_context_new();
        //create a main loop with context
        m_GLibLoop = g_main_loop_new(m_GLibContext, TRUE);
    }

    gLibEventMutex = 0; //g_mutex_new();


    GSource *source;
    source = g_source_new (&event_funcs, sizeof (NuxEventSource));
    NuxEventSource *event_source = (NuxEventSource *) source;

    g_source_set_priority (source, G_PRIORITY_DEFAULT);

#if defined(INL_OS_WINDOWS)
    event_source->event_poll_fd.fd = G_WIN32_MSG_HANDLE;
#elif defined(INL_OS_LINUX)
    event_source->event_poll_fd.fd = ConnectionNumber (GetThreadGLWindow()->GetX11Display());
#else
    #error Not implemented.
#endif
    
    event_source->event_poll_fd.events = G_IO_IN;

    g_source_add_poll (source, &event_source->event_poll_fd);
    g_source_set_can_recurse (source, TRUE);
    g_source_set_callback (source, 0, this, 0);
    g_source_attach (source, m_GLibContext);

    g_main_loop_run (m_GLibLoop);
    g_main_loop_unref (m_GLibLoop);

}

typedef struct  
{
    WindowThread* window_thread;
    t_u32 id;
} TimeoutData;

static gboolean nux_timeout_dispatch (gpointer user_data)
{
    TimeoutData* dd = INL_STATIC_CAST(TimeoutData*, user_data);

    dd->window_thread->ExecutionLoop(dd->id);

    //nuxDebugMsg(TEXT("[nux_timeout_dispatch] Timeout ID: %d"), dd->id);
    delete dd;

    return FALSE;
}

t_u32 WindowThread::AddGLibTimeout(t_u32 duration)
{
    if((m_GLibContext == 0) || (m_GLibLoop == 0))
    {
        nuxDebugMsg(TEXT("[WindowThread::AddGLibTimeout] WARNING: Trying to set a timeout before GLib Context is created."));
        return 0;
//         //create a context
//         m_GLibContext = g_main_context_new();
//         //create a main loop with context
//         m_GLibLoop = g_main_loop_new(m_GLibContext, FALSE);
    }

    GSource *timeout_source;
    t_u32 id;

    //create a new time-out source
    timeout_source = g_timeout_source_new(duration);
    //attach source to context
    id = g_source_attach(timeout_source, m_GLibContext);

    TimeoutData* dd = new TimeoutData;
    dd->window_thread = this;
    dd->id = id;

    //set the callback for this source
    g_source_set_callback(timeout_source, nux_timeout_dispatch, dd, NULL);

    return id;
}
#endif

IMPLEMENT_OBJECT_TYPE(WindowThread);

WindowThread::WindowThread(const TCHAR* WindowTitle, unsigned int width, unsigned int height, AbstractThread* Parent, bool Modal)
:   AbstractThread(Parent)
,   m_StartupWidth(width)
,   m_StartupHeight(height)
,   m_WindowTitle(WindowTitle)
,   m_WidgetInitialized(false)
,   m_WindowStyle(WINDOWSTYLE_NORMAL)
{
    // Thread specific objects
    m_GLWindow      = 0;
    m_StackManager  = 0;
    m_Painter       = 0;
    m_TimerHandler  = 0;
    m_Theme         = 0;

    // Protection for ThreadCtor and ThreadDtor;
    m_ThreadCtorCalled = false;
    m_ThreadDtorCalled = false;

    // 
    m_bWaitForModalWindow   = false;
    m_ModalWindowThread     = 0;
    m_bIsModal              = Modal;

    m_IsComputingMainLayout = 0;
    m_RedrawRequested       = 0;
    m_bFirstDrawPass        = true;

    // The layout of the window is null.
    m_FrameRate = 0;
    m_FrameCounter = 0;
    m_FramePeriodeCounter = 0;
    m_PeriodeTime = 0;

#if (defined(INL_OS_LINUX) || defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(INL_DISABLE_GLIB_LOOP))
    m_GLibLoop      = 0;
    m_GLibContext   = 0;
#endif
}

WindowThread::~WindowThread()
{
    ThreadDtor();
}

void WindowThread::SetLayout(smptr(Layout) layout)
{
    m_AppLayout = layout;
    m_AppLayout->SetStretchFactor(1);
    
    int w = m_GLWindow->GetGraphicsContext()->GetContextWidth();
    int h = m_GLWindow->GetGraphicsContext()->GetContextHeight();

    if(m_AppLayout.IsValid())
    {
        SetComputingLayout(true);
        m_AppLayout->setGeometry(0, 0, w, h);
        m_AppLayout->ComputeLayout2();
        m_AppLayout->ComputePosition2(0, 0);
        SetComputingLayout(false);

        EmptyLayoutRefreshList();
    }
}

void WindowThread::ReconfigureLayout()
{
    int w = m_GLWindow->GetGraphicsContext()->GetWindowWidth();
    int h = m_GLWindow->GetGraphicsContext()->GetWindowHeight();

    if(m_AppLayout.IsValid())
    {
        SetComputingLayout(true);
        m_AppLayout->setGeometry(0, 0, w, h);
        m_AppLayout->ComputeLayout2();
        m_AppLayout->ComputePosition2(0, 0);
        SetComputingLayout(false);

        EmptyLayoutRefreshList();
    }
}

long WindowThread::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    if(m_bWaitForModalWindow)
        return TraverseInfo;

    long ret = TraverseInfo;
    if(m_AppLayout.IsValid())
        ret = m_AppLayout->ProcessEvent(ievent, ret, ProcessEventInfo);
    return ret;
}

void WindowThread::ProcessDraw(GraphicsContext& GfxContext, bool force_draw)
{
    if(m_AppLayout.IsValid())
    {
        bool dirty = m_AppLayout->IsDrawDirty();
        if(dirty)
        {
            // A main layout re computation has happen. It was not initiated physically by resizing the window. We need to draw the background to 
            // clean any dirty region.
            int buffer_width = GfxContext.GetWindowWidth();
            int buffer_height = GfxContext.GetWindowHeight();
            gPainter.PaintBackground(GfxContext, Geometry(0, 0, buffer_width, buffer_height));
        }
        m_AppLayout->ProcessDraw(GfxContext, force_draw || dirty);
    }
}

void WindowThread::AddObjectToRefreshList(smptr(BaseObject) bo)
{
    nuxAssert(bo != 0);
    if(bo == 0)
        return;

    std::list<smptr(BaseObject)>::iterator it;
    it = find(m_LayoutRefreshList.begin(), m_LayoutRefreshList.end(), bo);
    if(it == m_LayoutRefreshList.end())
    {
        m_LayoutRefreshList.push_back(bo);
    }
}

void WindowThread::RemoveObjectFromRefreshList(smptr(BaseObject) bo)
{
    std::list<smptr(BaseObject)>::iterator it;
    it = find(m_LayoutRefreshList.begin(), m_LayoutRefreshList.end(), bo);
    if(it != m_LayoutRefreshList.end())
    {
        m_LayoutRefreshList.erase(it);
    }
}

void WindowThread::EmptyLayoutRefreshList()
{
    m_LayoutRefreshList.clear();
}

void WindowThread::AddClientAreaToRedrawList(ClientArea* clientarea)
{
    nuxAssert(clientarea);
    if(clientarea == 0)
        return;

    std::list<ClientArea*>::iterator it;
    it = find(m_ClientAreaList.begin(), m_ClientAreaList.end(), clientarea);
    if(it == m_ClientAreaList.end())
    {
        m_ClientAreaList.push_back(clientarea);
    }
}

void WindowThread::RemoveClientAreaFromRefreshList(ClientArea* clientarea)
{
    std::list<ClientArea*>::iterator it;
    it = find(m_ClientAreaList.begin(), m_ClientAreaList.end(), clientarea);
    if(it != m_ClientAreaList.end())
    {
        m_ClientAreaList.erase(it);
    }
}

void WindowThread::EmptyClientAreaRedrawList()
{
    m_ClientAreaList.clear();
}

bool WindowThread::IsMainLayoutDrawDirty() const
{
    if(m_AppLayout.IsValid() && m_AppLayout->IsDrawDirty())
    {
        return true;
    }
    return false;
}

void WindowThread::RefreshLayout()
{
    SetComputingLayout(true);
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutRefreshList.begin(); it != m_LayoutRefreshList.end(); it++)
    {
        smptr(BaseObject) bo = *it;

        if(bo->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
        {
            smptr(ActiveInterfaceObject) ic  = bo;
            if(!ic->CanBreakLayout())
                ic->NeedRedraw();
        }
        else if(bo->Type().IsDerivedFromType(Layout::StaticObjectType))
        {
            smptr(Layout) layout = bo;
            layout->SetDrawDirty(true);
            layout->SetDirty(true);
        }

        (*it)->ComputeLayout2();
    }
    
    SetComputingLayout(false);
    EmptyLayoutRefreshList();
}

void WindowThread::ComputeElementLayout(smptr(BaseObject) bo, bool RecurseToTopLevelLayout)
{
    nuxAssert(bo != 0);
    if(bo == 0)
        return;

    bool alreadyComputingLayout = IsComputingLayout();

    if((!alreadyComputingLayout) && (!RecurseToTopLevelLayout))
        SetComputingLayout(true);
    if(bo->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
    {
        smptr(ActiveInterfaceObject) ic  = bo;
        ic->NeedRedraw();
    }
    else if(bo->Type().IsDerivedFromType(Layout::StaticObjectType))
    {
        smptr(Layout) layout = bo;
        layout->SetDrawDirty(true);
        layout->SetDirty(true);
    }
    bo->ComputeLayout2();
    if(!alreadyComputingLayout)
        SetComputingLayout(false);
}

unsigned int WindowThread::Run(void* arg)
{
    if(m_UserInitFunc && (m_WidgetInitialized == false))
    {
        (*m_UserInitFunc)(this, m_InitData);
        m_WidgetInitialized = true;
    }
    RunUserInterface();
    return 0;
}

void WindowThread::RunUserInterface()
{
    m_GLWindow->ShowWindow();
    // Called the first time so we can initialize the size of the render targets
    // At this stage, the size of the window is known.
    m_StackManager->FormatRenderTargets(m_GLWindow->GetWindowWidth(), m_GLWindow->GetWindowHeight());

    while(GetThreadState() != THREADSTOP)
    {
        if(GetThreadState() == THREADRUNNING)
        {
#if (defined(INL_OS_LINUX) || defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(INL_DISABLE_GLIB_LOOP))
            InitGlibLoop();
#else
            ExecutionLoop();
#endif
            if(m_Parent)
            {
                if(m_Parent->Type().IsObjectType(SystemThread::StaticObjectType))
                    static_cast<SystemThread*>(m_Parent)->ChildHasFinished(this);
                if(m_Parent->Type().IsObjectType(WindowThread::StaticObjectType))
                    static_cast<WindowThread*>(m_Parent)->ChildHasFinished(this);
                {
                    SetThreadState(THREADSTOP);
                    TerminateAllChildThread();
                }
            }
            else
            {
                SetThreadState(THREADSTOP);
                TerminateAllChildThread();
            }
        }
        else
        {
            // Do not sleep. Just return and let the GLWindowImpl::SwapBuffer do the sleep if necessary.
        }
    }

    return;
}

extern EventToNameStruct EventToName[];

#if (defined(INL_OS_LINUX) || defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(INL_DISABLE_GLIB_LOOP))
t_u32 WindowThread::ExecutionLoop(t_u32 timer_id)
#else
t_u32 WindowThread::ExecutionLoop()
#endif
{
    IEvent event;
    float ms;
    bool KeepRunning = true;

    if(GetWindow().IsPauseThreadGraphicsRendering())
    {
        // Do not sleep. Just return and let the GLWindowImpl::SwapBuffer do the sleep if necessary.
        return 0;
    }
    WindowThread* Application = GetGraphicsThread();

#if (!defined(INL_OS_LINUX) && !defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) || defined(INL_DISABLE_GLIB_LOOP)
    while(KeepRunning)
#endif
    {
        if(Application->m_bFirstDrawPass)
        {
            ms = 0.0f;
        }
        else
        {
            ms = GetWindow().GetFrameTime();
        }

        memset(&event, 0, sizeof(IEvent));
        GetWindow().GetSystemEvent(&event);

        if(event.e_event ==	INL_TERMINATE_APP || (this->GetThreadState() == THREADSTOP))
        {
            KeepRunning = false;
            return 0; //break;
        }

        int w, h;
        // Call gGfx_OpenGL.getWindowSize after the gGfx_OpenGL.get_event.
        // Otherwise, w and h may not be correct for the current frame if a resizing happened.
        GetWindow().GetWindowSize(w, h);

        if(event.e_event == INL_MOUSE_PRESSED ||
            (event.e_event == INL_MOUSE_RELEASED) ||
            (event.e_event == INL_MOUSE_MOVE) ||
            (event.e_event == INL_SIZE_CONFIGURATION) ||
            (event.e_event == INL_KEYDOWN) ||
            (event.e_event == INL_KEYUP) ||
            (event.e_event == INL_WINDOW_CONFIGURATION) ||
            (event.e_event == INL_WINDOW_ENTER_FOCUS) ||
            (event.e_event == INL_WINDOW_EXIT_FOCUS) ||
            (event.e_event == INL_WINDOW_MOUSELEAVE) ||
            (event.e_event == INL_MOUSEWHEEL))
        {
            if((event.e_event == INL_SIZE_CONFIGURATION) ||
                (event.e_event == INL_WINDOW_ENTER_FOCUS) ||
                (event.e_event == INL_WINDOW_EXIT_FOCUS))
            {
                m_StackManager->SetMouseFocusArea(smptr(BaseArea)(0));
                m_StackManager->SetMouseOverArea(smptr(BaseArea)(0));
                m_StackManager->SetPreviousMouseOverArea(smptr(BaseArea)(0));
            }

            //DISPATCH EVENT HERE
            m_StackManager->ClearDrawList();
            m_StackManager->PushEventRectangle(Rect(0, 0, w, h));
            //event.Application = Application;
            m_StackManager->ProcessEvent(event);
            m_StackManager->EmptyEventRegion();
        }

        if(event.e_event == INL_SIZE_CONFIGURATION)
        {
            if(!GetWindow().isWindowMinimized())
            {
                GetWindow().SetViewPort(0, 0, event.width, event.height);
                ReconfigureLayout();
                m_StackManager->FormatRenderTargets(event.width, event.height);
            }
            m_StackManager->FloatingAreaConfigureNotify(event.width, event.height);
        }

        // Some action may have caused layouts and areas to request a recompute. 
        // Process them here before the Draw section.
        if(!GetWindow().isWindowMinimized())
        {
            // Process the layouts that requested a recompute.
            RefreshLayout();
        }

#if (defined(INL_OS_LINUX) || defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(INL_DISABLE_GLIB_LOOP))
        GetThreadTimer().ExecTimerHandler(timer_id);
#else
        GetThreadTimer().ExecTimerHandler();
#endif
        //gTimerManager.Update(ms);

        if(GetWindow().IsPauseThreadGraphicsRendering() == false)
        {
            bool SwapGLBuffer = false;
            if(Application->m_bFirstDrawPass)
            {
                m_StackManager->Draw(event, true);
                Application->m_bFirstDrawPass = false;
            }
            else
            {
                bool b = (event.e_event == INL_MOUSE_PRESSED) ||
                    (event.e_event == INL_MOUSE_RELEASED) ||
                    //(event.e_event == INL_MOUSE_MOVE) ||
                    (event.e_event == INL_SIZE_CONFIGURATION) ||
                    (event.e_event == INL_KEYDOWN) ||
                    (event.e_event == INL_KEYUP) ||
                    (event.e_event == INL_WINDOW_CONFIGURATION) ||
                    (event.e_event == INL_WINDOW_ENTER_FOCUS) ||
                    (event.e_event == INL_WINDOW_EXIT_FOCUS) ||
                    (event.e_event == INL_WINDOW_DIRTY);

                if(b && m_StackManager->IsTooltipActive())
                { 
                    // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
                    m_StackManager->CancelTooltip();
                    b |= true;
                }

                if(!m_StackManager->ValidateMouseInsideTooltipArea(event.e_x, event.e_y) && m_StackManager->IsTooltipActive())
                {
                    // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
                    m_StackManager->CancelTooltip();
                    b |= true;
                }

                int n = (int)m_ClientAreaList.size();

                if( n & (b || IsRedrawNeeded()) )
                {
                    //nuxDebugMsg("Event: %s", (const TCHAR*)EventToName[event.e_event].EventName);
                    m_StackManager->Draw(event, false);
                    SwapGLBuffer = true;
                }
                else if(b || IsRedrawNeeded())
                {
                    //nuxDebugMsg("Event: %s", (const TCHAR*)EventToName[event.e_event].EventName);
                    m_StackManager->Draw(event, false);
                    SwapGLBuffer = true;
                }
                else if(n)
                {
                    //nuxDebugMsg("Event: %s", (const TCHAR*)EventToName[event.e_event].EventName);
                    m_StackManager->Draw(event, false);
                    SwapGLBuffer = true;
                }
                else if(m_StackManager->GetWidgetDrawingOverlay() != 0)
                {
                    //nuxDebugMsg("Event: %s", (const TCHAR*)EventToName[event.e_event].EventName);
                    m_StackManager->Draw(event, false);
                    SwapGLBuffer = false;
                }
            }

            if(SwapGLBuffer)
            {
                // Something was rendered! Swap the rendering buffer!
                GetWindow().SwapBuffer(true);
            }

            float frame_time = GetWindow().GetFrameTime();

#if (!defined(INL_OS_LINUX) && !defined(INL_USE_GLIB_LOOP_ON_WINDOWS)) || defined(INL_DISABLE_GLIB_LOOP)
            // When we are not using the glib loop, we do sleep the thread ourselves if it took less that 16ms to render.
            if(16.6f - frame_time > 0)
            {
                SleepForMilliseconds(16.6f - frame_time);
            }
#endif
            // The frame rate calculation below is only reliable when we are constantly rendering.
            // Otherwise the frame rate drops, which does not mean that we are the performance is bad. 
            // What is happening is that rendering occurs only when there is something to render.
            // If nothing is happening, the application sleeps.
            GetWindow().ResetFrameTime();
            m_PeriodeTime += frame_time;

            m_FrameCounter++;
            m_FramePeriodeCounter++;
            if(m_FramePeriodeCounter >= 10)
            {
                m_FrameRate = m_FramePeriodeCounter * 1000.0f / m_PeriodeTime;
                m_PeriodeTime = 0.0f;
                m_FramePeriodeCounter = 0;
            }

            GetGraphicsThread()->GetGraphicsContext().ResetStats();
            m_ClientAreaList.clear();
            ClearRedrawFlag();
        }
    }

    return 1;
}

unsigned int SpawnThread(NThread& thread)
{
    return 0;
}

void WindowThread::AddChildThread(NThread* window)
{
    nuxAssert(window);
    std::list<NThread*>::iterator it;
    it = find(m_ChildThread.begin(), m_ChildThread.end(), window);
    if(it == m_ChildThread.end())
    {
        m_ChildThread.push_back(window);
    }
}

void WindowThread::RemoveChildThread(NThread* window)
{
    nuxAssert(window);
    std::list<NThread*>::iterator it;
    it = find(m_ChildThread.begin(), m_ChildThread.end(), window);
    if(it != m_ChildThread.end())
    {
        m_ChildThread.erase(it);
    }
}

void WindowThread::ChildHasFinished(NThread* thread)
{
    RemoveChildThread(thread);
    if(thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {           
        SuspendChildGraphics(static_cast<WindowThread*>(thread));
    }
    thread->SetThreadState(THREADSTOP);
}

void WindowThread::TerminateAllChildThread()
{
    std::list<NThread*>::iterator it;
    for(it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
    {
        (*it)->SetThreadState(THREADSTOP);
    }
    m_ChildThread.clear();
}

ThreadState WindowThread::Start( void* arg )
{
    if(!m_Parent)
    {
        return NThread::Start();
    }
    else
    {
        if(m_Parent->Type().IsObjectType(SystemThread::StaticObjectType))
            return static_cast<SystemThread*>(m_Parent)->StartChildThread(this, true);
        else if(m_Parent->Type().IsObjectType(WindowThread::StaticObjectType))
            return static_cast<WindowThread*>(m_Parent)->StartChildThread(this, true);
        
        nuxAssertMsg(0, TEXT("[WindowThread::Start] This should not happen."));
        return THREAD_START_ERROR;
    }
}

ThreadState WindowThread::StartChildThread(NThread* thread, bool Modal)
{
    if(m_bWaitForModalWindow)
    {
        // This window is already waiting for a modal window. It cannot start another windows.
        return thread->GetThreadState();
    }

    ThreadState state = thread->NThread::Start();
    //if(state == THREADRUNNING)
    {
        if(thread->Type().IsObjectType(WindowThread::StaticObjectType))
        {
            // While the child window is being created, the rendering is paused.
            // This is necessary to active OpenGL objects context sharing.
            // Cancel the pause by sending the message INL_THREADMSG_START_RENDERING to this thread.
            //GetWindow().PauseThreadGraphicsRendering();

            if(static_cast<WindowThread*>(thread)->m_bIsModal)
            {
                DisableMouseKeyboardInput();
                m_ModalWindowThread = static_cast<WindowThread*>(thread);
//                 std::list<NThread*>::iterator it;
//                 for(it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
//                 {
//                     static_cast<WindowThread*>(*it)->m_bWaitForModalWindow = true;
//                     // WIN32: Disable Mouse and Keyboard inputs for all windows child of this window
//                     ::EnableWindow(static_cast<WindowThread*>(*it)->m_GLWindow->GetWindowHandle(), FALSE);
//                 }
//                 // WIN32
//                 ::EnableWindow(m_GLWindow->GetWindowHandle(), FALSE);
//                 m_bWaitForModalWindow = true;
            }
            static_cast<WindowThread*>(thread)->m_bWaitForModalWindow = false;
            AddChildThread(thread);
        }
    }
    return state;
}

ThreadState WindowThread::SuspendChildGraphics(WindowThread* thread)
{
    if(m_bWaitForModalWindow)
    {
        if(m_ModalWindowThread != thread)
        {
            nuxAssertMsg(0, TEXT("[WindowThread::SuspendChildGraphics] cannot supend thread that is not the modal window."));
            return thread->GetThreadState();
        }
    }
    ThreadState state = thread->GetThreadState();
    if(m_bWaitForModalWindow)
    {
        m_ModalWindowThread = 0;

        EnableMouseKeyboardInput();
//         std::list<NThread*>::iterator it;
//         for(it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
//         {
//             static_cast<WindowThread*>(*it)->m_bWaitForModalWindow = false;
// 
//             // WIN32
//             ::EnableWindow(static_cast<WindowThread*>(*it)->m_GLWindow->GetWindowHandle(), TRUE);
//         }
    }

    // WIN32
#if defined(INL_OS_WINDOWS)
    ::EnableWindow(m_GLWindow->GetWindowHandle(), TRUE);
#elif defined(INL_OS_LINUX)

#endif
    return state;
}

void WindowThread::EnableMouseKeyboardInput()
{
    std::list<NThread*>::iterator it;
    for(it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
    {
        if(INL_STATIC_CAST(WindowThread*, *it)->Type().IsObjectType(WindowThread::StaticObjectType))
        {
            INL_STATIC_CAST(WindowThread*, *it)->EnableMouseKeyboardInput();
        }
    }
    // WIN32: Enable Mouse and Keyboard inputs for all windows child of this window
#if defined(INL_OS_WINDOWS)
    ::EnableWindow(m_GLWindow->GetWindowHandle(), TRUE);
#elif defined(INL_OS_LINUX)

#endif
    m_bWaitForModalWindow = false;
}

void WindowThread::DisableMouseKeyboardInput()
{
    std::list<NThread*>::iterator it;
    for(it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
    {
        if(INL_STATIC_CAST(WindowThread*, *it)->Type().IsObjectType(WindowThread::StaticObjectType))
        {
            INL_STATIC_CAST(WindowThread*, *it)->DisableMouseKeyboardInput();
        }
    }
    // WIN32: Disable Mouse and Keyboard inputs for all windows child of this window
#if defined(INL_OS_WINDOWS)
    ::EnableWindow(m_GLWindow->GetWindowHandle(), FALSE);
#elif defined(INL_OS_LINUX)

#endif
    m_bWaitForModalWindow = true;
}

void WindowThread::TerminateThread()
{
    SetThreadState(THREADSTOP);
}

bool WindowThread::ThreadCtor()
{
    nuxAssertMsg (m_ThreadCtorCalled == false, TEXT("[WindowThread::ThreadCtor] ThreadCtor should not be called more than once."));
    INL_RETURN_VALUE_IF_TRUE (m_ThreadCtorCalled, true);
    
#if defined(INL_OS_WINDOWS)
    SetWin32ThreadName(GetThreadId(), m_WindowTitle.GetTCharPtr());
#endif

    if(RegisterNuxThread(this) == FALSE)
    {
        nuxDebugMsg(TEXT("[WindowThread::ThreadCtor] Failed to register the WindowThread."));
        return false;
    }

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, this);
    GLWindowImpl* ParentWindow = 0;
    if(m_Parent && static_cast<WindowThread*>(m_Parent)->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        ParentWindow = &static_cast<WindowThread*>(m_Parent)->GetWindow();
    }
    else
    {
        ParentWindow = 0;
    }

    m_GLWindow = gGLWindowManager.CreateGLWindow(m_WindowTitle.GetTCharPtr(), m_StartupWidth, m_StartupHeight, m_WindowStyle, ParentWindow, false);
    if(m_GLWindow == 0)
    {
        nuxDebugMsg(TEXT("[WindowThread::ThreadCtor] Failed to create the window."));
        return false;
    }

    if(m_Parent && m_Parent->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        // Cancel the effect of PauseThreadGraphicsRendering on the parent window.
        //PostThreadMessage(m_Parent->GetThreadId(), INL_THREADMSG_START_RENDERING, (UINT_PTR)((void*)this), 0);
    }

    m_Painter = new BasePainter();
    m_TimerHandler = new TimerHandler();
    m_StackManager = new WindowCompositor;
    m_Theme = new UXTheme();

    SetThreadState(THREADRUNNING);
    m_ThreadCtorCalled = true;

    return true;
}

bool WindowThread::ThreadCtor(Display *X11Display, Window X11Window, GLXContext OpenGLContext)
{
    nuxAssertMsg (m_ThreadCtorCalled == false, TEXT("[WindowThread::ThreadCtor] ThreadCtor should not be called more than once."));
    INL_RETURN_VALUE_IF_TRUE (m_ThreadCtorCalled, true);
    
#if defined(INL_OS_WINDOWS)
    SetWin32ThreadName(GetThreadId(), m_WindowTitle.GetTCharPtr());
#endif

    if(RegisterNuxThread(this) == FALSE)
    {
        nuxDebugMsg(TEXT("[WindowThread::ThreadCtor] Failed to register the WindowThread."));
        return false;
    }

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, this);
    GLWindowImpl* ParentWindow = 0;
    if(m_Parent && static_cast<WindowThread*>(m_Parent)->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        ParentWindow = &static_cast<WindowThread*>(m_Parent)->GetWindow();
    }
    else
    {
        ParentWindow = 0;
    }

    m_GLWindow = gGLWindowManager.CreateFromForeignWindow(X11Display, X11Window, OpenGLContext);
    if(m_GLWindow == 0)
    {
        nuxDebugMsg(TEXT("[WindowThread::ThreadCtor] Failed to create the window."));
        return false;
    }

    if(m_Parent && m_Parent->Type().IsObjectType(WindowThread::StaticObjectType))
    {
        // Cancel the effect of PauseThreadGraphicsRendering on the parent window.
        //PostThreadMessage(m_Parent->GetThreadId(), INL_THREADMSG_START_RENDERING, (UINT_PTR)((void*)this), 0);
    }

    m_Painter = new BasePainter();
    m_TimerHandler = new TimerHandler();
    m_StackManager = new WindowCompositor;
    m_Theme = new UXTheme();

    SetThreadState(THREADRUNNING);
    m_ThreadCtorCalled = true;

    return true;
}

bool WindowThread::ThreadDtor()
{
    INL_RETURN_VALUE_IF_TRUE (m_ThreadDtorCalled, true);

    // Cleanup
    EmptyClientAreaRedrawList();
    EmptyLayoutRefreshList();

    if(m_AppLayout.IsValid())
    {
        m_AppLayout = smptr(Layout)(0);
    }

    INL_SAFE_DELETE(m_StackManager);
    INL_SAFE_DELETE(m_TimerHandler);
    INL_SAFE_DELETE(m_Painter);
    INL_SAFE_DELETE(m_GLWindow);
    INL_SAFE_DELETE(m_Theme);

#if defined(INL_OS_WINDOWS)
    PostThreadMessage(INL_GLOBAL_OBJECT_INSTANCE(NProcess).GetMainThreadID(),
        INL_THREADMSG_THREAD_TERMINATED,
        INL_GLOBAL_OBJECT_INSTANCE(NProcess).GetCurrentThreadID(),
        0);
#elif defined(INL_OS_LINUX)

#else
#error PostThreadMessage not implemented for this platform.
#endif

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, 0);
    UnregisterNuxThread(this);
    m_ThreadDtorCalled = true;
    return true;
}

void WindowThread::SetWindowSize(int width, int height)
{
    if(m_GLWindow)
        m_GLWindow->SetWindowSize(width, height);
}

void WindowThread::SetWindowBackgroundPaintLayer(AbstractPaintLayer* bkg)
{
    if(m_StackManager)
        m_StackManager->SetBackgroundPaintLayer(bkg);
}

float WindowThread::GetFrameRate() const
{
    return m_FrameRate;
}

t_u32 WindowThread::GetFrameCounter() const
{
    return m_FrameCounter;
}

t_u32 WindowThread::GetFramePeriodeCounter() const
{
    return m_FramePeriodeCounter;
}

NAMESPACE_END_GUI