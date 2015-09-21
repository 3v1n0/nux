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

#include <functional>

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
#include "MainLoopGLib.h"

namespace nux
{
DECLARE_LOGGER(logger, "nux.windows.thread");

  TimerFunctor *m_ScrollTimerFunctor;
  TimerHandle m_ScrollTimerHandler;

// Thread registration call. Hidden from the users. Implemented in Nux.cpp
  bool RegisterNuxThread(NThread *ThreadPtr);
  void UnregisterNuxThread(NThread *ThreadPtr);

  const int WindowThread::MINIMUM_WINDOW_WIDTH  = 1;
  const int WindowThread::MINIMUM_WINDOW_HEIGHT = 1;

  NUX_IMPLEMENT_OBJECT_TYPE(WindowThread);

  WindowThread::WindowThread(const char *WindowTitle, int width, int height, AbstractThread *Parent, bool Modal)
    : AbstractThread(Parent)
    , foreign_frame_frozen_(false)
    , window_initial_width_(width)
    , window_initial_height_(height)
    , window_title_(WindowTitle)
    , m_WidgetInitialized(false)
    , window_style_(WINDOWSTYLE_NORMAL)
    , embedded_window_(false)
    , window_size_configuration_event_(false)
    , force_rendering_(false)
    , external_glib_sources_(new ExternalGLibSources)
#ifdef NUX_GESTURES_SUPPORT
    , geis_adapter_(new GeisAdapter)
#endif
  {
    // Thread specific objects
    graphics_display_       = NULL;
    window_compositor_      = NULL;
    painter_                = NULL;
    timer_manager_          = NULL;
    theme_                  = NULL;
    main_layout_            = NULL;
    queue_main_layout_ = false;
    
    // Protection for ThreadCtor and ThreadDtor;
    thread_ctor_called_ = false;
    thread_dtor_called_ = false;

    //
    wait_for_modal_window_   = false;
    modal_window_thread_     = 0;
    is_modal_window_              = Modal;

    _inside_layout_cycle = 0;
    _draw_requested_to_host_wm       = false;
    first_pass_        = true;

#if !defined(NUX_MINIMAL)
    _Timelines = new std::list<Timeline*> ();
    gint64 micro_secs = g_get_real_time();
    last_timeline_frame_time_sec_ = micro_secs / 1000000;
    last_timeline_frame_time_usec_ = micro_secs % 1000000;
#endif
    _MasterClock = NULL;

    main_loop_glib_      = 0;
    main_loop_glib_context_   = 0;

#if defined(USE_X11)
    x11display_ = NULL;
    ownx11display_ = false;
#endif
    
    _pending_wake_up_timer = false;
    _inside_main_loop = false;
    _inside_timer_loop = false;
    async_wake_up_signal_ = new TimerFunctor();
    async_wake_up_signal_->tick.connect(sigc::mem_fun(this, &WindowThread::AsyncWakeUpCallback));
  }

  WindowThread::~WindowThread()
  {
    xim_controller_.reset();
    CleanupGlibLoop();

    ThreadDtor();
#if !defined(NUX_MINIMAL)
    std::list<Timeline*>::iterator li;
    for (li=_Timelines->begin(); li!=_Timelines->end(); ++li)
    {
      (*li)->UnReference();
    }
    delete _Timelines;
#endif

    delete async_wake_up_signal_;

#if defined(USE_X11)
    if (x11display_ && ownx11display_)
    {
      XCloseDisplay(x11display_);
    }
#endif
  }

  void WindowThread::ExitMainLoop()
  {
    StopGLibLoop();
  }

  unsigned int WindowThread::AddTimeout(unsigned int timeout_delay)
  {
    return AddGLibTimeout(timeout_delay);
  }

  TimerHandle WindowThread::SetAsyncTimerCallback(int time_ms, TimeOutSignal* timeout_signal, void *user_data)
  {
    if (timeout_signal == NULL)
      return TimerHandle();

    // Use "this->" because if called from a different thread, GetTimer and GetWindowThread are invalid.
    TimerHandle handle = this->GetTimerHandler().AddOneShotTimer(time_ms, timeout_signal, user_data, this);

    return handle;
  }

  void WindowThread::AsyncWakeUpCallback(void* /* data */)
  {
    this->GetTimerHandler().RemoveTimerHandler(async_wake_up_timer_handle_);
    _pending_wake_up_timer = false;
  }

  void WindowThread::ProcessDraw(GraphicsEngine &graphics_engine, bool force_draw)
  {
    if (main_layout_)
    {
      bool dirty = main_layout_->IsQueuedForDraw();

      if (dirty)
      {
        // A main layout re computation has happen. It was not initiated physically by resizing the window. We need to draw the background to
        // clean any dirty region.
        int buffer_width = graphics_engine.GetWindowWidth();
        int buffer_height = graphics_engine.GetWindowHeight();
        GetPainter().PaintBackground(graphics_engine, Geometry(0, 0, buffer_width, buffer_height));
      }

      main_layout_->ProcessDraw(graphics_engine, force_draw || dirty);
    }
  }

