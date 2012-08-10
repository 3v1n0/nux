#include <gtest/gtest.h>

#include <Nux/GeisAdapter.h>
#include "geis_mock.h"
#include <list>

class GeisAdapterTest : public ::testing::Test
{
  public:

  GeisAdapterTest()
  {
    adapter_emitted_init_complete = false;
  }

  void AddGestureClass(Geis geis, GeisGestureClass gesture_class)
  {
    GeisEvent event = new struct _GeisEvent;
    event->type = GEIS_EVENT_CLASS_AVAILABLE;
    event->attributes.AddPointer(GEIS_EVENT_ATTRIBUTE_CLASS, gesture_class);
    geis->pending_events.push_back(event);
  }

  void AddDevice(Geis geis, GeisDevice device)
  {
    GeisEvent event = new struct _GeisEvent;
    event->type = GEIS_EVENT_DEVICE_AVAILABLE;
    event->attributes.AddPointer(GEIS_EVENT_ATTRIBUTE_DEVICE, device);
    geis->pending_events.push_back(event);
  }

  void FinishInitialization(Geis geis)
  {
    GeisEvent event = new struct _GeisEvent;
    event->type = GEIS_EVENT_INIT_COMPLETE;
    geis->pending_events.push_back(event);
  }

  void StoreNuxGestureEvent(nux::GestureEvent &gesture_event)
  {
    gesture_events.push_back(gesture_event);
  }

  void MarkInitCompleteAsEmitted()
  {
    adapter_emitted_init_complete = true;
  }

  std::list<nux::GestureEvent> gesture_events;
  bool adapter_emitted_init_complete;
};

/*
  Send a GEIS_EVENT_INIT_COMPLETE and check if GeisAdapter emits init_complete
  and IsInitComplete() returns a correct value.
 */
TEST_F(GeisAdapterTest, SignalsInitComplete)
{
  nux::GeisAdapter geis_adapter;
  Geis geis = geis_adapter.GetGeisInstance();

  ASSERT_FALSE(geis_adapter.IsInitComplete());

  geis_adapter.init_complete.connect(sigc::mem_fun(this,
      &GeisAdapterTest::MarkInitCompleteAsEmitted));

  FinishInitialization(geis);

  geis_adapter.ProcessGeisEvents();

  ASSERT_TRUE(adapter_emitted_init_complete);
  ASSERT_TRUE(geis_adapter.IsInitComplete());
}

/*
  First send some necessary initialization events. One to define a multi-touch device,
  another defining a gesture class and one that tells that the initialization
  has completed.

  After that send a well formed GEIS_EVENT_GESTURE_BEGIN and check if GeisAdapter
  emits a corresponding nux::GestureEvent.
 */
TEST_F(GeisAdapterTest, EmitsWellFormedNuxGestureEvent)
{
  nux::GeisAdapter geis_adapter;
  Geis geis = geis_adapter.GetGeisInstance();

  struct _GeisGestureClass drag_class;
  drag_class.id = 12;
  drag_class.name = GEIS_GESTURE_DRAG;
  AddGestureClass(geis, &drag_class);

  struct _GeisDevice device;
  device.id = 7;
  device.attributes.AddBoolean(GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH, true);
  AddDevice(geis, &device);

  FinishInitialization(geis);

  struct _GeisFrame frame;
  frame.id = 321;
  frame.class_ids.insert(drag_class.id);
  frame.attributes.AddInteger(GEIS_GESTURE_ATTRIBUTE_DEVICE_ID, device.id);
  frame.attributes.AddFloat(GEIS_GESTURE_ATTRIBUTE_FOCUS_X, 87.3f);
  frame.attributes.AddFloat(GEIS_GESTURE_ATTRIBUTE_FOCUS_Y, 123.4f);

  struct _GeisGroup group;
  group.vector.push_back(&frame);

  struct _GeisGroupSet group_set;
  group_set.vector.push_back(&group);

  struct _GeisTouch touch;
  touch.id = 0;
  touch.attributes.AddFloat(GEIS_TOUCH_ATTRIBUTE_ID, 123);
  touch.attributes.AddFloat(GEIS_TOUCH_ATTRIBUTE_X, 777.7f);
  touch.attributes.AddFloat(GEIS_TOUCH_ATTRIBUTE_Y, 888.8f);

  struct _GeisTouchSet touch_set;
  touch_set.vector.push_back(&touch);

  GeisEvent event = new struct _GeisEvent;
  event->type = GEIS_EVENT_GESTURE_BEGIN;
  event->attributes.AddPointer(GEIS_EVENT_ATTRIBUTE_GROUPSET, &group_set);
  event->attributes.AddPointer(GEIS_EVENT_ATTRIBUTE_TOUCHSET, &touch_set);
  event->attributes.AddBoolean(GEIS_EVENT_ATTRIBUTE_CONSTRUCTION_FINISHED, true);
  geis->pending_events.push_back(event);

  geis_adapter.event_ready.connect(sigc::mem_fun(this,
      &GeisAdapterTest::StoreNuxGestureEvent));

  geis_adapter.ProcessGeisEvents();

  ASSERT_EQ(1, gesture_events.size());

  const nux::GestureEvent &gesture_event = gesture_events.front();

  ASSERT_EQ(nux::EVENT_GESTURE_BEGIN, gesture_event.type);

  ASSERT_EQ(frame.id, gesture_event.GetGestureId());

  ASSERT_EQ(device.GetBoolean(GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH),
            gesture_event.IsDirectTouch());

  ASSERT_FLOAT_EQ(frame.GetFloat(GEIS_GESTURE_ATTRIBUTE_FOCUS_X),
                  gesture_event.GetFocus().x);

  ASSERT_FLOAT_EQ(frame.GetFloat(GEIS_GESTURE_ATTRIBUTE_FOCUS_Y),
                  gesture_event.GetFocus().y);

  ASSERT_EQ(touch_set.vector.size(), gesture_event.GetTouches().size());
  for (size_t i = 0; i < gesture_event.GetTouches().size(); ++i)
  {
    ASSERT_FLOAT_EQ(touch_set.vector[i]->GetInteger(GEIS_TOUCH_ATTRIBUTE_ID),
                    gesture_event.GetTouches()[i].id);

    ASSERT_FLOAT_EQ(touch_set.vector[i]->GetFloat(GEIS_TOUCH_ATTRIBUTE_X),
                    gesture_event.GetTouches()[i].x);

    ASSERT_FLOAT_EQ(touch_set.vector[i]->GetFloat(GEIS_TOUCH_ATTRIBUTE_Y),
                    gesture_event.GetTouches()[i].y);
  }

}

