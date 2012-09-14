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

TEST(TestWindowCompositor, TestSetKeyFocusArea)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Nux Window", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

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

  delete wnd_thread;
}

#ifdef NUX_GESTURES_SUPPORT

/*
  We have two windows: one subscribed to 2-touches' pinch (A) and
  another without any subscriptions (B).

  Send events of a 2-touches' pinch within window A's boundaries.

  Check that the gesture got accepted, that window A got the gesture
  events and that window B didn't get anything.
 */
TEST(TestWindowCompositor, GestureEventsDelivery_1)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Nux Window", 500, 500,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);
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
  delete wnd_thread;
}

/*
  We have two windows: one subscribed to 2-touches' pinch (A) and
  another without any subscriptions (B).

  Send events of a 2-touches' pinch within window B's boundaries.

  Check that the gesture got rejected and that no window got
  any gesture event.
*/
TEST(TestWindowCompositor, GestureEventsDelivery_2)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Nux Window", 500, 500,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);
  nux::WindowCompositor &wnd_compositor = wnd_thread->GetWindowCompositor();
  nux::FakeGestureEvent fake_event;

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
  delete wnd_thread;
}

/*
  There are two input areas in main window's layout. They're both subscribed
  to 2-touches' pinch gestures.
  There's also a window on top that is subscribed to 2-touches' drag gestures.

  Send events of a 2-touches' pinch within that window's boundaries.

  Check that the gesture gets accepted and that only the input area behind that
  window gets the gesture events.
 */
TEST(TestWindowCompositor, GestureEventsDelivery_3)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Nux Window", 500, 500,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);
  nux::WindowCompositor &wnd_compositor = wnd_thread->GetWindowCompositor();
  nux::FakeGestureEvent fake_event;

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
  delete wnd_thread;
}

/*
  Check that if a gesture gets its construction finished only on its end event,
  it still gets accepted and delivered.
 */
TEST(TestWindowCompositor, GestureEventsDelivery_4)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Nux Window", 500, 500,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);
  nux::WindowCompositor &wnd_compositor = wnd_thread->GetWindowCompositor();
  nux::FakeGestureEvent fake_event;

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
  delete wnd_thread;
}

#endif // NUX_GESTURES_SUPPORT

}
