#include "Nux.h"
#include "Layout.h"
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
  static GMutex *gLibEventMutex = 0;

  static void nux_glib_threads_lock(void)
  {
    if (gLibEventMutex)
      g_mutex_lock(gLibEventMutex);
  }

  static void nux_glib_threads_unlock(void)
  {
    if (gLibEventMutex)
      g_mutex_unlock(gLibEventMutex);
  }

  struct NuxEventSource
  {
    GSource source;
    GPollFD event_poll_fd;
  };

  typedef struct
  {
    WindowThread *window_thread;
    unsigned int id;
  } TimeoutData;

  Event GetSystemEvent(WindowThread* window_thread)
  {
    return window_thread->GetNextEvent();
  }

  gboolean nux_timeout_dispatch(gpointer user_data)
  {
    bool repeat = false;
    TimeoutData* dd = NUX_STATIC_CAST(TimeoutData*, user_data);
    unsigned int return_code = 1;

    dd->window_thread->_inside_timer_loop = true;
    repeat = GetTimer().ExecTimerHandler(dd->id);
    dd->window_thread->_inside_timer_loop = false;

    if (dd->window_thread->IsEmbeddedWindow())
    {
      dd->window_thread->RedrawRequested.emit();
    }
    else
    {
      Event event = GetSystemEvent(dd->window_thread);
      return_code = dd->window_thread->ProcessEvent(event);
    }

    if ((return_code == 0) && !dd->window_thread->IsEmbeddedWindow())
    {
      g_main_loop_quit(dd->window_thread->main_loop_glib_);
    }

    if (!repeat)
      delete dd;

    return repeat;
  }

  static gboolean nux_event_prepare(GSource * /* source */, gint *timeout)
  {
    nux_glib_threads_lock();

    gboolean retval;
    *timeout = -1;
#if defined(NUX_OS_WINDOWS)
    MSG msg;
    retval = PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE) ? TRUE : FALSE;
#elif defined(NUX_OS_LINUX)
# if defined(USE_X11)
    retval = GetGraphicsDisplay()->HasXPendingEvent() ? TRUE : FALSE;
# else
    retval = false;
