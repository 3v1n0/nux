/*
 * Copyright 2010 InalogicÂ® Inc.
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


#include "NuxCore.h"
#include "Math/MathUtility.h"
#include <cmath>
#include "SystemTypes.h"
#include "Color.h"


namespace nux
{
  Color::Color()
    : _red (0.0), _green (0.0), _blue (0.0f), _alpha (1.0f)
  {
  }

  Color::Color (unsigned int c)
  {
    _red   = (float) (NUX_RGBA_GET_RED (c) ) * (1.f / 255.f);
    _green = (float) (NUX_RGBA_GET_GREEN (c) ) * (1.f / 255.f);
    _blue  = (float) (NUX_RGBA_GET_BLUE (c) ) * (1.f / 255.f);
    _alpha = (float) (NUX_RGBA_GET_ALPHA (c) ) * (1.f / 255.f);
  }

  Color::Color(int r, int g, int b)
    : _red(r / 255.f)
    , _green(g / 255.f)
    , _blue(b / 255.f)
    , _alpha(1)
  {
  }

  Color::Color (float r, float g, float b)
  {
    _red = r;
    _green = g;
    _blue = b;
    _alpha = 1.0f;
  }

  Color::Color (float r, float g, float b, float a)
  {
    _red = r;
    _green = g;
    _blue = b;
    _alpha = a;
  }

  Color *Color::Clone() const
  {
    return new Color (*this);
  }

  float Color::GetRed() const
  {
    return _red;
  };

  float Color::GetGreen() const
  {
    return _green;
  };

  float Color::GetBlue() const
  {
    return _blue;
  };

  float Color::GetAlpha() const
  {
    return _alpha;
  };

  float Color::R() const
  {
    return _red;
  };

  float Color::G() const
  {
    return _green;
  };

  float Color::B() const
  {
    return _blue;
  };

  float Color::A() const
  {
    return _alpha;
  };

  void Color::SetRed (float r)
  {
    _red = r;
  };
  void Color::SetGreen (float g)
  {
    _green = g;
  };
  void Color::SetBlue (float b)
  {
    _blue = b;
  };
  void Color::SetAlpha (float a)
  {
    _alpha = a;
  };

  bool Color::operator == (const Color &color) const
  {
    if ( (_red == color.R() )
         && (_green == color.G() )
         && (_blue == color.B() )
         && (_alpha == color.A() )
       )
    {
      return true;
    }

    return false;
  }

  bool Color::operator!= (const Color &color) const
  {
    if ( (_red == color.R() )
         && (_green == color.G() )
         && (_blue == color.B() )
         && (_alpha == color.A() )
       )
    {
      return false;
    }

    return true;
  }

  void Color::SetRGB (float r, float g, float b)
  {
    _red   = r;
    _green = g;
    _blue  = b;
    _alpha = 1.0f;
  }

  void Color::SetRGBA (float r, float g, float b, float a)
  {
    _red   = r;
    _green = g;
    _blue  = b;
    _alpha = a;
  }

  /*void Color::set_rgba(t_uint32 val)
  {
    _red   = (float)RGBA_GET_RED(   val )*(1.f/255.f);
    _green = (float)RGBA_GET_GREEN( val )*(1.f/255.f);
    _blue  = (float)RGBA_GET_BLUE(  val )*(1.f/255.f);
    _alpha = (float)RGBA_GET_ALPHA( val )*(1.f/255.f);
  }*/

  void Color::ClampVal()
  {
    _red = Max<float> (0.f, Min (1.f, _red) );
    _green = Max<float> (0.f, Min (1.f, _green) );
    _blue  = Max<float> (0.f, Min (1.f, _blue) );
    _alpha = Max<float> (0.f, Min (1.f, _alpha) );
  }

  void Color::Saturate()
  {
    _red   = Min<float> (1.f, _red);
    _green = Min<float> (1.f, _green);
    _blue  = Min<float> (1.f, _blue);
    _alpha = Min<float> (1.f, _alpha);
  }

  void Color::Complement()
  {
    _red   = 1.0f - _red;
    _green = 1.0f - _green;
    _blue  = 1.0f - _blue;
    _alpha = 1.0f - _alpha;
  }

  Color Color::Luminance()
  {
    float L = 0.30 * _red + 0.59 * _green + 0.11 * _blue;
    return Color (L, L, L, 1.0f);
  }

  Color Color::OneMinusLuminance()
  {
    float L = 1.0f - (0.30 * _red + 0.59 * _green + 0.11 * _blue);
    return Color (L, L, L, 1.0f);
  }

  Color Color::RandomColor()
  {
    return Color (RandomUInt (255) / 255.0f, RandomUInt (255) / 255.0f, RandomUInt (255) / 255.0f, RandomUInt (255) / 255.0f);
  }

  unsigned int Color::RandomColorINT()
  {
    return (RandomUInt (255) << 24) | (RandomUInt (255) << 16) | (RandomUInt (255) << 8) | RandomUInt (255);
  }

  Color operator + (Color color0, Color color1)
  {
    Color result;
    result.SetRed   (color0.R() + color1.R() );
    result.SetGreen (color0.G() + color1.G() );
    result.SetBlue  (color0.B() + color1.B() );
    result.SetAlpha (color0.A() + color1.A() );
    return result;
  }

  Color operator - (Color color0, Color color1)
  {
    Color result;
    result.SetRed   (color0.R() - color1.R() );
    result.SetGreen (color0.G() - color1.G() );
    result.SetBlue  (color0.B() - color1.B() );
    result.SetRed   (color0.A() - color1.A() );
    return result;
  }

  Color operator + (float v, Color color)
  {
    Color result;
    result.SetRed   (v + color.R() );
    result.SetGreen (v + color.G() );
    result.SetBlue  (v + color.B() );
    result.SetAlpha (v + color.A() );
    return result;
  }

  Color operator + (Color color, float v)
  {
    return v + color;
  }

  Color operator - (float v, Color color)
  {
    Color result;
    result.SetRed   (v - color.R() );
    result.SetGreen (v - color.G() );
    result.SetBlue  (v - color.B() );
    result.SetAlpha (v - color.A() );
    return result;
  }

  Color operator - (Color color, float v)
  {
    Color result;
    result.SetRed   (color.R() - v);
    result.SetGreen (color.G() - v);
    result.SetBlue  (color.B() - v);
    result.SetAlpha (color.A() - v);
    return result;
  }

  Color operator * (float v, Color color)
  {
    Color result;
    result.SetRed   (v * color.R() );
    result.SetGreen (v * color.G() );
    result.SetBlue  (v * color.B() );
    result.SetAlpha (v * color.A() );
    return result;
  }

  Color operator * (Color color, float v)
  {
    return v * color;
  }


