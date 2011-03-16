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


#ifndef COLORS_H
#define COLORS_H

#include "SystemTypes.h"

namespace nux
{
  namespace Colors
  {
    //X11 color names from:http://en.wikipedia.org/wiki/Web_colors

    //Red colors
    extern const Color      IndianRed           ;//     CD 5C 5C 	205 92 92
    extern const Color      LightCoral          ;//        F0 80 80 	240 128 128
    extern const Color      Salmon              ;//        FA 80 72 	250 128 114
    extern const Color      DarkSalmon          ;//        E9 96 7A 	233 150 122
    extern const Color      LightSalmon         ;//        FF A0 7A 	255 160 122
    extern const Color      Crimson             ;//        DC 14 3C 	220 20 60
    extern const Color      Red                 ;//        FF 00 00 	255 0 0
    extern const Color      FireBrick           ;//        B2 22 22 	178 34 34
    extern const Color      DarkRed             ;//        8B 00 00 	139 0 0

    //Pink colors
    extern const Color      Pink                ;//        FF C0 CB 	255 192 203
    extern const Color      LightPink           ;//        FF B6 C1 	255 182 193
    extern const Color      HotPink             ;//        FF 69 B4 	255 105 180
    extern const Color      DeepPink            ;//        FF 14 93 	255 20 147
    extern const Color      MediumVioletRed     ;//        C7 15 85 	199 21 133
    extern const Color      PaleVioletRed       ;//        DB 70 93 	219 112 147

    //Orange colors                             ;//
    //extern const Color      LightSalmon         ;//        FF A0 7A 	255 160 122
    extern const Color      Coral               ;//        FF 7F 50 	255 127 80
    extern const Color      Tomato              ;//        FF 63 47 	255 99 71
    extern const Color      OrangeRed           ;//        FF 45 00 	255 69 0
    extern const Color      DarkOrange          ;//        FF 8C 00 	255 140 0
    extern const Color      Orange              ;//        FF A5 00 	255 165 0

    //Yellow colors                             ;//
    extern const Color      Gold                ;//        FF D7 00 	255 215 0
    extern const Color      Yellow              ;//        FF FF 00 	255 255 0
    extern const Color      LightYellow         ;//        FF FF E0 	255 255 224
    extern const Color      LemonChiffon        ;//        FF FA CD 	255 250 205
    extern const Color      LightGoldenrodYellow;// 	    FA FA D2 	250 250 210
    extern const Color      PapayaWhip          ;//        FF EF D5 	255 239 213
    extern const Color      Moccasin            ;//        FF E4 B5 	255 228 181
    extern const Color      PeachPuff           ;//        FF DA B9 	255 218 185
    extern const Color      PaleGoldenrod       ;//        EE E8 AA 	238 232 170
    extern const Color      Khaki               ;//        F0 E6 8C 	240 230 140
    extern const Color      DarkKhaki           ;//        BD B7 6B 	189 183 107

    //Purple colors
    extern const Color      Lavender            ;//        E6 E6 FA 	230 230 250
    extern const Color      Thistle             ;//        D8 BF D8 	216 191 216
    extern const Color      Plum                ;//        DD A0 DD 	221 160 221
    extern const Color      Violet              ;//        EE 82 EE 	238 130 238
    extern const Color      Orchid              ;//        DA 70 D6 	218 112 214
    extern const Color      Fuchsia             ;//        FF 00 FF 	255 0 255
    extern const Color      Magenta             ;//        FF 00 FF 	255 0 255
    extern const Color      MediumOrchid        ;//        BA 55 D3 	186 85 211
    extern const Color      MediumPurple        ;//        93 70 DB 	147 112 219
    extern const Color      BlueViolet          ;//        8A 2B E2 	138 43 226
    extern const Color      DarkViolet          ;//        94 00 D3 	148 0 211
    extern const Color      DarkOrchid          ;//        99 32 CC 	153 50 204
    extern const Color      DarkMagenta         ;//        8B 00 8B 	139 0 139
    extern const Color      Purple              ;//        80 00 80 	128 0 128
    extern const Color      Indigo              ;//        4B 00 82 	75 0 130
    extern const Color      SlateBlue           ;//        6A 5A CD 	106 90 205
    extern const Color      DarkSlateBlue       ;//        48 3D 8B 	72 61 139

