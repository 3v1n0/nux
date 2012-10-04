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

#ifndef NUX_AXIS_DECELERATION_ANIMATION_H
#define NUX_AXIS_DECELERATION_ANIMATION_H

namespace nux
{

/*!
  Animates the deceleration along an axis until it comes to a halt

  Usage:

  1 - Setup the animation

  animation.SetMinimumPosition(0.0f);
  animation.SetMaximumPosition(100.0f);
  animation.SetStartPosition(34.0f);
  animation.SetStartVelocity(2.0f);

  2 - Start it

  animation.Start();

  3 - Update it at regular intervals until it finishes

  animation.Update(delta_time);
  new_pos = animation.GetPosition();
  if (!animation.IsActive())
    Stop updating this animation
 */
class AxisDecelerationAnimation
{
 public:
  /*! Sets whether position limits can be momentarily overshot
   */
  void SetOvershootBoundsEnabled(bool enable);

  void SetMinimumPosition(float min_pos);
  void SetMaximumPosition(float max_pos);
  void SetStartPosition(float position);

  /*!
    \param velocity Initial velocity, in axis units per millisecond.
   */
  void SetStartVelocity(float velocity);

  /*!
    Marks the end of the setup phase and sets the animation time to 0.

    After that, Update() should be called until IsActive() returns false.

    This method can also be used to restart an ongoing animation.
   */
  void Start();

  /*!
    Moves the animation forward by delta_time milliseconds.

    After calling this method you will normally want to know the new
    position (GetPosition()) and whether the animation has finished (IsActive()),
    meaning that movement along that axis came to a halt.

    \param delta_time Time elapsed since the last call to Update()
   */ 
  void Update(int delta_time);

  /*!
    Current position along that axis. It changes by every call made to Update()
    while IsActive() is true.
   */
  float GetPosition() const;

  /*!
    Returns whether the animation has finished, meaning that further calls to Update()
    will no longer change the position. I.e., movement reached a complete halt.
   */
  bool IsActive() const;

 private:
  void PullBackIfBeyondBoundaries(int delta_time);
  void StopIfBeyondBoundaries();

  static const float MINIMUM_VELOCITY;
  float start_pos_;
  float start_vel_;
  float pos_;
  float vel_;
  float min_pos_;
  float max_pos_;
  int elapsed_time_;
  float decay_constant_;
  float overshoot_constant_;
  bool overshoot_bounds_;
};

} // namespace nux
#endif // NUX_AXIS_DECELERATION_ANIMATION_H
