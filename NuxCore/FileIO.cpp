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

namespace nux
{
//
// Load a binary file to a dynamic array.
//
  bool LoadFileToArray (std::vector<BYTE>& Result, const TCHAR *Filename, NFileManager &FileManager )
  {
    NSerializer *Reader = FileManager.CreateFileReader ( Filename );

    if ( !Reader )
      return FALSE;

    Result.clear();

    if (Reader->GetFileSize() < 0)
    {
      Reader->Close();
      delete Reader;
      return FALSE;
    }

    Result.resize (Reader->GetFileSize() );
    Reader->Serialize (&Result[0], Result.size() );
    bool Success = Reader->Close();
    delete Reader;
    return Success;
  }

  /*!
      Load a text file the a ANSICHAR array. The Array is null terminated. Useful for loading shader files.
  */
  bool LoadTextFileToAnsiArray ( std::vector<ANSICHAR>& Result, const TCHAR *Filename, NFileManager &FileManager )
  {
    Result.clear();
    NSerializer *Reader = FileManager.CreateFileReader ( Filename );

    if ( !Reader )
      return FALSE;

    int Size = Reader->GetFileSize();

    if (Size < 0)
    {
      Reader->Close();
      delete Reader;
      return FALSE;
    }

    std::vector<BYTE> ByteArray;
    ByteArray.clear();
    ByteArray.resize (Size);
    Reader->Serialize (&ByteArray[0], Result.size() );
    bool Success = Reader->Close();
    delete Reader;

    if ( Size >= 2 && ! (Size & 1) && Memcmp (&Result[0], &NUX_UTF16_LE[1], NUX_UTF16_LE[0]) == 0) // (BYTE)ByteArray[0]==0xff && (BYTE)ByteArray[1]==0xfe )
    {
      // UTF16 - Little Endian
      int numElement = Size / sizeof (UNICHAR) + 1; // +1 for null char
      Result.resize (numElement);

      for ( int i = 0; i < numElement - 1; i++ )
        Result[i] = ConvertUnicodeCharToTCHAR ( (WORD) (ANSIUCHAR) ByteArray[i*2+2] + (WORD) (ANSIUCHAR) ByteArray[i*2+3] * 256 );

      Result[numElement] = 0;
    }
    else if ( Size >= 2 && ! (Size & 1) && Memcmp (&Result[0], &NUX_UTF16_LE[1], NUX_UTF16_LE[0]) == 0)
    {
      // UTF16 - Big Endian.
      int numElement = Size / sizeof (TCHAR) + 1; // +1 for null char
      Result.resize (numElement);

      for ( int i = 0; i < numElement - 1; i++ )
        Result[i] = ConvertUnicodeCharToTCHAR ( (WORD) (ANSIUCHAR) ByteArray[i*2+3] + (WORD) (ANSIUCHAR) ByteArray[i*2+2] * 256 );

      Result[numElement] = 0;
    }
    else
    {
      // ANSI.
      Result.clear();
      Result.resize (Size + 1); // +1 for null char

      for (int i = 0; i < Size; i++)
        Result[i] = ByteArray[i];

      Result[Size] = 0;
    }

    return Success;
  }