# endif

    nux_glib_threads_unlock();
    return retval;
  }

  static gboolean nux_event_check(GSource *source)
  {
    nux_glib_threads_lock();

    gboolean retval = FALSE;
    NuxEventSource *event_source = (NuxEventSource*) source;

    if ((event_source->event_poll_fd.revents & G_IO_IN))
    {
#if defined(NUX_OS_WINDOWS)
      MSG msg;
      retval = PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE) ? TRUE : FALSE;
#elif defined(NUX_OS_LINUX)
#  if defined(USE_X11)
      retval = GetGraphicsDisplay()->HasXPendingEvent() ? TRUE : FALSE;
#  endif
#else
#  error Not implemented.
#endif
    }

    nux_glib_threads_unlock();
    return retval;
  }

  gboolean nux_event_dispatch(GSource * /* source */, GSourceFunc  /* callback */, gpointer user_data)
  {
    nux_glib_threads_lock();
    WindowThread *window_thread = NUX_STATIC_CAST(WindowThread *, user_data);

    Event event = GetSystemEvent(window_thread);
    unsigned int return_code = window_thread->ProcessEvent(event);

    if (return_code == 0 && !window_thread->IsEmbeddedWindow())
    {
      g_main_loop_quit(window_thread->main_loop_glib_);
    }

    nux_glib_threads_unlock();
    return return_code || window_thread->IsEmbeddedWindow();
  }

  static GSourceFuncs event_funcs =
  {
    nux_event_prepare,
    nux_event_check,
    nux_event_dispatch,
    NULL,
    NULL,
    NULL
  };

  // Timeline source functions
  static gboolean nux_timeline_prepare (GSource  * /* source */, gint *timeout)
  {
    // right now we are assuming that we are v-synced, so that will handle synchronizations
    // we could guess how long we have to wait for the next frame but thats rather ugly
    // ideally we need some more API that ensures that timeline/event/re-layout/draws are all in sync

    *timeout = 0;
    return TRUE;
  }

  static gboolean nux_timeline_check(GSource  * /* source */)
  {
    return TRUE;
  }

  static gboolean nux_timeline_dispatch(GSource *source, GSourceFunc /* callback */, gpointer user_data)
  {
#if !defined(NUX_MINIMAL)
    bool has_timelines_left = false;
    nux_glib_threads_lock();
    gint64 micro_secs = g_source_get_time(source);
    WindowThread *window_thread = NUX_STATIC_CAST(WindowThread *, user_data);

    // pump the timelines
    has_timelines_left = window_thread->ProcessTimelines(micro_secs);

    if (!has_timelines_left)
    {
      // no timelines left on the stack so lets go ahead and remove the
      // master clock, to save on wakeups
      window_thread->StopMasterClock();
    }

    nux_glib_threads_unlock();
#endif
    return TRUE;
  }

  static GSourceFuncs timeline_funcs =
  {
    nux_timeline_prepare,
    nux_timeline_check,
    nux_timeline_dispatch,
    NULL,
    NULL,
    NULL
  };

  void WindowThread::InitGlibLoop()
  {
    static bool main_context_created = false;

    if (!IsEmbeddedWindow())
    {

#if GLIB_MAJOR_VERSION < 2 || GLIB_MAJOR_VERSION == 2 && GLIB_MINOR_VERSION < 32
      static bool gthread_initialized = false;

      if (!gthread_initialized)
        g_thread_init(NULL);

      gthread_initialized = true;
#endif

      if (((main_loop_glib_context_ == 0) || (main_loop_glib_ == 0)) && (main_context_created == false))
      {
        //create a context
        main_loop_glib_context_ = g_main_context_default();
        //create a main loop with context
        main_loop_glib_ = g_main_loop_new(main_loop_glib_context_, TRUE);
      }
      else if ((main_loop_glib_context_ == 0) || (main_loop_glib_ == 0))
      {
        // Secondary physical windows goes in here
        //create a context
        main_loop_glib_context_ = g_main_context_new();
        //create a main loop with context
        main_loop_glib_ = g_main_loop_new(main_loop_glib_context_, TRUE);
      }
      else
      {
        return;
      }
    }

    main_context_created = true;

    gLibEventMutex = 0; //g_mutex_new();

  }

  void WindowThread::RunGlibLoop()
  {
    GSource *source = g_source_new(&event_funcs, sizeof(NuxEventSource));
    NuxEventSource *event_source = (NuxEventSource*) source;

    g_source_set_priority(source, G_PRIORITY_DEFAULT);

#if defined(NUX_OS_WINDOWS)
    event_source->event_poll_fd.fd = G_WIN32_MSG_HANDLE;
#elif defined(NUX_OS_LINUX)
#  if defined(USE_X11)
    event_source->event_poll_fd.fd = ConnectionNumber(GetGraphicsDisplay().GetX11Display());
#  endif
#else
#  error Not implemented.
#endif

    event_source->event_poll_fd.events = G_IO_IN;

    g_source_add_poll(source, &event_source->event_poll_fd);
    g_source_set_can_recurse(source, TRUE);
    g_source_set_callback(source, 0, this, 0);

    if (IsEmbeddedWindow())
      g_source_attach(source, NULL);
    else
      g_source_attach(source, main_loop_glib_context_);

#ifdef NUX_GESTURES_SUPPORT
    //TODO: Some people say that it's much faster to share a GSource among many
    // file descriptors than having a separate GSource for each file descriptor.
    // See if it would be better to have GeisAdapter sharing the same GSource with
    // WindowThread/Nux/X
    if (IsEmbeddedWindow())
      geis_adapter_->CreateGSource(nullptr);
    else
      geis_adapter_->CreateGSource(main_loop_glib_context_);

    geis_adapter_->event_ready.connect(
        sigc::mem_fun(this, &WindowThread::ProcessGestureEvent));
#endif

#if !defined(NUX_MINIMAL)
    if (!_Timelines->empty())
      StartMasterClock();
#endif

    if (!IsEmbeddedWindow())
    {
      g_main_loop_run(main_loop_glib_);
      g_main_loop_unref(main_loop_glib_);
    }
  }

  namespace
  {
    typedef struct _ExternalNuxSource
    {
      GSource source;
      GPollFD pfd;
    } ExternalNuxSource;

    gboolean ExternalSourcePrepareFunc(GSource *source, gint *timeout)
    {
      /* Always block for new events */
      *timeout = -1;
      return FALSE;
    }

    gboolean ExternalSourceCheckFunc(GSource *source)
    {
      /* Only return true if there are events waiting for us */
      ExternalNuxSource *extSource =
          reinterpret_cast <ExternalNuxSource *>(source);
      return extSource->pfd.revents == G_IO_IN;
    }

    gboolean ExternalSourceDispatchFunc(GSource     *source,
                                        GSourceFunc callback,
                                        gpointer    user_data)
    {
      return (*callback) (user_data);
    }

    static GSourceFuncs externalGLibFuncs =
    {
      &ExternalSourcePrepareFunc,
      &ExternalSourceCheckFunc,
      &ExternalSourceDispatchFunc, 
      NULL,
      /* Technically we shouldn't be touching these, but the compiler
       * will complain if we don't */
      0,
      0
    };
  }

  gboolean WindowThread::ExternalSourceCallback(gpointer user_data)
  {
    WindowThread::ExternalFdData *data =
      reinterpret_cast <WindowThread::ExternalFdData *> (user_data);

    data->cb ();
    return TRUE;
  }

  void ExternalGLibSources::AddFdToGLibLoop(gint fd,
                                            gpointer data,
                                            GSourceFunc callback,
                                            GMainContext *context)
  {
    GSource *source = g_source_new(&externalGLibFuncs, sizeof (ExternalNuxSource));
    ExternalNuxSource *extSource =
      reinterpret_cast <ExternalNuxSource *> (source);
    extSource->pfd.fd = fd;
    extSource->pfd.events = G_IO_IN;
    extSource->pfd.revents = 0;

    g_source_add_poll (source, &extSource->pfd);
    g_source_set_callback (source, callback, data, NULL);

    g_source_attach (source, context);
  }

  void ExternalGLibSources::RemoveFdFromGLibLoop(gpointer data)
  {
    g_source_remove_by_user_data(data);
  }

  void WindowThread::AddFdToGLibLoop(int fd,
                                     gpointer data,
                                     GSourceFunc callback)
  {
    external_glib_sources_->AddFdToGLibLoop(fd, data, callback, main_loop_glib_context_);
  }

  void WindowThread::RemoveFdFromGLibLoop(gpointer data)
  {
    external_glib_sources_->RemoveFdFromGLibLoop(data);
  }

  void WindowThread::CleanupGlibLoop()
  {
    g_source_remove_by_funcs_user_data(&event_funcs, this);

    for (std::list<ExternalFdData>::iterator it = _external_fds.begin();
         it != _external_fds.end();
         ++it)
    {
      gpointer data = reinterpret_cast<gpointer>(&(*it));
      external_glib_sources_->RemoveFdFromGLibLoop(data);
    }
  }

  unsigned int WindowThread::AddGLibTimeout(unsigned int duration)
  {
    if (IsEmbeddedWindow())
    {
      TimeoutData* dd = new TimeoutData;
      dd->window_thread = this;
      dd->id = g_timeout_add(duration, nux_timeout_dispatch, dd);

      return dd->id;
    }
    else
    {
      if ((main_loop_glib_context_ == 0) || (main_loop_glib_ == 0))
      {
        //LOG_WARNING(logger) << "Trying to set a timeout before GLib Context is created.\n";
        return 0;
      }

      GSource* timeout_source;

      //create a new time-out source
      timeout_source = g_timeout_source_new(duration);

      TimeoutData* dd = new TimeoutData;
      dd->window_thread = this;
      dd->id = 0;
      //set the callback for this source
      g_source_set_callback(timeout_source, nux_timeout_dispatch, dd, NULL);

      //attach source to context
      dd->id = g_source_attach(timeout_source, main_loop_glib_context_);

      return dd->id;
    }
  }

