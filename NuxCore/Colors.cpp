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


#include "NuxCore.h"

namespace nux
{
  namespace Colors
  {
    //Red colors
    const Color IndianRed           = Color (0xCD / 255.0f, 0x5C / 255.0f, 0x5C / 255.0f); // 	205     92      92
    const Color LightCoral          = Color (0xF0 / 255.0f, 0x80 / 255.0f, 0x80 / 255.0f); // 	240     128     128
    const Color Salmon              = Color (0xFA / 255.0f, 0x80 / 255.0f, 0x72 / 255.0f); // 	250     128     114
    const Color DarkSalmon          = Color (0xE9 / 255.0f, 0x96 / 255.0f, 0x7A / 255.0f); // 	233     150     122
    const Color LightSalmon         = Color (0xFF / 255.0f, 0xA0 / 255.0f, 0x7A / 255.0f); // 	255     160     122
    const Color Crimson             = Color (0xDC / 255.0f, 0x14 / 255.0f, 0x3C / 255.0f); // 	220     20      60
    const Color Red                 = Color (0xFF / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f); // 	255     0       0
    const Color FireBrick           = Color (0xB2 / 255.0f, 0x22 / 255.0f, 0x22 / 255.0f); // 	178     34      34
    const Color DarkRed             = Color (0x8B / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f); // 	139     0       0

    //Pink colors
    const Color Pink                = Color (0xFF / 255.0f, 0xC0 / 255.0f, 0xCB / 255.0f); // 	255     192     203
    const Color LightPink           = Color (0xFF / 255.0f, 0xB6 / 255.0f, 0xC1 / 255.0f); // 	255     182     193
    const Color HotPink             = Color (0xFF / 255.0f, 0x69 / 255.0f, 0xB4 / 255.0f); // 	255     105     180
    const Color DeepPink            = Color (0xFF / 255.0f, 0x14 / 255.0f, 0x93 / 255.0f); // 	255     20      147
    const Color MediumVioletRed     = Color (0xC7 / 255.0f, 0x15 / 255.0f, 0x85 / 255.0f); // 	199     21      133
    const Color PaleVioletRed       = Color (0xDB / 255.0f, 0x70 / 255.0f, 0x93 / 255.0f); //    219     112     147

    //Orange colors
    //const Color LightSalmon       = Color (0xFF/255.0f, 0xA0/255.0f, 0x7A/255.0f);// 	255     160     122
    const Color Coral               = Color (0xFF / 255.0f, 0x7F / 255.0f, 0x50 / 255.0f); //    255     127     80
    const Color Tomato              = Color (0xFF / 255.0f, 0x63 / 255.0f, 0x47 / 255.0f); //    255     99      71
    const Color OrangeRed           = Color (0xFF / 255.0f, 0x45 / 255.0f, 0x00 / 255.0f); //    255     69      0
    const Color DarkOrange          = Color (0xFF / 255.0f, 0x8C / 255.0f, 0x00 / 255.0f); //    255     140     0
    const Color Orange              = Color (0xFF / 255.0f, 0xA5 / 255.0f, 0x00 / 255.0f); //    255     165     0

    //Yellow colors
    const Color Gold                = Color (0xFF / 255.0f, 0xD7 / 255.0f, 0x00 / 255.0f); // 	255     215     0
    const Color Yellow              = Color (0xFF / 255.0f, 0xFF / 255.0f, 0x00 / 255.0f); // 	255     255     0
    const Color LightYellow         = Color (0xFF / 255.0f, 0xFF / 255.0f, 0xE0 / 255.0f); // 	255     255     224
    const Color LemonChiffon        = Color (0xFF / 255.0f, 0xFA / 255.0f, 0xCD / 255.0f); // 	255     250     205
    const Color LightGoldenrodYellow = Color (0xFA / 255.0f, 0xFA / 255.0f, 0xD2 / 255.0f); // 	250     250     210
    const Color PapayaWhip          = Color (0xFF / 255.0f, 0xEF / 255.0f, 0xD5 / 255.0f); // 	255     239     213
    const Color Moccasin            = Color (0xFF / 255.0f, 0xE4 / 255.0f, 0xB5 / 255.0f); // 	255     228     181
    const Color PeachPuff           = Color (0xFF / 255.0f, 0xDA / 255.0f, 0xB9 / 255.0f); // 	255     218     185
    const Color PaleGoldenrod       = Color (0xEE / 255.0f, 0xE8 / 255.0f, 0xAA / 255.0f); // 	238     232     170
    const Color Khaki               = Color (0xF0 / 255.0f, 0xE6 / 255.0f, 0x8C / 255.0f); // 	240     230     140
    const Color DarkKhaki           = Color (0xBD / 255.0f, 0xB7 / 255.0f, 0x6B / 255.0f); // 	189     183     107

