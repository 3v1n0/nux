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
#include "NPluginInterface.h"
#include "NPluginManager.h"


namespace nux
{

  void NPluginManager::GetPluginList (TCHAR *dirPath, bool addToList)
  {
    if (!addToList) ClearPluginList();

    WIN32_FIND_DATA fd;
    TCHAR fname[MAX_PATH];
    STRCPY_S (fname, MAX_PATH, dirPath);
    size_t len = _tcslen (fname);

    if (fname[len-1] == TEXT ('/') || fname[len-1] == TEXT ('\\') )
      STRCAT_S (fname, MAX_PATH, TEXT ("*.dll") );
    else
      STRCAT_S (fname, MAX_PATH, TEXT ("\\*.dll") );

    HANDLE hFind = FindFirstFile (fname, &fd);

    NFileName Path = dirPath;
    Path.RemoveBackSlashAtEnd();
    Path.RemoveSlashAtEnd();

    if (hFind == INVALID_HANDLE_VALUE)
    {
      FindClose (hFind);
      return;
    }

    do
    {
      HINSTANCE dllHandle = NULL;

      try
      {
        if (! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
        {
          NFileName FilePath = Path + NUX_PATH_SEPARATOR_STRING + fd.cFileName;
          dllHandle = LoadLibrary (FilePath.c_str() );

          if (dllHandle != NULL)
          {
            PLUGIN_FACTORYFUNC funcHandle;
            funcHandle = reinterpret_cast<PLUGIN_FACTORYFUNC> (GetProcAddress (dllHandle, "makePlugin") );

            if (funcHandle != NULL)
            {
              NPlugin *curPlugin = new NPlugin();
              curPlugin->SetFileName (FilePath.c_str() );

              PLUGIN_TEXTFUNC textFunc;
              textFunc = reinterpret_cast<PLUGIN_TEXTFUNC> (GetProcAddress (dllHandle, "getPluginType") );
              curPlugin->SetType (textFunc() );
              textFunc = reinterpret_cast<PLUGIN_TEXTFUNC> (GetProcAddress (dllHandle, "getPluginName") );
              curPlugin->SetName (textFunc() );

              pluginRegister.push_back (curPlugin);
            }

            FreeLibrary (dllHandle);
          }
        }
      }
      catch (...)
      {
        if (dllHandle != NULL) FreeLibrary (dllHandle);
      }
    }
    while (FindNextFile (hFind, &fd) );

    FindClose (hFind);
  }

}
