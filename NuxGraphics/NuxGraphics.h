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

#ifndef NUXGRAPHICS_H
#define NUXGRAPHICS_H

namespace nux
{
  class GraphicsDisplay;
  class GpuDevice;
  class GraphicsEngine;

  GraphicsDisplay *GetThreadGLWindow (); // Deprecated use GetDisplayDevice
  GraphicsDisplay *GetDisplayDevice ();
  GpuDevice *GetThreadGLDeviceFactory (); // Deprecated use GetGpuDevice
  GpuDevice *GetGpuDevice ();
  GraphicsEngine *GetThreadGraphicsContext();

  #define NUX_FINDRESOURCELOCATION(a) GNuxGraphicsResources.FindResourceLocation(a, false)()
  #define NUX_FIND_RESOURCE_LOCATION_NOFAIL(a) GNuxGraphicsResources.FindResourceLocation(a, true)()

  inlDeclareThreadLocalStorage (GraphicsDisplay *, 1, _TLS_GraphicsDisplay);
  inlDeclareThreadLocalStorage (GpuDevice *, 2, _TLS_GpuDevice_);

  void NuxGraphicsInitialize();
}


#endif // NUXGRAPHICS_H
