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
#include "Parsing.h"

#define CHAR_TAB        TEXT('\t')
#define CHAR_CR         TEXT('\r')
#define CHAR_FF         TEXT('\f')
#define CHAR_NEW_LINE   TEXT('\n')
#define CHAR_QUOTE      TEXT('\"')

namespace nux
{

  bool ParseCommand (const TCHAR **Stream, const TCHAR  *Match)
  {
    while ( (**Stream == TEXT (' ') ) || (**Stream == CHAR_TAB) )
      (*Stream) ++;

    if (TCharStringNICompare (*Stream, Match, StringLength (Match) ) == 0)
    {
      *Stream += StringLength (Match);

      if (!IsAlphanumericChar (**Stream) )
      {
        while ( (**Stream == TEXT (' ') ) || (**Stream == CHAR_TAB) )
          (*Stream) ++;

        return true; // Success.
      }
      else
      {
        *Stream -= StringLength (Match);
        return false; // Only found partial match.
      }
    }
    else return false; // No match.
  }

  bool Parse_tchar (const TCHAR *Stream, const TCHAR *Match, TCHAR *Value, int Size, int MaxLen)
  {
    const TCHAR *Found = Strfind (Stream, Match);

    if (Found)
    {
      const TCHAR *Start;
      Start = Found + StringLength (Match);

      if (*Start == '\x22') // Character '"'
      {
        // The value begins with the quotation mark character: ". We skip it.
        Strncpy (Value, Size, Start + 1, MaxLen);
        Value[MaxLen - 1] = 0;
        TCHAR *Temp = Strstr (Value, TEXT ("\x22") );

        if (Temp != NULL)
        {
          // We read in the termination quotation mark. Set it t0 0 to null terminate the Value buffer.
          *Temp = 0;
        }
      }
      else
      {
        // Non-quoted string without spaces.
        Strncpy (Value, Size, Start, MaxLen);
        Value[MaxLen - 1] = 0;
        TCHAR *Temp;
        Temp = Strstr (Value, TEXT (" ") );

        if (Temp) *Temp = 0;

        Temp = Strstr (Value, TEXT ("\r") );

        if (Temp) *Temp = 0;

        Temp = Strstr (Value, TEXT ("\n") );

        if (Temp) *Temp = 0;

        Temp = Strstr (Value, TEXT ("\t") );

        if (Temp) *Temp = 0;

        Temp = Strstr (Value, TEXT (",") );

        if (Temp) *Temp = 0;
      }

      return true;
    }
    else
      return false;
  }

  bool ParseParam (const TCHAR *Stream, const TCHAR *Param)
  {
    const TCHAR *Start = Stream;

    if (*Stream)
    {
      while ( (Start = Strfind (Start + 1, Param) ) != NULL)
      {
        if (Start > Stream && ( (Start[-1] == TEXT ('-') ) || (Start[-1] == TEXT ('/') ) ) )
        {
          const TCHAR *End = Start + StringLength (Param);

          if (End == NULL || *End == 0 || IsWhitespaceChar (*End) )
            return true;
        }
      }
    }

    return false;
  }

  bool Parse_string (const TCHAR *Stream, const TCHAR *Match, std::string &Value)
  {
    TCHAR Temp[4096] = TEXT ("");

    if (Parse_tchar (Stream, Match, Temp, NUX_ARRAY_COUNT (Temp), NUX_ARRAY_COUNT (Temp) ) )
    {
      Value = Temp;
      return true;
    }
    else return false;
  }

  bool Parse_u64 (const TCHAR *Stream, const TCHAR *Match, QWORD &Value)
  {
    return Parse_s64 (Stream, Match, * (SQWORD *) &Value);
  }

  bool Parse_s64 (const TCHAR *Stream, const TCHAR *Match, SQWORD &Value)
  {
    TCHAR Temp[4096] = TEXT (""), *Ptr = Temp;

    if (Parse_tchar (Stream, Match, Temp, NUX_ARRAY_COUNT (Temp), NUX_ARRAY_COUNT (Temp) ) )
    {
      Value = 0;
      bool Negative = (*Ptr == TEXT ('-') );
      Ptr += Negative;

      while ( (*Ptr >= TEXT ('0') ) && (*Ptr <= TEXT ('9') ) )
        Value = Value * 10 + *Ptr++ - TEXT ('0');

      if (Negative)
        Value = -Value;

      return true;
    }
    else
      return false;
  }

  bool Parse_u32 (const TCHAR *Stream, const TCHAR *Match, DWORD &Value)
  {
    const TCHAR *Temp = Strfind (Stream, Match);
    TCHAR *End;

    if (Temp == NULL)
      return false;

    Value = Strtoi (Temp + StringLength (Match), &End, 10);

    return true;
  }

  bool Parse_u8 (const TCHAR *Stream, const TCHAR *Match, BYTE &Value)
  {
    const TCHAR *Temp = Strfind (Stream, Match);

    if (Temp == NULL)
      return false;

    Temp += StringLength (Match);
    Value = (BYTE) CharToInteger (Temp);
    return (Value != 0) || IsDigitChar (Temp[0]);
  }

  bool Parse_s8 (const TCHAR *Stream, const TCHAR *Match, SBYTE &Value)
  {
    const TCHAR *Temp = Strfind (Stream, Match);

    if (Temp == NULL)
      return false;

    Temp += StringLength (Match);
    Value = CharToInteger (Temp);
    return Value != 0 || IsDigitChar (Temp[0]);
  }

