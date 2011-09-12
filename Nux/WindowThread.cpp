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


#include "Nux.h"
#include "Layout.h"
#include "NuxCore/Logger.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "ClientArea.h"
#include "WindowCompositor.h"
#include "TimerProc.h"
#include "SystemThread.h"
#include "FloatingWindow.h"

#include "WindowThread.h"

namespace nux
{
namespace
{
logging::Logger logger("nux.windows.thread");
}

    TimerFunctor *m_ScrollTimerFunctor;
    TimerHandle m_ScrollTimerHandler;  

// Thread registration call. Hidden from the users. Implemented in Nux.cpp
  bool RegisterNuxThread (NThread *ThreadPtr);
  void UnregisterNuxThread (NThread *ThreadPtr);

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))

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
#if defined(NUX_OS_WINDOWS)
    MSG msg;
    retval = PeekMessageW (&msg, NULL, 0, 0, PM_NOREMOVE) ? TRUE : FALSE;
#elif defined(NUX_OS_LINUX)
    retval = GetGraphicsDisplay()->HasXPendingEvent () ? TRUE : FALSE;
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

    if ( (event_source->event_poll_fd.revents & G_IO_IN) )
    {
#if defined(NUX_OS_WINDOWS)
      MSG msg;
      retval = PeekMessageW (&msg, NULL, 0, 0, PM_NOREMOVE) ? TRUE : FALSE;
#elif defined(NUX_OS_LINUX)
      retval = GetGraphicsDisplay()->HasXPendingEvent () ? TRUE : FALSE;
#else
#error Not implemented.
#endif
    }
    else
      retval = FALSE;

    nux_glib_threads_unlock();
    return retval;
  }

  gboolean
  nux_event_dispatch (GSource     *source,
                      GSourceFunc  callback,
                      gpointer     user_data)
  {
    nux_glib_threads_lock();
    WindowThread *window_thread = NUX_STATIC_CAST (WindowThread *, user_data);
    t_u32 return_code = window_thread->ExecutionLoop (0);

    if (return_code == 0 && !window_thread->IsEmbeddedWindow ())
    {
      //g_source_destroy (source);
      //g_source_unref (source);
      g_main_loop_quit (window_thread->m_GLibLoop);
    }

    nux_glib_threads_unlock();
    return return_code || window_thread->IsEmbeddedWindow ();
  }

  static GSourceFuncs event_funcs =
  {
    nux_event_prepare,
    nux_event_check,
    nux_event_dispatch,
    NULL
  };

   // Timeline source functions
  static gboolean
  nux_timeline_prepare  (GSource  *source,
                         gint     *timeout)
  {
    // right now we are assuming that we are vsynced, so that will handle syncronisation
    // we could guess how long we have to wait for the next frame but thats rather ugly
    // idealy we need some more api that ensures that timeline/event/relayout/draws are all in sync

    *timeout = 0;
    return TRUE;
  }

  static gboolean
  nux_timeline_check    (GSource  *source)
  {
    return TRUE;
  }

  static gboolean
  nux_timeline_dispatch (GSource    *source,
                         GSourceFunc callback,
                         gpointer    user_data)
  {
    GTimeVal time_val;
    bool has_timelines_left = false;
    nux_glib_threads_lock();
    g_source_get_current_time (source, &time_val);
    WindowThread *window_thread = NUX_STATIC_CAST (WindowThread *, user_data);

    // pump the timelines
    has_timelines_left = window_thread->ProcessTimelines (&time_val);

    if (!has_timelines_left)
    {
      // no timelines left on the stack so lets go ahead and remove the
      // master clock, to save on wakeups
      window_thread->StopMasterClock ();
    }

    nux_glib_threads_unlock();
    return TRUE;
  }

  static GSourceFuncs timeline_funcs =
  {
    nux_timeline_prepare,
    nux_timeline_check,
    nux_timeline_dispatch,
    NULL
  };

  void WindowThread::InitGlibLoop()
  {
    static bool main_context_created = false;

    GSource *source;

    if (!IsEmbeddedWindow ())
    {
      static bool gthread_initialized = false;

      if (!gthread_initialized)
        g_thread_init (NULL);

      gthread_initialized = true;

      if (((m_GLibContext == 0) || (m_GLibLoop == 0)) && (main_context_created == false))
      {
        //create a context
        m_GLibContext = g_main_context_default ();
        //create a main loop with context
        m_GLibLoop = g_main_loop_new (m_GLibContext, TRUE);
      }
      else
      {
        // Secondary physical windows goes in here
        //create a context
        m_GLibContext = g_main_context_new();
        //create a main loop with context
        m_GLibLoop = g_main_loop_new (m_GLibContext, TRUE);
      }
    }

    main_context_created = true;

    gLibEventMutex = 0; //g_mutex_new();

    source = g_source_new (&event_funcs, sizeof (NuxEventSource) );
    NuxEventSource *event_source = (NuxEventSource *) source;

    g_source_set_priority (source, G_PRIORITY_DEFAULT);

#if defined(NUX_OS_WINDOWS)
    event_source->event_poll_fd.fd = G_WIN32_MSG_HANDLE;
#elif defined(NUX_OS_LINUX)
    event_source->event_poll_fd.fd = ConnectionNumber (GetGraphicsDisplay()->GetX11Display() );
#else
#error Not implemented.
#endif

    event_source->event_poll_fd.events = G_IO_IN;

    g_source_add_poll (source, &event_source->event_poll_fd);
    g_source_set_can_recurse (source, TRUE);
    g_source_set_callback (source, 0, this, 0);
    
    if (IsEmbeddedWindow ())
      g_source_attach (source, NULL);
    else
      g_source_attach (source, m_GLibContext);

    if (_Timelines->size () > 0)
      StartMasterClock ();

    if (!IsEmbeddedWindow ())
    {
      g_main_loop_run (m_GLibLoop);
      g_main_loop_unref (m_GLibLoop);
    }
  }

  void WindowThread::StartMasterClock ()
  {
    // if we are not embedded and don't have a context yet
    if (!IsEmbeddedWindow () && m_GLibContext == 0)
      return;

    if (_MasterClock == NULL)
      {
        GTimeVal time_val;
        // make a source for our master clock
        _MasterClock = g_source_new (&timeline_funcs, sizeof (GSource));

        g_source_set_priority (_MasterClock, G_PRIORITY_DEFAULT + 10);
        g_source_set_callback (_MasterClock, 0, this, 0);
        g_source_set_can_recurse (_MasterClock, TRUE);

        if (IsEmbeddedWindow ())
          g_source_attach (_MasterClock, NULL);
        else if (m_GLibContext != 0)
          g_source_attach (_MasterClock, m_GLibContext);


        g_get_current_time (&time_val);
        _last_timeline_frame_time_sec = time_val.tv_sec;
        _last_timeline_frame_time_usec = time_val.tv_usec;
      }
  }

  void WindowThread::StopMasterClock ()
  {
    if (_MasterClock)
    {
      g_source_remove (g_source_get_id (_MasterClock));
      _MasterClock = NULL;
    }
  }

  void WindowThread::NuxMainLoopQuit ()
  {
    // woo no more main loop! this is prolly bad for nux, so erm
    // FIXME!! - Jay take a look at this, make sure just quitting the mainloop
    // is an idea that makes sense (needed for testing)
    if (!IsEmbeddedWindow ())
      g_main_loop_quit (m_GLibLoop);
  }

  typedef struct
  {
    WindowThread *window_thread;
    t_u32 id;
  } TimeoutData;

  gboolean nux_timeout_dispatch (gpointer user_data)
  {
    bool repeat = false;
    TimeoutData* dd = NUX_STATIC_CAST(TimeoutData*, user_data);

    dd->window_thread->_inside_timer_loop = true;
    repeat = GetTimer().ExecTimerHandler(dd->id)? true : false;
    dd->window_thread->_inside_timer_loop = false;

    if(dd->window_thread->IsEmbeddedWindow())
    {
      dd->window_thread->RedrawRequested.emit();
    }
    else
    {
      dd->window_thread->ExecutionLoop(0);
    }
    
    if(!repeat)
      delete dd;

    return repeat;
  }

  t_u32 WindowThread::AddGLibTimeout(t_u32 duration)
  {
    if(IsEmbeddedWindow())
    {
      TimeoutData* dd = new TimeoutData;
      dd->window_thread = this;
      dd->id = g_timeout_add (duration, nux_timeout_dispatch, dd);

      return dd->id;
    }
    else
    {
      if((m_GLibContext == 0) || (m_GLibLoop == 0))
      {
        LOG_WARNING(logger) << "Trying to set a timeout before GLib Context is created.\n"
                            << logging::backtrace();
        return 0;
      }

      GSource *timeout_source;

      //create a new time-out source
      timeout_source = g_timeout_source_new(duration);

      TimeoutData* dd = new TimeoutData;
      dd->window_thread = this;
      dd->id = 0;
      //set the callback for this source
      g_source_set_callback(timeout_source, nux_timeout_dispatch, dd, NULL);

      //attach source to context
      dd->id = g_source_attach(timeout_source, m_GLibContext);

      return dd->id;
    }
  }
  #endif

  NUX_IMPLEMENT_OBJECT_TYPE (WindowThread);

  WindowThread::WindowThread (const TCHAR *WindowTitle, unsigned int width, unsigned int height, AbstractThread *Parent, bool Modal)
    :   AbstractThread (Parent)
    ,   m_StartupWidth (width)
    ,   m_StartupHeight (height)
    ,   m_WindowTitle (WindowTitle)
    ,   m_WidgetInitialized (false)
    ,   m_WindowStyle (WINDOWSTYLE_NORMAL)
    ,	m_embedded_window (false)
    ,   m_size_configuration_event (false)
    ,   m_force_redraw (false)
  {
    // Thread specific objects
    _graphics_display      = 0;
    m_window_compositor  = 0;
    m_Painter       = 0;
    m_TimerHandler  = 0;
    m_Theme         = 0;
    _main_layout     = 0;
    _queue_main_layout = false;
    // Protection for ThreadCtor and ThreadDtor;
    m_ThreadCtorCalled = false;
    m_ThreadDtorCalled = false;

    //
    m_bWaitForModalWindow   = false;
    m_ModalWindowThread     = 0;
    m_bIsModal              = Modal;

    _inside_layout_cycle = 0;
    _draw_requested_to_host_wm       = false;
    m_bFirstDrawPass        = true;

    // The layout of the window is null.
    m_FrameRate = 0;
    m_FrameCounter = 0;
    m_FramePeriodeCounter = 0;
    m_PeriodeTime = 0;

    _Timelines = new std::list<Timeline*> ();
    GTimeVal time_val;
    g_get_current_time (&time_val);
    _last_timeline_frame_time_sec = time_val.tv_sec;
    _last_timeline_frame_time_usec = time_val.tv_usec;
    _MasterClock = NULL;

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    m_GLibLoop      = 0;
    m_GLibContext   = 0;
#endif
#if defined(NUX_OS_LINUX)
    _x11display = NULL;
    _ownx11display = false;
#endif
    
    _pending_wake_up_timer = false;
    _inside_main_loop = false;
    _inside_timer_loop = false;
    _async_wake_up_functor = new TimerFunctor();
    _async_wake_up_functor->OnTimerExpired.connect (sigc::mem_fun (this, &WindowThread::AsyncWakeUpCallback) );


    _fake_event_call_back = new TimerFunctor();
    _fake_event_call_back->OnTimerExpired.connect (sigc::mem_fun (this, &WindowThread::ReadyFakeEventProcessing));
    _ready_for_next_fake_event = true;
    _fake_event_mode = false;
    _processing_fake_event = false;
    _focused_area = NULL;
  }

  WindowThread::~WindowThread()
  {
    g_source_remove_by_funcs_user_data (&event_funcs, this);

    ThreadDtor();
    std::list<Timeline*>::iterator li;
    for (li=_Timelines->begin (); li!=_Timelines->end (); ++li)
    {
      (*li)->UnReference ();
    }
    
    delete _Timelines;
    delete _async_wake_up_functor;
    delete _fake_event_call_back;

#if defined(NUX_OS_LINUX)
    if (_x11display && _ownx11display)
    {
      XCloseDisplay(_x11display);
    }
#endif
  }

  void WindowThread::SetFocusedArea (Area *focused_area)
  {
    if (focused_area == _focused_area)
      return; 
    
    if (_focused_area != NULL)
    {
      _focused_area->SetFocused (false);

      if (_focused_area_destroyed_con.empty () == false)
      {
        _focused_area_destroyed_con.disconnect ();
      }

    }
    
    _focused_area = focused_area;
    _focused_area_destroyed_con = focused_area->object_destroyed.connect (sigc::mem_fun (this, &WindowThread::OnFocusedAreaDestroyed));
    
  }

  void WindowThread::OnFocusedAreaDestroyed (Object *object)
  {
    if (object == _focused_area)
    {
      _focused_area = NULL;
    }
  }

  void WindowThread::AsyncWakeUpCallback (void* data)
  {
    GetTimer ().RemoveTimerHandler (_async_wake_up_timer);
    _pending_wake_up_timer = false;
  }
  
  void WindowThread::SetFakeEventMode (bool enable)
  {
    _fake_event_mode = enable;
  }
  
  bool WindowThread::InFakeEventMode () const
  {
    return _fake_event_mode;
  }
  
  bool WindowThread::ReadyForNextFakeEvent () const
  {
    return _ready_for_next_fake_event;
  }
  
