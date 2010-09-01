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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef IOPENGLTEXTURE2D_H
#define IOPENGLTEXTURE2D_H

NAMESPACE_BEGIN_OGL

class IOpenGLBaseTexture;
class IOpenGLTexture2D: public IOpenGLBaseTexture
{
    DECLARE_OBJECT_TYPE(IOpenGLTexture2D, IOpenGLBaseTexture);
public:
    virtual ~IOpenGLTexture2D();

    void GetSurfaceLevel(int Level, TRefGL<IOpenGLSurface>& surface);
    TRefGL<IOpenGLSurface> GetSurfaceLevel(int Level);


    int LockRect(
        int Level,
        SURFACE_LOCKED_RECT * pLockedRect,
        const SURFACE_RECT * pRect);

    int UnlockRect(
        int Level
        );
    unsigned int EnableGammaCorrection(bool b);

    int GetLevelDesc(
        int Level,
        SURFACE_DESC * pDesc
        )
    {
        nuxAssert(Level >= 0 );
        nuxAssert(Level < _NumMipLevel);

        if((Level < 0) || (Level > _NumMipLevel))
        {
            pDesc->Width    = 0;
            pDesc->Height   = 0;
            pDesc->PixelFormat   = BITFMT_UNKNOWN;
            pDesc->Type     = _ResourceType;
        }
        else
        {
            pDesc->Width    = Max<unsigned int>(1, _Width >> Level);
            pDesc->Height   = Max<unsigned int>(1, _Height >> Level);
            pDesc->PixelFormat   = _PixelFormat;
            pDesc->Type     = _ResourceType;
        }

        return 1;
    }
private:

    // The Dummy boolean is used to create a skeletal frame for the default 
    // render target texture and surface; Only GLDeviceFactory uses it in its constructor.
    // Do not use it otherwise!!!

    IOpenGLTexture2D(
        unsigned int Width
        , unsigned int Height
        , unsigned int Levels
        , BitmapFormat PixelFormat, bool Dummy = false);

    //    unsigned int        _Width;
    //    unsigned int        _Height;
    std::vector< TRefGL<IOpenGLSurface> > _SurfaceArray;
    friend class GLDeviceFactory;
    friend class IOpenGLSurface;
};

NAMESPACE_END_OGL

#endif // IOPENGLTEXTURE2D_H
