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
#include "NuxCore/Color.h"
#include "ImageSurface.h"
#include "BitmapFormats.h"
#include "NuxCore/Math/MathFunctions.h"

#include "Tga.h"
#include "Bmp.h"
#include "NPng.h"
#include "DDS.h"
#include "RGBE.h"
#include "OpenEXR.h"
#include "NITX.h"
#include "NAnimatedTexture.h"

NAMESPACE_BEGIN

extern PixelFormatInfo GPixelFormats[];

NBitmapData* LoadImageFile(const TCHAR* filename)
{
//    if((filename == 0) || (filename == TEXT("")))
//        return 0;

    if(!GFileManager.FileExist(filename))
    {
        nuxAssertMsg(0, TEXT("[LoadImageFile] Can't find file: %s"), filename);
        return 0;
    }

    NBitmapData* BitmapData = 0;
    BitmapData = read_tga_file(filename);
    if(BitmapData) return BitmapData;
    BitmapData = read_bmp_file(filename);
    if(BitmapData) return BitmapData;
    BitmapData = read_png_rgba(filename);
    if(BitmapData) return BitmapData;
    BitmapData = read_png_rgb(filename);
    if(BitmapData) return BitmapData;
    BitmapData = Load_DDS_File(filename);
    if(BitmapData) return BitmapData;
    BitmapData = LoadRGBE(filename);
    if(BitmapData) return BitmapData;
    BitmapData = LoadAnimatedTextureFile(filename);
    if(BitmapData) return BitmapData;
    BitmapData = LoadITXFile(filename);
    if(BitmapData) return BitmapData;

#ifdef INL_OPENEXR_SUPPORT
    BitmapData = Load_OpenEXR(filename);
    if(BitmapData) return BitmapData;
#endif
    // Unsupported format
    nuxDebugMsg(TEXT("[LoadImageFile] Unknown file format: %s."), filename);
    return 0;
}

bool HasOpenEXRSupport()
{
#ifdef INL_OPENEXR_SUPPORT
    return true;
#else
    return false;
#endif
}

void MakeCheckBoardImage(ImageSurface& Image, t_u32 w, t_u32 h,
                         Color color0,
                         Color color1,
                         t_u32 TileWidth,
                         t_u32 TileHeight)
{
    Image.Allocate(BITFMT_R8G8B8A8, w, h);
    if(TileWidth <= 0)
        TileWidth = 4;

    if(TileHeight <= 0)
        TileHeight = 4;

    t_u32 i, j, c;
    for(j = 0; j < h; j++)
    {
        for(i = 0; i < w; i++)
        {
            /*every 8 bits, change color from black to white or vice versa */

            c = ( ( ( i & TileWidth ) == 0 ) ^ ( ( j & TileHeight )  == 0 ) );
            t_u32 a = ( (( i / TileWidth )%2) == 0 );
            t_u32 b = ( (( j / TileHeight )%2) == 0 );
            c = a ^ b;
            t_u8 R = c ? color0.R()*255     : color1.R()*255;
            t_u8 G = c ? color0.G()*255     : color1.G()*255;
            t_u8 B = c ? color0.B()*255     : color1.B()*255;
            t_u8 A = c ? color0.A()*255     : color1.A()*255;
            Image.Write(i, j, R, G, B, A);
        }
    }
    Image.FlipVertical();
}


NBitmapData::NBitmapData()
:   m_TotalMemorySize(0)
{

}

NBitmapData::~NBitmapData()
{
//     delete [] data_;
}


///////////////////////////////////////////////////////////////////////////////////////
ImageSurface::ImageSurface()
:   width_(0)
,   height_(0)
,   bpe_(0)
,   m_Pitch(0)
,   Alignment_(4)
,   RawData_(0)
,   format_(BITFMT_UNKNOWN)
{
    Allocate(format_, width_, height_);
}

ImageSurface::~ImageSurface()
{
    INL_SAFE_DELETE(RawData_);
}

ImageSurface::ImageSurface(BitmapFormat format, t_u32 width, t_u32 height)
:   width_(0)
,   height_(0)
,   bpe_(0)
,   m_Pitch(0)
,   Alignment_(4)
,   RawData_(0)
,   format_(BITFMT_UNKNOWN)
{
    //MakeCheckBoardImage(*this, width, height, 0xFFBBBBBB, 0xFF444444, 4, 4);
    Allocate(format, width, height);
}

ImageSurface::ImageSurface(const ImageSurface& surface)
{
    width_ = surface.width_;
    height_ = surface.height_;
    bpe_ = surface.bpe_;
    format_ = surface.format_;
    m_Pitch = surface.m_Pitch;
    Alignment_ = surface.Alignment_;

    RawData_ = new t_u8[surface.GetSize()];
    Memcpy(RawData_, surface.RawData_, surface.GetSize());
}

ImageSurface& ImageSurface::operator = (const ImageSurface& surface)
{
    if (this == &surface)
        return *this;   // Handle self assignment

    width_ = surface.width_;
    height_ = surface.height_;
    bpe_ = surface.bpe_;
    format_ = surface.format_;
    m_Pitch = surface.m_Pitch;
    Alignment_ = surface.Alignment_;

    delete [] RawData_;

    RawData_ = new t_u8[surface.GetSize()];
    Memcpy(RawData_, surface.RawData_, surface.GetSize());
    return *this;
}

t_u32 ImageSurface::GetPitch() const
{
    return m_Pitch;
}

t_u32 ImageSurface::GetBlockHeight() const
{
    t_u32 block = GPixelFormats[format_].BlockSizeY;
    t_u32 HeightInBlocks = Align(GetHeight(), block) / block;
    return HeightInBlocks;
}

BitmapFormat ImageSurface::GetFormat() const
{
    return format_;
}

t_u32 ImageSurface::GetAlignment() const
{
    return Alignment_;
}

