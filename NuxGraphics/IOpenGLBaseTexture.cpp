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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore/NuxCore.h"
#include "GLResource.h"
#include "IOpenGLResource.h"
#include "GLShaderParameter.h"
#include "GLTextureStates.h"
#include "IOpenGLBaseTexture.h"
#include "IOpenGLSurface.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (IOpenGLBaseTexture);

  IOpenGLBaseTexture::IOpenGLBaseTexture (OpenGLResourceType ResourceType,
                                          t_u32 Width,
                                          t_u32 Height,
                                          t_u32 Depth,
                                          t_u32 NumMipLevel,
                                          BitmapFormat PixelFormat, NUX_FILE_LINE_DECL)
    : IOpenGLResource (ResourceType, NUX_FILE_LINE_PARAM)
    , _IsPOT (false)
    , _NumMipLevel (NumMipLevel)
    , _PixelFormat (PixelFormat)
    , _Width (Width)
    , _Height (Height)
    , _Depth (Depth)
    , _RowMemoryAlignment (1)
  {
    //     if( (GPixelFormats[PixelFormat].BlockBytes == 1) ||
    //         (GPixelFormats[PixelFormat].BlockBytes == 2) ||
    //         (GPixelFormats[PixelFormat].BlockBytes == 3))
    //         _RowMemoryAlignment = 1;
    //     else
    //         _RowMemoryAlignment = 4;


    if (_ResourceType == RTTEXTURE)
      _TextureStates.SetType (GL_TEXTURE_2D);

    if (_ResourceType == RTTEXTURERECTANGLE)
      _TextureStates.SetType (GL_TEXTURE_RECTANGLE_ARB);

    if (_ResourceType == RTCUBETEXTURE)
      _TextureStates.SetType (GL_TEXTURE_CUBE_MAP_ARB);

    if (_ResourceType == RTVOLUMETEXTURE)
      _TextureStates.SetType (GL_TEXTURE_3D);

    if (_ResourceType == RTANIMATEDTEXTURE)
      _TextureStates.SetType (GL_TEXTURE_RECTANGLE_ARB);

    _RowMemoryAlignment = ImageSurface::GetMemAlignment (PixelFormat);

    if (IsPowerOf2 (Width) && IsPowerOf2 (Height) )
    {
      _IsPOT = true;
    }
  }

  IOpenGLBaseTexture::~IOpenGLBaseTexture()
  {

  }

  void IOpenGLBaseTexture::SetRenderStates()
  {
    if (_ResourceType == RTTEXTURE)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_2D, _OpenGLID) );
    }
    else if (_ResourceType == RTTEXTURERECTANGLE)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_RECTANGLE_ARB, _OpenGLID) );
    }
    else if (_ResourceType == RTCUBETEXTURE)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_CUBE_MAP, _OpenGLID) );
    }
    else if (_ResourceType == RTVOLUMETEXTURE)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_3D, _OpenGLID) );
    }
    else if (_ResourceType == RTANIMATEDTEXTURE)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_RECTANGLE_ARB, _OpenGLID) );
    }
    else
    {
      nuxError (TEXT ("[IOpenGLBaseTexture::SetRenderStates] Incorrect texture resource type.") );
    }

    _TextureStates.SetRenderStates();
  }

  void IOpenGLBaseTexture::SetFiltering (t_u32 MinFilter, t_u32 MagFilter/*, t_u32 MIP = GL_NEAREST*/)
  {
    _TextureStates.SetFiltering (MinFilter, MagFilter);
  }

  void IOpenGLBaseTexture::SetWrap (t_u32 U, t_u32 V, t_u32 W)
  {
    _TextureStates.SetWrap (U, V, W);
  }

  void IOpenGLBaseTexture::SetLOD (float MinLod, float MaxLod)
  {
    _TextureStates.SetLOD (MinLod, MaxLod);
  }

  void IOpenGLBaseTexture::SetMipLevel (t_u32 MinMip, t_u32 MaxMip)
  {
    _TextureStates.SetMipLevel (MinMip, MaxMip);
  }

  void IOpenGLBaseTexture::SetBorderColor (float R, float G, float B, float A)
  {
    _TextureStates.SetBorderColor (R, G, B, A);
  }

  int IOpenGLBaseTexture::BindTexture()
  {
    if (_OpenGLID == 0)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_2D, 0) );
    }
    else if (_ResourceType == RTTEXTURE)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_2D, _OpenGLID) );
    }
    else if (_ResourceType == RTTEXTURERECTANGLE)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_RECTANGLE_ARB, _OpenGLID) );
    }
    else if (_ResourceType == RTCUBETEXTURE)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_CUBE_MAP, _OpenGLID) );
    }
    else if (_ResourceType == RTVOLUMETEXTURE)
    {
      CHECKGL ( glBindTexture (GL_TEXTURE_3D, _OpenGLID) );
    }
    else if (_ResourceType == RTANIMATEDTEXTURE)
    {
      CHECKGL (glBindTexture (GL_TEXTURE_RECTANGLE_ARB, _OpenGLID) );
    }
    else
    {
      nuxError (TEXT ("[GpuDevice::BindTexture] Unknown texture type.") );
      return OGL_INVALID_TEXTURE;
    }

    _TextureStates.SetRenderStates();
    return OGL_OK;
  }

  int IOpenGLBaseTexture::BindTextureToUnit (int TextureUnitIndex)
  {
    CHECKGL ( glActiveTextureARB (TextureUnitIndex) );
    CHECKGL ( glBindTexture (GL_TEXTURE_2D, 0) );
    CHECKGL ( glBindTexture (GL_TEXTURE_3D, 0) );
    CHECKGL ( glBindTexture (GL_TEXTURE_CUBE_MAP, 0) );
    CHECKGL ( glBindTexture (GL_TEXTURE_RECTANGLE_ARB, 0) );
    CHECKGL ( glDisable (GL_TEXTURE_2D) );
    CHECKGL ( glDisable (GL_TEXTURE_3D) );
    CHECKGL ( glDisable (GL_TEXTURE_RECTANGLE_ARB) );
    CHECKGL ( glDisable (GL_TEXTURE_CUBE_MAP) );

    if (_ResourceType == RTTEXTURE)
    {
      CHECKGL (glBindTexture (GL_TEXTURE_2D, _OpenGLID) );
      CHECKGL (glEnable (GL_TEXTURE_2D) );
    }
    else if (_ResourceType == RTTEXTURERECTANGLE)
    {
      CHECKGL (glBindTexture (GL_TEXTURE_RECTANGLE_ARB, _OpenGLID) );
      CHECKGL (glEnable (GL_TEXTURE_RECTANGLE_ARB) );
    }
    else if (_ResourceType == RTCUBETEXTURE)
    {
      CHECKGL (glBindTexture (GL_TEXTURE_CUBE_MAP, _OpenGLID) );
      CHECKGL (glEnable (GL_TEXTURE_CUBE_MAP) );
    }
    else if (_ResourceType == RTVOLUMETEXTURE)
    {
      CHECKGL (glBindTexture (GL_TEXTURE_3D, _OpenGLID) );
      CHECKGL (glEnable (GL_TEXTURE_3D) );
    }
    else if (_ResourceType == RTANIMATEDTEXTURE)
    {
      CHECKGL (glBindTexture (GL_TEXTURE_RECTANGLE_ARB, _OpenGLID) );
      CHECKGL (glEnable (GL_TEXTURE_RECTANGLE_ARB) );
    }
    else
    {
      nuxError (TEXT ("[IOpenGLBaseTexture::BindTexture] unknown texture type.") );
      return OGL_ERROR;
    }

    _TextureStates.SetRenderStates();
    return OGL_OK;
  }

  void IOpenGLBaseTexture::GetSurfaceLevel (int Level, ObjectPtr<IOpenGLSurface>& surface)
  {

  }

  ObjectPtr<IOpenGLSurface> IOpenGLBaseTexture::GetSurfaceLevel (int Level)
  {
    return ObjectPtr<IOpenGLSurface> (0);
  }

  int IOpenGLBaseTexture::LockRect (int Level, SURFACE_LOCKED_RECT *pLockedRect, const SURFACE_RECT *pRect)
  {
    return 0;
  }

  int IOpenGLBaseTexture::UnlockRect (int Level)
  {
    return 0;
  }

  t_s32 GetTextureSize (IOpenGLBaseTexture *pTexture)
  {
    GLint unpack_alignment = GPixelFormats[pTexture->_PixelFormat].RowMemoryAlignment;
    t_u32 halfUnpack = Log2 (unpack_alignment);

    t_u32 TextureSize = 0;

    if (
      pTexture->_PixelFormat == BITFMT_DXT1 ||
      pTexture->_PixelFormat == BITFMT_DXT2 ||
      pTexture->_PixelFormat == BITFMT_DXT3 ||
      pTexture->_PixelFormat == BITFMT_DXT4 ||
      pTexture->_PixelFormat == BITFMT_DXT5)
    {
      for (int Level = 0; Level < pTexture->GetNumMipLevel(); Level++)
      {
        t_u32 Width            = Max<t_u32> (1, pTexture->_Width >> Level);
        t_u32 Height           = Max<t_u32> (1, pTexture->_Height >> Level);
        t_u32 WidthInBlocks    = Max<t_u32> ( (Width + 3) / 4, 1); // round the width to the upper multiple of 4. DXT blocks are 4x4 texels.
        t_u32 HeightInBlocks   = Max<t_u32> ( (Height + 3) / 4, 1); // round the height to the upper multiple of 4. DXT blocks are 4x4 texels.

        if (pTexture->_PixelFormat == BITFMT_DXT1)
        {
          TextureSize += WidthInBlocks * HeightInBlocks * 8;;
        }
        else
        {
          TextureSize += WidthInBlocks * HeightInBlocks * 16;
        }
      }
    }
    else
    {
      for (int Level = 0; Level < pTexture->GetNumMipLevel(); Level++)
      {
        t_u32 Width             = Max<t_u32> (1, pTexture->_Width >> Level);
        t_u32 Height            = Max<t_u32> (1, pTexture->_Height >> Level);
        t_u32 BytePerPixel      = GPixelFormats[pTexture->_PixelFormat].BlockBytes;
        t_u32 SlicePitch        = ( ( (Width * BytePerPixel + (unpack_alignment - 1) ) >> (halfUnpack) ) << (halfUnpack) ) * Height;

        TextureSize += SlicePitch;
      }
    }

    if (pTexture->GetResourceType() == RTANIMATEDTEXTURE)
    {
      return TextureSize * pTexture->GetDepth();
    }

    return TextureSize;
  }

  void GetTextureDesc (IOpenGLBaseTexture *pTexture,
                       t_u32 Level,
                       TEXTURE_DESC *pDesc)
  {
    GLint unpack_alignment = GPixelFormats[pTexture->_PixelFormat].RowMemoryAlignment;
    t_u32 halfUnpack = Log2 (unpack_alignment);

    t_u32 BytePerPixel = 0;

    pDesc->PixelFormat = pTexture->_PixelFormat;

    if ( /*pTexture->_Format  == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||*/
      pTexture->_PixelFormat == BITFMT_DXT1 ||
      pTexture->_PixelFormat == BITFMT_DXT2 ||
      pTexture->_PixelFormat == BITFMT_DXT3 ||
      pTexture->_PixelFormat == BITFMT_DXT4 ||
      pTexture->_PixelFormat == BITFMT_DXT5)
    {
      pDesc->Width            = Max<t_u32> (1, pTexture->_Width >> Level);
      pDesc->Height           = Max<t_u32> (1, pTexture->_Height >> Level);
      pDesc->WidthInBlocks    = Max<t_u32> ( (pDesc->Width + 3) / 4, 1); // round the width to the upper multiple of 4. DXT blocks are 4x4 texels.
      pDesc->HeightInBlocks   = Max<t_u32> ( (pDesc->Height + 3) / 4, 1); // round the height to the upper multiple of 4. DXT blocks are 4x4 texels.

      if ( /*pTexture->_Format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT ||*/
        pTexture->_PixelFormat == BITFMT_DXT1)
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

        // A line of n texel DXT1 data uses n/2 bytes (4 bits/texel). So the number of bytes used for a
        // texwidth texel, is texwidth/2 bytes.
        // Note that texwidth is divisible by 4(to to the upper rounding to 4), therefore, it is also divisible
        // by 2.

        // glCompressedTexImage2DARB, glCompressedTexImage3DARB,
        // glCompressedTexSubImage2DARB, glCompressedTexSubImage3DARB are not affected by glPixelStorei.

        pDesc->RowPitch = pDesc->WidthInBlocks * 8;
        pDesc->SlicePitch = pDesc->WidthInBlocks * pDesc->HeightInBlocks * 8;
        pDesc->BitsPerPixel     = 0; // compressed
        pDesc->BytesPerBlock    = 8;
      }
      else
      {
        // A line of n texel DXT3/5 data uses n bytes (1 byte/texel). So the number of bytes used for a
        // texwidth texels, is texwidth bytes.

        // glCompressedTexImage2DARB, glCompressedTexImage3DARB,
        // glCompressedTexSubImage2DARB, glCompressedTexSubImage3DARB are not affected by glPixelStorei.
        pDesc->RowPitch = pDesc->WidthInBlocks * 16;
        pDesc->SlicePitch = pDesc->WidthInBlocks * pDesc->HeightInBlocks * 16;
        pDesc->BitsPerPixel     = 0; // compressed
        pDesc->BytesPerBlock    = 16;
      }
    }
    else
    {
      pDesc->Width            = Max<t_u32> (1, pTexture->_Width >> Level);
      pDesc->Height           = Max<t_u32> (1, pTexture->_Height >> Level);
      pDesc->WidthInBlocks    = pDesc->Width;
      pDesc->HeightInBlocks   = pDesc->Height;
      BytePerPixel            = GPixelFormats[pTexture->_PixelFormat].BlockBytes;
      pDesc->BitsPerPixel     = BytePerPixel * 8;
      pDesc->BytesPerBlock    = BytePerPixel; // a block is a 1x1 pixel ie. 1 pixel.
      pDesc->RowPitch = ( ( (pDesc->Width * BytePerPixel + (unpack_alignment - 1) ) >> (halfUnpack) ) << (halfUnpack) );
      pDesc->SlicePitch = ( ( (pDesc->Width * BytePerPixel + (unpack_alignment - 1) ) >> (halfUnpack) ) << (halfUnpack) ) * pDesc->Height;
    }
  }

}
