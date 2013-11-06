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


#ifndef IOPENGLCUBETEXTURE_H
#define IOPENGLCUBETEXTURE_H

namespace nux
{

  class IOpenGLBaseTexture;
  class IOpenGLCubeTexture: public IOpenGLBaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLCubeTexture, IOpenGLBaseTexture);

  public:
    virtual ~IOpenGLCubeTexture();

    int GetCubeMapSurface(
      eCUBEMAP_FACES FaceType,
      int Level,
      IOpenGLSurface **ppCubeMapSurface
    );

    int CubeLockRect(
      eCUBEMAP_FACES FaceType,
      int Level,
      SURFACE_LOCKED_RECT *pLockedRect,
      const SURFACE_RECT *pRect);

    int CubeUnlockRect(
      eCUBEMAP_FACES FaceType,
      int Level
    );

    unsigned int EnableGammaCorrection(bool b);

    int GetLevelDesc(
      int Level,
      SURFACE_DESC *pDesc
    )
    {
      nuxAssert(Level >= 0 );
      nuxAssert(Level < _NumMipLevel);

      if ((Level < 0) || (Level > _NumMipLevel))
      {
        pDesc->Width    = 0;
        pDesc->Height   = 0;
        pDesc->PixelFormat   = BITFMT_UNKNOWN;
        pDesc->Type     = _ResourceType;
      }
      else
      {
        pDesc->Width    = Max<unsigned int> (1, _Width >> Level);
        pDesc->Height   = Max<unsigned int> (1, _Height >> Level);
        pDesc->PixelFormat   = _PixelFormat;
        pDesc->Type     = _ResourceType;
      }

      return OGL_OK;
    }
  private:

    IOpenGLCubeTexture(
      unsigned int EdgeLength
      , int Levels
      , BitmapFormat PixelFormat);

    //    unsigned int        _Width;
    //    unsigned int        _Height;
    std::map<eCUBEMAP_FACES, std::vector<IOpenGLSurface *>* > _SurfaceArray;

    friend class GpuDevice;
    friend class IOpenGLSurface;
  };

}
#endif // IOPENGLCUBETEXTURE_H
