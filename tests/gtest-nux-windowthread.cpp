#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <fcntl.h>
#include <unistd.h>
#include <sys/poll.h>

#include <string>
#include <fstream>

#include <memory>
#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>

#include <X11/X.h>

#include "Nux/Nux.h"
#include "Nux/View.h"
#include "Nux/HLayout.h"
#include "Nux/ProgramFramework/TestView.h"


using namespace testing;
using ::testing::Invoke;

namespace
{

class MockFdDispatch
{
  public:
    MOCK_METHOD0(dispatch, void());

    MockFdDispatch(int fd) :
      mFd (fd)
    {
      ON_CALL(*this, dispatch())
        .WillByDefault(Invoke(this, &MockFdDispatch::ClearStream));
    }

  private:

    void ClearStream()
    {
      char buf[2];
      if (read(mFd, reinterpret_cast <void *>(buf), 2) == -1)
        throw std::runtime_error(strerror(errno));
    }

    int mFd;
};

class TestWindowThread : public ::testing::Test
{
  public:

    TestWindowThread()
      : seconds_to_quit(0)
      , quit_thread (NULL)
    {
      nux::NuxInitialize(0);
      wnd_thread.reset (nux::CreateNuxWindow("Nux Window", 300, 200,
                                             nux::WINDOWSTYLE_NORMAL, NULL,
                                             false, TestWindowThread::WindowThreadInit,
                                             this));

      if (pipe2(quit_pipe_, O_CLOEXEC) == -1)
        throw std::runtime_error(strerror(errno));

      if (pipe2(test_quit_pipe_, O_CLOEXEC) == -1)
        throw std::runtime_error(strerror(errno));

      WatchFdForEventsOnRun(quit_pipe_[0],
                            std::bind(&TestWindowThread::QuitMessage,
                                      this));


    }

    ~TestWindowThread()
    {
      if (quit_thread)
        delete quit_thread;

      close(quit_pipe_[0]);
      close(quit_pipe_[1]);
      close(test_quit_pipe_[0]);
      close(test_quit_pipe_[1]);
    }

  protected:

    std::unique_ptr <nux::WindowThread> wnd_thread;
    unsigned int                        seconds_to_quit;
    nux::SystemThread                   *quit_thread;

    void WatchFdForEventsOnRun(int fd,
                               const nux::WindowThread::FdWatchCallback &cb)
    {
      WatchedFd wfd;
      wfd.fd = fd;
      wfd.callback = cb;

      watched_fds.push_back(wfd);
    }

    /* Watchdog threads are really not that great, but ping-ponging between
     * the window and application threads would over-complicate this test */
    void QuitAfter(unsigned int seconds)
    {
      seconds_to_quit = seconds;
      quit_thread = nux::CreateSystemThread(NULL, TestWindowThread::QuitTask, this);
      quit_thread->Start(NULL);
    }

    void WaitForQuit()
    {
      struct pollfd pfd;

      pfd.events = POLLIN;
      pfd.revents = 0;
      pfd.fd = test_quit_pipe_[0];

      if (poll(&pfd, 1, -1) == -1)
        throw std::runtime_error(strerror(errno));

      /* Something was written to the read end of our pipe,
       * we can continue now */
    }

  private:

    typedef struct _WatchedFd
    {
      int                                fd;
      nux::WindowThread::FdWatchCallback callback;
    } WatchedFd;

    static void WindowThreadInit(nux::NThread *, void *data)
    {
      TestWindowThread *tw = reinterpret_cast <TestWindowThread *>(data);

      /* Monitor all nominated fd's */
      for (std::vector<WatchedFd>::iterator it = tw->watched_fds.begin();
           it != tw->watched_fds.end();
           ++it)
      {
        tw->wnd_thread->WatchFdForEvents(it->fd, it->callback);
      }
    }

