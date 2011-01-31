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

#include "GLResource.h"
#include "FontTexture.h"
#include "NuxGraphics.h"
#include "IniFile.h"
#include "GLResourceManager.h"
#if defined(NUX_OS_WINDOWS)
  #include "GraphicsDisplay.h"
#elif defined(NUX_OS_LINUX)
  #include "GraphicsDisplay.h"
#endif

namespace nux
{
  void NuxGraphicsInitialize()
  {
    inlRegisterThreadLocalIndex (1, _TLS_GraphicsDisplay, NULL);
    inlRegisterThreadLocalIndex (2, _TLS_GpuDevice_, NULL);

#if defined(NUX_OS_LINUX)
    // Necessary in order to use gdk
    g_type_init();
#endif
    
    GNuxGraphicsResources.InitializeResources();
    
  }

  GraphicsDisplay *GetThreadGLWindow()
  {
    return GetDisplayDevice ();
  }

  GraphicsDisplay *GetDisplayDevice ()
  {
    return (GraphicsDisplay *) inlGetThreadLocalStorage (_TLS_GraphicsDisplay);
  }

  GpuDevice *GetThreadGLDeviceFactory () // Deprecated
  {
    return GetGpuDevice ();
  }

  GpuDevice *GetGpuDevice ()
  {
    return (GpuDevice *) inlGetThreadLocalStorage (_TLS_GpuDevice_);
  }

  GraphicsEngine *GetThreadGraphicsContext ()
  {
    GraphicsDisplay *glwindow = (GraphicsDisplay *) inlGetThreadLocalStorage (_TLS_GraphicsDisplay);
    return glwindow->GetGraphicsEngine ();
  }

}