  void WindowThread::RequestRedraw()
  {
    _draw_requested_to_host_wm = true;
    RedrawRequested.emit();

    if (!IsEmbeddedWindow())
    {
      // If the system is not in embedded mode and an asynchronous request for a Draw is made,
      // and the system is not in a timer processing cycle(always followed by a draw cycle) 
      // or not in the event processing cycle(also followed by a draw cycle), then we set a 0 delay 
      // timer that will wake up the system and initiate a draw cycle.
      if ((_inside_main_loop == false) && (_inside_timer_loop == false) && (_pending_wake_up_timer == false))
      {
        _pending_wake_up_timer = true;
        async_wake_up_timer_handle_ = this->GetTimerHandler().AddOneShotTimer(0, async_wake_up_signal_, this);
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

  void WindowThread::SetLayout(Layout *layout)
  {
    main_layout_ = layout;

    if (main_layout_)
    {
      int w = graphics_display_->GetGraphicsEngine()->GetContextWidth();
      int h = graphics_display_->GetGraphicsEngine()->GetContextHeight();

      main_layout_->Reference();
      main_layout_->SetScaleFactor(1);

      StartLayoutCycle();
      main_layout_->SetGeometry(0, 0, w, h);
      main_layout_->ComputeContentSize();
      main_layout_->ComputeContentPosition(0, 0);
      StopLayoutCycle();

      RemoveQueuedLayout();
    }
  }

  Layout* WindowThread::GetLayout()
  {
    return main_layout_;
  }

  void WindowThread::QueueLayout()
  {
    queue_main_layout_ = true;
    RequestRedraw();
  }

  void WindowThread::ReconfigureLayout()
  {
    int w = graphics_display_->GetGraphicsEngine()->GetWindowWidth();
    int h = graphics_display_->GetGraphicsEngine()->GetWindowHeight();

    if (main_layout_)
    {
      StartLayoutCycle();
      main_layout_->SetGeometry(0, 0, w, h);
      main_layout_->ComputeContentSize();
      main_layout_->ComputeContentPosition(0, 0);
      StopLayoutCycle();
    }

    RemoveQueuedLayout();
    queue_main_layout_ = false;
  }

  bool WindowThread::QueueObjectLayout(Area *area)
  {
    NUX_RETURN_VALUE_IF_NULL(area, false);

    std::list<Area *>::iterator it;
    it = find(_queued_layout_list.begin(), _queued_layout_list.end(), area);
    if (it == _queued_layout_list.end())
    {
      _queued_layout_list.push_back(area);
    }

    return true;
  }

  bool WindowThread::RemoveObjectFromLayoutQueue(Area *area)
  {
    NUX_RETURN_VALUE_IF_NULL(area, false);

    std::list<Area *>::iterator it;
    it = find(_queued_layout_list.begin(), _queued_layout_list.end(), area);

    if (it != _queued_layout_list.end())
    {
      _queued_layout_list.erase(it);
      return true;
    }
    return false;
  }

  void WindowThread::RemoveQueuedLayout()
  {
    _queued_layout_list.clear();
  }

  void WindowThread::ComputeQueuedLayout()
  {
    StartLayoutCycle();
    std::list<Area *>::iterator it;

    for (it = _queued_layout_list.begin(); it != _queued_layout_list.end(); ++it)
    {
      Area *area = *it;

      if (area->Type().IsDerivedFromType(View::StaticObjectType))
      {
        View *view  = NUX_STATIC_CAST(View *, area);

        if (!view->CanBreakLayout())
          view->QueueDraw();
      }
      else if (area->Type().IsDerivedFromType(Layout::StaticObjectType))
      {
        Layout *layout = NUX_STATIC_CAST(Layout *, area);
        layout->QueueDraw();
      }
      else
      {
        continue;
      }

      (*it)->ComputeContentSize();
    }

    StopLayoutCycle();
    
    RemoveQueuedLayout();
  }

  void WindowThread::StartLayoutCycle()
  {
    _inside_layout_cycle = true;
  }

  void WindowThread::StopLayoutCycle()
  {
    _inside_layout_cycle = false;
  }


  bool WindowThread::IsInsideLayoutCycle() const
  {
    return _inside_layout_cycle;
  }

  void WindowThread::ComputeElementLayout(Area *area, bool recurse_to_top_level_layout)
  {
    NUX_RETURN_IF_NULL(area);

    bool alreadyComputingLayout = IsInsideLayoutCycle();

    if ((!alreadyComputingLayout) && (!recurse_to_top_level_layout))
    {
      // When computing the layout, setting the size of widgets may cause the system to recurse 
      // upward an look for the up most container which size is affected by its this area.
      // This happens in Area::InitiateResizeLayout();
      // The search upward is not done if we are already in a layout cycle.
      StartLayoutCycle();
    }

    if (area->Type().IsDerivedFromType(View::StaticObjectType))
    {
      View *ic = NUX_STATIC_CAST(View *, area);
      ic->QueueDraw();
    }
    else if (area->Type().IsDerivedFromType(Layout::StaticObjectType))
    {
      Layout *layout = NUX_STATIC_CAST(Layout *, area);
      layout->QueueDraw();
    }

    area->ComputeContentSize();

    if (!alreadyComputingLayout)
      StopLayoutCycle();
  }

#if !defined(NUX_MINIMAL)
  void WindowThread::AddTimeline(Timeline *timeline)
  {
    _Timelines->push_back(timeline);
    _Timelines->unique();
    StartMasterClock();
  }

  void WindowThread::RemoveTimeline(Timeline *timeline)
  {
    _Timelines->remove(timeline);
    if (_Timelines->empty())
    {
      StopMasterClock();
    }
  }
#endif

  int WindowThread::Run(void * /* ptr */)
  {
    if (GetWindowThread() != this)
    {
      nuxDebugMsg("Cannot run a several WindowThreads in the same window.");
      return 1;
    }

    if (!IsEmbeddedWindow() && (thread_ctor_called_ == false))
    {
      nuxDebugMsg("Call WindowThread::Start to run this window in its own thread.");
      return 1;
    }

    // Setup the main loop first, so that user_init_func can add new sources
    // before running it
    SetupMainLoop();

    if (user_init_func_ && (m_WidgetInitialized == false))
    {
      (*user_init_func_) (this, initialization_data_);
      m_WidgetInitialized = true;
    }

    return MainLoop();
  }

  void WindowThread::SetupMainLoop()
  {
    if (IsEmbeddedWindow())
    {
      window_compositor_->FormatRenderTargets(graphics_display_->GetWindowWidth(), graphics_display_->GetWindowHeight());
      InitGlibLoop();
    }
    else
    {
      InitGlibLoop();
    }

    // Called the first time so we can initialize the size of the render targets
    // At this stage, the size of the window is known.
    window_compositor_->FormatRenderTargets(graphics_display_->GetWindowWidth(), graphics_display_->GetWindowHeight());
  }

  int WindowThread::MainLoop()
  {
    if (IsEmbeddedWindow())
    {
      RunGlibLoop();
      return 0;
    }
    else
    {
      graphics_display_->ShowWindow();
    }

    while (GetThreadState() != THREADSTOP)
    {
      if (GetThreadState() == THREADRUNNING)
      {
        RunGlibLoop();

        if (parent_)
        {
          if (parent_->Type().IsObjectType(SystemThread::StaticObjectType))
            static_cast<SystemThread *> (parent_)->ChildHasFinished(this);

          if (parent_->Type().IsObjectType(WindowThread::StaticObjectType))
            static_cast<WindowThread *> (parent_)->ChildHasFinished(this);

          {
            SetThreadState(THREADSTOP);
            // All children window must be terminated.
            TerminateChildWindows();
            JoinChildThreads();

            std::list<AbstractThread*>::iterator it;
            for (it = children_thread_list_.begin(); it != children_thread_list_.end(); ++it)
            {
              delete (*it);
            }
            children_thread_list_.clear();
          }
        }
        else
        {
          SetThreadState(THREADSTOP);
          // All child window must be terminated.
          TerminateChildWindows();
          JoinChildThreads();

          std::list<AbstractThread*>::iterator it;
          for (it = children_thread_list_.begin(); it != children_thread_list_.end(); ++it)
          {
            delete (*it);
          }
          children_thread_list_.clear();
        }
      }
      else
      {
        // Do not sleep. Just return and let the GraphicsDisplay::SwapBuffer do the sleep if necessary.
      }
    }

    return 0;
  }

  extern EventToNameStruct EventToName[];

  Event WindowThread::GetNextEvent()
  {
    Event event;
    graphics_display_->GetSystemEvent(&event);

#if defined(NUX_OS_LINUX) && defined(USE_X11)
    // Make sure the current xic is synced up with the current event window
    if ((event.type == KeyPress || event.type == KeyRelease) &&
         event.x11_window && xim_controller_->GetCurrentWindow() != event.x11_window)
    {
      xim_controller_->SetFocusedWindow(event.x11_window);
      graphics_display_->SetCurrentXIC(xim_controller_->GetXIC());
    }
#endif

    return event;
  }

  unsigned int WindowThread::ProcessEvent(Event &event)
  {
    if (!IsEmbeddedWindow() && graphics_display_->IsPauseThreadGraphicsRendering())
    {
      // Do not sleep. Just return and let the GraphicsDisplay::SwapBuffer do the sleep if necessary.
      return 0;
    }

    return DoProcessEvent(event);
  }

  unsigned int WindowThread::DoProcessEvent(Event &event)
  {
    _inside_main_loop = true;

    if (first_pass_)
    {
      // Reset the timers that were called before the mainloop got initialized.
      GetTimer().StartEarlyTimerObjects();
    }

    if ((event.type == NUX_DND_ENTER_WINDOW) ||
        (event.type == NUX_DND_LEAVE_WINDOW))
    {
      GetWindowCompositor().ResetDnDArea();
    }

    // Call event inspectors.
    bool event_discarded = CallEventInspectors(&event);
    if (event_discarded)
    {
      return 1;
    }

    if ((event.type == NUX_TERMINATE_APP) || (this->GetThreadState() == THREADSTOP))
    {
      return 0;
    }

    if (event.type == NUX_SIZE_CONFIGURATION)
    {
      window_size_configuration_event_ = true;
      Rect r = graphics_display_->GetWindowGeometry();
      window_configuration.emit(r.x, r.y, r.width, r.height);
    }

    int w, h;
    // Call gGfx_OpenGL.getWindowSize after the gGfx_OpenGL.get_event.
    // Otherwise, w and h may not be correct for the current frame if a resizing happened.
    graphics_display_->GetWindowSize(w, h);

    if ((event.type == NUX_MOUSE_PRESSED) ||
        (event.type == NUX_MOUSE_RELEASED) ||
        (event.type == NUX_MOUSE_DOUBLECLICK) ||
        (event.type == NUX_MOUSE_MOVE) ||
        (event.type == NUX_SIZE_CONFIGURATION) ||
        (event.type == NUX_KEYDOWN) ||
        (event.type == NUX_KEYUP) ||
        (event.type == NUX_NC_WINDOW_CONFIGURATION) ||
        (event.type == NUX_WINDOW_ENTER_FOCUS) ||
        (event.type == NUX_WINDOW_EXIT_FOCUS) ||
        (event.type == NUX_WINDOW_MOUSELEAVE) ||
        (event.type == NUX_DND_MOVE) ||
        (event.type == NUX_DND_DROP) ||
        (event.type == NUX_DND_ENTER) ||
        (event.type == NUX_DND_LEAVE) ||
        (event.type == NUX_MOUSE_WHEEL) ||
        event.type == EVENT_GESTURE_BEGIN ||
        event.type == EVENT_GESTURE_UPDATE ||
        event.type == EVENT_GESTURE_END)
    {
      //DISPATCH EVENT HERE
      //event.Application = Application;
      window_compositor_->ProcessEvent(event);
    }

    if (event.type == NUX_SIZE_CONFIGURATION)
    {
      if (!graphics_display_->isWindowMinimized())
      {
        graphics_display_->SetViewPort(0, 0, event.width, event.height);
        ReconfigureLayout();
        window_compositor_->FormatRenderTargets(event.width, event.height);
      }
      window_compositor_->FloatingAreaConfigureNotify(event.width, event.height);
      window_size_configuration_event_ = true;
    }

    // Some action may have caused layouts and areas to request a recompute. 
    // Process them here before the Draw section.
    if (!graphics_display_->isWindowMinimized() && !IsEmbeddedWindow())
    {
      if (queue_main_layout_)
      {
        ReconfigureLayout();
      }
      else
      {
        // Compute the layouts that have been queued.
        ComputeQueuedLayout();
      }
    }

    _inside_main_loop = false;

    if (!graphics_display_->IsPauseThreadGraphicsRendering() || IsEmbeddedWindow())
    {
      bool SwapGLBuffer = false;

      // Warn the host window manager to initiate a draw cycle.
      bool request_draw_cycle_to_host_wm = false;

      if (first_pass_)
      {
        if (IsEmbeddedWindow())
        {
          request_draw_cycle_to_host_wm = true;
          force_rendering_ = true;
        }
        else
        {
          window_compositor_->Draw(window_size_configuration_event_, true);
        }
        first_pass_ = false;
      }
      else
      {
        bool b = (event.type == NUX_MOUSE_PRESSED) ||
          (event.type == NUX_MOUSE_RELEASED) ||
          (event.type == NUX_MOUSE_DOUBLECLICK) ||
          //(event.type == NUX_MOUSE_MOVE) ||
          (event.type == NUX_SIZE_CONFIGURATION) ||
          (event.type == NUX_KEYDOWN) ||
          (event.type == NUX_KEYUP) ||
          (event.type == NUX_NC_WINDOW_CONFIGURATION) ||
          (event.type == NUX_WINDOW_ENTER_FOCUS) ||
          (event.type == NUX_WINDOW_EXIT_FOCUS) ||
          (event.type == NUX_WINDOW_DIRTY);

        if (b && window_compositor_->IsTooltipActive())
        {
          // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
          window_compositor_->CancelTooltip();
          b |= true;
        }

        if (!window_compositor_->ValidateMouseInsideTooltipArea(event.x, event.y) && window_compositor_->IsTooltipActive())
        {
          // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
          window_compositor_->CancelTooltip();
          b |= true;
        }

        if (b || IsRedrawNeeded())
        {
          if (IsEmbeddedWindow())
          {
            request_draw_cycle_to_host_wm = true;
          }
          else
          {
            window_compositor_->Draw(window_size_configuration_event_, false);
          }
          SwapGLBuffer = true;
        }
        else if (window_compositor_->GetWidgetDrawingOverlay() != 0)
        {
          if (IsEmbeddedWindow())
          {
            request_draw_cycle_to_host_wm = true;
          }
          else
          {
            window_compositor_->Draw(window_size_configuration_event_, false);
          }
          SwapGLBuffer = false;
        }

      }

      if (!IsEmbeddedWindow())
      {
        if (SwapGLBuffer)
        {
          // Something was rendered! Swap the rendering buffer!
          graphics_display_->SwapBuffer(true);
        }

        ClearRedrawFlag();
        GetWindowThread()->GetGraphicsEngine().ResetStats();
      }
      else if (IsEmbeddedWindow() && (_draw_requested_to_host_wm == false) && request_draw_cycle_to_host_wm)
      {
        RequestRedraw();
      }
      window_size_configuration_event_ = false;
    }

    return 1;
  }

  unsigned int SpawnThread(NThread & /* thread */)
  {
    return 0;
  }

  bool WindowThread::IsWaitingforModalWindow() const
  {
    return wait_for_modal_window_;
  }

  bool WindowThread::IsModalWindow() const
  {
    return is_modal_window_;
  }

  void WindowThread::AddChildThread(AbstractThread *window)
  {
    if (window == NULL)
      return;

    std::list<AbstractThread*>::iterator it;
    it = find(children_thread_list_.begin(), children_thread_list_.end(), window);

    if (it == children_thread_list_.end())
    {
      children_thread_list_.push_back(window);
    }
  }

  void WindowThread::RemoveChildThread(AbstractThread *window)
  {
    nuxAssert(window);
    std::list<AbstractThread*>::iterator it;
    it = find(children_thread_list_.begin(), children_thread_list_.end(), window);

    if (it != children_thread_list_.end())
    {
      children_thread_list_.erase(it);
    }
  }

  void WindowThread::ChildHasFinished(AbstractThread *thread)
  {
    RemoveChildThread(thread);

    if (thread->Type().IsObjectType(WindowThread::StaticObjectType))
    {
      SuspendChildGraphics(static_cast<WindowThread*> (thread));
    }

    thread->SetThreadState(THREADSTOP);
  }

  void WindowThread::TerminateChildThreads()
  {
    std::list<AbstractThread*>::iterator it;

    for (it = children_thread_list_.begin(); it != children_thread_list_.end(); ++it)
    {
      (*it)->SetThreadState(THREADSTOP);

      if ((*it)->Type().IsObjectType(WindowThread::StaticObjectType))
      {
        // Terminate by shutting down the main loop
        static_cast<WindowThread*>(*it)->ExitMainLoop();
      }

      if ((*it)->Type().IsObjectType(SystemThread::StaticObjectType))
      {
        // Just kill the thread
        static_cast<SystemThread*>(*it)->Stop(true);
      }
    }
  }

  ThreadState WindowThread::Start(void * /* ptr */)
  {
    if (!parent_)
    {
      return NThread::Start();
    }
    else
    {
      if (parent_->Type().IsObjectType(SystemThread::StaticObjectType))
      {
        return static_cast<SystemThread *> (parent_)->StartChildThread(this, true);
      }
      else if (parent_->Type().IsObjectType(WindowThread::StaticObjectType))
      {
        return static_cast<WindowThread *> (parent_)->StartChildThread(this, true);
      }

      nuxAssertMsg(0, "[WindowThread::Start] This should not happen.");
      return THREAD_START_ERROR;
    }
  }

  ThreadState WindowThread::StartChildThread(AbstractThread *thread, bool /* Modal */)
  {
    if (wait_for_modal_window_)
    {
      // This window is already waiting for a modal window. It cannot start another windows.
      return thread->GetThreadState();
    }

    ThreadState state = thread->NThread::Start();
    //if(state == THREADRUNNING)
    {
      if (thread->Type().IsObjectType(WindowThread::StaticObjectType))
      {
        // While the child window is being created, the rendering is paused.
        // This is necessary to active OpenGL objects context sharing.
        // Cancel the pause by sending the message NUX_THREADMSG_START_RENDERING to this thread.
        //graphics_display_->PauseThreadGraphicsRendering();

        if (static_cast<WindowThread *> (thread)->is_modal_window_)
        {
          DisableMouseKeyboardInput();
          modal_window_thread_ = static_cast<WindowThread *> (thread);
//                 std::list<NThread*>::iterator it;
//                 for (it = children_thread_list_.begin(); it != children_thread_list_.end(); it++)
//                 {
//                     static_cast<WindowThread*>(*it)->wait_for_modal_window_ = true;
//                     // WIN32: Disable Mouse and Keyboard inputs for all windows child of this window
//                     ::EnableWindow(static_cast<WindowThread*>(*it)->graphics_display_->GetWindowHandle(), FALSE);
//                 }
//                 // WIN32
//                 ::EnableWindow(graphics_display_->GetWindowHandle(), FALSE);
//                 wait_for_modal_window_ = true;
        }

        static_cast<WindowThread *> (thread)->wait_for_modal_window_ = false;

        AddChildThread(thread);
      }
    }
    return state;
  }

  ThreadState WindowThread::SuspendChildGraphics(WindowThread *thread)
  {
    if (wait_for_modal_window_)
    {
      if (modal_window_thread_ != thread)
      {
        nuxAssertMsg(0, "[WindowThread::SuspendChildGraphics] cannot supend thread that is not the modal window.");
        return thread->GetThreadState();
      }
    }

    ThreadState state = thread->GetThreadState();

    if (wait_for_modal_window_)
    {
      modal_window_thread_ = 0;

      EnableMouseKeyboardInput();
//         std::list<NThread*>::iterator it;
//         for (it = children_thread_list_.begin(); it != children_thread_list_.end(); it++)
//         {
//             static_cast<WindowThread*>(*it)->wait_for_modal_window_ = false;
//
//             // WIN32
//             ::EnableWindow(static_cast<WindowThread*>(*it)->graphics_display_->GetWindowHandle(), TRUE);
//         }
    }

    // WIN32
#if defined(NUX_OS_WINDOWS)
    ::EnableWindow(graphics_display_->GetWindowHandle(), TRUE);
#elif defined(NUX_OS_LINUX)

#endif
    return state;
  }

#if !defined(NUX_MINIMAL)
  bool WindowThread::ProcessTimelines(gint64 micro_secs)
  {
    // go through our timelines and tick them
    // return true if we still have active timelines

    long msecs;
    msecs = (micro_secs / 1000000 - last_timeline_frame_time_sec_) * 1000 +
            (micro_secs % 1000000 - last_timeline_frame_time_usec_) / 1000;

    if (msecs < 0)
    {
      last_timeline_frame_time_sec_ = micro_secs / 1000000;
      last_timeline_frame_time_usec_ = micro_secs % 1000000;
      return true;
    }

    if (msecs > 0)
    {
      last_timeline_frame_time_sec_ += msecs / 1000;
      last_timeline_frame_time_usec_ += msecs * 1000;
    }

    std::list<Timeline*>::iterator li;
    std::list<Timeline*> timelines_copy;

    for (li=_Timelines->begin(); li!=_Timelines->end(); ++li)
    {
      (*li)->Reference();
      timelines_copy.push_back((*li));
    }

    for(li=timelines_copy.begin(); li!=timelines_copy.end(); ++li)
    {
      (*li)->DoTick(msecs);
    }

    // unreference again
    for (li=timelines_copy.begin(); li!=timelines_copy.end(); ++li)
      (*li)->UnReference();

    // return if we have any timelines left
    return (_Timelines->size() != 0);
  }
#endif

  void WindowThread::EnableMouseKeyboardInput()
  {
    std::list<AbstractThread*>::iterator it;

    for (it = children_thread_list_.begin(); it != children_thread_list_.end(); ++it)
    {
      if (NUX_STATIC_CAST(WindowThread *, *it)->Type().IsObjectType(WindowThread::StaticObjectType))
      {
        NUX_STATIC_CAST(WindowThread *, *it)->EnableMouseKeyboardInput();
      }
    }

    // WIN32: Enable Mouse and Keyboard inputs for all windows child of this window
#if defined(NUX_OS_WINDOWS)
    ::EnableWindow(graphics_display_->GetWindowHandle(), TRUE);
#elif defined(NUX_OS_LINUX)

#endif
    wait_for_modal_window_ = false;
  }

  void WindowThread::DisableMouseKeyboardInput()
  {
    std::list<AbstractThread*>::iterator it;

    for (it = children_thread_list_.begin(); it != children_thread_list_.end(); ++it)
    {
      if (NUX_STATIC_CAST(WindowThread *, *it)->Type().IsObjectType(WindowThread::StaticObjectType))
      {
        NUX_STATIC_CAST(WindowThread *, *it)->DisableMouseKeyboardInput();
      }
    }

    // WIN32: Disable Mouse and Keyboard inputs for all windows child of this window
#if defined(NUX_OS_WINDOWS)
    ::EnableWindow(graphics_display_->GetWindowHandle(), FALSE);
#elif defined(NUX_OS_LINUX)

#endif
    wait_for_modal_window_ = true;
  }

  bool WindowThread::ThreadCtor()
  {
    if(thread_ctor_called_)
    {
      nuxDebugMsg("[WindowThread::ThreadCtor] ThreadCtor should not be entered more than once per WindowThread.");
      return true;
    }

#if defined(NUX_OS_WINDOWS)
    SetWin32ThreadName(GetThreadId(), window_title_.c_str());
#endif

    if (RegisterNuxThread(this) == FALSE)
    {
      nuxDebugMsg("[WindowThread::ThreadCtor] Failed to register the WindowThread.");
      return false;
    }

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, this);
    GraphicsDisplay *parent_window = NULL;

    if (parent_ && static_cast<WindowThread *> (parent_)->Type().IsObjectType(WindowThread::StaticObjectType))
    {
      parent_window = &static_cast<WindowThread *> (parent_)->GetGraphicsDisplay();
    }
    else
    {
      parent_window = NULL;
    }

    graphics_display_ = gGLWindowManager.CreateGLWindow(window_title_.c_str(), window_initial_width_, window_initial_height_, window_style_, parent_window, false);

    if (graphics_display_ == NULL)
    {
      nuxDebugMsg("[WindowThread::ThreadCtor] Failed to create the window.");
      return false;
    }

    if (parent_ && parent_->Type().IsObjectType(WindowThread::StaticObjectType))
    {
      // Cancel the effect of PauseThreadGraphicsRendering on the parent window.
      //PostThreadMessage(parent_->GetThreadId(), NUX_THREADMSG_START_RENDERING, (UINT_PTR)((void*)this), 0);
    }

    painter_ = new BasePainter(this);
    timer_manager_ = new TimerHandler(this);
    window_compositor_ = new WindowCompositor(this);

    xim_controller_ = std::make_shared<XIMController>(graphics_display_->GetX11Display());

    SetThreadState(THREADRUNNING);
    thread_ctor_called_ = true;
    return true;
  }

#if defined(NUX_OS_WINDOWS)
  bool WindowThread::ThreadCtor(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext)
  {
    nuxAssertMsg(thread_ctor_called_ == false, "[WindowThread::ThreadCtor] ThreadCtor should not be entered more than once per WindowThread.");
    if(thread_ctor_called_)
    {
      return true;
    }

    SetWin32ThreadName(GetThreadId(), window_title_.c_str());

    if (RegisterNuxThread(this) == FALSE)
    {
      nuxDebugMsg("[WindowThread::ThreadCtor] Failed to register the WindowThread.");
      return false;
    }

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, this);
    GraphicsDisplay *ParentWindow = 0;

    if (parent_ && static_cast<WindowThread *> (parent_)->Type().IsObjectType(WindowThread::StaticObjectType))
    {
      ParentWindow = &static_cast<WindowThread *> (parent_)->GetGraphicsDisplay();
    }
    else
    {
      ParentWindow = 0;
    }

    graphics_display_ = gGLWindowManager.CreateFromForeignWindow(WindowHandle, WindowDCHandle, OpenGLRenderingContext);

    if (graphics_display_ == 0)
    {
      nuxDebugMsg("[WindowThread::ThreadCtor] Failed to create the window.");
      return false;
    }

    if (parent_ && parent_->Type().IsObjectType(WindowThread::StaticObjectType))
    {
      // Cancel the effect of PauseThreadGraphicsRendering on the parent window.
      //PostThreadMessage(parent_->GetThreadId(), NUX_THREADMSG_START_RENDERING, (UINT_PTR)((void*)this), 0);
    }

    painter_ = new BasePainter(this);
    timer_manager_ = new TimerHandler(this);
    window_compositor_ = new WindowCompositor(this);

    SetThreadState(THREADRUNNING);
    thread_ctor_called_ = true;

    // Set initial states
    int w = graphics_display_->GetWindowWidth();
    int h = graphics_display_->GetWindowHeight();

    graphics_display_->SetViewPort(0, 0, w, h);
    window_compositor_->FormatRenderTargets(w, h);
    window_compositor_->FloatingAreaConfigureNotify(w, h);

    return true;
  }
#elif defined(NO_X11)
#elif defined(NUX_OS_LINUX)
#ifdef NUX_OPENGLES_20
  bool WindowThread::ThreadCtor(Display *X11Display, Window X11Window, EGLContext OpenGLContext)
#else
  bool WindowThread::ThreadCtor(Display *X11Display, Window X11Window, GLXContext OpenGLContext)
#endif
  {
    nuxAssertMsg(thread_ctor_called_ == false, "[WindowThread::ThreadCtor] ThreadCtor should not be entered more than once per WindowThread.");
    if(thread_ctor_called_)
    {
      return true;
    }

    if (RegisterNuxThread(this) == FALSE)
    {
      nuxDebugMsg("[WindowThread::ThreadCtor] Failed to register the WindowThread.");
      return false;
    }

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, this);

    if (X11Display)
    {
      x11display_ = X11Display;
      ownx11display_ = false;
    }
    else
    {
      x11display_ = XOpenDisplay(NULL);
      ownx11display_ = true;
    }

    graphics_display_ = gGLWindowManager.CreateFromForeignWindow(x11display_, X11Window, OpenGLContext);

    if (graphics_display_ == 0)
    {
      nuxDebugMsg("[WindowThread::ThreadCtor] Failed to create the window.");
      return false;
    }

    if (parent_ && parent_->Type().IsObjectType(WindowThread::StaticObjectType))
    {
      // Cancel the effect of PauseThreadGraphicsRendering on the parent window.
      //PostThreadMessage(parent_->GetThreadId(), NUX_THREADMSG_START_RENDERING, (UINT_PTR)((void*)this), 0);
    }

    painter_ = new BasePainter(this);
    timer_manager_ = new TimerHandler(this);
    window_compositor_ = new WindowCompositor(this);

    xim_controller_ = std::make_shared<XIMController>(graphics_display_->GetX11Display());

    SetThreadState(THREADRUNNING);
    thread_ctor_called_ = true;

    // Set initial states
    int w = graphics_display_->GetWindowWidth();
    int h = graphics_display_->GetWindowHeight();

    graphics_display_->SetViewPort(0, 0, w, h);
    window_compositor_->FormatRenderTargets(w, h);
    window_compositor_->FloatingAreaConfigureNotify(w, h);

    return true;
  }
#endif

