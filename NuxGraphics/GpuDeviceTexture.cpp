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
			int Width,
			int Height,
			int Levels,
			BitmapFormat PixelFormat,
			NUX_FILE_LINE_DECL)
	{
		int msz = GetGpuInfo().GetMaxTextureSize();
		if(!Width || !Height || (Width > msz) || Height > msz) {
			return ObjectPtr<IOpenGLTexture2D>();
		}

		unsigned int NumTotalMipLevel = 1 + floorf(Log2(Max(Width, Height)));
		unsigned int NumMipLevel = 0;

		if(Levels == 0) {
			NumMipLevel = NumTotalMipLevel;
		}
		else if(Levels > (int)NumTotalMipLevel) {
			NumMipLevel = NumTotalMipLevel;
		}
		else {
			NumMipLevel = Levels;
		}

		IOpenGLTexture2D *tex = new IOpenGLTexture2D(Width, Height, NumMipLevel, PixelFormat, false, NUX_FILE_LINE_PARAM);
		ObjectPtr<IOpenGLTexture2D> ptr;
		ptr.Adopt(tex);

		return ptr;
	}

	ObjectPtr<IOpenGLRectangleTexture> GpuDevice::CreateRectangleTexture(
			int Width,
			int Height,
			int Levels,
			BitmapFormat PixelFormat,
			NUX_FILE_LINE_DECL)
	{
		int msz = GetGpuInfo().GetMaxTextureSize();
		if(!Width || !Height || Width > msz || Height > msz) {
			return ObjectPtr<IOpenGLRectangleTexture>();
		}

		unsigned int NumTotalMipLevel = 1 + floorf(Log2(Max(Width, Height)));
		unsigned int NumMipLevel = 0;

		 if(Levels == 0) {
			 NumMipLevel = 1;
		 }
		 else if(Levels > NumTotalMipLevel) {
			 NumMipLevel = 1;
		 }
		 else {
			 NumMipLevel = 1;
		 }

		 IOpenGLRectangleTexture *tex = new IOpenGLRectangleTexture(Width, Height, NumMipLevel, PixelFormat, false, NUX_FILE_LINE_PARAM);
		 ObjectPtr<IOpenGLRectangleTexture> ptr;
		 ptr.Adopt(tex);

		 return ptr;
	}

	ObjectPtr<IOpenGLCubeTexture> GpuDevice::CreateCubeTexture(
			int EdgeLength,
			int Levels,
			BitmapFormat PixelFormat,
			NUX_FILE_LINE_DECL) {

		unsigned int NumTotalMipLevel = 1 + floorf(Log2(EdgeLength));
		unsigned int NumMipLevel = 0;

		if(Levels == 0) {
			NumMipLevel = NumTotalMipLevel;
		}
		else if(Levels > NumTotalMipLevel) {
			NumMipLevel = NumTotalMipLevel;
		}
		else {
			NumMipLevel = Levels;
		}

		IOpenGLCubeTexture *ctex = new IOpenGLCubeTexture(EdgeLength, NumMipLevel, PixelFormat);
		ObjectPtr<IOpenGLCubeTexture> ptr;
		ptr.Adopt(ctex);

		return ptr;
	}

	ObjectPtr<IOpenGLVolumeTexture> GpuDevice::CreateVolumeTexture(
			int Width,
			int Height,
			int Depth,
			int Levels,
			BitmapFormat PixelFormat,
			NUX_FILE_LINE_DECL)
	{
		int msz = GetGpuInfo().GetMaxTextureSize();
		if(!Width || !Height || Width > msz || Height > msz) {
			return ObjectPtr<IOpenGLVolumeTexture>();
		}

		unsigned int NumTotalMipLevel = 1 + floorf(Log2(Max(Max(Width, Height), Depth)));
		unsigned int NumMipLevel = 0;

		if(Levels == 0) {
			NumMipLevel = NumTotalMipLevel;
		}
		else if(Levels > NumTotalMipLevel) {
			NumMipLevel = NumTotalMipLevel;
		}
		else {
			NumMipLevel = Levels;
		}

		IOpenGLVolumeTexture *vtex = new IOpenGLVolumeTexture(Width, Height, Depth, NumMipLevel, PixelFormat);
		ObjectPtr<IOpenGLVolumeTexture> ptr;
		ptr.Adopt(vtex);

		return ptr;
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

}
