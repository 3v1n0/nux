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
// Copy a string with length checking.
//warning: Behavior differs from strncpy; last character is zeroed.
//
  TCHAR *Strncpy (TCHAR *Dest, size_t Size, const TCHAR *Src, size_t MaxLen)
  {
    STRNCPY_S (Dest, Size, Src, MaxLen);
    Dest[MaxLen] = 0;
    return Dest;
  }

// Search a string inside a string. Return a pointer to the beginning of the searched string if it is found. Else, return NULL;
// The shearched string must be preceded by a non alpha numeric character in Str.

  const TCHAR *Strfind (const TCHAR *Str, const TCHAR *Find)
  {
    nuxAssert (Find != NULL);
    nuxAssert (Str != NULL);

    if (Find == NULL || Str == NULL)
    {
      return NULL;
    }

    bool AlphaNum = 0;
    TCHAR First = ( (*Find < TEXT ('a') ) || (*Find > TEXT ('z') ) ) ? (*Find) : (*Find + TEXT ('A') - TEXT ('a') );
    size_t Len = StringLength (Find++) - 1;
    TCHAR chr = *Str++;

    while (chr)
    {
      if ( (chr >= TEXT ('a') ) && (chr <= TEXT ('z') ) )
      {
        chr += TEXT ('A') - TEXT ('a');
      }

      if (!AlphaNum && (chr == First) && !TCharStringNICompare (Str, Find, Len) )
      {
        return Str - 1;
      }

      AlphaNum = ( (chr >= TEXT ('A') ) && (chr <= TEXT ('Z') ) ) || ( (chr >= TEXT ('0') ) && (chr <= TEXT ('9') ) );
      chr = *Str++;
    }

    return NULL;
  }

  VARARG_BODY (int, Snprintf, const TCHAR *, VARARG_EXTRA (TCHAR *Dest) VARARG_EXTRA (int Size) VARARG_EXTRA (int Count) )
  {
    int Result = -1;
    GET_VARARGS_RESULT (Dest, Size, Count, Fmt, Result);
    return Result;
  }
}