  bool WindowThread::ThreadDtor()
  {
    if(thread_dtor_called_)
    {
      return true;
    }

    // Cleanup
    RemoveQueuedLayout();

    window_compositor_->BeforeDestructor();

    if (main_layout_)
    {
      main_layout_->UnReference();
    }

    NUX_SAFE_DELETE(window_compositor_);
    NUX_SAFE_DELETE(timer_manager_);
    NUX_SAFE_DELETE(painter_);
    NUX_SAFE_DELETE(theme_);
    NUX_SAFE_DELETE(graphics_display_);

#if defined(NUX_OS_WINDOWS)
    PostThreadMessage(NUX_GLOBAL_OBJECT_INSTANCE(NProcess).GetMainThreadID(),
                       NUX_THREADMSG_THREAD_TERMINATED,
                       NUX_GLOBAL_OBJECT_INSTANCE(NProcess).GetCurrentThreadID(),
                       0);
#elif defined(NUX_OS_LINUX)

#else
#error PostThreadMessage not implemented for this platform.
#endif

    inlSetThreadLocalStorage(ThreadLocal_InalogicAppImpl, 0);
    UnregisterNuxThread(this);
    thread_dtor_called_ = true;
    return true;
  }

  void WindowThread::SetWindowSize(int width, int height)
  {
    if (graphics_display_)
    {
      if (IsEmbeddedWindow())
      {
        // This is a passive way to set the window size through out the NuxGraphics system. This call gets the 
        // current window size and sets its accordingly to all sub-system.
        graphics_display_->ResetWindowSize();
      }
      else
      {
        graphics_display_->SetWindowSize(width, height);
        ReconfigureLayout();
      }
    }
  }

