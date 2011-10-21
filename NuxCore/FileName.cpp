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

  NString NFileName::GetDrive() const
  {
    size_t Pos = FindFirstOccurence (NUX_BACKSLASH_CHAR);

    if (Pos == tstring::npos)
    {
      Pos = FindFirstOccurence ("\\");

      if (Pos != tstring::npos)
      {
        return GetSubString (Pos);
      }
    }

    return TEXT ("");
  }

  NString NFileName::GetExtension() const
  {
    size_t Pos = FindLastOccurence (TEXT (".") );

    if (Pos != tstring::npos)
    {
      return GetSubString (Pos + 1, Length() - Pos - 1);
    }

    return TEXT ("");
  }

// Returns the base filename without the path
  NString NFileName::GetCleanFilename() const
  {
    size_t Pos = FindLastOccurence (NUX_BACKSLASH_CHAR);
    Pos = Max<size_t> (Pos, FindLastOccurence (TEXT ("/") ) ); // in case we are using slash and the NUX_BACKSLASH_CHAR is different.
    Pos = Max<size_t> (Pos, FindLastOccurence (TEXT ("\\") ) ); // in case we are using backslash and the NUX_BACKSLASH_CHAR is different.

    if (Pos != tstring::npos)
    {
      return GetSubString (Pos + 1, Length() - Pos - 1);
    }

    return *this;
  }

  NString NFileName::GetFilenameNoExtension() const
  {
    size_t Pos = FindLastOccurence (TEXT (".") );

    if (Pos != tstring::npos)
    {
      return GetSubString (Pos);
    }

    return *this;
  }

// Returns the base filename without the path and without the extension
  NString NFileName::GetBaseFilename() const
  {
    NString Wk = GetCleanFilename();

    size_t Pos = Wk.FindLastOccurence (TEXT (".") );

    if (Pos != tstring::npos)
    {
      return Wk.GetSubString (Pos);
    }

    return Wk;
  }

// Returns the path in front of the filename

  NString NFileName::GetDirectoryPath() const
  {
    size_t Pos = FindLastOccurence (NUX_BACKSLASH_CHAR);
    Pos = Max<size_t> (Pos, FindLastOccurence (TEXT ("/") ) ); // in case we are using slash and the NUX_BACKSLASH_CHAR is different.
    Pos = Max<size_t> (Pos, FindLastOccurence (TEXT ("\\") ) ); // in case we are using backslash and the NUX_BACKSLASH_CHAR is different.

    if (Pos != tstring::npos)
    {
      return GetSubString (Pos);
    }

    return *this;
  }

  void NFileName::ChangeFileExtension (const TCHAR *ext)
  {
    size_t Pos = FindLastOccurence (TEXT (".") );

    if (Pos != tstring::npos)
    {
      (*this) = GetSubString (Pos) + NString (TEXT (".") ) + NString (ext);
    }
    else
    {
      (*this) = (*this) + NString (TEXT (".") ) + NString (ext);
    }
  }

  void NFileName::ConvertSlashToBackslash()
  {
    size_t Pos = tstring::npos;
    Pos = FindFirstOccurence (TEXT ('/') );

    while (Pos != tstring::npos)
    {
      Replace (Pos, 1, 1, TEXT ('\\') );
    }
  }


  void NFileName::ConvertBackslashToSlash()
  {
    size_t Pos = tstring::npos;
    Pos = FindFirstOccurence (TEXT ('\\') );

    while (Pos != tstring::npos)
    {
      Replace (Pos, 1, 1, TEXT ('/') );
    }
  }

  void NFileName::AddSlashAtEnd()
  {
    if (GetLastChar() != TEXT ('/') )
      *this += TEXT ('/');
  }

  void NFileName::AddBackSlashAtEnd()
  {
    if (GetLastChar() != TEXT ('\\') )
      *this += TEXT ('\\');
  }

  void NFileName::AddSlashAtStart()
  {
    if (GetFirstChar() != TEXT ('/') )
      Insert (0, 1, TEXT ('/') );
  }

  void NFileName::AddBackSlashAtStart()
  {
    if (GetFirstChar() != TEXT ('\\') )
      Insert (0, 1, TEXT ('\\') );
  }

  void NFileName::RemoveSlashAtEnd()
  {
    RemoveSuffix (TEXT ('/') );
  }

  void NFileName::RemoveBackSlashAtEnd()
  {
    RemoveSuffix (TEXT ('\\') );
  }

  void NFileName::RemoveSlashAtStart()
  {
    RemovePrefix (TEXT ('/') );
  }

  void NFileName::RemoveBackSlashAtStart()
  {
    RemovePrefix (TEXT ('\\') );
  }

  void NFileName::ConvertToCleanSlash()
  {
    ConvertBackslashToSlash();

    size_t size = Size();

    for (size_t i = 0; i < size; )
    {
      if ( (i < size - 1) && (operator[] (i) == TEXT ('/') ) && (operator[] (i + 1) == TEXT ('/') ) )
      {
        Erase (i + 1, 1);
        --size;
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

    size_t size = Size();

    for (size_t i = 0; i < size; )
    {
      if ( (i < size - 1) && (operator[] (i) == TEXT ('\\') ) && (operator[] (i + 1) == TEXT ('\\') ) )
      {
        Erase (i + 1, 1);
      }
      else
      {
        i++;
      }
    }
  }

}
