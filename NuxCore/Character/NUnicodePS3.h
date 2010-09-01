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


#ifndef NUNICODEPS3_H
#define NUNICODEPS3_H

#include <cwchar>
#include <string>
#include <cstring>

#include <cell\l10n.h>

NAMESPACE_BEGIN

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

enum {UNICODE_BOM   = 0xfeff     };

#ifdef INL_UNICODE
    inline TCHAR    ConvertAnsiCharToTCHAR   ( ANSICHAR In )
    {
        TCHAR Out;
        MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)&In, 1, (LPWSTR)&Out, 2);
        return Out;
        //return (BYTE)In;
    }
    inline TCHAR    ConvertUnicodeCharToTCHAR( UNICHAR In  ) { return In;                              }
    inline ANSICHAR ConvertTCHARToAnsiChar   ( TCHAR In    )
    {
        ANSICHAR Out;
        WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)&In, 2, (LPSTR)&Out, 1, NULL, NULL);
        return Out;
        //return (WORD)In<0x100 ? In : MAXSBYTE;
    }
    inline UNICHAR  ConvertTCHARToUnicodeChar( TCHAR In    ) { return In;                              }
#else
    inline TCHAR    ConvertAnsiCharToTCHAR   ( ANSICHAR In ) { return In;                              }
    inline TCHAR    ConvertUnicodeCharToTCHAR( UNICHAR In  )
    {
        t_u8 Out;
        t_size in_size = 1;
        t_size out_size = 1;
        UTF16stoUTF8s(&In, &in_size, &Out, &out_size);
        //WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)&In, 2, (LPSTR)&Out, 1, NULL, NULL);
        return Out;
    }
    inline ANSICHAR ConvertTCHARToAnsiChar   ( TCHAR In    ) { return In; }
    inline UNICHAR  ConvertTCHARToUnicodeChar( TCHAR In    )
    {
        t_u16 Out;
        t_size in_size = 1;
        t_size out_size = 1;
        UTF8stoUTF16s((const t_u8*)&In, &in_size, &Out, &out_size);
        //MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)&In, 1, (LPWSTR)&Out, 2);
        return Out;
    }
#endif

/*!
    Convert a single UNICHAR to ANSICHAR.
*/
inline ANSICHAR ConvertUnicodeCharToAnsiChar(UNICHAR In)
{
    t_u8 Out;
    t_size in_size = 1;
    t_size out_size = 1;
    UTF16stoUTF8s(&In, &in_size, &Out, &out_size);
    //WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)&In, 2, (LPSTR)&Out, 1, NULL, NULL);
    return Out;
}

/*!
Convert a single ANSICHAR to UNICHAR.
*/
inline UNICHAR ConvertAnsiCharToUnicodeChar(ANSICHAR In)
{
    t_u16 Out;
    t_size in_size = 1;
    t_size out_size = 1;
    UTF8stoUTF16s((t_u8*)&In, &in_size, &Out, &out_size);
    //MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (LPCSTR)&In, 1, (LPWSTR)&Out, 2);
    return Out;
}

//! UNICHAR to ANSICHAR conversion 
class UNICHAR_To_ANSICHAR_Convert
{
public:
    // Default to ANSI code page
    UNICHAR_To_ANSICHAR_Convert(DWORD CP = 0)
        :   CodePage(CP)
    {
    }

    /*!
    Convert from UNICHAR to ANSICHAR
    @param Source String to convert. Null terminated.
    @return Return a pointer to the new string. Null terminated.
    */
    ANSICHAR* Convert(const UNICHAR* Src)
    {
        ANSICHAR* Dest;
        size_t UnicodeLen = wcslen((const wchar_t*)Src) + 1; // +1 for NULL character
        size_t AnsiCharLen = 0;

        UTF16stoUTF8s(Src, &UnicodeLen, 0, &AnsiCharLen);
        Dest = new ANSICHAR[ AnsiCharLen ]; // nUserNameLen includes the NULL character
        UTF16stoUTF8s(Src, &UnicodeLen, (t_u8*)Dest, &AnsiCharLen);
        return Dest;
    }
private:
    DWORD CodePage;
};

//! ANSICHAR to UNICHAR conversion 
class ANSICHAR_To_UNICHAR_Convert
{
public:
    // Default to ANSI code page
    INL_INLINE ANSICHAR_To_UNICHAR_Convert(DWORD CP = 0)
        :   CodePage(CP)
    {
    }

    /*!
    Convert from ANSICHAR to UNICHAR
    @param Source String to convert. Null terminated.
    @return Return a pointer to the new string. Null terminated.
    */
    INL_INLINE UNICHAR* Convert(const ANSICHAR* Src)
    {
        UNICHAR* Dest;
        size_t AnsiCharLen = strlen(Src) + 1; // +1 for NULL character
        size_t UnicodeLen = 0;
        UTF8stoUTF16s((t_u8*)Src, &AnsiCharLen, 0, &UnicodeLen);
        Dest = new UNICHAR[ UnicodeLen ];
        UTF8stoUTF16s((t_u8*)Src, &AnsiCharLen, Dest, &UnicodeLen);
        return Dest;
    }
private:
    DWORD CodePage;
};

