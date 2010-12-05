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


#ifndef WINDOWTHREAD_H
#define WINDOWTHREAD_H

#include "TimerProc.h"

namespace nux
{

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
  class Timeline;
  struct ClientAreaDraw;

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
  gboolean nux_event_dispatch (GSource *source, GSourceFunc callback, gpointer user_data);
  gboolean nux_timeout_dispatch (gpointer user_data);
#endif

  class WindowThread: public AbstractThread
  {
    NUX_DECLARE_OBJECT_TYPE (WindowThread, AbstractThread);
  public:
    WindowThread (const TCHAR *WindowTitle, unsigned int width, unsigned int height, AbstractThread *Parent, bool Modal);
    ~WindowThread();

    //! Set the main layout for this window user interface.
    /*!
        @param layout The layout of the user interface.
    */
    void SetLayout (Layout *layout);

    //! Compute the layout of this window.
    /*!
        Reconfigure the layout of this window. Start by setting the size of the layout to the size of this window.
    */
    void ReconfigureLayout();

    void TerminateThread();

    //! Start running the user interface
    void RunUserInterface();

    // Event, Drawing
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    void ProcessDraw (GraphicsEngine &GfxContext, bool force_draw);
    void SetWindowTitle (const TCHAR *WindowTitle)
    {
      m_WindowTitle = WindowTitle;
    }

    GraphicsDisplay &GetWindow() const
    {
      return *m_GLWindow;
    }
    GraphicsEngine &GetGraphicsEngine() const
    {
      return *m_GLWindow->GetGraphicsEngine();
    }
    BasePainter &GetPainter() const
    {
      return *m_Painter;
    }
    TimerHandler &GetTimerHandler() const
    {
      return *m_TimerHandler;
    }
    UXTheme &GetTheme() const
    {
      return *m_Theme;
    }

    void SetWindowSize (int width, int height);

    void SetWindowBackgroundPaintLayer (AbstractPaintLayer *bkg);

    void RequestRedraw();

    void ClearRedrawFlag()
    {
      m_RedrawRequested = false;
    }
    bool IsRedrawNeeded() const
    {
      return m_RedrawRequested;
    }

    /*!
      Returns the main layout, a BaseWindow object or 0 as the parent of the area.
      A BaseWindow has no parent so 0 is returned. Also for objects that have not been
      added to the rendering tree, 0 is returned.
    */
    Area* GetTopRenderingParent(Area* area);

    void AddToDrawList (View *view);

    void ClearDrawList ();

    std::vector<Geometry> GetDrawList ();

    // Layout

    //! Schedule a size computation cycle on an area before the rendering is performed.
    /*!
        This list contains the area whose size need to be computed.
        @param area The object that will perform a size computation cycle.
        \sa RefreshLayout.
    */
    void AddObjectToRefreshList (Area *area);

    //! Remove an area from the list of object whose size was scheduled to be computed before the rendering cycle.
    /*!
        @param area The object to remove form the list.
        \sa RefreshLayout, AddObjectToRefreshList.
    */
    void RemoveObjectFromRefreshList (Area *area);

    //! Empty the list that contains the layout that need to be recomputed following the resizing of one of the sub element.
    /*!
        Empty the list that contains the layout that need to be recomputed following the resizing of one of the sub element.
    */
    void EmptyLayoutRefreshList();

    //! Execute the size computation cycle on objects.
    /*
        The objects whose size is to be computed are added to a list with a call to AddObjectToRefreshList.
        Size computation is performed just before the rendering cycle.
        \sa AddObjectToRefreshList
    */
    void RefreshLayout();

    //! Return true if we are computing any layout that is part of this window.
    /*!
        Return true if we are computing any layout that is part of this window.
    */
    bool IsComputingLayout() const
    {
      return m_IsComputingMainLayout;
    }
    void SetComputingLayout (bool b)
    {
      m_IsComputingMainLayout = b;
    }

    bool IsMainLayoutDrawDirty() const;

    //! Compute the layout of a specific element
    void ComputeElementLayout(Area* bo, bool RecurseToTopLevelLayout = false);

    bool IsWaitingforModalWindow()
    {
      return m_bWaitForModalWindow;
    }

    bool IsModalWindow()
    {
      return m_bWaitForModalWindow;
    }

    void SetWindowStyle(WindowStyle wstyle)
    {
      m_WindowStyle = wstyle;
    }

    WindowStyle GetWindowStyle() const
    {
      return m_WindowStyle;
    }

    virtual ThreadState Start( void* arg = NULL );

    WindowCompositor& GetWindowCompositor()
    {
      return *m_window_compositor;
    }
    // should be private

    float GetFrameRate() const;
    t_u32 GetFrameCounter() const;
    t_u32 GetFramePeriodeCounter() const;

    bool IsEmbeddedWindow();

#if defined(NUX_OS_WINDOWS)
    bool ProcessForeignEvent (HWND hWnd, MSG msg, WPARAM wParam, LPARAM lParam, void *data);
#elif defined(NUX_OS_LINUX)
    bool ProcessForeignEvent (XEvent *event, void *data);
#endif

