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

#include "GesturesSubscription.h"
#include "Nux.h"
#include "NuxCore/Logger.h"
#include "Nux/GeisAdapter.h"

namespace nux
{
DECLARE_LOGGER(logger, "nux.gestures_subscription");

GesturesSubscription::GesturesSubscription()
  : gesture_classes_(DRAG_GESTURE|PINCH_GESTURE|ROTATE_GESTURE),
    num_touches_(2),
#if defined(USE_X11)
    window_id_(GetWindowThread()->GetGraphicsDisplay().GetWindowHandle()),
#else
    window_id_(0),
#endif
    sub_(nullptr),
    is_active_(false),
    drag_threshold_(0.0026f),
    drag_timeout_(0),
    pinch_threshold_(1.1f),
    pinch_timeout_(0),
    rotate_threshold_(2.0f*3.1415926f/50.0f),
    rotate_timeout_(0),
    tap_threshold_(0.0026f),
    tap_timeout_(300)
{
}

GesturesSubscription::~GesturesSubscription()
{
  if (sub_)
    geis_subscription_delete(sub_);
}

void GesturesSubscription::Activate()
{
  if (is_active_)
    return;

  is_active_ = true;

  if (sub_)
  {
    GeisStatus status = geis_subscription_activate(sub_);
    if (status != GEIS_STATUS_SUCCESS)
    {
      LOG_ERROR(logger) << "Failed to activate Geis subscription.";
    }
    // configuration only works on active subscription (should be fixed in geis)
    ConfigureGeisSubscription();
  }
  else
  {
    CreateGeisSubscriptionWhenPossible();
  }
}

void GesturesSubscription::Deactivate()
{
  if (!is_active_)
    return;

  is_active_ = false;

  if (sub_)
  {
    GeisStatus status = geis_subscription_deactivate(sub_);
    if (status != GEIS_STATUS_SUCCESS)
    {
      LOG_ERROR(logger) << "Failed to deactivate Geis subscription.";
    }
  }
}

void GesturesSubscription::SetGestureClasses(int gesture_classes)
{
  SetProperty(gesture_classes_, gesture_classes);

  #define NUX_ALL_GESTURES (DRAG_GESTURE|PINCH_GESTURE|TAP_GESTURE|TOUCH_GESTURE);
  unwanted_gesture_classes_ = ~gesture_classes & NUX_ALL_GESTURES;
  #undef NUX_ALL_GESTURES
}

void GesturesSubscription::SetNumTouches(unsigned int num_touches)
{
  SetProperty(num_touches_, num_touches);
}

void GesturesSubscription::SetWindowId(int window_id)
{
  SetProperty(window_id_, window_id);
}

void GesturesSubscription::SetRecognitionThreshold(GestureClass gesture_class,
                                                   float threshold)
{
  if (threshold < 0.0f)
    return;

  switch(gesture_class)
  {
    case DRAG_GESTURE:
      SetProperty(drag_threshold_, threshold);
      break;
    case PINCH_GESTURE:
      SetProperty(pinch_threshold_, threshold);
      break;
    case TAP_GESTURE:
      SetProperty(tap_threshold_, threshold);
      break;
    case ROTATE_GESTURE:
      SetProperty(rotate_threshold_, threshold);
      break;
    default:
      break;
  }
}

void GesturesSubscription::SetRecognitionTimeout(GestureClass gesture_class,
                                                 int timeout)
{
  if (timeout < 0)
    return;

  switch(gesture_class)
  {
    case DRAG_GESTURE:
      SetProperty(drag_timeout_, timeout);
      break;
    case PINCH_GESTURE:
      SetProperty(pinch_timeout_, timeout);
      break;
    case TAP_GESTURE:
      SetProperty(tap_timeout_, timeout);
      break;
    case ROTATE_GESTURE:
      SetProperty(rotate_timeout_, timeout);
      break;
    default:
      break;
  }
}

std::vector<const char *> GesturesSubscription::CreateGeisGestureClasses()
{
  std::vector<const char *> geis_gesture_classes;

  #define ADD_GESTURE(name) \
    if (gesture_classes_ & name##_GESTURE) \
      geis_gesture_classes.push_back(GEIS_GESTURE_##name);

  ADD_GESTURE(DRAG)
  ADD_GESTURE(PINCH)
  ADD_GESTURE(ROTATE)
  ADD_GESTURE(TAP)
  ADD_GESTURE(TOUCH)

  #undef ADD_GESTURE

  return geis_gesture_classes;
}

GeisStatus GesturesSubscription::AddGestureClassAndNumTouchesTerm(GeisFilter filter)
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  std::vector<const char *> geis_gesture_classes = CreateGeisGestureClasses();

  switch (geis_gesture_classes.size())
  {
    case 1:
      status = geis_filter_add_term(filter,
          GEIS_FILTER_CLASS,
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[0],
          GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, num_touches_,
          nullptr);
      break;
    case 2:
      status = geis_filter_add_term(filter,
          GEIS_FILTER_CLASS,
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[0],
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[1],
          GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, num_touches_,
          nullptr);
      break;
    case 3:
      status = geis_filter_add_term(filter,
          GEIS_FILTER_CLASS,
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[0],
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[1],
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[2],
          GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, num_touches_,
          nullptr);
      break;
    case 4:
      status = geis_filter_add_term(filter,
          GEIS_FILTER_CLASS,
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[0],
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[1],
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[2],
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[3],
          GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, num_touches_,
          nullptr);
      break;
    case 5:
      status = geis_filter_add_term(filter,
          GEIS_FILTER_CLASS,
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[0],
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[1],
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[2],
          GEIS_CLASS_ATTRIBUTE_NAME, GEIS_FILTER_OP_EQ, geis_gesture_classes[4],
          GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, num_touches_,
          nullptr);
      break;
    default:
      status = geis_filter_add_term(filter,
          GEIS_FILTER_CLASS,
          GEIS_GESTURE_ATTRIBUTE_TOUCHES, GEIS_FILTER_OP_EQ, num_touches_,
          nullptr);
      break;
  }

  return status;
}

void GesturesSubscription::CreateGeisSubscription()
{
  GeisStatus status = GEIS_STATUS_UNKNOWN_ERROR;
  GeisFilter filter = nullptr;
  Geis geis = GetWindowThread()->GetGeisAdapter()->GetGeisInstance();

  sub_ = geis_subscription_new(geis, "nux", GEIS_SUBSCRIPTION_NONE);
  if (!sub_)
  {
    LOG_ERROR(logger) << "Failed to create Geis subscription.";
    goto cleanup;
  }

  filter = geis_filter_new(geis, "filter");
  if (!filter)
  {
    LOG_ERROR(logger) << "Failed to create Geis filter.";
    goto cleanup;
  }

  status = AddGestureClassAndNumTouchesTerm(filter);
  if (status != GEIS_STATUS_SUCCESS)
  {
    LOG_ERROR(logger) << "Failed to add gesture term to Geis filter.";
    goto cleanup;
  }

  status = geis_filter_add_term(filter,
      GEIS_FILTER_REGION,
      GEIS_REGION_ATTRIBUTE_WINDOWID, GEIS_FILTER_OP_EQ,
      window_id_,
      nullptr);
  if (status != GEIS_STATUS_SUCCESS)
  {
    LOG_ERROR(logger) << "Failed to add region term to Geis filter.";
    goto cleanup;
  }

  status = geis_subscription_add_filter(sub_, filter);
  if (status != GEIS_STATUS_SUCCESS)
  {
    LOG_ERROR(logger) << "Failed to add filter to subscription.";
    goto cleanup;
  }
  filter = nullptr; // it now belongs to the subscription

  if (is_active_)
  {
    status = geis_subscription_activate(sub_);
    if (status != GEIS_STATUS_SUCCESS)
    {
      LOG_ERROR(logger) << "Failed to activate Geis subscription.";
      goto cleanup;
    }
    // configuration only works on active subscription (should be fixed in geis)
    ConfigureGeisSubscription();
  }

cleanup:
  if (status != GEIS_STATUS_SUCCESS)
  {
    geis_filter_delete(filter);
    geis_subscription_delete(sub_);
    sub_ = nullptr;
  }
}

void GesturesSubscription::CreateGeisSubscriptionWhenPossible()
{
  GeisAdapter *geis_adapter = GetWindowThread()->GetGeisAdapter();

  if (geis_adapter->IsInitComplete())
    CreateGeisSubscription();
  else
    geis_adapter->init_complete.connect(
        sigc::mem_fun(this, &GesturesSubscription::CreateGeisSubscription));
}

void GesturesSubscription::UpdateGeisSubscription()
{
  nuxAssert(sub_ != nullptr);

  if (is_active_)
    geis_subscription_deactivate(sub_);

  geis_subscription_delete(sub_);

  // Recreate the subscription only once it's active again
  if (is_active_)
    CreateGeisSubscription();
}

bool GesturesSubscription::MatchesGesture(const GestureEvent &event) const
{
  if (event.GetTouches().size() != num_touches_)
    return false;

  if ((event.GetGestureClasses() & gesture_classes_) == 0)
    return false;

  if ((event.GetGestureClasses() & unwanted_gesture_classes_) != 0)
    return false;

  return true;
}

void GesturesSubscription::ConfigureGeisSubscription()
{
  nuxAssert(sub_);
  GeisStatus status;

  auto setGeisConfig = [&](const char *name, GeisPointer var)
  {
    status = geis_subscription_set_configuration(sub_, name, var);
    if (status != GEIS_STATUS_SUCCESS)
    {
      LOG_ERROR(logger) << "Failed to set Geis subscription configuration "
        << name;
    }
  };

  setGeisConfig(GEIS_CONFIG_DRAG_THRESHOLD, &drag_threshold_);
  setGeisConfig(GEIS_CONFIG_DRAG_TIMEOUT, &drag_timeout_);
  setGeisConfig(GEIS_CONFIG_PINCH_THRESHOLD, &pinch_threshold_);
  setGeisConfig(GEIS_CONFIG_PINCH_TIMEOUT, &pinch_timeout_);
  setGeisConfig(GEIS_CONFIG_TAP_THRESHOLD, &tap_threshold_);
  setGeisConfig(GEIS_CONFIG_TAP_TIMEOUT, &tap_timeout_);
  setGeisConfig(GEIS_CONFIG_ROTATE_THRESHOLD, &rotate_threshold_);
  setGeisConfig(GEIS_CONFIG_ROTATE_TIMEOUT, &rotate_timeout_);
}

} // namespace nux
