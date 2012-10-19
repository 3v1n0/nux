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

#include <geis/geis.h>
#include "geis_mock.h"

#include <stdio.h>
#include <string.h>
#include <sys/eventfd.h>
#include <unistd.h>

//#define DEBUG_PRINT printf("%s mock called!\n", __func__);
#define DEBUG_PRINT

/******** GeisAttributes helper class *********/

GeisAttr GeisAttributes::GetByName(GeisString name)
{
  for (size_t i = 0; i < vector.size(); ++i)
  {
    GeisAttr attr = vector[i].get();
    if (attr->name.compare(name) == 0)
      return attr;
  }
  return nullptr;
}

GeisAttr GeisAttributes::GetByIndex(GeisSize index)
{
  if (vector.size() > index)
  {
    return vector[index].get();
  }
  else
  {
    return nullptr;
  }
}

GeisSize GeisAttributes::Count()
{
  return vector.size();
}

void GeisAttributes::AddBoolean(const char *name, GeisBoolean value)
{
  std::unique_ptr<struct _GeisAttr> attr(new struct _GeisAttr);
  attr->type = GEIS_ATTR_TYPE_BOOLEAN;
  attr->name = name;
  attr->b = value;
  vector.push_back(std::move(attr));
}

void GeisAttributes::AddFloat(const char *name, GeisFloat value)
{
  std::unique_ptr<struct _GeisAttr> attr(new struct _GeisAttr);
  attr->type = GEIS_ATTR_TYPE_FLOAT;
  attr->name = name;
  attr->f = value;
  vector.push_back(std::move(attr));
}

void GeisAttributes::AddInteger(const char *name, GeisInteger value)
{
  std::unique_ptr<struct _GeisAttr> attr(new struct _GeisAttr);
  attr->type = GEIS_ATTR_TYPE_INTEGER;
  attr->name = name;
  attr->i = value;
  vector.push_back(std::move(attr));
}

void GeisAttributes::AddPointer(const char *name, void *value)
{
  std::unique_ptr<struct _GeisAttr> attr(new struct _GeisAttr);
  attr->type = GEIS_ATTR_TYPE_POINTER;
  attr->name = name;
  attr->p    = value;
  vector.push_back(std::move(attr));
}

/******* Geis  *******/

Geis geis_new(GeisString /* init_arg_name */, ...)
{
  DEBUG_PRINT
  Geis geis = new struct _Geis;
  geis->fd = eventfd(0, EFD_NONBLOCK);
  return geis;
}

GeisStatus geis_delete(Geis geis)
{
  DEBUG_PRINT
  close(geis->fd);
  delete geis;
  return GEIS_STATUS_SUCCESS;
}

GeisStatus geis_get_configuration(Geis geis,
                                  GeisString configuration_item_name,
                                  void *configuration_item_value)
{
  DEBUG_PRINT
  if (strcmp(configuration_item_name, GEIS_CONFIGURATION_FD) == 0)
  {
    int *fd = static_cast<int*>(configuration_item_value);
    *fd = geis->fd;
    return GEIS_STATUS_SUCCESS;
  }
  else
  {
    return GEIS_STATUS_NOT_SUPPORTED;
  }
}

GeisStatus geis_dispatch_events(Geis /* geis */)
{
  DEBUG_PRINT
  return GEIS_STATUS_SUCCESS;
}

GeisStatus geis_next_event(Geis geis, GeisEvent *event)
{
  DEBUG_PRINT

  if (geis->pending_events.size() > 0)
  {
    *event = geis->pending_events.front();
    geis->pending_events.pop_front();

    if (geis->pending_events.size() > 0)
      return GEIS_STATUS_CONTINUE;
    else
      return GEIS_STATUS_SUCCESS;
  }
  else
  {
    *event = nullptr;
    return GEIS_STATUS_EMPTY;
  }
}

GeisStatus geis_gesture_accept(Geis          /* geis */,
                               GeisGroup     /* group */,
                               GeisGestureId /* gesture_id */)
{
  DEBUG_PRINT
  return GEIS_STATUS_SUCCESS;
}

