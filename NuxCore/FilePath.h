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

NAMESPACE_BEGIN

class FilePath
{
public:
    FilePath();
    ~FilePath();

    void AddSearchPath(const NString& searchpath);
    void AddSearchPath(const std::vector<NString>& searchpath);

    NString GetPathToFile(const TCHAR* filename) const;
    NString GetFile(const TCHAR* filename) const;
	
private:
    std::vector<NString>          m_SearchPath;
};

NAMESPACE_END

#endif // FILEPATH_H
