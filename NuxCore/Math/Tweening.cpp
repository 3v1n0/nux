/*
 * Copyright 2010-2012 Inalogic® Inc.
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


#include "../NuxCore.h"
#include "MathInc.h"

#include "Tweening.h"

namespace nux
{

// Back
  double BackEaseIn (double t, double b, double c, double d, double s)
  {
    //return c*(t/=d)*t*((s+1)*t - s) + b;
    t /= d;
    return c * t * t * ( (s + 1) * t - s) + b;
  }

  double BackEaseOut (double t, double b, double c, double d, double s)
  {
    //return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
    t = t / d - 1;
    return c * (t * t * ( (s + 1) * t + s) + 1) + b;
  }

  double BackEaseInOut (double t, double b, double c, double d, double s)
  {
//     if ((t/=d/2) < 1)
//         return c/2*(t*t*(((s*=(1.525))+1)*t - s)) + b;
//     return c/2*((t-=2)*t*(((s*=(1.525))+1)*t + s) + 2) + b;

    if ( (t /= d / 2) < 1)
    {
      s *= (1.525);
      return c / 2 * (t * t * ( (s + 1) * t - s) ) + b;
    }

    s *= 1.525;
    t -= 2.0;
    return c / 2 * (t * t * ( (s + 1) * t + s) + 2) + b;
  }

// Bounce
  double BounceEaseOut (double t, double b, double c, double d)
  {
    if ( (t /= d) < (1 / 2.75) )
    {
      return c * (7.5625 * t * t) + b;
    }
    else if (t < (2 / 2.75) )
    {
      t -= (1.5 / 2.75);
      return c * (7.5625 * t * t + .75) + b;
    }
    else if (t < (2.5 / 2.75) )
    {
      t -= (2.25 / 2.75);
      return c * (7.5625 * t * t + .9375) + b;
    }
    else
    {
      t -= (2.625 / 2.75);
      return c * (7.5625 * t * t + .984375) + b;
    }
  }

  double BounceEaseIn (double t, double b, double c, double d)
  {
    return c - BounceEaseOut (d - t, 0, c, d) + b;
  }

  double BounceEaseInOut (double t, double b, double c, double d)
  {
    if (t < d / 2.0)
      return BounceEaseIn (t * 2, 0, c, d) * .5 + b;
    else
      return BounceEaseOut (t * 2 - d, 0, c, d) * .5 + c * .5 + b;
  }

// Circ
  double CircEaseIn (double t, double b, double c, double d)
  {

    t /= d;
    return -c * (std::sqrt (1 - t * t) - 1) + b;
  }

  double CircEaseOut (double t, double b, double c, double d)
  {
    t = t / d - 1.0;
    return c * std::sqrt (1.0 - t * t) + b;
  }

  double CircEaseInOut (double t, double b, double c, double d)
  {
    if ( (t /= d / 2) < 1)
    {
      return -c / 2 * (std::sqrt (1 - t * t) - 1) + b;
    }

    t -= 2.0;
    return c / 2 * (std::sqrt (1.0 - t * t) + 1) + b;
  }

// Cubic
  double CubicEaseIn (double t, double b, double c, double d)
  {
    t /= d;
    return c * t * t * t + b;
  }

  double CubicEaseOut (double t, double b, double c, double d)
  {
    t = t / d - 1.0;
    return c * (t * t * t + 1.0) + b;
  }

  double CubicEaseInOut (double t, double b, double c, double d)
  {
    if ( (t /= d / 2) < 1)
    {
      return c / 2 * t * t * t + b;
    }

    t -= 2.0;
    return c / 2 * (t * t * t + 2) + b;
  }

// Elastic
  double ElasticEaseIn (double t, double b, double c, double d, double a, double p)
  {
    double s = 0;

    if (t == 0)
      return b;

    if ( (t /= d) == 1)
      return b + c;

    if (!p)
      p = d * .3;

    if (!a || a < std::abs (c) )
    {
      a = c;
      s = p / 4;
    }
    else
      s = p / (2 * constants::pi) * std::asin (c / a);

    t -= 1.0;
    return - (a * std::pow (2, 10 * t) * std::sin ( (t * d - s) * (2 * constants::pi) / p ) ) + b;
  }

  double ElasticEaseOut (double t, double b, double c, double d, double a, double p)
  {
    double s = 0;

    if (t == 0)
      return b;

    if ( (t /= d) == 1)
      return b + c;

    if (!p)
      p = d * .3;

    if (!a || a < std::abs (c) )
    {
      a = c;
      s = p / 4;
    }
    else
      s = p / (2 * constants::pi) * std::asin (c / a);

    return (a * std::pow (2, -10 * t) * std::sin ( (t * d - s) * (2 * constants::pi) / p ) + c + b);
  }

  double ElasticEaseInOut (double t, double b, double c, double d, double a, double p)
  {
    double s = 0;

    if (t == 0)
      return b;

    if ( (t /= d / 2) == 2)
      return b + c;

    if (!p)
      p = d * (.3 * 1.5);

    if (!a || a < std::abs (c) )
    {
      a = c;
      s = p / 4;
    }
    else
    {
      s = p / (2 * constants::pi) * std::asin (c / a);
    }

    if (t < 1.0)
    {
      t -= 1;
      return -0.5 * (a * std::pow (2, 10.0 * t) * std::sin ( (t * d - s) * (2 * constants::pi) / p ) ) + b;
    }

    t -= 1;
    return a * std::pow (2, -10 * t) * std::sin ( (t * d - s) * (2 * constants::pi) / p ) * .5 + c + b;
  }

// Expo
  double ExpoEaseIn (double t, double b, double c, double d)
  {
    return (t == 0) ? b : c * std::pow (2, 10 * (t / d - 1) ) + b;
  }

  double ExpoEaseOut (double t, double b, double c, double d)
  {
    return (t == d) ? b + c : c * (-std::pow (2, -10 * t / d) + 1) + b;
  }

  double ExpoEaseInOut (double t, double b, double c, double d)
  {
    if (t == 0)
      return b;

    if (t == d)
      return b + c;

    if ( (t /= d / 2) < 1)
      return c / 2 * std::pow (2, 10 * (t - 1) ) + b;

    return c / 2 * (-std::pow (2, -10 * --t) + 2) + b;
  }

// Linear
  double LinearEaseNone (double t, double b, double c, double d)
  {
    return c * t / d + b;
  }

  double LinearEaseIn (double t, double b, double c, double d)
  {
    return c * t / d + b;
  }

  double LinearEaseOut (double t, double b, double c, double d)
  {
    return c * t / d + b;
  }

  double LinearEaseInOut (double t, double b, double c, double d)
  {
    return c * t / d + b;
  }

// Quad
  double QuadEaseIn (double t, double b, double c, double d)
  {
    t /= d;
    return c * t * t + b;
  }

  double QuadEaseOut (double t, double b, double c, double d)
  {
    t /= d;
    return -c * t * (t - 2) + b;
  }

  double QuadEaseInOut (double t, double b, double c, double d)
  {
    if ( (t /= d / 2) < 1)
    {
      return c / 2 * t * t + b;
    }

    --t;
    return -c / 2 * (t * (t - 2) - 1) + b;
  }

// Quart
  double QuartEaseIn (double t, double b, double c, double d)
  {
    t /= d;
    return c * t * t * t * t + b;
  }

  double QuartEaseOut (double t, double b, double c, double d)
  {
    t = t / d - 1;
    return -c * (t * t * t * t - 1) + b;
  }

  double QuartEaseInOut (double t, double b, double c, double d)
  {
    if ( (t /= d / 2) < 1)
    {
      return c / 2 * t * t * t * t + b;
    }

    t -= 2.0;
    return -c / 2 * (t * t * t * t - 2) + b;
  }

// Quint
  double QuintEaseIn (double t, double b, double c, double d)
  {
    t /= d;
    return c * t * t * t * t * t + b;
  }

  double QuintEaseOut (double t, double b, double c, double d)
  {
    t = t / d - 1;
    return c * (t * t * t * t * t + 1) + b;
  }

  double QuintEaseInOut (double t, double b, double c, double d)
  {
    if ( (t /= d / 2) < 1)
    {
      return c / 2 * t * t * t * t * t + b;
    }

    t -= 2;
    return c / 2 * (t * t * t * t * t + 2) + b;
  }

// Sine
  double SineEaseIn (double t, double b, double c, double d)
  {
    return -c * std::cos (t / d * (constants::pi / 2) ) + c + b;
  }

  double SineEaseOut (double t, double b, double c, double d)
  {
    return c * std::sin (t / d * (constants::pi / 2) ) + b;
  }

  double SineEaseInOut (double t, double b, double c, double d)
  {
    return -c / 2 * (std::cos (constants::pi * t / d) - 1) + b;
  }

}