//! TCHAR to ANSI conversion
// TCHAR can be ansi or unicode depending if UNICODE is defined or not.
class TCHARToANSI_Convert
{
    DWORD CodePage;
public:
    INL_INLINE TCHARToANSI_Convert(DWORD CP = 0) :
      CodePage(CP)
      {
      }

      /*!
      Convert from TCHAR to ANSICHAR
      @param Source String to convert. Null terminated.
      @return Return a pointer to the new string. Null terminated.
      */
      INL_INLINE ANSICHAR* Convert(const TCHAR* Src)
      {
          // Determine whether we need to allocate memory or not
#ifdef INL_UNICODE
          ANSICHAR* Dest;
          size_t UnicodeLen = wcslen(Src) + 1; // +1 for NULL character
          size_t AnsiCharLen = 0;

          UTF16stoUTF8s(Src, &UnicodeLen, 0, &AnsiCharLen);
          Dest = new char[ AnsiCharLen ]; // nUserNameLen includes the NULL character
          UTF16stoUTF8s(Src, &UnicodeLen, Dest, &AnsiCharLen);
          return Dest;
#else
          DWORD LengthW = (DWORD)strlen(Src) + 1;
          INT Size = LengthW * sizeof(ANSICHAR);
          ANSICHAR* Dest = new ANSICHAR[Size];
          STRNCPY_S(Dest, Size, Src, LengthW);
          return Dest;
#endif
      }
};

//! ANSI to TCHAR conversion
// TCHAR can be ansi or unicode depending if UNICODE is defined or not.
class ANSIToTCHAR_Convert
{
    DWORD CodePage;
public:
    INL_INLINE ANSIToTCHAR_Convert(DWORD CP = 0)
        :   CodePage(CP)
    {
    }

    /*!
    Convert from ANSICHAR to TCHAR
    @param Source String to convert. Null terminated.
    @return Return a pointer to the new string. Null terminated.
    */
    INL_INLINE TCHAR* Convert(const ANSICHAR* Src)
    {
#ifdef INL_UNICODE
        UNICHAR* Dest;
        int d = sizeof(UNICHAR);
        size_t AnsiCharLen = strlen(Src) + 1; // +1 for NULL character
        size_t UnicodeLen = 0;
        UTF8stoUTF16s(Src, &AnsiCharLen, 0, &UnicodeLen);
        Dest = new UNICHAR[UnicodeLen];
        UTF8stoUTF16s(Src, &AnsiCharLen, Dest, &UnicodeLen);
        return Dest;
#else
        // Determine whether we need to allocate memory or not
        DWORD Length = (DWORD)strlen(Src) + 1;
        INT Size = Length;
        TCHAR* Dest = new TCHAR[Size];
        STRNCPY_S(Dest, Size, Src, Length);
        return Dest;
#endif
    }
};

/*!
Convert from one string format to another.
*/
template<typename CONVERT_TO,typename CONVERT_FROM, typename BASE_CONVERTER, DWORD DefaultConversionSize = 128>
class NStringConversion:   public BASE_CONVERTER
{
    CONVERT_TO* ConvertedString;

    // Hide the default constructor
    NStringConversion();

public:
    /*!
        Converts the data by using the Convert() method on the base class
    */
    explicit inline NStringConversion(const CONVERT_FROM* Source)
    {
        if (Source != NULL)
        {
            // Use base class' convert method
            ConvertedString = BASE_CONVERTER::Convert(Source);
        }
        else
        {
            ConvertedString = NULL;
        }
    }

    /*!
        If memory was allocated, then it is freed below
    */
    inline ~NStringConversion()
    {
        if (ConvertedString != NULL)
        {
            delete [] ConvertedString;
        }
    }

    // Operator to get access to the converted string
    inline operator CONVERT_TO*(void) const
    {
        return ConvertedString;
    }
};

// Conversion typedefs
typedef NStringConversion<TCHAR,ANSICHAR,ANSIToTCHAR_Convert>  ANSI_To_TCHAR_Conversion;
typedef NStringConversion<ANSICHAR,TCHAR,TCHARToANSI_Convert>  TCHAR_To_ANSI_Conversion;
//typedef NStringConversion<ANSICHAR,TCHAR,FTCHARToOEM_Convert>   TCHAR_To_OEM_Conversion;
typedef NStringConversion<ANSICHAR,UNICHAR,UNICHAR_To_ANSICHAR_Convert>   UNICHAR_To_ANSICHAR_Conversion;
typedef NStringConversion<UNICHAR,ANSICHAR,ANSICHAR_To_UNICHAR_Convert>   ANSICHAR_To_UNICHAR_Conversion;


NAMESPACE_END

#endif // NUNICODEPS3_H
