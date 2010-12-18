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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef IOPENGLSURFACE_H
#define IOPENGLSURFACE_H

namespace nux
{

  class IOpenGLResource;
  class IOpenGLSurface: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE (IOpenGLSurface, IOpenGLResource);
  public:
    virtual int RefCount() const;

    int LockRect (
      SURFACE_LOCKED_RECT *pLockedRect,
      const SURFACE_RECT *pRect);
    int UnlockRect();

    BitmapFormat GetPixelFormat() const
    {
      if (_BaseTexture == 0)
      {
        nuxAssert (0);  // should not happen
        return BITFMT_UNKNOWN;
      }

      return _BaseTexture->GetPixelFormat();
    }

    int GetWidth() const
    {
      if (_BaseTexture == 0)
      {
        nuxAssert (0);  // should not happen
        return 0;
      }

      return ImageSurface::GetLevelDim (_BaseTexture->_PixelFormat, _BaseTexture->_Width, _SMipLevel);
    }

    int GetHeight() const
    {
      if (_BaseTexture == 0)
      {
        nuxAssert (0);  // should not happen
        return 0;
      }

      return ImageSurface::GetLevelDim (_BaseTexture->_PixelFormat, _BaseTexture->_Height, _SMipLevel);
    }

    int GetMipLevel() const
    {
      return _SMipLevel;
    }

    int GetSurfaceTarget() const
    {
      return _SSurfaceTarget;
    }

    int GetDesc (SURFACE_DESC *pDesc)
    {
      pDesc->Width    = GetWidth();
      pDesc->Height   = GetHeight();
      pDesc->PixelFormat   = GetPixelFormat();
      pDesc->Type     = _ResourceType;
      return OGL_OK;
    }

  private:
    virtual ~IOpenGLSurface();

    int InitializeLevel();

    // By Default, the surface refers to the face 0 of the texture and to the mip level 0 of that face.
    //    IOpenGLSurface(IOpenGLBaseTexture* DeviceBaseTexture, GLenum OpenGLID)
    //        : _STextureTarget(GL_TEXTURE_2D)
    //        , _SSurfaceTarget(GL_TEXTURE_2D)
    //        , _SMipLevel(0)
    //        , _BaseTexture(DeviceBaseTexture)
    //        , IOpenGLResource(RTSURFACE)
    //    {
    //        // IOpenGLSurface surfaces are created inside a IOpenGLTexture2D, IOpenGLCubeTexture and IOpenGLVolumeTexture.
    //        // They reside within those classes. The reference counting starts once a call to GetSurfaceLevel,
    //        // GetCubeMapSurface or GetVolumeLevel is made to the container object.
    //        _RefCount = 0;
    //        _OpenGLID = OpenGLID;
    //    }
    IOpenGLSurface (IOpenGLBaseTexture *DeviceBaseTexture, GLenum OpenGLID, GLenum TextureTarget, GLenum SurfaceTarget, int MipLevel, int Slice = 0 /*for volume textures*/, NUX_FILE_LINE_PROTO)
      : IOpenGLResource (RTSURFACE, NUX_FILE_LINE_PARAM)
      , _STextureTarget (TextureTarget)
      , _SSurfaceTarget (SurfaceTarget)
      , _SMipLevel (MipLevel)
      , _SSlice (Slice)
      , _BaseTexture (DeviceBaseTexture)
      , _AllocatedUnpackBuffer (0xFFFFFFFF)
    {
      // IOpenGLSurface surfaces are created inside a IOpenGLTexture2D, IOpenGLCubeTexture and IOpenGLVolumeTexture.
      // They reside within those classes. The reference counting starts once a call to GetSurfaceLevel,
      // GetCubeMapSurface or GetVolumeLevel is made to the container object.
      _RefCount = 0;
      _OpenGLID = OpenGLID;
      _LockedRect.pBits = 0;
      _LockedRect.Pitch = 0;
      _CompressedDataSize = 0;
      _Initialized = 0;
    }

    // _STextureTarget may be
    //      GL_TEXTURE_2D
    //      GL_TEXTURE_RECTANGLE_ARB
    //      GL_TEXTURE_CUBE_MAP
    //      GL_TEXTURE_3D
    GLenum      _STextureTarget;

    // _SSurfaceTarget may be
    //      GL_TEXTURE_2D
    //      GL_TEXTURE_3D
    //      GL_TEXTURE_RECTANGLE_ARB
    //      GL_TEXTURE_CUBE_MAP_POSITIVE_X
    //      GL_TEXTURE_CUBE_MAP_NEGATIVE_X
    //      GL_TEXTURE_CUBE_MAP_POSITIVE_Y
    //      GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
    //      GL_TEXTURE_CUBE_MAP_POSITIVE_Z
    //      GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    GLenum      _SSurfaceTarget;
    int        _SMipLevel;
    int        _SSlice;                     // for volume texture

    SURFACE_LOCKED_RECT  _LockedRect;
    SURFACE_RECT        _Rect;
    int            _CompressedDataSize;

    IOpenGLBaseTexture *_BaseTexture;
    bool            _Initialized;

    int _AllocatedUnpackBuffer;
    friend class IOpenGLTexture2D;
    friend class IOpenGLRectangleTexture;
    friend class IOpenGLCubeTexture;
    friend class IOpenGLVolumeTexture;
    friend class IOpenGLAnimatedTexture;
    friend class ObjectPtr<IOpenGLSurface>;
  };

}

#endif // IOPENGLSURFACE_H
