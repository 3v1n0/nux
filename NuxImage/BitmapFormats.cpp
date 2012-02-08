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


#include "NuxCore/NuxCore.h"
#include "BitmapFormats.h"

namespace nux
{

  PixelFormatInfo GPixelFormats[] =
  {
    // Name                           BlockSizeX BlockSizeY BlockSizeZ            BlockBytes NumComponents    Platform  Internal    Memory      Flags       Supported
    //                                                                                                        Format    Format      Alignment
    { "BITFMT_UNKNOWN",          0,          0,       0,      0,              0,          0,             0,       0,           0,          0,        false       }, // BITFMT_Unknown
    { "BITFMT_R8G8B8A8",         1,          1,       1,      4,              4,          0,             0,       0,           4,          0,        true        }, // BITFMT_R8G8B8A8
    { "BITFMT_B8G8R8A8",         1,          1,       1,      4,              4,          0,             0,       0,           4,          0,        true        }, // BITFMT_B8G8R8A8
    { "BITFMT_R8G8B8",           1,          1,       1,      3,              3,          0,             0,       0,           1,          0,        true        }, // BITFMT_R8G8B8
    { "BITFMT_R5G6B5",           1,          1,       1,      2,              3,          0,             0,       0,           1,          0,        true        }, // BITFMT_R5G6B5
    { "BITFMT_D24S8",            1,          1,       1,      4,              2,          0,             0,       0,           1,          0,        true        }, // BITFMT_D24S8
    { "BITFMT_A8",               1,          1,       1,      1,              1,          0,             0,       0,           1,          0,        true        }, // BITFMT_A8

    { "BITFMT_DXT1",             4,          4,       1,      8,              3,          0,             0,       0,           1,          0,        true        }, // BITFMT_DXT1
    { "BITFMT_DXT2",             4,          4,       1,      16,             4,          0,             0,       0,           1,          0,        true        }, // BITFMT_DXT2
    { "BITFMT_DXT3",             4,          4,       1,      16,             4,          0,             0,       0,           1,          0,        true        }, // BITFMT_DXT3
    { "BITFMT_DXT4",             4,          4,       1,      16,             4,          0,             0,       0,           1,          0,        true        }, // BITFMT_DXT4
    { "BITFMT_DXT5",             4,          4,       1,      16,             4,          0,             0,       0,           1,          0,        true        }, // BITFMT_DXT5

#ifndef NUX_OPENGLES_20
    { "BITFMT_A8R8G8B8",         1,          1,       1,      4,              4,          0,             0,       0,           4,          0,        true        }, // BITFMT_A8R8G8B8
    { "BITFMT_A8B8G8R8",         1,          1,       1,      4,              4,          0,             0,       0,           4,          0,        true        }, // BITFMT_A8B8G8R8
    { "BITFMT_B8G8R8",           1,          1,       1,      3,              3,          0,             0,       0,           1,          0,        true        }, // BITFMT_B8G8R8
    { "BITFMT_RGBA16F",          1,          1,       1,      8,              4,          0,             0,       0,           1,          0,        true        }, // BITFMT_RGBA16F
    { "BITFMT_RGB32F",           1,          1,       1,      12,             3,          0,             0,       0,           1,          0,        true        }, // BITFMT_RGB32F
    { "BITFMT_RGBA32F",          1,          1,       1,      16,             4,          0,             0,       0,           1,          0,        true        }, // BITFMT_RGBA32F
    { "BITFMT_R10G10B10A2",      1,          1,       1,      4,              4,          0,             0,       0,           4,          0,        true        }, // BITFMT_R10G10B10A2
    { "BITFMT_B10G10R10A2",      1,          1,       1,      4,              4,          0,             0,       0,           4,          0,        true        }, // BITFMT_B10G10R10A2
    { "BITFMT_A2R10G10B10",      1,          1,       1,      4,              4,          0,             0,       0,           4,          0,        true        }, // BITFMT_A2R10G10B10
    { "BITFMT_A2B10G10R10",      1,          1,       1,      4,              4,          0,             0,       0,           4,          0,        true        }, // BITFMT_A2B10G10R10
#endif
    { "BITFMT_END_GFX_FORMATS", 0,          0,        0,      0,              0,          0,             0,       0,           0,          0,        false       }, //  BITFMT_END_GFX_FORMATS
  };


  ReadBackPixelFormatInfo GReadBackPixelFormats[] =
  {
    // Name                        BlockBytes  NumComponents Format    Type      Flags           Supported

    { "BITFMT_UNKNOWN",          0,          0,        0,          0,         0,           false       }, // BITFMT_Unknown
    { "BITFMT_R8G8B8A8",         4,          4,        0,          0,         0,           true        }, // BITFMT_R8G8B8A8
    { "BITFMT_B8G8R8A8",         4,          4,        0,          0,         0,           true        }, // BITFMT_B8G8R8A8
    { "BITFMT_R8G8B8",           3,          3,        0,          0,         0,           true        }, // BITFMT_R8G8B8
    { "BITFMT_R5G6B5",           2,          3,        0,          0,         0,           true        }, // BITFMT_R5G6B5
    { "BITFMT_D24S8",            4,          2,        0,          0,         0,           true        }, // BITFMT_D24S8
    { "BITFMT_A8",               1,          1,        0,          0,         0,           true        }, // BITFMT_A8
                                                                                                                    
    { "BITFMT_DXT1",             8,          3,        0,          0,         0,           true        }, // BITFMT_DXT1
    { "BITFMT_DXT2",             16,         4,        0,          0,         0,           true        }, // BITFMT_DXT2
    { "BITFMT_DXT3",             16,         4,        0,          0,         0,           true        }, // BITFMT_DXT3
    { "BITFMT_DXT4",             16,         4,        0,          0,         0,           true        }, // BITFMT_DXT4
    { "BITFMT_DXT5",             16,         4,        0,          0,         0,           true        }, // BITFMT_DXT5

#ifndef NUX_OPENGLES_20                                                                                             
    { "BITFMT_A8R8G8B8",         4,          4,        0,          0,         0,           true        }, // BITFMT_A8R8G8B8
    { "BITFMT_A8B8G8R8",         4,          4,        0,          0,         0,           true        }, // BITFMT_A8B8G8R8
    { "BITFMT_B8G8R8",           3,          3,        0,          0,         0,           true        }, // BITFMT_B8G8R8
    { "BITFMT_RGBA16F",          8,          4,        0,          0,         0,           true        }, // BITFMT_RGBA16F
    { "BITFMT_RGB32F",           12,         3,        0,          0,         0,           true        }, // BITFMT_RGBA32F
    { "BITFMT_RGBA32F",          16,         4,        0,          0,         0,           true        }, // BITFMT_RGBA32F
    { "BITFMT_R10G10B10A2",      4,          4,        0,          0,         0,           true        }, // BITFMT_R10G10B10A2
    { "BITFMT_B10G10R10A2",      4,          4,        0,          0,         0,           true        }, // BITFMT_B10G10R10A2
    { "BITFMT_A2B10G10R10",      4,          4,        0,          0,         0,           true        }, // BITFMT_A2B10G10R10
    { "BITFMT_A2R10G10B10",      4,          4,        0,          0,         0,           true        }, // BITFMT_A2R10G10B10
#endif                                                             
    { "BITFMT_END_GFX_FORMATS",  0,          0,        0,          0,         0,           false       }, //  BITFMT_END_GFX_FORMATS
  };

}
