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
    IOpenGLShaderProgram *ptr;
    CreateShaderProgram((IOpenGLShaderProgram **) &ptr);
    ObjectPtr<IOpenGLShaderProgram> h = ObjectPtr<IOpenGLShaderProgram> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateShaderProgram(IOpenGLShaderProgram **ppShaderProgram)
  {
    *ppShaderProgram = new IOpenGLShaderProgram();

    return OGL_OK;
  }

  ObjectPtr<IOpenGLVertexShader> GpuDevice::CreateVertexShader()
  {
    IOpenGLVertexShader *ptr;
    CreateVertexShader((IOpenGLVertexShader **) &ptr);
    ObjectPtr<IOpenGLVertexShader> h = ObjectPtr<IOpenGLVertexShader> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateVertexShader(IOpenGLVertexShader **ppVertexShader)
  {
    *ppVertexShader = new IOpenGLVertexShader();

    return OGL_OK;
  }

  ObjectPtr<IOpenGLPixelShader> GpuDevice::CreatePixelShader()
  {
    IOpenGLPixelShader *ptr;
    CreatePixelShader((IOpenGLPixelShader **) &ptr);
    ObjectPtr<IOpenGLPixelShader> h = ObjectPtr<IOpenGLPixelShader> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreatePixelShader(IOpenGLPixelShader **ppPixelShader)
  {
    *ppPixelShader = new IOpenGLPixelShader();

    return OGL_OK;
  }

  ObjectPtr<IOpenGLAsmShaderProgram> GpuDevice::CreateAsmShaderProgram()
  {
    IOpenGLAsmShaderProgram *ptr;
    CreateAsmShaderProgram((IOpenGLAsmShaderProgram **) &ptr);
    ObjectPtr<IOpenGLAsmShaderProgram> h = ObjectPtr<IOpenGLAsmShaderProgram> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateAsmShaderProgram(IOpenGLAsmShaderProgram **ppAsmShaderProgram)
  {
    *ppAsmShaderProgram = new IOpenGLAsmShaderProgram();

    return OGL_OK;
  }

  ObjectPtr<IOpenGLAsmVertexShader> GpuDevice::CreateAsmVertexShader()
  {
    IOpenGLAsmVertexShader *ptr;
    CreateAsmVertexShader((IOpenGLAsmVertexShader **) &ptr);
    ObjectPtr<IOpenGLAsmVertexShader> h = ObjectPtr<IOpenGLAsmVertexShader> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateAsmVertexShader(IOpenGLAsmVertexShader **ppAsmVertexShader)
  {
    *ppAsmVertexShader = new IOpenGLAsmVertexShader();

    return OGL_OK;
  }

  ObjectPtr<IOpenGLAsmPixelShader> GpuDevice::CreateAsmPixelShader()
  {
    IOpenGLAsmPixelShader *ptr;
    CreateAsmPixelShader((IOpenGLAsmPixelShader **) &ptr);
    ObjectPtr<IOpenGLAsmPixelShader> h = ObjectPtr<IOpenGLAsmPixelShader> (ptr);
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateAsmPixelShader(IOpenGLAsmPixelShader **ppAsmPixelShader)
  {
    *ppAsmPixelShader = new IOpenGLAsmPixelShader();

    return OGL_OK;
  }

#if (NUX_ENABLE_CG_SHADERS)

  ObjectPtr<ICgVertexShader> GpuDevice::CreateCGVertexShader()
  {
    ICgVertexShader *ptr;
    CreateCGVertexShader((ICgVertexShader **) &ptr);
    ObjectPtr<ICgVertexShader> h = ptr;
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateCGVertexShader(ICgVertexShader **ppCgVertexShader)
  {
    *ppCgVertexShader = new ICgVertexShader();

    return OGL_OK;
  }

  ObjectPtr<ICgPixelShader> GpuDevice::CreateCGPixelShader()
  {
    ICgPixelShader *ptr;
    CreateCGPixelShader((ICgPixelShader **) &ptr);
    ObjectPtr<ICgPixelShader> h = ptr;
    ptr->UnReference();
    return h;
  }

  int GpuDevice::CreateCGPixelShader(ICgPixelShader **ppCgPixelShader)
  {
    *ppCgPixelShader = new ICgPixelShader();

    return OGL_OK;
  }
#endif
}
