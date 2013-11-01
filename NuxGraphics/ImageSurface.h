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


#ifndef IMAGESURFACE_H
#define IMAGESURFACE_H

#include "BitmapFormats.h"
#include "NuxCore/Math/MathUtility.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#define DEBUG_ENTER(a)
#define DEBUG_WARNING(a)
#define DEBUG_EXIT(a)

namespace nux
{

  class RawData
  {
  public:
    RawData() {};
    ~RawData() {};

    //! Get number of elements.
    unsigned int GetElementCount()
    {
      return 0;
    }

    //! Get the number of bytes per elements.
    unsigned int GetElementSize()
    {
      return 0;
    }

    void GetSize() {};
  };

  struct DXTColBlock
  {
    unsigned short col0;
    unsigned short col1;

    unsigned char row[4];
  };

  struct DXT3AlphaBlock
  {
    unsigned short row[4];
  };

  struct DXT5AlphaBlock
  {
    unsigned char alpha0;
    unsigned char alpha1;

    unsigned char row[6];
  };

//! Image Surface class
  /*!
      Represent and image surface inside a complex data structure such as a 2D texture, Volume texture or Cube map.
  */
  class ImageSurface
  {
  public:
    ImageSurface();
    ImageSurface(BitmapFormat format, unsigned int width, unsigned int height);
    ImageSurface(const ImageSurface &);
    ImageSurface(ImageSurface&&);
    ImageSurface &operator = (ImageSurface);

    bool IsNull() const;
    int GetWidth() const
    {
      return width_;
    }
    int GetHeight() const
    {
      return height_;
    }
    void Allocate(BitmapFormat format, int width, int height);
    void Write32b(int i, int j, unsigned int value);
    void Write24b(int i, int j, unsigned int value);
    void Write16b(int i, int j, unsigned short value);
    void Write8b(int i, int j, unsigned char value);
    void Write(int i, int j, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    //! Read an element of the surface.
    /*!
        Return a 32 bits value representing the image element at coordinates(i, j).
        For the RGBA format, the LSB of the returned value represent the read value, and the MSB represents the alpha value.
        | LSB: Red | Green | Blue | MSB: Alpha|
        @return The image element at coordinates(i, j).
    */
    unsigned int Read(int i, int j);
    //! Set all surface elements to 0.
    void Clear();
    //! Flip the surface horizontally.
    void FlipHorizontal();
    //! Flip the surface vertically.
    void FlipVertical();
    //! Returns the surface pitch.
    int GetPitch() const;
    int GetBlockHeight() const;
    int GetAlignment() const;
    int GetSize() const;
    BitmapFormat GetFormat() const;
    const unsigned char *GetPtrRawData() const;
    unsigned char *GetPtrRawData();

    static int GetLevelPitch(BitmapFormat format, int width, int height, int miplevel);
    static int GetLevelPitchNoMemAlignment(BitmapFormat format, int width, int height, int miplevel);
    static int GetLevelSize(BitmapFormat format, int width, int height, int miplevel);
    static int GetLevelSize(BitmapFormat format, int width, int height, int depth, int miplevel);
    static int GetLevelWidth(BitmapFormat format, int width, int miplevel);
    static int GetLevelHeight(BitmapFormat format, int height, int miplevel);
    static int GetLevelDim(BitmapFormat format, int length, int miplevel);
    static int GetNumMipLevel(BitmapFormat format, int width, int height);
    static int GetMemAlignment(BitmapFormat format);
    static int GetLevelBlockWidth(BitmapFormat format, int width, int miplevel);
    static int GetLevelBlockHeight(BitmapFormat format, int height, int miplevel); // for DXT

    // Image Processing
    //! Compute the average color of the image surface.
    /*!
        Sum up all the image elements and divide by the number of elements.
        @return The average color of the image.
    */
    Color AverageColor();

  private:
    void FlipDXTVertical();
    void SwapBlocks(void *byte1, void *byte2, int size);
    void FlipBlocksDXT1(DXTColBlock *line, int numBlocks);
    void FlipBlocksDXT3(DXTColBlock *line, int numBlocks);
    void FlipBlocksDXT5(DXTColBlock *line, int numBlocks);
    void FlipDXT5Alpha(DXT5AlphaBlock *block);
    friend void swap(ImageSurface&, ImageSurface&);

    int width_;           //!< Image width
    int height_;          //!< Image height.
    BitmapFormat format_;   //!< Image format.
    int m_Pitch;          //!< Image pitch.
    int bpe_;             //!< Number of byte per element.
    int Alignment_;       //!< Data alignment.
    std::vector<unsigned char> RawData_;
  };

