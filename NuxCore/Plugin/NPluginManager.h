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


#ifndef NPLUGINMANAGER_H
#define NPLUGINMANAGER_H

#include "NPlugin.h"

namespace nux
{

  class NPluginManager
  {
  public:
    NPluginManager() {}
    ~NPluginManager()
    {
      ClearPluginList();
    }


    void GetPluginList (TCHAR *dirPath, bool addToList = false);

    NPluginInterface *MakeNewPluginInstance (int index)
    {
      return pluginRegister.at (index)->MakeNewInstance();
    }
    int GetNumPlugins()
    {
      return int (pluginRegister.size() );
    }

    TCHAR *GetPluginName (int index)
    {
      return pluginRegister.at (index)->GetName();
    }
    TCHAR *KetPluginType (int index)
    {
      return pluginRegister.at (index)->GetType();
    }

  private:
    void ClearPluginList()
    {
      for (unsigned int i = 0; i < pluginRegister.size(); i++)
      {
        delete pluginRegister.at (i);
      }

      pluginRegister.clear();
    }
    std::vector<NPlugin *> pluginRegister;
  };

}
#endif // NPLUGINMANAGER_H