void ImageSurface::Allocate(BitmapFormat format, t_u32 width, t_u32 height)
{
    nuxAssert(format < BITFMT_END_GFX_FORMATS);
    nuxAssert(width >= 0);
    nuxAssert(height >= 0);

    if(width < 0)
        width = 0;
    if(height < 0)
        height = 0;

    if((format_ == format) &&
        (width_ == width) &&
        (height_ == height))
    {
        // no need to recreate
        Clear();
        return;
    }

    INL_SAFE_DELETE_ARRAY(RawData_);
    if((width == 0) || (height == 0))
    {
        width_ = 0;
        height_ = 0;
        Alignment_ = 1;
        bpe_ = 0;
        format_ = BITFMT_UNKNOWN;
        m_Pitch = 0;
        return;
    }

    width_ = width;
    height_ = height;


    Alignment_ = GPixelFormats[format].RowMemoryAlignment;

    bpe_ = GPixelFormats[format].BlockBytes;
    format_ = format;
    t_u32 HalfAlignment = Log2(Alignment_);
    if((format_ == BITFMT_DXT1) ||
        (format_ == BITFMT_DXT2) ||
        (format_ == BITFMT_DXT3) ||
        (format_ == BITFMT_DXT4) ||
        (format_ == BITFMT_DXT5))
    {
        // For DXT, width and height are rounded up to a multiple of 4 in order to create 4x4 blocks of pixels;
        // And in this context, byte alignment is 1 ie. data is densely packed.
        t_u32 block = GPixelFormats[format].BlockSizeX;
        t_u32 shift = Log2(GPixelFormats[format].BlockSizeX);
        m_Pitch = Align( (bpe_ * ((width_ + (block-1)) >> shift)), Alignment_);

        block = GPixelFormats[format].BlockSizeY;
        shift = Log2(GPixelFormats[format].BlockSizeY);
        RawData_ = new t_u8[m_Pitch * Align((height + (block-1)) >> shift, block)];
    }
    else
    {
        t_u32 block = GPixelFormats[format].BlockSizeX;
        t_u32 shift = Log2(GPixelFormats[format].BlockSizeX);
        m_Pitch = Align( (bpe_ * ((width_ +(block-1)) >> shift)),  Alignment_);

        block = GPixelFormats[format].BlockSizeY;
        shift = Log2(GPixelFormats[format].BlockSizeY);
        RawData_ = new t_u8[m_Pitch * Align((height + (block-1)) >> shift, block)];
    }

    Clear();
}

// This computes the correct pitch of a line. For instance if the unpack alignment is 4, the pitch must have
// a number of pixel multiple of 4.
// See Avoiding 16 Common OpenGL Pitfalls http://www.opengl.org/resources/features/KilgardTechniques/oglpitfall/
// 7. Watch Your Pixel Store Alignment
t_u32 ImageSurface::GetLevelPitch(BitmapFormat format, t_u32 width, t_u32 height, t_u32 miplevel)
{
    t_u32 levelwidth = ImageSurface::GetLevelDim(format, width, miplevel);

    t_u32 bpe = GPixelFormats[format].BlockBytes;
    t_u32 memalignment = GPixelFormats[format].RowMemoryAlignment;
    t_u32 block = GPixelFormats[format].BlockSizeX;
    t_u32 shift = Log2(GPixelFormats[format].BlockSizeX);
    t_u32 pitch = Align( (bpe * ((levelwidth + (block-1)) >> shift)), memalignment);

    return pitch;
}

t_u32 ImageSurface::GetLevelPitchNoMemAlignment(BitmapFormat format, t_u32 width, t_u32 height, t_u32 miplevel)
{
    t_u32 levelwidth = ImageSurface::GetLevelDim(format, width, miplevel);

    t_u32 bpe = GPixelFormats[format].BlockBytes;
    t_u32 block = GPixelFormats[format].BlockSizeX;
    t_u32 shift = Log2(GPixelFormats[format].BlockSizeX);
    t_u32 pitch = Align( (bpe * ((levelwidth + (block-1)) >> shift)), 1);

    return pitch;
}

t_u32 ImageSurface::GetLevelSize(BitmapFormat format, t_u32 width, t_u32 height, t_u32 miplevel)
{
    t_u32 pitch = ImageSurface::GetLevelPitch(format, width, height, miplevel);
    t_u32 levelheight = ImageSurface::GetLevelDim(format, height, miplevel);

    t_u32 block = GPixelFormats[format].BlockSizeY;
    t_u32 shift = Log2(GPixelFormats[format].BlockSizeY);
    t_u32 HeightInBlocks = Align(levelheight, block) / block;

    t_u32 size = pitch * HeightInBlocks;
    return size;
}

t_u32 ImageSurface::GetLevelSize(BitmapFormat format, t_u32 width, t_u32 height, t_u32 depth, t_u32 miplevel)
{
    t_u32 pitch = ImageSurface::GetLevelPitch(format, width, height, miplevel);
    t_u32 levelheight = ImageSurface::GetLevelDim(format, height, miplevel);
    t_u32 leveldepth = ImageSurface::GetLevelDim(format, depth, miplevel);

    t_u32 block = GPixelFormats[format].BlockSizeY;
    t_u32 shift = Log2(GPixelFormats[format].BlockSizeY);
    t_u32 HeightInBlocks = Align(levelheight, block) / block;

    t_u32 size = pitch * HeightInBlocks;
    return leveldepth * size;
}

t_u32 ImageSurface::GetLevelWidth(BitmapFormat format, t_u32 width, t_u32 miplevel)
{
    // return 1 if the mip level does not exist.
    return Max<t_u32>(1, width >> miplevel);
}

t_u32 ImageSurface::GetLevelHeight(BitmapFormat format, t_u32 height, t_u32 miplevel)
{
    // return 1 if the mip level does not exist.
    return Max<t_u32>(1, height >> miplevel);
}

