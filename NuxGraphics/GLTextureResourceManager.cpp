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

namespace nux { //NUX_NAMESPACE_BEGIN

NUX_IMPLEMENT_OBJECT_TYPE(NTexture);
NUX_IMPLEMENT_OBJECT_TYPE(NTexture2D);
NUX_IMPLEMENT_OBJECT_TYPE(NRectangleTexture);
NUX_IMPLEMENT_OBJECT_TYPE(NTextureCube);
NUX_IMPLEMENT_OBJECT_TYPE(NTextureVolume);
NUX_IMPLEMENT_OBJECT_TYPE(NAnimatedTexture);

NUX_IMPLEMENT_OBJECT_TYPE(NGLTexture);
NUX_IMPLEMENT_OBJECT_TYPE(NGLTexture2D);
NUX_IMPLEMENT_OBJECT_TYPE(NGLRectangleTexture);
NUX_IMPLEMENT_OBJECT_TYPE(NGLTextureCube);
NUX_IMPLEMENT_OBJECT_TYPE(NGLTextureVolume);
NUX_IMPLEMENT_OBJECT_TYPE(NGLAnimatedTexture);

/*! Up cast a Resource.
    The source must be derived from the destination type
    @param 	T       Destination type.
    @param	U       Source type.
    @return	        The casted to the destination type
*/
template< class T, class U > 
static T* UpCastResource( U* Src )
{
    if( !Src || !Src->Type().IsDerivedFromType(T::StaticObjectType) )
        nuxError( TEXT("[UpCastResource] Cast of %s to %s failed"), Src ? (const TCHAR*)Src->GetResourceName()() : TEXT("NULL"), T::StaticObjectType.m_Name );
    return (T*)Src;
}


NTexture* CreateTextureFromFile(const TCHAR* TextureFilename)
{
    NBitmapData* BitmapData = LoadImageFile(TextureFilename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, 0);

    if(BitmapData->isTextureData())
    {
        NTexture2D* texture = new NTexture2D();
        texture->Update(BitmapData);
        return texture;
    }
    else if(BitmapData->isCubemapTextureData())
    {
        NTextureCube* texture = new NTextureCube();
        texture->Update(BitmapData);
        return texture;
    }
    else if(BitmapData->isVolumeTextureData())
    {
        NTextureVolume* texture = new NTextureVolume();
        texture->Update(BitmapData);
        return texture;
    }
    else if(BitmapData->isAnimatedTextureData())
    {
        NAnimatedTexture* texture = new NAnimatedTexture();
        texture->Update(BitmapData);
        return texture;
    }
    
    nuxDebugMsg("[CreateTextureFromFile] Invalid texture format type for file (%s)", TextureFilename);
    return 0;
}

NTexture* CreateTextureFromBitmapData(const NBitmapData* BitmapData)
{
    if(BitmapData == 0)
        return 0;

    if(BitmapData->isTextureData())
    {
        NTexture2D* texture = new NTexture2D();
        texture->Update(BitmapData);
        return texture;
    }
    else if(BitmapData->isCubemapTextureData())
    {
        NTextureCube* texture = new NTextureCube();
        texture->Update(BitmapData);
        return texture;
    }
    else if(BitmapData->isVolumeTextureData())
    {
        NTextureVolume* texture = new NTextureVolume();
        texture->Update(BitmapData);
        return texture;
    }
    else if(BitmapData->isAnimatedTextureData())
    {
        NAnimatedTexture* texture = new NAnimatedTexture();
        texture->Update(BitmapData);
        return texture;
    }
    return 0;
}

NTexture::NTexture()
{

}

NTexture::~NTexture()
{

}

TRefGL<IOpenGLBaseTexture> NTexture::GetDeviceTexture()
{
    TRefGL<NGLTexture> CachedTexture = GetThreadGraphicsContext()->CacheResource(this);
    return CachedTexture->m_Texture;
}

TRefGL<NGLTexture> NTexture::GetCachedTexture()
{
    return GetThreadGraphicsContext()->CacheResource(this);
}

NTexture2D::NTexture2D()
{
}

NTexture2D::NTexture2D(const NTexture2D& texture)
{
    m_Image = texture.m_Image;
}

NTexture2D::NTexture2D(const NTextureData& TextureData)
{
    m_Image = TextureData;
}

NTexture2D& NTexture2D::operator = (const NTexture2D& texture)
{
    if (this == &texture)
        return *this;   // Handle self assignment
    m_Image = texture.m_Image;
    return *this;
}

NTexture2D::~NTexture2D()
{

}

bool NTexture2D::Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource)
{
    nuxAssertMsg(BitmapData, TEXT("[NTexture2D::Update] Argument BitmapData is NULL."));
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    if(!BitmapData->isTextureData())
    {
        nuxAssertMsg(0, TEXT("[NTexture2D::Update] Argument BitmapData is not a 2D texture"));
        return false;
    }
    m_Image = *static_cast<const NTextureData*>(BitmapData);

    if(UpdateAndCacheResource)
    {
        // call the texture manager and recreate the texture (NGLTexture2D) associated with this object if any.
        GetThreadGraphicsContext()->UpdateResource(this);
    }
    return true;
}

