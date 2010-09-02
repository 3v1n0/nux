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


#include "NKernel.h"
#include "BitmapFormats.h"

NAMESPACE_BEGIN

PixelFormatInfo	GPixelFormats[] =
{
    // Name						BlockSizeX	BlockSizeY	BlockSizeZ	BlockBytes	NumComponents	Platform  Internal  Type    Memory      Flags  Supported        EPixelFormat
    //                                                                                          Format    Format            Alignment
    { TEXT("BITFMT_UNKNOWN"),           0,          0,          0,      0,              0,          0,      0,       0,      0,          0,		false       },	//	BITFMT_Unknown
    { TEXT("BITFMT_R8G8B8A8"),	        1,			1,      	1,      4,              4,          0,      0,       0,      4,          0,		true        },	//	BITFMT_R8G8B8A8
    { TEXT("BITFMT_A8B8G8R8"),	        1,			1,      	1,      4,              4,          0,      0,       0,      4,          0,		true        },	//	BITFMT_A8B8G8R8
    { TEXT("BITFMT_B8G8R8A8"),	        1,			1,      	1,      4,              4,          0,      0,       0,      4,          0,		true        },	//	BITFMT_B8G8R8A8
    { TEXT("BITFMT_A8R8G8B8"),	        1,			1,      	1,      4,              4,          0,      0,       0,      4,          0,		true        },	//	BITFMT_A8R8G8B8
    { TEXT("BITFMT_R8G8B8"),            1,          1,      	1,      3,              3,          0,      0,       0,      1,          0,		true        },	//	BITFMT_R8G8B8
    { TEXT("BITFMT_B8G8R8"),            1,          1,      	1,      3,              3,          0,      0,       0,      1,          0,		true        },	//	BITFMT_B8G8R8
    { TEXT("BITFMT_R5G6B5"),            1,          1,          1,      2,              3,          0,      0,       0,      1,          0,		true        },	//	BITFMT_R5G6B5
    { TEXT("BITFMT_RGBA16F"),           1,          1,          1,      8,              4,          0,      0,       0,      1,          0,		true        },	//	BITFMT_RGBA16F
    { TEXT("BITFMT_RGB32F"),            1,          1,          1,      12,             3,          0,      0,       0,      1,          0,		true        },	//	BITFMT_RGB32F
    { TEXT("BITFMT_RGBA32F"),           1,          1,          1,      16,             4,          0,      0,       0,      1,          0,		true        },	//	BITFMT_RGBA32F
    { TEXT("BITFMT_D24S8"),             1,          1,          1,      4,              2,          0,      0,       0,      1,          0,		true        },	//	BITFMT_D24S8
    { TEXT("BITFMT_DXT1"),              4,			4,      	1,      8,              3,          0,      0,       0,      1,          0,		true        },	//	BITFMT_DXT1
    { TEXT("BITFMT_DXT2"),              4,			4,      	1,      16,             4,          0,      0,       0,      1,          0,		true        },	//	BITFMT_DXT2
    { TEXT("BITFMT_DXT3"),              4,			4,      	1,      16,             4,          0,      0,       0,      1,          0,		true        },	//	BITFMT_DXT3
    { TEXT("BITFMT_DXT4"),              4,			4,      	1,      16,             4,          0,      0,       0,      1,          0,		true        },	//	BITFMT_DXT4
    { TEXT("BITFMT_DXT5"),              4,			4,      	1,      16,             4,          0,      0,       0,      1,          0,		true        },	//	BITFMT_DXT5
    { TEXT("BITFMT_R10G10B10A2"),	    1,			1,      	1,      4,              4,          0,      0,       0,      4,          0,		true        },	//	BITFMT_R10G10B10A2
    { TEXT("BITFMT_A2B10G10R10"),	    1,			1,      	1,      4,              4,          0,      0,       0,      4,          0,		true        },	//	BITFMT_A2B10G10R10
    { TEXT("BITFMT_B10G10R10A2"),	    1,			1,      	1,      4,              4,          0,      0,       0,      4,          0,		true        },	//	BITFMT_B10G10R10A2
    { TEXT("BITFMT_A2R10G10B10"),	    1,			1,      	1,      4,              4,          0,      0,       0,      4,          0,		true        },	//	BITFMT_A2R10G10B10

    { TEXT("BITFMT_A8"),	            1,			1,      	1,          1,          1,          0,      0,       0,      1,          0,		true        },	//	BITFMT_A8

    { TEXT("BITFMT_END_GFX_FORMATS"),	0,          0,          0,          0,          0,          0,      0,       0,      0,          0,		false       },  //  BITFMT_END_GFX_FORMATS
};