    //Purple colors
    const Color Lavender            = Color (0xE6 / 255.0f, 0xE6 / 255.0f, 0xFA / 255.0f); // 	230     230     250
    const Color Thistle             = Color (0xD8 / 255.0f, 0xBF / 255.0f, 0xD8 / 255.0f); // 	216     191     216
    const Color Plum                = Color (0xDD / 255.0f, 0xA0 / 255.0f, 0xDD / 255.0f); // 	221     160     221
    const Color Violet              = Color (0xEE / 255.0f, 0x82 / 255.0f, 0xEE / 255.0f); // 	238     130     238
    const Color Orchid              = Color (0xDA / 255.0f, 0x70 / 255.0f, 0xD6 / 255.0f); // 	218     112     214
    const Color Fuchsia             = Color (0xFF / 255.0f, 0x00 / 255.0f, 0xFF / 255.0f); // 	255     0       255
    const Color Magenta             = Color (0xFF / 255.0f, 0x00 / 255.0f, 0xFF / 255.0f); // 	255     0       255
    const Color MediumOrchid        = Color (0xBA / 255.0f, 0x55 / 255.0f, 0xD3 / 255.0f); // 	186     85      211
    const Color MediumPurple        = Color (0x93 / 255.0f, 0x70 / 255.0f, 0xDB / 255.0f); // 	147     112     219
    const Color BlueViolet          = Color (0x8A / 255.0f, 0x2B / 255.0f, 0xE2 / 255.0f); // 	138     43      226
    const Color DarkViolet          = Color (0x94 / 255.0f, 0x00 / 255.0f, 0xD3 / 255.0f); // 	148     0       211
    const Color DarkOrchid          = Color (0x99 / 255.0f, 0x32 / 255.0f, 0xCC / 255.0f); // 	153     50      204
    const Color DarkMagenta         = Color (0x8B / 255.0f, 0x00 / 255.0f, 0x8B / 255.0f); // 	139     0       139
    const Color Purple              = Color (0x80 / 255.0f, 0x00 / 255.0f, 0x80 / 255.0f); // 	128     0       128
    const Color Indigo              = Color (0x4B / 255.0f, 0x00 / 255.0f, 0x82 / 255.0f); // 	75      0       130
    const Color SlateBlue           = Color (0x6A / 255.0f, 0x5A / 255.0f, 0xCD / 255.0f); // 	106     90      205
    const Color DarkSlateBlue       = Color (0x48 / 255.0f, 0x3D / 255.0f, 0x8B / 255.0f); // 	72      61      139

