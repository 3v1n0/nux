#include <string>
#include <fstream>

#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>
#include <list>

#include "Nux/Nux.h"
#include "Nux/BaseWindow.h"
#include "Nux/InputAreaProximity.h"
#include "Nux/HLayout.h"
#include "Nux/VLayout.h"
#include "Nux/ProgramFramework/TestView.h"
#ifdef NUX_GESTURES_SUPPORT
#include "NuxGraphics/GestureEvent.h"
#include "FakeGestureEvent.h"
#endif

using namespace testing;

namespace {

#ifdef NUX_GESTURES_SUPPORT
class TestWindow : public nux::BaseWindow
{
  public:
  virtual nux::GestureDeliveryRequest GestureEvent(const nux::GestureEvent &event)
  {
    gesture_events_received.push_back(event);
    return nux::GestureDeliveryRequest::NONE;
  }

  std::list<nux::GestureEvent> gesture_events_received;
};

class TestInputArea : public nux::InputArea
{
  public:
  virtual nux::GestureDeliveryRequest GestureEvent(const nux::GestureEvent &event)
  {
    gesture_events_received.push_back(event);
    return nux::GestureDeliveryRequest::NONE;
  }

  std::list<nux::GestureEvent> gesture_events_received;
};
#endif

struct TestBaseWindow : public nux::BaseWindow
{
  TestBaseWindow() : input_area(new nux::InputArea())
  {
    ShowWindow(true);
  }

  nux::Area* FindAreaUnderMouse(const nux::Point& /* mouse_position */, nux::NuxEventType /* event_type */)
  {
    return input_area.GetPointer();
  }

  Area* FindKeyFocusArea(unsigned int /* key_symbol */, unsigned long /* x11_key_code */, unsigned long /* special_keys_state */)
  {
    return input_area.GetPointer();
  }

  nux::ObjectPtr<nux::InputArea> input_area;
};

struct TestHLayout : public nux::HLayout
{
  TestHLayout() : input_area(new nux::InputArea()) {}

  nux::Area* FindAreaUnderMouse(const nux::Point& /* mouse_position */, nux::NuxEventType /* event_type */)
  {
    return input_area.GetPointer();
  }

  Area* FindKeyFocusArea(unsigned int /* key_symbol */, unsigned long /* x11_key_code */, unsigned long /* special_keys_state */)
  {
    return input_area.GetPointer();
  }

  nux::ObjectPtr<nux::InputArea> input_area;
};

}

namespace nux
{
struct TestWindowCompositor : public testing::Test
{
  TestWindowCompositor()
  {}

  void SetUp()
  {
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("WindowCompositor Test", 300, 200, nux::WINDOWSTYLE_NORMAL,
                                          NULL, false, NULL, NULL));
  }

  void ForceSetKeyFocusArea(nux::InputArea* area)
  {
    nux::GetWindowCompositor().key_focus_area_ = area;
  }

  ObjectWeakPtr<InputArea> const& GetMouseOwnerArea() const
  {
    return nux::GetWindowCompositor().GetMouseOwnerArea();
  }

  void SetMouseOwnerArea(InputArea* area)
  {
    nux::GetWindowCompositor().mouse_owner_area_ = area;
  }

  InputArea* GetMouseOverArea()
  {
    return nux::GetWindowCompositor().mouse_over_area_.GetPointer();
  }

  void SetMouseOverArea(InputArea* area)
  {
    nux::GetWindowCompositor().mouse_over_area_ = area;
  }

  void GetAreaUnderMouse(const Point& mouse_position, NuxEventType event_type,
                         ObjectWeakPtr<InputArea>& area)
  {
    return nux::GetWindowCompositor().FindAreaUnderMouse(mouse_position, event_type, area);
  }

  void FindKeyFocusArea(NuxEventType event_type, unsigned int key_symbol,
                        unsigned int state,
                        ObjectWeakPtr<InputArea>& key_focus_area,
                        ObjectWeakPtr<BaseWindow>& window)
  {
    return nux::GetWindowCompositor().FindKeyFocusArea(event_type, key_symbol, state, key_focus_area, window);
  }

  boost::shared_ptr<nux::WindowThread> wnd_thread;
};

namespace
{
  typedef void (*NGLGenFramebuffers)(GLsizei, GLuint *);
  typedef void (*NGLGenRenderbuffers)(GLsizei, GLuint *);
  typedef void (*NGLBindFramebuffer)(GLenum, GLuint);
  typedef void (*NGLBindRenderbuffer)(GLenum, GLuint);
  typedef void (*NGLRenderbufferStorage)(GLenum, GLenum, GLsizei, GLsizei);
  typedef void (*NGLFramebufferRenderbuffer)(GLenum, GLenum, GLenum, GLuint);
  typedef void (*NGLDeleteFramebuffers)(GLsizei, const GLuint *);
  typedef void (*NGLDeleteRenderbuffers)(GLsizei, const GLuint *);