    //Green colors
    extern const Color      GreenYellow         ;//        AD FF 2F 	173 255 47
    extern const Color      Chartreuse          ;//        7F FF 00 	127 255 0
    extern const Color      LawnGreen           ;//        7C FC 00 	124 252 0
    extern const Color      Lime                ;//        00 FF 00 	0 255 0
    extern const Color      LimeGreen           ;//        32 CD 32 	50 205 50
    extern const Color      PaleGreen           ;//        98 FB 98 	152 251 152
    extern const Color      LightGreen          ;//        90 EE 90 	144 238 144
    extern const Color      MediumSpringGreen   ;//        00 FA 9A 	0 250 154
    extern const Color      SpringGreen         ;//        00 FF 7F 	0 255 127
    extern const Color      MediumSeaGreen      ;//        3C B3 71 	60 179 113
    extern const Color      SeaGreen            ;//        2E 8B 57 	46 139 87
    extern const Color      ForestGreen         ;//        22 8B 22 	34 139 34
    extern const Color      Green               ;//        00 80 00 	0 128 0
    extern const Color      DarkGreen           ;//        00 64 00 	0 100 0
    extern const Color      YellowGreen         ;//        9A CD 32 	154 205 50
    extern const Color      OliveDrab           ;//        6B 8E 23 	107 142 35
    extern const Color      Olive               ;//        80 80 00 	128 128 0
    extern const Color      DarkOliveGreen      ;//        55 6B 2F 	85 107 47
    extern const Color      MediumAquamarine    ;//        66 CD AA 	102 205 170
    extern const Color      DarkSeaGreen        ;//        8F BC 8F 	143 188 143
    extern const Color      LightSeaGreen       ;//        20 B2 AA 	32 178 170
    extern const Color      DarkCyan            ;//        00 8B 8B 	0 139 139
    extern const Color      Teal                ;//        00 80 80 	0 128 128

    //Blue colors                               ;//
    extern const Color      Aqua                ;//        00 FF FF 	0 255 255
    extern const Color      Cyan                ;//        00 FF FF 	0 255 255
    extern const Color      LightCyan           ;//        E0 FF FF 	224 255 255
    extern const Color      PaleTurquoise       ;//        AF EE EE 	175 238 238
    extern const Color      Aquamarine          ;//        7F FF D4 	127 255 212
    extern const Color      Turquoise           ;//        40 E0 D0 	64 224 208
    extern const Color      MediumTurquoise     ;//        48 D1 CC 	72 209 204
    extern const Color      DarkTurquoise       ;//        00 CE D1 	0 206 209
    extern const Color      CadetBlue           ;//        5F 9E A0 	95 158 160
    extern const Color      SteelBlue           ;//        46 82 B4 	70 130 180
    extern const Color      LightSteelBlue      ;//        B0 C4 DE 	176 196 222
    extern const Color      PowderBlue          ;//        B0 E0 E6 	176 224 230
    extern const Color      LightBlue           ;//        AD D8 E6 	173 216 230
    extern const Color      SkyBlue             ;//        87 CE EB 	135 206 235
    extern const Color      LightSkyBlue        ;//        87 CE FA 	135 206 250
    extern const Color      DeepSkyBlue         ;//        00 BF FF 	0 191 255
    extern const Color      DodgerBlue          ;//        1E 90 FF 	30 144 255
    extern const Color      CornflowerBlue      ;//        64 95 ED 	100 149 237
    extern const Color      MediumSlateBlue     ;//        7B 68 EE 	123 104 238
    extern const Color      RoyalBlue           ;//        41 69 E1 	65 105 225
    extern const Color      Blue                ;//        00 00 FF 	0 0 255
    extern const Color      MediumBlue          ;//        00 00 CD 	0 0 205
    extern const Color      DarkBlue            ;//        00 00 8B 	0 0 139
    extern const Color      Navy                ;//        00 00 80 	0 0 128
    extern const Color      MidnightBlue        ;//        19 19 70 	25 25 112

