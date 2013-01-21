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
#include "IOpenGLVolume.h"
#include "GraphicsDisplay.h"
#include "GpuDevice.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLVolume);

  IOpenGLVolume::~IOpenGLVolume()
  {

  }

  int IOpenGLVolume::RefCount() const
  {
    if (_VolumeTexture)
      return _VolumeTexture->RefCount();

    nuxAssert(0); // Surface with no underlying texture. That should not happen.
    return 0;
  }

  int IOpenGLVolume::LockBox(
    VOLUME_LOCKED_BOX *pLockedVolume,
    const VOLUME_BOX *pBox)

  {
#ifndef NUX_OPENGLES_20
    // If _LockedRect.pBits or _LockedRect.Pitch are not equal to zero, then we have already Locked the buffer
    // Unlock it before locking again.
    nuxAssert(_LockedBox.pBits == 0);
    nuxAssert(_LockedBox.RowPitch == 0);
    //nuxAssert(_LockedBox.SlicePitch == 0);
    nuxAssert(_CompressedDataSize == 0);

    if ((_LockedBox.pBits != 0) || (_LockedBox.RowPitch != 0) || (_CompressedDataSize != 0))
    {
      // already locked;
      return OGL_INVALID_LOCK;
    }

    _Box.Front = _Box.Back = _Box.Bottom = _Box.Left = _Box.Right = _Box.Top = 0;

    //GLint unpack_alignment = GPixelFormats[_VolumeTexture->_PixelFormat].RowMemoryAlignment;


    CHECKGL(glBindTexture(_STextureTarget, _VolumeTexture->_OpenGLID));

    unsigned int surface_size = 0;


    IOpenGLVolumeTexture *texture = _VolumeTexture;

    if (_VolumeTexture->_ResourceType != RTVOLUMETEXTURE)
    {
      nuxAssertMsg(0, "Unknown resource type");
    }

    int texwidth, texheight;
    texwidth = ImageSurface::GetLevelWidth(texture->_PixelFormat, texture->_Width, _SMipLevel);
    texheight = ImageSurface::GetLevelHeight(texture->_PixelFormat, texture->_Height, _SMipLevel);

    if ( texture->_PixelFormat == BITFMT_DXT1 ||
         texture->_PixelFormat == BITFMT_DXT2 ||
         texture->_PixelFormat == BITFMT_DXT3 ||
         texture->_PixelFormat == BITFMT_DXT4 ||
         texture->_PixelFormat == BITFMT_DXT5)
    {
      if (texture->_PixelFormat == BITFMT_DXT1)
      {
        // We can conceive a 4x4 DXT1 block as if each texel uses 4 bits.
        // Actually, for DXT, we have 2 16-bits colors(5:6:5), and each texel uses 2 bits to interpolate
        // between the 2 colors.
        //    ---------------------
        //    |      COLOR0       | 16 bits
        //    ---------------------
        //    |      COLOR1       | 16 bits
        //    ---------------------
        //    | xx | xx | xx | xx | xx = 2 bits
        //    ---------------------
        //    | xx | xx | xx | xx |
        //    ---------------------
        //    | xx | xx | xx | xx |
        //    ---------------------
        //    | xx | xx | xx | xx |
        //    ---------------------

        // A line of n texel DXT1 data uses n/2 bytes(4 bits/texel). So the number of bytes used for a
        // texwidth texel, is texwidth/2 bytes.
        // Note that texwidth is divisible by 4(to to the upper rounding to 4), therefore, it is also divisible
        // by 2.

        // glCompressedTexImage2DARB, glCompressedTexImage3DARB,
        // glCompressedTexSubImage2DARB, glCompressedTexSubImage3DARB are not affected by glPixelStorei.
        surface_size = ImageSurface::GetLevelSize(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);
        _LockedBox.RowPitch = ImageSurface::GetLevelPitch(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);

        if (_Initialized == false)
        {
          InitializeLevel();
        }
      }
      else
      {
        // A line of n texel DXT3/5 data uses n bytes(1 byte/texel). So the number of bytes used for a
        // texwidth texels, is texwidth bytes.

        // glCompressedTexImage2DARB, glCompressedTexImage3DARB,
        // glCompressedTexSubImage2DARB, glCompressedTexSubImage3DARB are not affected by glPixelStorei.
        surface_size = ImageSurface::GetLevelSize(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);
        _LockedBox.RowPitch = ImageSurface::GetLevelPitch(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);

        if (_Initialized == false)
        {
          InitializeLevel();
        }
      }
    }
    else
    {
      _LockedBox.RowPitch = ImageSurface::GetLevelPitch(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);
      surface_size = ImageSurface::GetLevelSize(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);

      if (_Initialized == false)
      {
        InitializeLevel();
      }
    }


    _Box.Left  = 0;
    _Box.Top   = 0;
    _Box.Bottom  = texheight;
    _Box.Right   = texwidth;
    _Box.Front   = 0;
    _Box.Back    = ImageSurface::GetLevelDim(texture->_PixelFormat, texture->GetDepth(), _SMipLevel);

    if (GetGraphicsDisplay()->GetGpuDevice()->UsePixelBufferObjects())
    {
      GetGraphicsDisplay()->GetGpuDevice()->AllocateUnpackPixelBufferIndex(&_AllocatedUnpackBuffer);
    }

    if (pBox == 0)
    {
      _CompressedDataSize = GetDepth() * surface_size;

      if (GetGraphicsDisplay()->GetGpuDevice()->UsePixelBufferObjects())
      {
        // Mapping the entire area of the surface
        if (1)
        {
          _LockedBox.pBits = GetGraphicsDisplay()->GetGpuDevice()->LockUnpackPixelBufferIndex(_AllocatedUnpackBuffer, _CompressedDataSize);
        }
        else
        {
          GetGraphicsDisplay()->GetGpuDevice()->BindUnpackPixelBufferIndex(_AllocatedUnpackBuffer);
          CHECKGL(glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, _CompressedDataSize, NULL, GL_STREAM_DRAW_ARB));
          _LockedBox.pBits = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY_ARB);
          CHECKGL_MSG(glMapBufferARB );
          CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
        }

        pLockedVolume->pBits = _LockedBox.pBits;
        pLockedVolume->RowPitch = _LockedBox.RowPitch;
        pLockedVolume->SlicePitch = surface_size;
      }
      else
      {
        //[DEBUGGING - NO PBO]
        // Mapping the entire area of the surface
        _LockedBox.pBits = new BYTE[_CompressedDataSize];
        pLockedVolume->pBits = _LockedBox.pBits;
        pLockedVolume->RowPitch = _LockedBox.RowPitch;
        pLockedVolume->SlicePitch = surface_size;
      }
    }
    else
    {
      nuxAssert(pBox->Front >= 0);
      nuxAssert(pBox->Back <= GetDepth());
      nuxAssert(pBox->Front < pBox->Back);

      int RectWidth = pBox->Right - pBox->Left;
      int RectHeight = pBox->Bottom - pBox->Top;
      int NumSlice = pBox->Back - pBox->Front;

      nuxAssert(RectWidth >= 0);
      nuxAssert(RectHeight >= 0);
      nuxAssert(NumSlice >= 0);



      unsigned int RectSize = ImageSurface::GetLevelSize(GetPixelFormat(), RectWidth, RectHeight, 0); //(((RectWidth * BytePerPixel + (unpack_alignment-1)) >> (halfUnpack)) << (halfUnpack)) * RectHeight;

      if (RectSize == 0)
      {
        pLockedVolume->pBits = 0;
        pLockedVolume->RowPitch = 0;
        return OGL_INVALID_LOCK;
      }

      _CompressedDataSize = NumSlice * RectSize;

      if (GetGraphicsDisplay()->GetGpuDevice()->UsePixelBufferObjects())
      {
        _LockedBox.pBits = GetGraphicsDisplay()->GetGpuDevice()->LockUnpackPixelBufferIndex(_AllocatedUnpackBuffer, NumSlice * RectSize);

//             GetGraphicsDisplay()->GetGpuDevice()->BindUnpackPixelBufferIndex(_AllocatedUnpackBuffer);
//             CHECKGL(glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, NumSlice * RectSize, NULL, GL_STATIC_DRAW_ARB));
//             _LockedBox.pBits = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY);
//             CHECKGL_MSG(glMapBufferARB );
//             CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));

        pLockedVolume->pBits = ((BYTE *) _LockedBox.pBits);
        pLockedVolume->RowPitch = ImageSurface::GetLevelPitch(GetPixelFormat(), RectWidth, RectHeight, 0); //(((RectWidth * BytePerPixel + (unpack_alignment-1)) >> (halfUnpack)) << (halfUnpack));
        pLockedVolume->SlicePitch = RectSize;
      }
      else
      {
        //[DEBUGGING - NO PBO]
        _LockedBox.pBits = new BYTE[_CompressedDataSize];
        pLockedVolume->pBits = ((BYTE *) _LockedBox.pBits);
        pLockedVolume->RowPitch = ImageSurface::GetLevelPitch(GetPixelFormat(), RectWidth, RectHeight, 0); //(((RectWidth * BytePerPixel + (unpack_alignment-1)) >> (halfUnpack)) << (halfUnpack));
        pLockedVolume->SlicePitch = RectSize;
      }

      _Box.Left   = pBox->Left;
      _Box.Top    = pBox->Top;
      _Box.Bottom = pBox->Bottom;
      _Box.Right  = pBox->Right;
      _Box.Front  = pBox->Front;
      _Box.Back   = pBox->Back;
    }