/*
  Checks that when a Tap Update GeisEvent is received, GeisAdpater generates
  two events from it: A Tap Begin and a Tap End.
 */
TEST_F(GeisAdapterTest, SplitTapUpdateIntoTapBeginAndTapEnd)
{
  nux::GeisAdapter geis_adapter;
  Geis geis = geis_adapter.GetGeisInstance();

  struct _GeisGestureClass tap_class;
  tap_class.id = 12;
  tap_class.name = GEIS_GESTURE_TAP;
  AddGestureClass(geis, &tap_class);

  struct _GeisDevice device;
  device.id = 7;
  device.attributes.AddBoolean(GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH, true);
  AddDevice(geis, &device);

  FinishInitialization(geis);

  struct _GeisFrame frame;
  frame.id = 321;
  frame.class_ids.insert(tap_class.id);
  frame.attributes.AddInteger(GEIS_GESTURE_ATTRIBUTE_DEVICE_ID, device.id);
  frame.attributes.AddFloat(GEIS_GESTURE_ATTRIBUTE_FOCUS_X, 87.3f);
  frame.attributes.AddFloat(GEIS_GESTURE_ATTRIBUTE_FOCUS_Y, 123.4f);

  struct _GeisGroup group;
  group.vector.push_back(&frame);

  struct _GeisGroupSet group_set;
  group_set.vector.push_back(&group);

  struct _GeisTouch touch;
  touch.id = 0;
  touch.attributes.AddFloat(GEIS_TOUCH_ATTRIBUTE_ID, 123);
  touch.attributes.AddFloat(GEIS_TOUCH_ATTRIBUTE_X, 777.7f);
  touch.attributes.AddFloat(GEIS_TOUCH_ATTRIBUTE_Y, 888.8f);

  struct _GeisTouchSet touch_set;
  touch_set.vector.push_back(&touch);

  GeisEvent event = new struct _GeisEvent;
  event->type = GEIS_EVENT_GESTURE_UPDATE;
  event->attributes.AddPointer(GEIS_EVENT_ATTRIBUTE_GROUPSET, &group_set);
  event->attributes.AddPointer(GEIS_EVENT_ATTRIBUTE_TOUCHSET, &touch_set);
  event->attributes.AddBoolean(GEIS_EVENT_ATTRIBUTE_CONSTRUCTION_FINISHED, false);
  geis->pending_events.push_back(event);

  geis_adapter.event_ready.connect(sigc::mem_fun(this,
      &GeisAdapterTest::StoreNuxGestureEvent));

  geis_adapter.ProcessGeisEvents();

  ASSERT_EQ(2, gesture_events.size());

  ASSERT_EQ(nux::EVENT_GESTURE_BEGIN, gesture_events.front().type);
  ASSERT_EQ(nux::TAP_GESTURE, gesture_events.front().GetGestureClasses());
  ASSERT_TRUE(gesture_events.front().IsConstructionFinished());

  ASSERT_EQ(nux::EVENT_GESTURE_END, gesture_events.back().type);
  ASSERT_EQ(nux::TAP_GESTURE, gesture_events.back().GetGestureClasses());
  ASSERT_TRUE(gesture_events.back().IsConstructionFinished());
}