  #ifdef NUX_OPENGLES_20
  GLuint NGL_RENDERBUFFER = GL_RENDERBUFFER;
  /* No separate draw or read targets on OpenGL|ES */
  GLuint NGL_DRAW_FRAMEBUFFER = GL_FRAMEBUFFER;
  GLuint NGL_READ_FRAMEBUFFER = GL_FRAMEBUFFER;
  GLuint NGL_DRAW_FRAMEBUFFER_BINDING = GL_FRAMEBUFFER_BINDING;
  GLuint NGL_READ_FRAMEBUFFER_BINDING = GL_FRAMEBUFFER_BINDING;
  GLuint NGL_COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0;
  GLuint NGL_RGBA_STORAGE = GL_RGBA4;
  #else
  GLuint NGL_RENDERBUFFER = GL_RENDERBUFFER_EXT;
  GLuint NGL_DRAW_FRAMEBUFFER = GL_DRAW_FRAMEBUFFER_EXT;
  GLuint NGL_READ_FRAMEBUFFER = GL_READ_FRAMEBUFFER_EXT;
  GLuint NGL_DRAW_FRAMEBUFFER_BINDING = GL_DRAW_FRAMEBUFFER_BINDING_EXT;
  GLuint NGL_READ_FRAMEBUFFER_BINDING = GL_DRAW_FRAMEBUFFER_BINDING_EXT;
  GLuint NGL_COLOR_ATTACHMENT0 = GL_COLOR_ATTACHMENT0_EXT;
  GLuint NGL_RGBA_STORAGE = GL_RGBA8_EXT;
  #endif

  class ReferenceFramebuffer
  {
    public:

      ReferenceFramebuffer ()
      {
        #ifdef NUX_OPENGLES_20
        nglGenFramebuffers = &glGenFramebuffers;
        nglGenRenderbuffers = &glGenRenderbuffers;
        nglBindRenderbuffer = &glBindRenderbuffer;
        nglBindFramebuffer = &glBindFramebuffer;
        nglRenderbufferStorage = &glRenderbufferStorage;
        nglFramebufferRenderbuffer = &glFramebufferRenderbuffer;
        nglDeleteRenderbuffers = &glDeleteRenderbuffers;
        nglDeleteFramebuffers = &glDeleteFramebuffers;
        #else
        nglGenFramebuffers = glGenFramebuffersEXT;
        nglGenRenderbuffers = glGenRenderbuffersEXT;
        nglBindRenderbuffer = glBindRenderbufferEXT;
        nglBindFramebuffer = glBindFramebufferEXT;
        nglRenderbufferStorage = glRenderbufferStorageEXT;
        nglFramebufferRenderbuffer = glFramebufferRenderbufferEXT;
        nglDeleteRenderbuffers = glDeleteRenderbuffersEXT;
        nglDeleteFramebuffers = glDeleteFramebuffersEXT;
        #endif

        nglGenFramebuffers(1, &fboName);
        nglGenRenderbuffers(1, &rbName);

        nglBindRenderbuffer(NGL_RENDERBUFFER, rbName);
        nglRenderbufferStorage(NGL_RENDERBUFFER, NGL_RGBA_STORAGE, 300, 200);
        nglBindFramebuffer(NGL_DRAW_FRAMEBUFFER, fboName);
        nglBindFramebuffer(NGL_READ_FRAMEBUFFER, fboName);
        nglFramebufferRenderbuffer(NGL_DRAW_FRAMEBUFFER, NGL_COLOR_ATTACHMENT0, NGL_RENDERBUFFER, rbName);
      }

      ~ReferenceFramebuffer ()
      {
        nglBindFramebuffer(NGL_DRAW_FRAMEBUFFER, 0);
        nglBindFramebuffer(NGL_READ_FRAMEBUFFER, 0);
        nglDeleteRenderbuffers(1, &rbName);
        nglDeleteFramebuffers(1, &fboName);
      }

      GLuint fboName, rbName;

      /* Each instance of the class needs to keep its own
       * copy of the extension functions as glewInit () needs
       * to be called in the desktop case before they are available */

      NGLGenFramebuffers nglGenFramebuffers;
      NGLGenRenderbuffers nglGenRenderbuffers;
      NGLBindRenderbuffer nglBindRenderbuffer;
      NGLBindFramebuffer nglBindFramebuffer;
      NGLRenderbufferStorage nglRenderbufferStorage;
      NGLFramebufferRenderbuffer nglFramebufferRenderbuffer;
      NGLDeleteRenderbuffers nglDeleteRenderbuffers;
      NGLDeleteFramebuffers nglDeleteFramebuffers; 
  };
}

TEST_F(TestWindowCompositor, TestRestoreReferenceFramebufferDirect)
{
  ReferenceFramebuffer reference;
  GLint  dfbBinding, rfbBinding;

  glGetIntegerv(NGL_DRAW_FRAMEBUFFER_BINDING, &dfbBinding);
  glGetIntegerv(NGL_READ_FRAMEBUFFER_BINDING, &rfbBinding);

  ASSERT_EQ (dfbBinding, reference.fboName);

  wnd_thread->GetWindowCompositor().SetReferenceFramebuffer(dfbBinding, rfbBinding, nux::Geometry (0, 0, 300, 200));

  ASSERT_TRUE (wnd_thread->GetWindowCompositor().RestoreReferenceFramebuffer());
  glGetIntegerv(NGL_DRAW_FRAMEBUFFER_BINDING, &dfbBinding);
  glGetIntegerv(NGL_READ_FRAMEBUFFER_BINDING, &rfbBinding);
  ASSERT_EQ (dfbBinding, reference.fboName);
  ASSERT_EQ (rfbBinding, reference.fboName);
}