  void WindowThread::SetWindowBackgroundPaintLayer(AbstractPaintLayer *background_layer)
  {
    if (background_layer == NULL)
      return;

    if (window_compositor_)
    {
      window_compositor_->SetBackgroundPaintLayer(background_layer);
      if (main_layout_)
      {
        main_layout_->QueueDraw();
      }
    }
  }

  void WindowThread::AddToDrawList(View *view)
  {
    Area *parent;

    Geometry const& geo = view->GetAbsoluteGeometry();
    parent = view->GetToplevel();

    if (parent && (view != parent))
    {
//       pgeo = parent->GetGeometry();
//       geo.x += pgeo.x;
//       geo.y += pgeo.y;

      if (parent->Type().IsDerivedFromType(BaseWindow::StaticObjectType))
      {
        BaseWindow* window = NUX_STATIC_CAST(BaseWindow*, parent);
        window->_child_need_redraw = true;
      }
    }

    if (view->Type().IsDerivedFromType(BaseWindow::StaticObjectType))
    {
      // If the view is a BaseWindow, allow it to mark itself for redraw, as if it was its own  child.
      BaseWindow* window = NUX_STATIC_CAST(BaseWindow*, view);
      window->_child_need_redraw = true;
    }

    dirty_areas_.push_back(geo);
  }

