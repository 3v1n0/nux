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


#ifndef IOPENGLSURFACE_H
#define IOPENGLSURFACE_H

namespace nux
{

  class IOpenGLResource;
  class IOpenGLSurface: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLSurface, IOpenGLResource);
  public:
    virtual int RefCount() const;

    int LockRect(
      SURFACE_LOCKED_RECT *pLockedRect,
      const SURFACE_RECT *pRect);
    int UnlockRect();

    BitmapFormat GetPixelFormat() const;
    int GetWidth() const;
    int GetHeight() const;
    int GetMipLevel() const;
    int GetSurfaceTarget() const;
    int GetDesc(SURFACE_DESC *pDesc);

    //! Copy the render target into the texture mip level. 
    void CopyRenderTarget(int x, int y, int width, int height);

    //! Return the mipmap data.
    /*!
        Return the mipmap data.

        @param width Returns the width in pixel of the image data.
        @param height Returns the height in pixel of the image data.
        @param stride Returns the row stride of the image data.
    */
    unsigned char* GetSurfaceData(int &width, int &height, int &stride);

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
    IOpenGLSurface(IOpenGLBaseTexture *DeviceBaseTexture
      , GLenum OpenGLID
      , GLenum TextureTarget
      , GLenum SurfaceTarget
      , int MipLevel
      , int Slice = 0 /*for volume textures*/
      , NUX_FILE_LINE_PROTO);

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

