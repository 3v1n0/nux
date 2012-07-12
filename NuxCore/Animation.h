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
#ifndef NUX_CORE_ANIMATION_H
#define NUX_CORE_ANIMATION_H

#include <boost/utility.hpp>
#include <sigc++/signal.h>

#include "EasingCurve.h"

namespace nux
{
namespace animation
{

class Animation : boost::noncopyable
{
public:
  enum State
  {
    Stopped,
    Paused,
    Running,
  };

  Animation();
  virtual ~Animation();

  virtual int Duration() const = 0;

  void Pause();
  void Resume();
  void SetCurrentTime(int msecs);
  void SetPaused(bool paused);
  void Start();
  void Stop();

  State CurrentState() const;
  sigc::signal<void> finished;

private:
  State state_;
};


template <typename VALUE_TYPE>
class AnimateValue : public Animation
{
public:
  AnimateValue();
  AnimateValue(VALUE_TYPE const& start,
               VALUE_TYPE const& finish,
               int msec_duration_);

  AnimateValue& SetStartValue(VALUE_TYPE const& start);
  AnimateValue& SetFinishValue(VALUE_TYPE const& finsih);
  AnimateValue& SetDuration(int msecs);
  AnimateValue& SetEasingCurve(EasingCurve const& curve);

  virtual int Duration() const;

  sigc::signal<void, VALUE_TYPE const&> updated;
  VALUE_TYPE const& GetCurrentValue() const;

private:
  int msec_duration_;
  VALUE_TYPE start_value_;
  VALUE_TYPE finish_value_;
  VALUE_TYPE current_value_;
  EasingCurve easing_curve_;
};


}}

#include "Animation-inl.h"

#endif
