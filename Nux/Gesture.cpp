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

#include "Gesture.h"
#include "InputArea.h"

using namespace nux;

/*****************************************************************************
 * Gesture
 *****************************************************************************/

Gesture::Gesture(const GestureEvent &event, InputArea *area)
      : target_area_(area), event_delivery_enabled_(false)
{
  queued_events_.reserve(20);
  queued_events_.push_back(event);
}

void Gesture::EnableEventDelivery()
{
  if (event_delivery_enabled_)
    return;

  event_delivery_enabled_ = true;

  if (queued_events_.size() == 0)
    return;

  if (!target_area_.IsValid())
    return;

  // Deliver all queued events but keep the last one
  last_event_ = queued_events_[queued_events_.size()-1];
  for (auto event : queued_events_)
  {
    target_area_->GestureEvent(event);
  }
  queued_events_.clear();
}

void Gesture::Update(const GestureEvent& event)
{
  if (event_delivery_enabled_)
  {
    if (target_area_.IsValid())
      target_area_->GestureEvent(event);
    last_event_ = event;
  }
  else
  {
    queued_events_.push_back(event);
  }
}

GestureEvent &Gesture::GetLatestEvent()
{
  return const_cast<GestureEvent&>(const_cast<Gesture*>(this)->GetLatestEvent());
}

const GestureEvent &Gesture::GetLatestEvent() const
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
  GetLatestEvent().Reject();
}

void Gesture::Accept()
{
  GetLatestEvent().Accept();
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

std::shared_ptr<Gesture> GestureSet::Get(int gesture_id)
{
  std::map<int, std::shared_ptr<Gesture> >::iterator it
    = map_id_to_gesture_.find(gesture_id);

  if (it != map_id_to_gesture_.end())
    return it->second;
  else
    return std::shared_ptr<Gesture>();
}

std::shared_ptr<Gesture> GestureSet::Get(InputArea *area)
{
  for (auto it : map_id_to_gesture_)
  {
    if (it.second->GetTargetArea() == area)
      return it.second;
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

void GestureSet::Remove(std::shared_ptr<Gesture> &gesture)
{
  map_id_to_gesture_.erase(gesture->GetId());
}
