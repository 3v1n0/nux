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
#include "NuxCore/Logger.h"
#include "NuxCore/Math/MathFunctions.h"

#include "BitmapFormats.h"
#include "GdkGraphics.h"

#if defined(NUX_OS_WINDOWS)
  #include "GdiImageLoader.h"
#endif

#include "ImageSurface.h"

namespace nux
{
DECLARE_LOGGER(logger, "nux.image");

  extern PixelFormatInfo GPixelFormats[];

  NBitmapData *LoadGdkPixbuf(GdkPixbuf *pixbuf)
  {
    NUX_RETURN_VALUE_IF_NULL(pixbuf, nullptr);
    unsigned int width = gdk_pixbuf_get_width(pixbuf);
    unsigned int height = gdk_pixbuf_get_height(pixbuf);
    unsigned int row_bytes = gdk_pixbuf_get_rowstride(pixbuf);

    NTextureData *Texture = new NTextureData(BITFMT_R8G8B8A8, width, height, 1);

    guchar *img = gdk_pixbuf_get_pixels(pixbuf);
    auto& surface = Texture->GetSurface(0);

    for (unsigned int i = 0; i < width; ++i)
    {
      for (unsigned int j = 0; j < height; ++j)
      {
        guchar *pixels = img + ( j * row_bytes + i * 4);
        UINT value =
          (* (pixels + 3) << 24) |  // a
          (* (pixels + 2) << 16) |  // b
          (* (pixels + 1) << 8)  |  // g
          * (pixels + 0);           // r

        surface.Write32b(i, j, value);
      }
    }

    return Texture;
  }

  NBitmapData *LoadImageFile(const TCHAR *filename)
  {
    if (!GFileManager.FileExist(filename))
    {
      nuxAssertMsg(0, "[LoadImageFile] Can't find file: %s", filename);
      return 0;
    }

    NBitmapData *BitmapData = 0;

#if defined(NUX_OS_WINDOWS)
    BitmapData = GdiLoadImageFile(filename);
    if (BitmapData) return BitmapData;
#elif defined(NUX_OS_LINUX)
    GdkGraphics gdkgraphics;
    gdkgraphics.LoadImage(filename);
    BitmapData = gdkgraphics.GetBitmap();
    if (BitmapData) return BitmapData;
#endif

    // Unsupported format
    LOG_DEBUG(logger) << "Unknown file format: " << filename;
    return 0;
  }

  bool HasOpenEXRSupport()
  {
#ifdef NUX_OPENEXR_SUPPORT
    return true;
#else
    return false;
#endif
  }