    //Green colors
    const Color GreenYellow         = Color (0xAD / 255.0f, 0xFF / 255.0f, 0x2F / 255.0f); // 	173     255     47
    const Color Chartreuse          = Color (0x7F / 255.0f, 0xFF / 255.0f, 0x00 / 255.0f); // 	127     255     0
    const Color LawnGreen           = Color (0x7C / 255.0f, 0xFC / 255.0f, 0x00 / 255.0f); // 	124     252     0
    const Color Lime                = Color (0x00 / 255.0f, 0xFF / 255.0f, 0x00 / 255.0f); // 	0       255     0
    const Color LimeGreen           = Color (0x32 / 255.0f, 0xCD / 255.0f, 0x32 / 255.0f); // 	50      205     50
    const Color PaleGreen           = Color (0x98 / 255.0f, 0xFB / 255.0f, 0x98 / 255.0f); // 	152     251     152
    const Color LightGreen          = Color (0x90 / 255.0f, 0xEE / 255.0f, 0x90 / 255.0f); // 	144     238     144
    const Color MediumSpringGreen   = Color (0x00 / 255.0f, 0xFA / 255.0f, 0x9A / 255.0f); // 	0       250     154
    const Color SpringGreen         = Color (0x00 / 255.0f, 0xFF / 255.0f, 0x7F / 255.0f); // 	0       255     127
    const Color MediumSeaGreen      = Color (0x3C / 255.0f, 0xB3 / 255.0f, 0x71 / 255.0f); // 	60      179     113
    const Color SeaGreen            = Color (0x2E / 255.0f, 0x8B / 255.0f, 0x57 / 255.0f); // 	46      139     87
    const Color ForestGreen         = Color (0x22 / 255.0f, 0x8B / 255.0f, 0x22 / 255.0f); // 	34      139     34
    const Color Green               = Color (0x00 / 255.0f, 0x80 / 255.0f, 0x00 / 255.0f); // 	0       128     0
    const Color DarkGreen           = Color (0x00 / 255.0f, 0x64 / 255.0f, 0x00 / 255.0f); // 	0       100     0
    const Color YellowGreen         = Color (0x9A / 255.0f, 0xCD / 255.0f, 0x32 / 255.0f); // 	154     205     50
    const Color OliveDrab           = Color (0x6B / 255.0f, 0x8E / 255.0f, 0x23 / 255.0f); // 	107     142     35
    const Color Olive               = Color (0x80 / 255.0f, 0x80 / 255.0f, 0x00 / 255.0f); // 	128     128     0
    const Color DarkOliveGreen      = Color (0x55 / 255.0f, 0x6B / 255.0f, 0x2F / 255.0f); // 	85      107     47
    const Color MediumAquamarine    = Color (0x66 / 255.0f, 0xCD / 255.0f, 0xAA / 255.0f); // 	102     205     170
    const Color DarkSeaGreen        = Color (0x8F / 255.0f, 0xBC / 255.0f, 0x8F / 255.0f); // 	143     188     143     // in .Net this is = Color (0x8F, 0xBC, 0x8B)
    const Color LightSeaGreen       = Color (0x20 / 255.0f, 0xB2 / 255.0f, 0xAA / 255.0f); // 	32      178     170
    const Color DarkCyan            = Color (0x00 / 255.0f, 0x8B / 255.0f, 0x8B / 255.0f); // 	0       139     139
    const Color Teal                = Color (0x00 / 255.0f, 0x80 / 255.0f, 0x80 / 255.0f); // 	0       128     128

