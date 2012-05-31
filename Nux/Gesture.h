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

#ifndef NUX_GESTURE_H
#define NUX_GESTURE_H
#include "Features.h"
#ifdef NUX_GESTURES_SUPPORT

#include <memory>

#include "NuxGraphics/GestureEvent.h"

/*
  A collection of helper classes used by WindowCompositor for the task of
  delivering GestureEvents to their correct target InputAreas and in the
  decision of whether a gesture should be accepted or rejected.
 */
namespace nux
{
class InputArea;

//! A class that relates a multitouch gesture to its target entity
/*!
  It relates a gesture to the entity (an InputArea or the main window)
  that should receive its updates (i.e., GestureEvents)

  It's fed with GestureEvents via Update().
  It stores those GestureEvents in a queue until EnableEventDelivery() is
  called, when all queued events are finally sent to their target. After
  that, further events fed via Update() are delivered directly to their
  target instead of being queued.
 */
class Gesture
{
  public:
    Gesture(const GestureEvent &event, InputArea *target_area);

    void EnableEventDelivery();
    void Update(const GestureEvent& event);
    bool IsConstructionFinished() const;
    bool IsDeliveringEvents() const {return event_delivery_enabled_;}
    int GetId() const;
    const ObjectWeakPtr<InputArea> &GetTargetArea() const {return target_area_;}
    const std::vector<TouchPoint> &GetTouches() const;

    //! Returns whether the given gesture has any touches in common with this one.
    bool HasTouchesInCommon(const std::shared_ptr<Gesture> &other_gesture) const;

    //! Rejects the gesture.
    /*
      After rejection a gesture is no longer valid
     */
    void Reject();

    //! Accepts the gesture
    void Accept();

  private:
    const GestureEvent &GetLatestEvent() const;
    GestureEvent &GetLatestEvent();

    ObjectWeakPtr<InputArea> target_area_;

    // events that are waiting to be delivered
    std::vector<GestureEvent> queued_events_;

    // last event delivered
    GestureEvent last_event_;

    bool event_delivery_enabled_;
};


class GestureSet
{
  public:
    void Add(Gesture *gesture);
    void Add(std::shared_ptr<Gesture> &gesture);
    std::shared_ptr<Gesture> Get(int gesture_id);
    std::shared_ptr<Gesture> Get(InputArea *area);
    void Remove(std::shared_ptr<Gesture> &gesture);

    std::vector< std::shared_ptr<Gesture> >
      GetConflictingGestures(std::shared_ptr<Gesture> &gesture);

  private:
    // TODO: Consider a simple memory pool scheme to avoid allocating and
    // deleting memory whenever gestures come and go (e.g. vector with
    // reserved memory + stack of free indexes)
    std::map<int, std::shared_ptr<Gesture> > map_id_to_gesture_;

};

} // namespace nux
#endif // NUX_GESTURES_SUPPORT
#endif // NUX_GESTURE_H
