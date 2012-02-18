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


#ifndef NUNICODE_H
#define NUNICODE_H


namespace nux
{

// UTF-16 is the primary encoding mechanism used by Microsoft Windows 2000, Windows 2000 Server, Windows XP and Windows 2003 Server.
// Unicode Byte Order Mark (BOM)
  enum {UNICODE_UTF32_BE   = 0x0000FEFF };
  enum {UNICODE_UTF32_LE   = 0xFFFE0000 };
  enum {UNICODE_UTF16_BE   = 0xFEFF };
  enum {UNICODE_UTF16_LE   = 0xFFFE };
  enum {UNICODE_UTF8       = 0xEFBBBF };

  const BYTE UTF32_BE[]   = {0x04 /*size*/, 0x00, 0x00, 0xFE, 0xFF };
  const BYTE UTF32_LE[]   = {0x04 /*size*/, 0xFF, 0xFE, 0x00, 0x00 };
  const BYTE UTF16_BE[]   = {0x02 /*size*/, 0xFE, 0xFF };
  const BYTE UTF16_LE[]   = {0x02 /*size*/, 0xFF, 0xFE };
  const BYTE UTF8[]       = {0x03 /*size*/, 0xEF, 0xBB, 0xBF };

  enum {UNICODE_BOM   = 0xfeff};

// UTF-16 is the default encoding form of the Unicode Standard
// On Linux and Mac OS X, wchar_t is 4 bytes!
// On windows wchar_t is 2 bytes!

#ifdef UNICODE
  inline TCHAR ConvertAnsiCharToTCHAR (ANSICHAR In)
  {
    TCHAR output;
    const unsigned char *source_start = &In;
    const unsigned char *source_end = source_start + 1;
    wchar_t *target_start = reinterpret_cast<wchar_t *> (&output);
    wchar_t *target_end = target_start + sizeof (wchar_t);

    ConversionResult res = ConvertUTF8toUTF16 (&source_start, source_end, &target_start, target_end, lenientConversion);

    if (res != conversionOK)
    {
      output = 0;
    }

    return output;
  }

  inline ANSICHAR ConvertTCHARToAnsiChar (TCHAR In)
  {
    ANSICHAR output;
    const wchar_t *source_start = &In;
    const wchar_t *source_end = source_start + 1;
    unsigned char *target_start = reinterpret_cast<unsigned char *> (&output);
    unsigned char *target_end = target_start + sizeof (wchar_t);

    ConversionResult res = ConvertUTF16toUTF8 (&source_start, source_end, &target_start, target_end, lenientConversion);

    if (res != conversionOK)
    {
      output = 0;
    }

    return output;
  }
  inline TCHAR ConvertUnicodeCharToTCHAR (UNICHAR In)
  {
    return In;
  }
  inline UNICHAR  ConvertTCHARToUnicodeChar (TCHAR In)
  {
    return In;
  }
#else
  inline TCHAR ConvertUnicodeCharToTCHAR (UNICHAR In)
  {
    TCHAR output;
    const wchar_t *source_start = &In;
    const wchar_t *source_end = source_start + 1;
    unsigned char *target_start = reinterpret_cast<unsigned char *> (&output);
    unsigned char *target_end = target_start + sizeof (wchar_t);

    ConversionResult res = ConvertUTF16toUTF8 (&source_start, source_end, &target_start, target_end, lenientConversion);

    if (res != conversionOK)
    {
      output = 0;
    }

    return output;
  }

  inline UNICHAR ConvertTCHARToUnicodeChar (TCHAR In)
  {
    UNICHAR output;
    const unsigned char *source_start = reinterpret_cast<const unsigned char *> (&In);
    const unsigned char *source_end = source_start + 1;
    wchar_t *target_start = reinterpret_cast<wchar_t *> (&output);
    wchar_t *target_end = target_start + sizeof (wchar_t);

    ConversionResult res = ConvertUTF8toUTF16 (&source_start, source_end, &target_start, target_end, lenientConversion);

    if (res != conversionOK)
    {
      output = 0;
    }

    return output;
  }

  inline TCHAR ConvertAnsiCharToTCHAR (ANSICHAR In)
  {
    return In;
  }
  inline ANSICHAR ConvertTCHARToAnsiChar (TCHAR In)
  {
    return In;
  }
#endif

  /*!
      Convert a single UNICHAR to ANSICHAR.
  */
  inline ANSICHAR ConvertUnicodeCharToAnsiChar (UNICHAR In)
  {
    TCHAR output;
    const wchar_t *source_start = &In;
    const wchar_t *source_end = source_start + 1;
    unsigned char *target_start = reinterpret_cast<unsigned char *> (&output);
    unsigned char *target_end = target_start + sizeof (wchar_t);

    ConversionResult res = ConvertUTF16toUTF8 (&source_start, source_end, &target_start, target_end, lenientConversion);

    if (res != conversionOK)
    {
      output = 0;
    }

    return output;
  }

  /*!
      Convert a single ANSICHAR to UNICHAR.
  */
  inline UNICHAR ConvertAnsiCharToUnicodeChar (ANSICHAR In)
  {
    UNICHAR output;
    const unsigned char *source_start = reinterpret_cast<const unsigned char *> (&In);
    const unsigned char *source_end = source_start + 1;
    wchar_t *target_start = reinterpret_cast<wchar_t *> (&output);
    wchar_t *target_end = target_start + sizeof (wchar_t);

    ConversionResult res = ConvertUTF8toUTF16 (&source_start, source_end, &target_start, target_end, lenientConversion);

    if (res != conversionOK)
    {
      output = 0;
    }

    return output;
  }

  class UnicharToAnsicharConvertion
  {
  public:
    // Default to ANSI code page
    UnicharToAnsicharConvertion() {}

