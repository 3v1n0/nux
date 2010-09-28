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


#ifndef GLTEXTURERESOURCEMANAGER_H
#define GLTEXTURERESOURCEMANAGER_H

#include "GLResourceManager.h"
#include "IOpenGLBaseTexture.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class NTextureData;
class NTexture;
class NGLTexture;

NTexture* CreateTextureFromFile(const TCHAR* TextureFilename);
NTexture* CreateTextureFromBitmapData(const NBitmapData* BitmapData);

class NTexture: public NResource
{
    NUX_DECLARE_OBJECT_TYPE(NTexture, NResource);

    NTexture();
    virtual ~NTexture();

    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true) = 0;
    virtual bool Update(const TCHAR* filename, bool UpdateAndCacheResource = true) = 0;

    virtual void GetData(void* Buffer, int MipIndex, int StrideY, int face = 0) = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetDepth() const = 0;
    virtual bool IsPowerOfTwo() const = 0;
    virtual BitmapFormat GetFormat() const = 0;
    virtual int GetNumMipLevel() const = 0;
    virtual bool IsNull() const = 0;
    TRefGL<IOpenGLBaseTexture> GetDeviceTexture();
    TRefGL<NGLTexture> GetCachedTexture();
};

class NTexture2D: public NTexture
{
    NUX_DECLARE_OBJECT_TYPE(NTexture2D, NTexture);

public:
    NTexture2D();
    NTexture2D(const NTexture2D& texture);
    NTexture2D(const NTextureData& TextureData);
    NTexture2D& operator = (const NTexture2D& texture);
    ~NTexture2D();

    /*!
        Update the hardware resources associated to this with the provided texture data.
        @param BitmapData The texture data to update into the hardware resource.
        @param UpdateAndCacheResource if True, then the texture data is loaded into this object, and the caching into
        hardware data is done right away. If false, the caching is done latter by calling 
        GetThreadGraphicsContext()->CacheResource(this);
        @return True is there was not error. 
    */
    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);

    /*!
        Update the hardware resources associated to this object with the data associated to the file name.
        @param Filename File name of texture data to update into the hardware resource.
        @param UpdateAndCacheResource if True, then the texture data is loaded into this object, and the caching into
        hardware data is done right away. If false, the caching is done latter by calling 
        GetThreadGraphicsContext()->CacheResource(this);
        @return True is there was not error. 
    */
    virtual bool Update(const TCHAR* Filename, bool UpdateAndCacheResource = true);

    virtual bool IsNull() const {return m_Image.IsNull();}
    void GetData(void* Buffer, int MipIndex, int StrideY, int face = 0);
    int GetNumMipLevel() const {return m_Image.GetNumMipmap();}
    int GetWidth() const  {return m_Image.GetWidth();}
    int GetHeight() const {return m_Image.GetHeight();}
    int GetDepth() const {return 1;}
    BitmapFormat GetFormat() const {return m_Image.GetFormat();}
    bool IsPowerOfTwo() const {return IsPowerOf2(m_Image.GetWidth()) && IsPowerOf2(m_Image.GetHeight());}

    virtual NString GetResourceName() { return NString(TEXT("NTexture2D")); }

private:
    NTextureData m_Image;
};

class NRectangleTexture: public NTexture
{
    NUX_DECLARE_OBJECT_TYPE(NRectangleTexture, NTexture);

public:
    NRectangleTexture();
    //NRectangleTexture(const NTextureData& Image);
    NRectangleTexture(const NRectangleTexture& texture);
    NRectangleTexture& operator = (const NRectangleTexture& texture);
    ~NRectangleTexture();

    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);
    virtual bool Update(const TCHAR* filename, bool UpdateAndCacheResource = true);

    virtual bool IsNull() const {return m_Image.IsNull();}
    void GetData(void* Buffer, int MipIndex, int StrideY, int face = 0);
    int GetNumMipLevel() const {return m_Image.GetNumMipmap();}
    int GetWidth() const  {return m_Image.GetWidth();}
    int GetHeight() const {return m_Image.GetHeight();}
    int GetDepth() const {return 1;}
    BitmapFormat GetFormat() const {return m_Image.GetFormat();}
    bool IsPowerOfTwo() const {return IsPowerOf2(m_Image.GetWidth()) && IsPowerOf2(m_Image.GetHeight());}

    virtual NString GetResourceName() { return NString(TEXT("NRectangleTexture")); }

