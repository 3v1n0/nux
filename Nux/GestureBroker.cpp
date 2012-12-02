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

#include "GestureBroker.h"
#include "Gesture.h"

using namespace nux;

GestureBroker::GestureBroker()
{
}

GestureBroker::~GestureBroker()
{
}

void GestureBroker::ProcessGestureBegin(nux::GestureEvent &event)
{
  std::vector<ShPtGestureTarget> target_list = FindGestureTargets(event);

  bool gesture_bound_to_target = false;
  for (auto target : target_list)
  {
    bool ok = BindNewGestureToTarget(event, target);
    gesture_bound_to_target |= ok;
  }

  if (!gesture_bound_to_target)
    event.Reject();
}

bool GestureBroker::BindNewGestureToTarget(nux::GestureEvent &event, ShPtGestureTarget target)
{
  bool successful;
  std::shared_ptr<Gesture> existing_gesture =
    gesture_set_.FindFromTarget(target);

  auto create_gesture_for_target = [&]()
  {
    std::shared_ptr<Gesture> gesture =
      gesture_set_.FindFromGestureId(event.GetGestureId());

    if (!gesture)
    {
      gesture = std::shared_ptr<Gesture>(new Gesture(event));
      gesture_lost_all_targets_connections_[gesture] =
        gesture->lost_all_targets.connect(sigc::mem_fun (&gesture_set_, &GestureSet::Remove));
    }

    gesture->AddTarget(target);
    gesture_set_.Add(gesture);

    if (event.IsConstructionFinished())
    {
      gesture->Accept();
      gesture->EnableEventDelivery();
    }
  };

  if (existing_gesture)
  {
    // There's a conflict here.
    // The target for this new gesture already has a gesture.

    if (existing_gesture->IsDeliveringEvents())
    {
      // A target can have only one gesture at a time.
      successful = false;
    }
    else
    {
      // Since the existing gesture hasn't been delivered to the target yet,
      // we can choose which will continue.
      // The rule is that the gesture with the most touches has precedence over
      // the other.
      int existing_num_touches = existing_gesture->GetTouches().size();
      int new_num_touches = event.GetTouches().size();
      if (new_num_touches < existing_num_touches)
      {
        successful = false;
      }
      else if (new_num_touches == existing_num_touches)
      {
        // It means that there are more fingers on the target area
        // than the target area can handle
        // (i.e. three fingers over an area that handles only two-fingers'
        // gestures)
        existing_gesture->RemoveTarget(*target);
        successful = false;
      }
      else // new_num_touches > existing_num_touches
      {
        existing_gesture->RemoveTarget(*target);
        create_gesture_for_target();
        successful = true;
      }
    }
  }
  else
  {
    create_gesture_for_target();
    successful = true;
  }

  return successful;
}

void GestureBroker::ProcessGestureUpdate(nux::GestureEvent &event)
{
  std::shared_ptr<Gesture> gesture = gesture_set_.FindFromGestureId(event.GetGestureId());
  if (!gesture)
  {
    return;
  }

  if (event.IsConstructionFinished() && !gesture->IsConstructionFinished())
  {
    // This is the first update for this gesture signaling that
    // its construction has finished.
    gesture->Update(event);
    ResolveBufferedGestureThatFinishedConstruction(gesture);
  }
  else
  {
    gesture->Update(event);
  }
}

void GestureBroker::ProcessGestureEnd(nux::GestureEvent &event)
{
  std::shared_ptr<Gesture> gesture = gesture_set_.FindFromGestureId(event.GetGestureId());
  if (!gesture)
    return;

  if (event.IsConstructionFinished() && !gesture->IsConstructionFinished())
  {
    // This is the first update for this gesture signaling that
    // its construction has finished.
    gesture->Update(event);
    ResolveBufferedGestureThatFinishedConstruction(gesture);
  }
  else
  {
    gesture->Update(event);
  }

  // We no longer have to keep track of it.
  auto connection_iterator = gesture_lost_all_targets_connections_.find(gesture);

  if (connection_iterator != gesture_lost_all_targets_connections_.end())
    connection_iterator->second.disconnect();

  gesture_set_.Remove(*gesture);

  // We cannot leave a gesture behing without making a decision on its acceptance.
  if (gesture->GetAcceptanceStatus() == Gesture::AcceptanceStatus::UNDECIDED)
  {
    // This will likely only happen if a geture is ending with its construction unfinished,
    // which is an odd situation, but just in case...
    gesture->Reject();
  }
}

void GestureBroker::ResolveBufferedGestureThatFinishedConstruction(
    std::shared_ptr<Gesture> &gesture)
{
  gesture->Accept();
  gesture->EnableEventDelivery(); // will flush all queued events

  std::vector< std::shared_ptr<Gesture> > conflicting_gestures =
    gesture_set_.GetConflictingGestures(gesture);
  if (!conflicting_gestures.empty())
  {
    // That shouldn't happen. All conflicting gestures should have been
    // dealt with when they begun.
    for (auto conflicting_gesture : conflicting_gestures)
    {
      conflicting_gesture->Reject();
      gesture_set_.Remove(*conflicting_gesture);
    }
  }
}
