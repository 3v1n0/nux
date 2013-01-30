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
#include "IOpenGLSurface.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(IOpenGLSurface);

  IOpenGLSurface::IOpenGLSurface(IOpenGLBaseTexture *DeviceBaseTexture
    , GLenum OpenGLID
    , GLenum TextureTarget
    , GLenum SurfaceTarget
    , int MipLevel
    , int Slice
    , NUX_FILE_LINE_DECL)
    : IOpenGLResource(RTSURFACE, NUX_FILE_LINE_PARAM)
    , _STextureTarget(TextureTarget)
    , _SSurfaceTarget(SurfaceTarget)
    , _SMipLevel(MipLevel)
    , _SSlice(Slice)
    , _BaseTexture(DeviceBaseTexture)
    , _AllocatedUnpackBuffer(0xFFFFFFFF)
  {
    // IOpenGLSurface surfaces are created inside a IOpenGLTexture2D, IOpenGLCubeTexture and IOpenGLVolumeTexture.
    // They reside within those classes. The reference counting starts once a call to GetSurfaceLevel,
    // GetCubeMapSurface or GetVolumeLevel is made to the container object.
    _RefCount = 0;
    _OpenGLID = OpenGLID;
    _LockedRect.pBits = 0;
    _LockedRect.Pitch = 0;
    _CompressedDataSize = 0;
    _Initialized = 0;
  }

  IOpenGLSurface::~IOpenGLSurface()
  {

  }

  BitmapFormat IOpenGLSurface::GetPixelFormat() const
  {
    if (_BaseTexture == 0)
    {
      nuxAssert(0);  // should not happen
      return BITFMT_UNKNOWN;
    }

    return _BaseTexture->GetPixelFormat();
  }

  int IOpenGLSurface::GetWidth() const
  {
    if (_BaseTexture == 0)
    {
      nuxAssert(0);  // should not happen
      return 0;
    }

    return ImageSurface::GetLevelDim(_BaseTexture->_PixelFormat, _BaseTexture->_Width, _SMipLevel);
  }

  int IOpenGLSurface::GetHeight() const
  {
    if (_BaseTexture == 0)
    {
      nuxAssert(0);  // should not happen
      return 0;
    }

    return ImageSurface::GetLevelDim(_BaseTexture->_PixelFormat, _BaseTexture->_Height, _SMipLevel);
  }

  int IOpenGLSurface::GetMipLevel() const
  {
    return _SMipLevel;
  }

  int IOpenGLSurface::GetSurfaceTarget() const
  {
    return _SSurfaceTarget;
  }

  int IOpenGLSurface::GetDesc(SURFACE_DESC *pDesc)
  {
    pDesc->Width    = GetWidth();
    pDesc->Height   = GetHeight();
    pDesc->PixelFormat   = GetPixelFormat();
    pDesc->Type     = _ResourceType;
    return OGL_OK;
  }

  int IOpenGLSurface::RefCount() const
  {
    if (_BaseTexture)
      return _BaseTexture->RefCount();

    nuxAssert(0); // Surface with no underlying texture. That should not happen.
    return 0;
  }

  int IOpenGLSurface::LockRect(
    SURFACE_LOCKED_RECT *pLockedRect,
    const SURFACE_RECT *pRect)
  {
    // If _LockedRect.pBits or _LockedRect.Pitch are not equal to zero, then we have already Locked the buffer
    // Unlock it before locking again.
    nuxAssert(_LockedRect.pBits == 0);
    nuxAssert(_LockedRect.Pitch == 0);
    nuxAssert(_CompressedDataSize == 0);

    if ((_LockedRect.pBits != 0) || (_LockedRect.Pitch != 0) || (_CompressedDataSize != 0))
    {
      // already locked;
      return OGL_INVALID_LOCK;
    }

    _Rect.bottom = _Rect.left = _Rect.right = _Rect.top = 0;

    GLint unpack_alignment = GPixelFormats[_BaseTexture->_PixelFormat].RowMemoryAlignment;
    unsigned int halfUnpack = Log2(unpack_alignment);

    CHECKGL(glBindTexture(_STextureTarget, _BaseTexture->_OpenGLID));

    unsigned int surface_size = 0;
    unsigned int BytePerPixel = 0;

    IOpenGLBaseTexture *texture = _BaseTexture;

    if (!((_BaseTexture->_ResourceType == RTTEXTURE) ||
          (_BaseTexture->_ResourceType == RTTEXTURERECTANGLE) ||
          (_BaseTexture->_ResourceType == RTCUBETEXTURE) ||
          (_BaseTexture->_ResourceType == RTVOLUMETEXTURE) ||
          (_BaseTexture->_ResourceType == RTANIMATEDTEXTURE)))
    {
      nuxAssertMsg(0, "Unknown resource type");
    }

    int texwidth, texheight;
    texwidth = ImageSurface::GetLevelDim(texture->_PixelFormat, texture->_Width, _SMipLevel);
    texheight = ImageSurface::GetLevelDim(texture->_PixelFormat, texture->_Height, _SMipLevel);

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
        _CompressedDataSize = surface_size;
        _LockedRect.Pitch = ImageSurface::GetLevelPitch(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);

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
        _CompressedDataSize = surface_size;
        _LockedRect.Pitch = ImageSurface::GetLevelPitch(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);

        if (_Initialized == false)
        {
          InitializeLevel();
        }
      }
    }
    else
    {
      _LockedRect.Pitch = ImageSurface::GetLevelPitch(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);
      surface_size = ImageSurface::GetLevelSize(texture->_PixelFormat, texture->_Width, texture->_Height, _SMipLevel);

      if (_Initialized == false)
      {
        InitializeLevel();
      }
    }

    _Rect.left  = 0;
    _Rect.top   = 0;
    _Rect.bottom  = texheight;
    _Rect.right   = texwidth;


    if (GetGraphicsDisplay()->GetGpuDevice()->UsePixelBufferObjects())
    {
      GetGraphicsDisplay()->GetGpuDevice()->AllocateUnpackPixelBufferIndex(&_AllocatedUnpackBuffer);
    }

    if (pRect == 0)
    {
      if (GetGraphicsDisplay()->GetGpuDevice()->UsePixelBufferObjects())
      {
        // Mapping the entire area of the surface
        _LockedRect.pBits = GetGraphicsDisplay()->GetGpuDevice()->LockUnpackPixelBufferIndex(_AllocatedUnpackBuffer, surface_size);
        pLockedRect->pBits = _LockedRect.pBits;
        pLockedRect->Pitch = _LockedRect.Pitch;
      }
      else
      {
        //[DEBUGGING - NO PBO]
        // Mapping the entire area of the surface
        _LockedRect.pBits = new BYTE[surface_size];
        pLockedRect->pBits = _LockedRect.pBits;
        pLockedRect->Pitch = _LockedRect.Pitch;
      }
    }
    else
    {
      //[WARNING]
      // this section of code is suppose to handle rectangles that are not the size of the entire surface.
      // It works for uncompressed texture. However it hasn't been tested for compressed texture.

      // reserve and lock a surface size equal  to(RectWidth * RectHeight * BytePerPixel)
      int RectWidth = pRect->right - pRect->left;
      int RectHeight = pRect->bottom - pRect->top;

      nuxAssert(RectWidth >= 0);
      nuxAssert(RectHeight >= 0);


      unsigned int RectSize = (((RectWidth * BytePerPixel + (unpack_alignment - 1)) >> (halfUnpack)) << (halfUnpack)) * RectHeight;

      if (RectSize == 0)
      {
        _LockedRect.pBits = NULL;
        _LockedRect.Pitch = 0;
        pLockedRect->pBits = NULL;
        pLockedRect->Pitch = 0;
        return OGL_INVALID_LOCK;
      }

      if (GetGraphicsDisplay()->GetGpuDevice()->UsePixelBufferObjects())
      {
        _LockedRect.pBits = GetGraphicsDisplay()->GetGpuDevice()->LockUnpackPixelBufferIndex(_AllocatedUnpackBuffer, RectSize);
        pLockedRect->pBits = ((BYTE *) _LockedRect.pBits);
        pLockedRect->Pitch = (((RectWidth * BytePerPixel + (unpack_alignment - 1)) >> (halfUnpack)) << (halfUnpack));
      }
      else
      {
        //[DEBUGGING - NO PBO]
        _LockedRect.pBits = new BYTE[RectSize];
        pLockedRect->pBits = ((BYTE *) _LockedRect.pBits);
        pLockedRect->Pitch = (((RectWidth * BytePerPixel + (unpack_alignment - 1)) >> (halfUnpack)) << (halfUnpack));
      }

      _Rect.left  = pRect->left;
      _Rect.top   = pRect->top;
      _Rect.bottom  = pRect->bottom;
      _Rect.right   = pRect->right;
    }

    return OGL_OK;
  }

  int IOpenGLSurface::UnlockRect()
  {
    if (_LockedRect.pBits == 0)
    {
      return OGL_INVALID_UNLOCK;
    }

    CHECKGL(glPixelStorei(GL_UNPACK_ALIGNMENT, _BaseTexture->GetFormatRowMemoryAlignment()));

#ifndef NUX_OPENGLES_20
    if (_STextureTarget == GL_TEXTURE_2D || _STextureTarget == GL_TEXTURE_RECTANGLE_ARB || _STextureTarget == GL_TEXTURE_CUBE_MAP || _STextureTarget == GL_TEXTURE_3D)
#else
    if (_STextureTarget == GL_TEXTURE_2D)
#endif
    {
      BYTE *DataPtr = 0;
      int w = _Rect.right - _Rect.left;
      int h = _Rect.bottom - _Rect.top;
      CHECKGL(glBindTexture(_STextureTarget, _BaseTexture->_OpenGLID));

#ifndef NUX_OPENGLES_20
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
#endif
        DataPtr = (BYTE *) _LockedRect.pBits;
#ifndef NUX_OPENGLES_20
      }
#endif

      IOpenGLTexture2D *texture = (IOpenGLTexture2D *) _BaseTexture;

#ifndef NUX_OPENGLES_20
      if ( /*texture->_PixelFormat == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||*/
        texture->_PixelFormat == BITFMT_DXT1 ||
        texture->_PixelFormat == BITFMT_DXT2 ||
        texture->_PixelFormat == BITFMT_DXT3 ||
        texture->_PixelFormat == BITFMT_DXT4 ||
        texture->_PixelFormat == BITFMT_DXT5)
      {
        nuxAssert(_CompressedDataSize != 0);
        int width = Max<int> (1, texture->_Width >> _SMipLevel);
        int height = Max<int> (1, texture->_Height >> _SMipLevel);

        int xoffset = _Rect.left;
        int yoffset = _Rect.top;

        if (_STextureTarget != GL_TEXTURE_3D)
        {
          glCompressedTexSubImage2DARB(_SSurfaceTarget,
                                        _SMipLevel,                 // level
                                        xoffset,
                                        yoffset,
                                        width,
                                        height,
                                        GPixelFormats[texture->_PixelFormat].PlatformFormat,
                                        _CompressedDataSize,        // image Size
                                        DataPtr                     // data
                                       );
          CHECKGL_MSG(glCompressedTexSubImage2DARB);
        }
        else
        {
          glCompressedTexSubImage3DARB(_SSurfaceTarget,
                                        _SMipLevel,                 // level
                                        xoffset,
                                        yoffset,
                                        0,
                                        width,
                                        height,
                                        _SSlice,
                                        GPixelFormats[texture->_PixelFormat].PlatformFormat,
                                        _CompressedDataSize,        // image Size
                                        DataPtr                     // data
                                       );
          CHECKGL_MSG(glCompressedTexSubImage3DARB);
        }

        // We can use glCompressedTexImage2DARB if we are sure we always lock
        // the entire area of the surface.
        //            glCompressedTexImage2DARB(_SSurfaceTarget,
        //                _SMipLevel,                 // level
        //                texture->_Format,
        //                width,
        //                height,
        //                0,                          // border
        //                _CompressedDataSize,        // image Size
        //                _LockedRect.pBits           // data
        //                );
        //            CHECKGL_MSG(glCompressedTexImage2DARB);


        //            { //[DEBUGGING - Red Texture]
        //                // This is going to put some red in the texture.
        //                // The texture is not compressed.
        //                BYTE *color_array = new BYTE[width*height*4];
        //                for (int i = 0; i < width*height*4; i += 4)
        //                {
        //                    color_array[i + 0] = 0xff;
        //                    color_array[i + 1] = _SMipLevel * 0x3F;
        //                    color_array[i + 2] = 0x0;
        //                    color_array[i + 3] = 0xFF;
        //                }
        //                glTexImage2D(GL_TEXTURE_2D,
        //                    _SMipLevel,
        //                    GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, color_array);
        //                CHECKGL_MSG(glTexImage2D);
        //                delete [] color_array;
        //            }
      }
      else
      {
        //CHECKGL(glPixelStorei(GL_UNPACK_ROW_LENGTH, w));
        if (_STextureTarget != GL_TEXTURE_3D)
        {
#endif
          CHECKGL(glTexSubImage2D(_SSurfaceTarget,
                                     _SMipLevel,
                                     _Rect.left,
                                     _Rect.top,
                                     w,
                                     h,
                                     GPixelFormats[texture->_PixelFormat].Format,
                                     GPixelFormats[texture->_PixelFormat].type,
                                     DataPtr
                                    ));
#ifndef NUX_OPENGLES_20
        }
        else
        {
          CHECKGL(glTexSubImage3D(_SSurfaceTarget,
                                     _SMipLevel,
                                     _Rect.left,
                                     _Rect.top,
                                     _SSlice,          // z offset
                                     w,
                                     h,
                                     1,
                                     GPixelFormats[texture->_PixelFormat].Format,
                                     GPixelFormats[texture->_PixelFormat].type,
                                     DataPtr
                                    ));
        }

        //CHECKGL(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));

        //            CHECKGL(glTexImage2D(_SSurfaceTarget,
        //                _SMipLevel,
        //                GPixelFormats[texture->_PixelFormat].PlatformFormat,
        //                w,
        //                h,
        //                0,
        //                //### todo:
        //                //  - find the correct format matching the internal format
        //                //  - find the correct type matching the internal format
        //                GPixelFormats[texture->_PixelFormat].Format,
        //                GPixelFormats[texture->_PixelFormat].type,
        //                DataPtr
        //                ));

      }
#endif
    }
    else
    {
      nuxDebugMsg("[IOpenGLSurface::UnlockRect] Incorrect Texture Target.");
    }

