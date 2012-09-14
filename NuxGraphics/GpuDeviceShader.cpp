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
		ObjectPtr<IOpenGLShaderProgram> ptr;
		ptr.Adopt(new IOpenGLShaderProgram());
		return ptr;
	}

	ObjectPtr<IOpenGLVertexShader> GpuDevice::CreateVertexShader()
	{
		ObjectPtr<IOpenGLVertexShader> ptr;
		ptr.Adopt(new IOpenGLVertexShader());
		return ptr;
	}

	ObjectPtr<IOpenGLPixelShader> GpuDevice::CreatePixelShader()
	{
		ObjectPtr<IOpenGLPixelShader> ptr;
		ptr.Adopt(new IOpenGLPixelShader());
		return ptr;
	}

	ObjectPtr<IOpenGLAsmShaderProgram> GpuDevice::CreateAsmShaderProgram()
	{
		ObjectPtr<IOpenGLAsmShaderProgram> ptr;
		ptr.Adopt(new IOpenGLAsmShaderProgram());
		return ptr;
	}

	ObjectPtr<IOpenGLAsmVertexShader> GpuDevice::CreateAsmVertexShader()
	{
		ObjectPtr<IOpenGLAsmVertexShader> ptr;
		ptr.Adopt(new IOpenGLAsmVertexShader());
		return ptr;
	}

	ObjectPtr<IOpenGLAsmPixelShader> GpuDevice::CreateAsmPixelShader()
	{
		ObjectPtr<IOpenGLAsmPixelShader> ptr;
		ptr.Adopt(new IOpenGLAsmPixelShader());
		return ptr;
	}

#if (NUX_ENABLE_CG_SHADERS)

	ObjectPtr<ICgVertexShader> GpuDevice::CreateCGVertexShader()
	{
		ObjectPtr<ICgVertexShader> ptr;
		ptr.Adopt(new ICgVertexShader());
		return ptr;
	}

	ObjectPtr<ICgPixelShader> GpuDevice::CreateCGPixelShader()
	{
		ObjectPtr<ICgPixelShader> ptr;
		ptr.Adopt(new ICgPixelShader());
		return ptr;
	}

#endif
}
