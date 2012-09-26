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

#include <Nux/Nux.h>
#include "AxisDecelerationAnimation.h"

using namespace nux;

const float AxisDecelerationAnimation::MINIMUM_VELOCITY = 0.07f;

void AxisDecelerationAnimation::SetOvershootBoundsEnabled(bool enable)
{
  overshoot_bounds_ = enable;
}

void AxisDecelerationAnimation::SetMinimumPosition(float min_pos)
{
  min_pos_ = min_pos;
}

void AxisDecelerationAnimation::SetMaximumPosition(float max_pos)
{
  max_pos_ = max_pos;
}

void AxisDecelerationAnimation::SetStartPosition(float position)
{
  start_pos_ = position;
}

void AxisDecelerationAnimation::SetStartVelocity(float velocity)
{
  start_vel_ = velocity;
}

void AxisDecelerationAnimation::Start()
{
  decay_constant_ = 0.01f;
  overshoot_constant_ = 0.007f;
  pos_ = start_pos_;
  vel_ = start_vel_;
  elapsed_time_ = 0;
}

void AxisDecelerationAnimation::Update(int delta_time)
{
  // Exponential decay
  elapsed_time_ += delta_time;
  vel_ = start_vel_ * expf(-(decay_constant_ * elapsed_time_));
  pos_ += vel_ * delta_time;

  if (overshoot_bounds_)
  {
    PullBackIfBeyondBoundaries(delta_time);
  }
  else
  {
    StopIfBeyondBoundaries();
  }
}

void AxisDecelerationAnimation::PullBackIfBeyondBoundaries(int delta_time)
{
  // If we overshot, apply a spring acceleration to bring position
  // back to its max/min limit.

  if (pos_ > max_pos_)
  {
    float dt = delta_time;
    float acceleration = -overshoot_constant_ * (pos_ - max_pos_);
    pos_ += (vel_*dt) + (acceleration*dt*dt/2.0f);
    vel_ += acceleration*dt;

    if (fabs(pos_ - max_pos_) < 1.5f)
    {
      pos_ = max_pos_;
      vel_ = 0.0f;
    }
  }
  else if (pos_ < min_pos_)
  {
    float dt = delta_time;
    float acceleration = -overshoot_constant_ * (pos_ - min_pos_);
    pos_ += (vel_*dt) + (acceleration*dt*dt/2.0f);
    vel_ += acceleration*dt;

    if (fabs(pos_ - min_pos_) < 1.5f)
    {
      pos_ = min_pos_;
      vel_ = 0.0f;
    }
  }
}

float AxisDecelerationAnimation::GetPosition() const
{
  return pos_;
}

bool AxisDecelerationAnimation::IsActive() const
{
  if (pos_ < min_pos_ || pos_ > max_pos_)
    return true;
  else
    return (fabs(vel_) > MINIMUM_VELOCITY);
}

void AxisDecelerationAnimation::StopIfBeyondBoundaries()
{
  if (pos_ > max_pos_)
  {
    pos_ = max_pos_;
    vel_ = 0.0f;
  }
  else if (pos_ < min_pos_)
  {
    pos_ = min_pos_;
    vel_ = 0.0f;
  }
}