bool NTexture2D::Update(const TCHAR* filename, bool UpdateAndCacheResource)
{
    NBitmapData* BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, TEXT("[NTexture2D::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    bool ret = Update(BitmapData, UpdateAndCacheResource);
    NUX_SAFE_DELETE(BitmapData);
    return ret;
}

void NTexture2D::GetData(void* Buffer, int MipIndex, int StrideY, int face)
{
    BYTE*	            Dest        = (BYTE*)Buffer;
    const BYTE*         Src         = m_Image.GetSurface(MipIndex).GetPtrRawData();
    int                 RowByteSize = m_Image.GetSurface(MipIndex).GetPitch();
    int                 NumRows     = m_Image.GetSurface(MipIndex).GetBlockHeight();

    for( int Y = 0; Y < NumRows; Y++ )
    {
        // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but 
        // they contain the same amount of valid data since they have the same width, height and format.
        Memcpy(Dest + Y * StrideY,&Src[Y * RowByteSize], Min(RowByteSize, StrideY));
    }
}

bool NGLTexture::RecreateTexture(NTexture* Source)
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

NGLTexture2D::NGLTexture2D(NResourceSet* ResourceManager, NTexture2D* SourceTexture)
: NGLTexture(ResourceManager)
{
    if(SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }

    m_Texture = GetThreadGLDeviceFactory()->CreateTexture(SourceTexture->GetWidth(),
        SourceTexture->GetHeight(), 
        SourceTexture->GetNumMipLevel(),
        SourceTexture->GetFormat());

    for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
        LoadMipLevel(SourceTexture, i);
    }
}

NGLTexture2D::~NGLTexture2D()
{

}

