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


#ifndef WINDOWTHREAD_H
#define WINDOWTHREAD_H

#include "TimerProc.h"

#ifdef NUX_GESTURES_SUPPORT
#include "GeisAdapter.h"
#endif

#if defined(NUX_OS_LINUX) && defined(USE_X11)
#include "XIMController.h"
#endif

namespace nux
{

  class BaseWindow;
  class WindowThread;
  class Layout;
  class HLayout;
  class GraphicsDisplay;
  class ClientArea;
  class WindowCompositor;
  class AbstractThread;
  class SystemThread;
  class UXTheme;
  class TimerHandler;
#if !defined(NUX_MINIMAL)
  class Timeline;
#endif
  class Event;
  class Area;
  struct ClientAreaDraw;

  class ExternalGLibSources;
  gboolean nux_event_dispatch(GSource *source, GSourceFunc callback, gpointer user_data);
  gboolean nux_timeout_dispatch(gpointer user_data);

  //! Event Inspector function prototype.
  /*!
      If an event inspector return true, then the event is discarded.
  */
  typedef int(*EventInspector) (Area* area, Event* event, void* data);

  //! Main class of a Nux window app.
  /*!
      Each WindowThread runs in its own loop. There cannot be more than one WindowThread per system thread.
  */
  class WindowThread: public AbstractThread
  {
    NUX_DECLARE_OBJECT_TYPE(WindowThread, AbstractThread);
  public:
    WindowThread(const char *WindowTitle, int width, int height, AbstractThread *Parent, bool Modal);
    virtual ~WindowThread();

    //! Start the WindowThread in the current Thread.
    /*!
        Run the main loop of the window. \n;
        The function blocks until the the main loop is stopped.

        @param ptr Reserved.
    */
    virtual int Run(void *ptr = NULL);

    /*!
        Start the user interface Window in it own thread. Start return immediately after the thread is created.

        @param ptr Reserved.
    */
    virtual ThreadState Start(void *ptr = NULL);

    //! Exit from the WindowThread loop.
    /*!
        Exit the WindowThread loop.
    */
    void ExitMainLoop();

    //! Set window size.
    /*!
        Set window size.

        @param width Window width.
        @param height Window height.
    */
    void SetWindowSize(int width, int height);

    //! Set the background for the window.
    /*!
        Set the background for the window.

        @param background_layer background layer.
    */
    void SetWindowBackgroundPaintLayer(AbstractPaintLayer *background_layer);

    /*!
        Get the graphics display (this is the physical window of this thread).
        @return The graphics display.
    */
    GraphicsDisplay &GetGraphicsDisplay() const;

    /*!
        Get the graphics engine (this is the object that renders the graphics primitives).
        @return The graphics display.
    */
    GraphicsEngine &GetGraphicsEngine() const;

    /*!
        Get the UI compositor (this is processes events and renders ui objects).
        @return The UI compositor.
    */
    WindowCompositor& GetWindowCompositor() const;

    /*!
        Get the painter object.
        @return The painter object.
    */
    BasePainter &GetPainter() const;

    /*!
        Get the timer manager.
        @return The timer manager.
    */
    TimerHandler &GetTimerHandler() const;

    /*!
        Get the UI resource manager (load textures and other data for user interface rendering).
        @param The ui resource manager.
    */
    UXTheme &GetTheme() const;


    //! Set the layout for this window thread.
    /*!
        @param layout The layout of the user interface.
    */
    void SetLayout(Layout *layout);

    //! Get the layout of this window.
    /*!
        @return The layout of this window.
    */
    Layout* GetLayout();

    //! Return true if the process is inside a layout cycle.
    /*!
        @return True if the process is inside a layout cycle.
    */
    bool IsInsideLayoutCycle() const;

    //! Deprecated. Replace with IsInsideLayoutCycle.
    bool IsComputingLayout() const
    {
      return IsInsideLayoutCycle();
    }

