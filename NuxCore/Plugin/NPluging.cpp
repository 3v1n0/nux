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


#include "../NuxCore.h"
#include "NPlugin.h"

namespace nux
{

  NPlugin::~NPlugin()
  {
    delete[] filename;
    delete[] pluginType;
    delete[] pluginName;

    if (dllHandle) FreeLibrary (dllHandle);

    ClearMembers();
  }

  void NPlugin::SetFileName (const TCHAR *nm)
  {
    size_t sz = _tcslen (nm) + 1;
    filename = new TCHAR[sz];
    STRCPY_S (filename, sz, nm);
  }

  void NPlugin::SetName (const TCHAR *nm)
  {
    size_t sz = _tcslen (nm) + 1;
    pluginName = new TCHAR[sz];
    STRCPY_S (pluginName, sz, nm);
  }

  void NPlugin::SetType (const TCHAR *nm)
  {
    size_t sz = _tcslen (nm) + 1;
    pluginType = new TCHAR[sz];
    STRCPY_S (pluginType, sz, nm);
  }


  NPluginInterface *NPlugin::MakeNewInstance()
  {
    if (!dllHandle) dllHandle = LoadLibrary (filename);

    if (dllHandle != NULL)
    {
      funcHandle = reinterpret_cast<PLUGIN_FACTORYFUNC> (GetProcAddress (dllHandle, "MakePlugin") );

      if (funcHandle != NULL)
      {
        return funcHandle();
      }
    }

    return NULL;
  }

}