    //Blue colors
    const Color Aqua                = Color (0x00 / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f); // 	0       255     255
    const Color Cyan                = Color (0x00 / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f); // 	0       255     255
    const Color LightCyan           = Color (0xE0 / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f); // 	224     255     255
    const Color PaleTurquoise       = Color (0xAF / 255.0f, 0xEE / 255.0f, 0xEE / 255.0f); // 	175     238     238
    const Color Aquamarine          = Color (0x7F / 255.0f, 0xFF / 255.0f, 0xD4 / 255.0f); // 	127     255     212
    const Color Turquoise           = Color (0x40 / 255.0f, 0xE0 / 255.0f, 0xD0 / 255.0f); // 	64      224     208
    const Color MediumTurquoise     = Color (0x48 / 255.0f, 0xD1 / 255.0f, 0xCC / 255.0f); // 	72      209     204
    const Color DarkTurquoise       = Color (0x00 / 255.0f, 0xCE / 255.0f, 0xD1 / 255.0f); // 	0       206     209
    const Color CadetBlue           = Color (0x5F / 255.0f, 0x9E / 255.0f, 0xA0 / 255.0f); // 	95      158     160
    const Color SteelBlue           = Color (0x46 / 255.0f, 0x82 / 255.0f, 0xB4 / 255.0f); // 	70      130     180
    const Color LightSteelBlue      = Color (0xB0 / 255.0f, 0xC4 / 255.0f, 0xDE / 255.0f); // 	176     196     222
    const Color PowderBlue          = Color (0xB0 / 255.0f, 0xE0 / 255.0f, 0xE6 / 255.0f); // 	176     224     230
    const Color LightBlue           = Color (0xAD / 255.0f, 0xD8 / 255.0f, 0xE6 / 255.0f); // 	173     216     230
    const Color SkyBlue             = Color (0x87 / 255.0f, 0xCE / 255.0f, 0xEB / 255.0f); // 	135     206     235
    const Color LightSkyBlue        = Color (0x87 / 255.0f, 0xCE / 255.0f, 0xFA / 255.0f); // 	135     206     250
    const Color DeepSkyBlue         = Color (0x00 / 255.0f, 0xBF / 255.0f, 0xFF / 255.0f); // 	0       191     255
    const Color DodgerBlue          = Color (0x1E / 255.0f, 0x90 / 255.0f, 0xFF / 255.0f); // 	30      144     255
    const Color CornflowerBlue      = Color (0x64 / 255.0f, 0x95 / 255.0f, 0xED / 255.0f); // 	100     149     237
    const Color MediumSlateBlue     = Color (0x7B / 255.0f, 0x68 / 255.0f, 0xEE / 255.0f); // 	123     104     238
    const Color RoyalBlue           = Color (0x41 / 255.0f, 0x69 / 255.0f, 0xE1 / 255.0f); // 	65      105     225
    const Color Blue                = Color (0x00 / 255.0f, 0x00 / 255.0f, 0xFF / 255.0f); // 	0       0       255
    const Color MediumBlue          = Color (0x00 / 255.0f, 0x00 / 255.0f, 0xCD / 255.0f); // 	0       0       205
    const Color DarkBlue            = Color (0x00 / 255.0f, 0x00 / 255.0f, 0x8B / 255.0f); // 	0       0       139
    const Color Navy                = Color (0x00 / 255.0f, 0x00 / 255.0f, 0x80 / 255.0f); // 	0       0       128
    const Color MidnightBlue        = Color (0x19 / 255.0f, 0x19 / 255.0f, 0x70 / 255.0f); // 	25      25      112

    //Brown colors
    const Color Cornsilk            = Color (0xFF / 255.0f, 0xF8 / 255.0f, 0xDC / 255.0f); // 	255     248     220
    const Color BlanchedAlmond      = Color (0xFF / 255.0f, 0xEB / 255.0f, 0xCD / 255.0f); // 	255     235     205
    const Color Bisque              = Color (0xFF / 255.0f, 0xE4 / 255.0f, 0xC4 / 255.0f); // 	255     228     196
    const Color NavajoWhite         = Color (0xFF / 255.0f, 0xDE / 255.0f, 0xAD / 255.0f); // 	255     222     173
    const Color Wheat               = Color (0xF5 / 255.0f, 0xDE / 255.0f, 0xB3 / 255.0f); // 	245     222     179
    const Color BurlyWood           = Color (0xDE / 255.0f, 0xB8 / 255.0f, 0x87 / 255.0f); // 	222     184     135
    const Color Tan                 = Color (0xD2 / 255.0f, 0xB4 / 255.0f, 0x8C / 255.0f); // 	210     180     140
    const Color RosyBrown           = Color (0xBC / 255.0f, 0x8F / 255.0f, 0x8F / 255.0f); // 	188     143     143
    const Color SandyBrown          = Color (0xF4 / 255.0f, 0xA4 / 255.0f, 0x60 / 255.0f); // 	244     164     96
    const Color Goldenrod           = Color (0xDA / 255.0f, 0xA5 / 255.0f, 0x20 / 255.0f); // 	218     165     32
    const Color DarkGoldenrod       = Color (0xB8 / 255.0f, 0x86 / 255.0f, 0x0B / 255.0f); // 	184     134     11
    const Color Peru                = Color (0xCD / 255.0f, 0x85 / 255.0f, 0x3F / 255.0f); // 	205     133     63
    const Color Chocolate           = Color (0xD2 / 255.0f, 0x69 / 255.0f, 0x1E / 255.0f); // 	210     105     30
    const Color SaddleBrown         = Color (0x8B / 255.0f, 0x45 / 255.0f, 0x13 / 255.0f); // 	139     69      19
    const Color Sienna              = Color (0xA0 / 255.0f, 0x52 / 255.0f, 0x2D / 255.0f); // 	160     82      45
    const Color Brown               = Color (0xA5 / 255.0f, 0x2A / 255.0f, 0x2A / 255.0f); // 	165     42      42
    const Color Maroon              = Color (0x80 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f); // 	128     0       0