    //! Schedule a size computation cycle on an area before the rendering is performed.
    /*!
        This list contains the area whose size need to be computed.
        @param area The object that will perform a size computation cycle.
        \sa ComputeQueuedLayout.

        @return True if the object was succefully queued.
    */
    bool QueueObjectLayout(Area *area);

    //! Compute the layout of a specific element
    /*!
        Immediate size negotiation for a View or a layout.
    */
    void ComputeElementLayout(Area* bo, bool recurse_to_top_level_layout = false);    

    //! Remove an area from the list of object whose size was scheduled to be computed before the rendering cycle.
    /*!
        @param area The object to remove form the list.
        @return True if the object was in the _queued_layout_list and has been removed.
        \sa ComputeQueuedLayout, QueueObjectLayout.
    */
    bool RemoveObjectFromLayoutQueue(Area *area);

    /*!
        Return \i true while waiting for a modal window to return.

        @return \i True while waiting for a modal window to return.
    */
    bool IsWaitingforModalWindow() const;

    /*!
        Return \i true if this window is modal.

        @return \i True if this window is modal.
    */
    bool IsModalWindow() const;

    /*!
        Return true if this windowThread is embedded inside Compiz.

        @return True if embedded inside Compiz.
    */
    bool IsEmbeddedWindow() const;

#if defined(NUX_OS_WINDOWS)
    bool ProcessForeignEvent(HWND hWnd, MSG msg, WPARAM wParam, LPARAM lParam, void *data);
#elif defined(USE_X11)
    bool ProcessForeignEvent(XEvent *event, void *data);
#else
    bool ProcessForeignEvent();
#endif

    /*!
        In embedded mode, allow presentation on any windows intersecting this
        rect. The effect of this is culmulative for the frame, so it can be
        called multiple times with many different rects until
        RenderInterfaceFromForeignCmd is called.
        \sa IsEmbeddedWindow

        @param rect Region of the display to consider for presenting windows
     */
    void PresentWindowsIntersectingGeometryOnThisFrame(Geometry const& rect);

    /*!
        Render the interface. This command is send from the pluging when the window thread is embedded.
        The clip region matches the surface of one single monitor screen, or a region inside that screen.
        \sa IsEmbeddedWindow.

        @param clip Region of the display to render.
    */
    void RenderInterfaceFromForeignCmd(Geometry const& clip);

    /*!
        Used to mark the end of the foreign frame. All calls to PresentInEmbeddedModeOnThisFrame
        are now redirected to this upcoming frame where we will be called next.
     */
    void ForeignFrameEnded();

    /*!
        Used to mark the cutoff point where all calls to PresentInEmbeddedModeOnThisFrame
        should be effective on the next frame, and not this one, because the parent context
        has stopped tracking damage events for this frame
     */
    void ForeignFrameCutoff();

#if !defined(NUX_MINIMAL)
    /*!
        Add a timeline to our window
    */
    void AddTimeline(Timeline* timeline);
    void RemoveTimeline(Timeline* timeline);
    bool ProcessTimelines(gint64 micro_secs);
    long last_timeline_frame_time_sec_;
    long last_timeline_frame_time_usec_;
#endif

    void StartMasterClock();
    void StopMasterClock();

    sigc::signal<void> RedrawRequested;
    sigc::signal<void, int, int, int, int> window_configuration; //!< emmitted when the window Geometry changes.

    //! Set an event inspector function.
    /*!
       Inspect all events and returns the action to be taken for the event(process or discard).

       If \a function as already been added, return its unique id.\n
       If \a function is null, return 0.\n

       @param function Event inspector function callback.
       @param data     User defined data.
       @return         Unique id for the event inspector callback.
    */
    int InstallEventInspector(EventInspector function, void* data);

    //! Remove an event inspector.
    /*!
       Remove the event inspector identified by the provided unique id.

       @param event_inspector_id Unique id for the event inspector.
       @return True              If the event inspector exists and has been removed.
    */
    bool RemoveEventInspector(int event_inspector_id);

