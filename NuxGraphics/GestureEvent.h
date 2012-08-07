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

#ifndef NUX_GESTURE_EVENT_H
#define NUX_GESTURE_EVENT_H
#include "Nux/Features.h"
#ifdef NUX_GESTURES_SUPPORT

#include <geis/geis.h>
#include <vector>

#include "Events.h"
#include "NuxCore/Math/Point2D.h"

namespace nux
{
  //! Enumerates all possible gesture classes.
  enum GestureClass
  {
    DRAG_GESTURE   = 1, //! A drag gesture
    PINCH_GESTURE  = 2, //! A pinch gesture
    ROTATE_GESTURE = 4, //! A rotation gesture
    TAP_GESTURE    = 8, //! A tap gesture.
    TOUCH_GESTURE  = 16 /*! A touch gesture. It merely groups two or more touch
                            points and send their information, without interpreting
                            what gestures the touch points are performing. */
  };

  class TouchPoint
  {
    public:
      TouchPoint() : id(-1), x(0.0f), y(0.0f) {}
      TouchPoint(int id, float x, float y) : id(id), x(x), y(y) {}

      bool operator ==(const TouchPoint& other) const
      {
        return id == other.id;
      }

      int id;
      float x;
      float y;
  };

  //! Gesture Event class
  class GestureEvent : public Event
  {
  public:
    //! Constructs a GestureEvent
    GestureEvent();

    //! Accepts the gesture.
    void Accept();
    //! Rejects the gesture.
    void Reject();

    //! The gesture classes that this gesture belongs to.
    /*!
      A bitwise "or" of one or more gesture classes from
      GestureClass enumeration.

      A single physical gesture can comply/belong to more than
      one gesture class, such as a rotating pinch.
     */
    int GetGestureClasses() const {return gesture_classes_;}

    //! Whether all gestures for the related touch points have been presented.
    /*!
      This property allows the client to determine if all the possible gestures
      from the set of touches in this event have already been presented. When
      this value is true, the client will have received all the information needed
      to make a gesture accept and reject decision based on potentially
      overlapping gestures. An example is when both one and two touch gestures are
      subscribed on the same window with the same gesture classes and thresholds.
      When this property is true for one touch gesture events, the client can be
      sure there are no other touches unless a two touch gesture event has already
      been sent.
      Another example is when you subscribe for three touches Touch and four
      touches Drag. As soon as a third finger is detected a three touches Touch
      gesture will begin, but you cannot be sure a fourth finger isn't coming
      right after (that can eventually cause a four touches Drag) until this
      property is true.
    */
    bool IsConstructionFinished() const { return is_construction_finished_;}

    int GetGestureId() const {return gesture_id_;}
    bool IsDirectTouch() const {return is_direct_touch_;}
    int GetTimestamp() const {return timestamp_;}
    const Point2D<float> &GetFocus() const {return focus_;}
    const Point2D<float> &GetDelta() const {return delta_;}
    float GetAngle() const {return angle_;}
    float GetAngleDelta() const {return angle_delta_;}
    float GetAngularVelocity() const {return angular_velocity_;}

    //! Duration of a tap gesture, in milliseconds
    int GetTapDuration() const {return tap_duration_;}

    const Point2D<float> &GetVelocity() {return velocity_;}
    float GetRadius() const {return radius_;}
    float GetRadiusDelta() const {return radius_delta_;}
    float GetRadialVelocity() const {return radial_velocity_;}
    const std::vector<TouchPoint> &GetTouches() const {return touches_;}

    virtual void Reset();

  private:
    int gesture_id_;
    int gesture_classes_;
    bool is_direct_touch_;
    int timestamp_;
    Point2D<float> focus_;
    Point2D<float> delta_;
    float angle_;
    float angle_delta_;
    float angular_velocity_;
    int tap_duration_;
    Point2D<float> velocity_;
    float radius_;
    float radius_delta_;
    float radial_velocity_;
    std::vector<TouchPoint> touches_;
    bool is_construction_finished_;

    Geis geis_;
    GeisGroup geis_group_;
    friend class GeisAdapter;
    friend class FakeGestureEvent;
  };

  //! Enumerates possible requests regarding delivery of gesture events.
  enum class GestureDeliveryRequest
  {
    NONE, /*!< No request. Continue with current delivery policy. */
    EXCLUSIVITY /*!< From this moment onwards, deliver events from the related
                  gesture only to this target. Other targets for this gesture
                  will receive an event of type EVENT_GESTURE_LOST instead of
                  this event.
                  Note that it doesn't affect targets that come before this
                  one in the order of delivery. */
  };
} // namespace nux

#endif // NUX_GESTURES_SUPPORT
#endif // NUX_GESTURE_EVENT_H
