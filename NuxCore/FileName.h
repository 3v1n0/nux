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


#ifndef NFILENAME_H
#define NFILENAME_H

#include "NString.h"

namespace nux
{

  class NFileName : public NString
  {
  public:
    NFileName()
      : NString()
    {}
    NFileName ( const NFileName &Filename )
      : NString ( Filename.GetTCharPtr() )
    {}

    NFileName ( const NString &str )
      : NString ( str )
    {}

    NFileName ( const ANSICHAR *str )
      : NString ( str )
    {}
    NFileName ( const UNICHAR *str )
      : NString ( str )
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

    NString GetDrive() const;
    //! Returns the text following the last period.
    NString GetExtension() const;
    //! Returns the base filename, minus any path information.
    NString GetCleanFilename() const;
    //! Returns the base filename, without the extension (keep the path)
    NString GetFilenameNoExtension() const;

    //! Returns the same thing as GetCleanFilename, but without the extension
    NString GetBaseFilename() const;
    //! Returns the path in front of the filename
    NString GetDirectoryPath() const;
    //! Change the file extension. Do not start ext with a dot character '.'.
    //! ie ext = "com"
    void ChangeFileExtension (const TCHAR *ext);
  };

}

#endif // NFILENAME_H
