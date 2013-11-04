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


#include "GLDeviceObjects.h"
#include "IOpenGLCubeTexture.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLCubeTexture);

  IOpenGLCubeTexture::IOpenGLCubeTexture(
    unsigned int EdgeLength
    , int Levels
    , BitmapFormat PixelFormat)
    : IOpenGLBaseTexture(RTCUBETEXTURE, EdgeLength, EdgeLength, 1, Levels, PixelFormat)
  {
    CHECKGL(glGenTextures(1, &_OpenGLID));
    CHECKGL(glBindTexture(GL_TEXTURE_CUBE_MAP, _OpenGLID));

    for (unsigned int face = CUBEMAP_FACE_POSITIVE_X; face < CUBEMAP_FACE_NEGATIVE_Z + 1; face++)
    {
      std::vector<IOpenGLSurface *> *array = new std::vector<IOpenGLSurface *>();
      _SurfaceArray[ (eCUBEMAP_FACES) face] = array;

      //array = (*(_SurfaceArray.find((eCUBEMAP_FACES)face))).second;
      for (int l = 0; l < Levels; l++)
      {
        IOpenGLSurface *surface = new IOpenGLSurface(this, _OpenGLID, GL_TEXTURE_CUBE_MAP, face, l);
        surface->InitializeLevel();
        array->push_back( surface );
        //IOpenGLSurface* surface = new(array) IOpenGLSurface(this, GL_TEXTURE_CUBE_MAP, face, l) ;
      }
    }

    CHECKGL(glBindTexture(GL_TEXTURE_CUBE_MAP, _OpenGLID));
    SetFiltering(GL_NEAREST, GL_NEAREST);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    SetRenderStates();
    GRunTimeStats.Register(this);
  }

  IOpenGLCubeTexture::~IOpenGLCubeTexture()
  {
    for (unsigned int face = CUBEMAP_FACE_POSITIVE_X; face < CUBEMAP_FACE_NEGATIVE_Z + 1; face++)
    {
      std::vector<IOpenGLSurface *> *array = (_SurfaceArray.find((eCUBEMAP_FACES) face))->second;

      for (int l = 0; l < _NumMipLevel; l++)
      {
        ((*array) [l])->UnReference();
      }

      array->clear();
      delete array;
    }

    _SurfaceArray.clear();
    CHECKGL(glDeleteTextures(1, &_OpenGLID));
    _OpenGLID = 0;
    GRunTimeStats.UnRegister(this);
  }

  int IOpenGLCubeTexture::GetCubeMapSurface(
    eCUBEMAP_FACES FaceType,
    int Level,
    IOpenGLSurface **ppCubeMapSurface
  )
  {
    if (Level < _NumMipLevel)
    {
      std::vector<IOpenGLSurface *> *array = (* (_SurfaceArray.find(FaceType))).second;
      *ppCubeMapSurface = (*array) [Level];
      //(*ppCubeMapSurface)->AddRef();
    }
    else
    {
      return OGL_INVALID_SURFACE_LEVEL;
    }

    return 1;
  }

  int IOpenGLCubeTexture::CubeLockRect(
    eCUBEMAP_FACES FaceType,
    int Level,
    SURFACE_LOCKED_RECT *pLockedRect,
    const SURFACE_RECT *pRect)
  {
    nuxAssertMsg(pLockedRect, "[IOpenGLCubeTexture::LockRect] Invalid parameter 'pLockedRect'.");
    nuxAssertMsg(Level >= 0, "[IOpenGLCubeTexture::LockRect] Invalid mipmap level.");
    nuxAssertMsg(Level < _NumMipLevel, "[IOpenGLCubeTexture::LockRect] Invalid mipmap level.");


    if (Level < _NumMipLevel)
    {
      std::vector<IOpenGLSurface *> *array = (* (_SurfaceArray.find(FaceType))).second;
      IOpenGLSurface *pCubeSurfaceLevel = (*array) [Level];
      return pCubeSurfaceLevel->LockRect(pLockedRect, pRect);

    }
    else
    {
      pLockedRect->pBits = 0;
      pLockedRect->Pitch = 0;
      return OGL_INVALID_SURFACE_LEVEL;
    }

    return OGL_OK;
  }

  int IOpenGLCubeTexture::CubeUnlockRect(
    eCUBEMAP_FACES FaceType,
    int Level
  )
  {
    nuxAssertMsg(Level >= 0, "[IOpenGLCubeTexture::LockRect] Invalid mipmap level.");
    nuxAssertMsg(Level < _NumMipLevel, "[IOpenGLCubeTexture::LockRect] Invalid mipmap level.");

    if (Level < _NumMipLevel)
    {
      std::vector<IOpenGLSurface *> *array = (* (_SurfaceArray.find(FaceType))).second;
      IOpenGLSurface *pCubeSurfaceLevel = (*array) [Level];
      return pCubeSurfaceLevel->UnlockRect();
    }
    else
    {
      return OGL_INVALID_SURFACE_LEVEL;
    }

    return OGL_OK;
  }

  unsigned int IOpenGLCubeTexture::EnableGammaCorrection(bool /* b */)
  {
    nuxAssert(_OpenGLID);
    return OGL_OK;
  }
}