TEST_F(TestWindowCompositor, TestRestoreReferenceFramebufferThroughRestoreMain)
{
  ReferenceFramebuffer reference;
  GLint  dfbBinding, rfbBinding;

  glGetIntegerv(NGL_DRAW_FRAMEBUFFER_BINDING, &dfbBinding);
  glGetIntegerv(NGL_READ_FRAMEBUFFER_BINDING, &rfbBinding);

  ASSERT_EQ(dfbBinding, reference.fboName);

  wnd_thread->GetWindowCompositor().SetReferenceFramebuffer(dfbBinding, rfbBinding, nux::Geometry (0, 0, 300, 200));
  wnd_thread->GetWindowCompositor().RestoreMainFramebuffer();
  glGetIntegerv(NGL_DRAW_FRAMEBUFFER_BINDING, &dfbBinding);
  glGetIntegerv(NGL_READ_FRAMEBUFFER_BINDING, &rfbBinding);
  ASSERT_EQ(dfbBinding, reference.fboName);
  ASSERT_EQ(rfbBinding, reference.fboName);
}

TEST_F(TestWindowCompositor, TestRestoreBackbufferThroughRestoreMain)
{
  ReferenceFramebuffer reference;
  GLint dfbBinding, rfbBinding;

  glGetIntegerv(NGL_DRAW_FRAMEBUFFER_BINDING, &dfbBinding);
  glGetIntegerv(NGL_READ_FRAMEBUFFER_BINDING, &rfbBinding);

  ASSERT_EQ (dfbBinding, reference.fboName);

  wnd_thread->GetWindowCompositor().RestoreMainFramebuffer();
  glGetIntegerv(NGL_DRAW_FRAMEBUFFER_BINDING, &dfbBinding);
  glGetIntegerv(NGL_READ_FRAMEBUFFER_BINDING, &rfbBinding);
  ASSERT_EQ(dfbBinding, 0);
  ASSERT_EQ(rfbBinding, 0);
}

TEST_F(TestWindowCompositor, TestSetKeyFocusArea)
{
  nux::TestView* test_view0 = new nux::TestView();
  nux::TestView* test_view1 = new nux::TestView();

  nux::HLayout* layout = new nux::HLayout();

  layout->AddView(test_view0, 1);
  layout->AddView(test_view1, 1);

  wnd_thread->SetLayout(layout);

  // Set keyboard focus on test_view0
  {
    wnd_thread->GetWindowCompositor().SetKeyFocusArea(test_view0);

    // Check that test_view0 has registered a keynav focus event
    EXPECT_EQ(test_view0->registered_begin_keynav_focus_, true);
    EXPECT_EQ(test_view0->registered_end_keynav_focus_, false);

    EXPECT_EQ(test_view1->registered_begin_keynav_focus_, false);
    EXPECT_EQ(test_view1->registered_end_keynav_focus_, false);

    test_view0->ResetKeyFocusEvents();
  }

  // Set keyboard focus on test_view1
  {
    wnd_thread->GetWindowCompositor().SetKeyFocusArea(test_view1);

    EXPECT_EQ(test_view0->registered_begin_keynav_focus_, false);
    // Check that test_view0 has loast the keynav focus.
    EXPECT_EQ(test_view0->registered_end_keynav_focus_, true);

    // Check that test_view1 has registered a keynav focus event
    EXPECT_EQ(test_view1->registered_begin_keynav_focus_, true);
    EXPECT_EQ(test_view1->registered_end_keynav_focus_, false);
  }
}

TEST_F(TestWindowCompositor, TestInputAreaProximityNullValue)
{
  const int prox_size = nux::GetWindowCompositor().GetProximityListSize();
  nux::InputAreaProximity* prox_area = new nux::InputAreaProximity(NULL, 10);

  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), prox_size);
  delete prox_area;
}

#ifdef NUX_GESTURES_SUPPORT

/*
  We have two windows: one subscribed to 2-touches' pinch (A) and
  another without any subscriptions (B).

  Send events of a 2-touches' pinch within window A's boundaries.

  Check that the gesture got accepted, that window A got the gesture
  events and that window B didn't get anything.
 */
