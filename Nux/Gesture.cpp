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

#include "Gesture.h"
#include "Nux/InputArea.h"

using namespace nux;

/*****************************************************************************
 * InputAreaTarget
 *****************************************************************************/

InputAreaTarget::InputAreaTarget(InputArea *input_area)
  : input_area_(input_area)
{
}

GestureDeliveryRequest InputAreaTarget::GestureEvent(const nux::GestureEvent &event)
{
  if (input_area_.IsValid())
    return input_area_->GestureEvent(event);
  else
    return GestureDeliveryRequest::NONE;
}

bool InputAreaTarget::Equals(const GestureTarget& other) const
{
  const InputAreaTarget *input_area_target = dynamic_cast<const InputAreaTarget *>(&other);

  if (input_area_target)
  {
    return input_area_ == input_area_target->input_area_;
  }
  else
  {
    return false;
  }
}

/*****************************************************************************
 * Gesture
 *****************************************************************************/

Gesture::Gesture(const nux::GestureEvent &event)
      : event_delivery_enabled_(false),
        acceptance_status_(AcceptanceStatus::UNDECIDED)
{
  queued_events_.reserve(20);
  queued_events_.push_back(event);
}

void Gesture::AddTarget(ShPtGestureTarget target)
{
  target_died_connections_[target] =
    target->died.connect(sigc::mem_fun (this, &Gesture::RemoveTarget));
  target_list_.push_back(target);
}

void Gesture::RemoveTarget(const GestureTarget &target)
{
  auto check_same_target = [&](const ShPtGestureTarget& other_target)
  {
    return *other_target == target;
  };

  auto target_iterator = std::find_if(target_list_.begin(),
                                      target_list_.end(),
                                      check_same_target);

  if (target_iterator != target_list_.end())
  {
    auto connection_iterator =
      target_died_connections_.find(*target_iterator);

    if (connection_iterator != target_died_connections_.end())
      connection_iterator->second.disconnect();

    target_list_.erase(target_iterator);
  }

  if (target_list_.empty())
  {
    /* Reject this gesture if we can no longer accept it */
    if (GetAcceptanceStatus() == Gesture::AcceptanceStatus::UNDECIDED)
      Reject ();

    lost_all_targets.emit (*this);
  }
}

void Gesture::EnableEventDelivery()
{
  if (event_delivery_enabled_)
    return;

  event_delivery_enabled_ = true;

  if (queued_events_.empty())
    return;

  // Deliver all queued events but keep the last one
  last_event_ = queued_events_[queued_events_.size()-1];
  for (auto event : queued_events_)
  {
    DeliverEvent(event);
  }
  queued_events_.clear();
}

void Gesture::Update(const nux::GestureEvent& event)
{
  if (event_delivery_enabled_)
  {
    DeliverEvent(event);
    last_event_ = event;
  }
  else
  {
    queued_events_.push_back(event);
  }
}

void Gesture::DeliverEvent(const GestureEvent &event)
{
  auto it = target_list_.begin();
  while (it != target_list_.end())
  {
    GestureDeliveryRequest request = (*it)->GestureEvent(event);

    switch(request)
    {
      case GestureDeliveryRequest::EXCLUSIVITY:
        ExecuteTargetExclusivityRequest(event, it);
        break;

      default: // NONE
        ++it;
    }
  }
}

void Gesture::ExecuteTargetExclusivityRequest(const GestureEvent &event,
    std::list<ShPtGestureTarget>::iterator &it_requestor)
{

  // Don't send gesture lost events for a gesture begin.
  // Targets can't lose a gesture they never knew about.
  if (event.type != EVENT_GESTURE_BEGIN)
  {
    GestureEvent event_lost = event;
    event_lost.type = EVENT_GESTURE_LOST;
    auto other_it = target_list_.rbegin();
    while (*other_it != *it_requestor)
    {
      (*other_it)->GestureEvent(event_lost);
      ++other_it;
    }
  }

  ++it_requestor;
  target_list_.erase(it_requestor, target_list_.end());
  it_requestor = target_list_.end();
}