// The Hue/Saturation/Value model was created by A. R. Smith in 1978. It is based on such intuitive color characteristics as tint,
// shade and tone (or family, purety and intensity). The coordinate system is cylindrical, and the colors are defined inside a hexcone.
// The hue value H runs from 0 to 360º. The saturation S is the degree of strength or purity and is from 0 to 1. Purity is how much white
// is added to the color, so S=1 makes the purest color (no white). Brightness V also ranges from 0 to 1, where 0 is the black.
// There is no transformation matrix for RGB/HSV conversion, but the algorithm follows:

// r,g,b values are from 0 to 1
// h = [0,360], s = [0,1], v = [0,1]
//		if s == 0, then h = -1 (undefined)

  void RGBtoHSV ( float r, float g, float b, float &h, float &s, float &v )
  {
    float min, max, delta;

    min = Min<float> ( Min<float> (r, g), b );
    max = Max<float> ( Max<float> (r, g), b );
    v = max;				// v

    delta = max - min;


    if ( max != 0 )
    {
      s = delta / max;		// s
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

    if ( r == max )
      h = ( g - b ) / delta;		// between yellow & magenta
    else if ( g == max )
      h = 2 + ( b - r ) / delta;	// between cyan & yellow
    else
      h = 4 + ( r - g ) / delta;	// between magenta & cyan

    h *= 60;				// degrees

    if ( h < 0 )
      h += 360;

    // convert h from [0, 360] to [0, 1]
    h = (h) / 360.0f;

  }

  void HSVtoRGB ( float &r, float &g, float &b, float h, float s, float v )
  {
    int i;
    float f, p, q, t;

    // convert h from [0, 1] to [0, 360]
    h = h * 360.0f;

    if ( s == 0 )
    {
      // achromatic (grey)
      r = g = b = v;
      return;
    }

    h /= 60;			// sector 0 to 5
    i = (int) std::floor ( h );
    f = h - i;			// factorial part of h
    p = v * ( 1 - s );
    q = v * ( 1 - s * f );
    t = v * ( 1 - s * ( 1 - f ) );

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
      default:		// case 5:
        r = v;
        g = p;
        b = q;
        break;
    }
  }

