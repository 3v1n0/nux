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

#include "VelocityCalculator.h"
#include <glib.h>

using namespace nux;

VelocityCalculator::VelocityCalculator()
{
  Reset();
}
#include <iostream>
void VelocityCalculator::ProcessMovement(int movement)
{
  if (samples_read_ == -1)
  {
    samples_read_ = samples_write_;
  }
  else if (samples_read_ == samples_write_)
  {
    /* the oldest value is going to be overwritten.
       so now the oldest will be the next one. */
    samples_read_ = (samples_read_ + 1) % MAX_SAMPLES;
  }

  samples_[samples_write_].mov = movement;
  samples_[samples_write_].time = g_get_monotonic_time() / 1000;
  samples_write_ = (samples_write_ + 1) % MAX_SAMPLES;
}

float VelocityCalculator::CalculateVelocity() const
{
  if (NumSamples() < 2)
    return 0.0f;

  int last_index;
  if (samples_write_ == 0)
    last_index = MAX_SAMPLES - 1;
  else
    last_index = samples_write_ - 1;

  int64_t curr_time = samples_[last_index].time;

  int total_time = 0;
  int total_distance = 0;

  int sample_index = (samples_read_ + 1) % MAX_SAMPLES;
  int64_t previous_time = samples_[samples_read_].time;
  while (sample_index != samples_write_)
  {
    // Skip this sample if it's too old
    if (curr_time - samples_[sample_index].time <= AGE_OLDEST_SAMPLE)
    {
      int delta_time = samples_[sample_index].time - previous_time;
      total_distance += samples_[sample_index].mov;
      total_time += delta_time;
    }

    previous_time = samples_[sample_index].time;
    sample_index = (sample_index + 1) % MAX_SAMPLES;
  }

  return float(total_distance) / float(total_time);
}

void VelocityCalculator::Reset()
{
  samples_read_ = -1;
  samples_write_ = 0;
}

int VelocityCalculator::NumSamples() const
{
  if (samples_read_ == -1)
  {
    return 0;
  }
  else
  {
    if (samples_write_ == 0)
    {
      /* consider only what's to the right of samples_read_ (including himself) */
      return MAX_SAMPLES - samples_read_;
    }
    else if (samples_write_ == samples_read_)
      return MAX_SAMPLES; /* buffer is full */
    else if (samples_write_ < samples_read_)
      return (MAX_SAMPLES - samples_read_) + samples_write_;
    else
      return samples_write_ - samples_read_;
  }
}
