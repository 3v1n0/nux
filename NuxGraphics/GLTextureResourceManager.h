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


#ifndef GLTEXTURERESOURCEMANAGER_H
#define GLTEXTURERESOURCEMANAGER_H

#include "GLResourceManager.h"
#include "IOpenGLBaseTexture.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

namespace nux
{

  class NTextureData;
  class BaseTexture;
  class CachedBaseTexture;

  /*!
   * Create and load a 2D texture filled with RGBA pixels loaded from the image
   * file pointed by filename. The loading uses the GdkPixbuf library which
   * implies that the supported formats depend on the GdkPixbuf loaders
   * installed on the client system.
   *
   * @max_size Specifies the maximal required size for the image to be loaded,
   * if the width or height exceeds that value, the image is scaled down
   * respecting the aspect ratio. A value of -1 means that no maximal value is
   * required.
   * @premultiply Specifies if the R, G and B color channels must be
   * premultiplied by the alpha channel before being uploaded to the texture.
   * Note that if there's no alpha channel, the argument is ignored.
   * @return The resulting texture.
   */
  BaseTexture* CreateTexture2DFromFile(const char* filename, int max_size,
                                        bool premultiply);

  /*!
   * Create and load a 2D texture filled with RGBA pixels loaded from the
   * GdkPixbuf pointed by pixbuf.
   *
   * @premultiply Specifies if the R, G and B color channels must be
   * premultiplied by the alpha channel before being uploaded to the texture.
   * Note that if there's no alpha channel, the argument is ignored.
   * @return The resulting texture.
   */
  BaseTexture* CreateTexture2DFromPixbuf(GdkPixbuf* pixbuf, bool premultiply);

  // FIXME(loicm) Should be deprecated.
  BaseTexture* CreateTextureFromPixbuf(GdkPixbuf* pixbuf);

  BaseTexture* CreateTextureFromFile(const char* TextureFilename);
  BaseTexture* CreateTextureFromBitmapData(const NBitmapData* BitmapData);

  BaseTexture* LoadTextureFromFile(const std::string& filename);

  //! Abstract base class for textures.
  class BaseTexture: public ResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(BaseTexture, ResourceData);

    BaseTexture(NUX_FILE_LINE_PROTO);
    virtual ~BaseTexture();