GeisStatus geis_gesture_reject(Geis          /* geis */,
                               GeisGroup     /* group */,
                               GeisGestureId /* gesture_id */)
{
  DEBUG_PRINT
  return GEIS_STATUS_SUCCESS;
}

/******* Attr  *******/

GeisString geis_attr_name(GeisAttr attr)
{
  DEBUG_PRINT
  return attr->name.c_str();
}

GeisAttrType geis_attr_type(GeisAttr attr)
{
  DEBUG_PRINT
  return attr->type;
}

GeisBoolean geis_attr_value_to_boolean(GeisAttr attr)
{
  DEBUG_PRINT
  return attr->b;
}

GeisFloat geis_attr_value_to_float(GeisAttr attr)
{
  DEBUG_PRINT
  return attr->f;
}

GeisInteger geis_attr_value_to_integer(GeisAttr attr)
{
  DEBUG_PRINT
  return attr->i;
}

GeisPointer geis_attr_value_to_pointer(GeisAttr attr)
{
  DEBUG_PRINT
  return attr->p;
}

GeisString geis_attr_value_to_string(GeisAttr attr)
{
  DEBUG_PRINT
  return attr->s.c_str();
}

/*********** Event  **************/

void geis_event_delete(GeisEvent event)
{
  DEBUG_PRINT
  delete event;
}

GeisEventType geis_event_type(GeisEvent event)
{
  DEBUG_PRINT
  return event->type;
}

GeisSize geis_event_attr_count(GeisEvent event)
{
  DEBUG_PRINT
  return event->attributes.Count();
}

GeisAttr geis_event_attr(GeisEvent event, GeisSize index)
{
  DEBUG_PRINT
  return event->attributes.GetByIndex(index);
}

GeisAttr geis_event_attr_by_name(GeisEvent event, GeisString attr_name)
{
  DEBUG_PRINT
  return event->attributes.GetByName(attr_name);
}

/******** Gesture Class   ***********/

void geis_gesture_class_ref(GeisGestureClass /* gesture_class */)
{
  DEBUG_PRINT
}

void geis_gesture_class_unref(GeisGestureClass /* gesture_class */)
{
  DEBUG_PRINT
}

GeisString geis_gesture_class_name(GeisGestureClass gesture_class)
{
  DEBUG_PRINT
  return const_cast<char *>(gesture_class->name.c_str());
}

GeisInteger geis_gesture_class_id(GeisGestureClass gesture_class)
{
  DEBUG_PRINT
  return gesture_class->id;
}

GeisSize geis_gesture_class_attr_count(GeisGestureClass gesture_class)
{
  DEBUG_PRINT
  return gesture_class->attributes.Count();
}

GeisAttr geis_gesture_class_attr(GeisGestureClass gesture_class, int index)
{
  DEBUG_PRINT
  return gesture_class->attributes.GetByIndex(index);
}

/********* Device ***********/

GeisInteger geis_device_id(GeisDevice device)
{
  DEBUG_PRINT
  return device->id;
}

GeisAttr geis_device_attr_by_name(GeisDevice device, GeisString attr_name)
{
  DEBUG_PRINT
  return device->attributes.GetByName(attr_name);
}

/********** Group Set **********/

GeisSize geis_groupset_group_count(GeisGroupSet groupset)
{
  DEBUG_PRINT
  return groupset->vector.size();
}

GeisGroup geis_groupset_group(GeisGroupSet groupset, GeisSize index)
{
  DEBUG_PRINT
  return groupset->vector[index];
}

/****** Group ******/

GeisSize geis_group_frame_count(GeisGroup group)
{
  DEBUG_PRINT
  return group->vector.size();
}

GeisFrame geis_group_frame(GeisGroup group, GeisSize index)
{
  DEBUG_PRINT
  return group->vector[index];
}

/***** Touch Set *****/

