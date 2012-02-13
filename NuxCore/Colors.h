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

#ifndef COLORS_H
#define COLORS_H

#include "Color.h"

namespace nux
{
  namespace color
  {
    // Definition of Luma coefficients as per ITU-R Recommendation BT.709
    // http://en.wikipedia.org/wiki/Rec._709

    extern const float LumaRed;
    extern const float LumaGreen;
    extern const float LumaBlue;

    // X11 color names from:http://en.wikipedia.org/wiki/Web_colors
    extern const Color Transparent;

    // Red colors
    extern const Color IndianRed;
    extern const Color LightCoral;
    extern const Color Salmon;
    extern const Color DarkSalmon;
    extern const Color LightSalmon;
    extern const Color Crimson;
    extern const Color Red;
    extern const Color FireBrick;
    extern const Color DarkRed;

    // Pink colors
    extern const Color Pink;
    extern const Color LightPink;
    extern const Color HotPink;
    extern const Color DeepPink;
    extern const Color MediumVioletRed;
    extern const Color PaleVioletRed;

    // Orange colors
    extern const Color Coral;
    extern const Color Tomato;
    extern const Color OrangeRed;
    extern const Color DarkOrange;
    extern const Color Orange;

    // Yellow colors
    extern const Color Gold;
    extern const Color Yellow;
    extern const Color LightYellow;
    extern const Color LemonChiffon;
    extern const Color LightGoldenrodYellow;
    extern const Color PapayaWhip;
    extern const Color Moccasin;
    extern const Color PeachPuff;
    extern const Color PaleGoldenrod;
    extern const Color Khaki;
    extern const Color DarkKhaki;

    // Purple colors
    extern const Color Lavender;
    extern const Color Thistle;
    extern const Color Plum;
    extern const Color Violet;
    extern const Color Orchid;
    extern const Color Fuchsia;
    extern const Color Magenta;
    extern const Color MediumOrchid;
    extern const Color MediumPurple;
    extern const Color BlueViolet;
    extern const Color DarkViolet;
    extern const Color DarkOrchid;
    extern const Color DarkMagenta;
    extern const Color Purple;
    extern const Color Indigo;
    extern const Color SlateBlue;
    extern const Color DarkSlateBlue;

    // Green colors
    extern const Color GreenYellow;
    extern const Color Chartreuse;
    extern const Color LawnGreen;
    extern const Color Lime;
    extern const Color LimeGreen;
    extern const Color PaleGreen;
    extern const Color LightGreen;
    extern const Color MediumSpringGreen;
    extern const Color SpringGreen;
    extern const Color MediumSeaGreen;
    extern const Color SeaGreen;
    extern const Color ForestGreen;
    extern const Color Green;
    extern const Color DarkGreen;
    extern const Color YellowGreen;
    extern const Color OliveDrab;
    extern const Color Olive;
    extern const Color DarkOliveGreen;
    extern const Color MediumAquamarine;
    extern const Color DarkSeaGreen;
    extern const Color LightSeaGreen;
    extern const Color DarkCyan;
    extern const Color Teal;

    // Blue colors
    extern const Color Aqua;
    extern const Color Cyan;
    extern const Color LightCyan;
    extern const Color PaleTurquoise;
    extern const Color Aquamarine;
    extern const Color Turquoise;
    extern const Color MediumTurquoise;
    extern const Color DarkTurquoise;
    extern const Color CadetBlue;
    extern const Color SteelBlue;
    extern const Color LightSteelBlue;
    extern const Color PowderBlue;
    extern const Color LightBlue;
    extern const Color SkyBlue;
    extern const Color LightSkyBlue;
    extern const Color DeepSkyBlue;
    extern const Color DodgerBlue;
    extern const Color CornflowerBlue;
    extern const Color MediumSlateBlue;
    extern const Color RoyalBlue;
    extern const Color Blue;
    extern const Color MediumBlue;
    extern const Color DarkBlue;
    extern const Color Navy;
    extern const Color MidnightBlue;

    // Brown colors
    extern const Color Cornsilk;
    extern const Color BlanchedAlmond;
    extern const Color Bisque;
    extern const Color NavajoWhite;
    extern const Color Wheat;
    extern const Color BurlyWood;
    extern const Color Tan;
    extern const Color RosyBrown;
    extern const Color SandyBrown;
    extern const Color Goldenrod;
    extern const Color DarkGoldenrod;
    extern const Color Peru;
    extern const Color Chocolate;
    extern const Color SaddleBrown;
    extern const Color Sienna;
    extern const Color Brown;
    extern const Color Maroon;

    // White colors
    extern const Color      White;
    extern const Color Snow;
    extern const Color Honeydew;
    extern const Color MintCream;
    extern const Color Azure;
    extern const Color AliceBlue;
    extern const Color GhostWhite;
    extern const Color WhiteSmoke;
    extern const Color Seashell;
    extern const Color Beige;
    extern const Color OldLace;
    extern const Color FloralWhite;
    extern const Color Ivory;
    extern const Color AntiqueWhite;
    extern const Color Linen;
    extern const Color LavenderBlush;
    extern const Color MistyRose;

    // Grey colors
    extern const Color Gainsboro;
    extern const Color LightGrey;
    extern const Color Silver;
    extern const Color DarkGray;
    extern const Color Gray;
    extern const Color DimGray;
    extern const Color LightSlateGray;
    extern const Color SlateGray;
    extern const Color DarkSlateGray;
    extern const Color Black;

    // More Colors
    extern const Color Aubergine;
  }
}

#endif // COLORS_H

