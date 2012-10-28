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

#include "GestureEvent.h"
#include "NuxCore/Logger.h"

using namespace nux;

DECLARE_LOGGER(logger, "nux.gestureevent");

GestureEvent::GestureEvent()
{
  // that's how many fingers you have on your hand (duh).
  // Can't have gestures with more than that many touch points.
  touches_.reserve(5);
}

void GestureEvent::Accept()
{
  GeisStatus status;
  status = geis_gesture_accept(geis_, geis_group_, gesture_id_);
  if (status != GEIS_STATUS_SUCCESS)
  {
    LOG_WARNING(logger) << "Failed to accept gesture with id " << gesture_id_;
  }
}

void GestureEvent::Reject()
{
  GeisStatus status;
  status = geis_gesture_reject(geis_, geis_group_, gesture_id_);
  if (status != GEIS_STATUS_SUCCESS)
  {
    LOG_WARNING(logger) << "Failed to reject gesture with id " << gesture_id_;
  }
}

void GestureEvent::Reset()
{
  Event::Reset();

  gesture_id_ = -1;
  gesture_classes_ = 0;
  timestamp_ = -1;
  focus_.x = focus_.y = 0.0f;
  delta_.x = delta_.y = 0.0f;
  angle_ = 0.0f;
  angle_delta_ = 0.0f;
  angular_velocity_ = 0.0f;
  tap_duration_ = -1;
  velocity_.x = velocity_.y = 0.0f;
  radius_ = 0.0f;
  radius_delta_ = 0.0f;
  radial_velocity_ = 0.0f;
  is_construction_finished_ = false;
  touches_.clear();
  geis_ = nullptr;
  geis_group_ = nullptr;
}