  void swap(ImageSurface&, ImageSurface&);


  class NBitmapData
  {
  public:
    NBitmapData();
    virtual ~NBitmapData();

    virtual const ImageSurface &GetSurface(int MipLevel) const = 0;
    virtual ImageSurface &GetSurface(int MipLevel) = 0;
    virtual const ImageSurface &GetSurface(int face, int MipLevel) const = 0;
    virtual ImageSurface &GetSurface(int face, int MipLevel) = 0;

    virtual bool IsTextureData() const
    {
      return false;
    };
    virtual bool IsCubemapTextureData() const
    {
      return false;
    }
    virtual bool IsVolumeTextureData() const
    {
      return false;
    }
    virtual bool IsAnimatedTextureData() const
    {
      return false;
    }

    virtual int GetNumMipmap() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetDepth() const
    {
      return 0;
    }
    virtual BitmapFormat GetFormat() const = 0;
    virtual bool IsNull() const = 0;
    virtual int GetMemorySize() const
    {
      return m_TotalMemorySize;
    }

  protected:
    unsigned int m_TotalMemorySize;
//protected:
//    unsigned int m_Width;
//    unsigned int m_Height;
//    BitmapFormat m_Format;
  };

  class NTextureData: public NBitmapData
  {
  public:
    NTextureData(BitmapFormat f = BITFMT_R8G8B8A8, int width = 16, int height = 16, int NumMipmap = 1);
    virtual ~NTextureData() {}
    //! Copy constructor.
    NTextureData(const NTextureData &);
    //! Assignment constructor.
    NTextureData &operator = (NTextureData);
    //! Move constructor
    NTextureData(NTextureData&&);

    virtual void Allocate(BitmapFormat f, int width, int height, int NumMipmap = 1);
    virtual void AllocateCheckBoardTexture(int width, int height, int NumMipmap, Color color0, Color color1, int TileWidth = 4, int TileHeight = 4);
    virtual void AllocateColorTexture(int width, int height, int NumMipmap, Color color0 = Color(0xFFFFFFF));

    virtual const ImageSurface &GetSurface(int MipLevel) const
    {
      return m_MipSurfaceArray[MipLevel];
    };
    virtual ImageSurface &GetSurface(int MipLevel)
    {
      return const_cast<ImageSurface &> ((const_cast< const NTextureData * > (this))->GetSurface(MipLevel));
    }
    virtual const ImageSurface &GetSurface(int /* face */, int MipLevel) const
    {
      //nuxAssertMsg(0, "[NTextureData::GetSurface] Use GetSurface(unsigned int MipLevel) for NTextureData.");
      return GetSurface(MipLevel);
    }
    virtual ImageSurface &GetSurface(int /* face */, int MipLevel)
    {
      //nuxAssertMsg(0, "[NTextureData::GetSurface] Use GetSurface(unsigned int MipLevel) for NTextureData.");
      return GetSurface(MipLevel);
    }

    bool SetSurface(int MipLevel, const ImageSurface &object);

    virtual bool IsTextureData() const
    {
      return true;
    }

    virtual int GetNumMipmap() const;
    virtual int GetWidth() const
    {
      return m_MipSurfaceArray[0].GetWidth();
    }
    virtual int GetHeight() const
    {
      return m_MipSurfaceArray[0].GetHeight();
    }
    virtual BitmapFormat GetFormat() const
    {
      return m_MipSurfaceArray[0].GetFormat();
    }
    virtual bool IsNull() const
    {
      return m_MipSurfaceArray[0].IsNull();
    }

  private:
    void swap(NTextureData& other);

    int m_NumMipmap;
    std::vector<ImageSurface> m_MipSurfaceArray;

#if defined(NUX_OS_WINDOWS)
    friend NBitmapData *read_tga_file(const TCHAR *file_name);
#endif
  };

  class NCubemapData: public NBitmapData
  {
  public:
    NCubemapData(BitmapFormat f = BITFMT_R8G8B8A8, int width = 16, int height = 16, int NumMipmap = 1);
    virtual ~NCubemapData();
    //! Copy constructor
    NCubemapData(const NCubemapData &);
    //! Assignment constructor
    NCubemapData &operator = (const NCubemapData &);

