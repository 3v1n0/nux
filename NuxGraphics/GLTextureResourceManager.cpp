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


#include "GLResource.h"
#include "GLResourceManager.h"
#include "GLDeviceFactory.h"
#include "OpenGLEngine.h"
#include "GLTextureResourceManager.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (BaseTexture);
  NUX_IMPLEMENT_OBJECT_TYPE (Texture2D);
  NUX_IMPLEMENT_OBJECT_TYPE (TextureRectangle);
  NUX_IMPLEMENT_OBJECT_TYPE (TextureCube);
  NUX_IMPLEMENT_OBJECT_TYPE (TextureVolume);
  NUX_IMPLEMENT_OBJECT_TYPE (TextureFrameAnimation);

  NUX_IMPLEMENT_OBJECT_TYPE (CachedBaseTexture);
  NUX_IMPLEMENT_OBJECT_TYPE (CachedTexture2D);
  NUX_IMPLEMENT_OBJECT_TYPE (CachedTextureRectangle);
  NUX_IMPLEMENT_OBJECT_TYPE (CachedTextureCube);
  NUX_IMPLEMENT_OBJECT_TYPE (CachedTextureVolume);
  NUX_IMPLEMENT_OBJECT_TYPE (CachedTextureFrameAnimation);

  /*! Up cast a Resource.
      The source must be derived from the destination type
      @param 	T       Destination type.
      @param	U       Source type.
      @return	        The casted to the destination type
  */
  template < class T, class U >
  static T *UpCastResource (U *Src)
  {
    if (!Src || !Src->Type().IsDerivedFromType (T::StaticObjectType))
      nuxError (TEXT ("[UpCastResource] Cast of %s to %s failed"), U::StaticObjectType.m_Name, T::StaticObjectType.m_Name);

    return (T *) Src;
  }
  
  BaseTexture *CreateTextureFromPixbuf (GdkPixbuf *pixbuf)
  {
    NBitmapData *BitmapData = LoadGdkPixbuf (pixbuf);
    NUX_RETURN_VALUE_IF_NULL (BitmapData, 0);

    if (BitmapData->IsTextureData() )
    {
      BaseTexture *texture = GetThreadGLDeviceFactory()->CreateSystemCapableTexture();
      texture->Update (BitmapData);
      return texture;
    }
    
    return 0;
  }


  BaseTexture *CreateTextureFromFile (const TCHAR *TextureFilename)
  {
    NBitmapData *BitmapData = LoadImageFile (TextureFilename);
    NUX_RETURN_VALUE_IF_NULL (BitmapData, 0);

    if (BitmapData->IsTextureData() )
    {
      BaseTexture *texture = GetThreadGLDeviceFactory()->CreateSystemCapableTexture();
      texture->Update (BitmapData);
      return texture;
    }
    else if (BitmapData->IsCubemapTextureData() )
    {
      TextureCube *texture = new TextureCube();
      texture->Update (BitmapData);
      return texture;
    }
    else if (BitmapData->IsVolumeTextureData() )
    {
      TextureVolume *texture = new TextureVolume();
      texture->Update (BitmapData);
      return texture;
    }
    else if (BitmapData->IsAnimatedTextureData() )
    {
      TextureFrameAnimation *texture = new TextureFrameAnimation();
      texture->Update (BitmapData);
      return texture;
    }

    nuxDebugMsg ("[CreateTextureFromFile] Invalid texture format type for file (%s)", TextureFilename);
    return 0;
  }

  BaseTexture *CreateTextureFromBitmapData (const NBitmapData *BitmapData)
  {
    if (BitmapData == 0)
      return 0;

    if (BitmapData->IsTextureData() )
    {
      BaseTexture *texture = GetThreadGLDeviceFactory()->CreateSystemCapableTexture();
      texture->Update (BitmapData);
      return texture;
    }
    else if (BitmapData->IsCubemapTextureData() )
    {
      TextureCube *texture = new TextureCube();
      texture->Update (BitmapData);
      return texture;
    }
    else if (BitmapData->IsVolumeTextureData() )
    {
      TextureVolume *texture = new TextureVolume();
      texture->Update (BitmapData);
      return texture;
    }
    else if (BitmapData->IsAnimatedTextureData() )
    {
      TextureFrameAnimation *texture = new TextureFrameAnimation();
      texture->Update (BitmapData);
      return texture;
    }

    return 0;
  }

  BaseTexture::BaseTexture(NUX_FILE_LINE_DECL)
    : ResourceData (NUX_FILE_LINE_PARAM)
  {

  }

  BaseTexture::~BaseTexture()
  {

  }

  IntrusiveSP < IOpenGLBaseTexture > BaseTexture::GetDeviceTexture()
  {
    IntrusiveSP<CachedBaseTexture> CachedTexture = GetThreadGraphicsContext()->CacheResource (this);
    return CachedTexture->m_Texture;
  }

  IntrusiveSP<CachedBaseTexture> BaseTexture::GetCachedTexture()
  {
    return GetThreadGraphicsContext()->CacheResource (this);
  }

  Texture2D::Texture2D(NUX_FILE_LINE_DECL)
  {
  }

  Texture2D::Texture2D (const Texture2D &texture, NUX_FILE_LINE_DECL)
  {
    _image = texture._image;
  }

  Texture2D::Texture2D (const NTextureData &BaseTexture, NUX_FILE_LINE_DECL)
  {
    _image = BaseTexture;
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

  bool Texture2D::Update (const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg (BitmapData, TEXT ("[Texture2D::Update] Argument BitmapData is NULL.") );
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);

    if (!BitmapData->IsTextureData() )
    {
      nuxAssertMsg (0, TEXT ("[Texture2D::Update] Argument BitmapData is not a 2D texture") );
      return false;
    }

    _image = *static_cast<const NTextureData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture (CachedTexture2D) associated with this object if any.
      GetThreadGraphicsContext()->UpdateResource (this);
    }

    return true;
  }

  bool Texture2D::Update (const TCHAR *filename, bool UpdateAndCacheResource)
  {
    NBitmapData *BitmapData = LoadImageFile (filename);
    nuxAssertMsg (BitmapData, TEXT ("[Texture2D::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);
    bool ret = Update (BitmapData, UpdateAndCacheResource);
    NUX_SAFE_DELETE (BitmapData);
    return ret;
  }

  void Texture2D::GetData (void *Buffer, int MipIndex, int StrideY, int face)
  {
    BYTE	            *Dest        = (BYTE *) Buffer;
    const BYTE         *Src         = _image.GetSurface (MipIndex).GetPtrRawData();
    int                 RowByteSize = _image.GetSurface (MipIndex).GetPitch();
    int                 NumRows     = _image.GetSurface (MipIndex).GetBlockHeight();

    for ( int Y = 0; Y < NumRows; Y++ )
    {
      // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
      // they contain the same amount of valid data since they have the same width, height and format.
      Memcpy (Dest + Y * StrideY, &Src[Y * RowByteSize], Min (RowByteSize, StrideY) );
    }
  }

  BaseTexture* Texture2D::Clone () const
  {
    Texture2D* texture = new Texture2D(*this);
    return texture;
  }

  CachedBaseTexture::CachedBaseTexture (NResourceSet *ResourceManager)
    :   CachedResourceData (ResourceManager),
    SourceWidth (0),
    SourceHeight (0),
    SourceDepth (0),
    SourceFormat (BITFMT_UNKNOWN)
  {

  }

  CachedBaseTexture::~CachedBaseTexture()
  {
    m_Texture.Release ();
  }

  bool CachedBaseTexture::UpdateResource (ResourceData *Resource)
  {
    UpdateTexture ( (BaseTexture *) Resource);
    return TRUE;
  }

  bool CachedBaseTexture::RecreateTexture (BaseTexture *Source)
  {
    int CurrentWidth   = m_Texture->GetWidth();
    int CurrentHeight  = m_Texture->GetHeight();
    int CurrentDepth   = m_Texture->GetDepth();
    int CurrentNumMipmap      = m_Texture->GetNumMipLevel();
    BitmapFormat CurrentFormat = m_Texture->GetPixelFormat();

    bool Recreate =
      (CurrentWidth       != Source->GetWidth() ) ||
      (CurrentHeight      != Source->GetHeight() ) ||
      (CurrentDepth       != Source->GetDepth() ) ||
      (CurrentNumMipmap   != Source->GetNumMipLevel() ) ||
      (CurrentFormat      != Source->GetFormat() );

    return Recreate;
  }

  CachedTexture2D::CachedTexture2D (NResourceSet *ResourceManager, Texture2D *SourceTexture)
    : CachedBaseTexture (ResourceManager)
  {
    if (SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP <IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetThreadGLDeviceFactory()->CreateTexture (SourceTexture->GetWidth(),
                SourceTexture->GetHeight(),
                SourceTexture->GetNumMipLevel(),
                SourceTexture->GetFormat() );

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel (SourceTexture, i);
    }
  }

  CachedTexture2D::~CachedTexture2D()
  {

  }

  void CachedTexture2D::UpdateTexture ( BaseTexture *SourceTexture )
  {
    if ( (SourceTexture == 0) || SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP <IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType (Texture2D::StaticObjectType) )
    {
      nuxAssertMsg (0, TEXT ("[Texture2D::UpdateTexture] Source texture is not of type Texture2D.") );
      return;
    }

    if ( RecreateTexture (SourceTexture) )
    {
      m_Texture = GetThreadGLDeviceFactory()->CreateTexture (SourceTexture->GetWidth(),
                  SourceTexture->GetHeight(),
                  SourceTexture->GetNumMipLevel(),
                  SourceTexture->GetFormat() );

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
  }

  void CachedTexture2D::LoadMipLevel (BaseTexture *SourceTexture, int MipLevel)
  {
    SURFACE_LOCKED_RECT LockedRect;
    IntrusiveSP < IOpenGLTexture2D > Texture2D = m_Texture; //m_Texture.CastRef<IOpenGLTexture2D>();

    OGL_CALL ( Texture2D->LockRect ( MipLevel, &LockedRect, NULL) );
    SourceTexture->GetData ( LockedRect.pBits, MipLevel, LockedRect.Pitch );
    OGL_CALL ( Texture2D->UnlockRect ( MipLevel ) );
  }

//////////////////////////////////////////////////////////////////////

  TextureRectangle::TextureRectangle (NUX_FILE_LINE_DECL)
    : BaseTexture (NUX_FILE_LINE_PARAM)
  {
  }

  TextureRectangle::TextureRectangle (const TextureRectangle &texture)
  {
    _image = texture._image;
  }

  TextureRectangle::TextureRectangle (const NTextureData &BaseTexture)
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

  bool TextureRectangle::Update (const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg (BitmapData, TEXT ("[TextureRectangle::Update] Argument BitmapData is NULL.") );
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);

    if (!BitmapData->IsTextureData() )
    {
      nuxAssertMsg (0, TEXT ("[TextureRectangle::Update] Argument BitmapData is not a 2D texture") );
      return false;
    }

    _image = *static_cast<const NTextureData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture (CachedTexture2D) associated with this object if any.
      GetThreadGraphicsContext()->UpdateResource (this);
    }
    return true;
  }

  bool TextureRectangle::Update (const TCHAR *filename, bool UpdateAndCacheResource)
  {
    bool b = false;
    NBitmapData *BitmapData = LoadImageFile (filename);
    nuxAssertMsg (BitmapData, TEXT ("[TextureRectangle::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);
    b = Update (BitmapData);
    NUX_SAFE_DELETE (BitmapData);
    return b;
  }

  void TextureRectangle::GetData (void *Buffer, int MipIndex, int StrideY, int face)
  {
    BYTE	            *Dest        = (BYTE *) Buffer;
    const BYTE         *Src         = _image.GetSurface (MipIndex).GetPtrRawData();
    int                 RowByteSize = _image.GetSurface (MipIndex).GetPitch();
    int                 NumRows     = _image.GetSurface (MipIndex).GetBlockHeight();

    for ( int Y = 0; Y < NumRows; Y++ )
    {
      // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
      // they contain the same amount of valid data since they have the same width, height and format.
      Memcpy (Dest + Y * StrideY, &Src[Y * RowByteSize], Min (RowByteSize, StrideY) );
    }
  }

  BaseTexture* TextureRectangle::Clone () const
  {
    TextureRectangle* texture = new TextureRectangle(*this);
    return texture;
  }

  CachedTextureRectangle::CachedTextureRectangle (NResourceSet *ResourceManager, TextureRectangle *SourceTexture)
    : CachedBaseTexture (ResourceManager)
  {
    if (SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP<IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetThreadGLDeviceFactory()->CreateRectangleTexture (SourceTexture->GetWidth(),
                SourceTexture->GetHeight(),
                SourceTexture->GetNumMipLevel(),
                SourceTexture->GetFormat() );

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel (SourceTexture, i);
    }
  }

  CachedTextureRectangle::~CachedTextureRectangle()
  {

  }

  void CachedTextureRectangle::UpdateTexture ( BaseTexture *SourceTexture )
  {
    if ( (SourceTexture == 0) || SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP<IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType (TextureRectangle::StaticObjectType) )
    {
      nuxAssertMsg (0, TEXT ("[CachedTextureRectangle::UpdateTexture] Source texture is not of type TextureRectangle.") );
      return;
    }

    if ( RecreateTexture (SourceTexture) )
    {
      m_Texture = GetThreadGLDeviceFactory()->CreateRectangleTexture (SourceTexture->GetWidth(),
                  SourceTexture->GetHeight(),
                  SourceTexture->GetNumMipLevel(),
                  SourceTexture->GetFormat() );

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
  }

  void CachedTextureRectangle::LoadMipLevel (BaseTexture *SourceTexture, int MipLevel)
  {
    SURFACE_LOCKED_RECT		        LockedRect;
    IntrusiveSP <IOpenGLRectangleTexture>	TextureRectangle = m_Texture; //m_Texture.CastRef<IOpenGLRectangleTexture>();

    OGL_CALL (TextureRectangle->LockRect ( MipLevel, &LockedRect, NULL) );
    SourceTexture->GetData ( LockedRect.pBits, MipLevel, LockedRect.Pitch );
    OGL_CALL (TextureRectangle->UnlockRect ( MipLevel ) );
  }

//////////////////////////////////////////////////////////////////////////

  TextureCube::TextureCube(NUX_FILE_LINE_DECL)
    : BaseTexture (NUX_FILE_LINE_PARAM)
  {
  }

  TextureCube::TextureCube (const TextureCube &texture)
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

  bool TextureCube::Update (const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg (BitmapData, TEXT ("[TextureCube::Update] Argument BitmapData is NULL.") );
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);

    if (!BitmapData->IsCubemapTextureData() )
    {
      nuxAssertMsg (0, TEXT ("[TextureCube::Update] Argument BitmapData is not a Cube texture") );
      return false;
    }

    _image = *static_cast<const NCubemapData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture (CachedTexture2D) associated with this object if any.
      GetThreadGraphicsContext()->UpdateResource (this);
    }

    return true;
  }

  bool TextureCube::Update (const TCHAR *filename, bool UpdateAndCacheResource)
  {
    NBitmapData *BitmapData = LoadImageFile (filename);
    nuxAssertMsg (BitmapData, TEXT ("[TextureCube::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);
    bool ret = Update (BitmapData);
    NUX_SAFE_DELETE (BitmapData);
    return ret;
  }

  void TextureCube::GetData (void *Buffer, int MipIndex, int StrideY, int face)
  {
    BYTE	            *Dest        = (BYTE *) Buffer;
    const BYTE         *Src         = _image.GetSurface (face, MipIndex).GetPtrRawData();
    int                 RowByteSize = _image.GetSurface (face, MipIndex).GetPitch();
    int                 NumRows     = _image.GetSurface (face, MipIndex).GetBlockHeight();

    for ( int Y = 0; Y < NumRows; Y++ )
    {
      // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
      // they contain the same amount of valid data since they have the same width, height and format.
      Memcpy (Dest + Y * StrideY, &Src[Y * RowByteSize], Min (RowByteSize, StrideY) );
    }
  }

  BaseTexture* TextureCube::Clone () const
  {
    TextureCube* texture = new TextureCube(*this);
    return texture;
  }

  CachedTextureCube::CachedTextureCube (NResourceSet *ResourceManager, TextureCube *SourceTexture)
    : CachedBaseTexture (ResourceManager)
  {
    if (SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP<IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetThreadGLDeviceFactory()->CreateCubeTexture (SourceTexture->GetWidth(),
                SourceTexture->GetNumMipLevel(),
                SourceTexture->GetFormat() );

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel (SourceTexture, i);
    }
  }

  CachedTextureCube::~CachedTextureCube()
  {

  }

  void CachedTextureCube::UpdateTexture ( BaseTexture *SourceTexture )
  {
    if ( (SourceTexture == 0) || SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP<IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType (TextureCube::StaticObjectType) )
    {
      nuxAssertMsg (0, TEXT ("[CachedTextureCube::UpdateTexture] Source texture is not of type TextureCube.") );
      return;
    }

    if ( RecreateTexture (SourceTexture) )
    {
      m_Texture = GetThreadGLDeviceFactory()->CreateCubeTexture (SourceTexture->GetWidth(),
                  SourceTexture->GetNumMipLevel(),
                  SourceTexture->GetFormat() );

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
  }

  void CachedTextureCube::LoadMipLevel (BaseTexture *SourceTexture, int MipLevel)
  {
    SURFACE_LOCKED_RECT		LockedRect;
    IntrusiveSP <IOpenGLCubeTexture> CubemapTexture = m_Texture; //m_Texture.CastRef<IOpenGLCubeTexture>();

    for (int face = CUBEMAP_FACE_POSITIVE_X; face < GL_TEXTURE_CUBE_MAP_NEGATIVE_Z + 1; face++)
    {
      OGL_CALL ( CubemapTexture->LockRect (eCUBEMAP_FACES (face), MipLevel, &LockedRect, NULL) );
      SourceTexture->GetData (LockedRect.pBits, MipLevel, LockedRect.Pitch, face - CUBEMAP_FACE_POSITIVE_X);
      OGL_CALL ( CubemapTexture->UnlockRect (eCUBEMAP_FACES (face), MipLevel ) );
    }
  }

//////////////////////////////////////////////////////////////////////////

  TextureVolume::TextureVolume(NUX_FILE_LINE_DECL)
    : BaseTexture (NUX_FILE_LINE_PARAM)
  {
  }

  TextureVolume::TextureVolume (const TextureVolume &texture)
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

  bool TextureVolume::Update (const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg (BitmapData, TEXT ("[TextureVolume::Update] Argument BitmapData is NULL.") );
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);

    if (!BitmapData->IsVolumeTextureData() )
    {
      nuxAssertMsg (0, TEXT ("[TextureVolume::Update] Argument BitmapData is not a Volume texture") );
      return false;
    }

    _image = *static_cast<const NVolumeData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture (CachedTexture2D) associated with this object if any.
      GetThreadGraphicsContext()->UpdateResource (this);
    }

    return true;
  }

  bool TextureVolume::Update (const TCHAR *filename, bool UpdateAndCacheResource)
  {
    NBitmapData *BitmapData = LoadImageFile (filename);
    nuxAssertMsg (BitmapData, TEXT ("[TextureVolume::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);
    bool ret = Update (filename);
    NUX_SAFE_DELETE (BitmapData);
    return ret;
  }

  void TextureVolume::GetData (void *Buffer, int MipIndex, int StrideY, int slice)
  {
    BYTE               *Dest        = (BYTE *) Buffer;
//     const BYTE*         Src         = _image.GetSurface(MipIndex, slice).GetPtrRawData();
//     int                 RowByteSize = _image.GetSurface(MipIndex, slice).GetPitch();
//     int                 NumRows     = _image.GetSurface(MipIndex, slice).GetBlockHeight();

    for (t_s32 slice = 0; slice < ImageSurface::GetLevelDim (_image.GetFormat(), _image.GetDepth(), MipIndex); slice++)
    {
      const BYTE         *Src         = _image.GetSurface (MipIndex, slice).GetPtrRawData();
      int                 RowByteSize = _image.GetSurface (MipIndex, slice).GetPitch();
      int                 NumRows     = _image.GetSurface (MipIndex, slice).GetBlockHeight();

      for ( int Y = 0; Y < NumRows; Y++ )
      {
        // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
        // they contain the same amount of valid data since they have the same width, height and format.
        Memcpy (Dest + Y * StrideY, &Src[Y * RowByteSize], Min (RowByteSize, StrideY) );
      }

      Dest += NumRows * StrideY;
    }

//     BYTE*	            Dest        = (BYTE*)Buffer;
//     const BYTE*         Src         = _image.GetSurface(MipIndex, slice).GetPtrRawData();
//     int                 RowByteSize = _image.GetSurface(MipIndex, slice).GetPitch();
//     int                 NumRows     = _image.GetSurface(MipIndex, slice).GetBlockHeight();
//
//     for( int Y = 0; Y < NumRows; Y++ )
//     {
//         // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
//         // they contain the same amount of valid data since they have the same width, height and format.
//         Memcpy(Dest + Y * StrideY,&Src[Y * RowByteSize], Min(RowByteSize, StrideY));
//     }
  }

  BaseTexture* TextureVolume::Clone () const
  {
    TextureVolume* texture = new TextureVolume(*this);
    return texture;
  }

  CachedTextureVolume::CachedTextureVolume (NResourceSet *ResourceManager, TextureVolume *SourceTexture)
    :   CachedBaseTexture (ResourceManager)
  {
    if (SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP<IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetThreadGLDeviceFactory()->CreateVolumeTexture (SourceTexture->GetWidth(),
                SourceTexture->GetHeight(),
                SourceTexture->GetDepth(),
                SourceTexture->GetNumMipLevel(),
                SourceTexture->GetFormat() );

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel (SourceTexture, i);
    }
  }

  CachedTextureVolume::~CachedTextureVolume()
  {

  }

  void CachedTextureVolume::UpdateTexture ( BaseTexture *SourceTexture )
  {
    if ( (SourceTexture == 0) || SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP<IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType (TextureVolume::StaticObjectType) )
    {
      nuxAssertMsg (0, TEXT ("[CachedTextureVolume::UpdateTexture] Source texture is not of type TextureVolume.") );
      return;
    }

    if ( RecreateTexture (SourceTexture) )
    {
      m_Texture = GetThreadGLDeviceFactory()->CreateVolumeTexture (SourceTexture->GetWidth(),
                  SourceTexture->GetHeight(),
                  SourceTexture->GetDepth(),
                  SourceTexture->GetNumMipLevel(),
                  SourceTexture->GetFormat() );

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
  }

  void CachedTextureVolume::LoadMipLevel (BaseTexture *SourceTexture, int MipLevel)
  {
    VOLUME_LOCKED_BOX       LockedBox;
    IntrusiveSP <IOpenGLVolumeTexture> VolumeTexture = m_Texture; //m_Texture.CastRef<IOpenGLVolumeTexture>();
    //TextureVolume*     Source          = UpCastResource<TextureVolume, BaseTexture>(SourceTexture);

    //for(int slice = 0; slice < ImageSurface::GetLevelDim(Source->GetFormat(), Source->GetDepth(), MipLevel); slice++)
    {
      OGL_CALL ( VolumeTexture->LockBox (MipLevel, &LockedBox, NULL) );
      SourceTexture->GetData (LockedBox.pBits, MipLevel, LockedBox.RowPitch, 0);
      OGL_CALL ( VolumeTexture->UnlockBox (MipLevel) );
    }

//     for(int slice = 0; slice < depth; slice++)
//     {
//         OGL_CALL( VolumeTexture->LockRect(slice, MipLevel, &LockedRect, NULL) );
//         SourceTexture->GetData(LockedRect.pBits, MipLevel, LockedRect.Pitch, slice);
//         OGL_CALL( VolumeTexture->UnlockRect(slice, MipLevel ) );
//     }
  }

//////////////////////////////////////////////////////////////////////////
  TextureFrameAnimation::TextureFrameAnimation(NUX_FILE_LINE_DECL)
    : BaseTexture (NUX_FILE_LINE_PARAM)
  {
  }

  TextureFrameAnimation::TextureFrameAnimation (const TextureFrameAnimation &texture)
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

  bool TextureFrameAnimation::Update (const NBitmapData *BitmapData, bool UpdateAndCacheResource)
  {
    nuxAssertMsg (BitmapData, TEXT ("[TextureFrameAnimation::Update] Argument BitmapData is NULL.") );
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);

    if (!BitmapData->IsAnimatedTextureData() )
    {
      nuxAssertMsg (0, TEXT ("[TextureFrameAnimation::Update] Argument BitmapData is not a Animated texture") );
      return false;
    }

    _image = *static_cast<const NAnimatedTextureData *> (BitmapData);

    if (UpdateAndCacheResource)
    {
      // call the texture manager and recreate the texture (CachedTexture2D) associated with this object if any.
      GetThreadGraphicsContext()->UpdateResource (this);
    }

    return true;
  }

  bool TextureFrameAnimation::Update (const TCHAR *filename, bool UpdateAndCacheResource)
  {
    NBitmapData *BitmapData = LoadImageFile (filename);
    nuxAssertMsg (BitmapData, TEXT ("[TextureFrameAnimation::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL (BitmapData, false);
    bool ret = Update (BitmapData);
    NUX_SAFE_DELETE (BitmapData);
    return ret;
  }

  void TextureFrameAnimation::GetData (void *Buffer, int MipIndex, int StrideY, int slice)
  {
    BYTE               *Dest        = (BYTE *) Buffer;
    //for(int slice = 0; slice < ImageSurface::GetLevelDim(_image.GetFormat(), _image.GetDepth(), MipIndex); slice++)
    {
      const BYTE         *Src         = _image.GetSurface (slice).GetPtrRawData();
      int                 RowByteSize = _image.GetSurface (slice).GetPitch();
      int                 NumRows     = _image.GetSurface (slice).GetBlockHeight();

      for (int Y = 0; Y < NumRows; Y++)
      {
        // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
        // they contain the same amount of valid data since they have the same width, height and format.
        Memcpy (Dest + Y * StrideY, &Src[Y * RowByteSize], Min (RowByteSize, StrideY) );
      }

      //Dest += NumRows * StrideY;
    }
  }

  int TextureFrameAnimation::GetFrameTime (int Frame)
  {
    return _image.GetFrameTime (Frame);
  }

  BaseTexture* TextureFrameAnimation::Clone () const
  {
    TextureFrameAnimation* texture = new TextureFrameAnimation(*this);
    return texture;
  }

  CachedTextureFrameAnimation::CachedTextureFrameAnimation (NResourceSet *ResourceManager, TextureFrameAnimation *SourceTexture)
    :   CachedBaseTexture (ResourceManager)
  {
    if (SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP<IOpenGLBaseTexture> (0);
      return;
    }

    m_Texture = GetThreadGLDeviceFactory()->CreateAnimatedTexture (SourceTexture->GetWidth(),
                SourceTexture->GetHeight(),
                SourceTexture->GetDepth(),
                SourceTexture->GetFormat() );

    for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
      LoadMipLevel (SourceTexture, i);
    }
  }

  CachedTextureFrameAnimation::~CachedTextureFrameAnimation()
  {

  }

  void CachedTextureFrameAnimation::UpdateTexture ( BaseTexture *SourceTexture )
  {
    if ( (SourceTexture == 0) || SourceTexture->IsNull() )
    {
      m_Texture = IntrusiveSP <IOpenGLBaseTexture> (0);
      return;
    }

    if (!SourceTexture->Type().IsObjectType (TextureFrameAnimation::StaticObjectType) )
    {
      nuxAssertMsg (0, TEXT ("[CachedTextureFrameAnimation::UpdateTexture] Source texture is not of type TextureFrameAnimation.") );
      return;
    }

    if (RecreateTexture (SourceTexture) )
    {
      m_Texture = GetThreadGLDeviceFactory()->CreateAnimatedTexture (SourceTexture->GetWidth(),
                  SourceTexture->GetHeight(),
                  SourceTexture->GetDepth(),
                  SourceTexture->GetFormat() );

      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
    else
    {
      for (int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
      {
        LoadMipLevel (SourceTexture, i);
      }
    }
  }

  void CachedTextureFrameAnimation::LoadMipLevel (BaseTexture *SourceTexture, int MipLevel)
  {
    SURFACE_LOCKED_RECT       LockedRect;
    IntrusiveSP <IOpenGLAnimatedTexture> AnimatedTexture = m_Texture; //m_Texture.CastRef<IOpenGLAnimatedTexture>();
    TextureFrameAnimation     *Source          = UpCastResource<TextureFrameAnimation, BaseTexture> (SourceTexture);

    for (int frame = 0; frame < Source->GetDepth(); frame++)
    {
      OGL_CALL ( AnimatedTexture->LockRect (frame, &LockedRect, NULL) );
      SourceTexture->GetData (LockedRect.pBits, 0, LockedRect.Pitch, frame);
      OGL_CALL ( AnimatedTexture->UnlockRect (frame) );

      AnimatedTexture->SetFrameTime (frame, Source->GetFrameTime (frame) );
    }
  }

}
