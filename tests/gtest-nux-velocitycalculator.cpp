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
#include <Nux/KineticScrolling/VelocityCalculator.h>
#include "gtest-nux-globals.h"

using namespace nux;

TEST(VelocityCalculator, SimpleSamples)
{
  VelocityCalculator vel_calc;

  g_fake_monotonic_time = 10 * 1000;
  vel_calc.ProcessMovement(20);

  g_fake_monotonic_time = 20 * 1000;
  vel_calc.ProcessMovement(20);

  g_fake_monotonic_time = 30 * 1000;
  vel_calc.ProcessMovement(20);

  float velocity = vel_calc.CalculateVelocity();

  ASSERT_FLOAT_EQ(2.0f, velocity);
}

TEST(VelocityCalculator, NoSamples)
{
  VelocityCalculator vel_calc;

  float velocity = vel_calc.CalculateVelocity();

  ASSERT_FLOAT_EQ(0.0f, velocity);
}

TEST(VelocityCalculator, OverflowSamples)
{
  VelocityCalculator vel_calc;

  for (int i = 0; i < 1000; ++i)
  {
    g_fake_monotonic_time += 10 * 1000;
    vel_calc.ProcessMovement(20);
  }

  /* overwrite all existing samples with faster ones */
  for (int i = 0; i < VelocityCalculator::MAX_SAMPLES; ++i)
  {
    g_fake_monotonic_time += 10 * 1000;
    vel_calc.ProcessMovement(40);
  }

  float velocity = vel_calc.CalculateVelocity();

  /* check that the calculated velocity correspond to the latest, faster, samples */
  ASSERT_FLOAT_EQ(4.0f, velocity);
}

TEST(VelocityCalculator, Average)
{
  VelocityCalculator vel_calc;

  g_fake_monotonic_time = 10 * 1000;
  vel_calc.ProcessMovement(20);

  g_fake_monotonic_time = 20 * 1000;
  vel_calc.ProcessMovement(20);

  /* the last sample is an erratic one and would yield a big velocity if
     considered isolatedly */
  g_fake_monotonic_time = 30 * 1000;
  vel_calc.ProcessMovement(100);

  float velocity = vel_calc.CalculateVelocity();

  /* calculated velocity is lower than the one from the last sample */
  ASSERT_TRUE(velocity < 9.0f);

  /* but it's higher the the slow samples */
  ASSERT_TRUE(velocity > 2.5f);
}
