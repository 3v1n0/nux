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


#ifndef IOPENGLRECTANGLETEXTURE_H
#define IOPENGLRECTANGLETEXTURE_H

namespace nux
{

  class IOpenGLBaseTexture;
  class IOpenGLRectangleTexture: public IOpenGLBaseTexture
  {
    NUX_DECLARE_OBJECT_TYPE(IOpenGLRectangleTexture, IOpenGLBaseTexture);

  public:
    virtual ~IOpenGLRectangleTexture();

    void GetSurfaceLevel(int Level, ObjectPtr<IOpenGLSurface>& surface);
    ObjectPtr<IOpenGLSurface> GetSurfaceLevel(int Level);

    int LockRect(
      int Level,
      SURFACE_LOCKED_RECT *pLockedRect,
      const SURFACE_RECT *pRect);

    int UnlockRect(
      int Level
    );
    unsigned int EnableGammaCorrection(bool b);

    int GetLevelDesc(
      int Level,
      SURFACE_DESC *pDesc
    )
    {
      nuxAssert(Level >= 0);
      nuxAssert(Level < _NumMipLevel);

      if ((Level < 0) || (Level > _NumMipLevel))
      {
        pDesc->Width    = 0;
        pDesc->Height   = 0;
        pDesc->PixelFormat = BITFMT_UNKNOWN;
        pDesc->Type     = _ResourceType;
      }
      else
      {
        pDesc->Width    = Max<unsigned int> (1, _Width >> Level);
        pDesc->Height   = Max<unsigned int> (1, _Height >> Level);
        pDesc->PixelFormat = _PixelFormat;
        pDesc->Type     = _ResourceType;
      }

      return OGL_OK;
    }

    //! Return a pointer to a mipmap level data.
    /*!
        Return a pointer to a mipmap level data. The data is in the RGBA format.\n
        Inherited from IOpenGLBaseTexture.

        @param level The requested texture mipmap level data.
        @param width Returns the width in pixel of the image data.
        @param height Returns the height in pixel of the image data.
        @param stride Returns the row stride of the image data.

        @return A pointer to RGBA data. The caller must dlete the data by calling delete [].
    */
    unsigned char* GetSurfaceData(int level, int &width, int &height, int &stride);

  private:

    // The Dummy boolean is used to create a skeletal frame for the default
    // render target texture and surface; Only GpuDevice uses it in its constructor.
    // Do not use it otherwise!!!

    IOpenGLRectangleTexture(
      unsigned int Width
      , unsigned int Height
      , unsigned int Levels
      , BitmapFormat PixelFormat, bool Dummy = false, NUX_FILE_LINE_PROTO);

    //    unsigned int        _Width;
    //    unsigned int        _Height;
    std::vector< ObjectPtr<IOpenGLSurface> > _SurfaceArray;
    friend class GpuDevice;
    friend class IOpenGLSurface;
  };

}

#endif // IOPENGLRECTANGLETEXTURE_H