    //Brown colors                              ;//
    extern const Color      Cornsilk            ;//        FF F8 DC 	255 248 220
    extern const Color      BlanchedAlmond      ;//        FF EB CD 	255 235 205
    extern const Color      Bisque              ;//        FF E4 C4 	255 228 196
    extern const Color      NavajoWhite         ;//        FF DE AD 	255 222 173
    extern const Color      Wheat               ;//        F5 DE B3 	245 222 179
    extern const Color      BurlyWood           ;//        DE B8 87 	222 184 135
    extern const Color      Tan                 ;//        D2 B4 8C 	210 180 140
    extern const Color      RosyBrown           ;//        BC 8F 8F 	188 143 143
    extern const Color      SandyBrown          ;//        F4 A4 60 	244 164 96
    extern const Color      Goldenrod           ;//        DA A5 20 	218 165 32
    extern const Color      DarkGoldenrod       ;//        B8 86 0B 	184 134 11
    extern const Color      Peru                ;//        CD 85 3F 	205 133 63
    extern const Color      Chocolate           ;//        D2 69 1E 	210 105 30
    extern const Color      SaddleBrown         ;//        8B 45 13 	139 69 19
    extern const Color      Sienna              ;//        A0 52 2D 	160 82 45
    extern const Color      Brown               ;//        A5 2A 2A 	165 42 42
    extern const Color      Maroon              ;//        80 00 00 	128 0 0

    //White colors                              ;//
    extern const Color      White               ;//        FF FF FF 	255 255 255
    extern const Color      Snow                ;//        FF FA FA 	255 250 250
    extern const Color      Honeydew            ;//        F0 FF F0 	240 255 240
    extern const Color      MintCream           ;//        F5 FF FA 	245 255 250
    extern const Color      Azure               ;//        F0 FF FF 	240 255 255
    extern const Color      AliceBlue           ;//        F0 F8 FF 	240 248 255
    extern const Color      GhostWhite          ;//        F8 F8 FF 	248 248 255
    extern const Color      WhiteSmoke          ;//        F5 F5 F5 	245 245 245
    extern const Color      Seashell            ;//        FF F5 EE 	255 245 238
    extern const Color      Beige               ;//        F5 F5 DC 	245 245 220
    extern const Color      OldLace             ;//        FD F5 E6 	253 245 230
    extern const Color      FloralWhite         ;//        FF FA F0 	255 250 240
    extern const Color      Ivory               ;//        FF FF F0 	255 255 240
    extern const Color      AntiqueWhite        ;//        FA EB D7 	250 235 215
    extern const Color      Linen               ;//        FA F0 E6 	250 240 230
    extern const Color      LavenderBlush       ;//        FF F0 F5 	255 240 245
    extern const Color      MistyRose           ;//        FF E4 E1 	255 228 225

    //Grey colors                               ;//
    extern const Color      Gainsboro           ;//        DC DC DC 	220 220 220
    extern const Color      LightGrey           ;//        D3 D3 D3 	211 211 211
    extern const Color      Silver              ;//        C0 C0 C0 	192 192 192
    extern const Color      DarkGray            ;//        A9 A9 A9 	169 169 169
    extern const Color      Gray                ;//        80 80 80 	128 128 128
    extern const Color      DimGray             ;//        69 69 69 	105 105 105
    extern const Color      LightSlateGray      ;//        77 88 99 	119 136 153
    extern const Color      SlateGray           ;//        70 80 90 	112 128 144
    extern const Color      DarkSlateGray       ;//        2F 4F 4F 	47 79 79
    extern const Color      Black               ;//        00 00 00 	0 0 0

    // More Colors
    extern const Color      Aubergine           ;//        2B 0B 30 	43 11 48
  }
}

#endif // COLORS_H

