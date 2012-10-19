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
#include "FileName.h"
#include "Math/MathUtility.h"

namespace nux
{

  std::string NFileName::GetDrive() const
  {
    size_t Pos = find('\\', 0);
    return Pos != std::string::npos ? substr(0, Pos) : "";
  }

  std::string NFileName::GetExtension() const
  {
    size_t Pos = rfind('.');
    return Pos != std::string::npos ? substr(Pos + 1) : "";
  }

// Returns the base filename without the path
  std::string NFileName::GetCleanFilename() const
  {
    size_t bsPos = rfind('\\');
    size_t sPos = rfind('/');
    size_t Pos = std::string::npos;
    if (bsPos != std::string::npos && sPos != std::string::npos)
    {
      Pos = Max<size_t>(bsPos, sPos);
    }
    else if (bsPos != std::string::npos)
    {
      Pos = bsPos;
    }
    else if (sPos != std::string::npos)
    {
      Pos = sPos;
    }
    return  Pos != std::string::npos ? (const std::string)substr(Pos + 1) : *this;
  }

  std::string NFileName::GetFilenameNoExtension() const
  {
    size_t Pos = rfind('.');
    if (Pos != std::string::npos && Pos != 0)
    {
      return substr(0, Pos);
    }
    return *this;
  }

// Returns the base filename without the path and without the extension
  std::string NFileName::GetBaseFilename() const
  {
    std::string Wk = GetCleanFilename();

    size_t Pos = Wk.rfind('.');

    if (Pos != std::string::npos)
    {
      return Wk.substr(Pos);
    }

    return Wk;
  }

// Returns the path in front of the filename

  std::string NFileName::GetDirectoryPath() const
  {
    size_t bsPos = rfind('\\');
    size_t sPos = rfind('/');
    size_t Pos = std::string::npos;
    if (bsPos != std::string::npos && sPos != std::string::npos)
    {
      Pos = Max<size_t>(bsPos, sPos);
    }
    else if (bsPos != std::string::npos)
    {
      Pos = bsPos;
    }
    else if (sPos != std::string::npos)
    {
      Pos = sPos;
    }
    return Pos != std::string::npos ? substr(Pos) : ".";
  }

  void NFileName::ChangeFileExtension (const TCHAR *ext)
  {
    size_t Pos = rfind('.');
    if (Pos != std::string::npos)
    {
      replace(begin() + Pos, end(), ext);
    }
    *this = *this + "." + ext;
  }

  void NFileName::ConvertSlashToBackslash()
  {
    size_t Pos = find('/');

    while (Pos != std::string::npos)
    {
      replace(Pos, 1, 1, '\\');
    }
  }

  void NFileName::ConvertBackslashToSlash()
  {
    size_t Pos = find('\\');

    while (Pos != std::string::npos)
    {
      replace(Pos, 1, 1, '/');
    }
  }

  void NFileName::AddSlashAtEnd()
  {
    if (!empty() && operator[](size() - 1) != '/')
      append("/");
  }

  void NFileName::AddBackSlashAtEnd()
  {
    if (!empty() && operator[](size() - 1) != '\\')
      *this += '\\';
  }

  void NFileName::AddSlashAtStart()
  {
    if (!empty() && operator[](0) != '/')
      insert(0, 1, '/');
  }

  void NFileName::AddBackSlashAtStart()
  {
    if (!empty() && operator[](0) != '\\')
      insert(0, 1, '\\');
  }

  void NFileName::RemoveSlashAtEnd()
  {
    if (!empty() && operator[](size() - 1) == '/')
    {
      erase(end() - 1);
    }
  }

  void NFileName::RemoveBackSlashAtEnd()
  {
    if (!empty() && operator[](size() - 1) == '\\')
    {
      erase(end() - 1);
    }
  }

  void NFileName::RemoveSlashAtStart()
  {
    if (!empty() && operator[](0) == '/')
    {
      erase(0, 1);
    }
  }

  void NFileName::RemoveBackSlashAtStart()
  {
    if (!empty() && operator[](0) == '\\')
    {
      erase(0, 1);
    }
  }

  void NFileName::ConvertToCleanSlash()
  {
    ConvertBackslashToSlash();

    size_t count = size();

    for (size_t i = 0; i < count; )
    {
      if ( (i < count - 1) && (operator[] (i) == '/' ) && (operator[] (i + 1) == ('/') ) )
      {
        erase (i + 1, 1);
        --count;
      }
      else
      {
        i++;
      }
    }
  }

  void NFileName::ConvertToCleanBackslash()
  {
    ConvertSlashToBackslash();

    size_t count = size();

    for (size_t i = 0; i < count; )
    {
      if ( (i < count - 1) && (operator[] (i) == '\\' ) && (operator[] (i + 1) == '\\' ) )
      {
        erase (i + 1, 1);
      }
      else
      {
        i++;
      }
    }
  }

}
