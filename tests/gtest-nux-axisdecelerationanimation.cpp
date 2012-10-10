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

#include <gtest/gtest.h>
#include <Nux/KineticScrolling/AxisDecelerationAnimation.h>

using namespace nux;

TEST(AxisDecelerationAnimation, ComesToAHalt)
{
  AxisDecelerationAnimation animation;
  float min_pos = 0.0f;
  float max_pos = 100.0f;
  float start_pos = 10.0f;

  animation.SetMinimumPosition(min_pos);
  animation.SetMaximumPosition(max_pos);
  animation.SetStartPosition(start_pos);
  animation.SetStartVelocity(100.0f);
  animation.SetOvershootBoundsEnabled(false);

  animation.Start();

  float last_pos = start_pos;
  float last_movement = 0.0f;
  for (int i = 0; i < 1000 && animation.IsActive(); ++i)
  {
    animation.Update(16);

    /* Check that it's moving along the velocity direction */
    ASSERT_TRUE(animation.GetPosition() > last_pos);

    float movement = animation.GetPosition() - last_pos;

    if (i > 0)
    {
      /* Check that it's decelerating */
      ASSERT_TRUE(movement - last_movement);
    }

    last_pos = animation.GetPosition();
    last_movement = movement;
  }

  /* Check that it came to a halt */
  ASSERT_FALSE(animation.IsActive());
}

TEST(AxisDecelerationAnimation, StopAtBounds)
{
  AxisDecelerationAnimation animation;
  float min_pos = 0.0f;
  float max_pos = 100.0f;
  float start_pos = 50.0f;

  animation.SetMinimumPosition(min_pos);
  animation.SetMaximumPosition(max_pos);

  /* test different velocities on both directions */
  std::vector<int> velocities;
  for (int i = 1; i <= 100; ++i)
  {
    velocities.push_back(i);
  }
  for (int i = -1; i >= -100; --i)
  {
    velocities.push_back(i);
  }

  for (size_t i = 0; i < velocities.size(); ++i)
  {
    animation.SetStartPosition(start_pos);
    animation.SetStartVelocity(velocities[i]);
    animation.Start();

    for (int j = 0; j < 1000 && animation.IsActive(); ++j)
    {
      animation.Update(16);
    }
    ASSERT_FALSE(animation.IsActive());

    ASSERT_LE(animation.GetPosition(), max_pos);
    ASSERT_GE(animation.GetPosition(), min_pos);
  }
}