    static void QuitTask(nux::NThread *, void *data)
    {
      TestWindowThread *tw = reinterpret_cast <TestWindowThread *> (data);

      nux::SleepForMilliseconds(tw->seconds_to_quit * 1000);

      const char *buf = "w\0";
      if (write(tw->quit_pipe_[1],
                reinterpret_cast<void *>(const_cast<char *>(buf)),
                2) == -1)
        throw std::runtime_error(strerror(errno));
    }

    void QuitMessage()
    {
      wnd_thread->ExitMainLoop();

      /* Send a message indicating that the main loop
       * exited and we can safely continue */
      const char *buf = "w\0";
      if (write(test_quit_pipe_[1],
                reinterpret_cast<void *>(const_cast<char *>(buf)),
                2) == -1)
        throw std::runtime_error(strerror(errno));
    }

    std::vector<WatchedFd> watched_fds;

    int quit_pipe_[2];
    int test_quit_pipe_[2];
};

TEST_F(TestWindowThread, Create)
{
  ASSERT_TRUE(wnd_thread != NULL);
  EXPECT_EQ(wnd_thread->GetWindowTitle(), std::string("Nux Window"));
  EXPECT_EQ(wnd_thread->IsModalWindow(), false);

  EXPECT_EQ(wnd_thread->IsComputingLayout(), false);
  EXPECT_EQ(wnd_thread->IsInsideLayoutCycle(), false);
}

TEST_F(TestWindowThread, WatchFd)
{
  int pipefd[2];

  if (pipe2 (pipefd, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));

  MockFdDispatch dispatch(pipefd[0]);

  EXPECT_CALL(dispatch, dispatch());

  WatchFdForEventsOnRun(pipefd[0], std::bind(&MockFdDispatch::dispatch, &dispatch));
  const char *buf = "w\0";
  if (write(pipefd[1], reinterpret_cast <void *> (const_cast <char *> (buf)), 2) == -1)
    throw std::runtime_error (strerror(errno));

  QuitAfter(3);
  wnd_thread->Run(NULL);

  /* We must wait for quit before closing the pipes */
  WaitForQuit();

  close (pipefd[0]);
  close (pipefd[1]);
}

TEST_F(TestWindowThread, MultiWatchFd)
{
  int pipefd[2], pipefd2[2];

  if (pipe2 (pipefd, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));
  if (pipe2 (pipefd2, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));

  MockFdDispatch dispatch(pipefd[0]), dispatch2(pipefd2[0]);

  EXPECT_CALL(dispatch, dispatch());
  EXPECT_CALL(dispatch2, dispatch());

  WatchFdForEventsOnRun(pipefd[0], std::bind(&MockFdDispatch::dispatch, &dispatch));
  WatchFdForEventsOnRun(pipefd2[0], std::bind(&MockFdDispatch::dispatch, &dispatch2));
  const char *buf = "w\0";
  if (write(pipefd[1], reinterpret_cast <void *> (const_cast <char *> (buf)), 2) == -1)
    throw std::runtime_error (strerror(errno));
  if (write(pipefd2[1], reinterpret_cast <void *> (const_cast <char *> (buf)), 2) == -1)
    throw std::runtime_error (strerror(errno));

  QuitAfter(3);
  wnd_thread->Run(NULL);

  /* We must wait for quit before closing the pipes */
  WaitForQuit();

  close (pipefd[0]);
  close (pipefd[1]);
  close (pipefd2[0]);
  close (pipefd2[1]);
}

TEST_F(TestWindowThread, OneFdEvent)
{
  int pipefd[2], pipefd2[2];

  if (pipe2 (pipefd, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));
  if (pipe2 (pipefd2, O_CLOEXEC) == -1)
    throw std::runtime_error(strerror(errno));

  MockFdDispatch dispatch(pipefd[0]), dispatch2(pipefd2[0]);

  EXPECT_CALL(dispatch, dispatch());
  EXPECT_CALL(dispatch2, dispatch()).Times(0);

  WatchFdForEventsOnRun(pipefd[0], std::bind(&MockFdDispatch::dispatch, &dispatch));
  WatchFdForEventsOnRun(pipefd2[0], std::bind(&MockFdDispatch::dispatch, &dispatch2));
  const char *buf = "w\0";
  if (write(pipefd[1], reinterpret_cast <void *> (const_cast <char *> (buf)), 2) == -1)
    throw std::runtime_error (strerror(errno));

  QuitAfter(3);
  wnd_thread->Run(NULL);

  /* We must wait for quit before closing the pipes */
  WaitForQuit();

  close (pipefd[0]);
  close (pipefd[1]);
  close (pipefd2[0]);
  close (pipefd2[1]);
}

class EmbeddedContext : public ::testing::Test
{
  public:

