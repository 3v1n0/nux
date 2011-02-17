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
#include "RunTimeStats.h"
#include "NuxGraphicsResources.h"
#include "FontTexture.h"
#include "GlobalGraphicsInitializer.h"

namespace nux
{

  static NuxGraphicsGlobalSingletonInitializer *GNuxGraphicsGlobalInitializer   = 0;

  static void SystemStart()
  {
    static t_u8 StaticBuffer[sizeof (NuxGraphicsGlobalSingletonInitializer) ];
    // Placement new in our reserved buffer.
    GNuxGraphicsGlobalInitializer =  new (StaticBuffer) NuxGraphicsGlobalSingletonInitializer();

    //GLogDevice.AddOutputDevice( &NUX_GLOBAL_OBJECT_INSTANCE(NOutputLogFile) );
    //GLogDevice.AddOutputDevice( &NUX_GLOBAL_OBJECT_INSTANCE(NOutputVisualDebugConsole) );
  }

  static void SystemShutdown()
  {
    // Manually destroy initializer
    if (GNuxGraphicsGlobalInitializer)
      GNuxGraphicsGlobalInitializer->~NuxGraphicsGlobalSingletonInitializer();

    GNuxGraphicsGlobalInitializer = 0;

  }

  bool NuxGraphicsGlobalSingletonInitializer::m_NuxGraphicsGlobalObjectsReady = false;
  NuxGraphicsGlobalSingletonInitializer::NuxGraphicsGlobalSingletonInitializer()
  {
    m_NuxGraphicsGlobalObjectsReady = true;
  }

  NuxGraphicsGlobalSingletonInitializer::~NuxGraphicsGlobalSingletonInitializer()
  {
    m_NuxGraphicsGlobalObjectsReady = false;
  }

  bool NuxGraphicsGlobalSingletonInitializer::Ready()
  {
    return m_NuxGraphicsGlobalObjectsReady;
  }

  int NuxGraphicsGlobalInitializer::m_Count = 0;
  NuxGraphicsGlobalInitializer::NuxGraphicsGlobalInitializer()
  {
    if (m_Count++ == 0)
    {
      SystemStart();
    }
  }

  NuxGraphicsGlobalInitializer::~NuxGraphicsGlobalInitializer()
  {
    if (--m_Count == 0)
    {
      SystemShutdown();
    }
  }

}