t_u32 ImageSurface::GetLevelDim(BitmapFormat format, t_u32 length, t_u32 miplevel)
{
    // return 1 if the mip level does not exist.
    return Max<t_u32>(1, length >> miplevel);
}

t_u32 ImageSurface::GetNumMipLevel(BitmapFormat format, t_u32 width, t_u32 height)
{
    t_u32 NumTotalMipLevel    = 1 + floorf(Log2(Max(width, height)));
    return NumTotalMipLevel;
}

t_u32 ImageSurface::GetMemAlignment(BitmapFormat format)
{
    return GPixelFormats[format].RowMemoryAlignment;
}

t_u32 ImageSurface::GetLevelBlockWidth(BitmapFormat format, t_u32 width, t_u32 miplevel)
{
    t_u32 block = GPixelFormats[format].BlockSizeX;
    t_u32 WidthInBlocks = Align(GetLevelDim(format, width, miplevel), block) / block;
    return WidthInBlocks;
}

t_u32 ImageSurface::GetLevelBlockHeight(BitmapFormat format, t_u32 height, t_u32 miplevel)
{
    t_u32 block = GPixelFormats[format].BlockSizeY;
    t_u32 HeightInBlocks = Align(GetLevelDim(format, height, miplevel), block) / block;
    return HeightInBlocks;
}

bool ImageSurface::IsNull() const
{
    if((width_ == 0) || (height_ == 0) || (format_ == BITFMT_UNKNOWN))
        return true;
    return false;
}

