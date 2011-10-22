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


#include "GLResource.h"
#include "GLResourceManager.h"
#include "GpuDevice.h"
#include "GraphicsEngine.h"
#include "GLTextureResourceManager.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(BaseTexture);
  NUX_IMPLEMENT_OBJECT_TYPE(Texture2D);
  NUX_IMPLEMENT_OBJECT_TYPE(TextureRectangle);
  NUX_IMPLEMENT_OBJECT_TYPE(TextureCube);
  NUX_IMPLEMENT_OBJECT_TYPE(TextureVolume);
  NUX_IMPLEMENT_OBJECT_TYPE(TextureFrameAnimation);

  NUX_IMPLEMENT_OBJECT_TYPE(CachedBaseTexture);
  NUX_IMPLEMENT_OBJECT_TYPE(CachedTexture2D);
  NUX_IMPLEMENT_OBJECT_TYPE(CachedTextureRectangle);
  NUX_IMPLEMENT_OBJECT_TYPE(CachedTextureCube);
  NUX_IMPLEMENT_OBJECT_TYPE(CachedTextureVolume);
  NUX_IMPLEMENT_OBJECT_TYPE(CachedTextureFrameAnimation);

  /*! Up cast a Resource.
      The source must be derived from the destination type
      @param 	T       Destination type.
      @param	U       Source type.
      @return	        The casted to the destination type
  */
  template < class T, class U >
  static T *UpCastResource(U *Src)
  {
    if (!Src || !Src->Type().IsDerivedFromType(T::StaticObjectType))
      nuxError("[UpCastResource] Cast of %s to %s failed", U::StaticObjectType.name, T::StaticObjectType.name);

    return(T *) Src;
  }

  BaseTexture *CreateTexture2DFromPixbuf(GdkPixbuf *pixbuf, bool premultiply)
  {
    const unsigned int rowstride = gdk_pixbuf_get_rowstride(pixbuf);
    const unsigned int width = gdk_pixbuf_get_width(pixbuf);
    const unsigned int height = gdk_pixbuf_get_height(pixbuf);

    // Put the RGB or RGBA pixels in a RGBA texture data object taking care
    // of alpha premultiplication if requested.
    // FIXME(loicm) Implies a useless copy. NTextureData should be able to
    //     take ownership of pre-allocated memory.
    // FIXME(loicm) Add support for big-endian systems. For performance
    //     reasons, pixels are loaded by block of 4 bytes with the color
    //     components bit-shifted considering little-endian ordering. Nux
    //     doesn't seem to be big-endian aware anyway.
    // FIXME(loicm) Surface::Write32b does branching, splits the 32-bit value
    //     as four 8-bit values(using bit-shifts) that are then stored
    //     separately, that's slow considering it's meant to be used a lot in
    //     deep loops.
    NTextureData *data = new NTextureData(BITFMT_R8G8B8A8, width, height, 1);
    ImageSurface &surface = data->GetSurface(0);
    if (gdk_pixbuf_get_has_alpha(pixbuf) == TRUE)
    {
      unsigned char *pixels_u8 = gdk_pixbuf_get_pixels(pixbuf);
      unsigned int *pixels_u32 = reinterpret_cast<unsigned int *> (pixels_u8);

      if (premultiply == true)
      {
        // Copy from pixbuf(RGBA) to surface(premultiplied RGBA).
        for (unsigned int i = 0; i < height; i++)
        {
          for (unsigned int j = 0; j < width; j++)
          {
            const unsigned int pixel = pixels_u32[j];
            const unsigned int a = pixel >> 24;
            if (a == 0)
              surface.Write32b(j, i, 0);
            else
            {
              const unsigned int b = (((pixel >> 16) & 0xff) * a) / 255;
              const unsigned int g = (((pixel >> 8) & 0xff) * a) / 255;
              const unsigned int r = ((pixel & 0xff) * a) / 255;
              const unsigned int p = a << 24 | b << 16 | g << 8 | r;
              surface.Write32b(j, i, p);
            }
          }
          pixels_u8 += rowstride;
          pixels_u32 = reinterpret_cast<unsigned int *> (pixels_u8);
        }
      }
      else
      {
        // Copy from pixbuf(RGBA) to surface(RGBA).
        for (unsigned int i = 0; i < height; i++)
        {
          for (unsigned int j = 0; j < width; j++)
            surface.Write32b(j, i, pixels_u32[j]);
          pixels_u8 += rowstride;
          pixels_u32 = reinterpret_cast<unsigned int *> (pixels_u8);
        }
      }
    }
    else
    {
      // Copy from pixbuf(RGB) to surface(RGBA).
      unsigned char *pixels = gdk_pixbuf_get_pixels(pixbuf);
      for (unsigned int i = 0; i < height; i++)
      {
        for (unsigned int j = 0; j < width; j++)
        {
          const unsigned char r = pixels[j*3];
          const unsigned char g = pixels[j*3+1];
          const unsigned char b = pixels[j*3+2];
          surface.Write(j, i, r, g, b, 0xff);
        }
        pixels += rowstride;
      }
    }

    // Create a 2D texture and upload the pixels.
    BaseTexture *texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    texture->Update(data);
    
    delete data; // data is deleted as texture->Update() copies it.
    return texture;
  }

  BaseTexture* CreateTexture2DFromFile(const char *filename, int max_size,
                                        bool premultiply)
  {
    GError *error = NULL;
    GdkPixbuf *pixbuf =
      gdk_pixbuf_new_from_file_at_size(filename, max_size, max_size, &error);
    if (error == NULL)
    {
      BaseTexture *texture = CreateTexture2DFromPixbuf(pixbuf, premultiply);
      g_object_unref(pixbuf);
      return texture;
    }
    else
    {
      nuxDebugMsg("%s", error->message);
      return NULL;
    }
  }

  BaseTexture *CreateTextureFromPixbuf(GdkPixbuf *pixbuf)
  {
    NBitmapData *BitmapData = LoadGdkPixbuf(pixbuf);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, 0);

    if (BitmapData->IsTextureData())
    {
      BaseTexture *texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
      texture->Update(BitmapData);
      return texture;
    }
    delete BitmapData;
    return 0;
  }

  BaseTexture *CreateTextureFromFile(const char *TextureFilename)
  {
    BaseTexture* texture = nullptr;

    NBitmapData* BitmapData = LoadImageFile(TextureFilename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, 0);

    if (BitmapData->IsTextureData())
    {
      texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    }
    else if (BitmapData->IsCubemapTextureData())
    {
      texture = new TextureCube();
    }
    else if (BitmapData->IsVolumeTextureData())
    {
      texture = new TextureVolume();
    }
    else if (BitmapData->IsAnimatedTextureData())
    {
      texture = new TextureFrameAnimation();
    }

    if (texture)
    {
      texture->Update(BitmapData);
    }
    else
    {
      nuxDebugMsg("[CreateTextureFromFile] Invalid texture format type for file(%s)", TextureFilename);
    }

    delete BitmapData;
    return texture;
  }

  BaseTexture *CreateTextureFromBitmapData(const NBitmapData *BitmapData)
  {
    if (BitmapData == 0)
      return 0;

    if (BitmapData->IsTextureData())
    {
      BaseTexture *texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
      texture->Update(BitmapData);
      return texture;
    }
    else if (BitmapData->IsCubemapTextureData())
    {
      TextureCube *texture = new TextureCube();
      texture->Update(BitmapData);
      return texture;
    }
    else if (BitmapData->IsVolumeTextureData())
    {
      TextureVolume *texture = new TextureVolume();
      texture->Update(BitmapData);
      return texture;
    }
    else if (BitmapData->IsAnimatedTextureData())
    {
      TextureFrameAnimation *texture = new TextureFrameAnimation();
      texture->Update(BitmapData);
      return texture;
    }
    return 0;
  }

  BaseTexture* LoadTextureFromFile(const std::string &filename)
  {
    NBitmapData* bitmap = LoadImageFile(filename.c_str());

    if (bitmap == NULL)
      return NULL;

    BaseTexture *texture = CreateTextureFromBitmapData(bitmap);
    return texture;
  }

  BaseTexture::BaseTexture(NUX_FILE_LINE_DECL)
    : ResourceData(NUX_FILE_LINE_PARAM)
  {

  }

  BaseTexture::~BaseTexture()
  {

  }

  ObjectPtr < IOpenGLBaseTexture > BaseTexture::GetDeviceTexture()
  {
    ObjectPtr<CachedBaseTexture> CachedTexture = GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(this);
    return CachedTexture->m_Texture;
  }

  ObjectPtr<CachedBaseTexture> BaseTexture::GetCachedTexture()
  {
    return GetGraphicsDisplay()->GetGraphicsEngine()->CacheResource(this);
  }

  Texture2D::Texture2D(NUX_FILE_LINE_DECL)
    : BaseTexture(NUX_FILE_LINE_PARAM)
  {
  }

  Texture2D::Texture2D(const Texture2D &texture, NUX_FILE_LINE_DECL)
    : BaseTexture(NUX_FILE_LINE_PARAM)
  {
    _image = texture._image;
  }

  Texture2D::Texture2D(const NTextureData &texture_data, NUX_FILE_LINE_DECL)
    : BaseTexture(NUX_FILE_LINE_PARAM)
  {
    _image = texture_data;
  }

  Texture2D &Texture2D::operator = (const Texture2D &texture)
  {
    if (this == &texture)
      return *this;   // Handle self assignment

    _image = texture._image;
    return *this;
  }

  Texture2D::~Texture2D()
  {
  }

  bool Texture2D::Update(const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg(BitmapData, "[Texture2D::Update] Argument BitmapData is NULL.");
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);

    if (!BitmapData->IsTextureData())
    {
      nuxAssertMsg(0, "[Texture2D::Update] Argument BitmapData is not a 2D texture");
      return false;
    }

    _image = *static_cast<const NTextureData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture(CachedTexture2D) associated with this object if any.
      GetGraphicsDisplay()->GetGraphicsEngine()->UpdateResource(this);
    }

    return true;
  }

  bool Texture2D::Update(const char *filename, bool UpdateAndCacheResource)
  {
    NBitmapData *BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, "[Texture2D::Update] Bitmap for file(%s) is NULL.", filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    bool ret = Update(BitmapData, UpdateAndCacheResource);
    NUX_SAFE_DELETE(BitmapData);
    return ret;
  }

  void Texture2D::GetData(void *Buffer, int MipIndex, int StrideY, int face)
  {
    BYTE	            *Dest        = (BYTE *) Buffer;
    const BYTE         *Src         = _image.GetSurface(MipIndex).GetPtrRawData();
    int                 RowByteSize = _image.GetSurface(MipIndex).GetPitch();
    int                 NumRows     = _image.GetSurface(MipIndex).GetBlockHeight();

    for ( int Y = 0; Y < NumRows; Y++ )
    {
      // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
      // they contain the same amount of valid data since they have the same width, height and format.
      Memcpy(Dest + Y * StrideY, &Src[Y * RowByteSize], Min(RowByteSize, StrideY));
    }
  }

  BaseTexture* Texture2D::Clone() const
  {
    Texture2D* texture = new Texture2D(*this);
    return texture;
  }

  CachedBaseTexture::CachedBaseTexture(NResourceSet *ResourceManager)
    :   CachedResourceData(ResourceManager),
    SourceWidth(0),
    SourceHeight(0),
    SourceDepth(0),
    SourceFormat(BITFMT_UNKNOWN)
  {

  }

  CachedBaseTexture::~CachedBaseTexture()
  {
    m_Texture.Release();
  }

  bool CachedBaseTexture::UpdateResource(ResourceData *Resource)
  {
    UpdateTexture((BaseTexture *) Resource);
    return TRUE;
  }

  bool CachedBaseTexture::RecreateTexture(BaseTexture *Source)
  {
    int CurrentWidth   = m_Texture->GetWidth();
    int CurrentHeight  = m_Texture->GetHeight();
    int CurrentDepth   = m_Texture->GetDepth();
    int CurrentNumMipmap      = m_Texture->GetNumMipLevel();
    BitmapFormat CurrentFormat = m_Texture->GetPixelFormat();

    bool Recreate =
      (CurrentWidth       != Source->GetWidth()) ||
      (CurrentHeight      != Source->GetHeight()) ||
      (CurrentDepth       != Source->GetDepth()) ||
      (CurrentNumMipmap   != Source->GetNumMipLevel()) ||
      (CurrentFormat      != Source->GetFormat());

    return Recreate;
  }

  CachedTexture2D::CachedTexture2D(NResourceSet *ResourceManager, Texture2D *SourceTexture)
    : CachedBaseTexture(ResourceManager)
  {
    if (SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr <IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateTexture(SourceTexture->GetWidth(),
                SourceTexture->GetHeight(),
                SourceTexture->GetNumMipLevel(),
                SourceTexture->GetFormat(), NUX_TRACKER_LOCATION);

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel(SourceTexture, i);
    }
  }

  CachedTexture2D::~CachedTexture2D()
  {

  }

  void CachedTexture2D::UpdateTexture( BaseTexture *SourceTexture )
  {
    if ((SourceTexture == 0) || SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr <IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType(Texture2D::StaticObjectType))
    {
      nuxAssertMsg(0, "[Texture2D::UpdateTexture] Source texture is not of type Texture2D.");
      return;
    }

    if ( RecreateTexture(SourceTexture))
    {
      m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateTexture(SourceTexture->GetWidth(),
                  SourceTexture->GetHeight(),
                  SourceTexture->GetNumMipLevel(),
                  SourceTexture->GetFormat(), NUX_TRACKER_LOCATION);

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
  }

  void CachedTexture2D::LoadMipLevel(BaseTexture *SourceTexture, int MipLevel)
  {
    SURFACE_LOCKED_RECT LockedRect;
    ObjectPtr < IOpenGLTexture2D > Texture2D = m_Texture; //m_Texture.CastRef<IOpenGLTexture2D>();

    OGL_CALL( Texture2D->LockRect( MipLevel, &LockedRect, NULL));
    SourceTexture->GetData( LockedRect.pBits, MipLevel, LockedRect.Pitch );
    OGL_CALL( Texture2D->UnlockRect( MipLevel ));
  }

  TextureRectangle::TextureRectangle(NUX_FILE_LINE_DECL)
    : BaseTexture(NUX_FILE_LINE_PARAM)
  {
  }

  TextureRectangle::TextureRectangle(const TextureRectangle &texture)
  {
    _image = texture._image;
  }

  TextureRectangle::TextureRectangle(const NTextureData &BaseTexture)
  {
    _image = BaseTexture;
  }

  TextureRectangle &TextureRectangle::operator = (const TextureRectangle &texture)
  {
    if (this == &texture)
      return *this;   // Handle self assignment

    _image = texture._image;
    return *this;
  }

  TextureRectangle::~TextureRectangle()
  {

  }

  bool TextureRectangle::Update(const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg(BitmapData, "[TextureRectangle::Update] Argument BitmapData is NULL.");
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);

    if (!BitmapData->IsTextureData())
    {
      nuxAssertMsg(0, "[TextureRectangle::Update] Argument BitmapData is not a 2D texture");
      return false;
    }

    _image = *static_cast<const NTextureData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture(CachedTexture2D) associated with this object if any.
      GetGraphicsDisplay()->GetGraphicsEngine()->UpdateResource(this);
    }
    return true;
  }

  bool TextureRectangle::Update(const char *filename, bool UpdateAndCacheResource)
  {
    bool b = false;
    NBitmapData *BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, "[TextureRectangle::Update] Bitmap for file(%s) is NULL.", filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    b = Update(BitmapData);
    NUX_SAFE_DELETE(BitmapData);
    return b;
  }

  void TextureRectangle::GetData(void *Buffer, int MipIndex, int StrideY, int face)
  {
    BYTE	            *Dest        = (BYTE *) Buffer;
    const BYTE         *Src         = _image.GetSurface(MipIndex).GetPtrRawData();
    int                 RowByteSize = _image.GetSurface(MipIndex).GetPitch();
    int                 NumRows     = _image.GetSurface(MipIndex).GetBlockHeight();

    for ( int Y = 0; Y < NumRows; Y++ )
    {
      // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
      // they contain the same amount of valid data since they have the same width, height and format.
      Memcpy(Dest + Y * StrideY, &Src[Y * RowByteSize], Min(RowByteSize, StrideY));
    }
  }

  BaseTexture* TextureRectangle::Clone() const
  {
    TextureRectangle* texture = new TextureRectangle(*this);
    return texture;
  }

  CachedTextureRectangle::CachedTextureRectangle(NResourceSet *ResourceManager, TextureRectangle *SourceTexture)
    : CachedBaseTexture(ResourceManager)
  {
    if (SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr<IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateRectangleTexture(SourceTexture->GetWidth(),
                SourceTexture->GetHeight(),
                SourceTexture->GetNumMipLevel(),
                SourceTexture->GetFormat(), NUX_TRACKER_LOCATION);

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel(SourceTexture, i);
    }
  }

  CachedTextureRectangle::~CachedTextureRectangle()
  {

  }

  void CachedTextureRectangle::UpdateTexture( BaseTexture *SourceTexture )
  {
    if ((SourceTexture == 0) || SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr<IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType(TextureRectangle::StaticObjectType))
    {
      nuxAssertMsg(0, "[CachedTextureRectangle::UpdateTexture] Source texture is not of type TextureRectangle.");
      return;
    }

    if ( RecreateTexture(SourceTexture))
    {
      m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateRectangleTexture(SourceTexture->GetWidth(),
                  SourceTexture->GetHeight(),
                  SourceTexture->GetNumMipLevel(),
                  SourceTexture->GetFormat(), NUX_TRACKER_LOCATION);

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
  }

  void CachedTextureRectangle::LoadMipLevel(BaseTexture *SourceTexture, int MipLevel)
  {
    SURFACE_LOCKED_RECT		        LockedRect;
    ObjectPtr <IOpenGLRectangleTexture>	TextureRectangle = m_Texture; //m_Texture.CastRef<IOpenGLRectangleTexture>();

    OGL_CALL(TextureRectangle->LockRect( MipLevel, &LockedRect, NULL));
    SourceTexture->GetData( LockedRect.pBits, MipLevel, LockedRect.Pitch );
    OGL_CALL(TextureRectangle->UnlockRect( MipLevel ));
  }

  TextureCube::TextureCube(NUX_FILE_LINE_DECL)
    : BaseTexture(NUX_FILE_LINE_PARAM)
  {
  }

  TextureCube::TextureCube(const TextureCube &texture)
  {
    _image = texture._image;
  }

  TextureCube &TextureCube::operator = (const TextureCube &texture)
  {
    if (this == &texture)
      return *this;   // Handle self assignment

    _image = texture._image;
    return *this;
  }

  TextureCube::~TextureCube()
  {

  }

  bool TextureCube::Update(const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg(BitmapData, "[TextureCube::Update] Argument BitmapData is NULL.");
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);

    if (!BitmapData->IsCubemapTextureData())
    {
      nuxAssertMsg(0, "[TextureCube::Update] Argument BitmapData is not a Cube texture");
      return false;
    }

    _image = *static_cast<const NCubemapData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture(CachedTexture2D) associated with this object if any.
      GetGraphicsDisplay()->GetGraphicsEngine()->UpdateResource(this);
    }

    return true;
  }

  bool TextureCube::Update(const char *filename, bool UpdateAndCacheResource)
  {
    NBitmapData *BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, "[TextureCube::Update] Bitmap for file(%s) is NULL.", filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    bool ret = Update(BitmapData);
    NUX_SAFE_DELETE(BitmapData);
    return ret;
  }

  void TextureCube::GetData(void *Buffer, int MipIndex, int StrideY, int face)
  {
    BYTE	            *Dest        = (BYTE *) Buffer;
    const BYTE         *Src         = _image.GetSurface(face, MipIndex).GetPtrRawData();
    int                 RowByteSize = _image.GetSurface(face, MipIndex).GetPitch();
    int                 NumRows     = _image.GetSurface(face, MipIndex).GetBlockHeight();

    for ( int Y = 0; Y < NumRows; Y++ )
    {
      // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
      // they contain the same amount of valid data since they have the same width, height and format.
      Memcpy(Dest + Y * StrideY, &Src[Y * RowByteSize], Min(RowByteSize, StrideY));
    }
  }

  BaseTexture* TextureCube::Clone() const
  {
    TextureCube* texture = new TextureCube(*this);
    return texture;
  }

  CachedTextureCube::CachedTextureCube(NResourceSet *ResourceManager, TextureCube *SourceTexture)
    : CachedBaseTexture(ResourceManager)
  {
    if (SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr<IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateCubeTexture(SourceTexture->GetWidth(),
                SourceTexture->GetNumMipLevel(),
                SourceTexture->GetFormat());

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel(SourceTexture, i);
    }
  }

  CachedTextureCube::~CachedTextureCube()
  {

  }

  void CachedTextureCube::UpdateTexture( BaseTexture *SourceTexture )
  {
    if ((SourceTexture == 0) || SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr<IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType(TextureCube::StaticObjectType))
    {
      nuxAssertMsg(0, "[CachedTextureCube::UpdateTexture] Source texture is not of type TextureCube.");
      return;
    }

    if ( RecreateTexture(SourceTexture))
    {
      m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateCubeTexture(SourceTexture->GetWidth(),
                  SourceTexture->GetNumMipLevel(),
                  SourceTexture->GetFormat());

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
  }

  void CachedTextureCube::LoadMipLevel(BaseTexture *SourceTexture, int MipLevel)
  {
    SURFACE_LOCKED_RECT		LockedRect;
    ObjectPtr <IOpenGLCubeTexture> CubemapTexture = m_Texture; //m_Texture.CastRef<IOpenGLCubeTexture>();

    for (int face = CUBEMAP_FACE_POSITIVE_X; face < GL_TEXTURE_CUBE_MAP_NEGATIVE_Z + 1; face++)
    {
      OGL_CALL( CubemapTexture->LockRect(eCUBEMAP_FACES(face), MipLevel, &LockedRect, NULL));
      SourceTexture->GetData(LockedRect.pBits, MipLevel, LockedRect.Pitch, face - CUBEMAP_FACE_POSITIVE_X);
      OGL_CALL( CubemapTexture->UnlockRect(eCUBEMAP_FACES(face), MipLevel ));
    }
  }

  TextureVolume::TextureVolume(NUX_FILE_LINE_DECL)
    : BaseTexture(NUX_FILE_LINE_PARAM)
  {
  }

  TextureVolume::TextureVolume(const TextureVolume &texture)
  {
    _image = texture._image;
  }

  TextureVolume &TextureVolume::operator = (const TextureVolume &texture)
  {
    if (this == &texture)
      return *this;   // Handle self assignment

    _image = texture._image;
    return *this;
  }

  TextureVolume::~TextureVolume()
  {

  }

  bool TextureVolume::Update(const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg(BitmapData, "[TextureVolume::Update] Argument BitmapData is NULL.");
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);

    if (!BitmapData->IsVolumeTextureData())
    {
      nuxAssertMsg(0, "[TextureVolume::Update] Argument BitmapData is not a Volume texture");
      return false;
    }

    _image = *static_cast<const NVolumeData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture(CachedTexture2D) associated with this object if any.
      GetGraphicsDisplay()->GetGraphicsEngine()->UpdateResource(this);
    }

    return true;
  }

  bool TextureVolume::Update(const char *filename, bool UpdateAndCacheResource)
  {
    NBitmapData *BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, "[TextureVolume::Update] Bitmap for file(%s) is NULL.", filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    bool ret = Update(filename);
    NUX_SAFE_DELETE(BitmapData);
    return ret;
  }

  void TextureVolume::GetData(void *Buffer, int MipIndex, int StrideY, int slice)
  {
    BYTE               *Dest        = (BYTE *) Buffer;
//     const BYTE*         Src         = _image.GetSurface(MipIndex, slice).GetPtrRawData();
//     int                 RowByteSize = _image.GetSurface(MipIndex, slice).GetPitch();
//     int                 NumRows     = _image.GetSurface(MipIndex, slice).GetBlockHeight();

    for (int slice = 0; slice < ImageSurface::GetLevelDim(_image.GetFormat(), _image.GetDepth(), MipIndex); slice++)
    {
      const BYTE         *Src         = _image.GetSurface(MipIndex, slice).GetPtrRawData();
      int                 RowByteSize = _image.GetSurface(MipIndex, slice).GetPitch();
      int                 NumRows     = _image.GetSurface(MipIndex, slice).GetBlockHeight();

      for ( int Y = 0; Y < NumRows; Y++ )
      {
        // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
        // they contain the same amount of valid data since they have the same width, height and format.
        Memcpy(Dest + Y * StrideY, &Src[Y * RowByteSize], Min(RowByteSize, StrideY));
      }

      Dest += NumRows * StrideY;
    }

//     BYTE*	            Dest        = (BYTE*)Buffer;
//     const BYTE*         Src         = _image.GetSurface(MipIndex, slice).GetPtrRawData();
//     int                 RowByteSize = _image.GetSurface(MipIndex, slice).GetPitch();
//     int                 NumRows     = _image.GetSurface(MipIndex, slice).GetBlockHeight();
//
//     for ( int Y = 0; Y < NumRows; Y++ )
//     {
//         // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
//         // they contain the same amount of valid data since they have the same width, height and format.
//         Memcpy(Dest + Y * StrideY,&Src[Y * RowByteSize], Min(RowByteSize, StrideY));
//     }
  }

  BaseTexture* TextureVolume::Clone() const
  {
    TextureVolume* texture = new TextureVolume(*this);
    return texture;
  }

  CachedTextureVolume::CachedTextureVolume(NResourceSet *ResourceManager, TextureVolume *SourceTexture)
    :   CachedBaseTexture(ResourceManager)
  {
    if (SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr<IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateVolumeTexture(SourceTexture->GetWidth(),
                SourceTexture->GetHeight(),
                SourceTexture->GetDepth(),
                SourceTexture->GetNumMipLevel(),
                SourceTexture->GetFormat());

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel(SourceTexture, i);
    }
  }

  CachedTextureVolume::~CachedTextureVolume()
  {

  }

  void CachedTextureVolume::UpdateTexture( BaseTexture *SourceTexture )
  {
    if ((SourceTexture == 0) || SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr<IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType(TextureVolume::StaticObjectType))
    {
      nuxAssertMsg(0, "[CachedTextureVolume::UpdateTexture] Source texture is not of type TextureVolume.");
      return;
    }

    if ( RecreateTexture(SourceTexture))
    {
      m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateVolumeTexture(SourceTexture->GetWidth(),
                  SourceTexture->GetHeight(),
                  SourceTexture->GetDepth(),
                  SourceTexture->GetNumMipLevel(),
                  SourceTexture->GetFormat());

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
  }

  void CachedTextureVolume::LoadMipLevel(BaseTexture *SourceTexture, int MipLevel)
  {
    VOLUME_LOCKED_BOX       LockedBox;
    ObjectPtr <IOpenGLVolumeTexture> VolumeTexture = m_Texture; //m_Texture.CastRef<IOpenGLVolumeTexture>();
    //TextureVolume*     Source          = UpCastResource<TextureVolume, BaseTexture>(SourceTexture);

    //for(int slice = 0; slice < ImageSurface::GetLevelDim(Source->GetFormat(), Source->GetDepth(), MipLevel); slice++)
    {
      OGL_CALL( VolumeTexture->LockBox(MipLevel, &LockedBox, NULL));
      SourceTexture->GetData(LockedBox.pBits, MipLevel, LockedBox.RowPitch, 0);
      OGL_CALL( VolumeTexture->UnlockBox(MipLevel));
    }

//     for (int slice = 0; slice < depth; slice++)
//     {
//         OGL_CALL( VolumeTexture->LockRect(slice, MipLevel, &LockedRect, NULL));
//         SourceTexture->GetData(LockedRect.pBits, MipLevel, LockedRect.Pitch, slice);
//         OGL_CALL( VolumeTexture->UnlockRect(slice, MipLevel ));
//     }
  }

  TextureFrameAnimation::TextureFrameAnimation(NUX_FILE_LINE_DECL)
    : BaseTexture(NUX_FILE_LINE_PARAM)
  {
  }

  TextureFrameAnimation::TextureFrameAnimation(const TextureFrameAnimation &texture)
  {
    _image = texture._image;
  }

  TextureFrameAnimation &TextureFrameAnimation::operator = (const TextureFrameAnimation &texture)
  {
    if (this == &texture)
      return *this;   // Handle self assignment

    _image = texture._image;
    return *this;
  }

  TextureFrameAnimation::~TextureFrameAnimation()
  {

  }

  bool TextureFrameAnimation::Update(const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg(BitmapData, "[TextureFrameAnimation::Update] Argument BitmapData is NULL.");
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);

    if (!BitmapData->IsAnimatedTextureData())
    {
      nuxAssertMsg(0, "[TextureFrameAnimation::Update] Argument BitmapData is not a Animated texture");
      return false;
    }

    _image = *static_cast<const NAnimatedTextureData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture(CachedTexture2D) associated with this object if any.
      GetGraphicsDisplay()->GetGraphicsEngine()->UpdateResource(this);
    }

    return true;
  }

  bool TextureFrameAnimation::Update(const char *filename, bool UpdateAndCacheResource)
  {
    NBitmapData *BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, "[TextureFrameAnimation::Update] Bitmap for file(%s) is NULL.", filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    bool ret = Update(BitmapData);
    NUX_SAFE_DELETE(BitmapData);
    return ret;
  }

  void TextureFrameAnimation::GetData(void *Buffer, int MipIndex, int StrideY, int slice)
  {
    BYTE               *Dest        = (BYTE *) Buffer;
    //for(int slice = 0; slice < ImageSurface::GetLevelDim(_image.GetFormat(), _image.GetDepth(), MipIndex); slice++)
    {
      const BYTE         *Src         = _image.GetSurface(slice).GetPtrRawData();
      int                 RowByteSize = _image.GetSurface(slice).GetPitch();
      int                 NumRows     = _image.GetSurface(slice).GetBlockHeight();

      for (int Y = 0; Y < NumRows; Y++)
      {
        // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
        // they contain the same amount of valid data since they have the same width, height and format.
        Memcpy(Dest + Y * StrideY, &Src[Y * RowByteSize], Min(RowByteSize, StrideY));
      }

      //Dest += NumRows * StrideY;
    }
  }

  int TextureFrameAnimation::GetFrameTime(int Frame)
  {
    return _image.GetFrameTime(Frame);
  }

  BaseTexture* TextureFrameAnimation::Clone() const
  {
    TextureFrameAnimation* texture = new TextureFrameAnimation(*this);
    return texture;
  }

  CachedTextureFrameAnimation::CachedTextureFrameAnimation(NResourceSet *ResourceManager, TextureFrameAnimation *SourceTexture)
    :   CachedBaseTexture(ResourceManager)
  {
    if (SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr<IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateAnimatedTexture(SourceTexture->GetWidth(),
                SourceTexture->GetHeight(),
                SourceTexture->GetDepth(),
                SourceTexture->GetFormat());

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel(SourceTexture, i);
    }
  }

  CachedTextureFrameAnimation::~CachedTextureFrameAnimation()
  {

  }

  void CachedTextureFrameAnimation::UpdateTexture( BaseTexture *SourceTexture )
  {
    if ((SourceTexture == 0) || SourceTexture->IsNull())
    {
      m_Texture = ObjectPtr <IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType(TextureFrameAnimation::StaticObjectType))
    {
      nuxAssertMsg(0, "[CachedTextureFrameAnimation::UpdateTexture] Source texture is not of type TextureFrameAnimation.");
      return;
    }

    if (RecreateTexture(SourceTexture))
    {
      m_Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateAnimatedTexture(SourceTexture->GetWidth(),
                  SourceTexture->GetHeight(),
                  SourceTexture->GetDepth(),
                  SourceTexture->GetFormat());

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel(SourceTexture, i);
      }
    }
  }

  void CachedTextureFrameAnimation::LoadMipLevel(BaseTexture *SourceTexture, int MipLevel)
  {
    SURFACE_LOCKED_RECT       LockedRect;
    ObjectPtr <IOpenGLAnimatedTexture> AnimatedTexture = m_Texture; //m_Texture.CastRef<IOpenGLAnimatedTexture>();
    TextureFrameAnimation     *Source          = UpCastResource<TextureFrameAnimation, BaseTexture> (SourceTexture);

    for (int frame = 0; frame < Source->GetDepth(); frame++)
    {
      OGL_CALL( AnimatedTexture->LockRect(frame, &LockedRect, NULL));
      SourceTexture->GetData(LockedRect.pBits, 0, LockedRect.Pitch, frame);
      OGL_CALL( AnimatedTexture->UnlockRect(frame));

      AnimatedTexture->SetFrameTime(frame, Source->GetFrameTime(frame));
    }
  }

}
