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

#include "FilePath.h"

namespace nux
{

  FilePath::FilePath()
  {
    if (std::find (m_SearchPath.begin(), m_SearchPath.end(), TEXT ("") ) != m_SearchPath.end() )
      return;

    m_SearchPath.push_back (TEXT ("") ); // for fully qualified path
  }

  FilePath::~FilePath()
  {

  }

  void FilePath::AddSearchPath (const std::string &searchpath)
  {
    if (std::find (m_SearchPath.begin(), m_SearchPath.end(), searchpath) != m_SearchPath.end() )
      return;

    m_SearchPath.push_back (searchpath);
  }

  void FilePath::AddSearchPath (const std::vector<std::string>& searchpath)
  {
    for (unsigned int i = 0; i < searchpath.size(); i++)
    {
      if (std::find (m_SearchPath.begin(), m_SearchPath.end(), searchpath[i]) == m_SearchPath.end() )
        m_SearchPath.push_back (searchpath[i]);
    }
  }

  std::string FilePath::GetPathToFile (const TCHAR *filename) const
  {
    std::string FileName = GetFile (filename);

    size_t loc = FileName.rfind(TEXT('\\'), 0);

    if (loc == std::string::npos)
    {
      loc = FileName.rfind(TEXT('/'), 0);
    }

    if (loc != std::string::npos)
      FileName = FileName.substr(0, loc);
    else
      FileName = std::string(TEXT ("."));

    return FileName;
  }

  std::string FilePath::GetFile (const TCHAR *filename) const
  {
    NUX_RETURN_VALUE_IF_NULL (filename, std::string (TEXT ("") ) );

    if (std::string (filename) == std::string (TEXT ("") ) )
      return std::string (TEXT ("") );

    std::string FileName = filename;

    if (GFileManager.FileExist(FileName.c_str() ) )
      return FileName;

    for (unsigned int i = 0; i < m_SearchPath.size(); i++)
    {
      if (m_SearchPath[i].size() == 0)
        continue;

      std::string FilePath;
   
      char last = m_SearchPath[i][m_SearchPath[i].size() - 1];
      if (last == TEXT('/') || last == TEXT('\\'))
        FilePath = m_SearchPath[i] + filename;
      else
        FilePath = m_SearchPath[i] + NUX_PATH_SEPARATOR_STRING + filename;


      if (GFileManager.FileExist (FilePath.c_str() ) )
        return FilePath;
    }

    // Still not found. Then peel off the root of the filename and append our custom search path.
    //      filename = "MediaProg/UI3DGraphics/MyFile.txt"
    //      search for:
    //            CustomPath0/UI3DGraphics/MyFile.txt
    //            CustomPath1/UI3DGraphics/MyFile.txt
    //            CustomPath2/UI3DGraphics/MyFile.txt
    //            CustomPath3/UI3DGraphics/MyFile.txt
    //            CustomPath0/MyFile.txt
    //            CustomPath1/MyFile.txt
    //            CustomPath2/MyFile.txt
    //            CustomPath3/MyFile.txt
    //

    FileName = filename;

    for (size_t i = 0; i < m_SearchPath.size(); i++)
    {
      size_t pos;
      std::string PathName;

      while (FileName.find_first_of(TEXT ("\\/")) != std::string::npos)
      {
        pos = FileName.find_first_of(TEXT ("\\/")) + 1;

        FileName = FileName.substr(pos, FileName.length() - pos);

        char last = m_SearchPath[i][m_SearchPath[i].size() - 1];
        if (last == TEXT('/') || last == TEXT('\\'))
          PathName = m_SearchPath[i] + FileName;
        else
          PathName = m_SearchPath[i] + NUX_PATH_SEPARATOR_STRING + FileName;


        if (GFileManager.FileExist (PathName.c_str() ) )
          return PathName;
      }
    }

    nuxDebugMsg (TEXT ("[FilePath::GetFile] Cannot find file: %s"), filename);
    return std::string (TEXT ("") );
  }

}
