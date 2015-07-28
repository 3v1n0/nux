/*
 * Copyright 2012 Canonical Ltd.
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
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 *
 */

#include <NuxCore/Logger.h>
#include <string.h>
#include "GeisAdapter.h"

#define GEIS_CLASS_UNREF(c) if (c) {geis_gesture_class_unref(c); c = nullptr;}

using namespace nux;

DECLARE_LOGGER(logger, "nux.geisadapter");

namespace
{
  struct GeisAdapterEventSource
  {
    GSource source;
    GPollFD event_poll_fd;
  };

  static gboolean geis_source_prepare(GSource * /* source */, gint *timeout)
  {
    *timeout = -1;
    return FALSE;
  }

  static gboolean geis_source_check(GSource *source)
  {
    gboolean retval;
    GeisAdapterEventSource *event_source = reinterpret_cast<GeisAdapterEventSource*>(source);

    if ((event_source->event_poll_fd.revents & G_IO_IN))
    {
      retval = TRUE;
    }
    else
    {
      retval = FALSE;
    }

    return retval;
  }

  gboolean geis_source_dispatch(GSource * /* source */, GSourceFunc /* callback */, gpointer user_data)
  {
    GeisAdapter *geis_adapter = NUX_STATIC_CAST(GeisAdapter*, user_data);
    geis_adapter->ProcessGeisEvents();
    return TRUE;
  }

  static GSourceFuncs geis_source_funcs =
  {
    geis_source_prepare,
    geis_source_check,
    geis_source_dispatch,
    NULL,
    NULL,
    NULL
  };
}

GeisAdapter::GeisAdapter() :
    geis_(nullptr)
  , class_drag_(nullptr)
  , class_pinch_(nullptr)
  , class_rotate_(nullptr)
  , class_tap_(nullptr)
  , class_touch_(nullptr)
  , is_init_complete_(false)
{
  geis_ = geis_new(GEIS_INIT_NO_ATOMIC_GESTURES, 
                   GEIS_INIT_GRAIL_BACKEND,
                   nullptr);
  if (!geis_)
  {
    LOG_WARNING(logger) << "Couldn't create a Geis instance."
                           " Multitouch gesture support disabled.";
  }
}

GeisAdapter::~GeisAdapter()
{
  GEIS_CLASS_UNREF(class_drag_)
  GEIS_CLASS_UNREF(class_pinch_)
  GEIS_CLASS_UNREF(class_rotate_)
  GEIS_CLASS_UNREF(class_tap_)
  GEIS_CLASS_UNREF(class_touch_)
  geis_delete(geis_);

  g_source_remove_by_funcs_user_data(&geis_source_funcs, this);
}

void GeisAdapter::CreateGSource(GMainContext *context)
{
  GSource *source = g_source_new(&geis_source_funcs,
                                 sizeof(GeisAdapterEventSource));
  GeisAdapterEventSource *event_source = reinterpret_cast<GeisAdapterEventSource*>(source);

  g_source_set_priority(source, G_PRIORITY_DEFAULT);

  GeisStatus status = geis_get_configuration(geis_, GEIS_CONFIGURATION_FD,
      &event_source->event_poll_fd.fd);
  if (status != GEIS_STATUS_SUCCESS)
  {
    LOG_WARNING(logger) << "Failed to get geis file descriptor."
      " Multitouch gesture support disabled.";
    g_source_destroy(source);
    return;
  }

  event_source->event_poll_fd.events = G_IO_IN;

  g_source_add_poll(source, &event_source->event_poll_fd);
  g_source_set_callback(source, 0, this, 0);

  g_source_attach(source, context);
}