    //White colors
    const Color White               = Color (0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f); // 	255     255     255
    const Color Snow                = Color (0xFF / 255.0f, 0xFA / 255.0f, 0xFA / 255.0f); // 	255     250     250
    const Color Honeydew            = Color (0xF0 / 255.0f, 0xFF / 255.0f, 0xF0 / 255.0f); // 	240     255     240
    const Color MintCream           = Color (0xF5 / 255.0f, 0xFF / 255.0f, 0xFA / 255.0f); // 	245     255     250
    const Color Azure               = Color (0xF0 / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f); // 	240     255     255
    const Color AliceBlue           = Color (0xF0 / 255.0f, 0xF8 / 255.0f, 0xFF / 255.0f); // 	240     248     255
    const Color GhostWhite          = Color (0xF8 / 255.0f, 0xF8 / 255.0f, 0xFF / 255.0f); // 	248     248     255
    const Color WhiteSmoke          = Color (0xF5 / 255.0f, 0xF5 / 255.0f, 0xF5 / 255.0f); // 	245     245     245
    const Color Seashell            = Color (0xFF / 255.0f, 0xF5 / 255.0f, 0xEE / 255.0f); // 	255     245     238
    const Color Beige               = Color (0xF5 / 255.0f, 0xF5 / 255.0f, 0xDC / 255.0f); // 	245     245     220
    const Color OldLace             = Color (0xFD / 255.0f, 0xF5 / 255.0f, 0xE6 / 255.0f); // 	253     245     230
    const Color FloralWhite         = Color (0xFF / 255.0f, 0xFA / 255.0f, 0xF0 / 255.0f); // 	255     250     240
    const Color Ivory               = Color (0xFF / 255.0f, 0xFF / 255.0f, 0xF0 / 255.0f); // 	255     255     240
    const Color AntiqueWhite        = Color (0xFA / 255.0f, 0xEB / 255.0f, 0xD7 / 255.0f); // 	250     235     215
    const Color Linen               = Color (0xFA / 255.0f, 0xF0 / 255.0f, 0xE6 / 255.0f); // 	250     240     230
    const Color LavenderBlush       = Color (0xFF / 255.0f, 0xF0 / 255.0f, 0xF5 / 255.0f); // 	255     240     245
    const Color MistyRose           = Color (0xFF / 255.0f, 0xE4 / 255.0f, 0xE1 / 255.0f); // 	255     228     225

    //Grey colors
    const Color Gainsboro           = Color (0xDC / 255.0f, 0xDC / 255.0f, 0xDC / 255.0f); // 	220     220     220
    const Color LightGrey           = Color (0xD3 / 255.0f, 0xD3 / 255.0f, 0xD3 / 255.0f); // 	211     211     211
    const Color Silver              = Color (0xC0 / 255.0f, 0xC0 / 255.0f, 0xC0 / 255.0f); // 	192     192     192
    const Color DarkGray            = Color (0xA9 / 255.0f, 0xA9 / 255.0f, 0xA9 / 255.0f); // 	169     169     169
    const Color Gray                = Color (0x80 / 255.0f, 0x80 / 255.0f, 0x80 / 255.0f); // 	128     128     128
    const Color DimGray             = Color (0x69 / 255.0f, 0x69 / 255.0f, 0x69 / 255.0f); // 	105     105     105
    const Color LightSlateGray      = Color (0x77 / 255.0f, 0x88 / 255.0f, 0x99 / 255.0f); // 	119     136     153
    const Color SlateGray           = Color (0x70 / 255.0f, 0x80 / 255.0f, 0x90 / 255.0f); // 	112     128     144
    const Color DarkSlateGray       = Color (0x2F / 255.0f, 0x4F / 255.0f, 0x4F / 255.0f); // 	47      79      79
    const Color Black               = Color (0x00 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f); // 	0       0       0

    // More Colors
    const Color Aubergine           = Color (0x2B / 255.0f, 0x0B / 255.0f, 0x30 / 255.0f); //  43      11      48
  }
}