void ImageSurface::Write32b(t_u32 i, t_u32 j, t_u32 value)
{
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ >= 4);

    if((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
        return;
    RawData_[j * m_Pitch + i * bpe_ + 0] = (t_u8) (value & 0xff);
    RawData_[j * m_Pitch + i * bpe_ + 1] = (t_u8) ((value & 0xff00) >> 8);
    RawData_[j * m_Pitch + i * bpe_ + 2] = (t_u8) ((value & 0xff0000) >> 16);
    RawData_[j * m_Pitch + i * bpe_ + 3] = (t_u8) ((value & 0xff000000) >> 24);
}

void ImageSurface::Write24b(t_u32 i, t_u32 j, t_u32 value)
{
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ >= 3);

    if((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
        return;
    RawData_[j * m_Pitch + i * bpe_ + 0] = (t_u8) (value & 0xff);
    RawData_[j * m_Pitch + i * bpe_ + 1] = (t_u8) ((value & 0xff00) >> 8);
    RawData_[j * m_Pitch + i * bpe_ + 2] = (t_u8) ((value & 0xff0000) >> 16);
}

void ImageSurface::Write16b(t_u32 i, t_u32 j, t_u16 value)
{
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ >= 2);

    if((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
        return;
    RawData_[j * m_Pitch + i * bpe_ + 0] = (t_u8) (value & 0xff);
    RawData_[j * m_Pitch + i * bpe_ + 1] = (t_u8) ((value & 0xff00) >> 8);
}

void ImageSurface::Write8b(t_u32 i, t_u32 j, t_u8 value)
{
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ >= 1);

    if((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
        return;
    RawData_[j * m_Pitch + i * bpe_ + 0] = (t_u8) (value & 0xff);
}

void ImageSurface::Write(t_u32 i, t_u32 j, t_u8 r, t_u8 g, t_u8 b, t_u8 a)
{
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ == 4);

    if((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
        return;
    if((i < 0) || (i > width_))
        return;

    if((j < 0) || (j > height_))
        return;

    if(bpe_ != 4)
        return;

    RawData_[j * m_Pitch + i * bpe_ + 0] = r;
    RawData_[j * m_Pitch + i * bpe_ + 1] = g;
    RawData_[j * m_Pitch + i * bpe_ + 2] = b;
    RawData_[j * m_Pitch + i * bpe_ + 3] = a;
}

t_u32 ImageSurface::Read(t_u32 i, t_u32 j)
{
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_);

    if((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
        return 0x00000000;

    if(bpe_ == 4)
    {
        return  ((t_u32)RawData_[j * m_Pitch + i * bpe_ + 3] << 24)   |
            ((t_u32)RawData_[j * m_Pitch + i * bpe_ + 2] << 16)   |
            ((t_u32)RawData_[j * m_Pitch + i * bpe_ + 1] << 8)    |
            ((t_u32)RawData_[j * m_Pitch + i * bpe_ + 0] << 0);      
    }

    if(bpe_ == 3)
    {
        return  ((t_u32)RawData_[j * m_Pitch + i * bpe_ + 2] << 16)   |
            ((t_u32)RawData_[j * m_Pitch + i * bpe_ + 1] << 8)    |
            ((t_u32)RawData_[j * m_Pitch + i * bpe_ + 0] << 0);        
    }

    if(bpe_ == 2)
    {
        return  ((t_u32)RawData_[j * m_Pitch + i * bpe_ + 1] << 8)    |
            ((t_u32)RawData_[j * m_Pitch + i * bpe_ + 0] << 0);        
    }

    if(bpe_ == 1)
    {
        return  (t_u32)RawData_[j * m_Pitch + i * bpe_ + 0];
    }

    return 0x0000000;
}

void ImageSurface::Clear()
{
    t_u32 i;

    if(RawData_ == 0)
        return;

    if((width_ == 0) || (height_ == 0))
        return;

    for(i = 0; i < GetSize(); i++)
    {
        RawData_[i] = 0;
    }
}

void ImageSurface::FlipHorizontal()
{
    if((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
        return;
    t_u32 i, j, k;
    t_u8 *flip_data;

    if(RawData_ == 0)
        return;

    if(width_ == 0 || height_ == 0)
        return;

    flip_data =  new t_u8[m_Pitch*height_];

    for(j = 0; j < height_; j++)
    {
        for(i = 0; i < width_; i++)
        {     
            for(k = 0; k < bpe_; k++)
            {
                flip_data[(j*m_Pitch) + i * bpe_ + k] = RawData_[(j*m_Pitch) + (width_-i-1) * bpe_ + k];
            }
        }
    }

    delete [] RawData_;
    RawData_ = flip_data;
}

void ImageSurface::FlipVertical()
{

    t_u32 i, j, k;
    t_u8 *flip_data;

    if(RawData_ == 0)
        return;

    if(width_ == 0 || height_ == 0)
        return;

    if((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2) || (format_ == BITFMT_DXT3) || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
    {
        FlipDXTVertical();
//        t_u32 h = (height_ + 3) >> 2;
//        flip_data =  new t_u8[m_Pitch*h];
//        for(j = 0; j < h; j++)
//        {
//            Memcpy(flip_data + (h-j-1)*m_Pitch, RawData_+j*m_Pitch, m_Pitch);
//        }
    }
    else
    {
        flip_data =  new t_u8[m_Pitch*height_];
        for(j = 0; j < height_; j++)
        {
            for(i = 0; i < width_; i++)
            {
                for(k = 0; k < bpe_; k++)
                {
                    flip_data[(j*m_Pitch) + i * bpe_ + k] = RawData_[(height_-j-1)*m_Pitch + i * bpe_ + k];
                }
            }
        }
        delete [] RawData_;
        RawData_ = flip_data;
    }

}

void ImageSurface::FlipDXTVertical()
{
    //void (CDDSImage::*flipblocks)(DXTColBlock*, t_u32);
    t_u32 xblocks = (width_+3) / 4;
    t_u32 yblocks = (height_+3) / 4;
    t_u32 blocksize;
    t_u32 linesize;

    switch (format_)
    {
    case BITFMT_DXT1: 
        blocksize = 8;
        //flipblocks = &CDDSImage::flip_blocks_dxtc1; 
        break;
    case BITFMT_DXT2: 
        blocksize = 16;
        //flipblocks = &CDDSImage::flip_blocks_dxtc2; 
        break;
    case BITFMT_DXT3: 
        blocksize = 16;
        //flipblocks = &CDDSImage::flip_blocks_dxtc3; 
        break;
    case BITFMT_DXT4: 
        blocksize = 16;
        //flipblocks = &CDDSImage::flip_blocks_dxtc4; 
        break;
    case BITFMT_DXT5: 
        blocksize = 16;
        //flipblocks = &CDDSImage::flip_blocks_dxtc5; 
        break;
    default:
        return;
    }

    linesize = xblocks * blocksize;

    DXTColBlock *top;
    DXTColBlock *bottom;

    for (t_u32 j = 0; j < (yblocks >> 1); j++)
    {
        top = (DXTColBlock*)((unsigned char*)RawData_+ j * linesize);
        bottom = (DXTColBlock*)((unsigned char*)RawData_ + (((yblocks-j)-1) * linesize));

        switch (format_)
        {
        case BITFMT_DXT1:
            FlipBlocksDXT1(top, xblocks);
            FlipBlocksDXT1(bottom, xblocks);
            break;
        case BITFMT_DXT2:
            FlipBlocksDXT3(top, xblocks);
            FlipBlocksDXT3(bottom, xblocks);
            break;
        case BITFMT_DXT3:
            FlipBlocksDXT3(top, xblocks);
            FlipBlocksDXT3(bottom, xblocks);
            break;
        case BITFMT_DXT4:
            FlipBlocksDXT5(top, xblocks);
            FlipBlocksDXT5(bottom, xblocks);
            break;
        case BITFMT_DXT5:
            FlipBlocksDXT5(top, xblocks);
            FlipBlocksDXT5(bottom, xblocks);
            break;
        }
        SwapBlocks(bottom, top, linesize);
    }
}

void ImageSurface::SwapBlocks(void *byte1, void *byte2, t_u32 size)
{
    unsigned char *tmp = new unsigned char[size];

    memcpy(tmp, byte1, size);
    memcpy(byte1, byte2, size);
    memcpy(byte2, tmp, size);

    delete [] tmp;
}

void ImageSurface::FlipBlocksDXT1(DXTColBlock *line, t_u32 numBlocks)
{
    DXTColBlock *curblock = line;

    for (t_u32 i = 0; i < numBlocks; i++)
    {
        SwapBlocks(&curblock->row[0], &curblock->row[3], sizeof(unsigned char));
        SwapBlocks(&curblock->row[1], &curblock->row[2], sizeof(unsigned char));

        curblock++;
    }
}

void ImageSurface::FlipBlocksDXT3(DXTColBlock *line, t_u32 numBlocks)
{
    DXTColBlock *curblock = line;
    DXT3AlphaBlock *alphablock;

    for (t_u32 i = 0; i < numBlocks; i++)
    {
        alphablock = (DXT3AlphaBlock*)curblock;

        SwapBlocks(&alphablock->row[0], &alphablock->row[3], sizeof(unsigned short));
        SwapBlocks(&alphablock->row[1], &alphablock->row[2], sizeof(unsigned short));

        curblock++;

        SwapBlocks(&curblock->row[0], &curblock->row[3], sizeof(unsigned char));
        SwapBlocks(&curblock->row[1], &curblock->row[2], sizeof(unsigned char));

        curblock++;
    }
}

void ImageSurface::FlipBlocksDXT5(DXTColBlock *line, t_u32 numBlocks)
{
    DXTColBlock *curblock = line;
    DXT5AlphaBlock *alphablock;

    for (t_u32 i = 0; i < numBlocks; i++)
    {
        alphablock = (DXT5AlphaBlock*)curblock;

        FlipDXT5Alpha(alphablock);

        curblock++;

        SwapBlocks(&curblock->row[0], &curblock->row[3], sizeof(unsigned char));
        SwapBlocks(&curblock->row[1], &curblock->row[2], sizeof(unsigned char));

        curblock++;
    }
}

void ImageSurface::FlipDXT5Alpha(DXT5AlphaBlock *block)
{
    unsigned char gBits[4][4];

    const unsigned long mask = 0x00000007;          // bits = 00 00 01 11
    unsigned long bits = 0;
    memcpy(&bits, &block->row[0], sizeof(unsigned char) * 3);

    gBits[0][0] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[0][1] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[0][2] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[0][3] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[1][0] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[1][1] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[1][2] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[1][3] = (unsigned char)(bits & mask);

    bits = 0;
    memcpy(&bits, &block->row[3], sizeof(unsigned char) * 3);

    gBits[2][0] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[2][1] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[2][2] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[2][3] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[3][0] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[3][1] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[3][2] = (unsigned char)(bits & mask);
    bits >>= 3;
    gBits[3][3] = (unsigned char)(bits & mask);

    unsigned long *pBits = ((unsigned long*) &(block->row[0]));

    *pBits = *pBits | (gBits[3][0] << 0);
    *pBits = *pBits | (gBits[3][1] << 3);
    *pBits = *pBits | (gBits[3][2] << 6);
    *pBits = *pBits | (gBits[3][3] << 9);

    *pBits = *pBits | (gBits[2][0] << 12);
    *pBits = *pBits | (gBits[2][1] << 15);
    *pBits = *pBits | (gBits[2][2] << 18);
    *pBits = *pBits | (gBits[2][3] << 21);

    pBits = ((unsigned long*) &(block->row[3]));

#ifdef __APPLE__
    *pBits &= 0x000000ff;
#else
    *pBits &= 0xff000000;
#endif

    *pBits = *pBits | (gBits[1][0] << 0);
    *pBits = *pBits | (gBits[1][1] << 3);
    *pBits = *pBits | (gBits[1][2] << 6);
    *pBits = *pBits | (gBits[1][3] << 9);

    *pBits = *pBits | (gBits[0][0] << 12);
    *pBits = *pBits | (gBits[0][1] << 15);
    *pBits = *pBits | (gBits[0][2] << 18);
    *pBits = *pBits | (gBits[0][3] << 21);
}

const t_u8* ImageSurface::GetPtrRawData() const
{
    return RawData_;
}

t_u8* ImageSurface::GetPtrRawData()
{
    return RawData_;
}

t_u32 ImageSurface::GetSize() const
{
    if((format_ == BITFMT_DXT1) ||
        (format_ == BITFMT_DXT2) ||
        (format_ == BITFMT_DXT3) ||
        (format_ == BITFMT_DXT4) ||
        (format_ == BITFMT_DXT5))
    {
        return m_Pitch * ((height_ + 3) >> 2);
    }
    else
    {
        return m_Pitch * height_;
    }
}

Color ImageSurface::AverageColor()
{
    if(width_ == 0 || height_ == 0)
        return Color(0.f, 0.f, 0.f, 0.f);
    
    t_float r, g, b, a;
    r = g = b = a = 0;
    if(bpe_ == 8)
    {
        for(int j = 0; j < height_; j++)
        {
            for(int i = 0; i < width_; i++)
            {
                t_u32 v = Read(i, j);
                r += (v & 0x000000FF);
                g += (v & 0x0000FF00) >> 1;
                b += (v & 0x00FF0000) >> 2;
                a += (v & 0xFF000000) >> 3;
            }
        }
    }
    t_u32 num_pixels = width_ * height_;
    return Color(r/num_pixels, g/num_pixels, b/num_pixels, a/num_pixels);
}

///////////////////////////////////////////////////////////////////
NTextureData::NTextureData(BitmapFormat f, t_u32 width, t_u32 height, t_u32 NumMipmap)
:   m_NumMipmap(0)
{
    Allocate(f, width, height, NumMipmap);
}

NTextureData::~NTextureData()
{
    ClearData();
}

void NTextureData::ClearData()
{
    for(t_u32 i = 0; i < m_MipSurfaceArray.size(); i++)
        delete m_MipSurfaceArray[i];
    m_MipSurfaceArray.clear();
}

//! Copy constructor
NTextureData::NTextureData(const NTextureData& object)
{
    for(t_u32 i = 0; i < object.GetNumMipmap(); i++)
        m_MipSurfaceArray.push_back(new ImageSurface(object.GetSurface(i)));
}

//! Assignment constructor
NTextureData& NTextureData::operator = (const NTextureData& copy)
{
    ClearData();
    m_NumMipmap = copy.m_NumMipmap;
    m_TotalMemorySize = copy.m_TotalMemorySize;
    for(t_u32 i = 0; i < copy.GetNumMipmap(); i++)
        m_MipSurfaceArray.push_back(new ImageSurface(copy.GetSurface(i)));
    return *this;
}

void NTextureData::Allocate(BitmapFormat format, t_u32 width, t_u32 height, t_u32 NumMipmapRequested)
{
    nuxAssertMsg(width>=0, TEXT("[NTextureData::Allocate] Error: Negative texture width."));
    nuxAssertMsg(height>=0, TEXT("[NTextureData::Allocate] Error: Negative texture height."));
    nuxAssert(NumMipmapRequested >= 0);

    t_u32 NumTotalMipLevel    = 1 + (t_u32)Floor(Log2(Max(width, height)));
    m_NumMipmap = NumMipmapRequested;
    if(NumMipmapRequested == 0)
        m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;
    if(NumMipmapRequested > NumTotalMipLevel)
        m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    m_TotalMemorySize = 0;
    ClearData();
    for(t_u32 i = 0; i < m_NumMipmap; i++)
    {
        t_u32 w = width >> i;
        t_u32 h = height >> i;
        m_MipSurfaceArray.push_back(new ImageSurface(format, w, h));
        m_TotalMemorySize += m_MipSurfaceArray[i]->GetSize();
    }
}

void NTextureData::AllocateCheckBoardTexture(t_u32 width, t_u32 height, t_u32 NumMipmap, Color color0, Color color1, t_u32 TileWidth, t_u32 TileHeight)
{
    Allocate(BITFMT_R8G8B8A8, width, height, NumMipmap);
    for(t_u32 i = 0; i < m_NumMipmap; i++)
    {
        t_u32 w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), i);
        t_u32 h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), i);
        MakeCheckBoardImage(*m_MipSurfaceArray[i], w, h, color0, color1, TileWidth, TileHeight);
    }
}

void NTextureData::AllocateColorTexture(t_u32 width, t_u32 height, t_u32 NumMipmap, Color color0)
{
    Allocate(BITFMT_R8G8B8A8, width, height, NumMipmap);
    for(t_u32 i = 0; i < m_NumMipmap; i++)
    {
        t_u32 w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), i);
        t_u32 h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), i);
        MakeCheckBoardImage(*m_MipSurfaceArray[i], w, h, color0, color0);
    }
}

t_u32 NTextureData::GetNumMipmap() const
{
    return m_MipSurfaceArray.size();
}

bool NTextureData::SetSurface(t_u32 MipLevel, const ImageSurface& targetsurface)
{
    nuxAssert(MipLevel >= 0);
    nuxAssert(MipLevel < m_NumMipmap);

    ImageSurface& surface = GetSurface(MipLevel);
    if(surface.GetFormat() != targetsurface.GetFormat())
        return false;
    if(surface.GetWidth() != targetsurface.GetWidth())
        return false;
    if(surface.GetHeight() != targetsurface.GetHeight())
        return false;

    surface = targetsurface;
    return true;
}

///////////////////////////////////////////////////////////////////
NCubemapData::NCubemapData(BitmapFormat format, t_u32 width, t_u32 height, t_u32 NumMipmap)
:   m_NumMipmap(0)
{
    Allocate(format, width, height, NumMipmap);
}

NCubemapData::~NCubemapData()
{
    ClearData();
}

void NCubemapData::ClearData()
{
    t_u32 n = m_MipSurfaceArray[0].size();
    for(t_u32 i = 0; i < n; i++)
    {
        delete m_MipSurfaceArray[0][i];
        delete m_MipSurfaceArray[1][i];
        delete m_MipSurfaceArray[2][i];
        delete m_MipSurfaceArray[3][i];
        delete m_MipSurfaceArray[4][i];
        delete m_MipSurfaceArray[5][i];
    }
    m_MipSurfaceArray[0].clear();
    m_MipSurfaceArray[1].clear();
    m_MipSurfaceArray[2].clear();
    m_MipSurfaceArray[3].clear();
    m_MipSurfaceArray[4].clear();
    m_MipSurfaceArray[5].clear();
}

//! Copy constructor
NCubemapData::NCubemapData(const NCubemapData& object)
{
    for(t_u32 face = 0; face < 6; face++)
        for(t_u32 i = 0; i < object.GetNumMipmap(); i++)
            m_MipSurfaceArray[face].push_back(new ImageSurface(object.GetSurface(face, i)));
}

//! Assignment constructor
NCubemapData& NCubemapData::operator = (const NCubemapData& copy)
{
    ClearData();
    m_NumMipmap = copy.m_NumMipmap;
    m_TotalMemorySize = copy.m_TotalMemorySize;
    for(t_u32 face = 0; face < 6; face++)
        for(t_u32 i = 0; i < copy.GetNumMipmap(); i++)
            m_MipSurfaceArray[face].push_back(new ImageSurface(copy.GetSurface(face, i)));
    return *this;
}

void NCubemapData::Allocate(BitmapFormat format, t_u32 width, t_u32 height, t_u32 NumMipmapRequested)
{
    nuxAssertMsg(width>=0, TEXT("[NCubemapData::Allocate] Error: Negative texture width."));
    nuxAssertMsg(height>=0, TEXT("[NCubemapData::Allocate] Error: Negative texture height."));
    nuxAssert(NumMipmapRequested >= 0);

    ClearData();
    
    t_u32 NumTotalMipLevel    = 1 + (t_u32)Floor(Log2(Max(width, height)));
    m_NumMipmap = NumMipmapRequested;
    if(NumMipmapRequested == 0)
        m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;
    if(NumMipmapRequested > NumTotalMipLevel)
        m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    m_TotalMemorySize = 0;
    for(t_u32 face = 0; face < 6; face++)
    {
        for(t_u32 i = 0; i < m_NumMipmap; i++)
        {
            t_u32 w = width >> i;
            t_u32 h = height >> i;
            m_MipSurfaceArray[face].push_back(new ImageSurface(format, w, h));
            m_TotalMemorySize += m_MipSurfaceArray[face][i]->GetSize();
        }
    }
}

void NCubemapData::AllocateCheckBoardTexture(t_u32 width, t_u32 height, t_u32 NumMipmap, Color color0, Color color1, t_u32 TileWidth, t_u32 TileHeight)
{
    Allocate(BITFMT_R8G8B8A8, width, height, NumMipmap);
    for(t_u32 face = 0; face < 6; face++)
    {
        for(t_u32 i = 0; i < m_NumMipmap; i++)
        {
            t_u32 w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), i);
            t_u32 h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), i);
            MakeCheckBoardImage(*m_MipSurfaceArray[face][i], w, h, color0, color1, TileWidth, TileHeight);
        }
    }
}

