/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef COLOR_H
#define COLOR_H

#include "NSystemTypes.h"

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
    ~Color();
    Color (const Color &);
    explicit Color (unsigned int c);
    Color (t_float r, t_float g, t_float b);
    Color (t_float r, t_float g, t_float b, t_float a);
    Color *Clone() const;

    Color &operator= (const Color &);
    bool operator== (const Color &) const;
    bool operator!= (const Color &) const;

    void SetRGB (t_float r, t_float g, t_float b);
    void SetRGBA (t_float r, t_float g, t_float b, t_float a);

    void ClampVal();
    void Saturate();
    void Complement();
    Color Luminance();
    Color OneMinusLuminance();

    t_float GetRed() const
    {
      return red_;
    };
    t_float GetGreen() const
    {
      return green_;
    };
    t_float GetBlue() const
    {
      return blue_;
    };
    t_float GetAlpha() const
    {
      return alpha_;
    };

    t_float R() const
    {
      return red_;
    };
    t_float G() const
    {
      return green_;
    };
    t_float B() const
    {
      return blue_;
    };
    t_float A() const
    {
      return alpha_;
    };

    void SetRed (t_float r)
    {
      red_ = r;
    };
    void SetGreen (t_float g)
    {
      green_ = g;
    };
    void SetBlue (t_float b)
    {
      blue_ = b;
    };
    void SetAlpha (t_float a)
    {
      alpha_ = a;
    };

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
    t_float red_;
    t_float green_;
    t_float blue_;
    t_float alpha_;

  public:
