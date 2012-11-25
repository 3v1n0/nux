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


#ifndef NFILENAME_H
#define NFILENAME_H

#include "TextString.h"

namespace nux
{

  class NFileName : public std::string
  {
  public:
    NFileName()
      : std::string()
    {}
    NFileName ( const NFileName &Filename )
      : std::string ( Filename )
    {}

    NFileName ( const std::string &str )
      : std::string ( str )
    {}

    NFileName ( const ANSICHAR *str )
      : std::string ( str )
    {}

    //! Replace all slash with backslash.
    void ConvertSlashToBackslash();
    //! Replace all backslash with slash.
    void ConvertBackslashToSlash();
    //! Add slash at the end if there isn't one already.
    void AddSlashAtEnd();
    //! Add backslash at the end if there isn't one already.
    void AddBackSlashAtEnd();
    //! Add slash at the start if there isn't one already.
    void AddSlashAtStart();
    //! Add backslash at the start if there isn't one already.
    void AddBackSlashAtStart();
    //! Remove slash at the end if there is one.
    void RemoveSlashAtEnd();
    //! Remove backslash at the end if there is one.
    void RemoveBackSlashAtEnd();
    //! Remove slash at the start if there is one.
    void RemoveSlashAtStart();
    //! Remove backslash at the start if there is one.
    void RemoveBackSlashAtStart();

    //! Replace all backslash with slash. Replace multiple consecutive slash with one slash.
    void ConvertToCleanSlash();
    //! Replace all slash with backslash. Replace multiple consecutive backslash with one backslash.
    void ConvertToCleanBackslash();

    std::string GetDrive() const;
    //! Returns the text following the last period.
    std::string GetExtension() const;
    //! Returns the base filename, minus any path information.
    std::string GetCleanFilename() const;
    //! Returns the base filename, without the extension (keep the path)
    std::string GetFilenameNoExtension() const;

    //! Returns the same thing as GetCleanFilename, but without the extension
    std::string GetBaseFilename() const;
    //! Returns the path in front of the filename
    std::string GetDirectoryPath() const;
    //! Change the file extension. Do not start ext with a dot character '.'.
    //! ie ext = "com"
    void ChangeFileExtension (const TCHAR *ext);
  };

}

#endif // NFILENAME_H
