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


#ifndef IOPENGLVOLUMETEXTURE_H
#define IOPENGLVOLUMETEXTURE_H

namespace nux
{

  class IOpenGLBaseTexture;
  class IOpenGLVolumeTexture: public IOpenGLBaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLVolumeTexture, IOpenGLBaseTexture);

  public:
    virtual ~IOpenGLVolumeTexture();

    int GetVolumeLevel(
      int Level,
      IOpenGLVolume **ppVolumeLevel
    );

    int VolumeLockRect(
      int Slice,
      int Level,
      SURFACE_LOCKED_RECT *pLockedRect,
      const SURFACE_RECT *pRect);

    int VolumeUnlockRect(
      int Slice,
      int Level
    );

    int LockBox(
      int Level,
      VOLUME_LOCKED_BOX *pLockedVolume,
      const VOLUME_BOX *pBox);

    int UnlockBox(
      int Level
    );

    int GetLevelDepth(int MipLevel) const
    {
      return ImageSurface::GetLevelDim(_PixelFormat, _Depth, MipLevel);
    }

    int GetLevelDesc(
      int Level,
      VOLUME_DESC *pDesc
    )
    {
      nuxAssert(Level >= 0 );
      nuxAssert(Level < _NumMipLevel);

      if ((Level < 0) || (Level > _NumMipLevel))
      {
        pDesc->Width    = 0;
        pDesc->Height   = 0;
        pDesc->Depth    = 0;
        pDesc->PixelFormat   = BITFMT_UNKNOWN;
        pDesc->Type     = _ResourceType;
      }
      else
      {
        pDesc->Width    = Max<int> (1, _Width >> Level);
        pDesc->Height   = Max<int> (1, _Height >> Level);
        pDesc->Depth    = _Depth;
        pDesc->PixelFormat   = _PixelFormat;
        pDesc->Type     = _ResourceType;
      }

      return OGL_OK;
    }

  private:

    IOpenGLVolumeTexture(
      int Width
      , int Height
      , int Depth
      , int Levels
      , BitmapFormat PixelFormat);

    std::vector< ObjectPtr<IOpenGLSurface> > *_VolumeSurfaceArray;
    std::vector< ObjectPtr<IOpenGLVolume> > _VolumeArray;

    friend class GpuDevice;
    friend class IOpenGLSurface;
  };

}

#endif // IOPENGLVOLUMETEXTURE_H