private:
    NTextureData m_Image;
};

class NTextureCube: public NTexture
{
    NUX_DECLARE_OBJECT_TYPE(NTextureCube, NTexture);

public:
    NTextureCube();
    //NTexture2D(const NTextureData& Image);
    NTextureCube(const NTextureCube& texture);
    NTextureCube& operator = (const NTextureCube& texture);
    ~NTextureCube();

    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);
    virtual bool Update(const TCHAR* filename, bool UpdateAndCacheResource = true);

    virtual bool IsNull() const {return m_Image.IsNull();}
    void GetData(void* Buffer, int MipIndex, int StrideY, int face = 0);
    int GetNumMipLevel() const {return m_Image.GetNumMipmap();}
    int GetWidth() const  {return m_Image.GetWidth();}
    int GetHeight() const {return m_Image.GetHeight();}
    int GetDepth() const {return 1;}
    BitmapFormat GetFormat() const {return m_Image.GetFormat();}
    bool IsPowerOfTwo() const {return IsPowerOf2(m_Image.GetWidth()) && IsPowerOf2(m_Image.GetHeight());}

    virtual NString GetResourceName() { return NString(TEXT("NTextureCube")); }
private:
    NCubemapData m_Image;
};

class NTextureVolume: public NTexture
{
    NUX_DECLARE_OBJECT_TYPE(NTextureVolume, NTexture);

public:
    NTextureVolume();
    //NTexture2D(const NTextureData& Image);
    NTextureVolume(const NTextureVolume& texture);
    NTextureVolume& operator = (const NTextureVolume& texture);
    ~NTextureVolume();

    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);
    virtual bool Update(const TCHAR* filename, bool UpdateAndCacheResource = true);

    virtual bool IsNull() const {return m_Image.IsNull();}
    void GetData(void* Buffer, int MipIndex, int StrideY, int slice = 0);
    int GetNumMipLevel() const {return m_Image.GetNumMipmap();}
    int GetWidth() const  {return m_Image.GetWidth();}
    int GetHeight() const {return m_Image.GetHeight();}
    int GetDepth() const {return m_Image.GetDepth();}
    BitmapFormat GetFormat() const {return m_Image.GetFormat();}
    bool IsPowerOfTwo() const {return IsPowerOf2(m_Image.GetWidth()) && IsPowerOf2(m_Image.GetHeight());}

    NVolumeData m_Image;

    virtual NString GetResourceName() { return NString(TEXT("NTextureVolume")); }
private:
    //NVolumeData m_Image;
};

class NAnimatedTexture: public NTexture
{
    NUX_DECLARE_OBJECT_TYPE(NAnimatedTexture, NTexture);

public:
    NAnimatedTexture();
    NAnimatedTexture(const NAnimatedTexture& texture);
    NAnimatedTexture& operator = (const NAnimatedTexture& texture);
    ~NAnimatedTexture();

    virtual bool Update(const NBitmapData* BitmapData, bool UpdateAndCacheResource = true);
    virtual bool Update(const TCHAR* filename, bool UpdateAndCacheResource = true);

