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
#include "GpuDevice.h"
#include "GLDeviceObjects.h"
#include "GLResourceManager.h"

#include "GLTextureResourceManager.h"
#include "GLVertexResourceManager.h"
#include "GLDeviceFrameBufferObject.h"
#include "GLTemplatePrimitiveBuffer.h"
#include "GraphicsEngine.h"

namespace nux
{
  ObjectPtr<IOpenGLTexture2D> GpuDevice::CreateTexture(
    int Width
    , int Height
    , int Levels
    , BitmapFormat pixel_format
    , NUX_FILE_LINE_DECL)
  {
    if ((pixel_format == BITFMT_D24S8) && (!GetGpuInfo().Support_Depth_Buffer()))
    {
      // No support for depth buffers
      return ObjectPtr<IOpenGLTexture2D>();
    }

    GpuInfo gpu_info = GetGpuInfo();
    int msz = gpu_info.GetMaxTextureSize();
    if(Width <= 0 || Height <= 0 || Width > msz || Height > msz)
    {
      return ObjectPtr<IOpenGLTexture2D>();
    }

    unsigned int NumTotalMipLevel = 1 + floorf(Log2(Max(Width, Height)));
    unsigned int NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else if (Levels > (int)NumTotalMipLevel)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else
    {
      NumMipLevel = Levels;
    }

    ObjectPtr<IOpenGLTexture2D> result;
    result.Adopt(new IOpenGLTexture2D(Width, Height, NumMipLevel, pixel_format, false, NUX_FILE_LINE_PARAM));
    return result;
  }

  ObjectPtr<IOpenGLTexture2D> GpuDevice::CreateTexture2DFromID(int id
    , int width
    , int height
    , int levels
    , BitmapFormat pixel_format
    , NUX_FILE_LINE_DECL)
  {
    if ((pixel_format == BITFMT_D24S8) && (!GetGpuInfo().Support_Depth_Buffer()))
    {
      // No support for depth buffers
      return ObjectPtr<IOpenGLTexture2D>();
    }

    GpuInfo gpu_info = GetGpuInfo();
    int msz = gpu_info.GetMaxTextureSize();
    if(width <= 0 || height <=0 || width > msz || height > msz)
    {
      return ObjectPtr<IOpenGLTexture2D>();
    }

    ObjectPtr<IOpenGLTexture2D> result;
    result.Adopt(new IOpenGLTexture2D(width, height, levels, pixel_format, true, NUX_FILE_LINE_PARAM));

    /* Assign the external id to the internal id. This allows us
     * to use the foreign texture as if it were a native one.
     *
     * This is really important. Don't remove it */
    result->_OpenGLID = id;
    return result;
  }

  ObjectPtr<IOpenGLRectangleTexture> GpuDevice::CreateRectangleTexture(
    int Width
    , int Height
    , int Levels
    , BitmapFormat pixel_format
    , NUX_FILE_LINE_DECL)
  {
    if ((pixel_format == BITFMT_D24S8) && (!GetGpuInfo().Support_Depth_Buffer()))
    {
      // No support for depth buffers
      return ObjectPtr<IOpenGLRectangleTexture>();
    }

    GpuInfo gpu_info = GetGpuInfo();
    int msz = gpu_info.GetMaxTextureSize();
    if(Width <= 0 || Height <= 0 || Width > msz || Height > msz)
    {
      return ObjectPtr<IOpenGLRectangleTexture>();
    }

    unsigned int NumTotalMipLevel = 1 + floorf(Log2(Max(Width, Height)));
    unsigned int NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = 1;
    }
    else if (Levels > (int)NumTotalMipLevel)
    {
      NumMipLevel = 1;
    }
    else
    {
      NumMipLevel = 1;
    }

    ObjectPtr<IOpenGLRectangleTexture> result;
    result.Adopt(new IOpenGLRectangleTexture(Width, Height, NumMipLevel, pixel_format, false, NUX_FILE_LINE_PARAM));
    return result;
  }

  ObjectPtr<IOpenGLCubeTexture> GpuDevice::CreateCubeTexture(
    int EdgeLength
    , int Levels
    , BitmapFormat pixel_format
    , const char * /* __Nux_FileName__ */
    , int /* __Nux_LineNumber__ */)
  {
    if ((pixel_format == BITFMT_D24S8) && (!GetGpuInfo().Support_Depth_Buffer()))
    {
      // No support for depth buffers
      return ObjectPtr<IOpenGLCubeTexture>();
    }

    unsigned int NumTotalMipLevel = 1 + floorf(Log2(EdgeLength));
    unsigned int NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else if (Levels > (int)NumTotalMipLevel)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else
    {
      NumMipLevel = Levels;
    }

    ObjectPtr<IOpenGLCubeTexture> result;
    result.Adopt(new IOpenGLCubeTexture(EdgeLength, NumMipLevel, pixel_format));
    return result;
  }

  ObjectPtr<IOpenGLVolumeTexture> GpuDevice::CreateVolumeTexture(
    int Width
    , int Height
    , int Depth
    , int Levels
    , BitmapFormat pixel_format
    , const char * /* __Nux_FileName__ */
    , int /* __Nux_LineNumber__ */)
  {
    if ((pixel_format == BITFMT_D24S8) && (!GetGpuInfo().Support_Depth_Buffer()))
    {
      // No support for depth buffers
      return ObjectPtr<IOpenGLVolumeTexture>();
    }

    GpuInfo gpu_info = GetGpuInfo();
    int msz = gpu_info.GetMaxTextureSize();
    if(Width <= 0 || Height <= 0 || Width > msz || Height > msz)
    {
      return ObjectPtr<IOpenGLVolumeTexture>();
    }

    unsigned int NumTotalMipLevel = 1 + floorf(Log2(Max(Max(Width, Height), Depth)));
    unsigned int NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else if (Levels > (int)NumTotalMipLevel)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else
    {
      NumMipLevel = Levels;
    }

    ObjectPtr<IOpenGLVolumeTexture> result;
    result.Adopt(new IOpenGLVolumeTexture(Width, Height, Depth, NumMipLevel, pixel_format));
    return result;
  }

  ObjectPtr<IOpenGLAnimatedTexture> GpuDevice::CreateAnimatedTexture(
    int Width
    , int Height
    , int Depth
    , BitmapFormat pixel_format)
  {
    if ((pixel_format == BITFMT_D24S8) && (!GetGpuInfo().Support_Depth_Buffer()))
    {
      // No support for depth buffers
      return ObjectPtr<IOpenGLAnimatedTexture>();
    }

    GpuInfo gpu_info = GetGpuInfo();
    int msz = gpu_info.GetMaxTextureSize();
    if(Width <= 0 || Height <= 0 || Width > msz || Height > msz)
    {
      return ObjectPtr<IOpenGLAnimatedTexture>();
    }

    ObjectPtr<IOpenGLAnimatedTexture> result;
    result.Adopt(new IOpenGLAnimatedTexture(Width, Height, Depth, pixel_format));
    return result;
  }

  ObjectPtr<IOpenGLQuery> GpuDevice::CreateQuery(QUERY_TYPE Type)
  {
    ObjectPtr<IOpenGLQuery> result;
    result.Adopt(new IOpenGLQuery(Type));
    return result;
  }
}
