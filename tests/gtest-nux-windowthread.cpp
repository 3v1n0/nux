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

namespace nux {
  std::ostream &
  operator<< (std::ostream &os, const Geometry &geo)
  {
    return os << "Geometry: x: " <<
                 geo.x << " y: " <<
                 geo.y << " width: " <<
                 geo.width << " height: " <<
                 geo.height << std::endl;

  }
}

namespace {

TEST(TestWindowThread, TestCreate)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Nux Window", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  ASSERT_TRUE(wnd_thread != NULL);
  EXPECT_EQ(wnd_thread->GetWindowTitle(), std::string("Nux Window"));
  EXPECT_EQ(wnd_thread->IsModalWindow(), false);

  EXPECT_EQ(wnd_thread->IsComputingLayout(), false);
  EXPECT_EQ(wnd_thread->IsInsideLayoutCycle(), false);

  delete wnd_thread;
}

class EmbeddedContext : public ::testing::Test
{
  public:

    EmbeddedContext()
      : _display (XOpenDisplay(NULL))
      , _root (DefaultRootWindow(_display))
      , _window (XCreateSimpleWindow(_display,
                                     _root,
                                     0,
                                     0,
                                     300,
                                     200,
                                     0,
                                     0,
                                     0))
      , _visinfo (NULL)
      , _context (NULL)
    {
      XVisualInfo temp;
      XWindowAttributes attrib;
      if (!XGetWindowAttributes (_display, _window, &attrib))
        throw std::runtime_error ("failed to get window attributes");

      temp.visualid = XVisualIDFromVisual (attrib.visual);

      int nvinfo = 0;
      _visinfo = XGetVisualInfo (_display, VisualIDMask, &temp, &nvinfo);

      if (!_visinfo || !nvinfo)
        throw std::runtime_error ("failed to find visual");

#ifndef NUX_OPENGLES_20
      GLint value = 0;
      glXGetConfig (_display, _visinfo, GLX_USE_GL, &value);

      if (!value)
        std::runtime_error ("available visual is not a gl visual");

      _context = glXCreateContext(_display, _visinfo, NULL, true);
      glXMakeCurrent(_display, _window, _context);
#else
      EGLDisplay *eglDisplay = eglGetDisplay ((EGLNativeDisplayType)_display);
      EGLint major, minor;
      if (!eglInitialize (eglDisplay, &major, &minor))
        throw std::runtime_error ("eglInitialize failed");

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

      if (!eglChooseConfig (eglDisplay, config_attribs, configs, 1024, &count))
        throw std::runtime_error("eglChooseConfig failed");

      visualid = temp.visualid;
      config = configs[0];

      for (int i = 0; i < count; ++i)
      {
        EGLint val;
        eglGetConfigAttrib (eglDisplay, configs[i], EGL_NATIVE_VISUAL_ID, &val);
        if (visualid == val)
        {
          config = configs[i];
          break;
        }
      }

      eglBindAPI (EGL_OPENGL_ES_API);
      _surface = eglCreateWindowSurface(eglDisplay, config, _window, 0);
      if (_surface == EGL_NO_SURFACE)
        throw std::runtime_error ("eglCreateWindowSurface failed");

      _context = eglCreateContext (eglDisplay, config, EGL_NO_CONTEXT, context_attribs);
      if (_context == EGL_NO_CONTEXT)
        throw std::runtime_error ("eglCreateContext failed");

      if (!eglMakeCurrent (eglDisplay, _surface, _surface, _context))
        throw std::runtime_error ("eglMakeCurrent failed");
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
      EGLDisplay *eglDisplay = eglGetDisplay((EGLNativeDisplayType)_display);
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

  view->QueueDraw();
  EXPECT_TRUE(bw->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContext, DonePresentViewInEmbeddedMode)
{
  nux::ObjectPtr <nux::BaseWindow> bw(new nux::BaseWindow(TEXT("")));
  bw->PresentInEmbeddedModeOnThisFrame();
  bw->WasPresentedInEmbeddedMode();
  EXPECT_FALSE(bw->AllowPresentationInEmbeddedMode());
}

class EmbeddedContextWindow : public EmbeddedContext
{
  public:

    virtual void SetUp ()
    {
      EmbeddedContext::SetUp();
      _base_window = nux::ObjectPtr<nux::BaseWindow> (new nux::BaseWindow(TEXT("")));
      _base_window->ShowWindow(true, false);

      /* QueueDraw will call PresentInEmbeddedModeOnThisFrame - we
       * need to unset this state in order to test it properly */
      _base_window->WasPresentedInEmbeddedMode();
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

  EXPECT_CALL (verification, RedrawRequested());
  WindowThread()->RedrawRequested.connect (sigc::mem_fun (&verification,
                                                          &RedrawRequestVerification::RedrawRequested));
  Window()->PresentInEmbeddedModeOnThisFrame();
}

TEST_F(EmbeddedContextWindow, AllowPresentationAddsToPresentationList)
{
  Window()->PresentInEmbeddedModeOnThisFrame();
  std::vector <nux::Geometry> present_list (WindowThread()->GetPresentationListGeometries());

  ASSERT_EQ(1, present_list.size());
  EXPECT_EQ (present_list[0], Window()->GetAbsoluteGeometry());
}

TEST_F(EmbeddedContextWindow, MultipleAllowPresentationAddsToPresentationListUnique)
{
  Window()->PresentInEmbeddedModeOnThisFrame();
  Window()->PresentInEmbeddedModeOnThisFrame();
  std::vector <nux::Geometry> present_list (WindowThread()->GetPresentationListGeometries());

  ASSERT_EQ(1, present_list.size());
  EXPECT_EQ(present_list[0], Window()->GetAbsoluteGeometry());
}

TEST_F(EmbeddedContextWindow, QueueDrawAddsParentToPresentationList)
{
  nux::HLayout* layout = new nux::HLayout(NUX_TRACKER_LOCATION);
  nux::View* view = new nux::TestView("");
  layout->AddView(view, 1);
  Window()->SetLayout(layout);
  view->QueueDraw();
  std::vector <nux::Geometry> present_list (WindowThread()->GetPresentationListGeometries());

  ASSERT_EQ(1, present_list.size());
  EXPECT_EQ (present_list[0], Window()->GetAbsoluteGeometry());
}

class EmbeddedContextMultiWindow : public EmbeddedContext
{
  public:

    virtual void SetUp ()
    {
      EmbeddedContext::SetUp();
    }

    const nux::ObjectPtr<nux::BaseWindow> & SpawnWindow()
    {
      _base_windows.push_back(nux::ObjectPtr<nux::BaseWindow> (new nux::BaseWindow(TEXT(""))));
      _base_windows.back()->ShowWindow(true, false);

      /* QueueDraw will call PresentInEmbeddedModeOnThisFrame - we
       * need to unset this state in order to test it properly */
      _base_windows.back()->WasPresentedInEmbeddedMode();
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
  nux::Geometry geo (0, 0, 100, 100);
  nux::ObjectPtr<nux::BaseWindow> window(SpawnWindow());
  window->SetGeometry(geo);
  WindowThread()->PresentWindowsIntersectingGeometryOnThisFrame(geo);
  EXPECT_TRUE(window->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContextMultiWindow, PresentOnlyOneWindow)
{
  nux::Geometry geo (0, 0, 100, 100);
  nux::Geometry outside_geo (0, 101, 100, 100);
  nux::ObjectPtr<nux::BaseWindow> window(SpawnWindow());
  nux::ObjectPtr<nux::BaseWindow> outside(SpawnWindow());
  window->SetGeometry(geo);
  outside->SetGeometry(outside_geo);
  WindowThread()->PresentWindowsIntersectingGeometryOnThisFrame(geo);
  EXPECT_TRUE(window->AllowPresentationInEmbeddedMode());
  EXPECT_FALSE(outside->AllowPresentationInEmbeddedMode());
}

TEST_F(EmbeddedContextMultiWindow, PresentBoth)
{
  nux::Geometry geo (0, 0, 100, 101);
  nux::Geometry other_geo (0, 100, 100, 100);
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
  nux::Geometry geo (0, 0, 100, 100);
  nux::ObjectPtr<nux::BaseWindow> window(SpawnWindow());
  window->SetGeometry(geo);
  WindowThread()->PresentWindowsIntersectingGeometryOnThisFrame(geo);
  WindowThread()->ForeignFrameEnded();
  EXPECT_FALSE(window->AllowPresentationInEmbeddedMode());
  EXPECT_TRUE(WindowThread()->GetPresentationListGeometries().empty());
}


}
