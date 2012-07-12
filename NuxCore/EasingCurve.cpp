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