  void WindowThread::ClearDrawList()
  {
    dirty_areas_.clear();
  }

  std::vector<Geometry> const& WindowThread::GetDrawList() const
  {
    return dirty_areas_;
  }

  bool WindowThread::AddToPresentationList(BaseWindow* bw, bool force)
  {
    if (!bw)
      return false;

    RequestRedraw();

    bool force_or_not_frozen = (force || !foreign_frame_frozen_);
    std::vector<WeakBaseWindowPtr>& target_list = force_or_not_frozen ? presentation_list_embedded_ :
                                                                        presentation_list_embedded_next_frame_;

    if (std::find(target_list.begin(), target_list.end(), bw) != target_list.end())
      return force_or_not_frozen;

    target_list.push_back(WeakBaseWindowPtr(bw));
    return force_or_not_frozen;
  }

  std::vector<nux::Geometry> WindowThread::GetPresentationListGeometries() const
  {
    std::vector<nux::Geometry> presentation_geometries;
    for (auto const& base_window : presentation_list_embedded_)
    {
      if (base_window.IsValid())
      {
        nux::Geometry const& abs_geom = base_window->GetAbsoluteGeometry();
        nux::Geometry const& last_geom = base_window->LastPresentedGeometryInEmbeddedMode();
        presentation_geometries.push_back(abs_geom);

        if (abs_geom != last_geom)
        {
          if (!last_geom.IsNull())
            presentation_geometries.push_back(last_geom);
        }
      }
    }
    return presentation_geometries;
  }