#endif

    return OGL_OK;
  }

  int IOpenGLVolume::UnlockBox()
  {
#ifndef NUX_OPENGLES_20
    if (_LockedBox.pBits == 0)
    {
      return OGL_INVALID_UNLOCK;
    }

    int MemAlignment = ImageSurface::GetMemAlignment(_VolumeTexture->_PixelFormat);
    CHECKGL(glPixelStorei(GL_UNPACK_ALIGNMENT, MemAlignment));
    nuxAssert(MemAlignment == _VolumeTexture->GetFormatRowMemoryAlignment());

    if (_STextureTarget == GL_TEXTURE_3D)
    {
      BYTE *DataPtr = 0;
      CHECKGL(glBindTexture(_STextureTarget, _VolumeTexture->_OpenGLID));

      if (GetGraphicsDisplay()->GetGpuDevice()->UsePixelBufferObjects())
      {
        // Unmap the texture image buffer
        GetGraphicsDisplay()->GetGpuDevice()->BindUnpackPixelBufferIndex(_AllocatedUnpackBuffer);
        CHECKGL(glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB));
        DataPtr = NUX_BUFFER_OFFSET(0);
      }
      else
      {
        CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
        DataPtr = (BYTE *) _LockedBox.pBits;
      }

      IOpenGLTexture2D *texture = (IOpenGLTexture2D *) _VolumeTexture;
      int width = _Box.Right - _Box.Left;
      int height = _Box.Bottom - _Box.Top;
      int depth = _Box.Back - _Box.Front;
      int xoffset = _Box.Left;
      int yoffset = _Box.Top;
      int zoffset = _Box.Front;

      if ( texture->_PixelFormat == BITFMT_DXT1 ||
           texture->_PixelFormat == BITFMT_DXT2 ||
           texture->_PixelFormat == BITFMT_DXT3 ||
           texture->_PixelFormat == BITFMT_DXT4 ||
           texture->_PixelFormat == BITFMT_DXT5)
      {
        nuxAssert(_CompressedDataSize != 0);
        // This part does not work. The image is messed up.
        // Driver bug??? Tried glCompressedTexImage3DARB. Same result.
        // But I know the data that is loaded is correct.
        glCompressedTexSubImage3DARB(_SSurfaceTarget,
                                      _SMipLevel,                 // level
                                      xoffset,
                                      yoffset,
                                      zoffset,
                                      width,
                                      height,
                                      depth,
                                      GPixelFormats[texture->_PixelFormat].PlatformFormat,
                                      _CompressedDataSize,        // image Size
                                      DataPtr                     // data
                                     );
        CHECKGL_MSG(glCompressedTexSubImage3DARB);

        //             glCompressedTexImage3DARB(_SSurfaceTarget,
        //                 _SMipLevel,             // level
        //                 GPixelFormats[texture->_PixelFormat].PlatformFormat,
        //                 width,
        //                 height,
        //                 depth,
        //                 0,                      // border
        //                 _CompressedDataSize,    // image Size
        //                 DataPtr                 // data
        //                 );
        //             CHECKGL_MSG(glCompressedTexImage2DARB);
      }
      else
      {
        CHECKGL(glTexSubImage3D(_SSurfaceTarget,
                                   _SMipLevel,
                                   xoffset,
                                   yoffset,
                                   zoffset,          // z offset
                                   width,
                                   height,
                                   depth,
                                   GPixelFormats[texture->_PixelFormat].Format,
                                   GPixelFormats[texture->_PixelFormat].type,
                                   DataPtr
                                  ));
      }
    }
    else
    {
      nuxAssertMsg(0, "Incorrect Texture Target.");
    }

    if (GetGraphicsDisplay()->GetGpuDevice()->UsePixelBufferObjects())
    {
      CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
      GetGraphicsDisplay()->GetGpuDevice()->FreeUnpackPixelBufferIndex(_AllocatedUnpackBuffer);
    }
    else
    {
      //[DEBUGGING - NO PBO]
      if (_LockedBox.pBits)
      {
        delete [] ((BYTE *) _LockedBox.pBits);
      }
    }

    CHECKGL(glPixelStorei(GL_UNPACK_ALIGNMENT, GetGraphicsDisplay()->GetGpuDevice()->GetPixelStoreAlignment()));

    _LockedBox.pBits = 0;
    _LockedBox.RowPitch = 0;
    _CompressedDataSize = 0;
