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


#ifndef NFILE_H
#define NFILE_H

#include <vector>
#include <string>

namespace nux
{

  bool LoadFileToArray ( std::vector<BYTE>& Result, const TCHAR *Filename, NFileManager &FileManager = GFileManager );
  bool LoadTextFileToAnsiArray ( std::vector<ANSICHAR>& Result, const TCHAR *Filename, NFileManager &FileManager = GFileManager );
  bool LoadTextFileToUnicodeArray ( std::vector<UNICHAR>& Result, const TCHAR *Filename, NFileManager &FileManager = GFileManager );
  bool LoadFileToString(std::string &Result, const TCHAR *Filename, NFileManager &FileManager = GFileManager );
  bool SaveArrayToFile(const std::vector<BYTE>& Array, const TCHAR *Filename, NFileManager &FileManager = GFileManager );
  bool SaveStringToFile(const std::string &String, const TCHAR *Filename, NFileManager &FileManager = GFileManager );
}


#endif // NFILE_H