  bool WindowThread::IsEmbeddedWindow() const
  {
    return embedded_window_;
  }

#if defined(NUX_OS_WINDOWS)
  bool WindowThread::ProcessForeignEvent(HWND hWnd, MSG msg, WPARAM wParam, LPARAM lParam, void *data)
#elif defined(USE_X11)
  bool WindowThread::ProcessForeignEvent(XEvent *xevent, void * /* data */)
#else
  bool WindowThread::ProcessForeignEvent()
#endif
  {
    if (graphics_display_->IsPauseThreadGraphicsRendering())
    {
      return false;
    }

    Event nux_event;
    memset(&nux_event, 0, sizeof(Event));
#if defined(NUX_OS_WINDOWS)
    graphics_display_->ProcessForeignWin32Event(hWnd, msg, wParam, lParam, &nux_event);
#elif defined(USE_X11)
    graphics_display_->ProcessForeignX11Event(xevent, &nux_event);
#endif

    if (nux_event.type == NUX_TERMINATE_APP || (this->GetThreadState() == THREADSTOP))
    {
      return false;
    }

    if (nux_event.type == NUX_SIZE_CONFIGURATION)
    {
      window_size_configuration_event_ = true;
    }

    int w, h;
    // Call gGfx_OpenGL.getWindowSize after the gGfx_OpenGL.get_event.
    // Otherwise, w and h may not be correct for the current frame if a resizing happened.
    graphics_display_->GetWindowSize(w, h);

    if (nux_event.type == NUX_MOUSE_PRESSED ||
        (nux_event.type == NUX_MOUSE_RELEASED) ||
        (nux_event.type == NUX_MOUSE_DOUBLECLICK) ||
        (nux_event.type == NUX_MOUSE_MOVE) ||
        (nux_event.type == NUX_SIZE_CONFIGURATION) ||
        (nux_event.type == NUX_KEYDOWN) ||
        (nux_event.type == NUX_KEYUP) ||
        (nux_event.type == NUX_NC_WINDOW_CONFIGURATION) ||
        (nux_event.type == NUX_WINDOW_ENTER_FOCUS) ||
        (nux_event.type == NUX_WINDOW_EXIT_FOCUS) ||
        (nux_event.type == NUX_WINDOW_MOUSELEAVE) ||
        (nux_event.type == NUX_MOUSE_WHEEL))
    {
        //DISPATCH EVENT HERE
        //nux_event.Application = Application;
        window_compositor_->ProcessEvent(nux_event);
    }

    if (nux_event.type == NUX_SIZE_CONFIGURATION)
    {
        if (!graphics_display_->isWindowMinimized())
        {
            graphics_display_->SetViewPort(0, 0, nux_event.width, nux_event.height);
            ReconfigureLayout();
            window_compositor_->FormatRenderTargets(nux_event.width, nux_event.height);
        }
        window_compositor_->FloatingAreaConfigureNotify(nux_event.width, nux_event.height);
        window_size_configuration_event_ = true;
    }

    // Some action may have caused layouts and areas to request a recompute. 
    // Process them here before the Draw section.
    if (!graphics_display_->isWindowMinimized())
    {
      if (queue_main_layout_)
      {
        ReconfigureLayout();
      }
      else
      {
        // Compute the layouts that have been queued.
        ComputeQueuedLayout();
      }
    }

    // Warn the host window manager to initiate a draw cycle.
    bool request_draw_cycle_to_host_wm = false;

    if (this->first_pass_)
    {
      request_draw_cycle_to_host_wm = true;
      force_rendering_ = true;
      //window_compositor_->Draw(window_size_configuration_event_, true);
      this->first_pass_ = false;
    }
    else
    {
      bool b = (nux_event.type == NUX_MOUSE_PRESSED) ||
               (nux_event.type == NUX_MOUSE_RELEASED) ||
               (nux_event.type == NUX_MOUSE_DOUBLECLICK) ||
               //(event.type == NUX_MOUSE_MOVE) ||
               (nux_event.type == NUX_SIZE_CONFIGURATION) ||
               (nux_event.type == NUX_KEYDOWN) ||
               (nux_event.type == NUX_KEYUP) ||
               (nux_event.type == NUX_NC_WINDOW_CONFIGURATION) ||
               (nux_event.type == NUX_WINDOW_ENTER_FOCUS) ||
               (nux_event.type == NUX_WINDOW_EXIT_FOCUS) ||
               (nux_event.type == NUX_WINDOW_DIRTY);

      if (b && window_compositor_->IsTooltipActive())
      {
        // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
        window_compositor_->CancelTooltip();
        b |= true;
      }

      if (!window_compositor_->ValidateMouseInsideTooltipArea(nux_event.x, nux_event.y) && window_compositor_->IsTooltipActive())
      {
        // Cancel the tooltip since an event that should cause the tooltip to disappear has occurred.
        window_compositor_->CancelTooltip();
        b |= true;
      }

      if (b || IsRedrawNeeded())
      {
        request_draw_cycle_to_host_wm = true;
      }
      else if (window_compositor_->GetWidgetDrawingOverlay() != 0)
      {
        request_draw_cycle_to_host_wm = true;
      }
    }

    if (!_draw_requested_to_host_wm && request_draw_cycle_to_host_wm)
      RequestRedraw();

    return request_draw_cycle_to_host_wm;
  }