    virtual bool IsNull() const {return m_Image.IsNull();}
    void GetData(void* Buffer, int MipIndex, int StrideY, int slice = 0);
    int GetFrameTime(int Frame);
    int GetNumMipLevel() const {return m_Image.GetNumMipmap();}
    int GetWidth() const  {return m_Image.GetWidth();}
    int GetHeight() const {return m_Image.GetHeight();}
    int GetDepth() const {return m_Image.GetDepth();}
    BitmapFormat GetFormat() const {return m_Image.GetFormat();}
    bool IsPowerOfTwo() const {return IsPowerOf2(m_Image.GetWidth()) && IsPowerOf2(m_Image.GetHeight());}

    NAnimatedTextureData m_Image;

    virtual NString GetResourceName() { return NString(TEXT("NAnimatedTexture")); }
private:
    //NVolumeData m_Image;
};

class NGLTexture: public NGLResource
{
    NUX_DECLARE_OBJECT_TYPE(NGLTexture, NGLResource);
public: 
    TRefGL<IOpenGLBaseTexture>	m_Texture;

    NGLTexture(NResourceSet* ResourceManager)
        :   NGLResource(ResourceManager),
        SourceWidth(0),
        SourceHeight(0),
        SourceDepth(0),
        SourceFormat(BITFMT_UNKNOWN)
    {

    }
    ~NGLTexture()
    {

    }

    virtual void LoadMipLevel(NTexture* SourceTexture, int MipLevel) = 0;

    virtual bool UpdateResource(NResource* Resource) 
    { 
        UpdateTexture((NTexture*)Resource); 
        return TRUE; 
    }

    bool RecreateTexture(NTexture* Source);

    virtual void UpdateTexture(NTexture* Source) = 0;

    unsigned int SourceWidth;
    unsigned int SourceHeight;
    unsigned int SourceDepth;
    BitmapFormat SourceFormat;
};

class NGLTexture2D: public NGLTexture
{
    NUX_DECLARE_OBJECT_TYPE(NGLTexture2D, NGLTexture);
public: 
    NGLTexture2D(NResourceSet* ResourceManager, NTexture2D* SourceTexture);
    ~NGLTexture2D();

    virtual void UpdateTexture(NTexture* Source);
    virtual void LoadMipLevel(NTexture* SourceTexture, int MipLevel);
};

class NGLRectangleTexture: public NGLTexture
{
    NUX_DECLARE_OBJECT_TYPE(NGLRectangleTexture, NGLTexture);
public: 
    NGLRectangleTexture(NResourceSet* ResourceManager, NRectangleTexture* SourceTexture);
    ~NGLRectangleTexture();

    virtual void UpdateTexture(NTexture* Source);
    virtual void LoadMipLevel(NTexture* SourceTexture, int MipLevel);
};

class NGLTextureCube: public NGLTexture
{
    NUX_DECLARE_OBJECT_TYPE(NGLTextureCube, NGLTexture);
public: 
    NGLTextureCube(NResourceSet* ResourceManager, NTextureCube* SourceTexture);
    ~NGLTextureCube();

    virtual void UpdateTexture(NTexture* Source);
    virtual void LoadMipLevel(NTexture* SourceTexture, int MipLevel);
};

class NGLTextureVolume: public NGLTexture
{
    NUX_DECLARE_OBJECT_TYPE(NGLTextureVolume, NGLTexture);
public: 
    NGLTextureVolume(NResourceSet* ResourceManager, NTextureVolume* SourceTexture);
    ~NGLTextureVolume();

    virtual void UpdateTexture(NTexture* Source);
    virtual void LoadMipLevel(NTexture* SourceTexture, int MipLevel);
};

class NGLAnimatedTexture: public NGLTexture
{
    NUX_DECLARE_OBJECT_TYPE(NGLAnimatedTexture, NGLTexture);
public: 
    NGLAnimatedTexture(NResourceSet* ResourceManager, NAnimatedTexture* SourceTexture);
    ~NGLAnimatedTexture();

    virtual void UpdateTexture(NTexture* Source);
    virtual void LoadMipLevel(NTexture* SourceTexture, int MipLevel);
};

} //NUX_NAMESPACE_END

#endif // GLTEXTURERESOURCEMANAGER_H
