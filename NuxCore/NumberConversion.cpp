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

#include "NuxCore.h"
#include "NumberConversion.h"

namespace nux
{

  /**
   * From: http://www.strudel.org.uk/itoa/
   * C++ version 0.4 char* style "itoa":
   * Written by Lukás Chmela
   * Released under GPLv3.
   */
  NString IntegerToChar (int value, int base)
  {
    char result[65];

    // check that the base if valid
    if (base < 2 || base > 36)
    {
      //*result = '\0';
      NString str = TEXT ('\0');
      return str;
    }

    char *ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do
    {
      tmp_value = value;
      value /= base;
      *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base) ];
    }
    while ( value );

    // Apply negative sign
    if (tmp_value < 0)
      *ptr++ = '-';

    *ptr-- = '\0';

    while (ptr1 < ptr)
    {
      tmp_char = *ptr;
      *ptr-- = *ptr1;
      *ptr1++ = tmp_char;
    }

    NString str = result;
    return str;
  }

  t_double CharToDouble (const TCHAR *digit)
  {
    char *endptr = NULL;
    NString str = TCHAR_TO_ANSICHAR (digit);
    errno = 0;
    t_double ret = std::strtod (str.GetTCharPtr(), &endptr);
    t_u32 error = errno;

    if (error == ERANGE)
    {
      nuxDebugMsg ("[CharToDouble] Out for range value");
    }

    return ret;
  }

  NString DoubleToChar (double d)
  {
    TCHAR *buffer = new TCHAR[64];
    Memset (buffer, 0, 64);
    SPRINTF_S (buffer, 64, "%.39f", d);
    NString str = buffer;
    str = str.TrimRight (TEXT ("0") );
    return str;
  }

  t_s32 CharToInteger (const TCHAR *digit)
  {
    NString str = TCHAR_TO_ANSICHAR (digit);
    t_s64 ret = std::atoi (str.GetTCharPtr() );
    return ret;
  }

// convert an hexadecimal string to t_u32
  t_u32 HexCharToInteger (const TCHAR *s)
  {
    int n = 0;         // position in string
    int m = 0;         // position in digit[] to shift
    int count;         // loop index
    unsigned long intValue = 0;  // integer value of hex string
    int digit[16];      // hold values to convert

    const TCHAR *hexStg = s;

    if ( (s[0] == TEXT ('0') ) && ( (s[1] == TEXT ('X') ) || (s[1] == TEXT ('x') ) ) )
    {
      hexStg = s + 2;
    }

    while (n < 16)
    {
      if (hexStg[n] == TEXT ('\0') )
        break;

      if (hexStg[n] > 0x29 && hexStg[n] < 0x40 ) //if 0 to 9
        digit[n] = hexStg[n] & 0x0f;            //convert to int
      else if (hexStg[n] >= TEXT ('a') && hexStg[n] <= TEXT ('f') ) //if a to f
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
      else if (hexStg[n] >= TEXT ('A') && hexStg[n] <= TEXT ('F') ) //if A to F
        digit[n] = (hexStg[n] & 0x0f) + 9;      //convert to int
      else break;

      n++;
    }

    count = n;
    m = n - 1;
    n = 0;

    while (n < count)
    {
      // digit[n] is value of hex digit at position n
      // (m << 2) is the number of positions to shift
      // OR the bits into return value
      intValue = intValue | (digit[n] << (m << 2) );
      m--;   // adjust the position to set
      n++;   // next digit to process
    }

    return (intValue);
  }

}