void GeisAdapter::ProcessGeisEvents()
{
  GeisEvent event = nullptr;
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GestureEvent nux_event;

  status = geis_dispatch_events(geis_);
  if (status != GEIS_STATUS_SUCCESS)
  {
    LOG_WARNING(logger) << "Failed to dispatch events! Status: " << status;
  }

  status = geis_next_event(geis_, &event);
  if (status != GEIS_STATUS_SUCCESS && status != GEIS_STATUS_CONTINUE
      && status != GEIS_STATUS_EMPTY)
  {
    LOG_WARNING(logger) << "Failed to get next event! Status: " << status;
  }

  while (status == GEIS_STATUS_CONTINUE || status == GEIS_STATUS_SUCCESS)
  {
    switch (geis_event_type(event))
    {
      case GEIS_EVENT_GESTURE_BEGIN:
        FillNuxEvent(nux_event, event, EVENT_GESTURE_BEGIN);
        event_ready.emit(nux_event);
        break;
      case GEIS_EVENT_GESTURE_UPDATE:
        FillNuxEvent(nux_event, event, EVENT_GESTURE_UPDATE);
        if (nux_event.GetGestureClasses() == TAP_GESTURE)
        {
          // Geis sends a single Update event for a tap gesture and nothing more,
          // but it's better to be consistent with the rule that all gestures
          // must begin and end (with any number of updates in between).
          // Otherwise code in upper layers will have to add special cases just
          // for the tap gesture.
          nuxAssert(!pending_next_event_);
          SplitUpdateIntoBeginAndEnd(nux_event);
          nuxAssert(pending_next_event_);
          event_ready.emit(nux_event);
          event_ready.emit(*pending_next_event_);
          pending_next_event_.reset();
        }
        else
          event_ready.emit(nux_event);
        break;
      case GEIS_EVENT_GESTURE_END:
        FillNuxEvent(nux_event, event, EVENT_GESTURE_END);
        event_ready.emit(nux_event);
        break;
      case GEIS_EVENT_CLASS_AVAILABLE:
        this->ProcessEventClassAvailable(event);
        break;
      case GEIS_EVENT_DEVICE_AVAILABLE:
        this->ProcessEventDeviceAvailable(event);
        break;
      case GEIS_EVENT_DEVICE_UNAVAILABLE:
        this->ProcessEventDeviceUnavailable(event);
        break;
      case GEIS_EVENT_INIT_COMPLETE:
        this->is_init_complete_ = true;
        this->init_complete.emit();
        break;
      default:
        break;
    }
    geis_event_delete(event);
    status = geis_next_event(this->geis_, &event);
  }
}

void GeisAdapter::SplitUpdateIntoBeginAndEnd(GestureEvent &nux_event)
{
  nux_event.type = EVENT_GESTURE_BEGIN;

  // Work around a bug in geis. A very quick gesture (e.g. a quick tap)
  // will end with its is_construction_finished still set to false.
  // https://bugs.launchpad.net/grail/+bug/1012315
  nux_event.is_construction_finished_ = true;

  pending_next_event_.reset(new GestureEvent);
  *pending_next_event_ = nux_event;
  pending_next_event_->type = EVENT_GESTURE_END;
}

bool GeisAdapter::ProcessNextEvent(GestureEvent *nux_event)
{
  if (pending_next_event_)
  {
    *nux_event = *pending_next_event_;
    pending_next_event_.reset();
    return true;
  }

  GeisEvent event = nullptr;
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  bool filled_nux_event = false;

  status = geis_dispatch_events(geis_);
  if (status != GEIS_STATUS_SUCCESS)
  {
    LOG_WARNING(logger) << "Failed to dispatch events! Status: " << status;
  }

  status = geis_next_event(geis_, &event);
  if (status != GEIS_STATUS_SUCCESS && status != GEIS_STATUS_CONTINUE
      && status != GEIS_STATUS_EMPTY)
  {
    LOG_WARNING(logger) << "Failed to get next event! Status: " << status;
  }

  if (status == GEIS_STATUS_CONTINUE || status == GEIS_STATUS_SUCCESS)
  {
    switch (geis_event_type(event))
    {
      case GEIS_EVENT_GESTURE_BEGIN:
        FillNuxEvent(*nux_event, event, EVENT_GESTURE_BEGIN);
        filled_nux_event = true;
        break;
      case GEIS_EVENT_GESTURE_UPDATE:
        FillNuxEvent(*nux_event, event, EVENT_GESTURE_UPDATE);
        if (nux_event->GetGestureClasses() == TAP_GESTURE)
        {
          // Geis sends a single Update event for a tap gesture and nothing more,
          // but it's better to be consistent with the rule that all gestures
          // must begin and end (with any number of updates in between).
          // Otherwise code in upper layers will have to add special cases just
          // for the tap gesture.
          SplitUpdateIntoBeginAndEnd(*nux_event);
        }
        filled_nux_event = true;
        break;
      case GEIS_EVENT_GESTURE_END:
        FillNuxEvent(*nux_event, event, EVENT_GESTURE_END);
        filled_nux_event = true;
        break;
      case GEIS_EVENT_CLASS_AVAILABLE:
        this->ProcessEventClassAvailable(event);
        break;
      case GEIS_EVENT_DEVICE_AVAILABLE:
        this->ProcessEventDeviceAvailable(event);
        break;
      case GEIS_EVENT_DEVICE_UNAVAILABLE:
        this->ProcessEventDeviceUnavailable(event);
        break;
      case GEIS_EVENT_INIT_COMPLETE:
        this->is_init_complete_ = true;
        this->init_complete.emit();
        break;
      default:
        break;
    }
    geis_event_delete(event);
  }

  if (!filled_nux_event)
    nux_event->Reset();

  return filled_nux_event;
}