    virtual void Allocate(BitmapFormat f, int width, int height, int NumMipmap = 1);
    virtual void AllocateCheckBoardTexture(int width, int height, int NumMipmap, Color color0, Color color1, int TileWidth = 4, int TileHeight = 4);
    virtual void AllocateColorTexture(int width, int height, int NumMipmap, Color color0 = Color(0xFFFFFFF));

    virtual const ImageSurface &GetSurface(int face, int MipLevel) const
    {
      return *m_MipSurfaceArray[face][MipLevel];
    };
    virtual ImageSurface &GetSurface(int face, int MipLevel)
    {
      return const_cast<ImageSurface &> ((const_cast< const NCubemapData * > (this))->GetSurface(face, MipLevel));
    }
    virtual const ImageSurface &GetSurface(int MipLevel) const
    {
      nuxAssertMsg(0, "[NCubemapData::GetSurface] Use GetSurface(unsigned int face, unsigned int MipLevel) for NCubemapData.");
      return GetSurface(0, MipLevel);
    }
    virtual ImageSurface &GetSurface(int MipLevel)
    {
      nuxAssertMsg(0, "[NCubemapData::GetSurface] Use GetSurface(unsigned int face, unsigned int MipLevel) for NCubemapData.");
      return GetSurface(0, MipLevel);
    }
    bool SetSurface(int face, int MipLevel, const ImageSurface &object);

    virtual bool IsCubemapTextureData() const
    {
      return true;
    }

    virtual int GetNumMipmap() const;
    virtual int GetWidth() const
    {
      return m_MipSurfaceArray[0][0]->GetWidth();
    }
    virtual int GetHeight() const
    {
      return m_MipSurfaceArray[0][0]->GetHeight();
    }
    virtual BitmapFormat GetFormat() const
    {
      return m_MipSurfaceArray[0][0]->GetFormat();
    }
    virtual bool IsNull() const
    {
      return m_MipSurfaceArray[0][0]->IsNull();
    }

  private:
    void ClearData();
    int m_NumMipmap;
    std::vector<ImageSurface *> m_MipSurfaceArray[6];
  };

  class NVolumeData: public NBitmapData
  {
  public:
    NVolumeData(BitmapFormat f = BITFMT_R8G8B8A8, int width = 16, int height = 16, int slice = 1, int NumMipmap = 1);
    virtual ~NVolumeData();
    //! Copy constructor
    NVolumeData(const NVolumeData &);
    //! Assignment constructor
    NVolumeData &operator = (const NVolumeData &);

    virtual void Allocate(BitmapFormat f, int width, int height, int slice, int NumMipmap = 1);
    virtual void AllocateCheckBoardTexture(int width, int height, int slice, int NumMipmap, Color color0, Color color1, int TileWidth = 4, int TileHeight = 4);
    virtual void AllocateColorTexture(int width, int height, int slice, int NumMipmap, Color color0 = Color(0xFFFFFFF));

    virtual const ImageSurface &GetSurface(int slice, int MipLevel) const
    {
      return *m_MipSurfaceArray[MipLevel][slice];
    };
    virtual ImageSurface &GetSurface(int slice, int MipLevel)
    {
      return const_cast<ImageSurface &> ((const_cast< const NVolumeData * > (this))->GetSurface(slice, MipLevel));
    }
    virtual const ImageSurface &GetSurface(int MipLevel) const
    {
      nuxAssertMsg(0, "[NVolumeData::GetSurface] Use GetSurface(unsigned int MipLevel, unsigned int MipLevel) for NVolumeData.");
      return GetSurface(MipLevel, 0);
    }
    virtual ImageSurface &GetSurface(int MipLevel)
    {
      nuxAssertMsg(0, "[NVolumeData::GetSurface] Use GetSurface(unsigned int MipLevel, unsigned int MipLevel) for NVolumeData.");
      return GetSurface(MipLevel, 0);
    }
    bool SetSurface(int face, int MipLevel, const ImageSurface &object);

    virtual bool IsVolumeTextureData() const
    {
      return true;
    }

    int GetNumMipmap() const;
    virtual int GetWidth() const
    {
      return m_MipSurfaceArray[0][0]->GetWidth();
    }
    virtual int GetHeight() const
    {
      return m_MipSurfaceArray[0][0]->GetHeight();
    }
    virtual int GetDepth() const
    {
      return m_Depth;
    }
    virtual BitmapFormat GetFormat() const
    {
      return m_MipSurfaceArray[0][0]->GetFormat();
    }
    virtual bool IsNull() const
    {
      return m_MipSurfaceArray[0][0]->IsNull();
    }

