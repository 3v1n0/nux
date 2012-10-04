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
#include "KineticAxisScroller.h"
#include "AxisDecelerationAnimation.h"
#include "VelocityCalculator.h"
#include <memory>
#include "KineticScrollingTickSource.h"

using namespace nux;

/*****************************************************************************
 * Private class
 *****************************************************************************/

class KineticAxisScroller::Private : public sigc::trackable
{
 public:

  Private();

  void CalculateContentPosLimits();

  void ProcessFingerDown();

  void ProcessFingerUp();

  void ProcessFingerDrag(int mov);
  void ProcessFingerDrag_Pressed(int mov);
  void ProcessFingerDrag_FollowingFinger(int mov);

  void UpdateAnimations(int time);

  int LimitOutOfBoundsMovement(int movement, int position,
                               int min_position) const;

  float viewport_length_;
  float content_length_;
  float content_pos_;
  float min_content_pos_;

  BoundsBehavior bounds_behavior_;

  KineticScrollerAxisState state_;

  VelocityCalculator velocity_calculator_;
  AxisDecelerationAnimation deceleration_animation_;
  int accumulated_movement_;
};

KineticAxisScroller::Private::Private()
{
  viewport_length_ = 0.0f;
  content_length_ = 0.0f;
  content_pos_ = 0.0f;
  min_content_pos_ = 0.0f;
  bounds_behavior_ = DragAndOvershootBounds;
  state_ = KineticScrollerAxisStateIdle;
}

void KineticAxisScroller::Private::CalculateContentPosLimits()
{
  if (content_length_ > viewport_length_)
    min_content_pos_ = -(content_length_ - viewport_length_);
  else
    min_content_pos_ = 0.0f;
}

void KineticAxisScroller::Private::ProcessFingerDown()
{
  switch (state_)
  {
    case KineticScrollerAxisStateIdle:
      state_ = KineticScrollerAxisStatePressed;
      accumulated_movement_ = 0;
      break;
    case KineticScrollerAxisStateMovingByInertia:
      state_ = KineticScrollerAxisStatePressed;
      accumulated_movement_ = 0;
      break;
    default:
      /* Ignore it */
      break;
  }
}

void KineticAxisScroller::Private::ProcessFingerUp()
{
  // consider the elapsed time between the last drag update and now, that the
  // finger has been lifted (it could have been still for a while).
  velocity_calculator_.ProcessMovement(0);
  float velocity = velocity_calculator_.CalculateVelocity();

  if (fabs(velocity) < MINIMUM_FLICK_SPEED)
    velocity = 0.0f;
  else
    velocity *= FLICK_BOOST;

  /* Always run the animation, even if there's no starting speed,
     as it will pull the content back to a valid position if it
     was dragged beyond valid boundaries */
  deceleration_animation_.SetMinimumPosition(min_content_pos_);
  deceleration_animation_.SetMaximumPosition(0.0f);
  deceleration_animation_.SetStartPosition(content_pos_);
  deceleration_animation_.SetStartVelocity(velocity);
  deceleration_animation_.Start();

  state_ = KineticScrollerAxisStateMovingByInertia;
}

void KineticAxisScroller::Private::ProcessFingerDrag(int movement)
{
  movement = LimitOutOfBoundsMovement(movement, content_pos_, min_content_pos_);

  if (movement == 0)
    return;

  if (state_ == KineticScrollerAxisStatePressed)
  {
    ProcessFingerDrag_Pressed(movement);
  }
  else // KineticScrollerAxisState::FollowingFinger
  {
    ProcessFingerDrag_FollowingFinger(movement);
  }
}

void KineticAxisScroller::Private::ProcessFingerDrag_Pressed(int movement)
{
  accumulated_movement_ += movement;

  if (abs(accumulated_movement_) > KineticAxisScroller::MOVEMENT_THRESHOLD)
  {
    state_ = KineticScrollerAxisStateFollowingFinger;
    velocity_calculator_.Reset();

    ProcessFingerDrag_FollowingFinger(movement);
  }
}

void KineticAxisScroller::Private::ProcessFingerDrag_FollowingFinger(int movement)
{
  /* TODO: Filter the input for a smoother movement.
    E.g.: make the content follow the finger with a small delay instead of
    exactly following it. That will remove the inherent jitter in input coming
    from a touchscreen. */

  content_pos_ += movement;

  velocity_calculator_.ProcessMovement(movement);
}

void KineticAxisScroller::Private::UpdateAnimations(int delta_time)
{
  if (deceleration_animation_.IsActive())
  {
    deceleration_animation_.Update(delta_time);
    content_pos_ = deceleration_animation_.GetPosition();

    if (!deceleration_animation_.IsActive())
      state_ = KineticScrollerAxisStateIdle;
  }
  else
  {
    state_ = KineticScrollerAxisStateIdle;
  }
}

int KineticAxisScroller::Private::LimitOutOfBoundsMovement(int movement,
                                                           int position,
                                                           int min_position) const
{
  int new_position = position + movement;

  if (new_position > 0)
  {
    if (bounds_behavior_ == StopAtBounds)
    {
      return 0 - position;
    }
    else
    {
      /* hinder movement to hint user that he's dragging content beyound its
         boundaries*/
      return movement / 2;
    }
  }
  else if (new_position < min_position)
  {
    if (bounds_behavior_ == StopAtBounds)
    {
      return min_position - position;
    }
    else
    {
      return movement / 2;
    }
  }
  else
  {
    /* new_position is still within bounds */
    return movement;
  }
}

/*****************************************************************************
 * Public class
 *****************************************************************************/

const float KineticAxisScroller::MINIMUM_FLICK_SPEED = 0.003f;
const float KineticAxisScroller::FLICK_BOOST = 2.0f;

KineticAxisScroller::KineticAxisScroller()
  : p(new Private)
{
}

KineticAxisScroller::~KineticAxisScroller()
{
  delete p;
}

void KineticAxisScroller::SetViewportLength(int length)
{
  p->viewport_length_ = length;
  p->CalculateContentPosLimits();
}

int KineticAxisScroller::GetViewportLength() const
{
  return p->viewport_length_;
}

void KineticAxisScroller::SetContentLength(int length)
{
  p->content_length_ = length;
  p->CalculateContentPosLimits();
}

int KineticAxisScroller::GetContentLength() const
{
  return p->content_length_;
}

void KineticAxisScroller::SetContentPosition(int pos)
{
  p->content_pos_ = pos;
}

void KineticAxisScroller::SetBoundsBehavior(BoundsBehavior bounds_behavior)
{
  p->bounds_behavior_ = bounds_behavior;

  p->deceleration_animation_.SetOvershootBoundsEnabled(
      bounds_behavior != BoundsBehavior::StopAtBounds);
}

void KineticAxisScroller::ProcessFingerDown()
{
  p->ProcessFingerDown();
}

void KineticAxisScroller::ProcessFingerUp()
{
  p->ProcessFingerUp();
}

void KineticAxisScroller::ProcessFingerDrag(int movement)
{
  p->ProcessFingerDrag(movement);
}

void KineticAxisScroller::UpdateTime(int delta_time)
{
  p->UpdateAnimations(delta_time);
}

bool KineticAxisScroller::NeedTimeUpdates() const
{
  return p->state_ == KineticScrollerAxisStateMovingByInertia
    && p->deceleration_animation_.IsActive();
}

KineticScrollerAxisState KineticAxisScroller::GetState() const
{
  return p->state_;
}

int KineticAxisScroller::GetContentPosition() const
{
  return p->content_pos_;
}
