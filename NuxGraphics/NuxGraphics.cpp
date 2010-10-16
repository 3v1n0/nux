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
#include "FontTexture.h"
#include "NuxGraphics.h"
#include "IniFile.h"
#include "GLResourceManager.h"
#if defined(NUX_OS_WINDOWS)
  #include "Gfx_OpenGL.h"
#elif defined(NUX_OS_LINUX)
  #include "GfxSetupX11.h"
#endif

namespace nux
{
  void NuxGraphicsInitialize()
  {
    inlRegisterThreadLocalIndex (1, ThreadLocal_GLWindowImpl, NULL);
    inlRegisterThreadLocalIndex (2, ThreadLocal_GLDeviceFactory, NULL);

    GNuxGraphicsResources.InitializeResources();
  }

  GLWindowImpl *GetThreadGLWindow()
  {
    return (GLWindowImpl *) inlGetThreadLocalStorage (ThreadLocal_GLWindowImpl);
  }

  GLDeviceFactory *GetThreadGLDeviceFactory()
  {
    return (GLDeviceFactory *) inlGetThreadLocalStorage (ThreadLocal_GLDeviceFactory);
  }

  GraphicsContext *GetThreadGraphicsContext()
  {
    GLWindowImpl *glwindow = (GLWindowImpl *) inlGetThreadLocalStorage (ThreadLocal_GLWindowImpl);
    return glwindow->GetGraphicsContext();
  }

}