  bool Parse_u16 (const TCHAR *Stream, const TCHAR *Match, WORD &Value)
  {
    const TCHAR *Temp = Strfind (Stream, Match);

    if (Temp == NULL)
      return false;

    Temp += StringLength (Match);
    Value = (unsigned short) CharToInteger (Temp);
    return Value != 0 || IsDigitChar (Temp[0]);
  }

  bool Parse_s16 (const TCHAR *Stream, const TCHAR *Match, SWORD &Value)
  {
    const TCHAR *Temp = Strfind (Stream, Match);

    if (Temp == NULL)
      return false;

    Temp += StringLength (Match);
    Value = (short) CharToInteger (Temp);
    return Value != 0 || IsDigitChar (Temp[0]);
  }

  bool Parse_float (const TCHAR *Stream, const TCHAR *Match, float &Value)
  {
    const TCHAR *Temp = Strfind (Stream, Match);

    if (Temp == NULL)
      return false;

    Value = CharToDouble (Temp + StringLength (Match) );
    return true;
  }

  bool Parse_int (const TCHAR *Stream, const TCHAR *Match, int &Value)
  {
    const TCHAR *Temp = Strfind (Stream, Match);

    if (Temp == NULL)
      return false;

    Value = CharToInteger (Temp + StringLength (Match) );
    return true;
  }

  bool Parse_bool (const TCHAR *Stream, const TCHAR *Match, bool &OnOff)
  {
    TCHAR TempStr[16];

    if (Parse_tchar (Stream, Match, TempStr, NUX_ARRAY_COUNT (TempStr), NUX_ARRAY_COUNT (TempStr) - 1) )
    {
      OnOff =	!Stricmp (TempStr, TEXT ("On") ) || !Stricmp (TempStr, TEXT ("True") ) || !Stricmp (TempStr, TEXT ("1") );
      return true;
    }
    else
      return false;
  }

  void ParseToNextLine (const TCHAR **Stream, TCHAR CommentChar)
  {
    // Skip over spaces, tabs, cr's, and linefeeds.

    while (1)
    {
      while ( (**Stream == TEXT (' ') ) || (**Stream == CHAR_TAB) || (**Stream == CHAR_CR) || (**Stream == CHAR_NEW_LINE)  || (**Stream == CHAR_FF) )
      {
        // Skip tabs, cr, new line, form feed
        ++*Stream;
      }

      if (**Stream == CommentChar)
      {
        // Start of a comment
        while ( (**Stream != 0) && (**Stream != CHAR_NEW_LINE) && (**Stream != CHAR_CR) )
        {
          // Advance to a new line
          ++*Stream;
        }
      }
      else
      {
        break;
      }
    }
  }

  bool ParseToken (const TCHAR *Str, TCHAR *TokenBuffer, int BufferSize)
  {
    int sz = 0;

    while ( (*Str == TEXT (' ') ) || (*Str == CHAR_TAB) )
    {
      // Skip spaces and tabs.
      Str++;
    }

    if (*Str == CHAR_QUOTE)
    {
      // Get quoted string.
      Str++;

      while (*Str && (*Str != CHAR_QUOTE) && (sz + 1 < BufferSize) )
      {
        TCHAR c = *Str++;

        if (sz + 1 < BufferSize)
          TokenBuffer[sz++] = c;
      }

      if (*Str == CHAR_QUOTE)
        Str++;
    }
    else
    {
      // Get unquoted string.
      for (; *Str && (*Str != TEXT (' ') ) && (*Str != CHAR_TAB); Str++)
      {
        if (sz + 1 < BufferSize)
          TokenBuffer[sz++] = *Str;
      }
    }

    TokenBuffer[sz] = 0;
    return sz != 0;
  }

  bool ParseToken (const TCHAR *Str, std::string &TokenString)
  {
    TokenString.clear();

    // Skip spaces and tabs.
    while (IsWhitespaceChar (*Str) )
      Str++;

    if (*Str == CHAR_QUOTE)
    {
      // Get quoted string.
      Str++;

      while (*Str && *Str != CHAR_QUOTE)
      {
        TCHAR c = *Str++;
        TokenString += c;
      }

      if (*Str == CHAR_QUOTE)
        Str++;
    }
    else
    {
      // Get unquoted string.
      for (; *Str && !IsWhitespaceChar (*Str); Str++)
      {
        TokenString += *Str;
      }
    }

    return TokenString.length() > 0;
  }

  std::string ParseToken (const TCHAR *Str, bool /* UseEscape */)
  {
    TCHAR Buffer[1024];

    if (ParseToken (Str, Buffer, NUX_ARRAY_COUNT (Buffer) ) )
      return Buffer;
    else
      return TEXT ("");
  }

//
// Get a line of Stream (everything up to, but not including, CR/LF.
// Returns 0 if ok, nonzero if at end of stream and returned 0-length string.
//
  bool ParseLine (const TCHAR **Stream, TCHAR *LineBuffer, int BufferSize)
  {
    TCHAR *tmp = LineBuffer;
    *tmp = 0;

    while ( (**Stream != 0) && (**Stream != CHAR_NEW_LINE) && (**Stream != CHAR_CR) && (**Stream != CHAR_FF) && (--BufferSize > 0) )
    {
      * (tmp++) = * ( (*Stream) ++);
    }

    *tmp = 0;
    return LineBuffer[0] != 0;
  }

  bool ParseLine (const TCHAR **Stream, std::string &LineString)
  {
    LineString.clear();

    while ( (**Stream != 0) && (**Stream != CHAR_NEW_LINE) && (**Stream != CHAR_CR) && (**Stream != CHAR_FF) )
    {
      LineString += **Stream++;
    }

    return LineString.size() > 0;
  }

}
