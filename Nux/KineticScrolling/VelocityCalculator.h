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

#ifndef NUX_VELOCITY_CALCULATOR_H
#define NUX_VELOCITY_CALCULATOR_H

#include <stdint.h>

namespace nux
{

/*
  Estimates the current velocity of a finger based on recent movement

  Taking an estimate from a reasonable number of samples, instead of only
  from its last movement, removes wild variations in velocity caused
  by the jitter normally present in input from a touchscreen.
 */
class VelocityCalculator
{
 public:
  VelocityCalculator();
  /*
    How much the finger has moved since ProcessMovement() was last called.
   */
  void ProcessMovement(int movement);

  /*
    Calculates the finger velocity, in axis units/millisecond
   */
  float CalculateVelocity() const;

  /*
    Removes all stored movements from previous calls to ProcessMovement()
   */
  void Reset();

  /*
    Maximum number of movement samples stored
   */
  static const int MAX_SAMPLES = 10;

  /*
    Age of the oldest sample considered in the velocity calculations, in
    milliseconds, compared to the most recent one.
   */
  static const int AGE_OLDEST_SAMPLE = 200;

 private:
  int NumSamples() const;

  class Sample
  {
    public:
      int mov; /* movement distance since last sample */
      int64_t time; /* time, in milliseconds */
  };

  /* a circular buffer of samples */
  Sample samples_[MAX_SAMPLES];
  int samples_read_; /* index of the oldest sample available. -1 if buffer is empty */
  int samples_write_; /* index where the next sample will be written */
};

} // namespace nux
#endif // NUX_VELOCITY_CALCULATOR_H
