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

  t_s32 GetTextureSize (IOpenGLBaseTexture *pTexture);

// todo: It should not be possible to create an object of type IOpenGLBaseTexture directly.
  class IOpenGLBaseTexture: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE (IOpenGLBaseTexture, IOpenGLResource);

  public:

    IOpenGLBaseTexture (OpenGLResourceType ResourceType,
                        t_u32 Width,
                        t_u32 Height,
                        t_u32 Depth,
                        t_u32 NumMipLevel,
                        BitmapFormat PixelFormat, NUX_FILE_LINE_PROTO);

    virtual ~IOpenGLBaseTexture();

    BitmapFormat GetPixelFormat() const
    {
      return _PixelFormat;
    }
    t_s32 GetNumMipLevel() const
    {
      return _NumMipLevel;
    }
    bool IsPowerOfTwo() const
    {
      return _IsPOT;
    }

    t_s32 GetWidth() const
    {
      if (_ResourceType == RTTEXTURE)
        return _Width;

      if (_ResourceType == RTTEXTURERECTANGLE)
        return _Width;

      if (_ResourceType == RTCUBETEXTURE)
        return _Width;

      return 0;
    }

    t_s32 GetHeight() const
    {
      if (_ResourceType == RTTEXTURE)
        return _Height;

      if (_ResourceType == RTTEXTURERECTANGLE)
        return _Height;

      if (_ResourceType == RTCUBETEXTURE)
        return _Width;

      return 0;
    }

    t_s32 GetDepth() const
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
    void SetFiltering (t_u32 MIN = GL_NEAREST, t_u32 MAG = GL_NEAREST/*, t_u32 MIP = GL_NEAREST*/);
    void SetWrap (t_u32 U = GL_REPEAT, t_u32 V = GL_REPEAT, t_u32 W = GL_REPEAT);
    void SetLOD (float MinLod = -1000.0f, float MaxLod = +1000.0f);
    void SetMipLevel (t_u32 MinMip = 0, t_u32 MaxMip = 1000);
    void SetBorderColor (float R, float G, float B, float A);

    int BindTexture();
    int BindTextureToUnit (int TextureUnitIndex);

    virtual void GetSurfaceLevel (int Level, IntrusiveSP<IOpenGLSurface>& surface);
    virtual IntrusiveSP<IOpenGLSurface> GetSurfaceLevel (int Level);
    virtual int LockRect (
      int Level,
      SURFACE_LOCKED_RECT *pLockedRect,
      const SURFACE_RECT *pRect);

    virtual int UnlockRect (
      int Level
      );

  protected:
    GLTextureStates _TextureStates;
    bool            _IsPOT;             // is power of two?
    t_s32           _NumMipLevel;
    BitmapFormat    _PixelFormat;

    // These parameters are scalable across textures, cube textures and volume textures.
    // For texture and cube texture _Depth is equal to 1.
    // For cube texture, _Width = _Height
    t_s32        _Width;
    t_s32        _Height;
    int         _Depth;
    int         _RowMemoryAlignment;

    friend class IOpenGLSurface;
    friend class IOpenGLVolume;
    friend void GetTextureDesc (
      IOpenGLBaseTexture *pTexture,
      t_u32 Level,
      TEXTURE_DESC *pDesc
    );
    friend t_s32 GetTextureSize (IOpenGLBaseTexture *pTexture);
  };

}

#endif // IOPENGLBASETEXTURE_H