void NCubemapData::AllocateColorTexture(t_u32 width, t_u32 height, t_u32 NumMipmap, Color color0)
{
    Allocate(BITFMT_R8G8B8A8, width, height, NumMipmap);
    for(t_u32 face = 0; face < 6; face++)
    {
        for(t_u32 i = 0; i < m_NumMipmap; i++)
        {
            t_u32 w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), i);
            t_u32 h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), i);
            MakeCheckBoardImage(*m_MipSurfaceArray[face][i], w, h, color0, color0);
        }
    }
}

t_u32 NCubemapData::GetNumMipmap() const
{
    return m_MipSurfaceArray[0].size();
}

bool NCubemapData::SetSurface(t_u32 face, t_u32 MipLevel, const ImageSurface& targetsurface)
{
    nuxAssert(face >= 0);
    nuxAssert(face < 6);
    nuxAssert(MipLevel >= 0);
    nuxAssert(MipLevel < m_NumMipmap);

    ImageSurface& surface = GetSurface(face, MipLevel);
    if(surface.GetFormat() != targetsurface.GetFormat())
        return false;
    if(surface.GetWidth() != targetsurface.GetWidth())
        return false;
    if(surface.GetHeight() != targetsurface.GetHeight())
        return false;

    surface = targetsurface;
    return true;
}