    /*!
      Update the texture with the provided Bitmap data. In doing so, if the texture as been cached in the resource manager, then
      the the DeviceTexture inside the CachedTexture will no longer be returned by GetDeviceTexture(). Instead a new device texture will
      be returned.

      @BitmapData Pointer to the bitmap data.
      @UpdateAndCacheResource If true, then the texture is cached immediately. If false, the texture will be cached the first time
      GetDeviceTexture() or GetCachedTexture() is called.
      @return True if there was no error during the update.
    */
    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true) = 0;

    /*!
      Update the texture with the provided filename. In doing so, if the texture as been cached in the resource manager, then
      the the DeviceTexture inside the CachedTexture will no longer be returned by GetDeviceTexture(). Instead a new device texture will
      be returned.

      @BitmapData Pointer to the bitmap data.
      @UpdateAndCacheResource If true, then the texture is cached immediately. If false, the texture will be cached the first time
      GetDeviceTexture() or GetCachedTexture() is called.
      @return True if there was no error during the update.
    */
    virtual bool Update(const char* filename, bool UpdateAndCacheResource = true) = 0;

    virtual void GetData(void* Buffer, int MipIndex, int StrideY, int face = 0) = 0;

    /*!
      @return The texture width.
    */
    virtual int GetWidth() const = 0;

    /*!
      @return The texture height.
    */
    virtual int GetHeight() const = 0;

    /*!
      @return The texture depth. Return 1 for Texture2D, TextureCube, TextureRenctangle. TextureVolume and TextureFrameAnimation have a depth equal or greater than 1.
      For TextureFrameAnimation the depth represents the number of frames.
    */
    virtual int GetDepth() const
    {
      return 1;
    }

    /*!
      @return True if the width and height of the texture are powers of two.
    */
    virtual bool IsPowerOfTwo() const = 0;

    /*!
      @return The texture data format.
    */
    virtual BitmapFormat GetFormat() const = 0;

    /*!
      @return The number of mip maps in the texture.
    */
    virtual int GetNumMipLevel() const = 0;

    /*!
      @return True if the texture storage contains valid bitmap data.
    */
    virtual bool IsNull() const = 0;

    virtual BaseTexture* Clone() const = 0;

    /*!
        Cache the texture if it hasn't been been already and return the device texture.
        \sa IOpenGLBaseTexture;

        @return The device texture.
    */
    ObjectPtr<IOpenGLBaseTexture> GetDeviceTexture();

    /*!
        Cache the texture if it hasn't been been already and return the cached texture.
        \sa CachedBaseTexture;

        @return The cached texture.
    */
    ObjectPtr<CachedBaseTexture> GetCachedTexture();
  };

  //! General Texture
  /*!
      The class of texture that cover power-of-two and non-power-of-two dimensions.
  */
  class Texture2D: public BaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(Texture2D, BaseTexture);

  public:
    Texture2D(NUX_FILE_LINE_PROTO); // TODO: Why can we create a texture without a texture?
    Texture2D(const NTextureData& Image, NUX_FILE_LINE_PROTO);
    ~Texture2D();

    /*!
        Update the hardware resources associated to this with the provided texture data.
        @param BitmapData The texture data to update into the hardware resource.
        @param UpdateAndCacheResource if True, then the texture data is loaded into this object, and the caching into
        hardware data is done right away. If false, the caching is done latter by calling
        GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(this);
        @return True is there was not error.
    */
    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);

    /*!
        Update the hardware resources associated to this object with the data associated to the file name.
        @param Filename File name of texture data to update into the hardware resource.
        @param UpdateAndCacheResource if True, then the texture data is loaded into this object, and the caching into
        hardware data is done right away. If false, the caching is done latter by calling
        GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(this);
        @return True is there was not error.
    */
    virtual bool Update(const char* Filename, bool UpdateAndCacheResource = true);

    /*!
      @return True if the texture storage contains valid bitmap data.
    */
    virtual bool IsNull() const
    {
      return _image.IsNull();
    }

    void GetData(void* Buffer, int MipIndex, int StrideY, int face = 0);

    /*!
      @return The number of mip maps in the texture.
    */
    int GetNumMipLevel() const
    {
      return _image.GetNumMipmap();
    }
    /*!
      @return The texture width.
    */
    int GetWidth() const
    {
      return _image.GetWidth();
    }

    /*!
      @return The texture height.
    */
    int GetHeight() const
    {
      return _image.GetHeight();
    }

    /*!
      @return The texture data format.
    */
    BitmapFormat GetFormat() const
    {
      return _image.GetFormat();
    }

    /*!
      @return True if the width and height of the texture are powers of two.
    */
    bool IsPowerOfTwo() const
    {
      return IsPowerOf2(_image.GetWidth()) && IsPowerOf2(_image.GetHeight());
    }

    virtual Texture2D* Clone() const;

  private:
    NTextureData _image; //!< Storage for the texture data
  };

  class TextureRectangle: public BaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(TextureRectangle, BaseTexture);

  public:
    TextureRectangle(NUX_FILE_LINE_PROTO);        // TODO: Why can we create a texture without a texture?
    TextureRectangle(const NTextureData& Image, NUX_FILE_LINE_PROTO);
    ~TextureRectangle();

    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);
    virtual bool Update(const char* filename, bool UpdateAndCacheResource = true);

    /*!
      @return True if the texture storage contains valid bitmap data.
    */
    virtual bool IsNull() const
    {
      return _image.IsNull();
    }

    void GetData(void* Buffer, int MipIndex, int StrideY, int face = 0);

    /*!
      @return The number of mip maps in the texture.
    */
    int GetNumMipLevel() const
    {
      return _image.GetNumMipmap();
    }

    /*!
      @return The texture width.
    */
    int GetWidth() const
    {
      return _image.GetWidth();
    }

    /*!
      @return The texture height.
    */
    int GetHeight() const
    {
      return _image.GetHeight();
    }

    /*!
      @return The texture data format.
    */
    BitmapFormat GetFormat() const
    {
      return _image.GetFormat();
    }

    /*!
      @return True if the width and height of the texture are powers of two.
    */
    bool IsPowerOfTwo() const
    {
      return IsPowerOf2(_image.GetWidth()) && IsPowerOf2(_image.GetHeight());
    }

    virtual TextureRectangle* Clone() const;

  private:
    NTextureData _image;
  };

  class TextureCube: public BaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(TextureCube, BaseTexture);

  public:
    TextureCube(NUX_FILE_LINE_PROTO);       // TODO: why can we have a texture without a texture?
    TextureCube(const NCubemapData& Image);
    ~TextureCube();

    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);
    virtual bool Update(const char* filename, bool UpdateAndCacheResource = true);

    /*!
      @return True if the texture storage contains valid bitmap data.
    */
    virtual bool IsNull() const
    {
      return _image.IsNull();
    }
    void GetData(void* Buffer, int MipIndex, int StrideY, int face = 0);

    /*!
      @return The number of mip maps in the texture.
    */
    int GetNumMipLevel() const
    {
      return _image.GetNumMipmap();
    }

    /*!
      @return The texture width.
    */
    int GetWidth() const
    {
      return _image.GetWidth();
    }

    /*!
      @return The texture height.
    */
    int GetHeight() const
    {
      return _image.GetHeight();
    }

    /*!
      @return The texture data format.
    */
    BitmapFormat GetFormat() const
    {
      return _image.GetFormat();
    }

    /*!
      @return True if the width and height of the texture are powers of two.
    */
    bool IsPowerOfTwo() const
    {
      return IsPowerOf2(_image.GetWidth()) && IsPowerOf2(_image.GetHeight());
    }

    virtual TextureCube* Clone() const;

  private:
    NCubemapData _image;
  };

  class TextureVolume: public BaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(TextureVolume, BaseTexture);

  public:
    TextureVolume(NUX_FILE_LINE_PROTO);       // TODO: why can we have a texture without a texture?
    TextureVolume(const NVolumeData& Image);
    ~TextureVolume();

    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);
    virtual bool Update(const char* filename, bool UpdateAndCacheResource = true);

    /*!
      @return True if the texture storage contains valid bitmap data.
    */
    virtual bool IsNull() const
    {
      return _image.IsNull();
    }
    void GetData(void* Buffer, int MipIndex, int StrideY, int slice = 0);
    
    /*!
      @return The number of mip maps in the texture.
    */
    int GetNumMipLevel() const
    {
      return _image.GetNumMipmap();
    }

    /*!
      @return The texture width.
    */
    int GetWidth() const
    {
      return _image.GetWidth();
    }

    /*!
      @return The texture height.
    */
    int GetHeight() const
    {
      return _image.GetHeight();
    }

    /*!
      @return The texture depth. 
      For TextureFrameAnimation the depth represents the number of frames.
    */
    int GetDepth() const
    {
      return _image.GetDepth();
    }

    /*!
      @return The texture data format.
    */
    BitmapFormat GetFormat() const
    {
      return _image.GetFormat();
    }

    /*!
      @return True if the width and height of the texture are powers of two.
    */
    bool IsPowerOfTwo() const
    {
      return IsPowerOf2(_image.GetWidth()) && IsPowerOf2(_image.GetHeight());
    }

    virtual TextureVolume* Clone() const;

  private:
    NVolumeData _image;
  };

  class TextureFrameAnimation: public BaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(TextureFrameAnimation, BaseTexture);

  public:
    TextureFrameAnimation(NUX_FILE_LINE_PROTO);       // TODO: why can we have a texture without a texture?
    TextureFrameAnimation(const NAnimatedTextureData& Image);
    ~TextureFrameAnimation();

    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);
    virtual bool Update(const char* filename, bool UpdateAndCacheResource = true);

    /*!
      @return True if the texture storage contains valid bitmap data.
    */
    virtual bool IsNull() const
    {
      return _image.IsNull();
    }
    void GetData(void* Buffer, int MipIndex, int StrideY, int slice = 0);
    int GetFrameTime(int Frame);

    /*!
      @return The number of mip maps in the texture.
    */
    int GetNumMipLevel() const
    {
      return _image.GetNumMipmap();
    }

    /*!
      @return The texture width.
    */
    int GetWidth() const
    {
      return _image.GetWidth();
    }

    /*!
      @return The texture height.
    */
    int GetHeight() const
    {
      return _image.GetHeight();
    }

    /*!
      @return The number of animation frames in the texture.
    */
    int GetDepth() const
    {
      return _image.GetDepth();
    }

    /*!
      @return The texture data format.
    */
    BitmapFormat GetFormat() const
    {
      return _image.GetFormat();
    }

    /*!
      @return true if the width and heigth of the texture are powers of two.
    */
    bool IsPowerOfTwo() const
    {
      return IsPowerOf2(_image.GetWidth()) && IsPowerOf2(_image.GetHeight());
    }

    virtual TextureFrameAnimation* Clone() const;

  private:
    NAnimatedTextureData _image;
  };

  class CachedBaseTexture: public CachedResourceData
  {
    NUX_DECLARE_OBJECT_TYPE(CachedBaseTexture, CachedResourceData);
  public:
    ObjectPtr < IOpenGLBaseTexture > m_Texture;

    CachedBaseTexture(NResourceSet* ResourceManager);
    ~CachedBaseTexture();

    virtual void LoadMipLevel(BaseTexture* SourceTexture, int MipLevel) = 0;

    virtual bool UpdateResource(ResourceData* Resource);

    bool RecreateTexture(BaseTexture* Source);

    virtual void UpdateTexture(BaseTexture* Source) = 0;

    unsigned int SourceWidth;
    unsigned int SourceHeight;
    unsigned int SourceDepth;
    BitmapFormat SourceFormat;
  };

  class CachedTexture2D: public CachedBaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(CachedTexture2D, CachedBaseTexture);
  public:
    CachedTexture2D(NResourceSet* ResourceManager, Texture2D* SourceTexture);
    ~CachedTexture2D();

    virtual void UpdateTexture(BaseTexture* Source);
    virtual void LoadMipLevel(BaseTexture* SourceTexture, int MipLevel);
  };

  class CachedTextureRectangle: public CachedBaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(CachedTextureRectangle, CachedBaseTexture);
  public:
    CachedTextureRectangle(NResourceSet* ResourceManager, TextureRectangle* SourceTexture);
    ~CachedTextureRectangle();

    virtual void UpdateTexture(BaseTexture* Source);
    virtual void LoadMipLevel(BaseTexture* SourceTexture, int MipLevel);
  };

  class CachedTextureCube: public CachedBaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(CachedTextureCube, CachedBaseTexture);
  public:
    CachedTextureCube(NResourceSet* ResourceManager, TextureCube* SourceTexture);
    ~CachedTextureCube();

    virtual void UpdateTexture(BaseTexture* Source);
    virtual void LoadMipLevel(BaseTexture* SourceTexture, int MipLevel);
  };

  class CachedTextureVolume: public CachedBaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(CachedTextureVolume, CachedBaseTexture);
  public:
    CachedTextureVolume(NResourceSet* ResourceManager, TextureVolume* SourceTexture);
    ~CachedTextureVolume();

    virtual void UpdateTexture(BaseTexture* Source);
    virtual void LoadMipLevel(BaseTexture* SourceTexture, int MipLevel);
  };

  class CachedTextureFrameAnimation: public CachedBaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(CachedTextureFrameAnimation, CachedBaseTexture);
  public:
    CachedTextureFrameAnimation(NResourceSet* ResourceManager, TextureFrameAnimation* SourceTexture);
    ~CachedTextureFrameAnimation();

    virtual void UpdateTexture(BaseTexture* Source);
    virtual void LoadMipLevel(BaseTexture* SourceTexture, int MipLevel);
  };

}

#endif // GLTEXTURERESOURCEMANAGER_H
