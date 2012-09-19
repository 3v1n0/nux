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
    , BitmapFormat PixelFormat
    , NUX_FILE_LINE_DECL)
  {
    IOpenGLTexture2D *ptr;
    CreateTexture(Width, Height, Levels, PixelFormat, (IOpenGLTexture2D **) &ptr, NUX_FILE_LINE_PARAM);
    ObjectPtr<IOpenGLTexture2D> h = ObjectPtr<IOpenGLTexture2D> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateTexture(
    unsigned int Width
    , unsigned int Height
    , unsigned int Levels
    , BitmapFormat PixelFormat
    , IOpenGLTexture2D **ppTexture
    , NUX_FILE_LINE_DECL
    )
  {
    if ((Width <= 0) || (Height <= 0))
    {
      *ppTexture = NULL;
      return 0;
    }

    // From : http://oss.sgi.com/projects/ogl-sample/registry/ARB/texture_non_power_of_two.txt
    //    The "floor" convention has a relatively straightforward way to
    //        evaluate(with integer math) means to determine how many mipmap
    //        levels are required for a complete pyramid:
    //    numLevels = 1 + floor(log2(max(w, h, d)))
    unsigned int NumTotalMipLevel    = 1 + floorf(Log2(Max(Width, Height)));

    //    Levels
    //        [in] Number of levels in the texture. If this is zero, generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mip-maps textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    unsigned int NumMipLevel = 0;

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

    *ppTexture = new IOpenGLTexture2D(Width, Height, NumMipLevel, PixelFormat, false, NUX_FILE_LINE_PARAM);

    return 1;
  }

  ObjectPtr<IOpenGLTexture2D> GpuDevice::CreateTexture2DFromID(int id
    , int width
    , int height
    , int levels
    , BitmapFormat pixel_format
    , NUX_FILE_LINE_DECL)
  {
    IOpenGLTexture2D *ptr;
    ptr = new IOpenGLTexture2D(width, height, levels, pixel_format, true, NUX_FILE_LINE_PARAM); // ref count = 1;
    ptr->_OpenGLID = id;
    ObjectPtr<IOpenGLTexture2D> h = ObjectPtr<IOpenGLTexture2D> (ptr); // ref count = 2
    ptr->UnReference(); // ref count = 1
    return h;
  }

  ObjectPtr<IOpenGLRectangleTexture> GpuDevice::CreateRectangleTexture(
    int Width
    , int Height
    , int Levels
    , BitmapFormat PixelFormat
    , NUX_FILE_LINE_DECL)
  {
    IOpenGLRectangleTexture *ptr;
    CreateRectangleTexture(Width, Height, Levels, PixelFormat, (IOpenGLRectangleTexture **) &ptr, NUX_FILE_LINE_PARAM);
    ObjectPtr<IOpenGLRectangleTexture> h = ObjectPtr<IOpenGLRectangleTexture> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateRectangleTexture(
    unsigned int Width
    , unsigned int Height
    , unsigned int Levels
    , BitmapFormat PixelFormat
    , IOpenGLRectangleTexture **ppTexture
    , NUX_FILE_LINE_DECL
    )
  {
    if ((Width <= 0) || (Height <= 0))
    {
      *ppTexture = NULL;
      return 0;
    }

    // From : http://oss.sgi.com/projects/ogl-sample/registry/ARB/texture_non_power_of_two.txt
    //    The "floor" convention has a relatively straightforward way to
    //        evaluate(with integer math) means to determine how many mipmap
    //        levels are required for a complete pyramid:
    //    numLevels = 1 + floor(log2(max(w, h, d)))
    unsigned int NumTotalMipLevel    = 1 + floorf(Log2(Max(Width, Height)));

    //    Levels
    //        [in] Number of levels in the texture. If this is zero, generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mip-maps textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    unsigned int NumMipLevel = 0;

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

    *ppTexture = new IOpenGLRectangleTexture(Width, Height, NumMipLevel, PixelFormat, false, NUX_FILE_LINE_PARAM);


    return 1;
  }

  ObjectPtr<IOpenGLCubeTexture> GpuDevice::CreateCubeTexture(
    int EdgeLength
    , int Levels
    , BitmapFormat PixelFormat
    , NUX_FILE_LINE_DECL)
  {
    IOpenGLCubeTexture *ptr;
    CreateCubeTexture(EdgeLength, Levels, PixelFormat, (IOpenGLCubeTexture **) &ptr);
    ObjectPtr<IOpenGLCubeTexture> h = ObjectPtr<IOpenGLCubeTexture> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateCubeTexture(
    unsigned int EdgeLength
    , unsigned int Levels
    , BitmapFormat PixelFormat
    , IOpenGLCubeTexture **ppCubeTexture
    , NUX_FILE_LINE_DECL
    )
  {
    unsigned int NumTotalMipLevel    = 1 + floorf(Log2(EdgeLength));
    //    Levels
    //        [in] Number of levels in the texture. If this is zero, Direct3D will generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mipmapped textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    unsigned int NumMipLevel = 0;

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
    , BitmapFormat PixelFormat
    , NUX_FILE_LINE_DECL)
  {
    IOpenGLVolumeTexture *ptr;
    CreateVolumeTexture(Width, Height, Depth, Levels, PixelFormat, (IOpenGLVolumeTexture **) &ptr, NUX_FILE_LINE_PARAM);
    ObjectPtr<IOpenGLVolumeTexture> h = ObjectPtr<IOpenGLVolumeTexture> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateVolumeTexture(
    unsigned int Width
    , unsigned int Height
    , unsigned int Depth
    , unsigned int Levels
    , BitmapFormat PixelFormat
    , IOpenGLVolumeTexture **ppVolumeTexture
    , NUX_FILE_LINE_DECL
    )
  {
    unsigned int NumTotalMipLevel = 1 + floorf(Log2(Max(Max(Width, Height), Depth)));
    //    Levels
    //        [in] Number of levels in the texture. If this is zero, Direct3D will generate all texture sublevels
    //        down to 1 by 1 pixels for hardware that supports mipmapped textures. Call GetNumMipLevel to see the
    //        number of levels generated.
    unsigned int NumMipLevel = 0;

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

  int GpuDevice::CreateAnimatedTexture(unsigned int Width,
    unsigned int Height,
    unsigned int Depth,
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
