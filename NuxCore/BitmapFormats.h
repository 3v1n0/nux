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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef BITMAPFORMATS_H
#define BITMAPFORMATS_H

#include "NSystemTypes.h"

NAMESPACE_BEGIN

typedef struct
{
    const TCHAR*	Name;
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
    const TCHAR*	Name;
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
    BITFMT_A8B8G8R8,            // 32-bit ABGR pixel format with alpha, using 8 bits per channel.

    BITFMT_B8G8R8A8,            // 32-bit BGRA pixel format with alpha, using 8 bits per channel.
    BITFMT_A8R8G8B8,            // 32-bit ARGB pixel format with alpha, using 8 bits per channel.

    BITFMT_R8G8B8,              // 24-bit RGB pixel format with 8 bits per channel. 
    BITFMT_B8G8R8,              // 24-bit RGB pixel format with 8 bits per channel. 

    BITFMT_R5G6B5,             // 16-bit RGB pixel format with 5 bits for red, 6 bits for green, and 5 bits for blue. 

    BITFMT_RGBA16F,
    BITFMT_RGB32F,
    BITFMT_RGBA32F,
    BITFMT_D24S8,

    BITFMT_DXT1,
    BITFMT_DXT2,
    BITFMT_DXT3,
    BITFMT_DXT4,
    BITFMT_DXT5,
    
    BITFMT_R10G10B10A2,         // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
    BITFMT_B10G10R10A2,         // 32-bit pixel format using 10 bits for each color and 2 bits for alpha.
    BITFMT_A2R10G10B10,         // 32-bit pixel format using 10 bits for each color and 2 bits for alpha. 
    BITFMT_A2B10G10R10,         // 32-bit pixel format using 10 bits for each color and 2 bits for alpha. 

    BITFMT_A8,                  // Gray level 8 bits

    BITFMT_END_GFX_FORMATS,

    BITFMT_X8R8G8B8,            // 32-bit RGB pixel format, where 8 bits are reserved for each color. 
    BITFMT_X8B8G8R8,            // 32-bit RGB pixel format, where 8 bits are reserved for each color. 

    BITFMT_X1R5G5B5,           // 16-bit pixel format where 5 bits are reserved for each color. 
    BITFMT_A1R5G5B5,           // 16-bit pixel format where 5 bits are reserved for each color and 1 bit is reserved for alpha. 
    BITFMT_A4R4G4B4,           // 16-bit ARGB pixel format with 4 bits for each channel. 
    BITFMT_X4R4G4B4,           // 16-bit RGB pixel format using 4 bits for each color. 
    BITFMT_A8R3G3B2,           // 16-bit ARGB texture format using 8 bits for alpha, 3 bits each for red and green, and 2 bits for blue. 
   
    //BITFMT_A8P8,               // 8-bit color indexed with 8 bits of alpha. 
    //BITFMT_A8L8,               // 16-bit using 8 bits each for alpha and luminance. 
    //BITFMT_GRAY16,             // Gray level 16 bits

    BITFMT_R3G3B2,             // 8-bit RGB texture format using 3 bits for red, 3 bits for green, and 2 bits for blue. 

    //BITFMT_P8,                 // 8-bit color indexed. 
    //BITFMT_L8,                 // 8-bit luminance only. 
    //BITFMT_A4L4,               // 8-bit using 4 bits each for alpha and luminance.
    //BITFMT_A8,                 // 8-bit alpha only. 
    

} BitmapFormat;
  
extern PixelFormatInfo GPixelFormats[];
extern ReadBackPixelFormatInfo	GReadBackPixelFormats[];

NAMESPACE_END

#endif // BITMAPFORMATS_H