TEST_F(TestWindowCompositor, GestureEventsDelivery_1)
{
  nux::WindowCompositor &wnd_compositor = wnd_thread->GetWindowCompositor();
  nux::FakeGestureEvent fake_event;

  TestWindow *target_window = new TestWindow;
  target_window->SetBaseXY(10, 10);
  target_window->SetBaseWidth(90);
  target_window->SetBaseHeight(90);
  target_window->ShowWindow(true);
  target_window->CreateGesturesSubscription(nux::PINCH_GESTURE, 2);

  TestWindow *innocent_window = new TestWindow;
  innocent_window->SetBaseXY(300, 300);
  innocent_window->SetBaseWidth(100);
  innocent_window->SetBaseHeight(100);
  innocent_window->ShowWindow(true);

  g_gesture_acceptance[0] = 0;

  fake_event.type = nux::EVENT_GESTURE_BEGIN;
  fake_event.gesture_id = 0;
  fake_event.gesture_classes = nux::PINCH_GESTURE | nux::ROTATE_GESTURE;
  fake_event.is_direct_touch = false;
  fake_event.focus.x = 30.0f; // hits target_window
  fake_event.focus.y = 30.0f;
  // in touch device's coordinate system (because it's not a direct device).
  // Thus not used by WindowCompositor
  fake_event.touches.push_back(nux::TouchPoint(0, 10.0f, 10.0f));
  fake_event.touches.push_back(nux::TouchPoint(1, 20.0f, 20.0f));
  fake_event.is_construction_finished = false;
  wnd_compositor.ProcessEvent(fake_event.ToGestureEvent()); 

  ASSERT_EQ(0, g_gesture_acceptance[0]);
  ASSERT_EQ(0, target_window->gesture_events_received.size());
  ASSERT_EQ(0, innocent_window->gesture_events_received.size());

  fake_event.type = nux::EVENT_GESTURE_UPDATE;
  fake_event.focus.x += 2.0f;
  fake_event.focus.y += 2.0f;
  fake_event.touches.clear();
  fake_event.touches.push_back(nux::TouchPoint(0, 11.0f, 11.0f));
  fake_event.touches.push_back(nux::TouchPoint(1, 21.0f, 21.0f));
  fake_event.is_construction_finished = true;
  wnd_compositor.ProcessEvent(fake_event.ToGestureEvent()); 

  ASSERT_EQ(1, g_gesture_acceptance[0]);
  ASSERT_EQ(2, target_window->gesture_events_received.size());
  ASSERT_EQ(0, innocent_window->gesture_events_received.size());

  target_window->Dispose();
  innocent_window->Dispose();
}

/*
  We have two windows: one subscribed to 2-touches' pinch (A) and
  another without any subscriptions (B).

  Send events of a 2-touches' pinch within window B's boundaries.

  Check that the gesture got rejected and that no window got
  any gesture event.
*/
TEST_F(TestWindowCompositor, GestureEventsDelivery_2)
{
  nux::FakeGestureEvent fake_event;
  nux::WindowCompositor& wnd_compositor = nux::GetWindowCompositor();

  TestWindow *subscribed_window = new TestWindow;
  subscribed_window->SetBaseXY(10, 10);
  subscribed_window->SetBaseWidth(90);
  subscribed_window->SetBaseHeight(90);
  subscribed_window->ShowWindow(true);
  subscribed_window->CreateGesturesSubscription(nux::PINCH_GESTURE, 2);

  TestWindow *innocent_window = new TestWindow;
  innocent_window->SetBaseXY(300, 300);
  innocent_window->SetBaseWidth(100);
  innocent_window->SetBaseHeight(100);
  innocent_window->ShowWindow(true);

  g_gesture_acceptance[0] = 0;
  fake_event.type = nux::EVENT_GESTURE_BEGIN;
  fake_event.gesture_id = 0;
  fake_event.gesture_classes = nux::PINCH_GESTURE | nux::ROTATE_GESTURE;
  fake_event.is_direct_touch = false;
  fake_event.focus.x = 350.0f; // hits innocent_window
  fake_event.focus.y = 350.0f;
  // in touch device's coordinate system (because it's not a direct device).
  // Thus not used by WindowCompositor
  fake_event.touches.push_back(nux::TouchPoint(0, 10.0f, 10.0f));
  fake_event.touches.push_back(nux::TouchPoint(1, 20.0f, 20.0f));
  fake_event.is_construction_finished = false;
  wnd_compositor.ProcessEvent(fake_event.ToGestureEvent()); 

  ASSERT_EQ(-1, g_gesture_acceptance[0]);
  ASSERT_EQ(0, subscribed_window->gesture_events_received.size());
  ASSERT_EQ(0, innocent_window->gesture_events_received.size());

  subscribed_window->Dispose();
  innocent_window->Dispose();
}

/*
  There are two input areas in main window's layout. They're both subscribed
  to 2-touches' pinch gestures.
  There's also a window on top that is subscribed to 2-touches' drag gestures.

  Send events of a 2-touches' pinch within that window's boundaries.

  Check that the gesture gets accepted and that only the input area behind that
  window gets the gesture events.
 */