    EmbeddedContext()
      : _display(XOpenDisplay(NULL))
      , _root(DefaultRootWindow(_display))
      , _window(XCreateSimpleWindow(_display,
                                    _root,
                                    0,
                                    0,
                                    300,
                                    200,
                                    0,
                                    0,
                                    0))
      , _visinfo(NULL)
      , _context(NULL)
    {
      XVisualInfo temp;
      XWindowAttributes attrib;
      if(!XGetWindowAttributes(_display, _window, &attrib))
        throw std::runtime_error("failed to get window attributes");

      temp.visualid = XVisualIDFromVisual(attrib.visual);

      int nvinfo = 0;
      _visinfo = XGetVisualInfo(_display, VisualIDMask, &temp, &nvinfo);

      if (!_visinfo || !nvinfo)
        throw std::runtime_error("failed to find visual");

#ifndef NUX_OPENGLES_20
      GLint value = 0;
      glXGetConfig(_display, _visinfo, GLX_USE_GL, &value);

      if (!value)
        std::runtime_error("available visual is not a gl visual");

      _context = glXCreateContext(_display, _visinfo, NULL, true);
      glXMakeCurrent(_display, _window, _context);
#else
      EGLDisplay eglDisplay = eglGetDisplay((EGLNativeDisplayType)_display);
      EGLint major, minor;
      if (!eglInitialize(eglDisplay, &major, &minor))
        throw std::runtime_error("eglInitialize failed");

      const EGLint config_attribs[] = {
          EGL_SURFACE_TYPE,         EGL_WINDOW_BIT,
          EGL_RED_SIZE,             1,
          EGL_GREEN_SIZE,           1,
          EGL_BLUE_SIZE,            1,
          EGL_ALPHA_SIZE,           0,
          EGL_RENDERABLE_TYPE,      EGL_OPENGL_ES2_BIT,
          EGL_CONFIG_CAVEAT,        EGL_NONE,
          EGL_STENCIL_SIZE,	  1,
          EGL_NONE
      };

      const EGLint context_attribs[] = {
          EGL_CONTEXT_CLIENT_VERSION, 2,
          EGL_NONE
      };

      EGLint               count, visualid;
      EGLConfig            configs[1024];

      if (!eglChooseConfig(eglDisplay, config_attribs, configs, 1024, &count))
        throw std::runtime_error("eglChooseConfig failed");

      visualid = temp.visualid;
      EGLConfig config = configs[0];

      for (int i = 0; i < count; ++i)
      {
        EGLint val;
        eglGetConfigAttrib(eglDisplay, configs[i], EGL_NATIVE_VISUAL_ID, &val);
        if (visualid == val)
        {
          config = configs[i];
          break;
        }
      }

      eglBindAPI(EGL_OPENGL_ES_API);
      _surface = eglCreateWindowSurface(eglDisplay, config, _window, 0);
      if (_surface == EGL_NO_SURFACE)
        throw std::runtime_error("eglCreateWindowSurface failed");

      _context = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, context_attribs);
      if (_context == EGL_NO_CONTEXT)
        throw std::runtime_error("eglCreateContext failed");

      if (!eglMakeCurrent(eglDisplay, _surface, _surface, _context))
        throw std::runtime_error("eglMakeCurrent failed");
#endif
    }

    MOCK_METHOD0(verifyInit, void());

