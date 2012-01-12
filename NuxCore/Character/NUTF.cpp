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
#include "NUTF.h"

namespace nux
{

  NUTF8::NUTF8 (const UNICHAR *Source)
  {
    Convert (Source);
  }

  NUTF8::NUTF8 (const std::wstring &Source)
  {
    Convert (NUX_REINTERPRET_CAST (UNICHAR *, NUX_CONST_CAST (wchar_t *, Source.c_str() ) ) );
  }

  void NUTF8::Convert (const UNICHAR *Source)
  {
    int NumBytes = 0;
    // *6 each UTF16 char can translate to up to 6 bytes in UTF8
    // +1 for NULL char
    size_t Size = wcslen ( (wchar_t *) Source) * 6 + 1;
    utf8 = new char[Size];
    memset (utf8, 0, Size);

    unsigned char TwoBytes[2];
    TwoBytes[0] = '\0';
    TwoBytes[1] = '\0';

    utf8[0] = '\0';

    //     U-00000000 – U-0000007F: 	0xxxxxxx
    //     U-00000080 – U-000007FF: 	110xxxxx 10xxxxxx
    //     U-00000800 – U-0000FFFF: 	1110xxxx 10xxxxxx 10xxxxxx
    //     U-00010000 – U-001FFFFF: 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    //     U-00200000 – U-03FFFFFF: 	111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //     U-04000000 – U-7FFFFFFF: 	1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    // The original specification of UTF-8 allowed for sequences of up to six bytes covering numbers up to 31 bits
    // (the original limit of the universal character set). However, UTF-8 was restricted by RFC 3629 to use only
    // the area covered by the formal Unicode definition, U+0000 to U+10FFFF, in November 2003. So UTF-8 code point is at most 4 bytes.

    for (size_t n = 0; Source[n] != 0; n++)
    {
      if (Source[n] <= 0x7F)
      {
        TwoBytes[0] = (char) Source[n];
        STRCAT_S (utf8, Size, (const char *) &TwoBytes[0]);
      }
      else
      {
        // 11 valid bits 2 bytes
        if (Source[n] <= 0x7FF)
        {
          // Extract the 5 highest bits
          TwoBytes[0] = (char) (0xC0 + (Source[n] >> 6) );
          NumBytes = 2;
        }
        // 16 valid bits 3 bytes
        else if (Source[n] <= 0xFFFF)
        {
          // Extract the highest 4 bits
          TwoBytes[0] = (char) (0xE0 + (Source[n] >> 12) );
          NumBytes = 3;
        }
        // Unichar is only 16 bits. Do no continue because (Source[n] >> 18) does not make sense.
        // 21 valid bits 4 bytes
        else if (Source[n] <= 0x1FFFFF)
        {
          // Extract the highest 3 bits
          TwoBytes[0] = (char) (0xF0 + (Source[n] >> 18) );
          NumBytes = 4;
        }
        // Split a 26 bit character into 5 bytes
        else if (Source[n] <= 0x3FFFFFF)
        {
          // Extract the highest 2 bits
          TwoBytes[0] = (char) (0xF8 + (Source[n] >> 24) );
          NumBytes = 5;
        }
        // Split a 31 bit character into 6 bytes
        else if (Source[n] <= 0x7FFFFFFF)
        {
          // Extract the highest bit
          TwoBytes[0] = (char) (0xFC + (Source[n] >> 30) );
          NumBytes = 6;
        }

        STRCAT_S (utf8, Size, (const char *) &TwoBytes[0]);

        // Extract the remaining bits - 6 bits at a time
        for (int i = 1, shift = (NumBytes - 2) * 6; shift >= 0; i++, shift -= 6)
        {
          TwoBytes[0] = (char) (0x80 + ( (Source[n] >> shift) & 0x3F) );
          STRCAT_S (utf8, Size, (const char *) &TwoBytes[0]);
        }
      }
    }
  }

// void NUTF8::Convert(const t_UTF32* Source)
// {
//     int NumBytes = 0;
//
//     int Size = 0;
//     while(Source[Size] != 0)
//     {
//         ++Size;
//     }
//     // *6: each UTF16 char can translate to up to 6 bytes in UTF8
//     // +1: for NULL char
//     Size = Size * 6 + 1;
//     utf8 = new char[Size*sizeof(t_UTF32)];
//     memset(utf8, 0, Size*sizeof(t_UTF32));
//
//     unsigned char TwoBytes[2];
//     TwoBytes[0] = '\0'; TwoBytes[1] = '\0';
//
//     utf8[0] = '\0';
//
//     //     U-00000000 – U-0000007F: 	0xxxxxxx
//     //     U-00000080 – U-000007FF: 	110xxxxx 10xxxxxx
//     //     U-00000800 – U-0000FFFF: 	1110xxxx 10xxxxxx 10xxxxxx
//     //     U-00010000 – U-001FFFFF: 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//     //     U-00200000 – U-03FFFFFF: 	111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//     //     U-04000000 – U-7FFFFFFF: 	1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//     // The original specification of UTF-8 allowed for sequences of up to six bytes covering numbers up to 31 bits
//     // (the original limit of the universal character set). However, UTF-8 was restricted by RFC 3629 to use only
//     // the area covered by the formal Unicode definition, U+0000 to U+10FFFF, in November 2003. So UTF-8 code point is at most 4 bytes.
//
//     for(size_t n = 0; Source[n] != 0; n++)
//     {
//         if (Source[n] <= 0x7F)
//         {
//             TwoBytes[0] = (char)Source[n];
//             STRCAT_S(utf8, Size, (const char*)&TwoBytes[0]);
//         }
//         else
//         {
//             // 11 valid bits 2 bytes
//             if (Source[n] <= 0x7FF)
//             {
//                 // Extract the 5 highest bits
//                 TwoBytes[0] = (char)(0xC0 + (Source[n] >> 6));
//                 NumBytes = 2;
//             }
//             // 16 valid bits 3 bytes
//             else if (Source[n] <= 0xFFFF)
//             {
//                 // Extract the highest 4 bits
//                 TwoBytes[0] = (char)(0xE0 + (Source[n] >> 12));
//                 NumBytes = 3;
//             }
//             // 21 valid bits 4 bytes
//             else if (Source[n] <= 0x1FFFFF)
//             {
//                 // Extract the highest 3 bits
//                 TwoBytes[0] = (char)(0xF0 + (Source[n] >> 18));
//                 NumBytes = 4;
//             }
//             // Split a 26 bit character into 5 bytes
//             else if (Source[n] <= 0x3FFFFFF)
//             {
//                 // Extract the highest 2 bits
//                 TwoBytes[0] = (char)(0xF8 + (Source[n] >> 24));
//                 NumBytes = 5;
//             }
//             // Split a 31 bit character into 6 bytes
//             else if (Source[n] <= 0x7FFFFFFF)
//             {
//                 // Extract the highest bit
//                 TwoBytes[0] = (char)(0xFC + (Source[n] >> 30));
//                 NumBytes = 6;
//             }
//
//             STRCAT_S(utf8, Size, (const char*)&TwoBytes[0]);
//
//             // Extract the remaining bits - 6 bits at a time
//             for(int i = 1, shift = (NumBytes-2)*6; shift >= 0; i++, shift -= 6)
//             {
//                 TwoBytes[0] = (char)(0x80 + ((Source[n] >> shift) & 0x3F));
//                 STRCAT_S(utf8, Size, (const char*)&TwoBytes[0]);
//             }
//         }
//     }
// }

