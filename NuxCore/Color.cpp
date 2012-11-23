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

#include "Color.h"
#include "ColorPrivate.h"

#include <cmath>
#include <cstdlib>
#include <algorithm>


#define NUX_RGBA_GET_ALPHA(rgba)      ((rgba) >> 24)
#define NUX_RGBA_GET_RED(rgba)        (((rgba) >> 16) & 0xff)
#define NUX_RGBA_GET_GREEN(rgba)      (((rgba) >> 8) & 0xff)
#define NUX_RGBA_GET_BLUE(rgba)       ((rgba) & 0xff)

namespace nux
{
namespace color
{
  Color::Color()
  : red(0.0f)
  , green(0.0f)
  , blue(0.0f)
  , alpha(1.0f)
  , premultiplied_(false)
  {}

  Color::Color(RedGreenBlue const& rgb, float a)
  : red(rgb.red)
  , green(rgb.green)
  , blue(rgb.blue)
  , alpha(a)
  , premultiplied_(false)
  {}

  Color::Color (unsigned int c)
  : red(NUX_RGBA_GET_RED(c) / 255.0f)
  , green(NUX_RGBA_GET_GREEN(c) / 255.0f)
  , blue(NUX_RGBA_GET_BLUE(c) / 255.0f)
  , alpha(NUX_RGBA_GET_ALPHA(c) / 255.0f)
  , premultiplied_(false)
  {}

  Color::Color(int r, int g, int b)
  : red(r / 255.0f)
  , green(g / 255.0f)
  , blue(b / 255.0f)
  , alpha(1.0f)
  , premultiplied_(false)
  {}

  Color::Color(float r, float g, float b, float a)
  : red(r)
  , green(g)
  , blue(b)
  , alpha(a)
  , premultiplied_(false)
  {}

  Color::Color(std::string const& hex)
  : red(0.0f)
  , green(0.0f)
  , blue(0.0f)
  , alpha(1.0f)
  , premultiplied_(false)
  {
    HexToRGBA(hex, red, green, blue, alpha);
  }

  Color Color::GetPremultiplied()
  {
    if (premultiplied_)
    {
      // Already pre-multiplied. Return *this;
      return *this;
    }

    Color c;
    c.SetPremultiplied(red, green, blue, alpha);
    return c;
  }

  void Color::SetPremultiplied(float r, float g, float b, float a)
  {
    red   = r * a;
    green = g * a;
    blue  = b * a;
    alpha = a;
    premultiplied_ = true;
  }

  bool Color::IsPremultiplied()
  {
    return premultiplied_;
  }

  bool operator == (const Color& lhs, const Color& rhs)
  {
    return (lhs.red == rhs.red &&
            lhs.green == rhs.green &&
            lhs.blue == rhs.blue &&
            lhs.alpha == rhs.alpha &&
            lhs.premultiplied_ == rhs.premultiplied_);
  }

  bool operator != (const Color& lhs, const Color& rhs)
  {
    return !(lhs == rhs);
  }

  Color RandomColor()
  {
    return Color(RandomColorINT());
  }

  unsigned int RandomColorINT()
  {
    // std::rand isn't defined to be more random than 2^15, so we need
    // to generate the full unsigned in chunks.
    // MSB is alpha, which is set to 255. The next three bytes are
    // red, green, blue.
    return (0xff << 24) | ((std::rand() & 0xff) << 16) | ((std::rand() & 0xff) << 8) | (std::rand() & 0xff);
  }

  Color operator + (Color const& lhs, Color const& rhs)
  {
    return Color(lhs.red + rhs.red,
                 lhs.green + rhs.green,
                 lhs.blue + rhs.blue,
                 lhs.alpha + rhs.alpha);
  }

  Color operator - (Color const& lhs, Color const& rhs)
  {
    return Color(lhs.red - rhs.red,
                 lhs.green - rhs.green,
                 lhs.blue - rhs.blue,
                 lhs.alpha - rhs.alpha);
  }

  Color operator + (float scalar, Color const& c)
  {
    return Color(c.red + scalar,
                 c.green + scalar,
                 c.blue + scalar,
                 c.alpha + scalar);
  }

  Color operator + (Color const& c, float scalar)
  {
    return scalar + c;
  }

  Color operator - (float scalar, Color const& c)
  {
    return Color(scalar - c.red,
                 scalar - c.green,
                 scalar - c.blue,
                 scalar - c.alpha);
  }


