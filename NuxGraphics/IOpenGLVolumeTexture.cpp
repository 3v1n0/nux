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


#include "GLDeviceObjects.h"
#include "IOpenGLVolumeTexture.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (IOpenGLVolumeTexture);

  IOpenGLVolumeTexture::IOpenGLVolumeTexture (
    int Width
    , int Height
    , int Depth
    , int Levels
    , BitmapFormat PixelFormat)
    : IOpenGLBaseTexture (RTVOLUMETEXTURE, Width, Height, Depth, Levels, PixelFormat)
  {
    CHECKGL ( glGenTextures (1, &_OpenGLID) );
    CHECKGL ( glBindTexture (GL_TEXTURE_3D, _OpenGLID) );

    _VolumeSurfaceArray = new std::vector< ObjectPtr<IOpenGLSurface> >[_NumMipLevel];

    for (t_s32 mip = 0; mip < _NumMipLevel; mip++)
    {
      for (t_s32 slice = 0; slice < ImageSurface::GetLevelDim (_PixelFormat, _Depth, mip); slice++)
      {
        //IOpenGLSurface* surface = new IOpenGLSurface(this, _OpenGLID, GL_TEXTURE_3D, GL_TEXTURE_3D, mip, slice);
        //surface->InitializeLevel();
        IOpenGLSurface* surface = new IOpenGLSurface (this, _OpenGLID, GL_TEXTURE_3D, GL_TEXTURE_3D, mip, slice);
        _VolumeSurfaceArray[mip].push_back (ObjectPtr<IOpenGLSurface> (surface));
        surface->UnReference ();
      }
    }

    for (int mip = 0; mip < _NumMipLevel; mip++)
    {
      IOpenGLVolume *volume = new IOpenGLVolume (this, _OpenGLID, GL_TEXTURE_3D, GL_TEXTURE_3D, mip);
      volume->InitializeLevel();
      _VolumeArray.push_back (ObjectPtr<IOpenGLVolume> (volume));
    }

    CHECKGL ( glBindTexture (GL_TEXTURE_3D, _OpenGLID) );
    SetFiltering (GL_NEAREST, GL_NEAREST);
    SetWrap (GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    SetRenderStates();
    GRunTimeStats.Register (this);
  }

  IOpenGLVolumeTexture::~IOpenGLVolumeTexture()
  {

    for (t_s32 mip = 0; mip < _NumMipLevel; mip++)
    {
      for (t_s32 slice = 0; slice < ImageSurface::GetLevelDim (_PixelFormat, _Depth, mip); slice++)
      {
        // destroying a surface
        _VolumeSurfaceArray[mip][slice] = ObjectPtr<IOpenGLSurface> (0);
      }

      _VolumeSurfaceArray[mip].clear();
    }

    NUX_SAFE_DELETE_ARRAY (_VolumeSurfaceArray);


    for (int mip = 0; mip < _NumMipLevel; mip++)
    {
      // destroying a IOpenGLVolume
      _VolumeArray[mip] = ObjectPtr<IOpenGLVolume> (0);
    }

    CHECKGL ( glDeleteTextures (1, &_OpenGLID) );
    _OpenGLID = 0;
    GRunTimeStats.UnRegister (this);
  }

  int IOpenGLVolumeTexture::LockRect (
    int Slice,
    int Level,
    SURFACE_LOCKED_RECT *pLockedRect,
    const SURFACE_RECT *pRect)
  {
    nuxAssertMsg (pLockedRect, TEXT ("[IOpenGLVolumeTexture::LockRect] Invalid parameter 'pLockedRect'.") );
    nuxAssertMsg (Level >= 0, TEXT ("[IOpenGLVolumeTexture::LockRect] Invalid mipmap level.") );
    nuxAssertMsg (Level < _NumMipLevel, TEXT ("[IOpenGLVolumeTexture::LockRect] Invalid mipmap level.") );
    nuxAssertMsg (Slice >= 0, TEXT ("[IOpenGLVolumeTexture::LockRect] Invalid slice index.") );
    nuxAssertMsg (Slice < ImageSurface::GetLevelDim (_PixelFormat, _Depth, Level), TEXT ("[IOpenGLVolumeTexture::LockRect] Invalid slice index.") );

    if (Slice < 0)
      Slice = 0;

    if (Slice >= _Depth)
      Slice = _Depth - 1;

    if (Level < _NumMipLevel)
    {
      ObjectPtr<IOpenGLSurface> pVolumeSurfaceLevel = _VolumeSurfaceArray[Level][Slice];
      return pVolumeSurfaceLevel->LockRect (pLockedRect, pRect);
    }
    else
    {
      pLockedRect->pBits = 0;
      pLockedRect->Pitch = 0;
      return OGL_INVALID_SURFACE_LEVEL;
    }

    return OGL_OK;
  }

  int IOpenGLVolumeTexture::UnlockRect (
    int Slice,
    int Level
  )
  {
    nuxAssertMsg (Level >= 0, TEXT ("[IOpenGLVolumeTexture::LockRect] Invalid mipmap level.") );
    nuxAssertMsg (Level < _NumMipLevel, TEXT ("[IOpenGLVolumeTexture::LockRect] Invalid mipmap level.") );

    if (Level < _NumMipLevel)
    {
      ObjectPtr<IOpenGLSurface> pVolumeSurfaceLevel = _VolumeSurfaceArray[Level][Slice];
      return pVolumeSurfaceLevel->UnlockRect();
    }
    else
    {
      return OGL_INVALID_SURFACE_LEVEL;
    }

    return OGL_OK;
  }

  int IOpenGLVolumeTexture::LockBox (int Level,
                                     VOLUME_LOCKED_BOX *pLockedVolume,
                                     const VOLUME_BOX *pBox)
  {
    nuxAssertMsg (pLockedVolume, TEXT ("[IOpenGLVolumeTexture::LockBox] Invalid parameter 'pLockedRect'.") );
    nuxAssertMsg (Level >= 0, TEXT ("[IOpenGLVolumeTexture::LockBox] Invalid mipmap level.") );
    nuxAssertMsg (Level < _NumMipLevel, TEXT ("[IOpenGLVolumeTexture::LockBox] Invalid mipmap level.") );

    if (pBox)
    {
      nuxAssertMsg (pBox->Front >= 0, TEXT ("[IOpenGLVolumeTexture::LockBox] Invalid slice index.") );
      nuxAssertMsg (pBox->Front < pBox->Back, TEXT ("[IOpenGLVolumeTexture::LockBox] Invalid slice index.") );
      nuxAssertMsg (pBox->Back <= ImageSurface::GetLevelDim (_PixelFormat, _Depth, Level),
                    TEXT ("[IOpenGLVolumeTexture::LockBox] Invalid slice index.") );
    }

    return _VolumeArray[Level]->LockBox (pLockedVolume, pBox);
  }

  int IOpenGLVolumeTexture::UnlockBox (int Level)
  {
    nuxAssertMsg (Level >= 0, TEXT ("[IOpenGLVolumeTexture::LockBox] Invalid mipmap level.") );
    nuxAssertMsg (Level < _NumMipLevel, TEXT ("[IOpenGLVolumeTexture::LockBox] Invalid mipmap level.") );

    return _VolumeArray[Level]->UnlockBox();
  }

}