  NUTF8::~NUTF8()
  {
    delete [] utf8;
  }

  NUTF8::operator const char* ()
  {
    return utf8;
  }

///////////////////////////////////////////////////////////////////////////////
// Convert each unicode character in the source to UTF-8

  NUTF16::NUTF16 (const char *Source)
  {
    Convert (Source);
  }

  NUTF16::NUTF16 (const std::string &Source)
  {
    Convert (Source.c_str() );
  }

  void NUTF16::Convert (const char *Source)
  {
    //     U-00000000 – U-0000007F: 	0xxxxxxx
    //     U-00000080 – U-000007FF: 	110xxxxx 10xxxxxx
    //     U-00000800 – U-0000FFFF: 	1110xxxx 10xxxxxx 10xxxxxx
    //     U-00010000 – U-001FFFFF: 	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    //     U-00200000 – U-03FFFFFF: 	111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //     U-04000000 – U-7FFFFFFF: 	1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

    unsigned char MSB;
    int temp = 0;
    int numbytes = 0; // Number of bytes used to represent the unicode char
    int pos = 0;

    size_t len = strlen (Source) + 1; // +1 for NULL char
    unicode = new UNICHAR[len*6];

    // Loop through the characters in the string and decode them
    for (size_t n = 0; n < len; ++n)
    {
      // Find the hexadecimal number following the equals sign
      MSB = Source[n];

      if (MSB <= 0x7F)
      {
        unicode[pos++] = (UNICHAR) MSB;
      }
      else
      {
        // 2 bytes
        if (MSB >= 0xC0 && MSB <= 0xDF)
        {
          temp = (MSB - 0xC0) << 6;
          numbytes = 2;
        }
        // 3 bytes
        else if (MSB >= 0xE0 && MSB <= 0xEF)
        {
          temp = (MSB - 0xE0) << 12;
          numbytes = 3;
        }
        // 4 bytes
        else if (MSB >= 0xF0 && MSB <= 0xF7)
        {
          temp = (MSB - 0xF0) << 18;
          numbytes = 4;
        }
        // 5 bytes
        else if (MSB >= 0xF8 && MSB <= 0xFB)
        {
          temp = (MSB - 0xF8) << 24;
          numbytes = 5;
        }
        // 6 bytes
        else if (MSB >= 0xFC && MSB <= 0xFD)
        {
          temp = (MSB - 0xFC) << 30;
          numbytes = 6;
        }

        // Loop through the remaining hexadecimal numbers representing the next unicode character
        for (int i = 0, shift = (numbytes - 2) * 6; shift >= 0; i++, shift -= 6)
        {
          int nVal = ( ( (unsigned char) Source[n+1+i]) - 0x80 ) << shift;
          temp += nVal;
        }

        // Add the unicode character to the final string
        unicode[pos++] = (UNICHAR) temp;

        // Move the character index in the source to the next unicode character
        n += (numbytes - 1);
      }
    }
  }

  NUTF16::~NUTF16()
  {
    delete [] unicode;
  }

  NUTF16::operator const UNICHAR* ()
  {
    return unicode;
  }

}
