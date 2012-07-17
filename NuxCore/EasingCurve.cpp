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

#include "EasingCurve.h"

namespace na = nux::animation;

namespace
{
// These easing curves are an attempt to match the Qt curves
// defined here:
//   http://doc.qt.nokia.com/4.7-snapshot/qeasingcurve.html

double linear(double progress)
{
  return progress;
}

double in_quad(double progress)
{
  return progress * progress;
}

double reverse(double progress, EasingCurve::EasingFunction func)
{
  // invert progress, and pass to ease_in
  progress = 1 - progress;
  return 1 - func(progress);
}

doube in_out(double progress, EasingCurve::EasingFunction func)
{
  if (progress > 0.5)
  {
    double out_progress = 1 - (progress - 0.5) * 2;
    return 0.5 - (func(out_progress) / 2);
  }
  else
  {
    double in_progress = progress * 2;
    return func(in_progress) / 2;
  }
}

double out_quad(double progress)
{
  double shift = 1 - progress;
  return 1 - (shift * shift);
}

double in_out_quad(double progress)
{
  if (progress > 0.5)
  {
    double out_progress = (progress - 0.5) * 2;
    return 0.5 + (out_quad(out_progress) / 2);
  }
  else
  {
    double in_progress = progress * 2;
    return in_quad(in_progress) / 2;
  }
}

double back_ease_in(double progress)
{
  // (s+1)*t^3 - s*t^2
  const double overshoot = 1.70158;
  return (((overshoot + 1) * progress * progress * progress) -
          (overshoot * progress * progress));
}

double back_ease_out(double progress)
{
  return reverse(progress, back_ease_in);
}

double back_ease_in_out(double progress)
{
  return in_out(progress, back_ease_in);
}

na::EasingCurve::EasingFunction GetEasingFunction(na::EasingCurve::Type type)
{
  switch (type)
  {
  case na::EasingCurve::Type::InQuad:
    return in_quad;
  case na::EasingCurve::Type::OutQuad:
    return out_quad;
  case na::EasingCurve::Type::InOutQuad:
    return in_out_quad;
  case na::EasingCurve::Type::BackEaseIn:
    return back_ease_in;
  case na::EasingCurve::Type::BackEaseOut:
    return back_ease_out;
  case na::EasingCurve::Type::BackEaseInOut:
    return back_ease_in_out;
  case na::EasingCurve::Type::Linear:
  default:
    return linear;
  }
}

}


na::EasingCurve::EasingCurve(Type type)
  : func_(GetEasingFunction(type))
{}

double na::EasingCurve::ValueForProgress(double progress)
{
  if (progress <= 0) return 0;
  if (progress >= 1) return 1;

  return func_(progress);
}