#ifndef NUX_OPENGLES_20
    if (GetGraphicsDisplay()->GetGpuDevice()->UsePixelBufferObjects())
    {
      CHECKGL(glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0));
      GetGraphicsDisplay()->GetGpuDevice()->FreeUnpackPixelBufferIndex(_AllocatedUnpackBuffer);
    }
    else
    {
#endif
      //[DEBUGGING - NO PBO]
      if (_LockedRect.pBits)
      {
        delete [] ((BYTE *) _LockedRect.pBits);
      }
#ifndef NUX_OPENGLES_20
    }
#endif

    CHECKGL(glPixelStorei(GL_UNPACK_ALIGNMENT, GetGraphicsDisplay()->GetGpuDevice()->GetPixelStoreAlignment()));

    _LockedRect.pBits = 0;
    _LockedRect.Pitch = 0;
    _CompressedDataSize = 0;

    return OGL_OK;
  }

  int IOpenGLSurface::InitializeLevel()
  {
    // Because we use SubImage when unlocking surfaces, we must first get some dummy data in the surface before we can make a lock.
    int texwidth = ImageSurface::GetLevelWidth(_BaseTexture->_PixelFormat, _BaseTexture->_Width, _SMipLevel);
    int texheight = ImageSurface::GetLevelHeight(_BaseTexture->_PixelFormat, _BaseTexture->_Height, _SMipLevel);
    int MemAlignment = ImageSurface::GetMemAlignment(_BaseTexture->_PixelFormat);

    nuxAssert( texwidth > 0 ); // Should never happen
    nuxAssert( texheight > 0 ); // Should never happen

    CHECKGL(glPixelStorei(GL_UNPACK_ALIGNMENT, MemAlignment));

#ifndef NUX_OPENGLES_20
    int size = ImageSurface::GetLevelSize(_BaseTexture->_PixelFormat, _BaseTexture->_Width, _BaseTexture->_Height, _SMipLevel);
    nuxAssert( size > 0 ); // Should never happen

    if ( _BaseTexture->_PixelFormat == BITFMT_DXT1 ||
         _BaseTexture->_PixelFormat == BITFMT_DXT2 ||
         _BaseTexture->_PixelFormat == BITFMT_DXT3 ||
         _BaseTexture->_PixelFormat == BITFMT_DXT4 ||
         _BaseTexture->_PixelFormat == BITFMT_DXT5)
    {
      if (_STextureTarget != GL_TEXTURE_3D)
      {
        glCompressedTexImage2DARB(
          _SSurfaceTarget,
          _SMipLevel,                 // level
          GPixelFormats[_BaseTexture->_PixelFormat].PlatformFormat,
          texwidth,
          texheight,
          0,                          // border
          size,                       // image Size
          NULL                        // data
        );
        CHECKGL_MSG(glCompressedTexImage2DARB);
      }
      else
      {
        //             glCompressedTexImage3DARB(
        //                 _SSurfaceTarget,
        //                 _SMipLevel,                 // level
        //                 GPixelFormats[_BaseTexture->_PixelFormat].PlatformFormat,
        //                 texwidth,
        //                 texheight,
        //                 _SSlice,
        //                 0,                          // border
        //                 size,                       // image Size
        //                 DummyBuffer                           // data
        //                 );
        //             CHECKGL_MSG(glCompressedTexImage3DARB);
      }
    }
    else
    {
      if (_STextureTarget != GL_TEXTURE_3D)
      {
#endif
        glTexImage2D(
          _SSurfaceTarget,
          _SMipLevel,                 // level
          GPixelFormats[_BaseTexture->_PixelFormat].PlatformFormat,
          texwidth,
          texheight,
          0,                          // border
          GPixelFormats[_BaseTexture->_PixelFormat].Format,
          GPixelFormats[_BaseTexture->_PixelFormat].type,
          NULL);
        CHECKGL_MSG(glTexImage2D);
#ifndef NUX_OPENGLES_20
      }
      else
      {
        //             glTexImage3D(
        //                 _SSurfaceTarget,
        //                 _SMipLevel,                 // level
        //                 GPixelFormats[PixelFormat].PlatformFormat,
        //                 texwidth,
        //                 texheight,
        //                 _SSlice,
        //                 0,                          // border
        //                 GPixelFormats[PixelFormat].Format,
        //                 GPixelFormats[PixelFormat].type,
        //                 DummyBuffer);
        //             CHECKGL_MSG(glTexImage3D);
      }
    }
#endif

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
    return OGL_OK;
  }

  void IOpenGLSurface::CopyRenderTarget(int x, int y, int width, int height)
  {
    CHECKGL(glPixelStorei(GL_UNPACK_ALIGNMENT, _BaseTexture->GetFormatRowMemoryAlignment()));

#ifndef NUX_OPENGLES_20
    if (_STextureTarget == GL_TEXTURE_2D || _STextureTarget == GL_TEXTURE_RECTANGLE_ARB || _STextureTarget == GL_TEXTURE_CUBE_MAP || _STextureTarget == GL_TEXTURE_3D)
#else
    if (_STextureTarget == GL_TEXTURE_2D)
#endif
    {
      CHECKGL(glBindTexture(_STextureTarget, _BaseTexture->_OpenGLID));


#ifndef NUX_OPENGLES_20
      if (_STextureTarget != GL_TEXTURE_3D)
      {
        CHECKGL(glCopyTexImage2D(_SSurfaceTarget,
          _SMipLevel,
          GPixelFormats [_BaseTexture->_PixelFormat].Format,
          x,
          y,
          width,
          height,
          0));
      }
      else
      {
        CHECKGL(glCopyTexSubImage3D(_SSurfaceTarget,
          _SMipLevel,
          0,
          0,
          0,
          x,
          y,
          width,
          height));
      }
#else
      CHECKGL(glCopyTexImage2D(_SSurfaceTarget,
        _SMipLevel,
        GPixelFormats [_BaseTexture->_PixelFormat].Format,
        x,
        y,
        width,
        height,
        0));
#endif
    }
  }

  unsigned char* IOpenGLSurface::GetSurfaceData(int &width, int &height, int &stride)
  {
    width = 0;
    height = 0;
    stride = 0;
    
    if (_BaseTexture->_OpenGLID == 0)
    {
      return NULL;
    }

#ifndef NUX_OPENGLES_20
    CHECKGL(glBindTexture(_STextureTarget, _BaseTexture->_OpenGLID));

    // Despite a 1 byte pack alignment not being the most optimum, do it for simplicity.
    CHECKGL(glPixelStorei(GL_PACK_ALIGNMENT, 1));

    // We want RGBA data
    int mip_level_size = GetWidth() * GetHeight() * 4;
    unsigned char* img = new unsigned char[mip_level_size];

    // Internal OpenGL textures are in the RGBA format.
    // If the selected texture image does not contain four components, the following mappings are applied:
    // - Single-component textures are treated as RGBA buffers with red set to the single-component value, green set to 0, blue set to 0, and alpha set to 1.
    //   - red set to component zero
    //   - green set to 0
    //   - blue set to 0
    //   - alpha set to 1.0
    // - Two-component textures are treated as RGBA buffers with:
    //   - red set to component zero
    //   - green set to 0
    //   - blue set to 0
    //   - alpha set to component one
    // 
    // - Three-component textures are treated as RGBA buffers with:
    //   - red set to component zero
    //   - green set to component one
    //   - blue set to component two
    //   - alpha set to 1.0

    CHECKGL(glGetTexImage(_STextureTarget, _SMipLevel, GL_RGBA, GL_UNSIGNED_BYTE, img));

    width = GetWidth();
    height = GetHeight(); 
    stride = width * 4;

    return img;
#else
    //FIXME: need to render to framebuffer and use glReadPixels
    return NULL;
#endif
  }
}

