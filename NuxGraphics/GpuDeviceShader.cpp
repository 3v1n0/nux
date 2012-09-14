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
	ObjectPtr<IOpenGLShaderProgram> GpuDevice::CreateShaderProgram()
	{
		IOpenGLShaderProgram *sprog = new IOpenGLShaderProgram();
		ObjectPtr<IOpenGLShaderProgram> ptr;
		ptr.Adopt(sprog);

		return ptr;
	}

	ObjectPtr<IOpenGLVertexShader> GpuDevice::CreateVertexShader()
	{
		IOpenGLVertexShader *vsdr = new IOpenGLVertexShader();
		ObjectPtr<IOpenGLVertexShader> ptr;
		ptr.Adopt(vsdr);

		return ptr;
	}

	ObjectPtr<IOpenGLPixelShader> GpuDevice::CreatePixelShader()
	{
		IOpenGLPixelShader *fsdr = new IOpenGLPixelShader();
		ObjectPtr<IOpenGLPixelShader> ptr;
		ptr.Adopt(fsdr);

		return ptr;
	}

	ObjectPtr<IOpenGLAsmShaderProgram> GpuDevice::CreateAsmShaderProgram()
	{
		IOpenGLAsmShaderProgram *asprog = new IOpenGLAsmShaderProgram();
		ObjectPtr<IOpenGLAsmShaderProgram> ptr;
		ptr.Adopt(asprog);

		return ptr;
	}

	ObjectPtr<IOpenGLAsmVertexShader> GpuDevice::CreateAsmVertexShader()
	{
		IOpenGLAsmVertexShader *avsdr = new IOpenGLAsmVertexShader();
		ObjectPtr<IOpenGLAsmVertexShader> ptr;
		ptr.Adopt(avsdr);

		return ptr;
	}

	ObjectPtr<IOpenGLAsmPixelShader> GpuDevice::CreateAsmPixelShader()
	{
		IOpenGLAsmPixelShader* afsdr = new IOpenGLAsmPixelShader();
		ObjectPtr<IOpenGLAsmPixelShader> ptr;
		ptr.Adopt(afsdr);

		return ptr;
	}

#if (NUX_ENABLE_CG_SHADERS)

	ObjectPtr<ICgVertexShader> GpuDevice::CreateCGVertexShader()
	{
		ICgVertexShader* cvsdr = new ICgVertexShader();
		ObjectPtr<ICgVertexShader> ptr;
		ptr.Adopt(cvsdr);

		return ptr;
	}

	ObjectPtr<ICgPixelShader> GpuDevice::CreateCGPixelShader()
	{
		ICgPixelShader *cfsdr = new ICgPixelShader();
		ObjectPtr<ICgPixelShader> ptr;
		ptr.Adopt(cfsdr);

		return ptr;
	}

#endif
}
