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


#ifndef IOPENGLBASETEXTURE_H
#define IOPENGLBASETEXTURE_H

#include "GLResource.h"
#include "IOpenGLResource.h"
#include "GLShaderParameter.h"
#include "GLTextureStates.h"

namespace nux
{

  class IOpenGLResource;
  class IOpenGLBaseTexture;

  int GetTextureSize(IOpenGLBaseTexture *pTexture);

// todo: It should not be possible to create an object of type IOpenGLBaseTexture directly.
  class IOpenGLBaseTexture: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLBaseTexture, IOpenGLResource);

  public:

    IOpenGLBaseTexture(OpenGLResourceType ResourceType,
                        unsigned int Width,
                        unsigned int Height,
                        unsigned int Depth,
                        unsigned int NumMipLevel,
                        BitmapFormat PixelFormat, NUX_FILE_LINE_PROTO);

    virtual ~IOpenGLBaseTexture();

    BitmapFormat GetPixelFormat() const
    {
      return _PixelFormat;
    }
    int GetNumMipLevel() const
    {
      return _NumMipLevel;
    }
    bool IsPowerOfTwo() const
    {
      return _IsPOT;
    }

    int GetWidth() const
    {
      if (_ResourceType == RTTEXTURE)
        return _Width;

      if (_ResourceType == RTTEXTURERECTANGLE)
        return _Width;

      if (_ResourceType == RTCUBETEXTURE)
        return _Width;

      return 0;
    }

    int GetHeight() const
    {
      if (_ResourceType == RTTEXTURE)
        return _Height;

      if (_ResourceType == RTTEXTURERECTANGLE)
        return _Height;

      if (_ResourceType == RTCUBETEXTURE)
        return _Width;

      return 0;
    }

    int GetDepth() const
    {
      if (_ResourceType == RTTEXTURE)
        return 1;

      if (_ResourceType == RTTEXTURERECTANGLE)
        return 1;

      if (_ResourceType == RTCUBETEXTURE)
        return 1;

      if (_ResourceType == RTVOLUMETEXTURE)
        return _Depth;

      if (_ResourceType == RTANIMATEDTEXTURE)
        return _Depth;

      return 0;
    }

    int GetFormatRowMemoryAlignment() const
    {
      return _RowMemoryAlignment;
    }
    //GLTextureStates& GetTextureStates() {return _TextureStates;}

    void SetRenderStates();
    void SetFiltering(unsigned int MIN = GL_NEAREST, unsigned int MAG = GL_NEAREST/*, unsigned int MIP = GL_NEAREST*/);
    void SetWrap(unsigned int U = GL_REPEAT, unsigned int V = GL_REPEAT, unsigned int W = GL_REPEAT);
    void SetLOD(float MinLod = -1000.0f, float MaxLod = +1000.0f);
    void SetMipLevel(unsigned int MinMip = 0, unsigned int MaxMip = 1000);
    void SetBorderColor(float R, float G, float B, float A);

    int BindTexture();
    int BindTextureToUnit(int TextureUnitIndex);

    virtual void GetSurfaceLevel(int Level, ObjectPtr<IOpenGLSurface>& surface);
    virtual ObjectPtr<IOpenGLSurface> GetSurfaceLevel(int Level);
    virtual int LockRect(
      int Level,
      SURFACE_LOCKED_RECT *pLockedRect,
      const SURFACE_RECT *pRect);

    virtual int UnlockRect(int Level);

    //! Return a pointer to a mipmap level data.
    /*!
        Return a pointer to a mipmap level data. The data is in the RGBA format.\n

        @param level The requested texture mipmap level data.
        @param width Returns the width in pixel of the image data.
        @param height Returns the height in pixel of the image data.
        @param stride Returns the row stride of the image data.

        @return A pointer to RGBA data. The caller must delete the data by calling delete [].
    */
    virtual unsigned char* GetSurfaceData(int level, int &width, int &height, int &stride);

    void Save(const char* filename);

  protected:
    GLTextureStates _TextureStates;
    bool            _IsPOT;             // is power of two?
    int           _NumMipLevel;
    BitmapFormat    _PixelFormat;

    // These parameters are scalable across textures, cube textures and volume textures.
    // For texture and cube texture _Depth is equal to 1.
    // For cube texture, _Width = _Height
    int        _Width;
    int        _Height;
    int         _Depth;
    int         _RowMemoryAlignment;

    friend class IOpenGLSurface;
    friend class IOpenGLVolume;
    friend void GetTextureDesc(
      IOpenGLBaseTexture *pTexture,
      unsigned int Level,
      TEXTURE_DESC *pDesc
    );
    friend int GetTextureSize(IOpenGLBaseTexture *pTexture);
  };

}

#endif // IOPENGLBASETEXTURE_H
