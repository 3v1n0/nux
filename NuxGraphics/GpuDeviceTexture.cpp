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
    , BitmapFormat PixelFormat)
  {
    IOpenGLTexture2D *ptr;
    CreateTexture(Width, Height, Levels, PixelFormat, (IOpenGLTexture2D **) &ptr);
    ObjectPtr<IOpenGLTexture2D> h = ObjectPtr<IOpenGLTexture2D> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateTexture(
    t_u32 Width
    , t_u32 Height
    , t_u32 Levels
    //, DWORD Usage    // no use
    , BitmapFormat PixelFormat
    , IOpenGLTexture2D **ppTexture
    //, HANDLE* pSharedHandle       // no use
    )
  {
    // From : http://oss.sgi.com/projects/ogl-sample/registry/ARB/texture_non_power_of_two.txt
    //    The "floor" convention has a relatively straightforward way to
    //        evaluate(with integer math) means to determine how many mipmap
    //        levels are required for a complete pyramid:
    //    numLevels = 1 + floor(log2(max(w, h, d)))
    t_u32 NumTotalMipLevel    = 1 + floorf(Log2(Max(Width, Height)));

    //    Levels
    //        [in] Number of levels in the texture. If this is zero, generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mip-maps textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    t_u32 NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else if (Levels > NumTotalMipLevel)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else
    {
      NumMipLevel = Levels;
    }


    //    The "floor" convention can be evaluated incrementally with the
    //        following recursion:
    //
    //    nextLODdim = max(1, currentLODdim >> 1)
    //
    //        where currentLODdim is the dimension of a level N and nextLODdim
    //        is the dimension of level N+1.  The recursion stops when level
    //        numLevels-1 is reached.

    *ppTexture = new IOpenGLTexture2D(Width, Height, NumMipLevel, PixelFormat, false, NUX_TRACKER_LOCATION);

    return 1;
  }

  ObjectPtr<IOpenGLTexture2D> GpuDevice::CreateTexture2DFromID(int id,
    int width,
    int height,
    int levels,
    BitmapFormat pixel_format)
  {
    IOpenGLTexture2D *ptr;
    ptr = new IOpenGLTexture2D(width, height, levels, pixel_format, true, NUX_TRACKER_LOCATION); // ref count = 1;
    ptr->_OpenGLID = id;
    ObjectPtr<IOpenGLTexture2D> h = ObjectPtr<IOpenGLTexture2D> (ptr); // ref count = 2
    ptr->UnReference(); // ref count = 1
    return h;
  }

  ObjectPtr<IOpenGLRectangleTexture> GpuDevice::CreateRectangleTexture(
    int Width
    , int Height
    , int Levels
    , BitmapFormat PixelFormat)
  {
    IOpenGLRectangleTexture *ptr;
    CreateRectangleTexture(Width, Height, Levels, PixelFormat, (IOpenGLRectangleTexture **) &ptr);
    ObjectPtr<IOpenGLRectangleTexture> h = ObjectPtr<IOpenGLRectangleTexture> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateRectangleTexture(
    t_u32 Width
    , t_u32 Height
    , t_u32 Levels
    //, DWORD Usage    // no use
    , BitmapFormat PixelFormat
    , IOpenGLRectangleTexture **ppTexture
    //, HANDLE* pSharedHandle       // no use
    )
  {

    // From : http://oss.sgi.com/projects/ogl-sample/registry/ARB/texture_non_power_of_two.txt
    //    The "floor" convention has a relatively straightforward way to
    //        evaluate(with integer math) means to determine how many mipmap
    //        levels are required for a complete pyramid:
    //    numLevels = 1 + floor(log2(max(w, h, d)))
    t_u32 NumTotalMipLevel    = 1 + floorf(Log2(Max(Width, Height)));

    //    Levels
    //        [in] Number of levels in the texture. If this is zero, generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mip-maps textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    t_u32 NumMipLevel = 0;

    if (Levels == 0)
    {
      //Rectangle texture texture don't support mipmaps
      NumMipLevel = 1;
    }
    else if (Levels > NumTotalMipLevel)
    {
      NumMipLevel = 1;
    }
    else
    {
      NumMipLevel = 1;
    }


    //    The "floor" convention can be evaluated incrementally with the
    //        following recursion:
    //
    //    nextLODdim = max(1, currentLODdim >> 1)
    //
    //        where currentLODdim is the dimension of a level N and nextLODdim
    //        is the dimension of level N+1.  The recursion stops when level
    //        numLevels-1 is reached.

    *ppTexture = new IOpenGLRectangleTexture(Width, Height, NumMipLevel, PixelFormat, false, NUX_TRACKER_LOCATION);


    return 1;
  }

  ObjectPtr<IOpenGLCubeTexture> GpuDevice::CreateCubeTexture(
    int EdgeLength
    , int Levels
    , BitmapFormat PixelFormat)
  {
    IOpenGLCubeTexture *ptr;
    CreateCubeTexture(EdgeLength, Levels, PixelFormat, (IOpenGLCubeTexture **) &ptr);
    ObjectPtr<IOpenGLCubeTexture> h = ObjectPtr<IOpenGLCubeTexture> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateCubeTexture(
    t_u32 EdgeLength
    , t_u32 Levels
    //, DWORD Usage    // no use
    , BitmapFormat PixelFormat
    , IOpenGLCubeTexture **ppCubeTexture
    //, HANDLE* pSharedHandle    // no use
    )
  {
    t_u32 NumTotalMipLevel    = 1 + floorf(Log2(EdgeLength));
    //    Levels
    //        [in] Number of levels in the texture. If this is zero, Direct3D will generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mipmapped textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    t_u32 NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else if (Levels > NumTotalMipLevel)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else
    {
      NumMipLevel = Levels;
    }

    *ppCubeTexture = new IOpenGLCubeTexture(EdgeLength, NumMipLevel, PixelFormat);

    return 1;
  }

  ObjectPtr<IOpenGLVolumeTexture> GpuDevice::CreateVolumeTexture(
    int Width
    , int Height
    , int Depth
    , int Levels
    , BitmapFormat PixelFormat)
  {
    IOpenGLVolumeTexture *ptr;
    CreateVolumeTexture(Width, Height, Depth, Levels, PixelFormat, (IOpenGLVolumeTexture **) &ptr);
    ObjectPtr<IOpenGLVolumeTexture> h = ObjectPtr<IOpenGLVolumeTexture> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateVolumeTexture(
    t_u32 Width
    , t_u32 Height
    , t_u32 Depth
    , t_u32 Levels
    //, DWORD Usage        // no use
    , BitmapFormat PixelFormat
    , IOpenGLVolumeTexture **ppVolumeTexture
    //, HANDLE* pSharedHandle       // no use
    )
  {
    t_u32 NumTotalMipLevel = 1 + floorf(Log2(Max(Max(Width, Height), Depth)));
    //    Levels
    //        [in] Number of levels in the texture. If this is zero, Direct3D will generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mipmapped textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    t_u32 NumMipLevel = 0;

    if (Levels == 0)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else if (Levels > NumTotalMipLevel)
    {
      NumMipLevel = NumTotalMipLevel;
    }
    else
    {
      NumMipLevel = Levels;
    }

    *ppVolumeTexture = new IOpenGLVolumeTexture(Width, Height, Depth, NumMipLevel, PixelFormat);

    return OGL_OK;
  }

  ObjectPtr<IOpenGLAnimatedTexture> GpuDevice::CreateAnimatedTexture(
    int Width
    , int Height
    , int Depth
    , BitmapFormat PixelFormat)
  {
    IOpenGLAnimatedTexture *ptr;
    CreateAnimatedTexture(Width, Height, Depth, PixelFormat, (IOpenGLAnimatedTexture **) &ptr);
    ObjectPtr<IOpenGLAnimatedTexture> h = ObjectPtr<IOpenGLAnimatedTexture> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateAnimatedTexture(t_u32 Width,
    t_u32 Height,
    t_u32 Depth,
    BitmapFormat PixelFormat,
    IOpenGLAnimatedTexture **ppAnimatedTexture)
  {
    *ppAnimatedTexture = new IOpenGLAnimatedTexture(Width, Height, Depth, PixelFormat);

    return OGL_OK;
  }

  ObjectPtr<IOpenGLQuery> GpuDevice::CreateQuery(QUERY_TYPE Type)
  {
    IOpenGLQuery *ptr;
    CreateQuery(Type, (IOpenGLQuery **) &ptr);
    ObjectPtr<IOpenGLQuery> h = ObjectPtr<IOpenGLQuery> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateQuery(QUERY_TYPE Type, IOpenGLQuery **ppQuery)
  {
    *ppQuery = new IOpenGLQuery(Type);

    return OGL_OK;
  }
}