#if defined (NUX_OS_WINDOWS)
  bool WindowThread::PumpFakeEventIntoPipe (WindowThread* window_thread, INPUT *win_event)
  {
    if (!_fake_event_mode)
    {
      nuxDebugMsg (TEXT("[WindowThread::PumpFakeEventIntoPipe] Cannot register a fake event. Fake event mode is not enabled."));
      return false;
    }
    
    if (!_ready_for_next_fake_event)
    {
      nuxDebugMsg (TEXT("[WindowThread::PumpFakeEventIntoPipe] The fake event pipe is full. Only one fake event can be registered at any time."));
      return false;
    }
    
    _ready_for_next_fake_event = false;
//     _fake_event = *xevent;
//     _fake_event.xany.window = this->GetWindow ().GetWindowHandle ();
    _fake_event_timer = this->GetTimerHandler().AddTimerHandler (0, _fake_event_call_back, this, this);
    return true;
  }
#elif defined (NUX_OS_LINUX)
  bool WindowThread::PumpFakeEventIntoPipe (WindowThread* window_thread, XEvent *xevent)
  {
    if (!_fake_event_mode)
    {
      nuxDebugMsg (TEXT("[WindowThread::PumpFakeEventIntoPipe] Cannot register a fake event. Fake event mode is not enabled."));
      return false;
    }

    if (!_ready_for_next_fake_event)
    {
      nuxDebugMsg (TEXT("[WindowThread::PumpFakeEventIntoPipe] The fake event pipe is full. Only one fake event can be registered at any time."));
      return false;
    }

    _ready_for_next_fake_event = false;
    _fake_event = *xevent;
    _fake_event.xany.window = this->GetWindow ().GetWindowHandle ();
    _fake_event_timer = this->GetTimerHandler().AddTimerHandler (0, _fake_event_call_back, this, this);
    return true;
  }