TEST_F(TestWindowCompositor, GestureEventsDelivery_3)
{
  nux::FakeGestureEvent fake_event;
  nux::WindowCompositor& wnd_compositor = nux::GetWindowCompositor();

  TestWindow *window = new TestWindow;
  window->SetBaseXY(10, 10);
  window->SetBaseWidth(90);
  window->SetBaseHeight(90);
  window->ShowWindow(true);
  window->CreateGesturesSubscription(nux::DRAG_GESTURE, 2);

  TestInputArea *target_input_area = new TestInputArea;
  target_input_area->CreateGesturesSubscription(nux::PINCH_GESTURE, 2);

  TestInputArea *other_input_area = new TestInputArea;
  other_input_area->CreateGesturesSubscription(nux::PINCH_GESTURE, 2);

  nux::VLayout *root_layout = new nux::VLayout;
  root_layout->AddView(target_input_area); // should have geometry (0, 0, 500, 250)
  root_layout->AddView(other_input_area); // should have geometry (0, 250, 500, 250)
  wnd_thread->SetLayout(root_layout);

  g_gesture_acceptance[0] = 0;

  fake_event.type = nux::EVENT_GESTURE_BEGIN;
  fake_event.gesture_id = 0;
  fake_event.gesture_classes = nux::PINCH_GESTURE | nux::ROTATE_GESTURE;
  fake_event.is_direct_touch = false;
  fake_event.focus.x = 30.0f; // hits window and target_input_area behind it
  fake_event.focus.y = 30.0f;
  // in touch device's coordinate system (because it's not a direct device).
  // Thus not used by WindowCompositor
  fake_event.touches.push_back(nux::TouchPoint(0, 10.0f, 10.0f));
  fake_event.touches.push_back(nux::TouchPoint(1, 20.0f, 20.0f));
  fake_event.is_construction_finished = false;
  wnd_compositor.ProcessEvent(fake_event.ToGestureEvent()); 

  // shouldn't be accepted right away because is_construction_finished == false
  ASSERT_EQ(0, g_gesture_acceptance[0]);
  ASSERT_EQ(0, window->gesture_events_received.size());
  ASSERT_EQ(0, target_input_area->gesture_events_received.size());
  ASSERT_EQ(0, other_input_area->gesture_events_received.size());

  fake_event.type = nux::EVENT_GESTURE_UPDATE;
  fake_event.focus.x += 2.0f;
  fake_event.focus.y += 2.0f;
  fake_event.touches.clear();
  fake_event.touches.push_back(nux::TouchPoint(0, 11.0f, 11.0f));
  fake_event.touches.push_back(nux::TouchPoint(1, 21.0f, 21.0f));
  fake_event.is_construction_finished = true;
  wnd_compositor.ProcessEvent(fake_event.ToGestureEvent()); 

  ASSERT_EQ(1, g_gesture_acceptance[0]);
  ASSERT_EQ(0, window->gesture_events_received.size());
  ASSERT_EQ(2, target_input_area->gesture_events_received.size());
  ASSERT_EQ(0, other_input_area->gesture_events_received.size());

  window->Dispose();
}

/*
  Check that if a gesture gets its construction finished only on its end event,
  it still gets accepted and delivered.
 */
TEST_F(TestWindowCompositor, GestureEventsDelivery_4)
{
  nux::FakeGestureEvent fake_event;
  nux::WindowCompositor& wnd_compositor = nux::GetWindowCompositor();

  TestWindow *window = new TestWindow;
  window->SetBaseXY(10, 10);
  window->SetBaseWidth(90);
  window->SetBaseHeight(90);
  window->ShowWindow(true);
  window->CreateGesturesSubscription(nux::DRAG_GESTURE, 2);

  TestInputArea *target_input_area = new TestInputArea;
  target_input_area->CreateGesturesSubscription(nux::PINCH_GESTURE, 2);

  TestInputArea *other_input_area = new TestInputArea;
  other_input_area->CreateGesturesSubscription(nux::PINCH_GESTURE, 2);

  nux::VLayout *root_layout = new nux::VLayout;
  root_layout->AddView(target_input_area); // should have geometry (0, 0, 500, 250)
  root_layout->AddView(other_input_area); // should have geometry (0, 250, 500, 250)
  wnd_thread->SetLayout(root_layout);

  g_gesture_acceptance[0] = 0;

  fake_event.type = nux::EVENT_GESTURE_BEGIN;
  fake_event.gesture_id = 0;
  fake_event.gesture_classes = nux::PINCH_GESTURE | nux::ROTATE_GESTURE;
  fake_event.is_direct_touch = false;
  fake_event.focus.x = 30.0f; // hits window and target_input_area behind it
  fake_event.focus.y = 30.0f;
  // in touch device's coordinate system (because it's not a direct device).
  // Thus not used by WindowCompositor
  fake_event.touches.push_back(nux::TouchPoint(0, 10.0f, 10.0f));
  fake_event.touches.push_back(nux::TouchPoint(1, 20.0f, 20.0f));
  fake_event.is_construction_finished = false;
  wnd_compositor.ProcessEvent(fake_event.ToGestureEvent()); 

  // shouldn't be accepted right away because is_construction_finished == false
  ASSERT_EQ(0, g_gesture_acceptance[0]);
  ASSERT_EQ(0, window->gesture_events_received.size());
  ASSERT_EQ(0, target_input_area->gesture_events_received.size());
  ASSERT_EQ(0, other_input_area->gesture_events_received.size());

  fake_event.type = nux::EVENT_GESTURE_END;
  fake_event.focus.x += 2.0f;
  fake_event.focus.y += 2.0f;
  fake_event.touches.clear();
  fake_event.touches.push_back(nux::TouchPoint(0, 11.0f, 11.0f));
  fake_event.touches.push_back(nux::TouchPoint(1, 21.0f, 21.0f));
  fake_event.is_construction_finished = true;
  wnd_compositor.ProcessEvent(fake_event.ToGestureEvent()); 

  ASSERT_EQ(1, g_gesture_acceptance[0]);
  ASSERT_EQ(0, window->gesture_events_received.size());
  ASSERT_EQ(2, target_input_area->gesture_events_received.size());
  ASSERT_EQ(0, other_input_area->gesture_events_received.size());

  window->Dispose();
}

