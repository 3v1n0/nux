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


#ifndef COLOR_H
#define COLOR_H

#include <string>

namespace nux
{
namespace color
{
  // DirectX D3DFormat
  //
  // All formats are listed from left to right, most significant bit (MSB) to
  // least significant bit (LSB). For example, D3DFORMAT_ARGB is ordered from
  // the MSB channel A (alpha), to the LSB channel B (blue). When traversing
  // surface data, the data is stored in memory from LSB to MSB, which means
  // that the channel order in memory is from LSB (blue) to MSB (alpha).
  //
  // The default value for formats that contain undefined channels (G16R16,
  // A8, and so on) is 1. The only exception is the A8 format, which is
  // initialized to 000 for the three color channels.
  //
  // The order of the bits is from the most significant byte first, so
  // D3DFMT_A8L8 indicates that the high byte of this 2-byte format is
  // alpha. D3DFMT_D16 indicates a 16-bit integer value and an
  // application-lockable surface.
  //
  // Pixel formats have been chosen to enable the expression of
  // hardware-vendor-defined extension formats, as well as to include the
  // well-established four-character code (FOURCC) method. The set of formats
  // understood by the Microsoft Direct3D runtime is defined by D3DFORMAT.

  //Format of RGBA colors is
  //7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //|    alpha        |      red    |     green      |     blue     |
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //MSB 31                                                             0 LSB


  //Format of RGB colors is
  //7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //|   ignored     |      red      |     green     |     blue      |
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  //Format of BGR colors is
  //7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //|   ignored     |      blue     |     green     |      red      |
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  //Format of RGBA colors is
  //7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //|      red      |      green    |     blue      |     alpha     |
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  //Format of BGRA colors is
  //7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //|      blue      |      green    |     red      |     alpha     |
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  enum Model
  {
    RGB,
    HSV,
    HLS,
    YUV
  };

  enum Channel
  {
    RED,
    GREEN,
    BLUE,
    HUE,
    SATURATION,
    LIGHT,
    VALUE
  };

  enum Format
  {
    FLOAT,
    HEX,
    INT
  };

  class RedGreenBlue;

  class Color
  {
  public:
    Color();
    explicit Color(unsigned int c);
    Color(int r, int g, int b);
    Color(float r, float g, float b, float a = 1.0f);
    Color(RedGreenBlue const& rgb, float a = 1.0f);

    //! Constructor - Initializes the color using an hex string 
    /*!
        Initializes the color using an hex string, which may be in one of these formats:

        #RGB (each of R, G, B, A is a single hex digit)
        #RGBA
        #RRGGBB
        #RRGGBBAA

        The color is #ffffffff if hex cannot be parsed.
    */
    Color(std::string const& hex);
    
    //! Returns the pre-multiplied version of this color.
    /*!
        Returns the pre-multiplied version of this color. If this color is already pre-multiplied
        then *this is returned.\n
        The premultiplied color is Color(red * alpha, green * alpha, blue * alpha, alpha).

        @return The pre-multiplied version of this color.
    */
    Color GetPremultiplied();

    //! Sets a pre-multiplied color
    /*!
        Sets a pre-multiplied color

        @param r Red value.
        @param g Green value.
        @param b Blue value.
        @param a Alpha value.
    */
    void SetPremultiplied(float r, float g, float b, float a);

    //! Returns True if this color is pre-multiplied.
    /*!
        Returns True if this color is pre-multiplied.

        @return True is this color is pre-multiplied.
    */
    bool IsPremultiplied();

    float red;
    float green;
    float blue;
    float alpha;

  protected:
    bool premultiplied_; //!< True if the rgb components have been pre-multiplied with the alpha component.

    friend bool operator == (Color const& lhs, Color const& rhs);
  };

  bool operator == (Color const& lhs, Color const& rhs);
  bool operator != (Color const& lhs, Color const& rhs);

  Color operator + (Color const&, Color const&);
  Color operator + (float, Color const&);
  Color operator + (Color const&, float);

  Color operator - (Color const&, Color const&);
  Color operator - (float, Color const&);
  Color operator - (Color const&, float);

  Color operator * (float, Color const&);
  Color operator * (Color const&, float);

  Color RandomColor();
  unsigned int RandomColorINT();

  class HueSaturationValue;
  class HueLightnessSaturation;

  class RedGreenBlue
  {
  public:
    RedGreenBlue(float r, float g, float b);
    RedGreenBlue(Color const& color);
    RedGreenBlue(HueSaturationValue const&);
    RedGreenBlue(HueLightnessSaturation const&);

    float red;
    float green;
    float blue;
  };

  class HueSaturationValue
  {
  public:
    HueSaturationValue(float h, float s, float v);
    HueSaturationValue(Color const&);
    HueSaturationValue(RedGreenBlue const&);

    float hue;
    float saturation;
    float value;
  };

  class HueLightnessSaturation
  {
  public:
    HueLightnessSaturation(float h, float l, float s);
    HueLightnessSaturation(Color const&);
    HueLightnessSaturation(RedGreenBlue const&);

    float hue;
    float lightness;
    float saturation;
  };

}
using color::Color;
}

#endif // COLOR_H
