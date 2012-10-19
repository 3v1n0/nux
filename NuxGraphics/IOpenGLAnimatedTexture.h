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


#ifndef IOPENGLANIMATEDTEXTURE_H
#define IOPENGLANIMATEDTEXTURE_H

namespace nux
{

  class IOpenGLBaseTexture;

// Note that for Animated texture, the Depth value is the number of frames.
// The number of mipmap levels is always 1;
  class IOpenGLAnimatedTexture: public IOpenGLBaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLAnimatedTexture, IOpenGLBaseTexture);

  public:
    virtual ~IOpenGLAnimatedTexture();

    void GetSurfaceFrame(int Frame, ObjectPtr<IOpenGLSurface>& surface);
    ObjectPtr<IOpenGLSurface> GetSurfaceFrame(int Level);

    int LockRect(
      int Frame,
      SURFACE_LOCKED_RECT *pLockedRect,
      const SURFACE_RECT *pRect);

    int UnlockRect(
      int Frame
    );

    int GetDepth(int /* MipLevel */) const
    {
      return _Depth;
    }

    int GetDesc(ANIMATEDTEXTURE_DESC *pDesc)
    {
      pDesc->Width    = Max<int> (1, _Width);
      pDesc->Height   = Max<int> (1, _Height);
      pDesc->Depth    = _Depth;
      pDesc->PixelFormat   = _PixelFormat;
      pDesc->Type     = _ResourceType;
      return OGL_OK;
    }

    void PresentFirstFrame();
    void PresentNextFrame();
    void PresentLastFrame();
    int GetFrameTime();
    unsigned int GetNumFrame();
    void SetFrameTime(int Frame, int time_ms);

  private:
    IOpenGLAnimatedTexture(
      int Width
      , int Height
      , int Depth
      , BitmapFormat PixelFormat);

    //    unsigned int        _Width;
    //    unsigned int        _Height;
    //    unsigned int        _Depth;
    //std::vector<IOpenGLSurface*> _VolumeArray;

    int _CurrentFrame;
    std::vector< ObjectPtr<IOpenGLBaseTexture> > _FrameTextureArray;

    std::vector< int >  _FrameTimeArray;

    friend class GpuDevice;
    friend class IOpenGLSurface;
  };
}

#endif // IOPENGLANIMATEDTEXTURE_H