//X11 color names from:http://en.wikipedia.org/wiki/Web_colors

    //Red colors
    static const Color      IndianRed           ;//     CD 5C 5C 	205 92 92
    static const Color      LightCoral          ;//        F0 80 80 	240 128 128
    static const Color      Salmon              ;//        FA 80 72 	250 128 114
    static const Color      DarkSalmon          ;//        E9 96 7A 	233 150 122
    static const Color      LightSalmon         ;//        FF A0 7A 	255 160 122
    static const Color      Crimson             ;//        DC 14 3C 	220 20 60
    static const Color      Red                 ;//        FF 00 00 	255 0 0
    static const Color      FireBrick           ;//        B2 22 22 	178 34 34
    static const Color      DarkRed             ;//        8B 00 00 	139 0 0

    //Pink colors
    static const Color      Pink                ;//        FF C0 CB 	255 192 203
    static const Color      LightPink           ;//        FF B6 C1 	255 182 193
    static const Color      HotPink             ;//        FF 69 B4 	255 105 180
    static const Color      DeepPink            ;//        FF 14 93 	255 20 147
    static const Color      MediumVioletRed     ;//        C7 15 85 	199 21 133
    static const Color      PaleVioletRed       ;//        DB 70 93 	219 112 147

    //Orange colors                             ;//
    //static const Color      LightSalmon         ;//        FF A0 7A 	255 160 122
    static const Color      Coral               ;//        FF 7F 50 	255 127 80
    static const Color      Tomato              ;//        FF 63 47 	255 99 71
    static const Color      OrangeRed           ;//        FF 45 00 	255 69 0
    static const Color      DarkOrange          ;//        FF 8C 00 	255 140 0
    static const Color      Orange              ;//        FF A5 00 	255 165 0

    //Yellow colors                             ;//
    static const Color      Gold                ;//        FF D7 00 	255 215 0
    static const Color      Yellow              ;//        FF FF 00 	255 255 0
    static const Color      LightYellow         ;//        FF FF E0 	255 255 224
    static const Color      LemonChiffon        ;//        FF FA CD 	255 250 205
    static const Color      LightGoldenrodYellow;// 	    FA FA D2 	250 250 210
    static const Color      PapayaWhip          ;//        FF EF D5 	255 239 213
    static const Color      Moccasin            ;//        FF E4 B5 	255 228 181
    static const Color      PeachPuff           ;//        FF DA B9 	255 218 185
    static const Color      PaleGoldenrod       ;//        EE E8 AA 	238 232 170
    static const Color      Khaki               ;//        F0 E6 8C 	240 230 140
    static const Color      DarkKhaki           ;//        BD B7 6B 	189 183 107

    //Purple colors
    static const Color      Lavender            ;//        E6 E6 FA 	230 230 250
    static const Color      Thistle             ;//        D8 BF D8 	216 191 216
    static const Color      Plum                ;//        DD A0 DD 	221 160 221
    static const Color      Violet              ;//        EE 82 EE 	238 130 238
    static const Color      Orchid              ;//        DA 70 D6 	218 112 214
    static const Color      Fuchsia             ;//        FF 00 FF 	255 0 255
    static const Color      Magenta             ;//        FF 00 FF 	255 0 255
    static const Color      MediumOrchid        ;//        BA 55 D3 	186 85 211
    static const Color      MediumPurple        ;//        93 70 DB 	147 112 219
    static const Color      BlueViolet          ;//        8A 2B E2 	138 43 226
    static const Color      DarkViolet          ;//        94 00 D3 	148 0 211
    static const Color      DarkOrchid          ;//        99 32 CC 	153 50 204
    static const Color      DarkMagenta         ;//        8B 00 8B 	139 0 139
    static const Color      Purple              ;//        80 00 80 	128 0 128
    static const Color      Indigo              ;//        4B 00 82 	75 0 130
    static const Color      SlateBlue           ;//        6A 5A CD 	106 90 205
    static const Color      DarkSlateBlue       ;//        48 3D 8B 	72 61 139

    //Green colors
    static const Color      GreenYellow         ;//        AD FF 2F 	173 255 47
    static const Color      Chartreuse          ;//        7F FF 00 	127 255 0
    static const Color      LawnGreen           ;//        7C FC 00 	124 252 0
    static const Color      Lime                ;//        00 FF 00 	0 255 0
    static const Color      LimeGreen           ;//        32 CD 32 	50 205 50
    static const Color      PaleGreen           ;//        98 FB 98 	152 251 152
    static const Color      LightGreen          ;//        90 EE 90 	144 238 144
    static const Color      MediumSpringGreen   ;//        00 FA 9A 	0 250 154
    static const Color      SpringGreen         ;//        00 FF 7F 	0 255 127
    static const Color      MediumSeaGreen      ;//        3C B3 71 	60 179 113
    static const Color      SeaGreen            ;//        2E 8B 57 	46 139 87
    static const Color      ForestGreen         ;//        22 8B 22 	34 139 34
    static const Color      Green               ;//        00 80 00 	0 128 0
    static const Color      DarkGreen           ;//        00 64 00 	0 100 0
    static const Color      YellowGreen         ;//        9A CD 32 	154 205 50
    static const Color      OliveDrab           ;//        6B 8E 23 	107 142 35
    static const Color      Olive               ;//        80 80 00 	128 128 0
    static const Color      DarkOliveGreen      ;//        55 6B 2F 	85 107 47
    static const Color      MediumAquamarine    ;//        66 CD AA 	102 205 170
    static const Color      DarkSeaGreen        ;//        8F BC 8F 	143 188 143
    static const Color      LightSeaGreen       ;//        20 B2 AA 	32 178 170
    static const Color      DarkCyan            ;//        00 8B 8B 	0 139 139
    static const Color      Teal                ;//        00 80 80 	0 128 128

    //Blue colors                               ;//
    static const Color      Aqua                ;//        00 FF FF 	0 255 255
    static const Color      Cyan                ;//        00 FF FF 	0 255 255
    static const Color      LightCyan           ;//        E0 FF FF 	224 255 255
    static const Color      PaleTurquoise       ;//        AF EE EE 	175 238 238
    static const Color      Aquamarine          ;//        7F FF D4 	127 255 212
    static const Color      Turquoise           ;//        40 E0 D0 	64 224 208
    static const Color      MediumTurquoise     ;//        48 D1 CC 	72 209 204
    static const Color      DarkTurquoise       ;//        00 CE D1 	0 206 209
    static const Color      CadetBlue           ;//        5F 9E A0 	95 158 160
    static const Color      SteelBlue           ;//        46 82 B4 	70 130 180
    static const Color      LightSteelBlue      ;//        B0 C4 DE 	176 196 222
    static const Color      PowderBlue          ;//        B0 E0 E6 	176 224 230
    static const Color      LightBlue           ;//        AD D8 E6 	173 216 230
    static const Color      SkyBlue             ;//        87 CE EB 	135 206 235
    static const Color      LightSkyBlue        ;//        87 CE FA 	135 206 250
    static const Color      DeepSkyBlue         ;//        00 BF FF 	0 191 255
    static const Color      DodgerBlue          ;//        1E 90 FF 	30 144 255
    static const Color      CornflowerBlue      ;//        64 95 ED 	100 149 237
    static const Color      MediumSlateBlue     ;//        7B 68 EE 	123 104 238
    static const Color      RoyalBlue           ;//        41 69 E1 	65 105 225
    static const Color      Blue                ;//        00 00 FF 	0 0 255
    static const Color      MediumBlue          ;//        00 00 CD 	0 0 205
    static const Color      DarkBlue            ;//        00 00 8B 	0 0 139
    static const Color      Navy                ;//        00 00 80 	0 0 128
    static const Color      MidnightBlue        ;//        19 19 70 	25 25 112

    //Brown colors                              ;//
    static const Color      Cornsilk            ;//        FF F8 DC 	255 248 220
    static const Color      BlanchedAlmond      ;//        FF EB CD 	255 235 205
    static const Color      Bisque              ;//        FF E4 C4 	255 228 196
    static const Color      NavajoWhite         ;//        FF DE AD 	255 222 173
    static const Color      Wheat               ;//        F5 DE B3 	245 222 179
    static const Color      BurlyWood           ;//        DE B8 87 	222 184 135
    static const Color      Tan                 ;//        D2 B4 8C 	210 180 140
    static const Color      RosyBrown           ;//        BC 8F 8F 	188 143 143
    static const Color      SandyBrown          ;//        F4 A4 60 	244 164 96
    static const Color      Goldenrod           ;//        DA A5 20 	218 165 32
    static const Color      DarkGoldenrod       ;//        B8 86 0B 	184 134 11
    static const Color      Peru                ;//        CD 85 3F 	205 133 63
    static const Color      Chocolate           ;//        D2 69 1E 	210 105 30
    static const Color      SaddleBrown         ;//        8B 45 13 	139 69 19
    static const Color      Sienna              ;//        A0 52 2D 	160 82 45
    static const Color      Brown               ;//        A5 2A 2A 	165 42 42
    static const Color      Maroon              ;//        80 00 00 	128 0 0

    //White colors                              ;//
    static const Color      White               ;//        FF FF FF 	255 255 255
    static const Color      Snow                ;//        FF FA FA 	255 250 250
    static const Color      Honeydew            ;//        F0 FF F0 	240 255 240
    static const Color      MintCream           ;//        F5 FF FA 	245 255 250
    static const Color      Azure               ;//        F0 FF FF 	240 255 255
    static const Color      AliceBlue           ;//        F0 F8 FF 	240 248 255
    static const Color      GhostWhite          ;//        F8 F8 FF 	248 248 255
    static const Color      WhiteSmoke          ;//        F5 F5 F5 	245 245 245
    static const Color      Seashell            ;//        FF F5 EE 	255 245 238
    static const Color      Beige               ;//        F5 F5 DC 	245 245 220
    static const Color      OldLace             ;//        FD F5 E6 	253 245 230
    static const Color      FloralWhite         ;//        FF FA F0 	255 250 240
    static const Color      Ivory               ;//        FF FF F0 	255 255 240
    static const Color      AntiqueWhite        ;//        FA EB D7 	250 235 215
    static const Color      Linen               ;//        FA F0 E6 	250 240 230
    static const Color      LavenderBlush       ;//        FF F0 F5 	255 240 245
    static const Color      MistyRose           ;//        FF E4 E1 	255 228 225

    //Grey colors                               ;//
    static const Color      Gainsboro           ;//        DC DC DC 	220 220 220
    static const Color      LightGrey           ;//        D3 D3 D3 	211 211 211
    static const Color      Silver              ;//        C0 C0 C0 	192 192 192
    static const Color      DarkGray            ;//        A9 A9 A9 	169 169 169
    static const Color      Gray                ;//        80 80 80 	128 128 128
    static const Color      DimGray             ;//        69 69 69 	105 105 105
    static const Color      LightSlateGray      ;//        77 88 99 	119 136 153
    static const Color      SlateGray           ;//        70 80 90 	112 128 144
    static const Color      DarkSlateGray       ;//        2F 4F 4F 	47 79 79
    static const Color      Black               ;//        00 00 00 	0 0 0

    static Color    Dummy;
  };

  void RGBtoHSV ( float r, float g, float b, float &h, float &s, float &v );
  void HSVtoRGB ( float &r, float &g, float &b, float h, float s, float v );
  void HLStoRGB (float &r, float &g, float &b, float hue, float light, float satur);
  void RGBtoHLS (float rr, float gg, float bb, float &hue, float &light, float &satur);

}

#endif // COLOR_H

/*
  http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnopen/html/msdn_gl5.asp

  OpenGL V: Translating Windows DIBs
  February 8, 1995

  Abstract

  OpenGL (TM) is a portable language for rendering three-dimensional (3-D) graphics. OpenGL does not understand Microsoft? Windows? device-independent bitmaps (DIBs); instead, it has its own format for representing images. This article explains how to translate a Windows DIB into a format usable with OpenGL. Some knowledge of the Windows DIB format and the Microsoft Foundation Class Library (MFC) is expected. The EasyDIB sample application and GLlib dynamic-link library (DLL) demonstrate the ideas presented in this article.

*/