void GeisAdapter::ProcessEventDeviceAvailable(GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_DEVICE);
  if (!attr)
  {
    LOG_WARNING(logger) <<
        "Missing device attr in \"Device Available\" event.";
    return;
  }

  GeisDevice geis_device = static_cast<GeisDevice>(geis_attr_value_to_pointer(attr));
  if (!geis_device)
  {
    LOG_WARNING(logger) << "\"Device Available\" event contains a null device";
    return;
  }

  Device device;

  attr = geis_device_attr_by_name(geis_device, GEIS_DEVICE_ATTRIBUTE_DIRECT_TOUCH);
  if (!attr)
  {
    LOG_WARNING(logger) << "Missing \"direct touch\" attr in device.";
    return;
  }
  device.direct_touch = geis_attr_value_to_boolean(attr) == GEIS_TRUE;

  device.id = geis_device_id(geis_device);
  devices_[device.id] = device;
}

void GeisAdapter::ProcessEventDeviceUnavailable(GeisEvent event)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_DEVICE);
  if (!attr)
  {
    LOG_WARNING(logger) <<
        "Missing device attr in  \"Device Available\" event.";
    return;
  }

  GeisDevice geis_device = static_cast<GeisDevice>(geis_attr_value_to_pointer(attr));
  if (!geis_device)
  {
    LOG_WARNING(logger) << "\"Device Available\" event contains a null device";
    return;
  }

  devices_.erase(geis_device_id(geis_device));
}

void GeisAdapter::FillNuxEvent(GestureEvent &nux_event,
                               GeisEvent event,
                               EventType nux_event_type)
{
  GeisAttr attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_GROUPSET);
  GeisGroupSet group_set =
    static_cast<GeisGroupSet>(geis_attr_value_to_pointer(attr));

  /* I expect only one group. */
  if (geis_groupset_group_count(group_set) != 1)
  {
    LOG_WARNING(logger) << "Received a gesture event with "
      << geis_groupset_group_count(group_set) << " groups";
    return;
  }

  GeisGroup group = geis_groupset_group(group_set, 0);

  /* I expect only one frame. */
  if (geis_group_frame_count(group) != 1)
  {
    LOG_WARNING(logger) << "Received a gesture event with a group containing "
      << geis_group_frame_count(group) << " frames";
    return;
  }

  nux_event.Reset();
  nux_event.type = nux_event_type;
  nux_event.geis_ = geis_;
  nux_event.geis_group_ = group;

  GeisFrame frame = geis_group_frame(group, 0);
  FillNuxEventGestureAttributes(nux_event, frame);

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_CONSTRUCTION_FINISHED);
  nux_event.is_construction_finished_ = geis_attr_value_to_boolean(attr) == GEIS_TRUE;

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_TOUCHSET);
  GeisTouchSet touch_set =
    static_cast<GeisTouchSet>(geis_attr_value_to_pointer(attr));
  FillNuxEventTouches(nux_event, touch_set);

  // OBS: A gesture may belong to more than one gesture class
  if (class_drag_ && geis_frame_is_class(frame, class_drag_))
  {
    nux_event.gesture_classes_ |= DRAG_GESTURE;
  }
  if (class_pinch_ && geis_frame_is_class(frame, class_pinch_))
  {
    nux_event.gesture_classes_ |= PINCH_GESTURE;
  }
  if (class_rotate_ && geis_frame_is_class(frame, class_rotate_))
  {
    nux_event.gesture_classes_ |= ROTATE_GESTURE;
  }
  if (class_tap_ && geis_frame_is_class(frame, class_tap_))
  {
    nux_event.gesture_classes_ |= TAP_GESTURE;
  }
  if (class_touch_ && geis_frame_is_class(frame, class_touch_))
  {
    nux_event.gesture_classes_ |= TOUCH_GESTURE;
  }

  // Work around a bug in geis. A very quick gesture (e.g. a quick tap)
  // will end with its is_construction_finished still set to false.
  // https://bugs.launchpad.net/grail/+bug/1012315
  if (nux_event_type == EVENT_GESTURE_END)
    nux_event.is_construction_finished_ = true;
}

