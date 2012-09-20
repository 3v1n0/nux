#include <string>
#include <fstream>

#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>
#include <list>

#include "Nux/Nux.h"
#include "Nux/BaseWindow.h"
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

  nux::Area* FindAreaUnderMouse(const nux::Point& mouse_position, nux::NuxEventType event_type)
  {
    return input_area.GetPointer();
  }

  Area* FindKeyFocusArea(unsigned int key_symbol, unsigned long x11_key_code, unsigned long special_keys_state)
  {
    return input_area.GetPointer();
  }

  nux::ObjectPtr<nux::InputArea> input_area;
};

struct TestHLayout : public nux::HLayout
{
  TestHLayout() : input_area(new nux::InputArea()) {}

  nux::Area* FindAreaUnderMouse(const nux::Point& mouse_position, nux::NuxEventType event_type)
  {
    return input_area.GetPointer();
  }

  Area* FindKeyFocusArea(unsigned int key_symbol, unsigned long x11_key_code, unsigned long special_keys_state)
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
    return nux::GetWindowCompositor().GetAreaUnderMouse(mouse_position, event_type, area);
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
  class ReferenceFramebuffer
  {
    public:

      ReferenceFramebuffer ()
      {
        glGenFramebuffersEXT (1, &fboName);
        glGenRenderbuffersEXT (1, &rbName);

        glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, rbName);
        glRenderbufferStorageEXT (GL_RENDERBUFFER_EXT, GL_RGBA8_EXT, 300, 200);
        glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fboName);
        glFramebufferRenderbufferEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, rbName);
      }

      ~ReferenceFramebuffer ()
      {
        glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
        glDeleteRenderbuffers (1, &rbName);
        glDeleteRenderbuffers (1, &fboName);
      }

    GLuint fboName, rbName;
  };
}
TEST_F(TestWindowCompositor, TestRestoreReferenceFramebufferDirect)
{
  ReferenceFramebuffer reference;
  GLint  fbBinding;

  glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &fbBinding);

  ASSERT_EQ (fbBinding, reference.fboName);

  wnd_thread->GetWindowCompositor().SetReferenceFramebuffer(fbBinding, nux::Geometry (0, 0, 300, 200));

  ASSERT_TRUE (wnd_thread->GetWindowCompositor().RestoreReferenceFramebuffer());
  glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &fbBinding);
  ASSERT_EQ (fbBinding, reference.fboName);
}

TEST_F(TestWindowCompositor, TestRestoreReferenceFramebufferThroughRestoreMain)
{
  ReferenceFramebuffer reference;
  GLint  fbBinding;

  glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &fbBinding);

  ASSERT_EQ (fbBinding, reference.fboName);

  wnd_thread->GetWindowCompositor().SetReferenceFramebuffer(fbBinding, nux::Geometry (0, 0, 300, 200));
  wnd_thread->GetWindowCompositor().RestoreMainFramebuffer();
  glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &fbBinding);
  ASSERT_EQ (fbBinding, reference.fboName);
}

TEST_F(TestWindowCompositor, TestNoRestoreReferenceFramebufferDirectIfNoReferenceFramebuffer)
{
  GLint fbBinding;
  ASSERT_FALSE (wnd_thread->GetWindowCompositor().RestoreReferenceFramebuffer());
  glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &fbBinding);
  ASSERT_EQ (fbBinding, 0);
}

TEST_F(TestWindowCompositor, TestRestoreBackbufferThroughRestoreMain)
{
  ReferenceFramebuffer reference;
  GLint  fbBinding;

  glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &fbBinding);

  ASSERT_EQ (fbBinding, reference.fboName);

  wnd_thread->GetWindowCompositor().RestoreMainFramebuffer();
  glGetIntegerv (GL_FRAMEBUFFER_BINDING_EXT, &fbBinding);
  ASSERT_EQ (fbBinding, 0);
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

#endif // NUX_GESTURES_SUPPORT

}