  Color operator - (Color const& c, float scalar)
  {
    return Color(c.red - scalar,
                 c.green - scalar,
                 c.blue - scalar,
                 c.alpha - scalar);
  }

  Color operator * (float scalar, Color const& c)
  {
    return Color(c.red * scalar,
                 c.green * scalar,
                 c.blue * scalar,
                 c.alpha * scalar);
  }

  Color operator * (Color const& c, float scalar)
  {
    return scalar * c;
  }

  // The Hue/Saturation/Value model was created by A. R. Smith in 1978. It is
  // based on such intuitive color characteristics as tint, shade and tone (or
  // family, purety and intensity). The coordinate system is cylindrical, and
  // the colors are defined inside a hexcone.  The hue value H runs from 0 to
  // 360º. The saturation S is the degree of strength or purity and is from 0 to
  // 1. Purity is how much white is added to the color, so S=1 makes the purest
  // color (no white). Brightness V also ranges from 0 to 1, where 0 is the
  // black.  There is no transformation matrix for RGB/HSV conversion, but the
  // algorithm follows:

  // r,g,b values are from 0 to 1
  // h = [0,360], s = [0,1], v = [0,1]
  //    if s == 0, then h = -1 (undefined)
  void RGBtoHSV(float r, float g, float b, float& h, float& s, float& v)
  {
    float mini, maxi, delta;

    mini = std::min(std::min(r, g), b);
    maxi = std::max(std::max(r, g), b);
    v = maxi;

    delta = maxi - mini;


    if (maxi != 0)
    {
      s = delta / maxi;
    }
    else
    {
      // MAX = 0 (i.e. if v = 0), then s is undefined.  r = g = b = 0
      s = 0;
      h = -1;
      return;
    }

    if (delta == 0) // MAX = MIN (i.e. s = 0), then h is undefined. r = g = b
    {
      h = 0;
      s = 0;
      return;
    }

    if (r == maxi)
      h = (g - b) / delta;      // between yellow & magenta
    else if (g == maxi)
      h = 2 + (b - r) / delta;  // between cyan & yellow
    else
      h = 4 + (r - g) / delta;  // between magenta & cyan

    h *= 60;  // degrees

    if (h < 0)
      h += 360;

    // convert h from [0, 360] to [0, 1]
    h = h / 360.0f;

  }

  void HSVtoRGB(float& r, float& g, float& b, float h, float s, float v )
  {
    int i;
    float f, p, q, t;

    // convert h from [0, 1] to [0, 360]
    h = h * 360.0f;

    if (s == 0)
    {
      // achromatic (grey)
      r = g = b = v;
      return;
    }

    h /= 60.0f;         // sector 0 to 5
    i = (int) std::floor(h);
    f = h - i;          // factorial part of h
    p = v * (1 - s);
    q = v * (1 - s * f);
    t = v * (1 - s * (1 - f));

    switch ( i )
    {
      case 0:
        r = v;
        g = t;
        b = p;
        break;
      case 1:
        r = q;
        g = v;
        b = p;
        break;
      case 2:
        r = p;
        g = v;
        b = t;
        break;
      case 3:
        r = p;
        g = q;
        b = v;
        break;
      case 4:
        r = t;
        g = p;
        b = v;
        break;
      default:    // case 5:
        r = v;
        g = p;
        b = q;
        break;
    }
  }

  /////////////////////////
  // HLS-RGB Conversions //
  /////////////////////////

  // Static function. Auxiliary to HLS2RGB().
  static float HLStoRGB_(float rn1, float rn2, float huei)
  {
    float hue = huei;

    if (hue > 360.0f)
      hue = hue - 360.0f;

    if (hue < 0.0f)
      hue = hue + 360.0f;

    if (hue < 60.0f )
      return rn1 + (rn2 - rn1) * hue / 60.0f;

    if (hue < 180.0f)
      return rn2;

    if (hue < 240.0f)
      return rn1 + (rn2 - rn1) * (240.0f - hue) / 60.0f;

    return rn1;
  }