    /*!
    Convert from UNICHAR to ANSICHAR
    @param Source String to convert. Null terminated.
    @return Return a pointer to the new string. Null terminated.
    */
    ANSICHAR *Convert (const UNICHAR *Source);
    /*{
        std::wstring utf16string(Source);
        size_t utf16size = utf16string.length();
        size_t utf8size = 6 * utf16size;
        ANSICHAR *utf8string = new ANSICHAR[utf8size+1];

        const wchar_t *source_start = utf16string.c_str();
        const wchar_t *source_end = source_start + utf16size;
        unsigned char* target_start = reinterpret_cast<unsigned char*>(utf8string);
        unsigned char* target_end = target_start + utf8size;

        ConversionResult res = ConvertUTF16toUTF8(&source_start, source_end, &target_start, target_end, lenientConversion);
        if (res != conversionOK)
        {
            delete utf8string;
            utf8string = 0;
        }
        // mark end of string
        *target_start = 0;
        return utf8string;
    }*/
  };

  //! ANSICHAR to UNICHAR conversion
  class AnsicharToUnicharConvertion
  {
  public:
    AnsicharToUnicharConvertion() {}

    /*!
        Convert from ANSICHAR to UNICHAR
        @param Source String to convert. Null terminated.
        @return Return a pointer to the new string. Null terminated.
    */
    UNICHAR *Convert (const ANSICHAR *Source);
  };

//! TCHAR to ANSI conversion
// TCHAR can be ansi or unicode depending if UNICODE is defined or not.
  class TCharToAnsiConvertion
  {
  public:
    NUX_INLINE TCharToAnsiConvertion() {}

    /*!
    Convert from TCHAR to ANSICHAR
    @param Source String to convert. Null terminated.
    @return Return a pointer to the new string. Null terminated.
    */
    NUX_INLINE ANSICHAR *Convert (const TCHAR *Source)
    {
      // Determine whether we need to allocate memory or not
#ifdef UNICODE
      UnicharToAnsicharConvertion convert;
      return convert.Convert (Source);
#else
      size_t length = strlen (Source) + 1;
      size_t size = length * sizeof (ANSICHAR);
      ANSICHAR *Dest = new ANSICHAR[size];
      STRNCPY_S (Dest, size, Source, length);
      return Dest;
#endif
    }
  };

  //! TCHAR to Unichar conversion
  // TCHAR can be ansi or unicode depending if UNICODE is defined or not.
  class TCharToUnicharConvertion
  {
  public:
    NUX_INLINE TCharToUnicharConvertion() {}

    /*!
    Convert from TCHAR to ANSICHAR
    @param Source String to convert. Null terminated.
    @return Return a pointer to the new string. Null terminated.
    */
    NUX_INLINE UNICHAR *Convert (const TCHAR *Source)
    {
      // Determine whether we need to allocate memory or not
#ifdef UNICODE
      size_t length = strlen (Source) + 1;
      size_t size = length * sizeof (UNICHAR);
      UNICHAR *Dest = new UNICHAR[size];
      STRNCPY_S (Dest, size, Source, length);
      return Dest;
#else
      AnsicharToUnicharConvertion convert;
      return convert.Convert (Source);
#endif
    }
  };

//! ANSI to TCHAR conversion
// TCHAR can be ansi or unicode depending if UNICODE is defined or not.
  class AnsiToTCharConversion
  {
  public:
    NUX_INLINE AnsiToTCharConversion() {}

    /*!
    Convert from ANSICHAR to TCHAR
    @param Source String to convert. Null terminated.
    @return Return a pointer to the new string. Null terminated.
    */
    NUX_INLINE TCHAR *Convert (const ANSICHAR *Source)
    {
#ifdef UNICODE
      AnsicharToUnicharConvertion convert;
      return convert.Convert (Source);
#else
      size_t length = strlen (Source) + 1;
      size_t size = length;
      TCHAR *Dest = new TCHAR[size];
      STRNCPY_S (Dest, size, Source, length);
      return Dest;
#endif
    }
  };

  /*!
  Convert from one string format to another.
  */
  template < typename CONVERT_TO, typename CONVERT_FROM, typename BASE_CONVERTER, DWORD DefaultConversionSize = 128 >
  class NCharacterConversion:   public BASE_CONVERTER
  {
    CONVERT_TO *ConvertedString;

    // Hide the default constructor
    NCharacterConversion();

  public:
    /*!
        Converts the data by using the Convert() method on the base class
    */
    explicit inline NCharacterConversion (const CONVERT_FROM *Source)
    {
      if (Source != NULL)
      {
        // Use base class' convert method
        ConvertedString = BASE_CONVERTER::Convert (Source);
      }
      else
      {
        ConvertedString = NULL;
      }
    }

    /*!
        If memory was allocated, then it is freed below
    */
    inline ~NCharacterConversion()
    {
      if (ConvertedString != NULL)
      {
        delete [] ConvertedString;
      }
    }

    // Operator to get access to the converted string
    inline operator CONVERT_TO* (void) const
    {
      return ConvertedString;
    }
  };

// Conversion typedefs
// typedef NCharacterConversion<TCHAR, ANSICHAR, AnsiToTCharConversion>           ANSI_To_TCHAR_Conversion;
// typedef NCharacterConversion<ANSICHAR, TCHAR, TCharToAnsiConvertion>           TCHAR_To_ANSI_Conversion;
// typedef NCharacterConversion<ANSICHAR, UNICHAR, UnicharToAnsicharConvertion>   UNICHAR_To_ANSICHAR_Conversion;
// typedef NCharacterConversion<UNICHAR, ANSICHAR, AnsicharToUnicharConvertion>   ANSICHAR_To_UNICHAR_Conversion;

}

#endif // NUNICODE_H
