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
#include "IOpenGLRectangleTexture.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLRectangleTexture);

  IOpenGLRectangleTexture::IOpenGLRectangleTexture(
    unsigned int Width
    , unsigned int Height
    , unsigned int Levels
    , BitmapFormat PixelFormat, bool Dummy, NUX_FILE_LINE_DECL)
    : IOpenGLBaseTexture(RTTEXTURERECTANGLE, Width, Height, 1, Levels, PixelFormat, NUX_FILE_LINE_PARAM)
  {
#ifndef NUX_OPENGLES_20
    if (Dummy == false)
    {
      CHECKGL(glGenTextures(1, &_OpenGLID));
      CHECKGL(glBindTexture(GL_TEXTURE_RECTANGLE_ARB, _OpenGLID));
    }

    //_SurfaceArray.Empty(Levels);
    for (unsigned int l = 0; l < Levels; l++)
    {
      IOpenGLSurface *surface = new IOpenGLSurface(this, _OpenGLID, GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_RECTANGLE_ARB, l, 0, NUX_FILE_LINE_PARAM);

      if (Dummy == false) surface->InitializeLevel();

      _SurfaceArray.push_back(ObjectPtr<IOpenGLSurface> (surface));
      surface->UnReference();
    }

    SetFiltering(GL_NEAREST, GL_NEAREST);
    SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    SetRenderStates();
    GRunTimeStats.Register(this);
#endif
  }

  IOpenGLRectangleTexture::~IOpenGLRectangleTexture()
  {
    for (int l = 0; l < _NumMipLevel; l++)
    {
      _SurfaceArray[l] = ObjectPtr<IOpenGLSurface> (0);;
    }

    _SurfaceArray.clear();
    CHECKGL(glDeleteTextures(1, &_OpenGLID));
    _OpenGLID = 0;
    GRunTimeStats.UnRegister(this);
  }

  ObjectPtr<IOpenGLSurface> IOpenGLRectangleTexture::GetSurfaceLevel(int Level)
  {
    if (Level < _NumMipLevel)
    {
      return _SurfaceArray[Level];
    }
    else
    {
      nuxAssertMsg(0, "[IOpenGLRectangleTexture::GetSurfaceLevel] Invalid surface level");
    }

    return ObjectPtr<IOpenGLSurface> (0);
  }

  void IOpenGLRectangleTexture::GetSurfaceLevel(int Level, ObjectPtr<IOpenGLSurface>& surface)
  {
    surface = GetSurfaceLevel(Level);
  }

  int IOpenGLRectangleTexture::LockRect(
    int Level,
    SURFACE_LOCKED_RECT *pLockedRect,
    const SURFACE_RECT *pRect)
  {
    nuxAssertMsg(pLockedRect, "[IOpenGLRectangleTexture::LockRect] Invalid parameter 'pLockedRect'.");
    nuxAssertMsg(Level >= 0, "[IOpenGLRectangleTexture::LockRect] Invalid mipmap level.");
    nuxAssertMsg(Level < _NumMipLevel, "[IOpenGLRectangleTexture::LockRect] Invalid mipmap level.");


    if (Level < _NumMipLevel)
    {
      ObjectPtr<IOpenGLSurface> pSurfaceLevel = _SurfaceArray[Level];
      return pSurfaceLevel->LockRect(pLockedRect, pRect);
    }
    else
    {
      pLockedRect->pBits = 0;
      pLockedRect->Pitch = 0;
      return OGL_INVALID_SURFACE_LEVEL;
    }

    return OGL_OK;
  }

  int IOpenGLRectangleTexture::UnlockRect(
    int Level
  )
  {
    nuxAssertMsg(Level >= 0, "[IOpenGLRectangleTexture::LockRect] Invalid mipmap level.");
    nuxAssertMsg(Level < _NumMipLevel, "[IOpenGLRectangleTexture::LockRect] Invalid mipmap level.");

    if (Level < _NumMipLevel)
    {
      ObjectPtr<IOpenGLSurface> pSurfaceLevel = _SurfaceArray[Level];
      return pSurfaceLevel->UnlockRect();
    }
    else
    {
      return OGL_INVALID_SURFACE_LEVEL;
    }

    return OGL_OK;
  }

  unsigned int IOpenGLRectangleTexture::EnableGammaCorrection(bool /* b */)
  {
    nuxAssert(_OpenGLID);
    return OGL_OK;
  }

  unsigned char* IOpenGLRectangleTexture::GetSurfaceData(int level, int &width, int &height, int &stride)
  {
    width = 0;
    height = 0;
    stride = 0;

    if (level < 0)
    {
      nuxAssertMsg(level >= 0, "[IOpenGLRectangleTexture::GetSurfaceData] Invalid mipmap level.");
      return NULL;
    }

    if (level >= _NumMipLevel)
    {
      nuxAssertMsg(level < _NumMipLevel, "[IOpenGLRectangleTexture::GetSurfaceData] Invalid mipmap level.");
      return NULL;
    }

    ObjectPtr<IOpenGLSurface> pSurfaceLevel = _SurfaceArray[level];
    return pSurfaceLevel->GetSurfaceData(width, height, stride);
  }
}