  void MakeCheckBoardImage(ImageSurface &Image,
                            int width, int height,
                            Color const& dark,
                            Color const& light,
                            int TileWidth,
                            int TileHeight)
  {
    Image.Allocate(BITFMT_R8G8B8A8, width, height);

    if (TileWidth <= 0)
      TileWidth = 4;

    if (TileHeight <= 0)
      TileHeight = 4;

    for (int j = 0; j < height; ++j)
    {
      for (int i = 0; i < width; ++i)
      {
        /*every 8 bits, change color from black to white or vice versa */
        bool even_column = ((i / TileWidth) % 2) == 0;
        bool even_row = ((j / TileHeight ) % 2) == 0;
        bool is_dark = even_column ^ even_row;
        Color const& c = is_dark ? dark : light;
        Image.Write(i, j,
                    c.red * 255,
                    c.green * 255,
                    c.blue * 255,
                    c.alpha * 255);
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
  }


  ImageSurface::ImageSurface()
    :   width_(0)
    ,   height_(0)
    ,   format_(BITFMT_UNKNOWN)
    ,   m_Pitch(0)
    ,   bpe_(0)
    ,   Alignment_(4)
  {
    Allocate(format_, width_, height_);
  }

  ImageSurface::ImageSurface(BitmapFormat format, unsigned int width, unsigned int height)
    :   width_(0)
    ,   height_(0)
    ,   format_(BITFMT_UNKNOWN)
    ,   m_Pitch(0)
    ,   bpe_(0)
    ,   Alignment_(4)
  {
    Allocate(format, width, height);
  }

  ImageSurface::ImageSurface(ImageSurface const& surface)
  {
    width_ = surface.width_;
    height_ = surface.height_;
    bpe_ = surface.bpe_;
    format_ = surface.format_;
    m_Pitch = surface.m_Pitch;
    Alignment_ = surface.Alignment_;
    RawData_ = surface.RawData_;
  }

  ImageSurface::ImageSurface(ImageSurface&& surface)
    : ImageSurface()
  {
    swap(*this, surface);
  }

  ImageSurface &ImageSurface::operator = (ImageSurface copy)
  {
    swap(*this, copy);

    return *this;
  }

  void swap(ImageSurface& lhs, ImageSurface& rhs)
  {
    using std::swap;

    swap(lhs.width_, rhs.width_);
    swap(lhs.height_, rhs.height_);
    swap(lhs.bpe_, rhs.bpe_);
    swap(lhs.format_, rhs.format_);
    swap(lhs.m_Pitch, rhs.m_Pitch);
    swap(lhs.Alignment_, rhs.Alignment_);
    swap(lhs.RawData_, rhs.RawData_);
  }

  int ImageSurface::GetPitch() const
  {
    return m_Pitch;
  }

  int ImageSurface::GetBlockHeight() const
  {
    unsigned int block = GPixelFormats[format_].BlockSizeY;
    unsigned int HeightInBlocks = Align(GetHeight(), block) / block;
    return HeightInBlocks;
  }

  BitmapFormat ImageSurface::GetFormat() const
  {
    return format_;
  }

  int ImageSurface::GetAlignment() const
  {
    return Alignment_;
  }

  void ImageSurface::Allocate(BitmapFormat format, int width, int height)
  {
    nuxAssert(format < BITFMT_END_GFX_FORMATS);
    nuxAssert(width >= 0);
    nuxAssert(height >= 0);

    if (width < 0)
      width = 0;

    if (height < 0)
      height = 0;

    if ((format_ == format) &&
         (width_ == width) &&
         (height_ == height))
    {
      // no need to recreate
      Clear();
      return;
    }

    if ((width == 0) || (height == 0))
    {
      width_ = 0;
      height_ = 0;
      Alignment_ = 1;
      bpe_ = 0;
      format_ = BITFMT_UNKNOWN;
      m_Pitch = 0;
      RawData_.clear();
      RawData_.shrink_to_fit();
      return;
    }

    width_ = width;
    height_ = height;

    Alignment_ = GPixelFormats[format].RowMemoryAlignment;

    bpe_ = GPixelFormats[format].BlockBytes;
    format_ = format;

    // For DXT, width and height are rounded up to a multiple of 4 in order
    // to create 4x4 blocks of pixels; And in this context, byte alignment
    // is 1 ie. data is densely packed.
    unsigned int block = GPixelFormats[format].BlockSizeX;
    unsigned int shift = Log2(GPixelFormats[format].BlockSizeX);
    m_Pitch = Align((bpe_ * ((width_ + (block - 1)) >> shift)), Alignment_);

    block = GPixelFormats[format].BlockSizeY;
    shift = Log2(GPixelFormats[format].BlockSizeY);

    RawData_.resize(m_Pitch * Align((height + (block-1)) >> shift, block));
    RawData_.shrink_to_fit();

    Clear();
  }

// This computes the correct pitch of a line. For instance if the unpack
// alignment is 4, the pitch must have a number of pixel multiple of 4.  See
// Avoiding 16 Common OpenGL Pitfalls
// http://www.opengl.org/resources/features/KilgardTechniques/oglpitfall/
// 7. Watch Your Pixel Store Alignment
  int ImageSurface::GetLevelPitch(BitmapFormat format, int width,
                                     int /* height */, int miplevel)
  {
    int levelwidth = ImageSurface::GetLevelDim(format, width, miplevel);

    int bpe = GPixelFormats[format].BlockBytes;
    int memalignment = GPixelFormats[format].RowMemoryAlignment;
    int block = GPixelFormats[format].BlockSizeX;
    int shift = Log2(GPixelFormats[format].BlockSizeX);
    int pitch = Align((bpe * ((levelwidth + (block - 1)) >> shift)), memalignment);

    return pitch;
  }

  int ImageSurface::GetLevelPitchNoMemAlignment(BitmapFormat format, int width, int /* height */, int miplevel)
  {
    int levelwidth = ImageSurface::GetLevelDim(format, width, miplevel);

    int bpe = GPixelFormats[format].BlockBytes;
    int block = GPixelFormats[format].BlockSizeX;
    int shift = Log2(GPixelFormats[format].BlockSizeX);
    int pitch = Align((bpe * ((levelwidth + (block - 1)) >> shift)), 1);

    return pitch;
  }

  int ImageSurface::GetLevelSize(BitmapFormat format, int width, int height, int miplevel)
  {
    int pitch = ImageSurface::GetLevelPitch(format, width, height, miplevel);
    int levelheight = ImageSurface::GetLevelDim(format, height, miplevel);

    int block = GPixelFormats[format].BlockSizeY;
    int HeightInBlocks = Align(levelheight, block) / block;

    int size = pitch * HeightInBlocks;
    return size;
  }

  int ImageSurface::GetLevelSize(BitmapFormat format, int width, int height, int depth, int miplevel)
  {
    int pitch = ImageSurface::GetLevelPitch(format, width, height, miplevel);
    int levelheight = ImageSurface::GetLevelDim(format, height, miplevel);
    int leveldepth = ImageSurface::GetLevelDim(format, depth, miplevel);

    int block = GPixelFormats[format].BlockSizeY;
    int HeightInBlocks = Align(levelheight, block) / block;

    int size = pitch * HeightInBlocks;
    return leveldepth * size;
  }

  int ImageSurface::GetLevelWidth(BitmapFormat /* format */, int width, int miplevel)
  {
    // return 1 if the mip level does not exist.
    return Max<int> (1, width >> miplevel);
  }

  int ImageSurface::GetLevelHeight(BitmapFormat /* format */, int height, int miplevel)
  {
    // return 1 if the mip level does not exist.
    return Max<int> (1, height >> miplevel);
  }

  int ImageSurface::GetLevelDim(BitmapFormat /* format */, int length, int miplevel)
  {
    // return 1 if the mip level does not exist.
    return Max<int> (1, length >> miplevel);
  }

  int ImageSurface::GetNumMipLevel(BitmapFormat /* format */, int width, int height)
  {
    int NumTotalMipLevel    = 1 + floorf(Log2(Max(width, height)));
    return NumTotalMipLevel;
  }

  int ImageSurface::GetMemAlignment(BitmapFormat format)
  {
    return GPixelFormats[format].RowMemoryAlignment;
  }

  int ImageSurface::GetLevelBlockWidth(BitmapFormat format, int width, int miplevel)
  {
    int block = GPixelFormats[format].BlockSizeX;
    int WidthInBlocks = Align(GetLevelDim(format, width, miplevel), block) / block;
    return WidthInBlocks;
  }

  int ImageSurface::GetLevelBlockHeight(BitmapFormat format, int height, int miplevel)
  {
    int block = GPixelFormats[format].BlockSizeY;
    int HeightInBlocks = Align(GetLevelDim(format, height, miplevel), block) / block;
    return HeightInBlocks;
  }

  bool ImageSurface::IsNull() const
  {
    if ((width_ == 0) || (height_ == 0) || (format_ == BITFMT_UNKNOWN))
      return true;

    return false;
  }

  void ImageSurface::Write32b(int i, int j, unsigned int value)
  {
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ >= 4);

    if ((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
      return;

    RawData_[j *m_Pitch + i *bpe_ + 0] = (unsigned char) (value & 0xff);
    RawData_[j *m_Pitch + i *bpe_ + 1] = (unsigned char) ((value & 0xff00) >> 8);
    RawData_[j *m_Pitch + i *bpe_ + 2] = (unsigned char) ((value & 0xff0000) >> 16);
    RawData_[j *m_Pitch + i *bpe_ + 3] = (unsigned char) ((value & 0xff000000) >> 24);
  }

  void ImageSurface::Write24b(int i, int j, unsigned int value)
  {
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ >= 3);

    if ((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
      return;

    RawData_[j *m_Pitch + i *bpe_ + 0] = (unsigned char) (value & 0xff);
    RawData_[j *m_Pitch + i *bpe_ + 1] = (unsigned char) ((value & 0xff00) >> 8);
    RawData_[j *m_Pitch + i *bpe_ + 2] = (unsigned char) ((value & 0xff0000) >> 16);
  }

  void ImageSurface::Write16b(int i, int j, unsigned short value)
  {
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ >= 2);

    if ((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
      return;

    RawData_[j *m_Pitch + i *bpe_ + 0] = (unsigned char) (value & 0xff);
    RawData_[j *m_Pitch + i *bpe_ + 1] = (unsigned char) ((value & 0xff00) >> 8);
  }

  void ImageSurface::Write8b(int i, int j, unsigned char value)
  {
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ >= 1);

    if ((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
      return;

    RawData_[j *m_Pitch + i *bpe_ + 0] = (unsigned char) (value & 0xff);
  }

  void ImageSurface::Write(int i, int j, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
  {
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_ == 4);

    if ((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
      return;

    if ((i < 0) || (i > width_))
      return;

    if ((j < 0) || (j > height_))
      return;

    if (bpe_ != 4)
      return;

    RawData_[j *m_Pitch + i *bpe_ + 0] = r;
    RawData_[j *m_Pitch + i *bpe_ + 1] = g;
    RawData_[j *m_Pitch + i *bpe_ + 2] = b;
    RawData_[j *m_Pitch + i *bpe_ + 3] = a;
  }

  unsigned int ImageSurface::Read(int i, int j)
  {
    nuxAssert(i < width_);
    nuxAssert(j < height_);
    nuxAssert(bpe_);

    if ((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
      return 0x00000000;

    if (bpe_ == 4)
    {
      return  ((unsigned int) RawData_[j * m_Pitch + i * bpe_ + 3] << 24)   |
              ((unsigned int) RawData_[j * m_Pitch + i * bpe_ + 2] << 16)   |
              ((unsigned int) RawData_[j * m_Pitch + i * bpe_ + 1] << 8)    |
              ((unsigned int) RawData_[j * m_Pitch + i * bpe_ + 0] << 0);
    }

    if (bpe_ == 3)
    {
      return  ((unsigned int) RawData_[j * m_Pitch + i * bpe_ + 2] << 16)   |
              ((unsigned int) RawData_[j * m_Pitch + i * bpe_ + 1] << 8)    |
              ((unsigned int) RawData_[j * m_Pitch + i * bpe_ + 0] << 0);
    }

    if (bpe_ == 2)
    {
      return  ((unsigned int) RawData_[j * m_Pitch + i * bpe_ + 1] << 8)    |
              ((unsigned int) RawData_[j * m_Pitch + i * bpe_ + 0] << 0);
    }

    if (bpe_ == 1)
    {
      return  (unsigned int) RawData_[j * m_Pitch + i * bpe_ + 0];
    }

    return 0x0000000;
  }

  void ImageSurface::Clear()
  {
    if (RawData_.empty())
      return;

    if ((width_ == 0) || (height_ == 0))
      return;

    auto size = GetSize();
    memset(RawData_.data(), 0, size);
  }

  void ImageSurface::FlipHorizontal()
  {
    if ((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2)  || (format_ == BITFMT_DXT3)  || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
      return;

    int i, j, k;

    if (RawData_.empty())
      return;

    if (width_ == 0 || height_ == 0)
      return;

    std::vector<unsigned char> flip_data(m_Pitch*height_);

    for (j = 0; j < height_; j++)
    {
      for (i = 0; i < width_; i++)
      {
        for (k = 0; k < bpe_; k++)
        {
          flip_data[ (j*m_Pitch) + i *bpe_ + k] = RawData_[ (j*m_Pitch) + (width_-i-1) * bpe_ + k];
        }
      }
    }

    RawData_.swap(flip_data);
  }

  void ImageSurface::FlipVertical()
  {
    if (RawData_.empty())
      return;

    if (width_ == 0 || height_ == 0)
      return;

    if ((format_ == BITFMT_DXT1) || (format_ == BITFMT_DXT2) || (format_ == BITFMT_DXT3) || (format_ == BITFMT_DXT4) || (format_ == BITFMT_DXT5))
    {
      FlipDXTVertical();
    }
    else
    {
      std::vector<unsigned char> flip_data(m_Pitch*height_);

      for (int j = 0; j < height_; j++)
      {
        for (int i = 0; i < width_; i++)
        {
          for (int k = 0; k < bpe_; k++)
          {
            flip_data[ (j*m_Pitch) + i *bpe_ + k] = RawData_[ (height_-j-1) *m_Pitch + i * bpe_ + k];
          }
        }
      }

      RawData_.swap(flip_data);
    }

  }

  void ImageSurface::FlipDXTVertical()
  {
    //void(CDDSImage::*flipblocks)(DXTColBlock*, unsigned int);
    int xblocks = (width_ + 3) / 4;
    int yblocks = (height_ + 3) / 4;
    int blocksize;
    int linesize;

    switch(format_)
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

    for (int j = 0; j < (yblocks >> 1); j++)
    {
      top = reinterpret_cast<DXTColBlock *>(&(RawData_[j * linesize]));
      bottom = reinterpret_cast<DXTColBlock *>(&(RawData_[((yblocks - j) - 1) * linesize]));

      switch(format_)
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
        default:
          nuxAssert("[ImageSurface::FlipDXTVertical] Invalid Switch option.");
          break;
      }

      SwapBlocks(bottom, top, linesize);
    }
  }

  void ImageSurface::SwapBlocks(void *byte1, void *byte2, int size)
  {
    std::vector<unsigned char> tmp(size);

    memcpy(tmp.data(), byte1, size);
    memcpy(byte1, byte2, size);
    memcpy(byte2, tmp.data(), size);
  }

  void ImageSurface::FlipBlocksDXT1(DXTColBlock *line, int numBlocks)
  {
    DXTColBlock *curblock = line;

    for (int i = 0; i < numBlocks; i++)
    {
      SwapBlocks(&curblock->row[0], &curblock->row[3], sizeof(unsigned char));
      SwapBlocks(&curblock->row[1], &curblock->row[2], sizeof(unsigned char));

      curblock++;
    }
  }

  void ImageSurface::FlipBlocksDXT3(DXTColBlock *line, int numBlocks)
  {
    DXTColBlock *curblock = line;
    DXT3AlphaBlock *alphablock;

    for (int i = 0; i < numBlocks; i++)
    {
      alphablock = (DXT3AlphaBlock *) curblock;

      SwapBlocks(&alphablock->row[0], &alphablock->row[3], sizeof(unsigned short));
      SwapBlocks(&alphablock->row[1], &alphablock->row[2], sizeof(unsigned short));

      curblock++;

      SwapBlocks(&curblock->row[0], &curblock->row[3], sizeof(unsigned char));
      SwapBlocks(&curblock->row[1], &curblock->row[2], sizeof(unsigned char));

      curblock++;
    }
  }

  void ImageSurface::FlipBlocksDXT5(DXTColBlock *line, int numBlocks)
  {
    DXTColBlock *curblock = line;
    DXT5AlphaBlock *alphablock;

    for (int i = 0; i < numBlocks; i++)
    {
      alphablock = (DXT5AlphaBlock *) curblock;

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

    gBits[0][0] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[0][1] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[0][2] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[0][3] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[1][0] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[1][1] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[1][2] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[1][3] = (unsigned char) (bits & mask);

    bits = 0;
    memcpy(&bits, &block->row[3], sizeof(unsigned char) * 3);

    gBits[2][0] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[2][1] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[2][2] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[2][3] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[3][0] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[3][1] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[3][2] = (unsigned char) (bits & mask);
    bits >>= 3;
    gBits[3][3] = (unsigned char) (bits & mask);

    unsigned long *pBits = ((unsigned long *) & (block->row[0]));

    *pBits = *pBits | (gBits[3][0] << 0);
    *pBits = *pBits | (gBits[3][1] << 3);
    *pBits = *pBits | (gBits[3][2] << 6);
    *pBits = *pBits | (gBits[3][3] << 9);

    *pBits = *pBits | (gBits[2][0] << 12);
    *pBits = *pBits | (gBits[2][1] << 15);
    *pBits = *pBits | (gBits[2][2] << 18);
    *pBits = *pBits | (gBits[2][3] << 21);

    pBits = ((unsigned long *) & (block->row[3]));

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

  const unsigned char *ImageSurface::GetPtrRawData() const
  {
    return RawData_.data();
  }

  unsigned char *ImageSurface::GetPtrRawData()
  {
    return RawData_.data();
  }

  int ImageSurface::GetSize() const
  {
    if ((format_ == BITFMT_DXT1) ||
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
    if (width_ == 0 || height_ == 0)
      return Color(0.f, 0.f, 0.f, 0.f);

    float r, g, b, a;
    r = g = b = a = 0;

    if (bpe_ == 8)
    {
      for (int j = 0; j < height_; j++)
      {
        for (int i = 0; i < width_; i++)
        {
          unsigned int v = Read(i, j);
          r += (v & 0x000000FF);
          g += (v & 0x0000FF00) >> 1;
          b += (v & 0x00FF0000) >> 2;
          a += (v & 0xFF000000) >> 3;
        }
      }
    }

    unsigned int num_pixels = width_ * height_;
    return Color(r / num_pixels, g / num_pixels, b / num_pixels, a / num_pixels);
  }


  ///////////////////////////////////////////////////////////////////
  NTextureData::NTextureData(BitmapFormat f, int width, int height, int NumMipmap)
    : m_NumMipmap(0)
    , m_MipSurfaceArray(1)
  {
    Allocate(f, width, height, NumMipmap);
  }

  void NTextureData::swap(NTextureData& other)
  {
    using std::swap;
    swap(this->m_NumMipmap, other.m_NumMipmap);
    swap(this->m_TotalMemorySize, other.m_TotalMemorySize);
    swap(this->m_MipSurfaceArray, other.m_MipSurfaceArray);
  }

  //! Copy constructor
  NTextureData::NTextureData(const NTextureData &object)
  {
    m_NumMipmap = object.m_NumMipmap;
    m_TotalMemorySize = object.m_TotalMemorySize;
    m_MipSurfaceArray = object.m_MipSurfaceArray;
  }

  //! Move constructor
  NTextureData::NTextureData(NTextureData&& other)
    : NTextureData()
  {
    swap(other);
  }

  //! Assignment constructor
  NTextureData &NTextureData::operator = (NTextureData copy)
  {
    swap(copy);

    return *this;
  }

  void NTextureData::Allocate(BitmapFormat format, int width, int height, int NumMipmapRequested)
  {
    nuxAssertMsg(width >= 0, "[NTextureData::Allocate] Error: Negative texture width.");
    nuxAssertMsg(height >= 0, "[NTextureData::Allocate] Error: Negative texture height.");
    nuxAssert(NumMipmapRequested >= 0);

    int NumTotalMipLevel    = 1 + (int) Floor(Log2(Max(width, height)));
    m_NumMipmap = NumMipmapRequested;

    if (NumMipmapRequested == 0)
      m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    if (NumMipmapRequested > NumTotalMipLevel)
      m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    m_TotalMemorySize = 0;
    m_MipSurfaceArray.resize(m_NumMipmap);
    m_MipSurfaceArray.shrink_to_fit();

    for (int i = 0; i < m_NumMipmap; ++i)
    {
      int w = width >> i;
      int h = height >> i;
      m_MipSurfaceArray[i] = ImageSurface(format, w, h);
      m_TotalMemorySize += m_MipSurfaceArray[i].GetSize();
    }
  }

  void NTextureData::AllocateCheckBoardTexture(int width, int height, int NumMipmap, Color color0, Color color1, int TileWidth, int TileHeight)
  {
    Allocate(BITFMT_R8G8B8A8, width, height, NumMipmap);

    for (int i = 0; i < m_NumMipmap; ++i)
    {
      int w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), i);
      int h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), i);
      MakeCheckBoardImage(m_MipSurfaceArray[i], w, h, color0, color1, TileWidth, TileHeight);
    }
  }

  void NTextureData::AllocateColorTexture(int width, int height, int NumMipmap, Color color0)
  {
    Allocate(BITFMT_R8G8B8A8, width, height, NumMipmap);

    for (int i = 0; i < m_NumMipmap; ++i)
    {
      int w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), i);
      int h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), i);
      MakeCheckBoardImage(m_MipSurfaceArray[i], w, h, color0, color0);
    }
  }

  int NTextureData::GetNumMipmap() const
  {
    return m_NumMipmap;
  }

  bool NTextureData::SetSurface(int MipLevel, const ImageSurface &targetsurface)
  {
    nuxAssert(MipLevel >= 0);
    nuxAssert(MipLevel < m_NumMipmap);

    ImageSurface &surface = GetSurface(MipLevel);

    if (surface.GetFormat() != targetsurface.GetFormat())
      return false;

    if (surface.GetWidth() != targetsurface.GetWidth())
      return false;

    if (surface.GetHeight() != targetsurface.GetHeight())
      return false;

    surface = targetsurface;
    return true;
  }

///////////////////////////////////////////////////////////////////
  NCubemapData::NCubemapData(BitmapFormat format, int width, int height, int NumMipmap)
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
    int n = (int) m_MipSurfaceArray[0].size();

    for (int i = 0; i < n; i++)
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
  NCubemapData::NCubemapData(const NCubemapData &object)
  {
    for (int face = 0; face < 6; face++)
      for (int i = 0; i < object.GetNumMipmap(); i++)
        m_MipSurfaceArray[face].push_back(new ImageSurface(object.GetSurface(face, i)));
  }

//! Assignment constructor
  NCubemapData &NCubemapData::operator = (const NCubemapData &copy)
  {
    ClearData();
    m_NumMipmap = copy.m_NumMipmap;
    m_TotalMemorySize = copy.m_TotalMemorySize;

    for (int face = 0; face < 6; face++)
      for (int i = 0; i < copy.GetNumMipmap(); i++)
        m_MipSurfaceArray[face].push_back(new ImageSurface(copy.GetSurface(face, i)));

    return *this;
  }

  void NCubemapData::Allocate(BitmapFormat format, int width, int height, int NumMipmapRequested)
  {
    nuxAssertMsg(width >= 0, "[NCubemapData::Allocate] Error: Negative texture width.");
    nuxAssertMsg(height >= 0, "[NCubemapData::Allocate] Error: Negative texture height.");
    nuxAssert(NumMipmapRequested >= 0);

    ClearData();

    int NumTotalMipLevel    = 1 + (int) Floor(Log2(Max(width, height)));
    m_NumMipmap = NumMipmapRequested;

    if (NumMipmapRequested == 0)
      m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    if (NumMipmapRequested > NumTotalMipLevel)
      m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    m_TotalMemorySize = 0;

    for (int face = 0; face < 6; face++)
    {
      for (int i = 0; i < m_NumMipmap; i++)
      {
        int w = width >> i;
        int h = height >> i;
        m_MipSurfaceArray[face].push_back(new ImageSurface(format, w, h));
        m_TotalMemorySize += m_MipSurfaceArray[face][i]->GetSize();
      }
    }
  }

  void NCubemapData::AllocateCheckBoardTexture(int width, int height, int NumMipmap, Color color0, Color color1, int TileWidth, int TileHeight)
  {
    Allocate(BITFMT_R8G8B8A8, width, height, NumMipmap);

    for (int face = 0; face < 6; face++)
    {
      for (int i = 0; i < m_NumMipmap; i++)
      {
        int w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), i);
        int h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), i);
        MakeCheckBoardImage(*m_MipSurfaceArray[face][i], w, h, color0, color1, TileWidth, TileHeight);
      }
    }
  }

  void NCubemapData::AllocateColorTexture(int width, int height, int NumMipmap, Color color0)
  {
    Allocate(BITFMT_R8G8B8A8, width, height, NumMipmap);

    for (int face = 0; face < 6; face++)
    {
      for (int i = 0; i < m_NumMipmap; i++)
      {
        int w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), i);
        int h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), i);
        MakeCheckBoardImage(*m_MipSurfaceArray[face][i], w, h, color0, color0);
      }
    }
  }

  int NCubemapData::GetNumMipmap() const
  {
    return m_MipSurfaceArray[0].size();
  }

  bool NCubemapData::SetSurface(int face, int MipLevel, const ImageSurface &targetsurface)
  {
    nuxAssert(face >= 0);
    nuxAssert(face < 6);
    nuxAssert(MipLevel >= 0);
    nuxAssert(MipLevel < m_NumMipmap);

    ImageSurface &surface = GetSurface(face, MipLevel);

    if (surface.GetFormat() != targetsurface.GetFormat())
      return false;

    if (surface.GetWidth() != targetsurface.GetWidth())
      return false;

    if (surface.GetHeight() != targetsurface.GetHeight())
      return false;

    surface = targetsurface;
    return true;
  }

