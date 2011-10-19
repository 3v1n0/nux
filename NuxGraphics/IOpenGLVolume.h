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


#ifndef IOPENGLVOLUME_H
#define IOPENGLVOLUME_H

namespace nux
{

  class IOpenGLResource;
  class IOpenGLVolumeTexture;

  class IOpenGLVolume: public IOpenGLResource
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLVolume, IOpenGLResource);

  public:
    virtual int RefCount() const;

    int LockBox(
      VOLUME_LOCKED_BOX *pLockedVolume,
      const VOLUME_BOX *pBox);

    int UnlockBox();

    BitmapFormat GetPixelFormat() const;
    int GetWidth() const;
    int GetHeight() const;
    int GetDepth() const;

    int GetMipLevel() const
    {
      return _SMipLevel;
    }

    int GetSurfaceTarget() const
    {
      return _SSurfaceTarget;
    }

    int GetLevelDesc(VOLUME_DESC *pDesc)
    {
      pDesc->Width    = GetWidth();
      pDesc->Height   = GetHeight();
      pDesc->Depth    = GetDepth();
      pDesc->PixelFormat   = GetPixelFormat();
      pDesc->Type     = _ResourceType;
      return OGL_OK;
    }

  private:
    virtual ~IOpenGLVolume();

    int InitializeLevel();

    IOpenGLVolume(IOpenGLVolumeTexture *VolumeTexture, GLenum OpenGLID, GLenum TextureTarget, GLenum SurfaceTarget, unsigned int MipLevel)
      : IOpenGLResource(RTVOLUME)
      , _STextureTarget(TextureTarget)
      , _SSurfaceTarget(SurfaceTarget)
      , _SMipLevel(MipLevel)
      , _VolumeTexture(VolumeTexture)
      , _AllocatedUnpackBuffer(0xFFFFFFFF)
    {
      // IOpenGLVolume surfaces are created inside a IOpenGLVolumeTexture.
      // They reside within this class. The reference counting starts once a call to GetVolumeLevel is made to the container object.
      _RefCount = 0;
      _OpenGLID = OpenGLID;
      _LockedBox.pBits = 0;
      _LockedBox.RowPitch = 0;
      _CompressedDataSize = 0;
      _Initialized = 0;
    }

    // _STextureTarget may be
    //      GL_TEXTURE_3D
    GLenum      _STextureTarget;

    // _SSurfaceTarget may be
    //      GL_TEXTURE_3D
    GLenum      _SSurfaceTarget;
    int         _SMipLevel;

    VOLUME_LOCKED_BOX _LockedBox;
    VOLUME_BOX        _Box;
    int               _CompressedDataSize;

    IOpenGLVolumeTexture *_VolumeTexture;
    bool           _Initialized;

    int            _AllocatedUnpackBuffer;

    friend class IOpenGLVolumeTexture;
    friend class ObjectPtr<IOpenGLVolume>;
  };

}

#endif // IOPENGLVOLUME_H