    //! Remove an event inspector.
    /*!
       Remove the event inspector identified by the provided function.

       @param function Event inspector function callback.
       @return True    If the event inspector exists and has been removed.
    */
    bool RemoveEventInspector(EventInspector function);

    //! Call event inspectors.
    /*!
        Call event inspectors to have a look at the event.

        @return True if the event should be discarded.
    */
    bool CallEventInspectors(Event* event);

    //! Sets a timer from a different thread.
    /*!
        Sets a timer and a callback. When the timer expires, the callback is executed. This function is meant to be called
        from a different thread. This function very carefully avoid calling any thread specific objects (TLS).

        @param time_ms Timer delay in milliseconds.
        @param timeout_signal Pointer to a TimeOutSignal.
        @param user_data Pointer to user data.

        @return A timer handle.
    */
    TimerHandle SetAsyncTimerCallback(int time_ms, TimeOutSignal* timeout_signal, void *user_data);

    /*!
        Return the Window title.

        @return The window title.
    */
    std::string GetWindowTitle() const;

    void ProcessDraw(GraphicsEngine &graphics_engine, bool force_draw);

    void RequestRedraw();

    void ClearRedrawFlag();

    bool IsRedrawNeeded() const;

    // DrawList - this is a maintained list of areas that will
    // be completely redraw on the next frame
    void AddToDrawList(View *view);
    void ClearDrawList();

    std::vector<Geometry> const& GetDrawList() const;

    // PresentationList - this is a maintained list of areas that
    // will be presented to the reference framebuffer or backbuffer
    // in embedded mode on the next frame
    bool AddToPresentationList(nux::BaseWindow*, bool force);

    std::vector<Geometry> GetPresentationListGeometries() const;

    Event GetNextEvent();

#ifdef NUX_GESTURES_SUPPORT
    /*!
      Simple wrapper for ProcessEvent for connection with GeisAdapter::event_ready
    */
    void ProcessGestureEvent(GestureEvent &event) { ProcessEvent(event); }

    GeisAdapter *GetGeisAdapter() const {return geis_adapter_.get();}
#endif

    typedef std::function<void()> FdWatchCallback;
    void WatchFdForEvents(int fd, const FdWatchCallback &);
    void UnwatchFd(int fd);

#if defined(NUX_OS_LINUX) && defined(USE_X11)
    void XICFocus(TextEntry* text_entry);
    void XICUnFocus();
#endif

  protected:

    /*!
        Constructor-like function for the thread.
        Will be called by EntryPoint before executing the thread body.
        For the main window, ThreadCtor is called in nux::CreateMainWindow.
        ThreadCtor creates and initialize the following elements:
            - Graphics Window
            - Timer
            - Painter
            - Compositor
            - Theme engine
        After ThreadCtor is called, thread_ctor_called_ is set to true;
    */
    virtual bool ThreadCtor();

#if defined(NUX_OS_WINDOWS)
    /*!
        Constructor-like function for the thread.
        Will be called by EntryPoint before executing the thread body.
        For the main window, ThreadCtor is called in nux::CreateMainWindow.
        ThreadCtor creates and initialize the following elements:
            - Graphics Window(from the externally created window)
            - Timer
            - Painter
            - Compositor
            - Theme engine
        After ThreadCtor is called, thread_ctor_called_ is set to true;
        This function is called when Nux is embedded. \sa IsEmbeddedWindow.
    */
    virtual bool ThreadCtor(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext);
#elif defined(USE_X11)
#ifdef NUX_OPENGLES_20
    /*!
        Constructor-like function for the thread.
        Will be called by EntryPoint before executing the thread body.
        For the main window, ThreadCtor is called in nux::CreateMainWindow.
        ThreadCtor creates and initialize the following elements:
            - Graphics Window(from the externally created window)
            - Timer
            - Painter
            - Compositor
            - Theme engine
        After ThreadCtor is called, thread_ctor_called_ is set to true;
        This function is called when Nux is embedded. \sa IsEmbeddedWindow.
    */
    virtual bool ThreadCtor(Display *X11Display, Window X11Window, EGLContext OpenGLContext);
#else
    /*!
        Constructor-like function for the thread.
        Will be called by EntryPoint before executing the thread body.
        For the main window, ThreadCtor is called in nux::CreateMainWindow.
        ThreadCtor creates and initialize the following elements:
            - Graphics Window(from the externally created window)
            - Timer
            - Painter
            - Compositor
            - Theme engine
        After ThreadCtor is called, thread_ctor_called_ is set to true;
        This function is called when Nux is embedded. \sa IsEmbeddedWindow.
    */
    virtual bool ThreadCtor(Display *X11Display, Window X11Window, GLXContext OpenGLContext);
#endif
    Display *x11display_;
    bool     ownx11display_;
#endif

