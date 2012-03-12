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


#include "Nux.h"
#include "NuxGlobalInitializer.h"

namespace nux
{

  static NuxGlobalSingletonInitializer* GNuxGlobalInitializer   = 0;

  static void SystemStart()
  {
    static unsigned char StaticBuffer[sizeof(NuxGlobalSingletonInitializer) ];
    // Placement new in our reserved buffer.
    GNuxGlobalInitializer =  new(StaticBuffer) NuxGlobalSingletonInitializer();

    //GLogDevice.AddOutputDevice( &NUX_GLOBAL_OBJECT_INSTANCE(NOutputLogFile));
    //GLogDevice.AddOutputDevice( &NUX_GLOBAL_OBJECT_INSTANCE(NOutputVisualDebugConsole));
  }

  static void SystemShutdown()
  {
    // Manually destroy initializer
    if (GNuxGlobalInitializer)
      GNuxGlobalInitializer->~NuxGlobalSingletonInitializer();

    GNuxGlobalInitializer = 0;

  }

  bool NuxGlobalSingletonInitializer::m_NuxGlobalObjectsReady = false;
  NuxGlobalSingletonInitializer::NuxGlobalSingletonInitializer()
  {
    m_NuxGlobalObjectsReady = true;
  }

  NuxGlobalSingletonInitializer::~NuxGlobalSingletonInitializer()
  {
    m_NuxGlobalObjectsReady = false;
  }

  bool NuxGlobalSingletonInitializer::Ready()
  {
    return m_NuxGlobalObjectsReady;
  }

  int NuxGlobalInitializer::m_Count = 0;
  NuxGlobalInitializer::NuxGlobalInitializer()
  {
    if (m_Count++ == 0)
    {
      SystemStart();
    }
  }

  NuxGlobalInitializer::~NuxGlobalInitializer()
  {
    if (--m_Count == 0)
    {
      SystemShutdown();
    }
  }

}
