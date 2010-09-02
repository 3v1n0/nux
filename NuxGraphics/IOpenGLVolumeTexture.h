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


#ifndef IOPENGLVOLUMETEXTURE_H
#define IOPENGLVOLUMETEXTURE_H

NAMESPACE_BEGIN_OGL

class IOpenGLBaseTexture;
class IOpenGLVolumeTexture: public IOpenGLBaseTexture
{
    DECLARE_OBJECT_TYPE(IOpenGLVolumeTexture, IOpenGLBaseTexture);

public:
    virtual ~IOpenGLVolumeTexture();

    int GetVolumeLevel(
        int Level,
        IOpenGLVolume ** ppVolumeLevel
        );

    int LockRect(
        int Slice,
        int Level,
        SURFACE_LOCKED_RECT * pLockedRect,
        const SURFACE_RECT * pRect);

    int UnlockRect(
        int Slice,
        int Level
        );

    int LockBox(
        int Level,
        VOLUME_LOCKED_BOX * pLockedVolume,
        const VOLUME_BOX * pBox);

    int UnlockBox(
        int Level
        );

    int GetLevelDepth(int MipLevel) const {return ImageSurface::GetLevelDim(_PixelFormat, _Depth, MipLevel);}

    int GetLevelDesc(
        int Level,
        VOLUME_DESC * pDesc
        )
    {
        nuxAssert(Level >= 0 );
        nuxAssert(Level < _NumMipLevel);

        if((Level < 0) || (Level > _NumMipLevel))
        {
            pDesc->Width    = 0;
            pDesc->Height   = 0;
            pDesc->Depth    = 0;
            pDesc->PixelFormat   = BITFMT_UNKNOWN;
            pDesc->Type     = _ResourceType;
        }
        else
        {
            pDesc->Width    = Max<unsigned int>(1, _Width >> Level);
            pDesc->Height   = Max<unsigned int>(1, _Height >> Level);
            pDesc->Depth    = _Depth;
            pDesc->PixelFormat   = _PixelFormat;
            pDesc->Type     = _ResourceType;
        }
    }

private:

    IOpenGLVolumeTexture(
        unsigned int Width
        , unsigned int Height
        , unsigned int Depth
        , unsigned int Levels
        , BitmapFormat PixelFormat);

    std::vector< TRefGL<IOpenGLSurface> > *_VolumeSurfaceArray;
    std::vector< TRefGL<IOpenGLVolume> > _VolumeArray;

    friend class GLDeviceFactory;
    friend class IOpenGLSurface;
};

NAMESPACE_END_OGL

#endif // IOPENGLVOLUMETEXTURE_H