    static void nuxInitThreadEntry(nux::NThread *, void *user_data)
    {
      EmbeddedContext *context = reinterpret_cast <EmbeddedContext *> (user_data);
      context->verifyInit();
    }

    void SetUp ()
    {
      EXPECT_CALL(*this, verifyInit());
      nux::NuxInitialize(0);
      _window_thread.reset(nux::CreateFromForeignWindow(_window,
                                                        _context,
                                                        EmbeddedContext::nuxInitThreadEntry,
                                                        this));
      _window_thread->Run(NULL);
      while (g_main_context_iteration(g_main_context_default(), false));
    }

    ~EmbeddedContext ()
    {
#ifndef NUX_OPENGLES_20
      glXDestroyContext(_display, _context);
#else
      EGLDisplay eglDisplay = eglGetDisplay((EGLNativeDisplayType)_display);
      eglDestroyContext(eglDisplay, _context);
      eglDestroySurface(eglDisplay, _surface);
      eglTerminate(eglDisplay);
#endif
      XDestroyWindow (_display, _window);
      XCloseDisplay (_display);
    }

    const std::shared_ptr <nux::WindowThread> &
    WindowThread () const
    {
      return _window_thread;
    }

  private:

    Display *_display;
    Window  _root;
    Window  _window;
    XVisualInfo *_visinfo;
#ifndef NUX_OPENGLES_20
    GLXContext _context;
#else
    EGLSurface _surface;
    EGLContext _context;
#endif

    std::shared_ptr <nux::WindowThread> _window_thread;
};

TEST_F(EmbeddedContext, WindowThreadIsEmbedded)
{
  EXPECT_TRUE(WindowThread()->IsEmbeddedWindow());
}

/* There's not a whole lot we can do to test this at the moment
 * since we can't really mock out the behaviour of GraphicsEngine
 */
