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



//////////////////////////////////////////////////////////////////////////
// This file comes from NVidia SDK 9.5 data_path.h                      //
//////////////////////////////////////////////////////////////////////////

#ifndef FILEPATH_H
#define FILEPATH_H

#ifdef _WIN32
#  pragma warning(disable:4786)   // symbol size limitation ... STL
#endif

#include <stdio.h>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

namespace nux
{

  class FilePath
  {
  public:
    FilePath();
    ~FilePath();

    void AddSearchPath (const std::string &searchpath);
    void AddSearchPath (const std::vector<std::string>& searchpath);

    std::string GetPathToFile (const TCHAR *filename) const;
    std::string GetFile (const TCHAR *filename) const;

  private:
    std::vector<std::string>          m_SearchPath;
  };

}

#endif // FILEPATH_H