void NGLTexture2D::UpdateTexture( NTexture* SourceTexture )
{
    if((SourceTexture == 0) || SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }

    if(!SourceTexture->Type().IsObjectType(NTexture2D::StaticObjectType))
    {
        nuxAssertMsg(0, TEXT("[NTexture2D::UpdateTexture] Source texture is not of type NTexture2D."));
        return;
    }

    if( RecreateTexture(SourceTexture) )
    {
        m_Texture = NULL;

        m_Texture = GetThreadGLDeviceFactory()->CreateTexture(SourceTexture->GetWidth(),
            SourceTexture->GetHeight(), 
            SourceTexture->GetNumMipLevel(),
            SourceTexture->GetFormat());

        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
    else
    {
        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
}

void NGLTexture2D::LoadMipLevel(NTexture* SourceTexture, int MipLevel)
{
    SURFACE_LOCKED_RECT LockedRect;
    TRefGL<IOpenGLTexture2D> Texture2D = m_Texture.CastRef<IOpenGLTexture2D>();

    OGL_CALL( Texture2D->LockRect( MipLevel, &LockedRect, NULL) );
    SourceTexture->GetData( LockedRect.pBits, MipLevel, LockedRect.Pitch );
    OGL_CALL( Texture2D->UnlockRect( MipLevel ) );
}

//////////////////////////////////////////////////////////////////////

NRectangleTexture::NRectangleTexture()
{
}

NRectangleTexture::NRectangleTexture(const NRectangleTexture& texture)
{
    m_Image = texture.m_Image;
}

NRectangleTexture& NRectangleTexture::operator = (const NRectangleTexture& texture)
{
    if (this == &texture)
        return *this;   // Handle self assignment
    m_Image = texture.m_Image;
    return *this;
}

NRectangleTexture::~NRectangleTexture()
{

}

bool NRectangleTexture::Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource)
{
    nuxAssertMsg(BitmapData, TEXT("[NRectangleTexture::Update] Argument BitmapData is NULL."));
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    if(!BitmapData->isTextureData())
    {
        nuxAssertMsg(0, TEXT("[NRectangleTexture::Update] Argument BitmapData is not a 2D texture"));
        return false;
    }
    m_Image = *static_cast<const NTextureData*>(BitmapData);
    // call the texture manager and recreate the texture (NGLRectangleTexture) associated with this object if any.
    GetThreadGraphicsContext()->UpdateResource(this);
    return true;
}

bool NRectangleTexture::Update(const TCHAR* filename, bool UpdateAndCacheResource)
{
    bool b = false;
    NBitmapData* BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, TEXT("[NRectangleTexture::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    b = Update(BitmapData);
    NUX_SAFE_DELETE(BitmapData);
    return b;
}

void NRectangleTexture::GetData(void* Buffer, int MipIndex, int StrideY, int face)
{
    BYTE*	            Dest        = (BYTE*)Buffer;
    const BYTE*         Src         = m_Image.GetSurface(MipIndex).GetPtrRawData();
    int                 RowByteSize = m_Image.GetSurface(MipIndex).GetPitch();
    int                 NumRows     = m_Image.GetSurface(MipIndex).GetBlockHeight();

    for( int Y = 0; Y < NumRows; Y++ )
    {
        // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but 
        // they contain the same amount of valid data since they have the same width, height and format.
        Memcpy(Dest + Y * StrideY,&Src[Y * RowByteSize], Min(RowByteSize, StrideY));
    }
}

NGLRectangleTexture::NGLRectangleTexture(NResourceSet* ResourceManager, NRectangleTexture* SourceTexture)
: NGLTexture(ResourceManager)
{
    if(SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }
    m_Texture = GetThreadGLDeviceFactory()->CreateRectangleTexture(SourceTexture->GetWidth(),
        SourceTexture->GetHeight(), 
        SourceTexture->GetNumMipLevel(),
        SourceTexture->GetFormat());

    for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
        LoadMipLevel(SourceTexture, i);
    }
}

NGLRectangleTexture::~NGLRectangleTexture()
{

}

void NGLRectangleTexture::UpdateTexture( NTexture* SourceTexture )
{
    if((SourceTexture == 0) || SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }

    if(!SourceTexture->Type().IsObjectType(NRectangleTexture::StaticObjectType))
    {
        nuxAssertMsg(0, TEXT("[NGLRectangleTexture::UpdateTexture] Source texture is not of type NRectangleTexture."));
        return;
    }

    if( RecreateTexture(SourceTexture) )
    {
        m_Texture = NULL;

        m_Texture = GetThreadGLDeviceFactory()->CreateRectangleTexture(SourceTexture->GetWidth(),
            SourceTexture->GetHeight(), 
            SourceTexture->GetNumMipLevel(),
            SourceTexture->GetFormat());

        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
    else
    {
        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
}

void NGLRectangleTexture::LoadMipLevel(NTexture* SourceTexture, int MipLevel)
{
    SURFACE_LOCKED_RECT		        LockedRect;
    TRefGL<IOpenGLRectangleTexture>	TextureRectangle     = m_Texture.CastRef<IOpenGLRectangleTexture>();

    OGL_CALL(TextureRectangle->LockRect( MipLevel, &LockedRect, NULL));
    SourceTexture->GetData( LockedRect.pBits, MipLevel, LockedRect.Pitch );
    OGL_CALL(TextureRectangle->UnlockRect( MipLevel ));
}

//////////////////////////////////////////////////////////////////////////

NTextureCube::NTextureCube()
{
}

NTextureCube::NTextureCube(const NTextureCube& texture)
{
    m_Image = texture.m_Image;
}

NTextureCube& NTextureCube::operator = (const NTextureCube& texture)
{
    if (this == &texture)
        return *this;   // Handle self assignment
    m_Image = texture.m_Image;
    return *this;
}

NTextureCube::~NTextureCube()
{

}

bool NTextureCube::Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource)
{
    nuxAssertMsg(BitmapData, TEXT("[NTextureCube::Update] Argument BitmapData is NULL."));
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    if(!BitmapData->isCubemapTextureData())
    {
        nuxAssertMsg(0, TEXT("[NTextureCube::Update] Argument BitmapData is not a Cube texture"));
        return false;
    }
    m_Image = *static_cast<const NCubemapData*>(BitmapData);
    // call the texture manager and recreate the texture (NGLTexture2D) associated with this object if any.
    GetThreadGraphicsContext()->UpdateResource(this);
    return true;
}

bool NTextureCube::Update(const TCHAR* filename, bool UpdateAndCacheResource)
{
    NBitmapData* BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, TEXT("[NTextureCube::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    bool ret = Update(BitmapData);
    NUX_SAFE_DELETE(BitmapData);
    return ret;
}

void NTextureCube::GetData(void* Buffer, int MipIndex, int StrideY, int face)
{
    BYTE*	            Dest        = (BYTE*)Buffer;
    const BYTE*         Src         = m_Image.GetSurface(face, MipIndex).GetPtrRawData();
    int                 RowByteSize = m_Image.GetSurface(face, MipIndex).GetPitch();
    int                 NumRows     = m_Image.GetSurface(face, MipIndex).GetBlockHeight();

    for( int Y = 0; Y < NumRows; Y++ )
    {
        // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but 
        // they contain the same amount of valid data since they have the same width, height and format.
        Memcpy(Dest + Y * StrideY,&Src[Y * RowByteSize], Min(RowByteSize, StrideY));
    }
}

NGLTextureCube::NGLTextureCube(NResourceSet* ResourceManager, NTextureCube* SourceTexture)
: NGLTexture(ResourceManager)
{
    if(SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }
    m_Texture = GetThreadGLDeviceFactory()->CreateCubeTexture(SourceTexture->GetWidth(),
        SourceTexture->GetNumMipLevel(),
        SourceTexture->GetFormat());

    for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
        LoadMipLevel(SourceTexture, i);
    }
}

NGLTextureCube::~NGLTextureCube()
{

}

void NGLTextureCube::UpdateTexture( NTexture* SourceTexture )
{
    if((SourceTexture == 0) || SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }
    if(!SourceTexture->Type().IsObjectType(NTextureCube::StaticObjectType))
    {
        nuxAssertMsg(0, TEXT("[NGLTextureCube::UpdateTexture] Source texture is not of type NTextureCube."));
        return;
    }

    if( RecreateTexture(SourceTexture) )
    {
        m_Texture = NULL;

        m_Texture = GetThreadGLDeviceFactory()->CreateCubeTexture(SourceTexture->GetWidth(),
            SourceTexture->GetNumMipLevel(),
            SourceTexture->GetFormat());
        
        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
    else
    {
        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
}

void NGLTextureCube::LoadMipLevel(NTexture* SourceTexture, int MipLevel)
{
    SURFACE_LOCKED_RECT		LockedRect;
    TRefGL<IOpenGLCubeTexture> CubemapTexture = m_Texture.CastRef<IOpenGLCubeTexture>();

    for(int face = CUBEMAP_FACE_POSITIVE_X; face < GL_TEXTURE_CUBE_MAP_NEGATIVE_Z + 1; face++)
    {
        OGL_CALL( CubemapTexture->LockRect(eCUBEMAP_FACES(face), MipLevel, &LockedRect, NULL) );
        SourceTexture->GetData(LockedRect.pBits, MipLevel, LockedRect.Pitch, face - CUBEMAP_FACE_POSITIVE_X);
        OGL_CALL( CubemapTexture->UnlockRect(eCUBEMAP_FACES(face), MipLevel ) );
    }
}

//////////////////////////////////////////////////////////////////////////

NTextureVolume::NTextureVolume()
{
}

NTextureVolume::NTextureVolume(const NTextureVolume& texture)
{
    m_Image = texture.m_Image;
}

NTextureVolume& NTextureVolume::operator = (const NTextureVolume& texture)
{
    if (this == &texture)
        return *this;   // Handle self assignment
    m_Image = texture.m_Image;
    return *this;
}

NTextureVolume::~NTextureVolume()
{

}

bool NTextureVolume::Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource)
{
    nuxAssertMsg(BitmapData, TEXT("[NTextureVolume::Update] Argument BitmapData is NULL."));
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    if(!BitmapData->isVolumeTextureData())
    {
        nuxAssertMsg(0, TEXT("[NTextureVolume::Update] Argument BitmapData is not a Volume texture"));
        return false;
    }
    m_Image = *static_cast<const NVolumeData*>(BitmapData);
    // call the texture manager and recreate the texture (NGLTexture2D) associated with this object if any.
    GetThreadGraphicsContext()->UpdateResource(this);
    return true;
}

bool NTextureVolume::Update(const TCHAR* filename, bool UpdateAndCacheResource)
{
    NBitmapData* BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, TEXT("[NTextureVolume::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    bool ret = Update(filename);
    NUX_SAFE_DELETE(BitmapData);
    return ret;
}

void NTextureVolume::GetData(void* Buffer, int MipIndex, int StrideY, int slice)
{
    BYTE*               Dest        = (BYTE*)Buffer;
//     const BYTE*         Src         = m_Image.GetSurface(MipIndex, slice).GetPtrRawData();
//     int                 RowByteSize = m_Image.GetSurface(MipIndex, slice).GetPitch();
//     int                 NumRows     = m_Image.GetSurface(MipIndex, slice).GetBlockHeight();

    for(t_s32 slice = 0; slice < ImageSurface::GetLevelDim(m_Image.GetFormat(), m_Image.GetDepth(), MipIndex); slice++)
    {
        const BYTE*         Src         = m_Image.GetSurface(MipIndex, slice).GetPtrRawData();
        int                 RowByteSize = m_Image.GetSurface(MipIndex, slice).GetPitch();
        int                 NumRows     = m_Image.GetSurface(MipIndex, slice).GetBlockHeight();
        for( int Y = 0; Y < NumRows; Y++ )
        {
            // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but 
            // they contain the same amount of valid data since they have the same width, height and format.
            Memcpy(Dest + Y * StrideY, &Src[Y * RowByteSize], Min(RowByteSize, StrideY));
        }
        Dest += NumRows * StrideY;
    }

//     BYTE*	            Dest        = (BYTE*)Buffer;
//     const BYTE*         Src         = m_Image.GetSurface(MipIndex, slice).GetPtrRawData();
//     int                 RowByteSize = m_Image.GetSurface(MipIndex, slice).GetPitch();
//     int                 NumRows     = m_Image.GetSurface(MipIndex, slice).GetBlockHeight();
// 
//     for( int Y = 0; Y < NumRows; Y++ )
//     {
//         // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but 
//         // they contain the same amount of valid data since they have the same width, height and format.
//         Memcpy(Dest + Y * StrideY,&Src[Y * RowByteSize], Min(RowByteSize, StrideY));
//     }
}

NGLTextureVolume::NGLTextureVolume(NResourceSet* ResourceManager, NTextureVolume* SourceTexture)
:   NGLTexture(ResourceManager)
{
    if(SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }
    m_Texture = GetThreadGLDeviceFactory()->CreateVolumeTexture(SourceTexture->GetWidth(),
        SourceTexture->GetHeight(),
        SourceTexture->GetDepth(),
        SourceTexture->GetNumMipLevel(),
        SourceTexture->GetFormat());

    for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
        LoadMipLevel(SourceTexture, i);
    }
}

NGLTextureVolume::~NGLTextureVolume()
{

}

void NGLTextureVolume::UpdateTexture( NTexture* SourceTexture )
{
    if((SourceTexture == 0) || SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }
    if(!SourceTexture->Type().IsObjectType(NTextureVolume::StaticObjectType))
    {
        nuxAssertMsg(0, TEXT("[NGLTextureVolume::UpdateTexture] Source texture is not of type NTextureVolume."));
        return;
    }

    if( RecreateTexture(SourceTexture) )
    {
        m_Texture = NULL;

        m_Texture = GetThreadGLDeviceFactory()->CreateVolumeTexture(SourceTexture->GetWidth(),
            SourceTexture->GetHeight(),
            SourceTexture->GetDepth(),
            SourceTexture->GetNumMipLevel(),
            SourceTexture->GetFormat());

        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
    else
    {
        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
}

void NGLTextureVolume::LoadMipLevel(NTexture* SourceTexture, int MipLevel)
{
    VOLUME_LOCKED_BOX       LockedBox;
    TRefGL<IOpenGLVolumeTexture> VolumeTexture = m_Texture.CastRef<IOpenGLVolumeTexture>();
    //NTextureVolume*     Source          = UpCastResource<NTextureVolume, NTexture>(SourceTexture);

    //for(int slice = 0; slice < ImageSurface::GetLevelDim(Source->GetFormat(), Source->GetDepth(), MipLevel); slice++)
    {
        OGL_CALL( VolumeTexture->LockBox(MipLevel, &LockedBox, NULL) );
        SourceTexture->GetData(LockedBox.pBits, MipLevel, LockedBox.RowPitch, 0);
        OGL_CALL( VolumeTexture->UnlockBox(MipLevel) );
    }

//     for(int slice = 0; slice < depth; slice++)
//     {
//         OGL_CALL( VolumeTexture->LockRect(slice, MipLevel, &LockedRect, NULL) );
//         SourceTexture->GetData(LockedRect.pBits, MipLevel, LockedRect.Pitch, slice);
//         OGL_CALL( VolumeTexture->UnlockRect(slice, MipLevel ) );
//     }
}

//////////////////////////////////////////////////////////////////////////
NAnimatedTexture::NAnimatedTexture()
{
}

NAnimatedTexture::NAnimatedTexture(const NAnimatedTexture& texture)
{
    m_Image = texture.m_Image;
}

NAnimatedTexture& NAnimatedTexture::operator = (const NAnimatedTexture& texture)
{
    if (this == &texture)
        return *this;   // Handle self assignment
    m_Image = texture.m_Image;
    return *this;
}

NAnimatedTexture::~NAnimatedTexture()
{

}

bool NAnimatedTexture::Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource)
{
    nuxAssertMsg(BitmapData, TEXT("[NAnimatedTexture::Update] Argument BitmapData is NULL."));
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    if(!BitmapData->isAnimatedTextureData())
    {
        nuxAssertMsg(0, TEXT("[NAnimatedTexture::Update] Argument BitmapData is not a Animated texture"));
        return false;
    }
    m_Image = *static_cast<const NAnimatedTextureData*>(BitmapData);
    // call the texture manager and recreate the texture (NGLTexture2D) associated with this object if any.
    GetThreadGraphicsContext()->UpdateResource(this);
    return true;
}

bool NAnimatedTexture::Update(const TCHAR* filename, bool UpdateAndCacheResource)
{
    NBitmapData* BitmapData = LoadImageFile(filename);
    nuxAssertMsg(BitmapData, TEXT("[NAnimatedTexture::Update] Bitmap for file (%s) is NULL."), filename);
    NUX_RETURN_VALUE_IF_NULL(BitmapData, false);
    bool ret = Update(BitmapData);
    NUX_SAFE_DELETE(BitmapData);
    return ret;
}

void NAnimatedTexture::GetData(void* Buffer, int MipIndex, int StrideY, int slice)
{
    BYTE*               Dest        = (BYTE*)Buffer;
    //for(int slice = 0; slice < ImageSurface::GetLevelDim(m_Image.GetFormat(), m_Image.GetDepth(), MipIndex); slice++)
    {
        const BYTE*         Src         = m_Image.GetSurface(slice).GetPtrRawData();
        int                 RowByteSize = m_Image.GetSurface(slice).GetPitch();
        int                 NumRows     = m_Image.GetSurface(slice).GetBlockHeight();
        for(int Y = 0; Y < NumRows; Y++)
        {
            // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but 
            // they contain the same amount of valid data since they have the same width, height and format.
            Memcpy(Dest + Y * StrideY, &Src[Y * RowByteSize], Min(RowByteSize, StrideY));
        }
        //Dest += NumRows * StrideY;
    }
}

int NAnimatedTexture::GetFrameTime(int Frame)
{
    return m_Image.GetFrameTime(Frame);
}

NGLAnimatedTexture::NGLAnimatedTexture(NResourceSet* ResourceManager, NAnimatedTexture* SourceTexture)
:   NGLTexture(ResourceManager)
{
    if(SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }
    m_Texture = GetThreadGLDeviceFactory()->CreateAnimatedTexture(SourceTexture->GetWidth(),
        SourceTexture->GetHeight(),
        SourceTexture->GetDepth(),
        SourceTexture->GetFormat());

    for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
    {
        LoadMipLevel(SourceTexture, i);
    }
}

NGLAnimatedTexture::~NGLAnimatedTexture()
{

}

void NGLAnimatedTexture::UpdateTexture( NTexture* SourceTexture )
{
    if((SourceTexture == 0) || SourceTexture->IsNull())
    {
        m_Texture = 0;
        return;
    }
    if(!SourceTexture->Type().IsObjectType(NAnimatedTexture::StaticObjectType))
    {
        nuxAssertMsg(0, TEXT("[NGLAnimatedTexture::UpdateTexture] Source texture is not of type NAnimatedTexture."));
        return;
    }

    if(RecreateTexture(SourceTexture))
    {
        m_Texture = NULL;

        m_Texture = GetThreadGLDeviceFactory()->CreateAnimatedTexture(SourceTexture->GetWidth(),
            SourceTexture->GetHeight(),
            SourceTexture->GetDepth(),
            SourceTexture->GetFormat());

        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
    else
    {
        for(int i = 0; i < SourceTexture->GetNumMipLevel(); i++)
        {
            LoadMipLevel(SourceTexture, i);
        }
    }
}

void NGLAnimatedTexture::LoadMipLevel(NTexture* SourceTexture, int MipLevel)
{
    SURFACE_LOCKED_RECT       LockedRect;
    TRefGL<IOpenGLAnimatedTexture> AnimatedTexture = m_Texture.CastRef<IOpenGLAnimatedTexture>();
    NAnimatedTexture*     Source          = UpCastResource<NAnimatedTexture, NTexture>(SourceTexture);

    for(int frame = 0; frame < Source->GetDepth(); frame++)
    {
        OGL_CALL( AnimatedTexture->LockRect(frame, &LockedRect, NULL) );
        SourceTexture->GetData(LockedRect.pBits, 0, LockedRect.Pitch, frame);
        OGL_CALL( AnimatedTexture->UnlockRect(frame) );

        AnimatedTexture->SetFrameTime(frame, Source->GetFrameTime(frame));
    }
}

} //NUX_NAMESPACE_END