#endif

  void WindowThread::ReadyFakeEventProcessing (void* data)
  {
    nuxDebugMsg (TEXT("[WindowThread::ReadyFakeEventProcessing] Ready to process fake event."));
    _processing_fake_event = true;
  }
  
  long WindowThread::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    if (m_bWaitForModalWindow)
      return TraverseInfo;

    long ret = TraverseInfo;

    if (_main_layout)
      ret = _main_layout->ProcessEvent (ievent, ret, ProcessEventInfo);

    return ret;
  }

  void WindowThread::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    if (_main_layout)
    {
      bool dirty = _main_layout->IsQueuedForDraw ();

      if (dirty)
      {
        // A main layout re computation has happen. It was not initiated physically by resizing the window. We need to draw the background to
        // clean any dirty region.
        int buffer_width = GfxContext.GetWindowWidth();
        int buffer_height = GfxContext.GetWindowHeight();
        GetPainter().PaintBackground (GfxContext, Geometry (0, 0, buffer_width, buffer_height) );
      }

      _main_layout->ProcessDraw (GfxContext, force_draw || dirty);
    }
  }

  void WindowThread::RequestRedraw()
  {
    _draw_requested_to_host_wm = true;
    RedrawRequested.emit();
    
    if (!IsEmbeddedWindow())
    {
      // If the system is not in embedded mode and an asynchronous request for a Draw is made,
      // and the system is not in a timer processing cycle (always followed by a draw cycle) 
      // or not in the event processing cycle (also followed by a draw cycle), then we set a 0 delay 
      // timer that will wake up the system and initiate a draw cycle.
      if ((_inside_main_loop == false) && (_inside_timer_loop == false) && (_pending_wake_up_timer == false))
      {
        _pending_wake_up_timer = true;
        _async_wake_up_timer = GetTimer().AddTimerHandler (0, _async_wake_up_functor, this);
      }
    }
  }

  void WindowThread::ClearRedrawFlag()
  {
    _draw_requested_to_host_wm = false;
  }

  bool WindowThread::IsRedrawNeeded() const
  {
    return _draw_requested_to_host_wm;
  }

  Layout* WindowThread::GetMainLayout()
  {
    return _main_layout;
  }

  void WindowThread::SetLayout (Layout *layout)
  {
    _main_layout = layout;

    if (_main_layout)
    {
      int w = _graphics_display->GetGraphicsEngine()->GetContextWidth();
      int h = _graphics_display->GetGraphicsEngine()->GetContextHeight();

      _main_layout->Reference();
      _main_layout->SetStretchFactor (1);

      StartLayoutCycle ();
      _main_layout->SetGeometry (0, 0, w, h);
      _main_layout->ComputeLayout2();
      _main_layout->ComputePosition2 (0, 0);
      StopLayoutCycle ();

      RemoveQueuedLayout ();
    }
  }

  void WindowThread::QueueMainLayout ()
  {
    _queue_main_layout = true;
    RequestRedraw ();
  }

  void WindowThread::ReconfigureLayout ()
  {
    int w = _graphics_display->GetGraphicsEngine()->GetWindowWidth();
    int h = _graphics_display->GetGraphicsEngine()->GetWindowHeight();

    if (_main_layout)
    {
      StartLayoutCycle ();
      _main_layout->SetGeometry (0, 0, w, h);
      _main_layout->ComputeLayout2();
      _main_layout->ComputePosition2 (0, 0);
      StopLayoutCycle ();
    }

    RemoveQueuedLayout ();
    _queue_main_layout = false;
  }

  bool WindowThread::QueueObjectLayout (Area *area)
  {
    NUX_RETURN_VALUE_IF_NULL (area, false);

    std::list<Area *>::iterator it;
    it = find (_queued_layout_list.begin(), _queued_layout_list.end(), area);
    if (it == _queued_layout_list.end() )
    {
      _queued_layout_list.push_back (area);
    }

    return true;
  }

  void WindowThread::AddObjectToRefreshList (Area *area)
  {
    QueueObjectLayout (area);
  }

  bool WindowThread::RemoveObjectFromLayoutQueue (Area *area)
  {
    NUX_RETURN_VALUE_IF_NULL (area, false);

    std::list<Area *>::iterator it;
    it = find (_queued_layout_list.begin(), _queued_layout_list.end(), area);

    if (it != _queued_layout_list.end() )
    {
      _queued_layout_list.erase (it);
      return true;
    }
    return false;
  }

  bool WindowThread::RemoveObjectFromRefreshList (Area *area)
  {
    return RemoveObjectFromLayoutQueue (area);
  }

  void WindowThread::RemoveQueuedLayout()
  {
    _queued_layout_list.clear();
  }

  void WindowThread::ComputeQueuedLayout ()
  {
    StartLayoutCycle ();
    std::list<Area *>::iterator it;

    for (it = _queued_layout_list.begin(); it != _queued_layout_list.end(); it++)
    {
      Area *area = *it;

      if (area->Type().IsDerivedFromType (View::StaticObjectType) )
      {
        View *view  = NUX_STATIC_CAST (View *, area);

        if (!view->CanBreakLayout ())
          view->QueueDraw ();
      }
      else if (area->Type().IsDerivedFromType (Layout::StaticObjectType) )
      {
        Layout *layout = NUX_STATIC_CAST (Layout *, area);
        layout->QueueDraw ();
      }
      else
      {
        continue;
      }

      (*it)->ComputeLayout2();
    }

    StopLayoutCycle ();
    
    RemoveQueuedLayout ();
  }

  void WindowThread::RefreshLayout()
  {
    ComputeQueuedLayout ();
  }

  void WindowThread::StartLayoutCycle ()
  {
    _inside_layout_cycle = true;
  }

  void WindowThread::StopLayoutCycle ()
  {
    _inside_layout_cycle = false;
  }


  bool WindowThread::IsInsideLayoutCycle () const
  {
    return _inside_layout_cycle;
  }

  void WindowThread::ComputeElementLayout (Area *area, bool recurse_to_top_level_layout)
  {
    NUX_RETURN_IF_NULL (area);

    bool alreadyComputingLayout = IsInsideLayoutCycle();

    if ((!alreadyComputingLayout) && (!recurse_to_top_level_layout))
    {
      // When computing the layout, setting the size of widgets may cause the system to recurse 
      // upward an look for the up most container which size is affected by its this area.
      // This happens in Area::InitiateResizeLayout ();
      // The search upward is not done if we are already in a layout cycle.
      StartLayoutCycle ();
    }

    if (area->Type().IsDerivedFromType (View::StaticObjectType))
    {
      View *ic = NUX_STATIC_CAST (View *, area);
      ic->QueueDraw ();
    }
    else if (area->Type().IsDerivedFromType (Layout::StaticObjectType))
    {
      Layout *layout = NUX_STATIC_CAST (Layout *, area);
      layout->QueueDraw ();
    }

    area->ComputeLayout2();

    if (!alreadyComputingLayout)
      StopLayoutCycle ();
  }

  void WindowThread::AddTimeline (Timeline *timeline)
  {
    _Timelines->push_back (timeline);
    _Timelines->unique ();
    StartMasterClock ();
  }

  void WindowThread::RemoveTimeline (Timeline *timeline)
  {
    _Timelines->remove (timeline);
    if (_Timelines->size () == 0)
    {
      StopMasterClock ();
    }
  }

  unsigned int WindowThread::Run (void *arg)
  {
    if (m_UserInitFunc && (m_WidgetInitialized == false) )
    {
      (*m_UserInitFunc) (this, m_InitData);
      m_WidgetInitialized = true;
    }

    RunUserInterface();

    return 0;
  }

  void WindowThread::RunUserInterface()
  {
    if (IsEmbeddedWindow ())
    {
      m_window_compositor->FormatRenderTargets (_graphics_display->GetWindowWidth(), _graphics_display->GetWindowHeight() );
      InitGlibLoop ();
      return;
    }
    else
    {
      _graphics_display->ShowWindow();
    }
    // Called the first time so we can initialize the size of the render targets
    // At this stage, the size of the window is known.
    m_window_compositor->FormatRenderTargets (_graphics_display->GetWindowWidth(), _graphics_display->GetWindowHeight() );

    while (GetThreadState() != THREADSTOP)
    {
      if (GetThreadState() == THREADRUNNING)
      {
#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
        InitGlibLoop();
#else
        ExecutionLoop();
#endif

        if (m_Parent)
        {
          if (m_Parent->Type().IsObjectType (SystemThread::StaticObjectType) )
            static_cast<SystemThread *> (m_Parent)->ChildHasFinished (this);

          if (m_Parent->Type().IsObjectType (WindowThread::StaticObjectType) )
            static_cast<WindowThread *> (m_Parent)->ChildHasFinished (this);

          {
            SetThreadState (THREADSTOP);
            TerminateAllChildThread();
          }
        }
        else
        {
          SetThreadState (THREADSTOP);
          TerminateAllChildThread();
        }
      }
      else
      {
        // Do not sleep. Just return and let the GraphicsDisplay::SwapBuffer do the sleep if necessary.
      }
    }

    return;
  }

  extern EventToNameStruct EventToName[];

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
  t_u32 WindowThread::ExecutionLoop (t_u32 timer_id)