  /*!
      Load a text file in an ANSICHAR array. The Array is null terminated.
  */
  bool LoadTextFileToUnicodeArray ( std::vector<UNICHAR>& Result, const TCHAR *Filename, NFileManager &FileManager )
  {
    Result.clear();
    NSerializer *Reader = FileManager.CreateFileReader ( Filename );

    if ( !Reader )
      return FALSE;

    int Size = Reader->GetFileSize();

    if (Size < 0)
    {
      Reader->Close();
      delete Reader;
      return FALSE;
    }

    std::vector<BYTE> ByteArray;
    ByteArray.clear();
    ByteArray.resize (Size);
    Reader->Serialize ( &ByteArray[0], Result.size() );
    bool Success = Reader->Close();
    delete Reader;

    if ( Size >= 2 && ! (Size & 1) && Memcmp (&Result[0], &NUX_UTF16_LE[1], NUX_UTF16_LE[0]) == 0) // (BYTE)ByteArray[0]==0xff && (BYTE)ByteArray[1]==0xfe )
    {
      // UTF16 - Little Endian
      int numElement = Size + 1; // +1 for null char
      Result.resize (numElement);

      for ( int i = 0; i < numElement - 1; i++ )
        Result[i] = ( (WORD) (ANSIUCHAR) ByteArray[i*2+2] + (WORD) (ANSIUCHAR) ByteArray[i*2+3] * 256 );

      Result[numElement] = 0;
    }
    else if ( Size >= 2 && ! (Size & 1) && Memcmp (&Result[0], &NUX_UTF16_LE[1], NUX_UTF16_LE[0]) == 0)
    {
      // UTF16 - Big Endian.
      int numElement = Size + 1; // +1 for null char
      Result.resize (numElement);

      for ( int i = 0; i < numElement - 1; i++ )
        Result[i] = ConvertUnicodeCharToTCHAR ( (WORD) (ANSIUCHAR) ByteArray[i*2+3] + (WORD) (ANSIUCHAR) ByteArray[i*2+2] * 256 );

      Result[numElement] = 0;
    }
    else
    {
      // There is no way to detect that a file really contains ascii. Or is there?
      // Make sure this file is really ascii.
      /*
      However as an additional check to
      the heuristic of looking for unprintable characters, another trick is to
      check if the newline string is consistent. It should always be either "\n"
      (for UNIX-like systems), "\r" (for Mac-like systems) or "\r\n" (for
      Windows-like systems). If the file starts switching around between these, it
      probably isn't a valid ASCII file on any of the above three platforms.
      */

      BOOL isASCII = TRUE;

      for ( int i = 0; i < Size; i++ )
      {
        if (Result[i] == 0 && (i != Size - 1) )
        {
          isASCII = FALSE;
        }

        if ( (Result[i] < 0x20 || Result[i] >= 0xFF) && ( (Result[i] != 0x0A/*New Line, Line feed*/) && (Result[i] != 0x0D/*Carriage return*/) ) )
        {
          isASCII = FALSE;
        }

        // http://www.websiterepairguy.com/articles/os/crlf.html
        /*
            The carriage return is often referred to by the capital letters CR.
            On a Macintosh, every line has a CR at the end.

            Under Linux (a variant of Unix), the end of a line is indicated by
            a line feed. Every line ends with a line feed or LF.

            Calling the end of a line an LF versus a CR is not just semantics.
            These are 2 very real characters with 2 very real and very separate
            numeric representations on a computer. A CR is a 13 in the ASCII table
            of characters and an LF is a 10 in the ASCII table of characters.

            Contributing to the confusion is that fact that Microsoft Windows does
            things yet another way. Under Microsoft Windows, lines end with a combination
            of 2 characters -- a CR followed by a LF. Symbolically, this is represented
            as CRLF or carriage return, line feed.
        */
        // Todo. Check if the file mixes a combination of \n (Linux) \r (Mac) and \r\n (Windows).
        // If it does, the file is not ASCII.

        if (isASCII == FALSE)
          return FALSE;
      }

      Result.clear();
      Result.resize (Size + 1);

      for ( int i = 0; i < Size; i++ )
        Result[i] = ConvertAnsiCharToUnicodeChar (ByteArray[i]);

      Result[Size] = 0;
    }


    return Success;
  }

