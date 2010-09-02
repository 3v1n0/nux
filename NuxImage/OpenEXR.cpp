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


#include "NuxCore/NKernel.h"

#include "NuxCore/NKernel.h"
#include "BitmapFormats.h"


#ifdef INL_OPENEXR_SUPPORT
#include "OpenEXR.h"
//#include "OpenEXR/ctlToLut.h"
//#include "OpenEXR/fileNameForFrame.h"
//#include "OpenEXR/FileReadingThread.h"
//#include "OpenEXR/osDependent.h"
//#include "OpenEXR/ImageBuffers.h"
//#include "OpenEXR/Timer.h"
#include "OpenEXR/ImfThreading.h"
#include "OpenEXR/ImfInputFile.h"
#include "OpenEXR/ImfChannelList.h"
#include "OpenEXR/ImfStandardAttributes.h"
#include "OpenEXR/ImfRgbaYca.h"
#include "OpenEXR/ImfArray.h"
#include "OpenEXR/ImfRgbaFile.h"
#include "OpenEXR/ImfChannelList.h"
#include "OpenEXR/half.h"
#include "OpenEXR/imathbox.h"
#include "OpenEXR/ImfFrameBuffer.h"


#include <stdlib.h>
#include <sstream>
#include <iomanip>
#include <cmath>

NAMESPACE_BEGIN

NBitmapData* Load_OpenEXR(const TCHAR *fileName)
{
    Imf::Array2D<float> rPixels;
    Imf::Array2D<float> gPixels;
    Imf::Array2D<float> bPixels;

    Imf::InputFile in(fileName);

    Imf::Header header = in.header();
    const Imath::Box2i &dw = in.header().dataWindow();

    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    const Imf::ChannelList &ch = in.header().channels();
    

    rPixels.resizeErase (height, width);
    gPixels.resizeErase (height, width);
    bPixels.resizeErase (height, width);

    float* FloatArray = 0;
    NBitmapData* BitmapData = 0;

    if (ch.findChannel ("Y") || ch.findChannel ("RY") || ch.findChannel ("BY"))
    {

    }
    else if (ch.findChannel ("R") && ch.findChannel ("G") && ch.findChannel ("B") && ch.findChannel ("A"))
    {
        //
        // RGBA mode
        //
        // The pixel buffers for the tree image channels (RGB)
        // are padded with a fourth dummy channel (A) and interleaved
        // (RGBARGBARGBA...).  All three buffers have the same width
        // and height as the frame.
        //

        BitmapData = (NBitmapData*) new NTextureData(
            BITFMT_RGBA32F,
            width,
            height, 1);

        BYTE* Dest = BitmapData->GetSurface(0).GetPtrRawData(); //new float[4*width*height];

        Imf::FrameBuffer frameBuffer;

        frameBuffer.insert ("R",        // name
            Imf::Slice (Imf::FLOAT,     // type
            (char*) (&Dest[0] -         // base
            dw.min.x -
            dw.min.y * width),
            sizeof (float) * 4,         // xStride
            sizeof (float) * 4 * width, // yStride
            1, 1,                       // x/y sampling
            0.0));                      // fillValue

        frameBuffer.insert ("G",        // name
            Imf::Slice (Imf::FLOAT,     // type
            (char*) (&Dest[4] -         // base
            dw.min.x -
            dw.min.y * width),
            sizeof (float) * 4,         // xStride
            sizeof (float) * 4 * width, // yStride
            1, 1,                       // x/y sampling
            0.0));                      // fillValue

        frameBuffer.insert ("B",        // name
            Imf::Slice (Imf::FLOAT,     // type
            (char*) (&Dest[8] -         // base
            dw.min.x -
            dw.min.y * width),
            sizeof (float) * 4,         // xStride
            sizeof (float) * 4 * width, // yStride
            1, 1,                       // x/y sampling
            0.0));                      // fillValue

        frameBuffer.insert ("A",        // name
            Imf::Slice (Imf::FLOAT,     // type
            (char*) (&Dest[12] -         // base
            dw.min.x -
            dw.min.y * width),
            sizeof (float) * 4,         // xStride
            sizeof (float) * 4 * width, // yStride
            1, 1,                       // x/y sampling
            1.0));                      // fillValue

        in.setFrameBuffer (frameBuffer);
        in.readPixels (dw.min.y, dw.max.y);

    }
    else if (ch.findChannel ("R") && ch.findChannel ("G") && ch.findChannel ("B"))
    {
        //
        // RGB mode
        //
        // The pixel buffers for the tree image channels (RGB)
        // are padded with a fourth dummy channel (A) and interleaved
        // (RGBRGBRGB...).  All three buffers have the same width
        // and height as the frame.
        //

        BitmapData = (NBitmapData*) new NTextureData(
            BITFMT_RGB32F,
            width,
            height, 1);

        BYTE* Dest = BitmapData->GetSurface(0).GetPtrRawData(); //new float[4*width*height];

        Imf::FrameBuffer frameBuffer;

        frameBuffer.insert ("R",        // name
            Imf::Slice (Imf::FLOAT,     // type
            (char*) (&Dest[0] -         // base
            dw.min.x -
            dw.min.y * width),
            sizeof (float) * 3,         // xStride
            sizeof (float) * 3 * width, // yStride
            1, 1,                       // x/y sampling
            0.0));                      // fillValue

        frameBuffer.insert ("G",        // name
            Imf::Slice (Imf::FLOAT,     // type
            (char*) (&Dest[4] -         // base
            dw.min.x -
            dw.min.y * width),
            sizeof (float) * 3,         // xStride
            sizeof (float) * 3 * width, // yStride
            1, 1,                       // x/y sampling
            0.0));                      // fillValue

        frameBuffer.insert ("B",        // name
            Imf::Slice (Imf::FLOAT,     // type
            (char*) (&Dest[8] -         // base
            dw.min.x -
            dw.min.y * width),
            sizeof (float) * 3,         // xStride
            sizeof (float) * 3 * width, // yStride
            1, 1,                       // x/y sampling
            0.0));                      // fillValue

        in.setFrameBuffer (frameBuffer);
        in.readPixels (dw.min.y, dw.max.y);

    }

    return BitmapData;
}

NAMESPACE_END

#endif // INL_OPENEXR_SUPPORT