///////////////////////////////////////////////////////////////////
NVolumeData::NVolumeData(BitmapFormat format, t_u32 width, t_u32 height, t_u32 depth, t_u32 NumMipmap)
:   m_Depth(0)
,   m_NumMipmap(0)
,   m_MipSurfaceArray(0)
{
    Allocate(format, width, height, depth, NumMipmap);
}

NVolumeData::~NVolumeData()
{
    ClearData();
}

void NVolumeData::ClearData()
{
    for(t_u32 mip = 0; mip < GetNumMipmap(); mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(GetFormat(), GetDepth(), mip); s++)
        {
            delete m_MipSurfaceArray[mip][s];
        }
        m_MipSurfaceArray[mip].clear();
    }
    INL_SAFE_DELETE_ARRAY(m_MipSurfaceArray);
}

//! Copy constructor
NVolumeData::NVolumeData(const NVolumeData& object)
{
    for(t_u32 mip = 0; mip < object.GetNumMipmap(); mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(object.GetFormat(), object.GetDepth(), mip); s++)
        {
            m_MipSurfaceArray[mip].push_back(new ImageSurface(object.GetSurface(mip, s)));
        }
    }
}

//! Assignment constructor
NVolumeData& NVolumeData::operator = (const NVolumeData& copy)
{
    ClearData();
    m_Depth = copy.m_Depth;
    m_NumMipmap = copy.m_NumMipmap;
    m_TotalMemorySize = copy.m_TotalMemorySize;

    m_MipSurfaceArray = new std::vector<ImageSurface*>[m_NumMipmap];
    for(t_u32 mip = 0; mip < copy.GetNumMipmap(); mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(copy.GetFormat(), copy.GetDepth(), mip); s++)
        {
            m_MipSurfaceArray[mip].push_back(new ImageSurface(copy.GetSurface(mip, s)));
        }
    }
    return *this;
}

