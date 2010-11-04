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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
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

#define MANAGEDEVICERESOURCE    0

namespace nux
{
  IntrusiveSP<IOpenGLShaderProgram> GpuDevice::CreateShaderProgram()
  {
    IOpenGLShaderProgram *ptr;
    CreateShaderProgram ( (IOpenGLShaderProgram **) &ptr);
    IntrusiveSP<IOpenGLShaderProgram> h = IntrusiveSP<IOpenGLShaderProgram> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateShaderProgram (IOpenGLShaderProgram **ppShaderProgram)
  {
    *ppShaderProgram = new IOpenGLShaderProgram();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< IntrusiveSP<IOpenGLShaderProgram> > (IntrusiveSP<IOpenGLShaderProgram> (*ppShaderProgram), &_CachedShaderProgramList);

    return OGL_OK;
  }

  IntrusiveSP<IOpenGLVertexShader> GpuDevice::CreateVertexShader()
  {
    IOpenGLVertexShader *ptr;
    CreateVertexShader ( (IOpenGLVertexShader **) &ptr);
    IntrusiveSP<IOpenGLVertexShader> h = IntrusiveSP<IOpenGLVertexShader> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateVertexShader (IOpenGLVertexShader **ppVertexShader)
  {
    *ppVertexShader = new IOpenGLVertexShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< IntrusiveSP<IOpenGLVertexShader> > (IntrusiveSP<IOpenGLVertexShader> (*ppVertexShader), &_CachedVertexShaderList);

    return OGL_OK;
  }

  IntrusiveSP<IOpenGLPixelShader> GpuDevice::CreatePixelShader()
  {
    IOpenGLPixelShader *ptr;
    CreatePixelShader ( (IOpenGLPixelShader **) &ptr);
    IntrusiveSP<IOpenGLPixelShader> h = IntrusiveSP<IOpenGLPixelShader> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreatePixelShader (IOpenGLPixelShader **ppPixelShader)
  {
    *ppPixelShader = new IOpenGLPixelShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< IntrusiveSP<IOpenGLPixelShader> > (IntrusiveSP<IOpenGLPixelShader> (*ppPixelShader), &_CachedPixelShaderList);

    return OGL_OK;
  }

  IntrusiveSP<IOpenGLAsmShaderProgram> GpuDevice::CreateAsmShaderProgram()
  {
    IOpenGLAsmShaderProgram *ptr;
    CreateAsmShaderProgram ( (IOpenGLAsmShaderProgram **) &ptr);
    IntrusiveSP<IOpenGLAsmShaderProgram> h = IntrusiveSP<IOpenGLAsmShaderProgram> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateAsmShaderProgram (IOpenGLAsmShaderProgram **ppAsmShaderProgram)
  {
    *ppAsmShaderProgram = new IOpenGLAsmShaderProgram();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< IntrusiveSP<IOpenGLAsmShaderProgram> > (IntrusiveSP<IOpenGLAsmShaderProgram> (*ppAsmShaderProgram), &_CachedAsmShaderProgramList);

    return OGL_OK;
  }

  IntrusiveSP<IOpenGLAsmVertexShader> GpuDevice::CreateAsmVertexShader()
  {
    IOpenGLAsmVertexShader *ptr;
    CreateAsmVertexShader ( (IOpenGLAsmVertexShader **) &ptr);
    IntrusiveSP<IOpenGLAsmVertexShader> h = IntrusiveSP<IOpenGLAsmVertexShader> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateAsmVertexShader (IOpenGLAsmVertexShader **ppAsmVertexShader)
  {
    *ppAsmVertexShader = new IOpenGLAsmVertexShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< IntrusiveSP<IOpenGLAsmVertexShader> > (IntrusiveSP<IOpenGLAsmVertexShader> (*ppAsmVertexShader), &_CachedAsmVertexShaderList);

    return OGL_OK;
  }

  IntrusiveSP<IOpenGLAsmPixelShader> GpuDevice::CreateAsmPixelShader()
  {
    IOpenGLAsmPixelShader *ptr;
    CreateAsmPixelShader ( (IOpenGLAsmPixelShader **) &ptr);
    IntrusiveSP<IOpenGLAsmPixelShader> h = IntrusiveSP<IOpenGLAsmPixelShader> (ptr);
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateAsmPixelShader (IOpenGLAsmPixelShader **ppAsmPixelShader)
  {
    *ppAsmPixelShader = new IOpenGLAsmPixelShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< IntrusiveSP<IOpenGLAsmPixelShader> > (IntrusiveSP<IOpenGLAsmPixelShader> (*ppAsmPixelShader), &_CachedAsmPixelShaderList);

    return OGL_OK;
  }

#if (NUX_ENABLE_CG_SHADERS)

  IntrusiveSP<ICgVertexShader> GpuDevice::CreateCGVertexShader()
  {
    ICgVertexShader *ptr;
    CreateCGVertexShader ( (ICgVertexShader **) &ptr);
    IntrusiveSP<ICgVertexShader> h = ptr;
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateCGVertexShader (ICgVertexShader **ppCgVertexShader)
  {
    *ppCgVertexShader = new ICgVertexShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< IntrusiveSP<ICgVertexShader> > (IntrusiveSP<ICgVertexShader> (*ppCgVertexShader), &_CachedCGVertexShaderList);

    return OGL_OK;
  }

  IntrusiveSP<ICgPixelShader> GpuDevice::CreateCGPixelShader()
  {
    ICgPixelShader *ptr;
    CreateCGPixelShader ( (ICgPixelShader **) &ptr);
    IntrusiveSP<ICgPixelShader> h = ptr;
    ptr->UnReference ();
    return h;
  }

  int GpuDevice::CreateCGPixelShader (ICgPixelShader **ppCgPixelShader)
  {
    *ppCgPixelShader = new ICgPixelShader();

    if (MANAGEDEVICERESOURCE) ManageDeviceResource< IntrusiveSP<ICgPixelShader> > (IntrusiveSP<ICgPixelShader> (*ppCgPixelShader), &_CachedCGPixelShaderList);

    return OGL_OK;
  }
#endif
}