nux::GestureEvent &Gesture::GetLatestEvent()
{
  if (event_delivery_enabled_)
  {
    nuxAssert(queued_events_.size() == 0);
    return last_event_;
  }
  else
  {
    nuxAssert(queued_events_.size() > 0);
    return queued_events_[queued_events_.size()-1];
  }
}

const nux::GestureEvent &Gesture::GetLatestEvent() const
{
  if (event_delivery_enabled_)
  {
    nuxAssert(queued_events_.size() == 0);
    return last_event_;
  }
  else
  {
    nuxAssert(queued_events_.size() > 0);
    return queued_events_[queued_events_.size()-1];
  }
}

bool Gesture::IsConstructionFinished() const
{
  return GetLatestEvent().IsConstructionFinished();
}

int Gesture::GetId() const
{
  return GetLatestEvent().GetGestureId();
}

const std::vector<TouchPoint> &Gesture::GetTouches() const
{
  return GetLatestEvent().GetTouches();
}

bool Gesture::HasTouchesInCommon(
    const std::shared_ptr<Gesture> &other_gesture) const
{
  const std::vector<TouchPoint> &my_touches = GetTouches();
  const std::vector<TouchPoint> &other_touches = other_gesture->GetTouches();

  for (const auto other_touch : other_touches)
  {
    for (const auto my_touch : my_touches)
    {
      if (other_touch == my_touch)
        return true;
    }
  }

  return false;
}

void Gesture::Reject()
{
  g_assert(acceptance_status_ == AcceptanceStatus::UNDECIDED);
  GetLatestEvent().Reject();
  acceptance_status_ = AcceptanceStatus::REJECTED;
}

void Gesture::Accept()
{
  g_assert(acceptance_status_ == AcceptanceStatus::UNDECIDED);
  GetLatestEvent().Accept();
  acceptance_status_ = AcceptanceStatus::ACCEPTED;
}

/*****************************************************************************
 * GestureSet
 *****************************************************************************/

void GestureSet::Add(Gesture *gesture)
{
  map_id_to_gesture_[gesture->GetId()] = std::shared_ptr<Gesture>(gesture);
}

void GestureSet::Add(std::shared_ptr<Gesture> &gesture)
{
  map_id_to_gesture_[gesture->GetId()] = gesture;
}

std::shared_ptr<Gesture> GestureSet::FindFromGestureId(int gesture_id)
{
  std::map<int, std::shared_ptr<Gesture> >::iterator it
    = map_id_to_gesture_.find(gesture_id);

  if (it != map_id_to_gesture_.end())
    return it->second;
  else
    return std::shared_ptr<Gesture>();
}

std::shared_ptr<Gesture> GestureSet::FindFromTarget(ShPtGestureTarget wanted_target)
{
  for (auto it : map_id_to_gesture_)
  {
    std::shared_ptr<Gesture> &gesture = it.second;
    const std::list<ShPtGestureTarget> &target_list = gesture->GetTargetList();
    for (auto target : target_list)
    {
      if (*target == *wanted_target)
        return gesture;
    }
  }
  return nullptr;
}

std::vector< std::shared_ptr<Gesture> >
  GestureSet::GetConflictingGestures(std::shared_ptr<Gesture> &gesture)
{
  std::vector< std::shared_ptr<Gesture> > conflicting_gestures;

  for (auto it : map_id_to_gesture_)
  {
    std::shared_ptr<Gesture> &other_gesture = it.second;

    if (other_gesture == gesture)
      continue;

    if (other_gesture->HasTouchesInCommon(gesture))
      conflicting_gestures.push_back(other_gesture);
  }

  return conflicting_gestures;
}

void GestureSet::Remove(const Gesture &gesture)
{
  map_id_to_gesture_.erase(gesture.GetId());
}
