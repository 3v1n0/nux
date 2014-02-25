// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2012 Inalogic® Inc.
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
 * Authored by: Tim Penhey <tim.penhey@canonical.com>
 *
 */
#ifndef NUX_CORE_ANIMATION_INL_H
#define NUX_CORE_ANIMATION_INL_H

namespace nux
{
namespace animation
{

template <typename VALUE_TYPE>
AnimateValue<VALUE_TYPE>::AnimateValue()
  : msec_current_(0)
  , msec_duration_(0)
  , start_value_(VALUE_TYPE())
  , finish_value_(VALUE_TYPE())
  , current_value_(start_value_)
{}

template <typename VALUE_TYPE>
AnimateValue<VALUE_TYPE>::AnimateValue(int msec_duration)
  : msec_current_(0)
  , msec_duration_(msec_duration)
  , start_value_(VALUE_TYPE())
  , finish_value_(VALUE_TYPE())
  , current_value_(start_value_)
{}

template <typename VALUE_TYPE>
AnimateValue<VALUE_TYPE>::AnimateValue(VALUE_TYPE const& start,
                                       VALUE_TYPE const& finish,
                                       int msec_duration)
  : msec_current_(0)
  , msec_duration_(msec_duration)
  , start_value_(start)
  , finish_value_(finish)
  , current_value_(start_value_)
{}

template <typename VALUE_TYPE>
AnimateValue<VALUE_TYPE>& AnimateValue<VALUE_TYPE>::SetStartValue(VALUE_TYPE const& start)
{
  start_value_ = start;
  return *this;
}

template <typename VALUE_TYPE>
AnimateValue<VALUE_TYPE>& AnimateValue<VALUE_TYPE>::SetFinishValue(VALUE_TYPE const& finish)
{
  finish_value_ = finish;
  return *this;
}

template <typename VALUE_TYPE>
AnimateValue<VALUE_TYPE>& AnimateValue<VALUE_TYPE>::SetDuration(int msecs)
{
  msec_duration_ = msecs;
  return *this;
}

template <typename VALUE_TYPE>
AnimateValue<VALUE_TYPE>& AnimateValue<VALUE_TYPE>::SetEasingCurve(EasingCurve const& curve)
{
  easing_curve_ = curve;
  return *this;
}

template <typename VALUE_TYPE>
int AnimateValue<VALUE_TYPE>::Duration() const
{
  return msec_duration_;
}

template <typename VALUE_TYPE>
int AnimateValue<VALUE_TYPE>::CurrentTimePosition() const
{
  return msec_current_;
}

template <typename VALUE_TYPE>
VALUE_TYPE const& AnimateValue<VALUE_TYPE>::GetStartValue() const
{
  return start_value_;
}

template <typename VALUE_TYPE>
VALUE_TYPE const& AnimateValue<VALUE_TYPE>::GetFinishValue() const
{
  return finish_value_;
}

template <typename VALUE_TYPE>
VALUE_TYPE const& AnimateValue<VALUE_TYPE>::GetCurrentValue() const
{
  return current_value_;
}

template <typename VALUE_TYPE>
void AnimateValue<VALUE_TYPE>::Advance(int msec)
{
  if (CurrentState() != Running)
    return;

  msec_current_ += msec;
  if (msec_current_ >= msec_duration_)
  {
    msec_current_ = msec_duration_;
    current_value_ = finish_value_;
    updated.emit(current_value_);
    Stop();
  }
  else
  {
    double progress = msec_current_ / static_cast<double>(msec_duration_);
    double value = easing_curve_.ValueForProgress(progress);
    // These operators work for most if not all the property types we care
    // about.  Should we need more, we'll reevaluate then.
    VALUE_TYPE new_value = start_value_ + ((finish_value_ - start_value_) * value);

    if (new_value != current_value_)
    {
      current_value_ = new_value;
      updated.emit(current_value_);
    }
  }
}

template <typename VALUE_TYPE>
void AnimateValue<VALUE_TYPE>::Restart()
{
  msec_current_ = 0;
  current_value_ = start_value_;
  updated.emit(current_value_);
}

template <typename VALUE_TYPE>
void AnimateValue<VALUE_TYPE>::Reverse()
{
  std::swap(start_value_, finish_value_);

  if (CurrentState() != Stopped)
  {
    msec_current_ = msec_duration_ - msec_current_;
  }
}


}}

#endif