    /*!
        Destructor-like function for the thread.
        Will be called by EntryPoint after executing the thread body.
        After ThreadDtor is called, thread_dtor_called_ is set to true.
        ThreadDtor is also called in the destructor of the WindowThread but is protected by thread_dtor_called_ so it is not called twice.
        In the case of the main window, ThreadDtor is called in the destructor of WindowThread.
    */
    virtual bool ThreadDtor();
    
    //! Causes the Main layout to be recomputed.
    /*!
        Causes the main layout to be recomputed. This will happen just before the next draw cycle.
    */
    void QueueLayout();

    //! Empty the queue of objects set for layout computation.
    /*!
        The queue was filled with calls to QueueObjectLayout.
    */
    void RemoveQueuedLayout();

    //! Compute the layout of this window thread.
    /*!
        Reconfigure the layout of this window. Start by setting the size of the layout to the size of this window.
        ReconfigureLayout is executed following an event of type NUX_SIZE_CONFIGURATION or a call to QueueMainLayout.
        \sa QueueMainLayout.
    */
    void ReconfigureLayout();

    /*!
        Suspend Win32 Mouse and Keyboard inputs for this window thread and its
        child thread that are also window (not SystemThread).
    */
    void EnableMouseKeyboardInput();

    /*!
        Enable Win32 Mouse and Keyboard inputs for this window thread and its
        child thread that are also window (not SystemThread).
    */
    void DisableMouseKeyboardInput();

    /*!
        It does the following:
          * processes the input events
          * resizes views
          * draw the frame
        This function is called when there is an input event, a gesture event or
        when a timer has expired.
    */
    unsigned int ProcessEvent(Event &event);

    virtual ThreadState StartChildThread(AbstractThread *thread, bool Modal);
    virtual void AddChildThread(AbstractThread *);
    virtual void RemoveChildThread(AbstractThread *);
    virtual void ChildHasFinished(AbstractThread *app);
    virtual void TerminateChildThreads();

    virtual ThreadState SuspendChildGraphics(WindowThread *app);

    bool is_modal_window_;
    bool wait_for_modal_window_;
    WindowThread *modal_window_thread_;

  private:
    /*!
      Internally called by ProcessEvent end ExecutionLopp after early return
      check.
     */
    unsigned int DoProcessEvent(Event &event);

    void SetupMainLoop();

    //! Execute the main loop of this thread.
    /*!
        Execute the main loop of this thread.

        @return And exit code. 0 if there was no error.
    */
    int MainLoop();

    //! Custom callback to wake up the main thread and start the execution loop.
    /*!
        This function is executed when \i async_wake_up_signal_ expires. It doesn't do 
        anything when called, but the main thread will wake up and start the execution loop.
        \sa async_wake_up_signal_

        @param user_ptr Pointer to user data.
    */
    void AsyncWakeUpCallback(void *user_ptr);

    TimeOutSignal *async_wake_up_signal_;
    TimerHandle async_wake_up_timer_handle_;

    //! Informs the system of the start of a layout cycle.
    /*!
        This call merely sets a flag to true or false. This flag is used to decided if some actions should be 
        performed or not. Used by the system only.
    */
    void StartLayoutCycle();

