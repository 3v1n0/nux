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

#define NUX_COLOR_RGB(r, g, b)

#define NUX_COLOR_ARGB(a,r,g,b)   ((Color)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

  // DirectX D3DFormat
  //    All formats are listed from left to right, most significant bit (MSB) to least significant bit (LSB). For example,
  //    D3DFORMAT_ARGB is ordered from the MSB channel A (alpha), to the LSB channel B (blue). When traversing surface data,
  //    the data is stored in memory from LSB to MSB, which means that the channel order in memory is from LSB (blue) to MSB (alpha).
  //
  //    The default value for formats that contain undefined channels (G16R16, A8, and so on) is 1. The only exception is the
  //    A8 format, which is initialized to 000 for the three color channels.
  //
  //    The order of the bits is from the most significant byte first, so D3DFMT_A8L8 indicates that the high byte of this 2-byte
  //    format is alpha. D3DFMT_D16 indicates a 16-bit integer value and an application-lockable surface.
  //
  //    Pixel formats have been chosen to enable the expression of hardware-vendor-defined extension formats, as well as to include
  //    the well-established four-character code (FOURCC) method. The set of formats understood by the Microsoft Direct3D runtime
  //    is defined by D3DFORMAT.
  //
  //


  //Format of RGBA colors is
  //7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0 7 6 5 4 3 2 1 0
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //|    alpha        |      red    |     green      |     blue     |
  //+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  //MSB 31                                                             0 LSB


#define NUX_RGBA_GET_ALPHA(rgba)      ((rgba) >> 24)
#define NUX_RGBA_GET_RED(rgba)        (((rgba) >> 16) & 0xff)
#define NUX_RGBA_GET_GREEN(rgba)      (((rgba) >> 8) & 0xff)
#define NUX_RGBA_GET_BLUE(rgba)       ((rgba) & 0xff)
#define NUX_RGBA(r, g, b, a)          ((a << 24) | (r << 16) | (g << 8) | b)
#define NUX_RGB(r, g, b)              ((r << 16) | (g << 8) | b)




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


  enum eColorModel {CM_RGB, CM_HSV, CM_HLS, CM_YUV};
  enum eColorChannel {CC_RED, CC_GREEN, CC_BLUE, CC_HUE, CC_SATURATION, CC_LIGHT , CC_VALUE};

  class Color
  {
  public:
    enum Format
    {
      COLORFORMAT_FLOAT = 0,
      COLORFORMAT_HEX,
      COLORFORMAT_INT,
    };

    Color();
    explicit Color (unsigned int c);
    Color(int r, int g, int b);
    Color(float r, float g, float b);
    Color(float r, float g, float b, float a);
    Color *Clone() const;

    bool operator == (const Color &) const;
    bool operator != (const Color &) const;

    void SetRGB (float r, float g, float b);
    void SetRGBA (float r, float g, float b, float a);

    void ClampVal();
    void Saturate();
    void Complement();
    Color Luminance();
    Color OneMinusLuminance();

    float GetRed() const;
    float GetGreen() const;
    float GetBlue() const;
    float GetAlpha() const;
    float R() const;
    float G() const;
    float B() const;
    float A() const;
    void SetRed (float r);
    void SetGreen (float g);
    void SetBlue (float b);
    void SetAlpha (float a);


    static Color RandomColor();
    static unsigned int RandomColorINT();

    friend Color operator + (Color color0, Color color1);
    friend Color operator - (Color color0, Color color1);
    friend Color operator + (float, Color color);
    friend Color operator + (Color color, float);
    friend Color operator - (float, Color color);
    friend Color operator - (Color color, float);
    friend Color operator * (float, Color color);
    friend Color operator * (Color color, float);

  private:
    float _red;
    float _green;
    float _blue;
    float _alpha;
  };

  void RGBtoHSV ( float r, float g, float b, float &h, float &s, float &v );
  void HSVtoRGB ( float &r, float &g, float &b, float h, float s, float v );
  void HLStoRGB (float &r, float &g, float &b, float hue, float light, float satur);
  void RGBtoHLS (float rr, float gg, float bb, float &hue, float &light, float &satur);

}

#endif // COLOR_H