  private:
    void ClearData();
    int m_NumMipmap;
    int m_Depth;
    std::vector<ImageSurface *> *m_MipSurfaceArray;
  };

  class NAnimatedTextureData: public NBitmapData
  {
  public:
    NAnimatedTextureData(BitmapFormat f = BITFMT_R8G8B8A8, int width = 16, int height = 16, int slice = 1 /*, unsigned int NumMipmap = 1*/);
    virtual ~NAnimatedTextureData();
    //! Copy constructor
    NAnimatedTextureData(const NAnimatedTextureData &);
    //! Assignment constructor
    NAnimatedTextureData &operator = (const NAnimatedTextureData &);

    virtual void Allocate(BitmapFormat f, int width, int height, int slice, int NumMipmap = 1);
    virtual void AllocateCheckBoardTexture(int width, int height, int slice, int NumMipmap, Color color0, Color color1, int TileWidth = 4, int TileHeight = 4);
    virtual void AllocateColorTexture(int width, int height, int slice, int NumMipmap, Color color0 = Color(0xFFFFFFF));

    virtual const ImageSurface &GetSurface(int /* MipLevel */, int /* slice */) const
    {
      nuxAssertMsg(0, "[NAnimatedTextureData::GetSurface] Use GetSurface(unsigned int Frame) for NAnimatedTextureData.");
      return GetSurface(0);
    }
    virtual ImageSurface &GetSurface(int /* MipLevel */, int /* slice */)
    {
      nuxAssertMsg(0, "[NAnimatedTextureData::GetSurface] Use GetSurface(unsigned int Frame) for NAnimatedTextureData.");
      return GetSurface(0);
    }
    virtual const ImageSurface &GetSurface(int Frame) const
    {
      nuxAssert(Frame >= 0);
      nuxAssert(Frame < m_Depth);
      return *m_MipSurfaceArray[0][Frame];
    }
    virtual ImageSurface &GetSurface(int Frame)
    {
      nuxAssert(Frame >= 0);
      nuxAssert(Frame < m_Depth);
      return const_cast<ImageSurface &> ((const_cast< const NAnimatedTextureData * > (this))->GetSurface(Frame));
    }

    bool SetSurface(int face, int MipLevel, const ImageSurface &object);

    virtual bool IsAnimatedTextureData() const
    {
      return true;
    }

    int GetFrameTime(int Frame) const
    {
      nuxAssert(Frame >= 0);
      nuxAssert(Frame < m_Depth);
      return m_FrameTimeArray[Frame];
    }

    void AddFrameTime(unsigned int FrameTime)
    {
      m_FrameTimeArray.push_back(FrameTime);
    }

    int GetNumMipmap() const;
    virtual int GetWidth() const
    {
      return m_MipSurfaceArray[0][0]->GetWidth();
    }
    virtual int GetHeight() const
    {
      return m_MipSurfaceArray[0][0]->GetHeight();
    }
    virtual int GetDepth() const
    {
      return m_Depth;
    }
    virtual BitmapFormat GetFormat() const
    {
      return m_MipSurfaceArray[0][0]->GetFormat();
    }
    virtual bool IsNull() const
    {
      return m_MipSurfaceArray[0][0]->IsNull();
    }

  private:
    void ClearData();
    int m_NumMipmap;
    int m_Depth;
    std::vector<ImageSurface *> *m_MipSurfaceArray;
    std::vector<unsigned int> m_FrameTimeArray;
  };


  struct ImageInfo
  {
    bool         isDelegate;        // true if delegate knows this format
    int width ;            // Image size(if known)
    int height;
    int bytes_per_pixel;   // Bytes per pixel(if known)
    int planes;            // Number of planes(if known) 0=mono, 3=color
    std::string  format;            // Additional image format information
  };

  void MakeCheckBoardImage(ImageSurface& Image,
                           int width, int height,
                           Color const& dark, Color const& light,
                           int TileWidth = 4, int TileHeight = 4);

  bool HasOpenEXRSupport();

  /*!
      Return and object that has to be destroyed with delete.

      @return A bitmap source. Destroy it with delete.
  */
  NBitmapData* LoadGdkPixbuf(GdkPixbuf *pixbuf);

  /*!
      Return and object that has to be destroyed with delete.

      @return A bitmap source. Destroy it with delete.
  */
  NBitmapData* LoadImageFile(const TCHAR *Filename);

}

#endif // IMAGE_H
