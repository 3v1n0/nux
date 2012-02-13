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

#include "GLResource.h"
#include "GraphicsDisplay.h"
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "IOpenGLAnimatedTexture.h"


namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLAnimatedTexture);

  IOpenGLAnimatedTexture::IOpenGLAnimatedTexture(
    int Width
    , int Height
    , int Depth
    , BitmapFormat PixelFormat)
    :    IOpenGLBaseTexture(RTANIMATEDTEXTURE, Width, Height, Depth, 1, PixelFormat)
    ,    _CurrentFrame(0)
  {
    for (int i = 0; i < Depth; i++)
    {
      ObjectPtr<IOpenGLBaseTexture> Texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(Width, Height, 1, PixelFormat, NUX_TRACKER_LOCATION);
      _FrameTextureArray.push_back(Texture);
      _FrameTimeArray.push_back(41); // 41 ms = 24 frames/second
    }

    _OpenGLID = _FrameTextureArray[0]->GetOpenGLID();

    SetFiltering(GL_NEAREST, GL_NEAREST);
    SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
    SetRenderStates();
  }

  IOpenGLAnimatedTexture::~IOpenGLAnimatedTexture()
  {
    for (int i = 0; i < _Depth; i++)
    {
      _FrameTextureArray[i].Release();
    }

    _FrameTextureArray.clear();
    _FrameTimeArray.clear();
  }


  ObjectPtr<IOpenGLSurface> IOpenGLAnimatedTexture::GetSurfaceFrame(int Frame)
  {
    nuxAssert(Frame >= 0);
    nuxAssert(Frame < _Depth);

    if ((Frame >= 0) && (Frame < _Depth))
    {
      return _FrameTextureArray[Frame]->GetSurfaceLevel(0);
    }
    else
    {
      nuxAssertMsg(0, "[IOpenGLAnimatedTexture::GetSurfaceFrame] Invalid surface level");
    }

    return ObjectPtr<IOpenGLSurface> (0);
  }

  void IOpenGLAnimatedTexture::GetSurfaceFrame(int Frame, ObjectPtr<IOpenGLSurface>& surface)
  {
    surface = ObjectPtr<IOpenGLSurface> (0);
    surface = GetSurfaceFrame(Frame);
  }

  int IOpenGLAnimatedTexture::LockRect(
    int Frame,
    SURFACE_LOCKED_RECT *pLockedRect,
    const SURFACE_RECT *pRect)
  {
    return _FrameTextureArray[Frame]->LockRect(0, pLockedRect, pRect);
  }

  int IOpenGLAnimatedTexture::UnlockRect(
    int Frame)
  {
    return _FrameTextureArray[Frame]->UnlockRect(0);
  }

  void IOpenGLAnimatedTexture::PresentFirstFrame()
  {
    _CurrentFrame = 0;
    _OpenGLID = _FrameTextureArray[_CurrentFrame]->GetOpenGLID();
  }

  void IOpenGLAnimatedTexture::PresentNextFrame()
  {
    ++_CurrentFrame;

    if (_CurrentFrame >= _Depth)
      _CurrentFrame = 0;

    _OpenGLID = _FrameTextureArray[_CurrentFrame]->GetOpenGLID();
  }

  void IOpenGLAnimatedTexture::PresentLastFrame()
  {
    _CurrentFrame = _Depth - 1;
    _OpenGLID = _FrameTextureArray[_CurrentFrame]->GetOpenGLID();
  }

  void IOpenGLAnimatedTexture::SetFrameTime(int Frame, int time_ms)
  {
    nuxAssert(_CurrentFrame < (int) _FrameTimeArray.size());
    _FrameTimeArray[Frame] = time_ms;
  }

  int IOpenGLAnimatedTexture::GetFrameTime()
  {
    nuxAssert(_CurrentFrame < (int) _FrameTimeArray.size());
    return _FrameTimeArray[_CurrentFrame];
  }

  unsigned int IOpenGLAnimatedTexture::GetNumFrame()
  {
    return _Depth;
  }

}
