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


#ifndef NPLUGIN_H
#define NPLUGIN_H

#define NUX_DECLARE_PLUGIN(x) extern "C"{__declspec(dllexport) NPluginInterface * MakePlugin(){ return new x;}}
#define NUX_SET_PLUGIN_TYPE(x) extern "C"{__declspec(dllexport) TCHAR * GetPluginType(){ return x;}}
#define NUX_SET_PLUGIN_NAME(x) extern "C"{__declspec(dllexport) TCHAR * GetPluginName(){ return x;}}

namespace nux
{

  class NPluginInterface;

  typedef NPluginInterface * (*PLUGIN_FACTORYFUNC) ();
  typedef TCHAR * (*PLUGIN_TEXTFUNC) ();

  class NPlugin
  {
  public:
    NPlugin()
    {
      ClearMembers();
    }

    ~NPlugin();

    NPluginInterface *MakeNewInstance();

    void SetFileName (const TCHAR *nm);
    TCHAR *GetName()
    {
      return pluginName;
    }
    TCHAR *GetType()
    {
      return pluginType;
    }
    void SetName (const TCHAR *nm);
    void SetType (const TCHAR *nm);

  private:
    void ClearMembers()
    {
      pluginType = NULL;
      pluginName = NULL;
      filename = NULL;
      dllHandle = NULL;
    }

    TCHAR *filename;
    TCHAR *pluginType;
    TCHAR *pluginName;
    HINSTANCE dllHandle;
    PLUGIN_FACTORYFUNC funcHandle;
  };

}

#endif // NPLUGIN_H