TEST_F(EmbeddedContext, PresentViewInEmbeddedReadiesForPresentation)
{
  nux::ObjectPtr <nux::BaseWindow> bw(new nux::BaseWindow(TEXT("")));
  bw->ShowWindow(true, false);
  WindowThread()->ForeignFrameEnded();
  bw->PresentInEmbeddedModeOnThisFrame();
  EXPECT_TRUE(bw->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContext, QueueDrawOnChildInEmbeddedReadiesForPresentation)
{
  nux::ObjectPtr <nux::BaseWindow> bw(new nux::BaseWindow(TEXT("")));
  nux::HLayout* layout = new nux::HLayout(NUX_TRACKER_LOCATION);
  nux::View* view = new nux::TestView("");
  layout->AddView(view, 1);
  bw->SetLayout(layout);
  bw->ShowWindow(true, false);

  /* Draw command is implicitly queued by ShowWindow, remove it */
  view->DoneRedraw();
  WindowThread()->ForeignFrameEnded();

  view->QueueDraw();
  EXPECT_TRUE(bw->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContext, DonePresentViewInEmbeddedMode)
{
  nux::ObjectPtr <nux::BaseWindow> bw(new nux::BaseWindow(TEXT("")));
  bw->PresentInEmbeddedModeOnThisFrame();
  bw->MarkPresentedInEmbeddedMode();
  EXPECT_FALSE(bw->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContext, NoPresentInvisibleOnQueueDraw)
{
  nux::ObjectPtr <nux::BaseWindow> window(new nux::BaseWindow(TEXT("")));
  nux::HLayout* layout = new nux::HLayout(NUX_TRACKER_LOCATION);
  nux::View* view = new nux::TestView("");
  layout->AddView(view, 1);
  window->SetLayout(layout);
  window->ShowWindow(false);
  view->QueueDraw();
  EXPECT_FALSE(window->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContext, NoPresentInvisible)
{
  nux::ObjectPtr <nux::BaseWindow> window(new nux::BaseWindow(TEXT("")));
  window->ShowWindow(false);
  window->PresentInEmbeddedModeOnThisFrame();
  EXPECT_FALSE(window->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContext, AllowPresentationSubsequentQueueDraw)
{
  nux::ObjectPtr <nux::BaseWindow> window(new nux::BaseWindow(TEXT("")));
  nux::HLayout* layout = new nux::HLayout(NUX_TRACKER_LOCATION);
  nux::View* view = new nux::TestView("");
  layout->AddView(view, 1);
  window->SetLayout(layout);

  /* This will call QueueDraw initially and attempt to add
   * the window to the presentation list */
  window->SetGeometry(nux::Geometry(0, 0, 100, 100));
  EXPECT_FALSE(window->AllowPresentationInEmbeddedMode());

  /* This will call it again */
  window->ShowWindow(true);
  EXPECT_TRUE(window->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContext, StillProcessDrawIfInvisible)
{
  nux::ObjectPtr <nux::BaseWindow> window(new nux::BaseWindow(TEXT("")));
  nux::HLayout* layout = new nux::HLayout(NUX_TRACKER_LOCATION);
  nux::View* view = new nux::TestView("");
  layout->AddView(view, 1);
  window->SetLayout(layout);
  window->ShowWindow(false);
  view->QueueDraw();
  nux::Geometry clip(0, 0, 100, 100);
  WindowThread()->RenderInterfaceFromForeignCmd(clip);
  EXPECT_FALSE(view->IsRedrawNeeded());
}

class EmbeddedContextWindow : public EmbeddedContext
{
  public:

    virtual void SetUp()
    {
      EmbeddedContext::SetUp();
      _base_window = nux::ObjectPtr<nux::BaseWindow>(new nux::BaseWindow(TEXT("")));
      _base_window->ShowWindow(true, false);

      /* QueueDraw will call PresentInEmbeddedModeOnThisFrame - we
       * need to unset this state in order to test it properly */
      _base_window->MarkPresentedInEmbeddedMode();
      _base_window->DoneRedraw();
    }

    virtual nux::ObjectPtr <nux::BaseWindow> const &
    Window()
    {
      return _base_window;
    }

  private:

    nux::ObjectPtr <nux::BaseWindow> _base_window;
};

class RedrawRequestVerification
{
  public:

    MOCK_METHOD0(RedrawRequested, void());
};

TEST_F(EmbeddedContextWindow, AllowPresentationRequestsRedraw)
{
  RedrawRequestVerification verification;

  EXPECT_CALL(verification, RedrawRequested());
  WindowThread()->RedrawRequested.connect(sigc::mem_fun (&verification,
                                                          &RedrawRequestVerification::RedrawRequested));
  Window()->PresentInEmbeddedModeOnThisFrame();
}

TEST_F(EmbeddedContextWindow, AllowPresentationAddsToPresentationList)
{
  Window()->PresentInEmbeddedModeOnThisFrame();
  std::vector <nux::Geometry> present_list(WindowThread()->GetPresentationListGeometries());

  ASSERT_EQ(1, present_list.size());
  EXPECT_EQ(present_list[0], Window()->GetAbsoluteGeometry());
}

TEST_F(EmbeddedContextWindow, AfterCutoffAddsToPresentationListNext)
{
  WindowThread()->ForeignFrameEnded();
  WindowThread()->ForeignFrameCutoff();
  ASSERT_TRUE(WindowThread()->GetPresentationListGeometries().empty());

  Window()->PresentInEmbeddedModeOnThisFrame();

  EXPECT_TRUE(WindowThread()->GetPresentationListGeometries().empty());
}

TEST_F(EmbeddedContextWindow, MultipleAllowPresentationAddsToPresentationListUnique)
{
  Window()->PresentInEmbeddedModeOnThisFrame();
  Window()->PresentInEmbeddedModeOnThisFrame();
  std::vector <nux::Geometry> present_list(WindowThread()->GetPresentationListGeometries());

  ASSERT_EQ(1, present_list.size());
  EXPECT_EQ(present_list[0], Window()->GetAbsoluteGeometry());
}

TEST_F(EmbeddedContextWindow, OneSetOfGeometryForRePresentOnUnchangedPosition)
{
  Window()->PresentInEmbeddedModeOnThisFrame();
  Window()->MarkPresentedInEmbeddedMode();
  Window()->PresentInEmbeddedModeOnThisFrame();
  std::vector <nux::Geometry> present_list(WindowThread()->GetPresentationListGeometries());

  ASSERT_EQ(1, present_list.size());
  EXPECT_EQ(present_list[0], Window()->GetAbsoluteGeometry());
}

TEST_F(EmbeddedContextWindow, TwoSetsOfGeometryForRePresentOnChangedPosition)
{
  Window()->PresentInEmbeddedModeOnThisFrame();
  Window()->MarkPresentedInEmbeddedMode();
  Window()->PresentInEmbeddedModeOnThisFrame();
  Window()->SetBaseX(Window()->GetBaseX() + 1);
  std::vector <nux::Geometry> present_list(WindowThread()->GetPresentationListGeometries());

  ASSERT_EQ(2, present_list.size());
  EXPECT_EQ(present_list[0], Window()->GetAbsoluteGeometry());
  EXPECT_EQ(present_list[1], Window()->LastPresentedGeometryInEmbeddedMode());
}

TEST_F(EmbeddedContextWindow, QueueDrawAddsParentToPresentationList)
{
  nux::HLayout* layout = new nux::HLayout(NUX_TRACKER_LOCATION);
  nux::View* view = new nux::TestView("");
  layout->AddView(view, 1);
  Window()->SetLayout(layout);
  view->QueueDraw();
  std::vector <nux::Geometry> present_list(WindowThread()->GetPresentationListGeometries());

  ASSERT_EQ(1, present_list.size());
  EXPECT_EQ(present_list[0], Window()->GetAbsoluteGeometry());
}

class EmbeddedContextMultiWindow : public EmbeddedContext
{
  public:

    virtual void SetUp()
    {
      EmbeddedContext::SetUp();
    }

    const nux::ObjectPtr<nux::BaseWindow> & SpawnWindow()
    {
      _base_windows.push_back(nux::ObjectPtr<nux::BaseWindow>(new nux::BaseWindow(TEXT(""))));
      _base_windows.back()->ShowWindow(true, false);

      /* QueueDraw will call PresentInEmbeddedModeOnThisFrame - we
       * need to unset this state in order to test it properly */
      _base_windows.back()->MarkPresentedInEmbeddedMode();
      return _base_windows.back();
    }

    virtual std::vector <nux::ObjectPtr <nux::BaseWindow> > const &
    Windows()
    {
      return _base_windows;
    }

  private:

    std::vector <nux::ObjectPtr <nux::BaseWindow> > _base_windows;
};

TEST_F(EmbeddedContextMultiWindow, PresentIfIntersectsRectOneWindow)
{
  nux::Geometry geo(0, 0, 100, 100);
  nux::ObjectPtr<nux::BaseWindow> window(SpawnWindow());
  window->SetGeometry(geo);
  WindowThread()->PresentWindowsIntersectingGeometryOnThisFrame(geo);
  EXPECT_TRUE(window->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContextMultiWindow, PresentOnlyOneWindow)
{
  nux::Geometry geo(0, 0, 100, 100);
  nux::Geometry outside_geo(0, 101, 100, 100);
  nux::ObjectPtr<nux::BaseWindow> window(SpawnWindow());
  nux::ObjectPtr<nux::BaseWindow> outside(SpawnWindow());
  window->SetGeometry(geo);
  outside->SetGeometry(outside_geo);

  /* Call ForeignFrameEnded to clear the presentation list set up
   * by making windows visible */
  WindowThread()->ForeignFrameEnded();

  WindowThread()->PresentWindowsIntersectingGeometryOnThisFrame(geo);
  EXPECT_TRUE(window->AllowPresentationInEmbeddedMode());
  EXPECT_FALSE(outside->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContextMultiWindow, PresentBoth)
{
  nux::Geometry geo(0, 0, 100, 101);
  nux::Geometry other_geo(0, 100, 100, 100);
  nux::ObjectPtr<nux::BaseWindow> window(SpawnWindow());
  nux::ObjectPtr<nux::BaseWindow> other(SpawnWindow());
  window->SetGeometry(geo);
  other->SetGeometry(other_geo);
  WindowThread()->PresentWindowsIntersectingGeometryOnThisFrame(geo);
  EXPECT_TRUE(window->AllowPresentationInEmbeddedMode());
  EXPECT_TRUE(other->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContextMultiWindow, ForeignFrameEndedPresentNone)
{
  nux::Geometry geo(0, 0, 100, 100);
  nux::ObjectPtr<nux::BaseWindow> window(SpawnWindow());
  window->SetGeometry(geo);
  WindowThread()->PresentWindowsIntersectingGeometryOnThisFrame(geo);
  WindowThread()->ForeignFrameEnded();
  EXPECT_FALSE(window->AllowPresentationInEmbeddedMode());
  EXPECT_TRUE(WindowThread()->GetPresentationListGeometries().empty());
}

TEST_F(EmbeddedContextMultiWindow, AddToPresentationListFailsAfterCutoff)
{
  nux::ObjectPtr<nux::BaseWindow> windowOne(SpawnWindow());
  nux::ObjectPtr<nux::BaseWindow> windowTwo(SpawnWindow());
  WindowThread()->ForeignFrameEnded();
  ASSERT_TRUE(WindowThread()->GetPresentationListGeometries().empty());
  EXPECT_TRUE(WindowThread()->AddToPresentationList(windowOne.GetPointer(), false));
  EXPECT_FALSE(WindowThread()->GetPresentationListGeometries().empty());
  WindowThread()->ForeignFrameCutoff();
  EXPECT_FALSE(WindowThread()->AddToPresentationList(windowTwo.GetPointer(), false));
}

TEST_F(EmbeddedContextMultiWindow, NoPresentInEmbeddedOnThisFrameAfterFrameCutoff)
{
  nux::ObjectPtr<nux::BaseWindow> windowOne(SpawnWindow());
  nux::ObjectPtr<nux::BaseWindow> windowTwo(SpawnWindow());
  WindowThread()->ForeignFrameEnded();
  windowOne->PresentInEmbeddedModeOnThisFrame();
  WindowThread()->ForeignFrameCutoff();
  windowTwo->PresentInEmbeddedModeOnThisFrame();
  EXPECT_TRUE(windowOne->AllowPresentationInEmbeddedMode());
  EXPECT_FALSE(windowTwo->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContextMultiWindow, PresentInEmbeddedOnThisFrameAfterFrameCutoffIfForced)
{
  nux::ObjectPtr<nux::BaseWindow> windowOne(SpawnWindow());
  nux::ObjectPtr<nux::BaseWindow> windowTwo(SpawnWindow());
  WindowThread()->ForeignFrameEnded();
  windowOne->PresentInEmbeddedModeOnThisFrame();
  WindowThread()->ForeignFrameCutoff();
  windowTwo->PresentInEmbeddedModeOnThisFrame(true);
  EXPECT_TRUE(windowOne->AllowPresentationInEmbeddedMode());
  EXPECT_TRUE(windowTwo->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContextMultiWindow, MoveToPresentationListAfterFrameEndedIfCaughtInCutoff)
{
  nux::ObjectPtr<nux::BaseWindow> windowOne(SpawnWindow());
  nux::ObjectPtr<nux::BaseWindow> windowTwo(SpawnWindow());
  WindowThread()->ForeignFrameEnded();
  windowOne->PresentInEmbeddedModeOnThisFrame();
  WindowThread()->ForeignFrameCutoff();
  windowTwo->PresentInEmbeddedModeOnThisFrame();
  WindowThread()->ForeignFrameEnded();
  EXPECT_FALSE(windowOne->AllowPresentationInEmbeddedMode());
  EXPECT_TRUE(windowTwo->AllowPresentationInEmbeddedMode());
}


}