#endif

    return OGL_OK;
  }

  int IOpenGLVolume::InitializeLevel()
  {
#ifndef NUX_OPENGLES_20
    // Because we use SubImage when unlocking surfaces, we must first get some dummy data in the surface before we can make a lock.
    int volwidth = ImageSurface::GetLevelDim(_VolumeTexture->_PixelFormat, _VolumeTexture->_Width, _SMipLevel);
    int volheight = ImageSurface::GetLevelDim(_VolumeTexture->_PixelFormat, _VolumeTexture->_Height, _SMipLevel);
    int voldepth = ImageSurface::GetLevelDim(_VolumeTexture->_PixelFormat, _VolumeTexture->_Depth, _SMipLevel);
    int size = ImageSurface::GetLevelSize(
                 _VolumeTexture->_PixelFormat,
                 _VolumeTexture->_Width,
                 _VolumeTexture->_Height,
                 _VolumeTexture->_Depth,
                 _SMipLevel);

    int MemAlignment = ImageSurface::GetMemAlignment(_VolumeTexture->_PixelFormat);

    nuxAssert( volwidth > 0 ); // Should never happen
    nuxAssert( volheight > 0 ); // Should never happen
    nuxAssert( voldepth > 0 ); // Should never happen
    nuxAssert( size > 0 ); // Should never happen

    BYTE *DummyBuffer = new BYTE[size];
    Memset(DummyBuffer, 0, size);

    CHECKGL(glPixelStorei(GL_UNPACK_ALIGNMENT, MemAlignment));

    if ( _VolumeTexture->_PixelFormat == BITFMT_DXT1 ||
         _VolumeTexture->_PixelFormat == BITFMT_DXT2 ||
         _VolumeTexture->_PixelFormat == BITFMT_DXT3 ||
         _VolumeTexture->_PixelFormat == BITFMT_DXT4 ||
         _VolumeTexture->_PixelFormat == BITFMT_DXT5)
    {
      {
        glCompressedTexImage3DARB(
          _SSurfaceTarget,
          _SMipLevel,                 // level
          GPixelFormats[_VolumeTexture->_PixelFormat].PlatformFormat,
          volwidth,
          volheight,
          voldepth,
          0,                          // border
          size,                       // image Size
          DummyBuffer                           // data
        );
        CHECKGL_MSG(glCompressedTexImage3DARB);
      }
    }
    else
    {
      glTexImage3D(
        _SSurfaceTarget,
        _SMipLevel,                 // level
        GPixelFormats[_VolumeTexture->_PixelFormat].PlatformFormat,
        volwidth,
        volheight,
        voldepth,
        0,                          // border
        GPixelFormats[_VolumeTexture->_PixelFormat].Format,
        GPixelFormats[_VolumeTexture->_PixelFormat].type,
        DummyBuffer);
      CHECKGL_MSG(glTexImage3D);
    }

    delete [] DummyBuffer;


    //    { //[DEBUGGING - Red Texture]
    //        // This is going to put some red in the texture.
    //        // The texture is not compressed.
    //        BYTE *color_array = new BYTE[texwidth*texheight*4];
    //        for (unsigned int i = 0; i < texwidth*texheight*4; i += 4)
    //        {
    //            color_array[i + 0] = 0xff;
    //            color_array[i + 1] = _SMipLevel * 0x3F;
    //            color_array[i + 2] = 0x0;
    //            color_array[i + 3] = 0xFF;
    //        }
    //        glTexImage2D(GL_TEXTURE_2D,
    //            _SMipLevel,
    //            GL_RGBA8, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, color_array);
    //        CHECKGL_MSG(glTexImage2D);
    //        delete [] color_array;
    //    }

    CHECKGL(glPixelStorei(GL_UNPACK_ALIGNMENT, GetGraphicsDisplay()->GetGpuDevice()->GetPixelStoreAlignment()));

    _Initialized = true;
#endif
    return OGL_OK;
  }

  BitmapFormat IOpenGLVolume::GetPixelFormat() const
  {
    if (_VolumeTexture == 0)
      return BITFMT_UNKNOWN;
    else
      return _VolumeTexture->GetPixelFormat();
  }

  int IOpenGLVolume::GetWidth() const
  {
    if (_VolumeTexture->_ResourceType == RTVOLUMETEXTURE)
      return ImageSurface::GetLevelDim(_VolumeTexture->_PixelFormat, _VolumeTexture->_Width, _SMipLevel);

    nuxAssert(0); // Should not happen
    return 0;
  }

  int IOpenGLVolume::GetHeight() const
  {
    if (_VolumeTexture->_ResourceType == RTVOLUMETEXTURE)
      return ImageSurface::GetLevelDim(_VolumeTexture->_PixelFormat, _VolumeTexture->_Height, _SMipLevel);

    nuxAssert(0); // Should not happen
    return 0;
  }

  int IOpenGLVolume::GetDepth() const
  {
    if (_VolumeTexture->_ResourceType == RTVOLUMETEXTURE)
      return ImageSurface::GetLevelDim(_VolumeTexture->_PixelFormat, _VolumeTexture->_Depth, _SMipLevel);

    nuxAssert(0); // Should not happen
    return 0;
  }

}