TEST_F(TestWindowCompositor, KeyFocusAreaAutomaticallyUnsets)
{
  nux::WindowCompositor& wnd_compositor = nux::GetWindowCompositor();
  nux::InputArea* test_area = new TestInputArea();

  ForceSetKeyFocusArea(test_area);
  ASSERT_EQ(wnd_compositor.GetKeyFocusArea(), test_area);

  test_area->UnReference();
  EXPECT_EQ(wnd_compositor.GetKeyFocusArea(), nullptr);
}

TEST_F(TestWindowCompositor, MouseOverAreaAutomaticallyUnsets)
{
  nux::InputArea* test_area = new TestInputArea();

  SetMouseOverArea(test_area);
  ASSERT_EQ(GetMouseOverArea(), test_area);

  test_area->UnReference();
  EXPECT_EQ(GetMouseOverArea(), nullptr);
}

TEST_F(TestWindowCompositor, MouseOwnerAreaAutomaticallyUnsets)
{
  nux::InputArea* test_area = new TestInputArea();

  SetMouseOwnerArea(test_area);
  ASSERT_EQ(GetMouseOwnerArea(), test_area);

  test_area->UnReference();
  EXPECT_EQ(GetMouseOwnerArea(), nullptr);
}

TEST_F(TestWindowCompositor, GetAreaUnderMouse)
{
  ObjectWeakPtr<InputArea> area;

  TestBaseWindow* test_win = new TestBaseWindow();

  GetAreaUnderMouse(Point(1, 2), NUX_MOUSE_MOVE, area);

  EXPECT_EQ(area.GetPointer(), test_win->input_area.GetPointer());

  test_win->UnReference();
  EXPECT_EQ(area.GetPointer(), nullptr);
}

TEST_F(TestWindowCompositor, GetAreaUnderMouseFallback)
{
  ObjectWeakPtr<InputArea> area;

  TestHLayout* layout = new TestHLayout();
  wnd_thread->SetLayout(layout);

  GetAreaUnderMouse(Point(1, 2), NUX_MOUSE_MOVE, area);

  EXPECT_EQ(area.GetPointer(), layout->input_area.GetPointer());

  wnd_thread->SetLayout(nullptr);
  layout->UnReference();
  EXPECT_EQ(area.GetPointer(), nullptr);
}

TEST_F(TestWindowCompositor, GetFocusedArea)
{
  ObjectWeakPtr<InputArea> area;
  ObjectWeakPtr<BaseWindow> window;

  TestBaseWindow* test_win = new TestBaseWindow();

  FindKeyFocusArea(NUX_KEYUP, 0, 0, area, window);

  EXPECT_EQ(area.GetPointer(), test_win->input_area.GetPointer());
  EXPECT_EQ(window.GetPointer(), test_win);

  test_win->UnReference();
  EXPECT_EQ(area.GetPointer(), nullptr);
  EXPECT_EQ(window.GetPointer(), nullptr);
}

TEST_F(TestWindowCompositor, GetFocusedAreaFallback)
{
  ObjectWeakPtr<InputArea> area;
  ObjectWeakPtr<BaseWindow> window;

  TestHLayout* layout = new TestHLayout();
  wnd_thread->SetLayout(layout);

  FindKeyFocusArea(NUX_KEYUP, 0, 0, area, window);

  EXPECT_EQ(area.GetPointer(), layout->input_area.GetPointer());
  EXPECT_EQ(window.GetPointer(), nullptr);

  wnd_thread->SetLayout(nullptr);
  layout->UnReference();
  EXPECT_EQ(area.GetPointer(), nullptr);
}

TEST_F(TestWindowCompositor, UpdateInputAreaMouseInsideOnRelease)
{
  Event ev;
  ObjectPtr<TestBaseWindow> ia1(new TestBaseWindow());

  ev.type = EVENT_MOUSE_DOWN;
  nux::GetWindowCompositor().ProcessEvent(ev);

  ASSERT_TRUE(ia1->input_area->IsMouseInside());
  ASSERT_TRUE(ia1->input_area->IsMouseOwner());

  ObjectPtr<TestBaseWindow> ia2(new TestBaseWindow());
  ev.type = EVENT_MOUSE_UP;
  nux::GetWindowCompositor().ProcessEvent(ev);

  EXPECT_FALSE(ia1->input_area->IsMouseOwner());
  EXPECT_FALSE(ia1->input_area->IsMouseInside());
  EXPECT_NE(GetMouseOverArea(), ia1->input_area.GetPointer());
}

TEST_F(TestWindowCompositor, UpdateNewInputAreaMouseInsideOnRelease)
{
  Event ev;
  ObjectPtr<TestBaseWindow> ia1(new TestBaseWindow());

  ev.type = EVENT_MOUSE_DOWN;
  nux::GetWindowCompositor().ProcessEvent(ev);

  ASSERT_TRUE(ia1->input_area->IsMouseInside());
  ASSERT_TRUE(ia1->input_area->IsMouseOwner());

  ObjectPtr<TestBaseWindow> ia2(new TestBaseWindow());
  ev.type = EVENT_MOUSE_UP;
  nux::GetWindowCompositor().ProcessEvent(ev);

  EXPECT_FALSE(ia2->input_area->IsMouseOwner());
  EXPECT_TRUE(ia2->input_area->IsMouseInside());
  EXPECT_EQ(GetMouseOverArea(), ia2->input_area.GetPointer());
}