GeisSize geis_touchset_touch_count(GeisTouchSet touchset)
{
  DEBUG_PRINT
  return touchset->vector.size();
}

GeisTouch geis_touchset_touch(GeisTouchSet touchset, GeisSize index)
{
  DEBUG_PRINT
  return touchset->vector[index];
}

/***** Touch ******/

GeisTouchId geis_touch_id(GeisTouch touch)
{
  DEBUG_PRINT
  return touch->id;
}

GeisSize geis_touch_attr_count(GeisTouch touch)
{
  DEBUG_PRINT
  return touch->attributes.Count();
}

GeisAttr geis_touch_attr(GeisTouch touch, GeisSize index)
{
  DEBUG_PRINT
  return touch->attributes.GetByIndex(index);
}

GeisAttr geis_touch_attr_by_name(GeisTouch touch, GeisString name)
{
  DEBUG_PRINT
  return touch->attributes.GetByName(name);
}

/***** Frame *****/

GeisGestureId geis_frame_id(GeisFrame frame)
{
  DEBUG_PRINT
  return frame->id;
}

GeisBoolean geis_frame_is_class(GeisFrame        frame,
                                GeisGestureClass gesture_class)
{
  DEBUG_PRINT
  std::set<int>::iterator it = frame->class_ids.find(gesture_class->id);
  if (it == frame->class_ids.end())
    return GEIS_FALSE;
  else
    return GEIS_TRUE;
}

GeisSize geis_frame_attr_count(GeisFrame frame)
{
  DEBUG_PRINT
  return frame->attributes.Count();
}

GeisAttr geis_frame_attr(GeisFrame frame, GeisSize index)
{
  DEBUG_PRINT
  return frame->attributes.GetByIndex(index);
}

GeisAttr geis_frame_attr_by_name(GeisFrame frame, GeisString name)
{
  DEBUG_PRINT
  return frame->attributes.GetByName(name);
}

/***** Subscription *****/

GeisSubscription geis_subscription_new(Geis                  /* geis */,
                                       GeisString            /* name */,
                                       GeisSubscriptionFlags /* flags */)
{
  DEBUG_PRINT
  return new struct _GeisSubscription;
}

GeisStatus geis_subscription_delete(GeisSubscription subscription)
{
  DEBUG_PRINT
  for (auto filter : subscription->filters)
  {
    delete filter;
  }
  delete subscription;
  return GEIS_STATUS_SUCCESS;
}

GeisStatus geis_subscription_activate(GeisSubscription /* subscription */)
{
  DEBUG_PRINT
  return GEIS_STATUS_SUCCESS;
}

GeisStatus geis_subscription_deactivate(GeisSubscription /* subscription */)
{
  DEBUG_PRINT
  return GEIS_STATUS_SUCCESS;
}

GeisString geis_subscription_name(GeisSubscription subscription)
{
  DEBUG_PRINT
  return const_cast<char *>(subscription->name.c_str());
}

GeisInteger geis_subscription_id(GeisSubscription subscription)
{
  DEBUG_PRINT
  return subscription->id;
}

GeisStatus geis_subscription_add_filter(GeisSubscription subscription,
                                        GeisFilter       filter)
{
  DEBUG_PRINT
  subscription->filters.push_back(filter);
  return GEIS_STATUS_SUCCESS;
}

GeisFilter geis_subscription_filter_by_name(GeisSubscription /* sub */,
                                            GeisString       /* name */)
{
  DEBUG_PRINT
  return nullptr;
}

GeisStatus geis_subscription_remove_filter(GeisSubscription subscription,
                                          GeisFilter       filter)
{
  DEBUG_PRINT
  std::vector<GeisFilter>::iterator it = subscription->filters.begin();
  while (it != subscription->filters.end())
  {
    if (filter == *it)
    {
      subscription->filters.erase(it);
      return GEIS_STATUS_SUCCESS;
    }
    ++it;
  }
  return GEIS_STATUS_BAD_ARGUMENT;
}
