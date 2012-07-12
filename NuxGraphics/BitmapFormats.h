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


#ifndef BITMAPFORMATS_H
#define BITMAPFORMATS_H

namespace nux
{

  typedef struct
  {
    const TCHAR	*Name;
    int             BlockSizeX;
    int             BlockSizeY;
    int             BlockSizeZ;
    int             BlockBytes;
    int             NumComponents;
    unsigned int	PlatformFormat; // internal format
    int             Format;
    int             type;
    int             RowMemoryAlignment; // best memory alignment in a row
    unsigned int	Flags;
    bool			Supported;
  }  PixelFormatInfo;

  typedef struct
  {
    const TCHAR	*Name;
    int             BlockBytes;
    int             NumComponents;
    unsigned int    Format;
    int             type;
    unsigned int	Flags;
    bool			Supported;
  } ReadBackPixelFormatInfo;

  typedef enum
  {
    BITFMT_UNKNOWN          = 0,
    BITFMT_R8G8B8A8,            // 32-bit RGBA pixel format with alpha, using 8 bits per channel.
    BITFMT_B8G8R8A8,            // 32-bit BGRA pixel format with alpha, using 8 bits per channel.
    BITFMT_R8G8B8,              // 24-bit RGB pixel format with 8 bits per channel.
    BITFMT_R5G6B5,             // 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue.
    BITFMT_D24S8,
    BITFMT_A8,                  // Gray level 8 bits

    BITFMT_DXT1,
    BITFMT_DXT2,
    BITFMT_DXT3,
    BITFMT_DXT4,
    BITFMT_DXT5,

#ifndef NUX_OPENGLES_20
    BITFMT_A8R8G8B8,            // 32-bit ARGB pixel format with alpha, using 8 bits per channel.
    BITFMT_A8B8G8R8,            // 32-bit ABGR pixel format with alpha, using 8 bits per channel.
    BITFMT_B8G8R8,              // 24-bit RGB pixel format with 8 bits per channel.
    BITFMT_RGBA16F,
    BITFMT_RGB32F,
    BITFMT_RGBA32F,

    BITFMT_R10G10B10A2,         // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
    BITFMT_B10G10R10A2,         // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
    BITFMT_A2R10G10B10,         // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
    BITFMT_A2B10G10R10,         // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
#endif
    BITFMT_END_GFX_FORMATS,
  } BitmapFormat;

  extern PixelFormatInfo GPixelFormats[];
  extern ReadBackPixelFormatInfo	GReadBackPixelFormats[];

}

#endif // BITMAPFORMATS_H