#else
  t_u32 WindowThread::ExecutionLoop()
#endif
  {
    IEvent event;

    if (!IsEmbeddedWindow() && GetWindow().IsPauseThreadGraphicsRendering() )
    {
      // Do not sleep. Just return and let the GraphicsDisplay::SwapBuffer do the sleep if necessary.
      return 0;
    }

    WindowThread *Application = GetWindowThread ();

#if (!defined(NUX_OS_LINUX) && !defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) || defined(NUX_DISABLE_GLIB_LOOP)
    while (true)
#endif
    {
      _inside_main_loop = true;
      if(Application->m_bFirstDrawPass)
      {
        GetTimer().StartEarlyTimerObjects ();
      }

      memset(&event, 0, sizeof(IEvent));
      GetWindow().GetSystemEvent(&event);

      if ((event.e_event == NUX_DND_ENTER_WINDOW) ||
        (event.e_event == NUX_DND_LEAVE_WINDOW))
      {
        GetWindowCompositor().ResetDnDArea();
      }

      // Call event inspectors.
      CallEventInspectors (&event);

#if defined(NUX_OS_LINUX)
      // Automation and fake event inputs
      if (_fake_event_mode && _processing_fake_event)
      {
        // Cancel the real X event and inject the fake event instead. This is wrong and should be improved.
        memset(&event, 0, sizeof(IEvent));
        
        GetWindow().InjectXEvent(&event, _fake_event);
        
        if (event.e_event == NUX_MOUSE_PRESSED)
        {
          nuxDebugMsg (TEXT("[WindowThread::ExecutionLoop] Fake Event: Mouse Down."));
        }
        else if (event.e_event == NUX_MOUSE_RELEASED)
        {
          nuxDebugMsg (TEXT("[WindowThread::ExecutionLoop] Fake Event: Mouse Up."));
        }
      }
      else if (_fake_event_mode)
      {
        // In fake event mode we don't allow X mouse up/down events.
        if ((event.e_event == NUX_MOUSE_PRESSED) || (event.e_event == NUX_MOUSE_RELEASED))
        {
          event.e_event = NUX_NO_EVENT;
        }
      }
#endif

      if((event.e_event ==	NUX_TERMINATE_APP) || (this->GetThreadState() == THREADSTOP))
      {
          return 0;
      }
      
      if (IsEmbeddedWindow () && (event.e_event == NUX_SIZE_CONFIGURATION))
        m_size_configuration_event = true;

      int w, h;
      // Call gGfx_OpenGL.getWindowSize after the gGfx_OpenGL.get_event.
      // Otherwise, w and h may not be correct for the current frame if a resizing happened.
      GetWindow().GetWindowSize(w, h);

      if((event.e_event == NUX_MOUSE_PRESSED) ||
          (event.e_event == NUX_MOUSE_RELEASED) ||
          (event.e_event == NUX_MOUSE_DOUBLECLICK) ||
          (event.e_event == NUX_MOUSE_MOVE) ||
          (event.e_event == NUX_SIZE_CONFIGURATION) ||
          (event.e_event == NUX_KEYDOWN) ||
          (event.e_event == NUX_KEYUP) ||
          (event.e_event == NUX_WINDOW_CONFIGURATION) ||
          (event.e_event == NUX_WINDOW_ENTER_FOCUS) ||
          (event.e_event == NUX_WINDOW_EXIT_FOCUS) ||
          (event.e_event == NUX_WINDOW_MOUSELEAVE) ||
          (event.e_event == NUX_DND_MOVE) ||
          (event.e_event == NUX_DND_DROP) ||
          (event.e_event == NUX_DND_ENTER) ||
          (event.e_event == NUX_DND_LEAVE) ||
          (event.e_event == NUX_MOUSE_WHEEL))
      {
          //DISPATCH EVENT HERE
          //event.Application = Application;
          m_window_compositor->ProcessEvent(event);
      }

      if(event.e_event == NUX_SIZE_CONFIGURATION)
      {
          if(!GetWindow().isWindowMinimized())
          {
              GetWindow().SetViewPort(0, 0, event.width, event.height);
              ReconfigureLayout ();
              m_window_compositor->FormatRenderTargets(event.width, event.height);
          }
          m_window_compositor->FloatingAreaConfigureNotify(event.width, event.height);
          m_size_configuration_event = true;
      }

      // Some action may have caused layouts and areas to request a recompute. 
      // Process them here before the Draw section.
      if(!GetWindow().isWindowMinimized() && !IsEmbeddedWindow ())
      {
        if (_queue_main_layout)
        {
          ReconfigureLayout ();
        }
        else 
        {
          // Compute the layouts that have been queued.
          ComputeQueuedLayout ();
        }
      }
      
      _inside_main_loop = false;

// #if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
//       GetTimer().ExecTimerHandler (timer_id);
// #else
//       GetTimer().ExecTimerHandler();
// #endif


      if (!GetWindow().IsPauseThreadGraphicsRendering() || IsEmbeddedWindow ())
      {
        bool SwapGLBuffer = false;
        
        // Warn the host window manager to initiate a draw cycle.
        bool request_draw_cycle_to_host_wm = false;

        if (Application->m_bFirstDrawPass)
        {
          if (IsEmbeddedWindow ())
          {
            request_draw_cycle_to_host_wm = true;
            m_force_redraw = true;
          }
          else
          {
            m_window_compositor->Draw (m_size_configuration_event, true);
          }
          Application->m_bFirstDrawPass = false;
        }
        else
        {
          bool b = (event.e_event == NUX_MOUSE_PRESSED) ||
                   (event.e_event == NUX_MOUSE_RELEASED) ||
                   (event.e_event == NUX_MOUSE_DOUBLECLICK) ||
                   //(event.e_event == NUX_MOUSE_MOVE) ||
                   (event.e_event == NUX_SIZE_CONFIGURATION) ||
                   (event.e_event == NUX_KEYDOWN) ||
                   (event.e_event == NUX_KEYUP) ||
                   (event.e_event == NUX_WINDOW_CONFIGURATION) ||
                   (event.e_event == NUX_WINDOW_ENTER_FOCUS) ||
                   (event.e_event == NUX_WINDOW_EXIT_FOCUS) ||
                   (event.e_event == NUX_WINDOW_DIRTY);

          if (b && m_window_compositor->IsTooltipActive() )
          {
            // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
            m_window_compositor->CancelTooltip();
            b |= true;
          }

          if (!m_window_compositor->ValidateMouseInsideTooltipArea (event.e_x, event.e_y) && m_window_compositor->IsTooltipActive() )
          {
            // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
            m_window_compositor->CancelTooltip();
            b |= true;
          }

          if (b || IsRedrawNeeded())
          {
            if (IsEmbeddedWindow ())
            {
              request_draw_cycle_to_host_wm = true;
            }
            else
            {
              m_window_compositor->Draw (m_size_configuration_event, false);
            }
            SwapGLBuffer = true;
          }
          else if (m_window_compositor->GetWidgetDrawingOverlay() != 0)
          {
            if (IsEmbeddedWindow ())
            {
              request_draw_cycle_to_host_wm = true;
            }
            else
            {
              m_window_compositor->Draw (m_size_configuration_event, false);
            }
            SwapGLBuffer = false;
          }

        }

        if (!IsEmbeddedWindow ())
        {
          if (SwapGLBuffer)
          {
            // Something was rendered! Swap the rendering buffer!
            GetWindow().SwapBuffer (true);
          }

          float frame_time = GetWindow().GetFrameTime();

#if (!defined(NUX_OS_LINUX) && !defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) || defined(NUX_DISABLE_GLIB_LOOP)

          // When we are not using the glib loop, we do sleep the thread ourselves if it took less that 16ms to render.
          if (16.6f - frame_time > 0)
          {
            SleepForMilliseconds (16.6f - frame_time);
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

          if (m_FramePeriodeCounter >= 10)
          {
            m_FrameRate = m_FramePeriodeCounter * 1000.0f / m_PeriodeTime;
            m_PeriodeTime = 0.0f;
            m_FramePeriodeCounter = 0;
          }

          ClearRedrawFlag ();
          GetWindowThread ()->GetGraphicsEngine().ResetStats();
        }
        else if (IsEmbeddedWindow () && (_draw_requested_to_host_wm == false) && request_draw_cycle_to_host_wm)
        {
          RequestRedraw ();
        }
        m_size_configuration_event = false;
      }
    }

#if defined(NUX_OS_LINUX)
      // Automation and fake event inputs
      if (_processing_fake_event)
      {
        _processing_fake_event = false;
        _ready_for_next_fake_event = true;
      }
#endif

    return 1;
  }

  unsigned int SpawnThread (NThread &thread)
  {
    return 0;
  }

  void WindowThread::AddChildThread (NThread *window)
  {
    nuxAssert (window);
    std::list<NThread *>::iterator it;
    it = find (m_ChildThread.begin(), m_ChildThread.end(), window);

    if (it == m_ChildThread.end() )
    {
      m_ChildThread.push_back (window);
    }
  }

  void WindowThread::RemoveChildThread (NThread *window)
  {
    nuxAssert (window);
    std::list<NThread *>::iterator it;
    it = find (m_ChildThread.begin(), m_ChildThread.end(), window);

    if (it != m_ChildThread.end() )
    {
      m_ChildThread.erase (it);
    }
  }

  void WindowThread::ChildHasFinished (NThread *thread)
  {
    RemoveChildThread (thread);

    if (thread->Type().IsObjectType (WindowThread::StaticObjectType) )
    {
      SuspendChildGraphics (static_cast<WindowThread *> (thread) );
    }

    thread->SetThreadState (THREADSTOP);
  }

  void WindowThread::TerminateAllChildThread()
  {
    std::list<NThread *>::iterator it;

    for (it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
    {
      (*it)->SetThreadState (THREADSTOP);
    }

    m_ChildThread.clear();
  }

  ThreadState WindowThread::Start ( void *arg )
  {
    if (!m_Parent)
    {
      return NThread::Start();
    }
    else
    {
      if (m_Parent->Type().IsObjectType (SystemThread::StaticObjectType) )
        return static_cast<SystemThread *> (m_Parent)->StartChildThread (this, true);
      else if (m_Parent->Type().IsObjectType (WindowThread::StaticObjectType) )
        return static_cast<WindowThread *> (m_Parent)->StartChildThread (this, true);

      nuxAssertMsg (0, TEXT ("[WindowThread::Start] This should not happen.") );
      return THREAD_START_ERROR;
    }
  }

  ThreadState WindowThread::StartChildThread (NThread *thread, bool Modal)
  {
    if (m_bWaitForModalWindow)
    {
      // This window is already waiting for a modal window. It cannot start another windows.
      return thread->GetThreadState();
    }

    ThreadState state = thread->NThread::Start();
    //if(state == THREADRUNNING)
    {
      if (thread->Type().IsObjectType (WindowThread::StaticObjectType) )
      {
        // While the child window is being created, the rendering is paused.
        // This is necessary to active OpenGL objects context sharing.
        // Cancel the pause by sending the message NUX_THREADMSG_START_RENDERING to this thread.
        //GetWindow().PauseThreadGraphicsRendering();

        if (static_cast<WindowThread *> (thread)->m_bIsModal)
        {
          DisableMouseKeyboardInput();
          m_ModalWindowThread = static_cast<WindowThread *> (thread);
//                 std::list<NThread*>::iterator it;
//                 for(it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
//                 {
//                     static_cast<WindowThread*>(*it)->m_bWaitForModalWindow = true;
//                     // WIN32: Disable Mouse and Keyboard inputs for all windows child of this window
//                     ::EnableWindow(static_cast<WindowThread*>(*it)->_graphics_display->GetWindowHandle(), FALSE);
//                 }
//                 // WIN32
//                 ::EnableWindow(_graphics_display->GetWindowHandle(), FALSE);
//                 m_bWaitForModalWindow = true;
        }

        static_cast<WindowThread *> (thread)->m_bWaitForModalWindow = false;

        AddChildThread (thread);
      }
    }
    return state;
  }

  ThreadState WindowThread::SuspendChildGraphics (WindowThread *thread)
  {
    if (m_bWaitForModalWindow)
    {
      if (m_ModalWindowThread != thread)
      {
        nuxAssertMsg (0, TEXT ("[WindowThread::SuspendChildGraphics] cannot supend thread that is not the modal window.") );
        return thread->GetThreadState();
      }
    }

    ThreadState state = thread->GetThreadState();

    if (m_bWaitForModalWindow)
    {
      m_ModalWindowThread = 0;

      EnableMouseKeyboardInput();
//         std::list<NThread*>::iterator it;
//         for(it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
//         {
//             static_cast<WindowThread*>(*it)->m_bWaitForModalWindow = false;
//
//             // WIN32
//             ::EnableWindow(static_cast<WindowThread*>(*it)->_graphics_display->GetWindowHandle(), TRUE);
//         }
    }

    // WIN32
#if defined(NUX_OS_WINDOWS)
    ::EnableWindow (_graphics_display->GetWindowHandle(), TRUE);
#elif defined(NUX_OS_LINUX)

#endif
    return state;
  }

  bool WindowThread::ProcessTimelines (GTimeVal *frame_time)
  {
    // go through our timelines and tick them
    // return true if we still have active timelines

    long msecs;
    msecs = (frame_time->tv_sec - _last_timeline_frame_time_sec) * 1000 +
            (frame_time->tv_usec - _last_timeline_frame_time_usec) / 1000;

    if (msecs < 0)
    {
      _last_timeline_frame_time_sec = frame_time->tv_sec;
      _last_timeline_frame_time_usec = frame_time->tv_usec;
      return true;
    }

    if (msecs > 0)
    {
      _last_timeline_frame_time_sec += msecs / 1000;
      _last_timeline_frame_time_usec += msecs * 1000;
    }

    std::list<Timeline*>::iterator li;
    std::list<Timeline*> timelines_copy;

    for (li=_Timelines->begin (); li!=_Timelines->end (); ++li)
    {
      (*li)->Reference ();
      timelines_copy.push_back ((*li));
    }

  	for(li=timelines_copy.begin(); li!=timelines_copy.end(); ++li)
    {
      (*li)->DoTick (msecs);
    }

    // unreference again
    for (li=timelines_copy.begin (); li!=timelines_copy.end (); ++li)
      (*li)->UnReference ();

    // return if we have any timelines left
    return (_Timelines->size () != 0);
  }

  void WindowThread::EnableMouseKeyboardInput()
  {
    std::list<NThread *>::iterator it;

    for (it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
    {
      if (NUX_STATIC_CAST (WindowThread *, *it)->Type().IsObjectType (WindowThread::StaticObjectType) )
      {
        NUX_STATIC_CAST (WindowThread *, *it)->EnableMouseKeyboardInput();
      }
    }

    // WIN32: Enable Mouse and Keyboard inputs for all windows child of this window
#if defined(NUX_OS_WINDOWS)
    ::EnableWindow (_graphics_display->GetWindowHandle(), TRUE);
#elif defined(NUX_OS_LINUX)

#endif
    m_bWaitForModalWindow = false;
  }

  void WindowThread::DisableMouseKeyboardInput()
  {
    std::list<NThread *>::iterator it;

    for (it = m_ChildThread.begin(); it != m_ChildThread.end(); it++)
    {
      if (NUX_STATIC_CAST (WindowThread *, *it)->Type().IsObjectType (WindowThread::StaticObjectType) )
      {
        NUX_STATIC_CAST (WindowThread *, *it)->DisableMouseKeyboardInput();
      }
    }

    // WIN32: Disable Mouse and Keyboard inputs for all windows child of this window
#if defined(NUX_OS_WINDOWS)
    ::EnableWindow (_graphics_display->GetWindowHandle(), FALSE);
#elif defined(NUX_OS_LINUX)

#endif
    m_bWaitForModalWindow = true;
  }

  void WindowThread::TerminateThread()
  {
    SetThreadState (THREADSTOP);
  }

  bool WindowThread::ThreadCtor()
  {
    nuxAssertMsg (m_ThreadCtorCalled == false, TEXT ("[WindowThread::ThreadCtor] ThreadCtor should not be called more than once.") );
    NUX_RETURN_VALUE_IF_TRUE (m_ThreadCtorCalled, true);

#if defined(NUX_OS_WINDOWS)
    SetWin32ThreadName (GetThreadId(), m_WindowTitle.GetTCharPtr() );
#endif

    if (RegisterNuxThread (this) == FALSE)
    {
      nuxDebugMsg (TEXT ("[WindowThread::ThreadCtor] Failed to register the WindowThread.") );
      return false;
    }

    inlSetThreadLocalStorage (ThreadLocal_InalogicAppImpl, this);
    GraphicsDisplay *ParentWindow = 0;

    if (m_Parent && static_cast<WindowThread *> (m_Parent)->Type().IsObjectType (WindowThread::StaticObjectType) )
    {
      ParentWindow = &static_cast<WindowThread *> (m_Parent)->GetWindow();
    }
    else
    {
      ParentWindow = 0;
    }

    _graphics_display = gGLWindowManager.CreateGLWindow (m_WindowTitle.GetTCharPtr(), m_StartupWidth, m_StartupHeight, m_WindowStyle, ParentWindow, false);

    if (_graphics_display == 0)
    {
      nuxDebugMsg (TEXT ("[WindowThread::ThreadCtor] Failed to create the window.") );
      return false;
    }

    if (m_Parent && m_Parent->Type().IsObjectType (WindowThread::StaticObjectType) )
    {
      // Cancel the effect of PauseThreadGraphicsRendering on the parent window.
      //PostThreadMessage(m_Parent->GetThreadId(), NUX_THREADMSG_START_RENDERING, (UINT_PTR)((void*)this), 0);
    }

    m_Painter = new BasePainter();
    m_TimerHandler = new TimerHandler();
    m_window_compositor = new WindowCompositor;
    // m_Theme = new UXTheme();

    SetThreadState (THREADRUNNING);
    m_ThreadCtorCalled = true;

    return true;
  }

#if defined(NUX_OS_WINDOWS)
  bool WindowThread::ThreadCtor (HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext)
  {
    nuxAssertMsg (m_ThreadCtorCalled == false, TEXT ("[WindowThread::ThreadCtor] ThreadCtor should not be called more than once.") );
    NUX_RETURN_VALUE_IF_TRUE (m_ThreadCtorCalled, true);

#if defined(NUX_OS_WINDOWS)
    SetWin32ThreadName (GetThreadId(), m_WindowTitle.GetTCharPtr() );
#endif

    if (RegisterNuxThread (this) == FALSE)
    {
      nuxDebugMsg (TEXT ("[WindowThread::ThreadCtor] Failed to register the WindowThread.") );
      return false;
    }

    inlSetThreadLocalStorage (ThreadLocal_InalogicAppImpl, this);
    GraphicsDisplay *ParentWindow = 0;

    if (m_Parent && static_cast<WindowThread *> (m_Parent)->Type().IsObjectType (WindowThread::StaticObjectType) )
    {
      ParentWindow = &static_cast<WindowThread *> (m_Parent)->GetWindow();
    }
    else
    {
      ParentWindow = 0;
    }

    _graphics_display = gGLWindowManager.CreateFromForeignWindow (WindowHandle, WindowDCHandle, OpenGLRenderingContext);

    if (_graphics_display == 0)
    {
      nuxDebugMsg (TEXT ("[WindowThread::ThreadCtor] Failed to create the window.") );
      return false;
    }

    if (m_Parent && m_Parent->Type().IsObjectType (WindowThread::StaticObjectType) )
    {
      // Cancel the effect of PauseThreadGraphicsRendering on the parent window.
      //PostThreadMessage(m_Parent->GetThreadId(), NUX_THREADMSG_START_RENDERING, (UINT_PTR)((void*)this), 0);
    }

    m_Painter = new BasePainter();
    m_TimerHandler = new TimerHandler();
    m_window_compositor = new WindowCompositor;
    // m_Theme = new UXTheme();

    SetThreadState (THREADRUNNING);
    m_ThreadCtorCalled = true;

    // Set initial states
    int w = _graphics_display->GetWindowWidth();
    int h = _graphics_display->GetWindowHeight();

    _graphics_display->SetViewPort (0, 0, w, h);
    m_window_compositor->FormatRenderTargets (w, h);
    m_window_compositor->FloatingAreaConfigureNotify (w, h);

    return true;
  }
#elif defined(NUX_OS_LINUX)
  bool WindowThread::ThreadCtor (Display *X11Display, Window X11Window, GLXContext OpenGLContext)
  {
    nuxAssertMsg (m_ThreadCtorCalled == false, TEXT ("[WindowThread::ThreadCtor] ThreadCtor should not be called more than once.") );
    NUX_RETURN_VALUE_IF_TRUE (m_ThreadCtorCalled, true);

    if (RegisterNuxThread (this) == FALSE)
    {
      nuxDebugMsg (TEXT ("[WindowThread::ThreadCtor] Failed to register the WindowThread.") );
      return false;
    }

    inlSetThreadLocalStorage (ThreadLocal_InalogicAppImpl, this);

    if (X11Display)
    {
      _x11display = X11Display;
      _ownx11display = false;
    }
    else
    {
      _x11display = XOpenDisplay(NULL);
      _ownx11display = true;
    }
      
    _graphics_display = gGLWindowManager.CreateFromForeignWindow (_x11display, X11Window, OpenGLContext);

    if (_graphics_display == 0)
    {
      nuxDebugMsg (TEXT ("[WindowThread::ThreadCtor] Failed to create the window.") );
      return false;
    }

    if (m_Parent && m_Parent->Type().IsObjectType (WindowThread::StaticObjectType) )
    {
      // Cancel the effect of PauseThreadGraphicsRendering on the parent window.
      //PostThreadMessage(m_Parent->GetThreadId(), NUX_THREADMSG_START_RENDERING, (UINT_PTR)((void*)this), 0);
    }

    m_Painter = new BasePainter();
    m_TimerHandler = new TimerHandler();
    m_window_compositor = new WindowCompositor;
    // m_Theme = new UXTheme();

    SetThreadState (THREADRUNNING);
    m_ThreadCtorCalled = true;

    // Set initial states
    int w = _graphics_display->GetWindowWidth();
    int h = _graphics_display->GetWindowHeight();

    _graphics_display->SetViewPort (0, 0, w, h);
    m_window_compositor->FormatRenderTargets (w, h);
    m_window_compositor->FloatingAreaConfigureNotify (w, h);

    return true;
  }
#endif

  bool WindowThread::ThreadDtor()
  {
    NUX_RETURN_VALUE_IF_TRUE (m_ThreadDtorCalled, true);

    // Cleanup
    RemoveQueuedLayout ();

    if (_main_layout)
    {
      _main_layout->UnReference();
    }

    NUX_SAFE_DELETE (m_window_compositor);
    NUX_SAFE_DELETE (m_TimerHandler);
    NUX_SAFE_DELETE (m_Painter);
    NUX_SAFE_DELETE (m_Theme);
    NUX_SAFE_DELETE (_graphics_display);

#if defined(NUX_OS_WINDOWS)
    PostThreadMessage (NUX_GLOBAL_OBJECT_INSTANCE (NProcess).GetMainThreadID(),
                       NUX_THREADMSG_THREAD_TERMINATED,
                       NUX_GLOBAL_OBJECT_INSTANCE (NProcess).GetCurrentThreadID(),
                       0);
#elif defined(NUX_OS_LINUX)

#else
#error PostThreadMessage not implemented for this platform.
#endif

    inlSetThreadLocalStorage (ThreadLocal_InalogicAppImpl, 0);
    UnregisterNuxThread (this);
    m_ThreadDtorCalled = true;
    return true;
  }

  void WindowThread::SetWindowSize (int width, int height)
  {
    if (_graphics_display)
    {
      if (IsEmbeddedWindow ())
      {
        // This is a passive way to set the window size through out the NuxGraphics system. This call gets the 
        // current window size and sets its accordingly to all sub-system.
        _graphics_display->ResetWindowSize ();
      }
      else
      {
        _graphics_display->SetWindowSize (width, height);
        ReconfigureLayout ();
      }
    }
  }

  void WindowThread::SetWindowBackgroundPaintLayer (AbstractPaintLayer *bkg)
  {
    if (m_window_compositor)
      m_window_compositor->SetBackgroundPaintLayer (bkg);
  }
  
  void WindowThread::AddToDrawList (View *view)
  {
    Area *parent;
    Geometry geo, pgeo;
    
    geo = view->GetAbsoluteGeometry ();
    parent = view->GetToplevel ();
    
    if (parent && (view != parent))
    {
//       pgeo = parent->GetGeometry();
//       geo.x += pgeo.x;
//       geo.y += pgeo.y;

      if (parent->Type ().IsDerivedFromType (BaseWindow::StaticObjectType))
      {
        BaseWindow* window = NUX_STATIC_CAST (BaseWindow*, parent);
        window->_child_need_redraw = true;
      }
    }

    if (view->Type().IsDerivedFromType (BaseWindow::StaticObjectType))
    {
      // If the view is a BaseWindow, allow it to mark itself for redraw, as if it was its own  child.
      BaseWindow* window = NUX_STATIC_CAST (BaseWindow*, view);
      window->_child_need_redraw = true;
    }

    m_dirty_areas.push_back(geo);
  }
  
  void WindowThread::ClearDrawList ()
  {
    m_dirty_areas.clear ();
  }
  
  std::vector<Geometry> WindowThread::GetDrawList ()
  {
    return m_dirty_areas;
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

  bool WindowThread::IsEmbeddedWindow()
  {
    return m_embedded_window;
  }

#if defined(NUX_OS_WINDOWS)
  bool WindowThread::ProcessForeignEvent (HWND hWnd, MSG msg, WPARAM wParam, LPARAM lParam, void *data)
#elif defined(NUX_OS_LINUX)
  bool WindowThread::ProcessForeignEvent (XEvent *xevent, void *data)
#endif
  {
    if (GetWindow().IsPauseThreadGraphicsRendering() )
    {
      return false;
    }

    IEvent nux_event;
    memset (&nux_event, 0, sizeof (IEvent) );
#if defined(NUX_OS_WINDOWS)
    _graphics_display->ProcessForeignWin32Event (hWnd, msg, wParam, lParam, &nux_event);
#elif defined(NUX_OS_LINUX)
    _graphics_display->ProcessForeignX11Event (xevent, &nux_event);
#endif

    if (nux_event.e_event ==	NUX_TERMINATE_APP || (this->GetThreadState() == THREADSTOP) )
    {
      return false;
    }

    if (nux_event.e_event ==	NUX_SIZE_CONFIGURATION)
      m_size_configuration_event = true;

    int w, h;
    // Call gGfx_OpenGL.getWindowSize after the gGfx_OpenGL.get_event.
    // Otherwise, w and h may not be correct for the current frame if a resizing happened.
    GetWindow().GetWindowSize (w, h);

    if (nux_event.e_event == NUX_MOUSE_PRESSED ||
        (nux_event.e_event == NUX_MOUSE_RELEASED) ||
        (nux_event.e_event == NUX_MOUSE_DOUBLECLICK) ||
        (nux_event.e_event == NUX_MOUSE_MOVE) ||
        (nux_event.e_event == NUX_SIZE_CONFIGURATION) ||
        (nux_event.e_event == NUX_KEYDOWN) ||
        (nux_event.e_event == NUX_KEYUP) ||
        (nux_event.e_event == NUX_WINDOW_CONFIGURATION) ||
        (nux_event.e_event == NUX_WINDOW_ENTER_FOCUS) ||
        (nux_event.e_event == NUX_WINDOW_EXIT_FOCUS) ||
        (nux_event.e_event == NUX_WINDOW_MOUSELEAVE) ||
        (nux_event.e_event == NUX_MOUSE_WHEEL))
    {
        //DISPATCH EVENT HERE
        //nux_event.Application = Application;
        m_window_compositor->ProcessEvent(nux_event);
    }

    if(nux_event.e_event == NUX_SIZE_CONFIGURATION)
    {
        if(!GetWindow().isWindowMinimized())
        {
            GetWindow().SetViewPort(0, 0, nux_event.width, nux_event.height);
            ReconfigureLayout ();
            m_window_compositor->FormatRenderTargets(nux_event.width, nux_event.height);
        }
        m_window_compositor->FloatingAreaConfigureNotify(nux_event.width, nux_event.height);
        m_size_configuration_event = true;
    }

    // Some action may have caused layouts and areas to request a recompute. 
    // Process them here before the Draw section.
    if (!GetWindow().isWindowMinimized() )
    {
      if (_queue_main_layout)
      {
        ReconfigureLayout ();
      }
      else 
      {
        // Compute the layouts that have been queued.
        ComputeQueuedLayout ();
      }
    }

    // Warn the host window manager to initiate a draw cycle.
    bool request_draw_cycle_to_host_wm = false;

    if (this->m_bFirstDrawPass)
    {
      request_draw_cycle_to_host_wm = true;
      m_force_redraw = true;
      //m_window_compositor->Draw(m_size_configuration_event, true);
      this->m_bFirstDrawPass = false;
    }
    else
    {
      bool b = (nux_event.e_event == NUX_MOUSE_PRESSED) ||
               (nux_event.e_event == NUX_MOUSE_RELEASED) ||
               (nux_event.e_event == NUX_MOUSE_DOUBLECLICK) ||
               //(event.e_event == NUX_MOUSE_MOVE) ||
               (nux_event.e_event == NUX_SIZE_CONFIGURATION) ||
               (nux_event.e_event == NUX_KEYDOWN) ||
               (nux_event.e_event == NUX_KEYUP) ||
               (nux_event.e_event == NUX_WINDOW_CONFIGURATION) ||
               (nux_event.e_event == NUX_WINDOW_ENTER_FOCUS) ||
               (nux_event.e_event == NUX_WINDOW_EXIT_FOCUS) ||
               (nux_event.e_event == NUX_WINDOW_DIRTY);

      if (b && m_window_compositor->IsTooltipActive() )
      {
        // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
        m_window_compositor->CancelTooltip();
        b |= true;
      }

      if (!m_window_compositor->ValidateMouseInsideTooltipArea (nux_event.e_x, nux_event.e_y) && m_window_compositor->IsTooltipActive() )
      {
        // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
        m_window_compositor->CancelTooltip();
        b |= true;
      }

      if (b || IsRedrawNeeded() )
      {
        request_draw_cycle_to_host_wm = true;
      }
      else if (m_window_compositor->GetWidgetDrawingOverlay() != 0)
      {
        request_draw_cycle_to_host_wm = true;
      }
    }

    if (!_draw_requested_to_host_wm && request_draw_cycle_to_host_wm)
      RequestRedraw ();

    return request_draw_cycle_to_host_wm;
  }

  void WindowThread::RenderInterfaceFromForeignCmd(Geometry *clip)
  {
    nuxAssertMsg (IsEmbeddedWindow() == true, TEXT ("[WindowThread::RenderInterfaceFromForeignCmd] You can only call RenderInterfaceFromForeignCmd if the window was created with CreateFromForeignWindow.") );

    if (!IsEmbeddedWindow() )
      return;

    // Set Nux opengl states. The other plugin in compiz have changed the GPU opengl states.
    // Nux keep tracks of its own opengl states and restore them before doing any drawing.
    GetWindowThread ()->GetGraphicsEngine().GetRenderStates().SubmitChangeStates();

    GetWindowThread ()->GetGraphicsEngine().SetOpenGLClippingRectangle (0, 0, GetWindowThread ()->GetGraphicsEngine().GetWindowWidth(),
        GetWindowThread ()->GetGraphicsEngine().GetWindowHeight() );

    if (GetWindow().IsPauseThreadGraphicsRendering() == false)
    {
      RefreshLayout ();
      
      if (clip)
        GetWindowThread ()->GetGraphicsEngine().SetGlobalClippingRectangle (Rect (clip->x, clip->y, clip->width, clip->height));
        
      m_window_compositor->Draw (m_size_configuration_event, m_force_redraw);
      
      if (clip)
        GetWindowThread ()->GetGraphicsEngine().DisableGlobalClippingRectangle ();
      // When rendering in embedded mode, nux does not attempt to measure the frame rate...

      // Cleanup
      GetWindowThread ()->GetGraphicsEngine().ResetStats();
      ClearRedrawFlag();

      m_size_configuration_event = false;
      m_force_redraw = false;
    }

    CHECKGL ( glColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE) );

    GetGraphicsDisplay()->GetGpuDevice()->DeactivateFrameBuffer();
  }

  int WindowThread::InstallEventInspector (EventInspector function, void* data)
  {
    NUX_RETURN_VALUE_IF_NULL (function, 0);

    std::map < int, EventInspectorStorage >::iterator it;

    for (it = _event_inspectors_map.begin (); it != _event_inspectors_map.end (); it++)
    {
      if ((*it).second._function == function)
      {
        // The inspector has already been added. Return its unique id
        return (*it).second._uid;
      }
    }

    // This is a new Event Inspector
    EventInspectorStorage new_inspector;
    new_inspector._function = function;
    new_inspector._data = data;
    new_inspector._uid = NUX_GLOBAL_OBJECT_INSTANCE (UniqueIndex).GetUniqueIndex ();

    _event_inspectors_map [new_inspector._uid] = new_inspector;
    return new_inspector._uid;
  }

  bool WindowThread::RemoveEventInspector (int event_inspector_id)
  {
    NUX_RETURN_VALUE_IF_NULL (event_inspector_id, false);

    std::map < int, EventInspectorStorage >::iterator it;

    for (it = _event_inspectors_map.begin (); it != _event_inspectors_map.end (); it++)
    {
      if ((*it).second._uid == event_inspector_id)
      {
        _event_inspectors_map.erase (it);
        return true;
      }
    }
    return false;
  }

  bool WindowThread::RemoveEventInspector (EventInspector function)
  {
    NUX_RETURN_VALUE_IF_NULL (function, false);

    std::map < int, EventInspectorStorage >::iterator it;

    for (it = _event_inspectors_map.begin (); it != _event_inspectors_map.end (); it++)
    {
      if ((*it).second._function == function)
      {
        _event_inspectors_map.erase (it);
        return true;
      }
    }
    return false;
  }

  bool WindowThread::CallEventInspectors (Event* event)
  {
    int n = _event_inspectors_map.size();
    if (n == 0)
    {
      // No event inspector installed.
      return false;
    }

    bool discard_event = false;
    std::map < int, EventInspectorStorage >::iterator it;

    for (it = _event_inspectors_map.begin (); it != _event_inspectors_map.end (); it++)
    {
      EventInspector callback = (*it).second._function;

      if (callback == 0)
        continue;

      int ret = callback (0, event, (*it).second._data);

      if (ret)
      {
        discard_event = true;
      }
    }

    return discard_event;
  }


UXTheme& WindowThread::GetTheme() const
{
  if (!m_Theme)
  {
    LOG_INFO(logger) << "Lazily creating nux::UXTheme";
    const_cast<WindowThread*>(this)->m_Theme = new UXTheme();
  }
  return *m_Theme;
}

}