    void RenderInterfaceFromForeignCmd();

  public:
    virtual unsigned int Run (void *);

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
        After ThreadCtor is called, m_ThreadCtorCalled is set to true;
    */
    virtual bool ThreadCtor();

#if defined(NUX_OS_WINDOWS)
    /*!
        Constructor-like function for the thread.
        Will be called by EntryPoint before executing the thread body.
        For the main window, ThreadCtor is called in nux::CreateMainWindow.
        ThreadCtor creates and initialize the following elements:
            - Graphics Window (from the externally created window)
            - Timer
            - Painter
            - Compositor
            - Theme engine
        After ThreadCtor is called, m_ThreadCtorCalled is set to true;


    */
    virtual bool ThreadCtor (HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext);
#elif defined(NUX_OS_LINUX)
    /*!
        Constructor-like function for the thread.
        Will be called by EntryPoint before executing the thread body.
        For the main window, ThreadCtor is called in nux::CreateMainWindow.
        ThreadCtor creates and initialize the following elements:
            - Graphics Window (from the externally created window)
            - Timer
            - Painter
            - Compositor
            - Theme engine
        After ThreadCtor is called, m_ThreadCtorCalled is set to true;


    */
    virtual bool ThreadCtor (Display *X11Display, Window X11Window, GLXContext OpenGLContext);
#endif

    /*!
        Destructor-like function for the thread.
        Will be called by EntryPoint after executing the thread body.
        After ThreadDtor is called, m_ThreadDtorCalled is set to true.
        ThreadDtor is also called in the destructor of the WindowThread but is protected by m_ThreadDtorCalled so it is not called twice.
        In the case of the main window, ThreadDtor is called in the destructor of WindowThread.
    */
    virtual bool ThreadDtor();

    //! Get the main layout of this thread.
    /*!
      @return The main layout of this thread.
    */
    Layout* GetMainLayout();

    /*!
        Add a timeline to our window
    */
    void AddTimeline (Timeline* timeline);
    void RemoveTimeline (Timeline* timeline);
    bool ProcessTimelines (GTimeVal *frame_time);
    long _last_timeline_frame_time_sec;
    long _last_timeline_frame_time_usec;

    void StartMasterClock ();
    void StopMasterClock ();
    /*!
        This pointer maybe set by the user in ThreadInitFunc and reused in ThreadExitFunc
    */
    void *m_InitData;
    void *m_ExitData;

    sigc::signal<void> RedrawRequested;

    bool _inside_main_loop;
    bool _inside_timer_loop;
    bool _pending_wake_up_timer;

    TimerFunctor *_async_wake_up_functor;
    TimerHandle _async_wake_up_timer;

    // quits the main loop.
    void NuxMainLoopQuit ();

    // Automation

#if defined (NUX_OS_WINDOWS)
    /*!
       Used by an external thread to push a fake event for processing.
       Start a 0 delay timer with a call back to ReadyFakeEventProcessing.
       
       @param xevent Simulated XEvent
       @return True if the fake event was successfully registered for later processing.
    */
    bool PumpFakeEventIntoPipe (WindowThread* window_thread, INPUT *win_event);
    
    INPUT _fake_event;
#elif defined (NUX_OS_LINUX)
    /*!
       Used by an external thread to push a fake event for processing.
       Start a 0 delay timer with a call back to ReadyFakeEventProcessing.
       
       @param xevent Simulated XEvent
       @return True if the fake event was successfully registered for later processing.
    */
    bool PumpFakeEventIntoPipe (WindowThread* window_thread, XEvent *xevent);
    
    XEvent _fake_event;
#endif
    
    /*!
        Enable the processing of fake events set through PumpFakeEventIntoPipe.
        Disable the processing of mouse up/down events coming from the display server.
        Process other mouse events normaly.
        
        @param enable True to enable fake events.
        \sa InFakeEventMode
    */
    void SetFakeEventMode (bool enable);
    
    /*!
        Return True if the system is in accepting fake events.
        
        @return True if the fake event mode is active.

    */
    bool InFakeEventMode () const;

    /*!
        Called when the timer set in PumpFakeEventIntoPipe expires.This is the signal that the main 
        thread is ready to process the fake event.
    */
    void ReadyFakeEventProcessing (void*);
    
    /*!
        Fake events are processed one after the other. While this function return false,
        PumpFakeEventIntoPipe should not be called.
    */
    bool ReadyForNextFakeEvent () const;

    bool _ready_for_next_fake_event;
    bool _processing_fake_event;
    bool _fake_event_mode;
    TimerFunctor *_fake_event_call_back;
    TimerHandle _fake_event_timer;

  protected:
    
    void AsyncWakeUpCallback (void*);

    //void SetModalWindow(bool b) {m_bIsModal = b;}

    /*!
        Suspend Win32 Mouse and Keyboard inputs for this window thread and its child thread that are also window (not SystemThread).
    */
    void EnableMouseKeyboardInput();