TEST_F(TestWindowCompositor, UpdateNewInputAreaMouseOwnerBeforeSignalEmission)
{
  Event ev;
  ObjectPtr<TestBaseWindow> ia1(new TestBaseWindow());

  ev.type = EVENT_MOUSE_DOWN;
  nux::GetWindowCompositor().ProcessEvent(ev);
  ASSERT_TRUE(ia1->input_area->IsMouseInside());
  ASSERT_TRUE(ia1->input_area->IsMouseOwner());

  bool got_cb = false;
  ia1->input_area->mouse_up.connect([this, &ia1, &got_cb] (int, int, unsigned long, unsigned long) {
    got_cb = true;
    EXPECT_TRUE(ia1->input_area->IsMouseInside());
    EXPECT_FALSE(ia1->input_area->IsMouseOwner());
  });

  ev.type = EVENT_MOUSE_UP;
  nux::GetWindowCompositor().ProcessEvent(ev);

  ASSERT_TRUE(got_cb);
  EXPECT_TRUE(ia1->input_area->IsMouseInside());
  EXPECT_FALSE(ia1->input_area->IsMouseOwner());
  EXPECT_EQ(GetMouseOverArea(), ia1->input_area.GetPointer());
}

TEST_F(TestWindowCompositor, InvalidMouseOverAreaOnReleaseDontCrash)
{
  Event ev;
  ObjectPtr<TestBaseWindow> ia1(new TestBaseWindow());

  ev.type = EVENT_MOUSE_DOWN;
  nux::GetWindowCompositor().ProcessEvent(ev);
  ASSERT_TRUE(ia1->input_area->IsMouseInside());
  ASSERT_TRUE(ia1->input_area->IsMouseOwner());

  // We save a copy of the input area, so that WC still things it's alive but
  // the test window won't return it as the target area for events.
  auto old_mouse_owner_area = ia1->input_area; (void)old_mouse_owner_area;
  ia1->input_area.Release();

  ev.type = EVENT_MOUSE_UP;
  nux::GetWindowCompositor().ProcessEvent(ev);
}

TEST_F(TestWindowCompositor, EmitMouseCancelOnReleasedMouseOwner)
{
  Event ev;
  ObjectPtr<TestBaseWindow> ia1(new TestBaseWindow());

  ev.type = EVENT_MOUSE_DOWN;
  nux::GetWindowCompositor().ProcessEvent(ev);
  ASSERT_TRUE(ia1->input_area->IsMouseOwner());

  bool mouse_cancel_called = false;
  ia1->input_area->mouse_cancel.connect([this, &mouse_cancel_called] {
    mouse_cancel_called = true;
  });

  ev.type = EVENT_MOUSE_UP;
  nux::GetWindowCompositor().ProcessEvent(ev);

  EXPECT_TRUE(mouse_cancel_called);
}

class DraggedWindow : public nux::BaseWindow
{
 public:
  DraggedWindow()
  : mouse_drag_emission_count(0)
  , mouse_drag_x(0)
  , mouse_drag_y(0)
  , mouse_drag_dx(0)
  , mouse_drag_dy(0)
  {}

 protected:
  virtual void EmitMouseDragSignal(int x, int y,
      int dx, int dy,
      unsigned long /*mouse_button_state*/,
      unsigned long /*special_keys_state*/)
  {
    ++mouse_drag_emission_count;
    mouse_drag_x = x;
    mouse_drag_y = y;
    mouse_drag_dx = dx;
    mouse_drag_dy = dy;
  }
 public:
  int mouse_drag_emission_count;
  int mouse_drag_x;
  int mouse_drag_y;
  int mouse_drag_dx;
  int mouse_drag_dy;
};

/*
  Regression test for lp1057995
 */
TEST_F(TestWindowCompositor, MouseDrag)
{
  nux::Event event;

  DraggedWindow *window = new DraggedWindow;
  window->SetBaseXY(60, 70);
  window->SetBaseWidth(200);
  window->SetBaseHeight(200);
  window->ShowWindow(true);

  event.type = NUX_MOUSE_PRESSED;
  event.x = 100;
  event.y = 200;
  nux::GetWindowCompositor().ProcessEvent(event);

  event.type = NUX_MOUSE_MOVE;
  event.x = 50;
  event.y = 250;
  nux::GetWindowCompositor().ProcessEvent(event);

  ASSERT_EQ(1, window->mouse_drag_emission_count);

  /* OBS: they're in local window coordinates */
  ASSERT_EQ(50 - 60, window->mouse_drag_x); 
  ASSERT_EQ(250 - 70, window->mouse_drag_y);
  ASSERT_EQ(50 - 100, window->mouse_drag_dx);
  ASSERT_EQ(250 - 200, window->mouse_drag_dy);

  window->Dispose();
}

class TrackerWindow : public nux::BaseWindow
{
  public:
    TrackerWindow()
    : mouse_up_emission_count(0)
    , mouse_drag_emission_count(0)
    , mouse_drag_dx(0)
    , mouse_drag_dy(0)
    {}

    virtual bool ChildMouseEvent(const nux::Event& event)
    {
      child_mouse_events_received.push_back(event);
      return wants_mouse_ownership;
    }
    std::vector<nux::Event> child_mouse_events_received;
    bool wants_mouse_ownership;

