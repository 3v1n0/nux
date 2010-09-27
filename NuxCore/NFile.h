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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef NFILE_H
#define NFILE_H

namespace nux { //NUX_NAMESPACE_BEGIN

bool LoadFileToArray( std::vector<BYTE>& Result, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool LoadTextFileToAnsiArray( std::vector<ANSICHAR>& Result, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool LoadTextFileToUnicodeArray( std::vector<UNICHAR>& Result, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool LoadFileToString( NString& Result, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool SaveArrayToFile( const std::vector<BYTE>& Array, const TCHAR* Filename, NFileManager& FileManager = GFileManager );
bool SaveStringToFile( const NString& String, const TCHAR* Filename, NFileManager& FileManager = GFileManager );

} //NUX_NAMESPACE_END


#endif // NFILE_H

