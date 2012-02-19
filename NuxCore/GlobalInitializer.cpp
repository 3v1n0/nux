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


#include "NuxCore.h"

namespace nux
{

  static GlobalSingletonInitializer      *GGlobalInitializer = 0;

  static void SystemStart()
  {
    static unsigned char StaticBuffer [sizeof (GlobalSingletonInitializer)];
    // Placement new in our reserved buffer.
    GGlobalInitializer =  new (StaticBuffer) GlobalSingletonInitializer ();

#if defined (NUX_OS_WINDOWS)
    GLogDevice.AddOutputDevice (&NUX_GLOBAL_OBJECT_INSTANCE (LogFileOutput));
    GLogDevice.AddOutputDevice (&NUX_GLOBAL_OBJECT_INSTANCE (VisualOutputConsole));
    GLogDevice.AddOutputDevice (&NUX_GLOBAL_OBJECT_INSTANCE (PrintfOutputConsole));
#else
    GLogDevice.AddOutputDevice (&NUX_GLOBAL_OBJECT_INSTANCE (LogFileOutput));
    GLogDevice.AddOutputDevice (&NUX_GLOBAL_OBJECT_INSTANCE (PrintfOutputConsole));
#endif
  }

  static void SystemShutdown()
  {
    // Manually destroy initializer
    if (GGlobalInitializer)
      GGlobalInitializer->~GlobalSingletonInitializer();

    GGlobalInitializer = 0;

  }

  bool GlobalSingletonInitializer::m_bGlobalObjectsReady = false;
  GlobalSingletonInitializer::GlobalSingletonInitializer ()
  {
    m_bGlobalObjectsReady = true;
  }

  GlobalSingletonInitializer::~GlobalSingletonInitializer()
  {
    m_bGlobalObjectsReady = false;
  }

  bool GlobalSingletonInitializer::Ready()
  {
    return m_bGlobalObjectsReady;
  }

  int GlobalInitializer::m_Count = 0;
  GlobalInitializer::GlobalInitializer()
  {
    if (m_Count++ == 0)
    {
      SystemStart();
    }
  }

  GlobalInitializer::~GlobalInitializer()
  {
    if (--m_Count == 0)
    {
      SystemShutdown();
    }
  }

void GlobalInitializer::ForceShutdown()
{
  SystemShutdown();
}

}