  protected:
    virtual void EmitMouseUpSignal(int, int,
                                   unsigned long,
                                   unsigned long)
    {
      ++mouse_up_emission_count;
    }

    virtual void EmitMouseDragSignal(int, int,
                                     int dx, int dy,
                                     unsigned long,
                                     unsigned long)
    {
      ++mouse_drag_emission_count;
      mouse_drag_dx = dx;
      mouse_drag_dy = dy;
    }
  public:
    int mouse_up_emission_count;
    int mouse_drag_emission_count;
    int mouse_drag_dx;
    int mouse_drag_dy;
};

class TrackedArea : public nux::InputArea
{
  public:
    TrackedArea()
    : mouse_down_emission_count(0)
    , mouse_up_emission_count(0)
    , mouse_drag_emission_count(0)
    , mouse_cancel_emission_count(0)
    {}

  protected:
    virtual void EmitMouseDownSignal(int, int,
                                     unsigned long,
                                     unsigned long)
    {
      ++mouse_down_emission_count;
    }

    virtual void EmitMouseUpSignal(int, int,
                                   unsigned long,
                                   unsigned long)
    {
      ++mouse_up_emission_count;
    }

    virtual void EmitMouseDragSignal(int, int,
                                     int, int,
                                     unsigned long,
                                     unsigned long)
    {
      ++mouse_drag_emission_count;
    }

    virtual void EmitMouseCancelSignal()
    {
      ++mouse_cancel_emission_count;
    }

  public:
    int mouse_down_emission_count;
    int mouse_up_emission_count;
    int mouse_drag_emission_count;
    int mouse_cancel_emission_count;
};

TEST_F(TestWindowCompositor, TrackingChildMouseEvents)
{
  nux::WindowCompositor &wnd_compositor = wnd_thread->GetWindowCompositor();

  TrackedArea *tracked_area = new TrackedArea;
  tracked_area->SetGeometry(0, 0, 100, 1000);
  tracked_area->SetMinimumSize(100, 1000);

  nux::VLayout *layout = new VLayout(NUX_TRACKER_LOCATION);
  layout->AddView(tracked_area, 1, eLeft, eFull);
  layout->Set2DTranslation(0, -123, 0); // x, y, z

  TrackerWindow *tracker_window = new TrackerWindow;
  tracker_window->SetBaseXY(0, 0);
  tracker_window->SetBaseWidth(100);
  tracker_window->SetBaseHeight(100);
  tracker_window->SetTrackChildMouseEvents(true);
  tracker_window->SetLayout(layout);
  tracker_window->ShowWindow(true);

  // Mouse pressed goes both to child and tracker window
  tracker_window->wants_mouse_ownership = false;
  tracked_area->mouse_down_emission_count = 0;
  nux::Event event;
  event.type = NUX_MOUSE_PRESSED;
  event.x = 50; // right in the center of tracker_window
  event.y = 50;
  wnd_compositor.ProcessEvent(event);
  ASSERT_EQ(1, tracker_window->child_mouse_events_received.size());
  ASSERT_EQ(1, tracked_area->mouse_down_emission_count);

  // Mouse move goes both to child and tracker window, but tracker asks
  // for mouse ownership. Therefore child also gets a mouse cancel.
  tracker_window->child_mouse_events_received.clear();
  tracker_window->wants_mouse_ownership = true;
  tracked_area->mouse_drag_emission_count = 0;
  tracked_area->mouse_cancel_emission_count = 0;
  event.type = NUX_MOUSE_MOVE;
  event.y = 60;
  wnd_compositor.ProcessEvent(event);
  ASSERT_EQ(1, tracker_window->child_mouse_events_received.size());
  ASSERT_EQ(1, tracked_area->mouse_drag_emission_count);
  ASSERT_EQ(2, tracked_area->mouse_cancel_emission_count);

  // The second mouse move goes only to the window, but now as a regular
  // mouse event since he's mouse owner now.
  tracker_window->child_mouse_events_received.clear();
  tracker_window->mouse_drag_emission_count = 0;
  tracked_area->mouse_drag_emission_count = 0;
  event.type = NUX_MOUSE_MOVE;
  event.y = 70;
  wnd_compositor.ProcessEvent(event);
  ASSERT_EQ(0, tracker_window->child_mouse_events_received.size());
  ASSERT_EQ(1, tracker_window->mouse_drag_emission_count);
  ASSERT_EQ(0, tracked_area->mouse_drag_emission_count);
  ASSERT_EQ(10, tracker_window->mouse_drag_dy);

  // Mouse release goes only to the window, again as a regular
  // mouse event.
  tracker_window->child_mouse_events_received.clear();
  tracker_window->mouse_up_emission_count = 0;
  tracked_area->mouse_up_emission_count = 0;
  event.type = NUX_MOUSE_RELEASED;
  wnd_compositor.ProcessEvent(event);
  ASSERT_EQ(0, tracker_window->child_mouse_events_received.size());
  ASSERT_EQ(1, tracker_window->mouse_up_emission_count);
  ASSERT_EQ(0, tracked_area->mouse_up_emission_count);

  tracker_window->Dispose();
}

#endif // NUX_GESTURES_SUPPORT

}