void NVolumeData::Allocate(BitmapFormat format, t_u32 width, t_u32 height, t_u32 depth, t_u32 NumMipmapRequested)
{
    nuxAssertMsg(depth>=0, TEXT("[NVolumeData::Allocate] Error: Negative number of slice."));
    nuxAssertMsg(width>=0, TEXT("[NVolumeData::Allocate] Error: Negative texture width."));
    nuxAssertMsg(height>=0, TEXT("[NVolumeData::Allocate] Error: Negative texture height."));
    nuxAssert(NumMipmapRequested >= 0);

    ClearData();

    t_u32 NumTotalMipLevel    = 1 + (t_u32)Floor(Log2(Max(width, height)));
    m_NumMipmap = NumMipmapRequested;
    if(NumMipmapRequested == 0)
        m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;
    if(NumMipmapRequested > NumTotalMipLevel)
        m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    m_Depth = depth;
    
    m_MipSurfaceArray = new std::vector<ImageSurface*>[m_NumMipmap];
    m_TotalMemorySize = 0;

    for(t_u32 mip = 0; mip < m_NumMipmap; mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(format, depth, mip); s++)
        {
            t_u32 w = ImageSurface::GetLevelDim(format, width, mip);
            t_u32 h = ImageSurface::GetLevelDim(format, height, mip);
            m_MipSurfaceArray[mip].push_back(new ImageSurface(format, w, h));
            m_TotalMemorySize += m_MipSurfaceArray[mip][s]->GetSize();
        }
    }
}

void NVolumeData::AllocateCheckBoardTexture(t_u32 width, t_u32 height, t_u32 slice, t_u32 NumMipmap, Color color0, Color color1, t_u32 TileWidth, t_u32 TileHeight)
{
    Allocate(BITFMT_R8G8B8A8, width, height, slice, NumMipmap);
    for(t_u32 mip = 0; mip < m_NumMipmap; mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetDepth(), mip); s++)
        {
            t_u32 w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), mip);
            t_u32 h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), mip);
            MakeCheckBoardImage(*(m_MipSurfaceArray[mip][s]), w, h, color0, color1, TileWidth, TileHeight);
        }
    }
}

void NVolumeData::AllocateColorTexture(t_u32 width, t_u32 height, t_u32 slice, t_u32 NumMipmap, Color color0)
{
    Allocate(BITFMT_R8G8B8A8, width, height, slice, NumMipmap);
    for(t_u32 mip = 0; mip < m_NumMipmap; mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetDepth(), mip); s++)
        {
            t_u32 w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), mip);
            t_u32 h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), mip);
            MakeCheckBoardImage(*(m_MipSurfaceArray[mip][s]), w, h, color0, color0);
        }
    }
}

t_u32 NVolumeData::GetNumMipmap() const
{
    return m_NumMipmap;
}

bool NVolumeData::SetSurface(t_u32 Slice, t_u32 MipLevel, const ImageSurface& targetsurface)
{
    nuxAssert(Slice >= 0);
    nuxAssert(Slice < m_Depth);
    nuxAssert(MipLevel >= 0);
    nuxAssert(MipLevel < m_NumMipmap);

    ImageSurface& surface = GetSurface(Slice, MipLevel);
    if(surface.GetFormat() != targetsurface.GetFormat())
        return false;
    if(surface.GetWidth() != targetsurface.GetWidth())
        return false;
    if(surface.GetHeight() != targetsurface.GetHeight())
        return false;

    surface = targetsurface;
    return true;
}

