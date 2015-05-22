/*
 * Copyright (C) 2012 - Canonical Ltd.
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
 */

#ifndef NUX_GESTURE_H
#define NUX_GESTURE_H
#include "Nux/Features.h"
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

/*
  Interface for gesture targets.
 */
class GestureTarget
{
  public:

    /*!
      Called whenever there's a new gesture event for this target.
      \param event GestureEvent to be processed by the target.
      \return A request about the delivery of events for the related gesture.
     */
    virtual GestureDeliveryRequest GestureEvent(const GestureEvent &event) = 0;

    bool operator ==(const GestureTarget& other) const
    {
      return Equals(other);
    }

    /***
     * We might not have ownership of every single object that we create
     * implementations of GestureTarget's to wrap around so this signal
     * indicates to the owner of the GestureTarget that the underlying
     * object is no longer available, and this target should be removed
     */
    sigc::signal<void, const GestureTarget &> died;

  private:
    /*!
      For some types of target, different instances may wrap the same actual target,
      in which case reimplementing this method is necessary.
     */
    virtual bool Equals(const GestureTarget& other) const
    {
      return this == &other;
    }
};
typedef std::shared_ptr<GestureTarget> ShPtGestureTarget;

class InputAreaTarget : public GestureTarget
{
  public:
    InputAreaTarget(InputArea *input_area);
    virtual GestureDeliveryRequest GestureEvent(const nux::GestureEvent &event);

  private:
    virtual bool Equals(const GestureTarget& other) const;
    ObjectWeakPtr<InputArea> input_area_;
};

//! A class that relates a multitouch gesture to its target entity
/*!
  It relates a gesture to its target, which can be either a window or
  unity.

  It's fed with GestureEvents via Update().
  It stores those GestureEvents in a queue until EnableEventDelivery() is
  called, when all queued events are finally acted upon. After
  that, further events fed via Update() will have an immediate effect over its
  target instead of being queued.
 */
class Gesture
{
  public:

    Gesture(const GestureEvent &event);

    void AddTarget(ShPtGestureTarget target);
    void RemoveTarget(const GestureTarget &target);
    const std::list<ShPtGestureTarget> &GetTargetList() const {return target_list_;}

    void EnableEventDelivery();
    void Update(const GestureEvent& event);
    bool IsConstructionFinished() const;
    bool IsDeliveringEvents() const {return event_delivery_enabled_;}
    int GetId() const;
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

    enum class AcceptanceStatus
    {
      UNDECIDED,
      ACCEPTED,
      REJECTED
    };

    AcceptanceStatus GetAcceptanceStatus() const {return acceptance_status_;}

    /***
     * This signal is emitted when a Gesture loses all of its targets and
     * can no longer be delivered to anything. This might provide a hint
     * to the owner to stop tracking the gesture
     */
    sigc::signal <void, Gesture &> lost_all_targets;

  private:
    const GestureEvent &GetLatestEvent() const;
    GestureEvent &GetLatestEvent();
    void DeliverEvent(const GestureEvent &event);
    void ExecuteTargetExclusivityRequest(const GestureEvent &event,
        std::list<ShPtGestureTarget>::iterator &it_requestor);

    std::list<ShPtGestureTarget> target_list_;
    std::map <ShPtGestureTarget, sigc::connection> target_died_connections_;

    // events that are waiting to be delivered
    std::vector<GestureEvent> queued_events_;

    // last event delivered
    GestureEvent last_event_;

    bool event_delivery_enabled_;
    AcceptanceStatus acceptance_status_;
};

/*
  Stores information on all curently active gestures.
 */
class GestureSet
{
  public:
    void Add(Gesture *gesture);
    void Add(std::shared_ptr<Gesture> &gesture);
    std::shared_ptr<Gesture> FindFromGestureId(int gesture_id);
    std::shared_ptr<Gesture> FindFromTarget(ShPtGestureTarget target);
    void Remove(const Gesture &gesture);

    std::vector< std::shared_ptr<Gesture> >
      GetConflictingGestures(std::shared_ptr<Gesture> &gesture);

  private:
    std::map<int, std::shared_ptr<Gesture> > map_id_to_gesture_;
};

} // namespace nux
#endif // NUX_GESTURES_SUPPORT
#endif // NUX_GESTURE_H