ReadBackPixelFormatInfo	GReadBackPixelFormats[] =
{
    // Name						    BlockBytes	NumComponents	 Format        Type       Flags    Supported			EPixelFormat

    { TEXT("BITFMT_UNKNOWN"),           0,          0,				0,	        0,         0,           false       },	//	BITFMT_Unknown
    { TEXT("BITFMT_R8G8B8A8"),	        4,          4,				0,	        0,         0,           true        },	//	BITFMT_R8G8B8A8
    { TEXT("BITFMT_A8B8G8R8"),	        4,          4,				0,	        0,         0,           true        },	//	BITFMT_A8B8G8R8
    { TEXT("BITFMT_B8G8R8A8"),	        4,          4,				0,	        0,         0,           true        },	//	BITFMT_B8G8R8A8
    { TEXT("BITFMT_A8R8G8B8"),	        4,          4,				0,	        0,         0,           true        },	//	BITFMT_A8R8G8B8
    { TEXT("BITFMT_R8G8B8"),            3,          3,				0,	        0,         0,           true        },	//	BITFMT_R8G8B8
    { TEXT("BITFMT_B8G8R8"),            3,          3,				0,	        0,         0,           true        },	//	BITFMT_B8G8R8
    { TEXT("BITFMT_R5G6B5"),            2,          3,              0,          0,         0,           true        },	//	BITFMT_R5G6B5
    { TEXT("BITFMT_RGBA16F"),           8,          4,				0,	        0,         0,           true        },	//	BITFMT_RGBA16F
    { TEXT("BITFMT_RGBA32F"),           16,         4,				0,	        0,         0,           true        },	//	BITFMT_RGBA32F
    { TEXT("BITFMT_D24S8"),             4,          2,              0,          0,         0,           true        },	//	BITFMT_D24S8
    { TEXT("BITFMT_DXT1"),              8,          3,				0,	        0,         0,           true        },	//	BITFMT_DXT1
    { TEXT("BITFMT_DXT2"),              16,         4,				0,	        0,         0,           true        },	//	BITFMT_DXT2
    { TEXT("BITFMT_DXT3"),              16,         4,				0,	        0,         0,           true        },	//	BITFMT_DXT3
    { TEXT("BITFMT_DXT4"),              16,         4,				0,	        0,         0,           true        },	//	BITFMT_DXT4
    { TEXT("BITFMT_DXT5"),              16,         4,				0,	        0,         0,           true        },	//	BITFMT_DXT5
    { TEXT("BITFMT_R10G10B10A2"),	    4,          4,				0,	        0,         0,           true        },	//	BITFMT_R10G10B10A2
    { TEXT("BITFMT_A2B10G10R10"),	    4,          4,				0,	        0,         0,           true        },	//	BITFMT_A2B10G10R10
    { TEXT("BITFMT_B10G10R10A2"),	    4,          4,				0,	        0,         0,           true        },	//	BITFMT_B10G10R10A2
    { TEXT("BITFMT_A2R10G10B10"),	    4,          4,				0,	        0,         0,           true        },	//	BITFMT_A2R10G10B10

    { TEXT("BITFMT_A8"),	            1,          1,				0,	        0,         0,           true        },	//	BITFMT_A8

    { TEXT("BITFMT_END_GFX_FORMATS"),   0,          0,				0,	        0,         0,		    false       },  //  BITFMT_END_GFX_FORMATS
};

NAMESPACE_END