/////////////////////////
// HLS-RGB Conversions //
/////////////////////////

  float HLStoRGB1 (float rn1, float rn2, float huei)
  {
    // Static method. Auxiliary to HLS2RGB().

    float hue = huei;

    if (hue > 360) hue = hue - 360;

    if (hue < 0)   hue = hue + 360;

    if (hue < 60 ) return rn1 + (rn2 - rn1) * hue / 60;

    if (hue < 180) return rn2;

    if (hue < 240) return rn1 + (rn2 - rn1) * (240 - hue) / 60;

    return rn1;
  }

  void HLStoRGB (float &r, float &g, float &b, float hue, float light, float satur)
  {
    // Static method to compute RGB from HLS. The l and s are between [0,1]
    // and h is between [0, 1]. The returned r,g,b triplet is between [0,1].
    hue *= 360.0f;

    float rh, rl, rs, rm1, rm2;
    rh = rl = rs = 0;

    if (hue   > 0) rh = hue;

    if (rh > 360) rh = 360;

    if (light > 0) rl = light;

    if (rl > 1)   rl = 1;

    if (satur > 0) rs = satur;

    if (rs > 1)   rs = 1;

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

    r = HLStoRGB1 (rm1, rm2, rh + 120);
    g = HLStoRGB1 (rm1, rm2, rh);
    b = HLStoRGB1 (rm1, rm2, rh - 120);
  }

  void HLStoRGBi (int h, int l, int s, int &r, int &g, int &b)
  {
    // Static method to compute RGB from HLS. The h,l,s are between [0,255].
    // The returned r,g,b triplet is between [0,255].

    float hh, ll, ss;
    float rr, gg, bb;
    hh = ll = ss = 0;
    rr = gg = bb = 0;

    hh = float (h) * 360 / 255;
    ll = float (l) / 255;
    ss = float (s) / 255;

    HLStoRGB (hh, ll, ss, rr, gg, bb);

    r = (int) (rr * 255);
    g = (int) (gg * 255);
    b = (int) (bb * 255);
  }

  void RGBtoHLS (float rr, float gg, float bb,
                 float &hue, float &light, float &satur)
  {
    // Static method to compute HLS from RGB. The r,g,b triplet is between
    // [0,1], hue is between [0,1], light and satur are [0,1].

    float rnorm, gnorm, bnorm, minval, maxval, msum, mdiff, r, g, b;
    r = g = b = 0;

    if (rr > 0) r = rr;

    if (r > 1) r = 1;

    if (gg > 0) g = gg;

    if (g > 1) g = 1;

    if (bb > 0) b = bb;

    if (b > 1) b = 1;

    minval = r;

    if (g < minval) minval = g;

    if (b < minval) minval = b;

    maxval = r;

    if (g > maxval) maxval = g;

    if (b > maxval) maxval = b;

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


  void RGBtoHLSi (int r, int g, int b, int &h, int &l, int &s)
  {
    // Static method to compute HLS from RGB. The r,g,b triplet is between
    // [0,255], hue, light and satur are between [0,255].

    float rr, gg, bb, hue, light, satur;

    rr = float (r) / 255;
    gg = float (g) / 255;
    bb = float (b) / 255;

    RGBtoHLS (rr, gg, bb, hue, light, satur);

    h = (int) (hue / 360 * 255);
    l = (int) (light * 255);
    s = (int) (satur * 255);
  }

}