    /*!
        Enable Win32 Mouse and Keyboard inputs for this window thread and its child thread that are also window (not SystemThread).
    */
    void DisableMouseKeyboardInput();

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    t_u32 ExecutionLoop (t_u32 timer_id);
#else
    t_u32 ExecutionLoop();
#endif

    virtual ThreadState StartChildThread (NThread *thread, bool Modal);
    virtual void AddChildThread (NThread *);
    virtual void RemoveChildThread (NThread *);
    virtual void ChildHasFinished (NThread *app);
    virtual void TerminateAllChildThread();
    virtual ThreadState SuspendChildGraphics (WindowThread *app);

    bool m_bWaitForModalWindow;
    WindowThread *m_ModalWindowThread;

    //typedef Loki::Functor< void, LOKI_TYPELIST_1(void*)   > ChildThreadExitCallback;

    typedef struct
    {
      NThread *thread;
      std::list< sigc::signal<void, void *> > ChildThreadExitCallback;
    } ThreadInfo;

    //std::list<NThread*> m_ChildThread;
    std::list< ThreadInfo * > m_ChildThreadInfo;

  private:

    GSource *_MasterClock;

    WindowThread (const WindowThread &);
    // Does not make sense for a singleton. This is a self assignment.
    WindowThread &operator= (const WindowThread &);
    // Declare operator address-of as private
    WindowThread *operator &();

  private:
    //! This list contains the layout that need to be recomputed following the resizing of one of the sub element.
    /*!
        This list contains the layout that need to be recomputed following the resizing of one of the sub element.
    */
    std::list<Area *> m_LayoutRefreshList;
    std::vector<Geometry> m_dirty_areas;

    //! This variable is true while we are computing the layout the starting from the outmost layout (the Main Layout);
    bool m_IsComputingMainLayout;

  private:
    float m_FrameRate;
    t_u32 m_FrameCounter;
    t_u32 m_FramePeriodeCounter;
    float m_PeriodeTime;

    std::list<Timeline*> *_Timelines;

    bool m_bFirstDrawPass;
    unsigned int m_StartupWidth;
    unsigned int m_StartupHeight;
    NString m_WindowTitle;

    bool m_RedrawRequested;
    Layout *m_AppLayout;

    UXTheme         *m_Theme;
    BasePainter     *m_Painter;
    TimerHandler    *m_TimerHandler;

    GraphicsDisplay *m_GLWindow;
    GraphicsEngine *m_GraphicsContext;
    WindowCompositor *m_window_compositor;
    std::list<NThread *> m_ThreadList;
    bool m_WidgetInitialized;
    WindowStyle m_WindowStyle;
    bool m_bIsModal;

    bool m_ThreadCtorCalled;	//!< True is the thread constructor has been called.

    bool m_ThreadDtorCalled;	//!< True is the thread destructor has been called.

    bool m_embedded_window;		//!< Flag to run the interface in embedded mode.

    /*!
        Record if there was a configuration nux_event (NUX_SIZE_CONFIGURATION) that requires a full redraw.
        Used in the case where event processing and rendering are decoupled (with foreign windows).
    */
    bool m_size_configuration_event;

    bool m_force_redraw;

    friend class BasePainter;
    friend class SystemThread;

#if (defined(NUX_OS_LINUX) || defined(NUX_USE_GLIB_LOOP_ON_WINDOWS)) && (!defined(NUX_DISABLE_GLIB_LOOP))
    GMainLoop *m_GLibLoop;
    GMainContext *m_GLibContext;
    friend gboolean nux_event_dispatch (GSource *source, GSourceFunc callback, gpointer user_data);
    friend gboolean nux_timeout_dispatch (gpointer user_data);

    void InitGlibLoop();
    t_u32 AddGLibTimeout (t_u32 duration);
#endif
    friend class TimerHandler;

    friend WindowThread *CreateGUIThread (const TCHAR *WindowTitle,
                                          t_u32 width,
                                          t_u32 height,
                                          WindowThread *Parent,
                                          ThreadUserInitFunc UserInitFunc,
                                          void *InitData);

    friend WindowThread *CreateWindowThread (WindowStyle WndStyle,
        const TCHAR *WindowTitle,
        t_u32 width,
        t_u32 height,
        WindowThread *Parent,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);

    friend WindowThread *CreateModalWindowThread (WindowStyle WndStyle,
        const TCHAR *WindowTitle,
        t_u32 width,
        t_u32 height,
        WindowThread *Parent,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);

#if defined(NUX_OS_WINDOWS)
    friend WindowThread *CreateFromForeignWindow (HWND WindowHandle, HDC WindowDCHandle, HGLRC OpenGLRenderingContext,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);
#elif defined(NUX_OS_LINUX)
    friend WindowThread *CreateFromForeignWindow (Window X11Window, GLXContext OpenGLContext,
        ThreadUserInitFunc UserInitFunc,
        void *InitData);
#endif

    friend SystemThread *CreateSystemThread (AbstractThread *Parent, ThreadUserInitFunc UserInitFunc, void *InitData);

  };

}

#endif // WINDOWTHREAD_H