void GeisAdapter::FillNuxEventGestureAttributes(GestureEvent &nux_event, GeisFrame frame)
{
  GeisSize attr_count = geis_frame_attr_count(frame);
  GeisAttr attr = 0;

  nux_event.gesture_id_ = geis_frame_id(frame);

  // Idea: use a hash mapping string attribute names to their correponding
  // addresses in the GestureEvent class.
  for (GeisSize i = 0; i < attr_count; ++i)
  {
    attr = geis_frame_attr(frame, i);
    if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_DEVICE_ID))
    {
      int device_id = geis_attr_value_to_integer(attr);
      nux_event.is_direct_touch_ = devices_[device_id].direct_touch;
    }
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_TIMESTAMP))
      nux_event.timestamp_ = geis_attr_value_to_integer(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_FOCUS_X))
      nux_event.focus_.x = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_FOCUS_Y))
      nux_event.focus_.y = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_DELTA_X))
      nux_event.delta_.x = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_DELTA_Y))
      nux_event.delta_.y = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_ANGLE))
      nux_event.angle_ = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_ANGLE_DELTA))
      nux_event.angle_delta_ = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_ANGULAR_VELOCITY))
      nux_event.angular_velocity_ = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_TAP_TIME))
      nux_event.tap_duration_ = geis_attr_value_to_integer(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_VELOCITY_X))
      nux_event.velocity_.x = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_VELOCITY_Y))
      nux_event.velocity_.y = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_RADIUS))
      nux_event.radius_ = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_RADIUS_DELTA))
      nux_event.radius_delta_ = geis_attr_value_to_float(attr);
    else if (g_str_equal(geis_attr_name(attr), GEIS_GESTURE_ATTRIBUTE_RADIAL_VELOCITY))
      nux_event.radial_velocity_ = geis_attr_value_to_float(attr);
  }
}

void GeisAdapter::FillNuxEventTouches(GestureEvent &nux_event, GeisTouchSet touch_set)
{
  GeisSize count = geis_touchset_touch_count(touch_set);

  TouchPoint point;
  for (GeisSize i = 0; i < count; ++i)
  {
    GeisTouch geis_touch = geis_touchset_touch(touch_set, i);

    point.id = geis_attr_value_to_integer(
        geis_touch_attr_by_name(geis_touch, GEIS_TOUCH_ATTRIBUTE_ID));
    point.x = geis_attr_value_to_float(
        geis_touch_attr_by_name(geis_touch, GEIS_TOUCH_ATTRIBUTE_X));
    point.y = geis_attr_value_to_float(
        geis_touch_attr_by_name(geis_touch, GEIS_TOUCH_ATTRIBUTE_Y));

    nux_event.touches_.push_back(point);
  }
}

void GeisAdapter::ProcessEventClassAvailable(GeisEvent event)
{
  GeisGestureClass gesture_class;
  GeisAttr attr;

  attr = geis_event_attr_by_name(event, GEIS_EVENT_ATTRIBUTE_CLASS);
  gesture_class = static_cast<GeisGestureClass>(geis_attr_value_to_pointer(attr));

  if (strcmp(GEIS_GESTURE_DRAG, geis_gesture_class_name(gesture_class)) == 0)
  {
    class_drag_ = gesture_class;
    geis_gesture_class_ref(gesture_class);
  }
  else if (strcmp(GEIS_GESTURE_PINCH, geis_gesture_class_name(gesture_class)) == 0)
  {
    class_pinch_ = gesture_class;
    geis_gesture_class_ref(gesture_class);
  }
  else if (strcmp(GEIS_GESTURE_ROTATE, geis_gesture_class_name(gesture_class)) == 0)
  {
    class_rotate_ = gesture_class;
    geis_gesture_class_ref(gesture_class);
  }
  else if (strcmp(GEIS_GESTURE_TAP, geis_gesture_class_name(gesture_class)) == 0)
  {
    class_tap_ = gesture_class;
    geis_gesture_class_ref(gesture_class);
  }
  else if (strcmp(GEIS_GESTURE_TOUCH, geis_gesture_class_name(gesture_class)) == 0)
  {
    class_touch_ = gesture_class;
    geis_gesture_class_ref(gesture_class);
  }
}
