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


#ifndef COLOR_H
#define COLOR_H

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

enum Model {
    RGB,
    HSV,
    HLS,
    YUV
};

enum Channel {
    RED,
    GREEN,
    BLUE,
    HUE,
    SATURATION,
    LIGHT,
    VALUE
};

enum Format {
    FLOAT,
    HEX,
    INT
};

class Color
{
public:
    Color();
    explicit Color (unsigned int c);
    Color(int r, int g, int b);
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);

    float red;
    float green;
    float blue;
    float alpha;
};

bool operator == (Color const& lhs, Color const& rhs);
bool operator != (Color const& lhs, Color const& rhs);

Color ClampVal(Color const&);
Color Saturate(Color const&);
Color Complement(Color const&);
Color Luminance(Color const&);
Color OneMinusLuminance(Color const&);

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

struct HSV;
struct HLS;

struct RGB
{
  RGB(Color const&);
  RGB(HSV const&);
  RGB(HLS const&);

  float red;
  float green;
  float blue;
};

struct HSV
{
  HSV(Color const&);
  HSV(RGB const&);
  HSV(HLS const&);

  float hue;
  float saturation;
  float value;
};
struct HLS
{
  HLS(Color const&);
  HLS(RGB const&);
  HLS(HSV const&);

  float hue;
  float lightness;
  float saturation;
};

  void RGBtoHSV ( float r, float g, float b, float &h, float &s, float &v );
  void HSVtoRGB ( float &r, float &g, float &b, float h, float s, float v );
  void HLStoRGB (float &r, float &g, float &b, float hue, float light, float satur);
  void RGBtoHLS (float rr, float gg, float bb, float &hue, float &light, float &satur);

}
  using color::Color;
}

#endif // COLOR_H
