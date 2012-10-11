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

#ifndef NUXGRAPHICS_H
#define NUXGRAPHICS_H

namespace nux
{
  class GraphicsDisplay;
  class GpuDevice;
  class GraphicsEngine;

  GraphicsDisplay* GetGraphicsDisplay();

  #define NUX_FINDRESOURCELOCATION(a) GNuxGraphicsResources.FindResourceLocation(a, false)
  #define NUX_FIND_RESOURCE_LOCATION_NOFAIL(a) GNuxGraphicsResources.FindResourceLocation(a, true)

  inlDeclareThreadLocalStorage(GraphicsDisplay *, 1, _TLS_GraphicsDisplay);

  void NuxGraphicsInitialize();
}


#endif // NUXGRAPHICS_H