///////////////////////////////////////////////////////////////////
NAnimatedTextureData::NAnimatedTextureData(BitmapFormat format, t_u32 width, t_u32 height, t_u32 depth)
:   m_Depth(0)
,   m_NumMipmap(0)
,   m_MipSurfaceArray(0)
{
    Allocate(format, width, height, depth, 1);
}

NAnimatedTextureData::~NAnimatedTextureData()
{
    ClearData();
}

void NAnimatedTextureData::ClearData()
{
    for(t_u32 mip = 0; mip < GetNumMipmap(); mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(GetFormat(), GetDepth(), mip); s++)
        {
            delete m_MipSurfaceArray[mip][s];
        }
        m_MipSurfaceArray[mip].clear();
    }
    m_FrameTimeArray.clear();
    INL_SAFE_DELETE_ARRAY(m_MipSurfaceArray);
}

//! Copy constructor
NAnimatedTextureData::NAnimatedTextureData(const NAnimatedTextureData& object)
{
    for(t_u32 mip = 0; mip < object.GetNumMipmap(); mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(object.GetFormat(), object.GetDepth(), mip); s++)
        {
            m_MipSurfaceArray[mip].push_back(new ImageSurface(object.GetSurface(mip, s)));
        }
    }

    for(t_u32 frame = 0; frame < object.GetDepth(); frame++)
    {
        m_FrameTimeArray.push_back(object.GetFrameTime(frame));
    }
}

//! Assignment constructor
NAnimatedTextureData& NAnimatedTextureData::operator = (const NAnimatedTextureData& copy)
{
    ClearData();
    m_Depth = copy.GetDepth();
    m_NumMipmap = copy.m_NumMipmap;
    m_TotalMemorySize = copy.m_TotalMemorySize;

    m_MipSurfaceArray = new std::vector<ImageSurface*>[m_NumMipmap];
    for(t_u32 mip = 0; mip < copy.GetNumMipmap(); mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(copy.GetFormat(), copy.GetDepth(), mip); s++)
        {
            m_MipSurfaceArray[mip].push_back(new ImageSurface(copy.GetSurface(s)));
        }
    }

    for(t_u32 frame = 0; frame < copy.GetDepth(); frame++)
    {
        m_FrameTimeArray.push_back(copy.GetFrameTime(frame));
    }

    return *this;
}

void NAnimatedTextureData::Allocate(BitmapFormat format, t_u32 width, t_u32 height, t_u32 depth, t_u32 NumMipmapRequested)
{
    nuxAssertMsg(depth>=0, TEXT("[NAnimatedTextureData::Allocate] Error: Negative number of slice."));
    nuxAssertMsg(width>=0, TEXT("[NAnimatedTextureData::Allocate] Error: Negative texture width."));
    nuxAssertMsg(height>=0, TEXT("[NAnimatedTextureData::Allocate] Error: Negative texture height."));
    nuxAssert(NumMipmapRequested >= 0);

    ClearData();

    t_u32 NumTotalMipLevel    = 1 + (t_u32)Floor(Log2(Max(width, height)));
    m_NumMipmap = NumMipmapRequested;
    if(NumMipmapRequested == 0)
        m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;
    if(NumMipmapRequested > NumTotalMipLevel)
        m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    m_Depth = depth;

    m_MipSurfaceArray = new std::vector<ImageSurface*>[m_NumMipmap];
    m_TotalMemorySize = 0;

    for(t_u32 mip = 0; mip < m_NumMipmap; mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(format, depth, mip); s++)
        {
            t_u32 w = ImageSurface::GetLevelDim(format, width, mip);
            t_u32 h = ImageSurface::GetLevelDim(format, height, mip);
            m_MipSurfaceArray[mip].push_back(new ImageSurface(format, w, h));
            m_TotalMemorySize += m_MipSurfaceArray[mip][s]->GetSize();
        }
    }
}

void NAnimatedTextureData::AllocateCheckBoardTexture(t_u32 width, t_u32 height, t_u32 slice, t_u32 NumMipmap, Color color0, Color color1, t_u32 TileWidth, t_u32 TileHeight)
{
    Allocate(BITFMT_R8G8B8A8, width, height, slice, NumMipmap);
    for(t_u32 mip = 0; mip < m_NumMipmap; mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetDepth(), mip); s++)
        {
            t_u32 w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), mip);
            t_u32 h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), mip);
            MakeCheckBoardImage(*(m_MipSurfaceArray[mip][s]), w, h, color0, color1, TileWidth, TileHeight);
        }
    }
}

void NAnimatedTextureData::AllocateColorTexture(t_u32 width, t_u32 height, t_u32 slice, t_u32 NumMipmap, Color color0)
{
    Allocate(BITFMT_R8G8B8A8, width, height, slice, NumMipmap);
    for(t_u32 mip = 0; mip < m_NumMipmap; mip++)
    {
        for(t_u32 s = 0; s < ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetDepth(), mip); s++)
        {
            t_u32 w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), mip);
            t_u32 h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), mip);
            MakeCheckBoardImage(*(m_MipSurfaceArray[mip][s]), w, h, color0, color0);
        }
    }
}

t_u32 NAnimatedTextureData::GetNumMipmap() const
{
    return m_NumMipmap;
}

bool NAnimatedTextureData::SetSurface(t_u32 Slice, t_u32 MipLevel, const ImageSurface& targetsurface)
{
    nuxAssert(Slice >= 0);
    nuxAssert(Slice < m_Depth);
    nuxAssert(MipLevel >= 0);
    nuxAssert(MipLevel < m_NumMipmap);

    ImageSurface& surface = GetSurface(Slice, MipLevel);
    if(surface.GetFormat() != targetsurface.GetFormat())
        return false;
    if(surface.GetWidth() != targetsurface.GetWidth())
        return false;
    if(surface.GetHeight() != targetsurface.GetHeight())
        return false;

    surface = targetsurface;
    return true;
}

NAMESPACE_END

