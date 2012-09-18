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
#ifndef NUX_CORE_EASING_CURVE_H
#define NUX_CORE_EASING_CURVE_H


namespace nux
{
namespace animation
{

class EasingCurve
{
public:
#if defined(NUX_OS_WINDOWS) && !defined(NUX_VISUAL_STUDIO_VC11)
  enum Type
#else
  enum class Type
#endif
  {
    Linear,
    InQuad,
    OutQuad,
    InOutQuad,
    BackEaseIn,
    BackEaseOut,
    BackEaseInOut,
    BounceIn,
    BounceOut,
    BounceInOut,
    ExpoEaseIn,
    ExpoEaseOut
  };

  typedef double (*EasingFunction)(double);

  explicit EasingCurve(Type type = Type::Linear);

  // progress is expected to be between zero and one inclusive.
  //
  // Values less than zero will return zero will return zero, and
  // greater than one will return one.
  //
  // The returned value may be greater than one, or less than zero for some
  // special curves.
  double ValueForProgress(double progress);

private:
  EasingFunction func_;
};



}}

#endif