  /*!
      Load a text file to an std::string. The file maybe ANSI or Unicode.
      The resulting string is TCHAR.
  */
  bool LoadFileToString(std::string &Result, const TCHAR *Filename, NFileManager &FileManager)
  {
    NSerializer *Reader = FileManager.CreateFileReader (Filename);

    if ( !Reader )
    {
      nuxDebugMsg (TEXT ("[LoadFileToString] Cannot read from file: %s"), Filename);
      return false;
    }

    unsigned int Size = Reader->GetFileSize();
    std::vector<ANSICHAR> ByteArray (Size + 2);
    Reader->Serialize (&ByteArray[0], Size);
    bool Success = Reader->Close();
    delete Reader;
    ByteArray[Size+0] = 0;
    ByteArray[Size+1] = 0;
    std::vector<TCHAR> ResultArray;

    // Detect Unicode Byte Order Mark
    //      EF BB BF 	    UTF-8
    //      FF FE 	        UTF-16, little endian
    //      FE FF 	        UTF-16, big endian
    //      FF FE 00 00 	UTF-32, little endian
    //      00 00 FE FF 	UTF-32, big-endian
    // Note: Microsoft uses UTF-16, little endian byte order.

    // Little Endian UTF-16: size should be >=2, even, and the first two bytes should be 0xFF followed by 0xFE
    if ( (Size >= 2) && ! (Size & 1) && ( (BYTE) ByteArray[0] == 0xff) && ( (BYTE) ByteArray[1] == 0xfe) )
    {
      // UTF16 - Little Endian
      int numElement = Size / sizeof (TCHAR);
      ResultArray.clear();
      ResultArray.resize (numElement);

      for ( int i = 0; i < numElement - 1; i++ )
        ResultArray[i] = ConvertUnicodeCharToTCHAR ( (WORD) (ANSIUCHAR) ByteArray[i*2+2] + (WORD) (ANSIUCHAR) ByteArray[i*2+3] * 256 );

      ResultArray[numElement] = 0;
    }
    else if ( (Size >= 2) && ! (Size & 1) && ( (BYTE) ByteArray[0] == 0xfe) && ( (BYTE) ByteArray[1] == 0xff) )
    {
      // UTF16 - Big Endian.
      int numElement = Size / sizeof (TCHAR);
      ResultArray.clear();
      ResultArray.resize (numElement);

      for (int i = 0; i < numElement - 1; i++)
        ResultArray[i] = ConvertUnicodeCharToTCHAR ( (WORD) (ANSIUCHAR) ByteArray[i*2+3] + (WORD) (ANSIUCHAR) ByteArray[i*2+2] * 256 );

      ResultArray[numElement] = 0;
    }
    else
    {
      // ANSI.
      ResultArray.clear();
      ResultArray.resize (Size + 1);

      for (unsigned int i = 0; i < Size; i++)
        ResultArray[i] = ConvertAnsiCharToTCHAR (ByteArray[i]);

      ResultArray[Size] = 0;
    }

    Result = &ResultArray[0];
    return Success;
  }

  /*!
      Save BYTE array to file.
  */
  bool SaveArrayToFile ( const std::vector<BYTE>& Array, const TCHAR *Filename, NFileManager &FileManager )
  {
    NSerializer *Ar = FileManager.CreateFileWriter ( Filename );

    if ( !Ar )
      return 0;

    Ar->Serialize ( const_cast<BYTE *> (&Array[0]), Array.size() );
    delete Ar;
    return 1;
  }

  /*!
      Save string to File. Attempt to write it as ASCII if possible. If not write as UTF16-BE.
  */
  bool SaveStringToFile(const std::string &String, const TCHAR *Filename, NFileManager &FileManager)
  {
    if ( !String.length() )
      return 0;

    NSerializer *Ar = FileManager.CreateFileWriter ( Filename );

    if ( !Ar )
      return 0;

    bool SaveAsUnicode = false, Success = true;
#if UNICODE

    for ( int i = 0; i < String.length(); i++ )
    {
      // Test if the UNICODE 0xABCD is the same as the ASCII 0x00CB.
      if ( (*String) [i] != (TCHAR) (ANSIUCHAR) ConvertTCHARToAnsiChar ( (*String) [i]) )
      {
        //The string need to be written in ASCII. We write the string as UTF16-BigEndian
        Ar->Serialize (NUX_CONST_CAST (BYTE *, &UTF16_BE[1]), UTF16_BE[0] /*size*/);
        SaveAsUnicode = true;
        break;
      }
    }

#endif

    if ( SaveAsUnicode || (sizeof (TCHAR) == 1) )
    {
      unsigned int s = (unsigned int) String.length() * sizeof (TCHAR);
      Ar->Serialize(NUX_CONST_CAST(TCHAR *, String.c_str()), (unsigned int) s);
    }
    else
    {
      unsigned int s = (unsigned int) String.length();
      std::vector<ANSICHAR> AnsiBuffer ( (unsigned int) s);

      // Cast all character down from UTF16 to ANSI
      for (unsigned int i = 0; i < (unsigned int) String.length(); i++)
        AnsiBuffer[i] = ConvertTCHARToAnsiChar ( (unsigned int) String[i]);

      // serialize
      s = (unsigned int) String.length();
      Ar->Serialize ( NUX_CONST_CAST (ANSICHAR *, &AnsiBuffer[0]), s);
    }

    delete Ar;

    if ( !Success )
      GFileManager.Delete ( Filename );

    return Success;
  }

}
