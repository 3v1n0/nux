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
    t_u32 GetElementCount()
    {
      return 0;
    }

    //! Get the number of bytes per elements.
    t_u32 GetElementSize()
    {
      return 0;
    }

    void GetSize() {};

  private:
    void *m_RawDataPtr;
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
    ~ImageSurface();
    ImageSurface (BitmapFormat format, t_u32 width, t_u32 height);
    ImageSurface (const ImageSurface &);
    ImageSurface &operator = (const ImageSurface &);

    bool IsNull() const;
    t_s32 GetWidth() const
    {
      return width_;
    }
    t_s32 GetHeight() const
    {
      return height_;
    }
    void Allocate (BitmapFormat format, t_s32 width, t_s32 height);
    void Write32b (t_s32 i, t_s32 j, t_u32 value);
    void Write24b (t_s32 i, t_s32 j, t_u32 value);
    void Write16b (t_s32 i, t_s32 j, t_u16 value);
    void Write8b (t_s32 i, t_s32 j, t_u8 value);
    void Write (t_s32 i, t_s32 j, t_u8 r, t_u8 g, t_u8 b, t_u8 a);
    //! Read an element of the surface.
    /*!
        Return a 32 bits value representing the image element at coordinates (i, j).
        For the RGBA format, the LSB of the returned value represent the read value, and the MSB represents the alpha value.
        | LSB: Red | Green | Blue | MSB: Alpha|
        @return The image element at coordinates (i, j).
    */
    t_u32 Read (t_s32 i, t_s32 j);
    //! Set all surface elements to 0.
    void Clear();
    //! Flip the surface horizontally.
    void FlipHorizontal();
    //! Flip the surface vertically.
    void FlipVertical();
    //! Returns the surface pitch.
    t_s32 GetPitch() const;
    t_s32 GetBlockHeight() const;
    t_s32 GetAlignment() const;
    t_s32 GetSize() const;
    BitmapFormat GetFormat() const;
    const t_u8 *GetPtrRawData() const;
    t_u8 *GetPtrRawData();

    static t_s32 GetLevelPitch (BitmapFormat format, t_s32 width, t_s32 height, t_s32 miplevel);
    static t_s32 GetLevelPitchNoMemAlignment (BitmapFormat format, t_s32 width, t_s32 height, t_s32 miplevel);
    static t_s32 GetLevelSize (BitmapFormat format, t_s32 width, t_s32 height, t_s32 miplevel);
    static t_s32 GetLevelSize (BitmapFormat format, t_s32 width, t_s32 height, t_s32 depth, t_s32 miplevel);
    static t_s32 GetLevelWidth (BitmapFormat format, t_s32 width, t_s32 miplevel);
    static t_s32 GetLevelHeight (BitmapFormat format, t_s32 height, t_s32 miplevel);
    static t_s32 GetLevelDim (BitmapFormat format, t_s32 length, t_s32 miplevel);
    static t_s32 GetNumMipLevel (BitmapFormat format, t_s32 width, t_s32 height);
    static t_s32 GetMemAlignment (BitmapFormat format);
    static t_s32 GetLevelBlockWidth (BitmapFormat format, t_s32 width, t_s32 miplevel);
    static t_s32 GetLevelBlockHeight (BitmapFormat format, t_s32 height, t_s32 miplevel); // for DXT

    // Image Processing
    //! Compute the average color of the image surface.
    /*!
        Sum up all the image elements and divide by the number of elements.
        @return The average color of the image.
    */
    struct Color AverageColor();

  private:
    void FlipDXTVertical();
    void SwapBlocks (void *byte1, void *byte2, t_s32 size);
    void FlipBlocksDXT1 (DXTColBlock *line, t_s32 numBlocks);
    void FlipBlocksDXT3 (DXTColBlock *line, t_s32 numBlocks);
    void FlipBlocksDXT5 (DXTColBlock *line, t_s32 numBlocks);
    void FlipDXT5Alpha (DXT5AlphaBlock *block);


    t_s32 width_;           //!< Image width
    t_s32 height_;          //!< Image height.
    BitmapFormat format_;   //!< Image format.
    t_s32 m_Pitch;          //!< Image pitch.
    t_s32 bpe_;             //!< Number of byte per element.
    t_s32 Alignment_;       //!< Data alignment.
    t_u8    *RawData_;
  };


  class NBitmapData
  {
  public:
    NBitmapData();
    virtual ~NBitmapData();

    virtual const ImageSurface &GetSurface (t_s32 MipLevel) const = 0;
    virtual ImageSurface &GetSurface (t_s32 MipLevel) = 0;
    virtual const ImageSurface &GetSurface (t_s32 face, t_s32 MipLevel) const = 0;
    virtual ImageSurface &GetSurface (t_s32 face, t_s32 MipLevel) = 0;

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

    virtual t_s32 GetNumMipmap() const = 0;
    virtual t_s32 GetWidth() const = 0;
    virtual t_s32 GetHeight() const = 0;
    virtual t_s32 GetDepth() const
    {
      return 0;
    }
    virtual BitmapFormat GetFormat() const = 0;
    virtual bool IsNull() const = 0;
    virtual t_s32 GetMemorySize() const
    {
      return m_TotalMemorySize;
    }

  protected:
    t_u32 m_TotalMemorySize;
//protected:
//    t_u32 m_Width;
//    t_u32 m_Height;
//    BitmapFormat m_Format;
  };

  class NTextureData: public NBitmapData
  {
  public:
    NTextureData (BitmapFormat f = BITFMT_R8G8B8A8, t_s32 width = 16, t_s32 height = 16, t_s32 NumMipmap = 1);
    virtual ~NTextureData();
    //! Copy constructor.
    NTextureData (const NTextureData &);
    //! Assignment constructor.
    NTextureData &operator = (const NTextureData &);

    virtual void Allocate (BitmapFormat f, t_s32 width, t_s32 height, t_s32 NumMipmap = 1);
    virtual void AllocateCheckBoardTexture (t_s32 width, t_s32 height, t_s32 NumMipmap, Color color0, Color color1, t_s32 TileWidth = 4, t_s32 TileHeight = 4);
    virtual void AllocateColorTexture (t_s32 width, t_s32 height, t_s32 NumMipmap, Color color0 = Color (0xFFFFFFF) );

    virtual const ImageSurface &GetSurface (t_s32 MipLevel) const
    {
      return *m_MipSurfaceArray[MipLevel];
    };
    virtual ImageSurface &GetSurface (t_s32 MipLevel)
    {
      return const_cast<ImageSurface &> ( (const_cast< const NTextureData * > (this) )->GetSurface (MipLevel) );
    }
    virtual const ImageSurface &GetSurface (t_s32 face, t_s32 MipLevel) const
    {
      //nuxAssertMsg(0, TEXT("[NTextureData::GetSurface] Use GetSurface(t_u32 MipLevel) for NTextureData."));
      return GetSurface (MipLevel);
    }
    virtual ImageSurface &GetSurface (t_s32 face, t_s32 MipLevel)
    {
      //nuxAssertMsg(0, TEXT("[NTextureData::GetSurface] Use GetSurface(t_u32 MipLevel) for NTextureData."));
      return GetSurface (MipLevel);
    }

    bool SetSurface (t_s32 MipLevel, const ImageSurface &object);

    virtual bool IsTextureData() const
    {
      return true;
    }

    virtual t_s32 GetNumMipmap() const;
    virtual t_s32 GetWidth() const
    {
      return m_MipSurfaceArray[0]->GetWidth();
    }
    virtual t_s32 GetHeight() const
    {
      return m_MipSurfaceArray[0]->GetHeight();
    }
    virtual BitmapFormat GetFormat() const
    {
      return m_MipSurfaceArray[0]->GetFormat();
    }
    virtual bool IsNull() const
    {
      return m_MipSurfaceArray[0]->IsNull();
    }

  private:
    t_s32 m_NumMipmap;
    std::vector<ImageSurface *> m_MipSurfaceArray;
    void ClearData();

    friend NBitmapData *read_tga_file (const TCHAR *file_name);
    friend NBitmapData *read_bmp_file (const TCHAR *file_name);
    friend NBitmapData *read_png_rgb (const TCHAR *filename);
    friend NBitmapData *read_png_rgba (const TCHAR *filename);

  };

  class NCubemapData: public NBitmapData
  {
  public:
    NCubemapData (BitmapFormat f = BITFMT_R8G8B8A8, t_s32 width = 16, t_s32 height = 16, t_s32 NumMipmap = 1);
    virtual ~NCubemapData();
    //! Copy constructor
    NCubemapData (const NCubemapData &);
    //! Assignment constructor
    NCubemapData &operator = (const NCubemapData &);

    virtual void Allocate (BitmapFormat f, t_s32 width, t_s32 height, t_s32 NumMipmap = 1);
    virtual void AllocateCheckBoardTexture (t_s32 width, t_s32 height, t_s32 NumMipmap, Color color0, Color color1, t_s32 TileWidth = 4, t_s32 TileHeight = 4);
    virtual void AllocateColorTexture (t_s32 width, t_s32 height, t_s32 NumMipmap, Color color0 = Color (0xFFFFFFF) );

    virtual const ImageSurface &GetSurface (t_s32 face, t_s32 MipLevel) const
    {
      return *m_MipSurfaceArray[face][MipLevel];
    };
    virtual ImageSurface &GetSurface (t_s32 face, t_s32 MipLevel)
    {
      return const_cast<ImageSurface &> ( (const_cast< const NCubemapData * > (this) )->GetSurface (face, MipLevel) );
    }
    virtual const ImageSurface &GetSurface (t_s32 MipLevel) const
    {
      nuxAssertMsg (0, TEXT ("[NCubemapData::GetSurface] Use GetSurface(t_u32 face, t_u32 MipLevel) for NCubemapData.") );
      return GetSurface (0, MipLevel);
    }
    virtual ImageSurface &GetSurface (t_s32 MipLevel)
    {
      nuxAssertMsg (0, TEXT ("[NCubemapData::GetSurface] Use GetSurface(t_u32 face, t_u32 MipLevel) for NCubemapData.") );
      return GetSurface (0, MipLevel);
    }
    bool SetSurface (t_s32 face, t_s32 MipLevel, const ImageSurface &object);

    virtual bool IsCubemapTextureData() const
    {
      return true;
    }

    virtual t_s32 GetNumMipmap() const;
    virtual t_s32 GetWidth() const
    {
      return m_MipSurfaceArray[0][0]->GetWidth();
    }
    virtual t_s32 GetHeight() const
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
    t_s32 m_NumMipmap;
    std::vector<ImageSurface *> m_MipSurfaceArray[6];
  };

  class NVolumeData: public NBitmapData
  {
  public:
    NVolumeData (BitmapFormat f = BITFMT_R8G8B8A8, t_s32 width = 16, t_s32 height = 16, t_s32 slice = 1, t_s32 NumMipmap = 1);
    virtual ~NVolumeData();
    //! Copy constructor
    NVolumeData (const NVolumeData &);
    //! Assignment constructor
    NVolumeData &operator = (const NVolumeData &);

    virtual void Allocate (BitmapFormat f, t_s32 width, t_s32 height, t_s32 slice, t_s32 NumMipmap = 1);
    virtual void AllocateCheckBoardTexture (t_s32 width, t_s32 height, t_s32 slice, t_s32 NumMipmap, Color color0, Color color1, t_s32 TileWidth = 4, t_s32 TileHeight = 4);
    virtual void AllocateColorTexture (t_s32 width, t_s32 height, t_s32 slice, t_s32 NumMipmap, Color color0 = Color (0xFFFFFFF) );

    virtual const ImageSurface &GetSurface (t_s32 MipLevel, t_s32 slice) const
    {
      return *m_MipSurfaceArray[MipLevel][slice];
    };
    virtual ImageSurface &GetSurface (t_s32 MipLevel, t_s32 slice)
    {
      return const_cast<ImageSurface &> ( (const_cast< const NVolumeData * > (this) )->GetSurface (MipLevel, slice) );
    }
    virtual const ImageSurface &GetSurface (t_s32 MipLevel) const
    {
      nuxAssertMsg (0, TEXT ("[NVolumeData::GetSurface] Use GetSurface(t_u32 MipLevel, t_u32 MipLevel) for NVolumeData.") );
      return GetSurface (MipLevel, 0);
    }
    virtual ImageSurface &GetSurface (t_s32 MipLevel)
    {
      nuxAssertMsg (0, TEXT ("[NVolumeData::GetSurface] Use GetSurface(t_u32 MipLevel, t_u32 MipLevel) for NVolumeData.") );
      return GetSurface (MipLevel, 0);
    }
    bool SetSurface (t_s32 face, t_s32 MipLevel, const ImageSurface &object);

    virtual bool IsVolumeTextureData() const
    {
      return true;
    }

    t_s32 GetNumMipmap() const;
    virtual t_s32 GetWidth() const
    {
      return m_MipSurfaceArray[0][0]->GetWidth();
    }
    virtual t_s32 GetHeight() const
    {
      return m_MipSurfaceArray[0][0]->GetHeight();
    }
    virtual t_s32 GetDepth() const
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
    t_s32 m_NumMipmap;
    t_s32 m_Depth;
    std::vector<ImageSurface *> *m_MipSurfaceArray;
  };

  class NAnimatedTextureData: public NBitmapData
  {
  public:
    NAnimatedTextureData (BitmapFormat f = BITFMT_R8G8B8A8, t_s32 width = 16, t_s32 height = 16, t_s32 slice = 1 /*, t_u32 NumMipmap = 1*/);
    virtual ~NAnimatedTextureData();
    //! Copy constructor
    NAnimatedTextureData (const NAnimatedTextureData &);
    //! Assignment constructor
    NAnimatedTextureData &operator = (const NAnimatedTextureData &);

    virtual void Allocate (BitmapFormat f, t_s32 width, t_s32 height, t_s32 slice, t_s32 NumMipmap = 1);
    virtual void AllocateCheckBoardTexture (t_s32 width, t_s32 height, t_s32 slice, t_s32 NumMipmap, Color color0, Color color1, t_s32 TileWidth = 4, t_s32 TileHeight = 4);
    virtual void AllocateColorTexture (t_s32 width, t_s32 height, t_s32 slice, t_s32 NumMipmap, Color color0 = Color (0xFFFFFFF) );

    virtual const ImageSurface &GetSurface (t_s32 MipLevel, t_s32 slice) const
    {
      nuxAssertMsg (0, TEXT ("[NAnimatedTextureData::GetSurface] Use GetSurface(t_u32 Frame) for NAnimatedTextureData.") );
      return GetSurface (0);
    }
    virtual ImageSurface &GetSurface (t_s32 MipLevel, t_s32 slice)
    {
      nuxAssertMsg (0, TEXT ("[NAnimatedTextureData::GetSurface] Use GetSurface(t_u32 Frame) for NAnimatedTextureData.") );
      return GetSurface (0);
    }
    virtual const ImageSurface &GetSurface (t_s32 Frame) const
    {
      nuxAssert (Frame >= 0);
      nuxAssert (Frame < m_Depth);
      return *m_MipSurfaceArray[0][Frame];
    }
    virtual ImageSurface &GetSurface (t_s32 Frame)
    {
      nuxAssert (Frame >= 0);
      nuxAssert (Frame < m_Depth);
      return const_cast<ImageSurface &> ( (const_cast< const NAnimatedTextureData * > (this) )->GetSurface (Frame) );
    }

    bool SetSurface (t_s32 face, t_s32 MipLevel, const ImageSurface &object);

    virtual bool IsAnimatedTextureData() const
    {
      return true;
    }

    t_s32 GetFrameTime (t_s32 Frame) const
    {
      nuxAssert (Frame >= 0);
      nuxAssert (Frame < m_Depth);
      return m_FrameTimeArray[Frame];
    }

    void AddFrameTime (t_u32 FrameTime)
    {
      m_FrameTimeArray.push_back (FrameTime);
    }

    t_s32 GetNumMipmap() const;
    virtual t_s32 GetWidth() const
    {
      return m_MipSurfaceArray[0][0]->GetWidth();
    }
    virtual t_s32 GetHeight() const
    {
      return m_MipSurfaceArray[0][0]->GetHeight();
    }
    virtual t_s32 GetDepth() const
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
    t_s32 m_NumMipmap;
    t_s32 m_Depth;
    std::vector<ImageSurface *> *m_MipSurfaceArray;
    std::vector<t_u32> m_FrameTimeArray;
  };


  struct ImageInfo
  {
    bool         isDelegate;        // true if delegate knows this format
    t_s32 width ;            // Image size (if known)
    t_s32 height;
    t_s32 bytes_per_pixel;   // Bytes per pixel (if known)
    t_s32 planes;            // Number of planes (if known) 0=mono, 3=color
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
