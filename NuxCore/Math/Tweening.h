/*
 * Copyright 2010 Inalogic® Inc.
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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef TWEENING_H
#define TWEENING_H


namespace nux
{

// Back
  double BackEaseIn (double t, double b, double c, double d, double s = 1.70158);
  double BackEaseOut (double t, double b, double c, double d, double s = 1.70158);
  double BackEaseInOut (double t, double b, double c, double d, double s = 1.70158);
// Bounce
  double BounceEaseOut (double t, double b, double c, double d);
  double BounceEaseIn (double t, double b, double c, double d);
  double BounceEaseInOut (double t, double b, double c, double d);
// Circ
  double CircEaseIn (double t, double b, double c, double d);
  double CircEaseOut (double t, double b, double c, double d);
  double CircEaseInOut (double t, double b, double c, double d);
// Cubic
  double CubicEaseIn (double t, double b, double c, double d);
  double CubicEaseOut (double t, double b, double c, double d);
  double CubicEaseInOut (double t, double b, double c, double d);
// Elastic
  double ElasticEaseIn (double t, double b, double c, double d, double a, double p);
  double ElasticEaseOut (double t, double b, double c, double d, double a, double p);
  double ElasticEaseInOut (double t, double b, double c, double d, double a, double p);
// Expo
  double ExpoEaseIn (double t, double b, double c, double d);
  double ExpoEaseOut (double t, double b, double c, double d);
  double ExpoEaseInOut (double t, double b, double c, double d);
// Linear
  double LinearEaseNone (double t, double b, double c, double d);
  double LinearEaseIn (double t, double b, double c, double d);
  double LinearEaseOut (double t, double b, double c, double d);
  double LinearEaseInOut (double t, double b, double c, double d);
// Quad
  double QuadEaseIn (double t, double b, double c, double d);
  double QuadEaseOut (double t, double b, double c, double d);
  double QuadEaseInOut (double t, double b, double c, double d);
// Quart
  double QuartEaseIn (double t, double b, double c, double d);
  double QuartEaseOut (double t, double b, double c, double d);
  double QuartEaseInOut (double t, double b, double c, double d);
// Quint
  double QuintEaseIn (double t, double b, double c, double d);
  double QuintEaseOut (double t, double b, double c, double d);
  double QuintEaseInOut (double t, double b, double c, double d);
// Sine
  double SineEaseIn (double t, double b, double c, double d);
  double SineEaseOut (double t, double b, double c, double d);
  double SineEaseInOut (double t, double b, double c, double d);

}

#endif // TWEENING_H