///////////////////////////////////////////////////////////////////
  NVolumeData::NVolumeData(BitmapFormat format, int width, int height, int depth, int NumMipmap)
    :   m_NumMipmap(0)
    ,   m_Depth(0)
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
    for (int mip = 0; mip < GetNumMipmap(); mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(GetFormat(), GetDepth(), mip); s++)
      {
        delete m_MipSurfaceArray[mip][s];
      }

      m_MipSurfaceArray[mip].clear();
    }

    delete [] m_MipSurfaceArray;
  }

//! Copy constructor
  NVolumeData::NVolumeData(const NVolumeData &object)
  {
    for (int mip = 0; mip < object.GetNumMipmap(); mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(object.GetFormat(), object.GetDepth(), mip); s++)
      {
        m_MipSurfaceArray[mip].push_back(new ImageSurface(object.GetSurface(s, mip)));
      }
    }
  }

//! Assignment constructor
  NVolumeData &NVolumeData::operator = (const NVolumeData &copy)
  {
    ClearData();
    m_Depth = copy.m_Depth;
    m_NumMipmap = copy.m_NumMipmap;
    m_TotalMemorySize = copy.m_TotalMemorySize;

    m_MipSurfaceArray = new std::vector<ImageSurface *>[m_NumMipmap];

    for (int mip = 0; mip < copy.GetNumMipmap(); mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(copy.GetFormat(), copy.GetDepth(), mip); s++)
      {
        m_MipSurfaceArray[mip].push_back(new ImageSurface(copy.GetSurface(s, mip)));
      }
    }

    return *this;
  }

  void NVolumeData::Allocate(BitmapFormat format, int width, int height, int depth, int NumMipmapRequested)
  {
    nuxAssertMsg(depth >= 0, "[NVolumeData::Allocate] Error: Negative number of slice.");
    nuxAssertMsg(width >= 0, "[NVolumeData::Allocate] Error: Negative texture width.");
    nuxAssertMsg(height >= 0, "[NVolumeData::Allocate] Error: Negative texture height.");
    nuxAssert(NumMipmapRequested >= 0);

    ClearData();

    int NumTotalMipLevel    = 1 + (int) Floor(Log2(Max(width, height)));
    m_NumMipmap = NumMipmapRequested;

    if (NumMipmapRequested == 0)
      m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    if (NumMipmapRequested > NumTotalMipLevel)
      m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    m_Depth = depth;

    m_MipSurfaceArray = new std::vector<ImageSurface *>[m_NumMipmap];
    m_TotalMemorySize = 0;

    for (int mip = 0; mip < m_NumMipmap; mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(format, depth, mip); s++)
      {
        int w = ImageSurface::GetLevelDim(format, width, mip);
        int h = ImageSurface::GetLevelDim(format, height, mip);
        m_MipSurfaceArray[mip].push_back(new ImageSurface(format, w, h));
        m_TotalMemorySize += m_MipSurfaceArray[mip][s]->GetSize();
      }
    }
  }

  void NVolumeData::AllocateCheckBoardTexture(int width, int height, int slice, int NumMipmap, Color color0, Color color1, int TileWidth, int TileHeight)
  {
    Allocate(BITFMT_R8G8B8A8, width, height, slice, NumMipmap);

    for (int mip = 0; mip < m_NumMipmap; mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetDepth(), mip); s++)
      {
        int w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), mip);
        int h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), mip);
        MakeCheckBoardImage(* (m_MipSurfaceArray[mip][s]), w, h, color0, color1, TileWidth, TileHeight);
      }
    }
  }

  void NVolumeData::AllocateColorTexture(int width, int height, int slice, int NumMipmap, Color color0)
  {
    Allocate(BITFMT_R8G8B8A8, width, height, slice, NumMipmap);

    for (int mip = 0; mip < m_NumMipmap; mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetDepth(), mip); s++)
      {
        int w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), mip);
        int h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), mip);
        MakeCheckBoardImage(* (m_MipSurfaceArray[mip][s]), w, h, color0, color0);
      }
    }
  }

  int NVolumeData::GetNumMipmap() const
  {
    return m_NumMipmap;
  }

  bool NVolumeData::SetSurface(int Slice, int MipLevel, const ImageSurface &targetsurface)
  {
    nuxAssert(Slice >= 0);
    nuxAssert(Slice < m_Depth);
    nuxAssert(MipLevel >= 0);
    nuxAssert(MipLevel < m_NumMipmap);

    ImageSurface &surface = GetSurface(Slice, MipLevel);

    if (surface.GetFormat() != targetsurface.GetFormat())
      return false;

    if (surface.GetWidth() != targetsurface.GetWidth())
      return false;

    if (surface.GetHeight() != targetsurface.GetHeight())
      return false;

    surface = targetsurface;
    return true;
  }