  void HLStoRGB(float& r, float& g, float& b, float hue, float light, float satur)
  {
    // Static method to compute RGB from HLS. The l and s are between [0,1]
    // and h is between [0, 1]. The returned r,g,b triplet is between [0,1].
    hue *= 360.0f;

    float rh, rl, rs, rm1, rm2;
    rh = rl = rs = 0;

    if (hue   > 0)
      rh = hue;

    if (rh > 360.0f)
      rh = 360.0f;

    if (light > 0)
      rl = light;

    if (rl > 1)
      rl = 1;

    if (satur > 0)
      rs = satur;

    if (rs > 1)
      rs = 1.0f;

    if (rl <= 0.5f)
      rm2 = rl * (1.0f + rs);
    else
      rm2 = rl + rs - rl * rs;

    rm1 = 2.0f * rl - rm2;

    if (!rs)
    {
      r = rl;
      g = rl;
      b = rl;
      return;
    }

    r = HLStoRGB_(rm1, rm2, rh + 120);
    g = HLStoRGB_(rm1, rm2, rh);
    b = HLStoRGB_(rm1, rm2, rh - 120);
  }

  void RGBtoHLS(float rr, float gg, float bb,
                float& hue, float& light, float& satur)
  {
    // Static method to compute HLS from RGB. The r,g,b triplet is between
    // [0,1], hue is between [0,1], light and satur are [0,1].

    float rnorm, gnorm, bnorm, minval, maxval, msum, mdiff, r, g, b;
    r = g = b = 0;

    if (rr > 0)
      r = rr;

    if (r > 1)
      r = 1;

    if (gg > 0)
      g = gg;

    if (g > 1)
      g = 1;

    if (bb > 0)
      b = bb;

    if (b > 1)
      b = 1;

    minval = r;

    if (g < minval)
      minval = g;

    if (b < minval)
      minval = b;

    maxval = r;

    if (g > maxval)
      maxval = g;

    if (b > maxval)
      maxval = b;

    rnorm = gnorm = bnorm = 0;
    mdiff = maxval - minval;
    msum  = maxval + minval;
    light = 0.5f * msum;

    if (maxval != minval)
    {
      rnorm = (maxval - r) / mdiff;
      gnorm = (maxval - g) / mdiff;
      bnorm = (maxval - b) / mdiff;
    }
    else
    {
      satur = hue = 0;
      return;
    }

    if (light < 0.5f)
      satur = mdiff / msum;
    else
      satur = mdiff / (2.0f - msum);

    if (r == maxval)
      hue = 60.0f * (6.0f + bnorm - gnorm);
    else if (g == maxval)
      hue = 60.0f * (2.0f + rnorm - bnorm);
    else
      hue = 60.0f * (4.0f + gnorm - rnorm);

    if (hue > 360)
      hue = hue - 360;

    hue = hue / 360.0f;
  }

  RedGreenBlue::RedGreenBlue(float r, float g, float b)
  : red(r)
  , green(g)
  , blue(b)
  {}

  RedGreenBlue::RedGreenBlue(Color const& color)
  : red(color.red)
  , green(color.green)
  , blue(color.blue)
  {}

  RedGreenBlue::RedGreenBlue(HueSaturationValue const& hsv)
  {
    HSVtoRGB(red, green, blue, hsv.hue, hsv.saturation, hsv.value);
  }

  RedGreenBlue::RedGreenBlue(HueLightnessSaturation const& hls)
  {
    HLStoRGB(red, green, blue, hls.hue, hls.lightness, hls.saturation);
  }

  HueSaturationValue::HueSaturationValue(float h, float s, float v)
  : hue(h)
  , saturation(s)
  , value(v)
  {}

  HueSaturationValue::HueSaturationValue(RedGreenBlue const& rgb)
  {
    RGBtoHSV(rgb.red, rgb.green, rgb.blue, hue, saturation, value);
  }

  HueSaturationValue::HueSaturationValue(Color const& c)
  {
    RGBtoHSV(c.red, c.green, c.blue, hue, saturation, value);
  }

  HueLightnessSaturation::HueLightnessSaturation(float h, float l, float s)
  : hue(h)
  , lightness(l)
  , saturation(s)
  {}

  HueLightnessSaturation::HueLightnessSaturation(RedGreenBlue const& rgb)
  {
    RGBtoHLS(rgb.red, rgb.green, rgb.blue, hue, lightness, saturation);
  }

  HueLightnessSaturation::HueLightnessSaturation(Color const& c)
  {
    RGBtoHLS(c.red, c.green, c.blue, hue, lightness, saturation);
  }
}
}