  void WindowThread::PresentWindowsIntersectingGeometryOnThisFrame(Geometry const& rect)
  {
    nuxAssertMsg(IsEmbeddedWindow(),
                 "[WindowThread::PresentWindowIntersectingGeometryOnThisFrame] "
                 "can only be called inside an embedded window");

    window_compositor_->ForEachBaseWindow([&rect] (WeakBaseWindowPtr const& w) {
      if (rect.IsIntersecting(w->GetAbsoluteGeometry()))
        w->PresentInEmbeddedModeOnThisFrame(true);
    });
  }

  void WindowThread::RenderInterfaceFromForeignCmd(Geometry const& clip)
  {
    nuxAssertMsg(IsEmbeddedWindow() == true, "[WindowThread::RenderInterfaceFromForeignCmd] You can only call RenderInterfaceFromForeignCmd if the window was created with CreateFromForeignWindow.");

    if (!IsEmbeddedWindow())
      return;

    IOpenGLShaderProgram::SetShaderTracking(true);

    // Set Nux opengl states. The other plugin in compiz have changed the GPU opengl states.
    // Nux keep tracks of its own opengl states and restore them before doing any drawing.
    GetWindowThread()->GetGraphicsEngine().GetRenderStates().SubmitChangeStates();

    GetWindowThread()->GetGraphicsEngine().SetOpenGLClippingRectangle(0, 0, GetWindowThread()->GetGraphicsEngine().GetWindowWidth(),
        GetWindowThread()->GetGraphicsEngine().GetWindowHeight());

    if (!graphics_display_->IsPauseThreadGraphicsRendering())
    {
      ComputeQueuedLayout();
      GetWindowThread()->GetGraphicsEngine().SetGlobalClippingRectangle(clip);
      window_compositor_->Draw(window_size_configuration_event_, force_rendering_);
      GetWindowThread()->GetGraphicsEngine().DisableGlobalClippingRectangle();
      // When rendering in embedded mode, nux does not attempt to measure the frame rate...

      // Cleanup
      GetWindowThread()->GetGraphicsEngine().ResetStats();
      ClearRedrawFlag();

      window_size_configuration_event_ = false;
      force_rendering_ = false;
    }

    CHECKGL( glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

    IOpenGLShaderProgram::SetShaderTracking(false);
  }

  void WindowThread::ForeignFrameEnded()
  {
    nuxAssertMsg(IsEmbeddedWindow(),
                 "[WindowThread::ForeignFrameEnded] "
                 "can only be called inside an embedded window");

    window_compositor_->ForEachBaseWindow([] (WeakBaseWindowPtr const& w) {
      w->MarkPresentedInEmbeddedMode();
    });

    presentation_list_embedded_.clear();

    foreign_frame_frozen_ = false;

    /* Move all the BaseWindows in presentation_list_embedded_next_frame_
     * to presentation_list_embedded_ and mark them for presentation
     */
    for (auto const& win : presentation_list_embedded_next_frame_)
    {
      if (win.IsValid())
        win->PresentInEmbeddedModeOnThisFrame();
    }

    presentation_list_embedded_next_frame_.clear();
  }

  void WindowThread::ForeignFrameCutoff()
  {
    foreign_frame_frozen_ = true;
  }

  int WindowThread::InstallEventInspector(EventInspector function, void* data)
  {
    NUX_RETURN_VALUE_IF_NULL(function, 0);

    std::map < int, EventInspectorStorage >::iterator it;

    for (it = _event_inspectors_map.begin(); it != _event_inspectors_map.end(); ++it)
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
    new_inspector._uid = NUX_GLOBAL_OBJECT_INSTANCE(UniqueIndex).GetUniqueIndex();

    _event_inspectors_map [new_inspector._uid] = new_inspector;
    return new_inspector._uid;
  }

  bool WindowThread::RemoveEventInspector(int event_inspector_id)
  {
    NUX_RETURN_VALUE_IF_NULL(event_inspector_id, false);

    std::map < int, EventInspectorStorage >::iterator it;

    for (it = _event_inspectors_map.begin(); it != _event_inspectors_map.end(); ++it)
    {
      if ((*it).second._uid == event_inspector_id)
      {
        _event_inspectors_map.erase(it);
        return true;
      }
    }
    return false;
  }

  bool WindowThread::RemoveEventInspector(EventInspector function)
  {
    NUX_RETURN_VALUE_IF_NULL(function, false);

    std::map < int, EventInspectorStorage >::iterator it;

    for (it = _event_inspectors_map.begin(); it != _event_inspectors_map.end(); ++it)
    {
      if ((*it).second._function == function)
      {
        _event_inspectors_map.erase(it);
        return true;
      }
    }
    return false;
  }

  bool WindowThread::CallEventInspectors(Event* event)
  {
    int n = _event_inspectors_map.size();
    if (n == 0)
    {
      // No event inspector installed.
      return false;
    }

    bool discard_event = false;
    std::map < int, EventInspectorStorage >::iterator it;

    for (it = _event_inspectors_map.begin(); it != _event_inspectors_map.end(); ++it)
    {
      EventInspector callback = (*it).second._function;

      if (callback == 0)
        continue;

      int ret = callback(0, event, (*it).second._data);

      if (ret)
      {
        discard_event = true;
      }
    }

    return discard_event;
  }

  bool
  WindowThread::FindDataByFd(const WindowThread::ExternalFdData &data, int fd)
  {
    return data.fd == fd;
  }

  void WindowThread::WatchFdForEvents(int fd, const WindowThread::FdWatchCallback &cb)
  {
    ExternalFdData data;
    data.fd = fd;
    data.cb = cb;

    _external_fds.push_back(data);
    AddFdToGLibLoop(fd,
                    reinterpret_cast <gpointer> (&_external_fds.back()),
                    WindowThread::ExternalSourceCallback);
  }

  void WindowThread::UnwatchFd(int fd)
  {
    using namespace std::placeholders;

    std::list<ExternalFdData>::iterator it =
      std::find_if (_external_fds.begin(), _external_fds.end(),
                    std::bind(FindDataByFd, _1, fd));

    if (it != _external_fds.end())
    {
      RemoveFdFromGLibLoop(&(*it));
      _external_fds.erase (it);
    }
  }
#if defined(NUX_OS_LINUX) && defined(USE_X11)
  void WindowThread::XICFocus(TextEntry* text_entry)
  {
    xim_controller_->FocusInXIC();
    xim_controller_->SetCurrentTextEntry(text_entry);
    graphics_display_->SetCurrentXIC(xim_controller_->GetXIC());
  }

  void WindowThread::XICUnFocus()
  {
    xim_controller_->FocusOutXIC();
  }
#endif

  GraphicsDisplay& WindowThread::GetGraphicsDisplay() const
  {
    return *graphics_display_;
  }

  GraphicsEngine& WindowThread::GetGraphicsEngine() const
  {
    return *graphics_display_->GetGraphicsEngine();
  }

  WindowCompositor& WindowThread::GetWindowCompositor() const
  {
    return *window_compositor_;
  }

  BasePainter& WindowThread::GetPainter() const
  {
    return *painter_;
  }

  TimerHandler& WindowThread::GetTimerHandler() const
  {
    return *timer_manager_;
  }

  UXTheme& WindowThread::GetTheme() const
  {
    if (!theme_)
    {
      LOG_INFO(logger) << "Lazily creating nux::UXTheme";
      const_cast<WindowThread*>(this)->theme_ = new UXTheme();
    }
    return *theme_;
  }

  std::string WindowThread::GetWindowTitle() const
  {
    return window_title_;
  }
}

