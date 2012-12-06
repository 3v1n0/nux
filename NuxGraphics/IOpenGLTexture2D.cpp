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
#include "IOpenGLTexture2D.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLTexture2D);
  IOpenGLTexture2D::IOpenGLTexture2D(unsigned int Width
                                      , unsigned int Height
                                      , unsigned int Levels
                                      , BitmapFormat PixelFormat, bool Dummy, NUX_FILE_LINE_DECL)
    :   IOpenGLBaseTexture(RTTEXTURE, Width, Height, 1, Levels, PixelFormat, NUX_FILE_LINE_PARAM)
  {
    external_id_ = Dummy;
    if (external_id_ == false)
    {
      CHECKGL(glGenTextures(1, &_OpenGLID));
      CHECKGL(glBindTexture(GL_TEXTURE_2D, _OpenGLID));
    }

    //_SurfaceArray.Empty(Levels);
    for (unsigned int l = 0; l < Levels; l++)
    {
      IOpenGLSurface *surface = new IOpenGLSurface(this, _OpenGLID, GL_TEXTURE_2D, GL_TEXTURE_2D, l, 0, NUX_FILE_LINE_PARAM);

      if (Dummy == false) surface->InitializeLevel();

      _SurfaceArray.push_back(ObjectPtr<IOpenGLSurface> (surface));
      surface->UnReference();
    }

    SetFiltering(GL_NEAREST, GL_NEAREST);
    SetWrap(GL_REPEAT, GL_REPEAT, GL_REPEAT);

#ifdef NUX_OPENGLES_20
    // NPOT textures in GLES2 only support GL_CLAMP_TO_EDGE unless
    // GL_OES_texture_npot is supported.
    // TODO: Check for GL_OES_texture_npot
    if (!_IsPOT)
    {
      SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    }
#endif

    SetRenderStates();

    GRunTimeStats.Register(this);
  }

  IOpenGLTexture2D::~IOpenGLTexture2D()
  {
    for (int l = 0; l < _NumMipLevel; l++)
    {
      _SurfaceArray[l] = ObjectPtr<IOpenGLSurface> (0);;
    }

    _SurfaceArray.clear();

    if (external_id_ == false)
    {
      CHECKGL(glDeleteTextures(1, &_OpenGLID));
    }
    GRunTimeStats.UnRegister(this);
    _OpenGLID = 0;

  }

  ObjectPtr<IOpenGLSurface> IOpenGLTexture2D::GetSurfaceLevel(int Level)
  {
    if ((Level >= 0) && (Level < _NumMipLevel))
    {
      return _SurfaceArray[Level];
    }
    else
    {
      nuxAssertMsg(0, "[IOpenGLTexture2D::GetSurfaceLevel] Invalid surface level");
    }

    return ObjectPtr<IOpenGLSurface> (0);
  }

  void IOpenGLTexture2D::GetSurfaceLevel(int Level, ObjectPtr<IOpenGLSurface>& surface)
  {
    surface = GetSurfaceLevel(Level);
  }

  int IOpenGLTexture2D::LockRect(
    int Level,
    SURFACE_LOCKED_RECT *pLockedRect,
    const SURFACE_RECT *pRect)
  {
    nuxAssertMsg(pLockedRect, "[IOpenGLTexture2D::LockRect] Invalid parameter 'pLockedRect'.");
    nuxAssertMsg(Level >= 0, "[IOpenGLTexture2D::LockRect] Invalid mipmap level.");
    nuxAssertMsg(Level < _NumMipLevel, "[IOpenGLTexture2D::LockRect] Invalid mipmap level.");

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

  int IOpenGLTexture2D::UnlockRect(int Level)
  {
    nuxAssertMsg(Level >= 0, "[IOpenGLTexture2D::LockRect] Invalid mipmap level.");
    nuxAssertMsg(Level < _NumMipLevel, "[IOpenGLTexture2D::LockRect] Invalid mipmap level.");

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

  unsigned int IOpenGLTexture2D::EnableGammaCorrection(bool /* b */)
  {
    nuxAssert(_OpenGLID);
    return OGL_OK;
  }

  unsigned char* IOpenGLTexture2D::GetSurfaceData(int level, int &width, int &height, int &stride)
  {
    width = 0;
    height = 0;
    stride = 0;

    if (level < 0)
    {
      nuxAssertMsg(level >= 0, "[IOpenGLTexture2D::GetSurfaceData] Invalid mipmap level.");
      return NULL;
    }

    if (level >= _NumMipLevel)
    {
      nuxAssertMsg(level < _NumMipLevel, "[IOpenGLTexture2D::GetSurfaceData] Invalid mipmap level.");
      return NULL;
    }

    ObjectPtr<IOpenGLSurface> pSurfaceLevel = _SurfaceArray[level];
    return pSurfaceLevel->GetSurfaceData(width, height, stride);
  }
}