    //! Informs the system of the end of a layout cycle.
    /*!
        This call merely sets a flag to true or false. This flag is used to decided if some actions should be 
        performed or not. Used by the system only.
    */
    void StopLayoutCycle();

    //! Execute the size computation cycle on objects.
    /*
        The objects whose size is to be computed are added to a list with a call to QueueObjectLayout.
        Size computation is performed just before the rendering cycle.
        \sa QueueObjectLayout
    */
    void ComputeQueuedLayout();

    GSource *_MasterClock;

    WindowThread(const WindowThread &);
    // Does not make sense for a singleton. This is a self assignment.
    WindowThread& operator = (const WindowThread &);
    // Declare operator address-of as private
    WindowThread* operator & ();

    bool _inside_main_loop;
    bool _inside_timer_loop;
    bool _pending_wake_up_timer;

    //! This list contains the layout that need to be recomputed following the resizing of one of the sub element.
    /*!
        This list contains the layout that need to be recomputed following the resizing of one of the sub element.
    */
    std::list<Area *> _queued_layout_list;
    std::vector<Geometry> dirty_areas_;

    typedef nux::ObjectWeakPtr<nux::BaseWindow> WeakBaseWindowPtr;

    std::vector<WeakBaseWindowPtr> presentation_list_embedded_;

    /*!
        This list contains al lthe windows which will be presented on the next frame
        (eg, after ForeignFrameEnded they are moved into presentation_list_embedded_
         and marked for presentation)
     */
    std::vector<WeakBaseWindowPtr> presentation_list_embedded_next_frame_;

    /*! Whether or not the current frame is "frozen" because the host WM has stopped tracking
        damage events. If so we should put all presentation requests on the next frame instead
        of this one
     */
    bool foreign_frame_frozen_;

    //! This variable is true while we are computing the layout the starting from the outmost layout(the Main Layout);
    bool _inside_layout_cycle;

    //! Set to true to schedule a compute cycle on the main layout.
    bool queue_main_layout_;

#if !defined(NUX_MINIMAL)
    std::list<Timeline*> *_Timelines;
#endif

    bool first_pass_;                     //!< True when going through the ExecutionLoop for the first time.
    unsigned int window_initial_width_;   //!< Window height at startup.
    unsigned int window_initial_height_;  //!< Window width at startup.
    std::string window_title_;            //!< Window title.

    bool _draw_requested_to_host_wm;      //!< Flags signaling that a draw cycle has been requested to the host window manager.
    Layout *main_layout_;

    UXTheme         *theme_;
    BasePainter     *painter_;
    TimerHandler    *timer_manager_;

    //bool created_own_thread_;             //!< If true, create a system thread and run the window in it.
    GraphicsDisplay *graphics_display_;
    WindowCompositor *window_compositor_;
    bool m_WidgetInitialized;
    WindowStyle window_style_;
    
    /*!
        True if the thread constructor has been called.
        This is a form of per-WindowThread reentry protection.
    */
    bool thread_ctor_called_;

    /*!
        True if the thread destructor has been called.
        This is a form of per-WindowThread reentry protection.
    */
    bool thread_dtor_called_;

    /*!
        True when running Ubuntu Unity+Nux has a plugin of Compiz.
    */
    bool embedded_window_;

    /*!
        Record if there was a configuration nux_event(NUX_SIZE_CONFIGURATION) that requires a full redraw.
        Used in the case where event processing and rendering are decoupled(with foreign windows).
    */
    bool window_size_configuration_event_;

    bool force_rendering_;

    typedef struct _EventInspectorStorage
    {
      _EventInspectorStorage()
      {
        _function = 0;
        _data = 0;
        _uid = 0;
      }
      EventInspector  _function;
      void*           _data;
      int             _uid;
    } EventInspectorStorage;
    
    //! Map of events inspectors
    /*!
        Events inspectors get to examine events before they are processed.
        They may also stop an event from being processed if they return true.
    */
    std::map<int, EventInspectorStorage> _event_inspectors_map; //!< map of events inspectors

    typedef struct _ExternalFdData
    {
      int             fd;
      FdWatchCallback cb;
    } ExternalFdData;