#if defined(NUX_OS_WINDOWS)
  bool WindowThread::AddChildWindowGlibLoop(WindowThread* wnd_thread)
#else
  bool WindowThread::AddChildWindowGlibLoop(WindowThread* /* wnd_thread */)
#endif
  {
#if defined(NUX_OS_WINDOWS)
    if (wnd_thread == NULL)
    {
      return false;
    }

    if (main_loop_glib_context_ == NULL)
    {
      InitGlibLoop();
    }

    GSource *source = g_source_new(&event_funcs, sizeof(NuxEventSource));
    NuxEventSource *event_source = (NuxEventSource*) source;
    g_source_set_priority(source, G_PRIORITY_DEFAULT);

    event_source->event_poll_fd.fd = (int)((int*)wnd_thread->GetThreadHandle());
    event_source->event_poll_fd.events = G_IO_IN;

    g_source_add_poll(source, &event_source->event_poll_fd);
    g_source_set_can_recurse(source, TRUE);
    g_source_set_callback(source, 0, this, 0);

    if (IsEmbeddedWindow())
      g_source_attach(source, NULL);
    else
      g_source_attach(source, main_loop_glib_context_);
#endif

    return true;
  }

  void WindowThread::StartMasterClock()
  {
    // if we are not embedded and don't have a context yet
    if (!IsEmbeddedWindow() && main_loop_glib_context_ == 0)
      return;

    if (_MasterClock == NULL)
    {
      // make a source for our master clock
      _MasterClock = g_source_new(&timeline_funcs, sizeof(GSource));

      g_source_set_priority(_MasterClock, G_PRIORITY_DEFAULT + 10);
      g_source_set_callback(_MasterClock, 0, this, 0);
      g_source_set_can_recurse(_MasterClock, TRUE);

      if (IsEmbeddedWindow())
        g_source_attach(_MasterClock, NULL);
      else if (main_loop_glib_context_ != 0)
        g_source_attach(_MasterClock, main_loop_glib_context_);

#if !defined(NUX_MINIMAL)
      gint64 micro_secs = g_source_get_time(_MasterClock);
      last_timeline_frame_time_sec_ = micro_secs / 1000000;
      last_timeline_frame_time_usec_ = micro_secs % 1000000;
#endif
    }
  }

  void WindowThread::StopMasterClock()
  {
    if (_MasterClock)
    {
      g_source_remove(g_source_get_id(_MasterClock));
      _MasterClock = NULL;
    }
  }

  void WindowThread::StopGLibLoop()
  {
    // woo no more main loop! this is prolly bad for nux, so erm
    // FIXME!! - Jay take a look at this, make sure just quitting the mainloop
    // is an idea that makes sense(needed for testing)
    if (!IsEmbeddedWindow())
      g_main_loop_quit(main_loop_glib_);
  }

#endif
}
