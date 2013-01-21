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
#include "FontTexture.h"
#include "NuxGraphics.h"
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
    inlRegisterThreadLocalIndex(1, _TLS_GraphicsDisplay, NULL);

#if defined(NUX_OS_WINDOWS)

#endif

#if defined(NUX_OS_LINUX) && G_ENCODE_VERSION (GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION) <= GLIB_VERSION_2_34
    // Necessary in order to use gdk
    g_type_init();
#endif

    GNuxGraphicsResources.InitializeResources();
  }

  GraphicsDisplay* GetGraphicsDisplay()
  {
    return(GraphicsDisplay *) inlGetThreadLocalStorage(_TLS_GraphicsDisplay);
  }
}