    //! List of external sources. This might make more sense as a map,
    //  but providing a struct provides us much nicer GLib integration
    std::list<ExternalFdData> _external_fds;

    static bool FindDataByFd(const WindowThread::ExternalFdData &data, int fd);

    GMainLoop *main_loop_glib_;
    GMainContext *main_loop_glib_context_;
    friend gboolean nux_event_dispatch(GSource *source, GSourceFunc callback, gpointer user_data);
    friend gboolean nux_timeout_dispatch(gpointer user_data);
    std::list<GSource*> child_window_list_;

    std::unique_ptr<ExternalGLibSources> external_glib_sources_;

    void InitGlibLoop();
    void RunGlibLoop();
    void StopGLibLoop();
    void CleanupGlibLoop();
    void AddFdToGLibLoop(int, gpointer, GSourceFunc);
    void RemoveFdFromGLibLoop(gpointer);
    bool AddChildWindowGlibLoop(WindowThread* wnd_thread);

    static gboolean ExternalSourceCallback(gpointer user_data);

    unsigned int AddGLibTimeout(unsigned int duration);

#ifdef NUX_GESTURES_SUPPORT
    std::unique_ptr<GeisAdapter> geis_adapter_;
#endif

#if defined(NUX_OS_LINUX) && defined(USE_X11)
    std::shared_ptr<XIMController> xim_controller_;
#endif

    /*!
        Add a timeout and return the timeout index.
        This function is used internally by Nux.

        @param timeout_delay Time laps before the timeout is fired.
        @return An index for the timeout.
    */
    unsigned int AddTimeout(unsigned int timeout_delay);

    static const int MINIMUM_WINDOW_WIDTH;  //!< Minimum width allowed for a window.
    static const int MINIMUM_WINDOW_HEIGHT; //!< Minimum height allowed for a window.

    friend class TimerHandler;
    friend class BasePainter;
    friend class SystemThread;

    friend WindowThread *CreateGUIThread(const char *WindowTitle,
                                          int width,
                                          int height,
                                          WindowThread *Parent,
                                          ThreadUserInitFunc UserInitFunc,
                                          void *InitData);

    friend WindowThread *CreateNuxWindow(const char *WindowTitle,
      int width,
      int height,
      ThreadUserInitFunc UserInitFunc,
      void *InitData);
    
    friend WindowThread *CreateNuxWindow(const char *window_title,
      int width,
      int height,
      WindowStyle window_border_style,
      AbstractThread *parent,
      bool modal,
      ThreadUserInitFunc user_init_func,
      void *data);

    friend WindowThread *CreateNuxWindowNewThread(const char *window_title,
      int width,
      int height,
      WindowStyle window_border_style,
      AbstractThread *parent,
      bool modal,
      ThreadUserInitFunc user_init_func,
      void *data);

    friend WindowThread *CreateWindowThread(WindowStyle WndStyle,
        const char *WindowTitle,
        int width,
        int height,
        WindowThread *Parent,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);

    friend WindowThread *CreateModalWindowThread(WindowStyle WndStyle,
        const char *WindowTitle,
        int width,
        int height,
        WindowThread *Parent,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);

#if defined(NUX_OS_WINDOWS)
    friend WindowThread *CreateFromForeignWindow(HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);
#elif defined(USE_X11)
#  ifdef NUX_OPENGLES_20
    friend WindowThread *CreateFromForeignWindow (Window X11Window, EGLContext OpenGLContext,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);
#  else
    friend WindowThread *CreateFromForeignWindow (Window X11Window, GLXContext OpenGLContext,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);
#  endif
#elif defined(NO_X11)
    friend WindowThread *CreateFromForeignWindow (EGLDisplay disp, EGLContext OpenGLContext,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);
#endif

    friend SystemThread *CreateSystemThread(AbstractThread *Parent, ThreadUserInitFunc UserInitFunc, void *InitData);

  };

}

#endif // WINDOWTHREAD_H