///////////////////////////////////////////////////////////////////
  NAnimatedTextureData::NAnimatedTextureData(BitmapFormat format, int width, int height, int depth)
    :   m_NumMipmap(0)
    ,   m_Depth(0)
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
    for (int mip = 0; mip < GetNumMipmap(); mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(GetFormat(), GetDepth(), mip); s++)
      {
        delete m_MipSurfaceArray[mip][s];
      }

      m_MipSurfaceArray[mip].clear();
    }

    m_FrameTimeArray.clear();
    delete [] m_MipSurfaceArray;
  }

//! Copy constructor
  NAnimatedTextureData::NAnimatedTextureData(const NAnimatedTextureData &object)
  {
    for (int mip = 0; mip < object.GetNumMipmap(); mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(object.GetFormat(), object.GetDepth(), mip); s++)
      {
        m_MipSurfaceArray[mip].push_back(new ImageSurface(object.GetSurface(mip, s)));
      }
    }

    for (int frame = 0; frame < object.GetDepth(); frame++)
    {
      m_FrameTimeArray.push_back(object.GetFrameTime(frame));
    }
  }

//! Assignment constructor
  NAnimatedTextureData &NAnimatedTextureData::operator = (const NAnimatedTextureData &copy)
  {
    ClearData();
    m_Depth = copy.GetDepth();
    m_NumMipmap = copy.m_NumMipmap;
    m_TotalMemorySize = copy.m_TotalMemorySize;

    m_MipSurfaceArray = new std::vector<ImageSurface *>[m_NumMipmap];

    for (int mip = 0; mip < copy.GetNumMipmap(); mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(copy.GetFormat(), copy.GetDepth(), mip); s++)
      {
        m_MipSurfaceArray[mip].push_back(new ImageSurface(copy.GetSurface(s)));
      }
    }

    for (int frame = 0; frame < copy.GetDepth(); frame++)
    {
      m_FrameTimeArray.push_back(copy.GetFrameTime(frame));
    }

    return *this;
  }

  void NAnimatedTextureData::Allocate(BitmapFormat format, int width, int height, int depth, int NumMipmapRequested)
  {
    nuxAssertMsg(depth >= 0, "[NAnimatedTextureData::Allocate] Error: Negative number of slice.");
    nuxAssertMsg(width >= 0, "[NAnimatedTextureData::Allocate] Error: Negative texture width.");
    nuxAssertMsg(height >= 0, "[NAnimatedTextureData::Allocate] Error: Negative texture height.");
    nuxAssert(NumMipmapRequested >= 0);

    ClearData();

    int NumTotalMipLevel    = 1 + (int) Floor(Log2(Max(width, height)));
    m_NumMipmap = NumMipmapRequested;

    if (NumMipmapRequested == 0)
      m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    if (NumMipmapRequested > NumTotalMipLevel)
      m_NumMipmap = NumTotalMipLevel ? NumTotalMipLevel : 1;

    m_Depth = depth;

    m_MipSurfaceArray = new std::vector<ImageSurface *>[m_NumMipmap];
    m_TotalMemorySize = 0;

    for (int mip = 0; mip < m_NumMipmap; mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(format, depth, mip); s++)
      {
        int w = ImageSurface::GetLevelDim(format, width, mip);
        int h = ImageSurface::GetLevelDim(format, height, mip);
        m_MipSurfaceArray[mip].push_back(new ImageSurface(format, w, h));
        m_TotalMemorySize += m_MipSurfaceArray[mip][s]->GetSize();
      }
    }
  }

  void NAnimatedTextureData::AllocateCheckBoardTexture(int width, int height, int slice, int NumMipmap, Color color0, Color color1, int TileWidth, int TileHeight)
  {
    Allocate(BITFMT_R8G8B8A8, width, height, slice, NumMipmap);

    for (int mip = 0; mip < m_NumMipmap; mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetDepth(), mip); s++)
      {
        int w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), mip);
        int h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), mip);
        MakeCheckBoardImage(* (m_MipSurfaceArray[mip][s]), w, h, color0, color1, TileWidth, TileHeight);
      }
    }
  }

  void NAnimatedTextureData::AllocateColorTexture(int width, int height, int slice, int NumMipmap, Color color0)
  {
    Allocate(BITFMT_R8G8B8A8, width, height, slice, NumMipmap);

    for (int mip = 0; mip < m_NumMipmap; mip++)
    {
      for (int s = 0; s < ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetDepth(), mip); s++)
      {
        int w = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetWidth(), mip);
        int h = ImageSurface::GetLevelDim(BITFMT_R8G8B8A8, GetHeight(), mip);
        MakeCheckBoardImage(* (m_MipSurfaceArray[mip][s]), w, h, color0, color0);
      }
    }
  }

  int NAnimatedTextureData::GetNumMipmap() const
  {
    return m_NumMipmap;
  }

  bool NAnimatedTextureData::SetSurface(int Slice, int MipLevel, const ImageSurface &targetsurface)
  {
    nuxAssert(Slice >= 0);
    nuxAssert(Slice < m_Depth);
    nuxAssert(MipLevel >= 0);
    nuxAssert(MipLevel < m_NumMipmap);

    ImageSurface &surface = GetSurface(Slice, MipLevel);

    if (surface.GetFormat() != targetsurface.GetFormat())
      return false;

    if (surface.GetWidth() != targetsurface.GetWidth())
      return false;

    if (surface.GetHeight() != targetsurface.GetHeight())
      return false;

    surface = targetsurface;
    return true;
  }

}

