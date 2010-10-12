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

namespace nux
{
//
// Copy a string with length checking.
//warning: Behavior differs from strncpy; last character is zeroed.
//
  TCHAR *Strncpy (TCHAR *Dest, t_size Size, const TCHAR *Src, t_size MaxLen)
  {
    nuxAssert (MaxLen >= 0);
    STRNCPY_S (Dest, Size, Src, MaxLen);
    Dest[MaxLen] = 0;
    return Dest;
  }

//
// Concatenate a string with length checking
//
  TCHAR *Strncat (TCHAR *Dest, t_size Size, const TCHAR *Src, t_size MaxLen)
  {
    t_size Len = StringLength (Dest);
    nuxAssert (Size >= Len);
    TCHAR *NewDest = Dest + Len;

    if ( (MaxLen -= Len) > 0)
    {
      Strncpy (NewDest, Size - Len, Src, MaxLen);
      NewDest[MaxLen-1] = 0;
    }

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
    t_size Len = StringLength (Find++) - 1;
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

  bool IsLastChar (const TCHAR *CharString, const TCHAR Chr)
  {
    nuxAssert (CharString != 0);

    if (CharString == 0)
      return false;

    t_size Size = StringLength (CharString);

    if (Size == 0)
      return false;

    if (CharString[Size-1] == Chr)
      return true;

    return false;
  }

  NString Itoa (int InNum)
  {
    SQWORD	Num					= InNum; // This avoids having to deal with negating -MaxS32 - 1
    NString NumberString;
    const TCHAR	*NumberChar[10]		= { TEXT ("0"), TEXT ("1"), TEXT ("2"), TEXT ("3"), TEXT ("4"), TEXT ("5"), TEXT ("6"), TEXT ("7"), TEXT ("8"), TEXT ("9") };
    bool	bIsNumberNegative	= FALSE;

    // Correctly handle negative numbers and convert to positive integer.
    if (Num < 0)
    {
      bIsNumberNegative = TRUE;
      Num = -Num;
    }

    // Convert to string assuming base ten and a positive integer.
    do
    {
      NumberString += NumberChar[Num % 10];
      Num /= 10;
    }
    while (Num);

    // Append sign as we're going to reverse string afterwards.
    if (bIsNumberNegative)
    {
      NumberString += TEXT ("-");
    }

    NumberString.Reverse();
    return NumberString;
  }

//! Duplicate a null terminated string using new[]. The resulting string is NULL terminated. Use delete[] to destroy.
  TCHAR *Strdup (const TCHAR *str)
  {
    if (str == 0)
      return 0;

    t_size len = StringLength (str);

    if (len >= 0)
    {
      TCHAR *res = new TCHAR[len+1];
      Strncpy (res, len + 1, str, len);
      return res;
    }

    return 0;
  }

//! Duplicate a null terminated ANSICHAR string using new[]. The resulting string is NULL terminated. Use delete[] to destroy.
  ANSICHAR *StrdupA ( const ANSICHAR *str)
  {
    if (str == 0)
      return 0;

    int len = (int) strlen (str);

    if (len >= 0)
    {
      ANSICHAR *res = new ANSICHAR[len+1];
      STRNCPY_S ( (char *) res, len + 1, (const char *) str, len);
      return res;
    }

    return 0;
  }

//! Duplicate a null terminated UNICHAR string using new[]. The resulting string is NULL terminated. Use delete[] to destroy.
  UNICHAR *StrdupU ( const UNICHAR *str)
  {
    if (str == 0)
      return 0;

    int len = (int) wcslen ( (const wchar_t *) str);

    if (len >= 0)
    {
      UNICHAR *res = new UNICHAR[len+1];
      WCSNCPY_S ( (wchar_t *) res, len + 1, (const wchar_t *) str, len);
      return res;
    }

    return 0;
  }
// /*
// * Standard string formatted print.
// */
// VARARG_BODY(int, inlSprintf, const TCHAR*, VARARG_EXTRA(TCHAR* Dest))
// {
//     int	Result = -1;
//     va_list ap;
//     va_start(ap, Fmt);
//     //@warning: make sure code using inlSprintf allocates enough memory if the below 1024 is ever changed.
//     GET_VARARGS_RESULT(Dest,1024/*!!*/,Fmt,Result);
//     return Result;
// }


  t_size ValueToLiteralString (char *buffer, t_u32 len, t_u16     value)
  {
    return ToCharString (buffer, len, "%hu",     value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_s16     value)
  {
    return ToCharString (buffer, len, "%hi",     value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_u32     value)
  {
    return ToCharString (buffer, len, "%lu",     value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_s32     value)
  {
    return ToCharString (buffer, len, "%li",     value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_ulong   value)
  {
    return ToCharString (buffer, len, "%lu",     value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_long    value)
  {
    return ToCharString (buffer, len, "%li",     value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_u64     value)
  {
    return ToCharString (buffer, len, "%I64u",   value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_s64     value)
  {
    return ToCharString (buffer, len, "%I64i",   value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_float   value)
  {
    return ToCharString (buffer, len, "%.10g",   value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_double  value)
  {
    return ToCharString (buffer, len, "%.20lg",  value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_u8      value)
  {
    return ValueToLiteralString (buffer, len,    (t_u32) value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_char    value)
  {
    return ValueToLiteralString (buffer, len,    (t_s32) value);
  }
  t_size ValueToLiteralString (char *buffer, t_u32 len, t_s8      value)
  {
    return ValueToLiteralString (buffer, len,    (t_s32) value);
  }

  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_u16    &value)
  {
    return FromCharString (buffer, len, "%hu", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_s16    &value)
  {
    return FromCharString (buffer, len, "%hi", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_u32    &value)
  {
    return FromCharString (buffer, len, "%lu", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_s32    &value)
  {
    return FromCharString (buffer, len, "%li", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_ulong  &value)
  {
    return FromCharString (buffer, len, "%lu", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_long   &value)
  {
    return FromCharString (buffer, len, "%li", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_u64    &value)
  {
    return FromCharString (buffer, len, "%I64u", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_s64    &value)
  {
    return FromCharString (buffer, len, "%I64i", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_float  &value)
  {
    return FromCharString (buffer, len, "%g", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_double &value)
  {
    return FromCharString (buffer, len, "%lg", value );
  }
  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_u8     &value)
  {
    t_u32 tmp = 0;
    bool result;
    result = ValueFromLiteralString (buffer, len, tmp);
    value = (t_u8) tmp;
    return result;
  }

  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_char &value)
  {
    t_s32 tmp = 0;
    bool result;
    result = ValueFromLiteralString (buffer, len, tmp);
    value = (t_char) tmp;
    return result;
  }

  bool ValueFromLiteralString (const char *buffer, t_u32 len, t_s8 &value)
  {
    t_s32 tmp = 0;
    bool result;
    result = ValueFromLiteralString (buffer, len, tmp);
    value = (t_s8) tmp;
    return result;
  }

  VARARG_BODY (int, Snprintf, const TCHAR *, VARARG_EXTRA (TCHAR *Dest) VARARG_EXTRA (int Size) VARARG_EXTRA (int Count) )
  {
    int Result = -1;
    GET_VARARGS_RESULT (Dest, Size, Count, Fmt, Result);
    return Result;
  }

  NString::NString()
  {

  }

  NString::NString (const NString &s)
  {
    m_string = s.m_string;
  }

  NString &NString::operator= (const NString &s)
  {
    m_string = s.m_string;
    return *this;
  }

  NString::NString (const tstring &s)
  {
    m_string = s;
  }

  NString::NString (const TCHAR &s)
  {
    m_string = s;
  }

  NString::NString (const ANSICHAR *s)
  {
#ifdef UNICODE

    if (s == 0)
      m_string = TEXT ("");
    else
      m_string = ANSICHAR_TO_UNICHAR (s);

#else

    if (s == 0)
      m_string = TEXT ("");
    else
      m_string = s;

#endif
  }

  NString::NString (const UNICHAR *s)
  {
#ifdef UNICODE

    if (s == 0)
      m_string = TEXT ("");
    else
      m_string = s;

#else

    if (s == 0)
      m_string = TEXT ("");
    else
      m_string = UNICHAR_TO_ANSICHAR (s);

#endif
  }


  NString::~NString()
  {
  }

  const tstring &NString::GetTStringRef() const
  {
    return m_string;
  }

//const TCHAR* NString::GetTChar() const
//{
//    return m_string.c_str();
//}

  const TCHAR *NString::GetTCharPtr() const
  {
    return m_string.c_str();
  }

  t_size NString::Length() const
  {
    return m_string.length();
  }

  t_size NString::Size() const
  {
    return m_string.size();
  }

  void NString::Clear()
  {
    m_string.clear();
  }

  bool NString::IsEmpty()
  {
    return m_string.empty();
  }

  void NString::Erase (t_size Pos, t_size Count)
  {
    m_string.erase (Pos, Count);
  }

  NString &NString::Insert (t_size Pos, const TCHAR *Ptr)
  {
    m_string.insert (Pos, Ptr);
    return *this;
  }

  NString &NString::Insert (t_size Pos, const TCHAR *Ptr, t_size Count)
  {
    m_string.insert (Pos, Ptr, Count);
    return *this;
  }

  NString &NString::Insert (t_size Pos, const tstring &Str)
  {
    m_string.insert (Pos, Str);
    return *this;
  }

  NString &NString::Insert (t_size Pos, const tstring &Str, t_size Offset, t_size Count)
  {
    m_string.insert (Pos, Str, Offset, Count);
    return *this;
  }

  NString &NString::Insert (t_size Pos, const NString &Str)
  {
    m_string.insert (Pos, Str.m_string);
    return *this;
  }

  NString &NString::Insert (t_size Pos, const NString &Str, t_size Offset, t_size Count)
  {
    m_string.insert (Pos, Str.m_string, Offset, Count);
    return *this;
  }

  NString &NString::Insert (t_size Pos, int Count, const TCHAR &Ch)
  {
    m_string.insert (Pos, Count, Ch);
    return *this;
  }

  const TCHAR &NString::operator[] (t_size ChPos) const
  {
    return m_string[ChPos];
  }

  TCHAR &NString::operator[] (t_size ChPos)
  {
    return m_string[ChPos];
  }

  NString &NString::Replace (t_size Pos1, t_size Num1, const TCHAR *Ptr)
  {
    m_string.replace (Pos1, Num1, Ptr);
    return *this;
  }

  NString &NString::Replace (t_size Pos1, t_size Num1, const TCHAR *Ptr, t_size Num2)
  {
    m_string.replace (Pos1, Num1, Ptr, Num2);
    return *this;
  }

  NString &NString::Replace (t_size Pos1, t_size Num1, const tstring &Str)
  {
    m_string.replace (Pos1, Num1, Str);
    return *this;
  }

  NString &NString::Replace (t_size Pos1, t_size Num1, const tstring &Str, t_size Pos2, t_size Num2)
  {
    m_string.replace (Pos1, Num1, Str, Pos2, Num2);
    return *this;
  }

  NString &NString::Replace (t_size Pos1, t_size Num1, const NString &Str)
  {
    m_string.replace (Pos1, Num1, Str.m_string);
    return *this;
  }

  NString &NString::Replace (t_size Pos1, t_size Num1, const NString &Str, t_size Pos2, t_size Num2)
  {
    m_string.replace (Pos1, Num1, Str.m_string, Pos2, Num2);
    return *this;
  }

  NString &NString::Replace (t_size Pos1, t_size Num1, t_size Count, TCHAR Ch)
  {
    m_string.replace (Pos1, Num1, Count, Ch);
    return *this;
  }

  void NString::Reverse()
  {
    NString rev;
    t_size l = Length();

    for (t_size i = l - 1; i >= 0; i--)
    {
      rev += m_string[i];
    }

    (*this) = rev;
  }

  NString &NString::SearchAndReplace (TCHAR ChOut, TCHAR ChIn)
  {
    for (t_size i = 0; i < Length(); i++)
      if (m_string[i] == ChOut)
        m_string[i] = ChIn;

    return *this;
  }

//! Return The last position of the substring suffix or -1 if it is not found.
  t_size NString::FindLastOccurence (const TCHAR &suffix) const
  {
    t_size start = 0;
    t_size pos = 0;

    do
    {
      pos = m_string.find (suffix, start);

      if (pos != tstring::npos)
        start = pos + 1;
    }
    while (pos != tstring::npos);

    return start - 1;
  }

//! Return The last position of the substring suffix or -1 if it is not found.
  t_size NString::FindLastOccurence (const TCHAR *suffix) const
  {
    t_size start = 0;
    t_size pos = 0;

    do
    {
      pos = m_string.find (suffix, start);

      if (pos != tstring::npos)
        start = pos + 1;
    }
    while (pos != tstring::npos);

    return start - 1;
  }

//! Return The last position of the substring suffix or -1 if it is not found.
  t_size NString::FindLastOccurence (const tstring &suffix) const
  {
    t_size start = 0;
    t_size pos = 0;

    do
    {
      pos = m_string.find (suffix, start);

      if (pos != tstring::npos)
        start = pos + 1;
    }
    while (pos != tstring::npos);

    return start - 1;
  }

//! Return The last position of the substring suffix or -1 if it is not found.
  t_size NString::FindLastOccurence (const NString &suffix) const
  {
    t_size start = 0;
    t_size pos = 0;

    do
    {
      pos = m_string.find (suffix.m_string, start);

      if (pos != tstring::npos)
        start = pos + 1;
    }
    while (pos != tstring::npos);

    return start - 1;
  }


//! Return the position of the first occurrence of the substring suffix or -1 if it is not found.
  t_size NString::FindFirstOccurence (const TCHAR &suffix)  const
  {
    t_size pos = 0;
    pos = m_string.find (suffix, pos);
    return (pos != tstring::npos) ? pos : -1;
  }

//! Return the position of the first occurrence of the substring suffix or -1 if it is not found.
  t_size NString::FindFirstOccurence (const TCHAR *suffix)  const
  {
    t_size pos = 0;
    pos = m_string.find (suffix, pos);
    return (pos != tstring::npos) ? pos : -1;
  }

//! Return the position of the first occurrence of the substring suffix or -1 if it is not found.
  t_size NString::FindFirstOccurence (const tstring &suffix)  const
  {
    t_size pos = 0;
    pos = m_string.find (suffix, pos);
    return (pos != tstring::npos) ? pos : -1;
  }

//! Return the position of the first occurrence of the substring suffix or -1 if it is not found.
  t_size NString::FindFirstOccurence (const NString &suffix) const
  {
    t_size pos = 0;
    pos = m_string.find (suffix.m_string, pos);
    return (pos != tstring::npos) ? pos : -1;
  }

//! Return the position of the first occurrence of the substring suffix or -1 if it is not found.
  t_size NString::FindNextOccurence (const TCHAR &suffix, t_size start) const
  {
    t_size pos = 0;
    pos = m_string.find (suffix, start);
    return (pos != tstring::npos) ? pos : -1;
  }

//! Return the position of the first occurrence of the substring suffix or -1 if it is not found.
  t_size NString::FindNextOccurence (const TCHAR *suffix, t_size start) const
  {
    t_size pos = 0;
    pos = m_string.find (suffix, start);
    return (pos != tstring::npos) ? pos : -1;
  }

//! Return the position of the first occurrence of the substring suffix or -1 if it is not found.
  t_size NString::FindNextOccurence (const tstring &suffix, t_size start) const
  {
    t_size pos = 0;
    pos = m_string.find (suffix, start);
    return (pos != tstring::npos) ? pos : -1;
  }

//! Return the position of the first occurrence of the substring suffix or -1 if it is not found.
  t_size NString::FindNextOccurence (const NString &suffix, t_size start) const
  {
    t_size pos = 0;
    pos = m_string.find (suffix.m_string, start);
    return (pos != tstring::npos) ? pos : -1;
  }

//! Searches through a string for the first character that matches any element of a specified string. Return -1 if it is not found.
  t_size NString::FindFirstOccurenceOf (const TCHAR &str) const
  {
    t_size pos = 0;
    pos = m_string.find_first_of (str, pos);
    return (pos != tstring::npos) ? pos : -1;
  }
//! Searches through a string for the first character that matches any element of a specified string. Return -1 if it is not found.
  t_size NString::FindFirstOccurenceOf (const TCHAR *str) const
  {
    t_size pos = 0;
    pos = m_string.find_first_of (str, pos);
    return (pos != tstring::npos) ? pos : -1;
  }
//! Searches through a string for the first character that matches any element of a specified string. Return -1 if it is not found.
  t_size NString::FindFirstOccurenceOf (const tstring &str) const
  {
    t_size pos = 0;
    pos = m_string.find_first_of (str, pos);
    return (pos != tstring::npos) ? pos : -1;
  }
//! Searches through a string for the first character that matches any element of a specified string. Return -1 if it is not found.
  t_size NString::FindFirstOccurenceOf (const NString &str) const
  {
    t_size pos = 0;
    pos = m_string.find_first_of (str.m_string, pos);
    return (pos != tstring::npos) ? pos : -1;
  }

//! Searches through a string for the last character that matches any element of a specified string. Return -1 if it is not found.
  t_size NString::FindLastOccurenceOf (const TCHAR &str) const
  {
    t_size pos = 0;
    pos = m_string.find_last_of (str, pos);
    return (pos != tstring::npos) ? pos : -1;
  }
//! Searches through a string for the last character that matches any element of a specified string. Return -1 if it is not found.
  t_size NString::FindLastOccurenceOf (const TCHAR *str) const
  {
    t_size pos = 0;
    pos = m_string.find_last_of (str, pos);
    return (pos != tstring::npos) ? pos : -1;
  }
//! Searches through a string for the last character that matches any element of a specified string. Return -1 if it is not found.
  t_size NString::FindLastOccurenceOf (const tstring &str) const
  {
    t_size pos = 0;
    pos = m_string.find_last_of (str, pos);
    return (pos != tstring::npos) ? pos : -1;
  }
//! Searches through a string for the last character that matches any element of a specified string. Return -1 if it is not found.
  t_size NString::FindLastOccurenceOf (const NString &str) const
  {
    t_size pos = 0;
    pos = m_string.find_last_of (str.m_string, pos);
    return (pos != tstring::npos) ? pos : -1;
  }

  t_size NString::Find (NString str, int start)
  {
    t_size pos = m_string.find (str.m_string, start);
    return (pos != tstring::npos) ? pos : -1;
  }

  t_size NString::Find (TCHAR c, int start)
  {
    t_size pos = m_string.find (c, start);
    return (pos != tstring::npos) ? pos : -1;
  }

  bool NString::IsSuffix (const TCHAR &suffix)
  {
    t_size l = m_string.length() - 1;

    if (l < 0)
      return FALSE;

    t_size pos = FindLastOccurence (suffix);

    if (pos == tstring::npos)
      return FALSE;

    return (pos == l);
  }

  bool NString::IsSuffix (const TCHAR *suffix)
  {
    t_size sl = StringLength (suffix);

    if (sl == 0)
      return FALSE;

    t_size l = m_string.length() - sl;

    if (l < 0)
      return FALSE;

    t_size pos = FindLastOccurence (suffix);

    if (pos == tstring::npos)
      return FALSE;

    return (pos == l);
  }

//! Return True is the the string is terminated by the tstring 'suffix'
  bool NString::IsSuffix (const tstring &suffix)
  {
    t_size sl = suffix.length();

    if (sl == 0)
      return FALSE;

    t_size l = m_string.length() - sl;

    if (l < 0)
      return FALSE;

    t_size pos = FindLastOccurence (suffix);

    if (pos == tstring::npos)
      return FALSE;

    return (pos == l);
  }

//! Return True is the the string is terminated by the NString 'suffix'
  bool NString::IsSuffix (const NString &suffix)
  {
    t_size sl = suffix.Length();

    if (sl == 0)
      return FALSE;

    t_size l = m_string.length() - sl;

    if (l < 0)
      return FALSE;

    t_size pos = FindLastOccurence (suffix);

    if (pos == tstring::npos)
      return FALSE;

    return (pos == l);
  }

//! Return True if the string start with the character contained in prefix
  bool NString::IsPrefix (const TCHAR &prefix)
  {
    t_size l = m_string.length() - 1;

    if (l < 0)
      return FALSE;

    t_size pos = FindFirstOccurence (prefix);

    if (pos == tstring::npos)
      return FALSE;

    return (pos == 0);
  }

//! Return True if the string start with the character string contained in prefix
  bool NString::IsPrefix (const TCHAR *prefix)
  {
    t_size sl = StringLength (prefix);

    if (sl == 0)
      return FALSE;

    t_size pos = FindFirstOccurence (prefix);

    if (pos == tstring::npos)
      return FALSE;

    return (pos == 0);
  }

//! Return True if the string start with the tstring contained in prefix
  bool NString::IsPrefix (const tstring &prefix)
  {
    t_size sl = prefix.length();

    if (sl == 0)
      return FALSE;

    t_size pos = FindFirstOccurence (prefix);

    if (pos == tstring::npos)
      return FALSE;

    return (pos == 0);
  }
//! Return True if the string start with the NString contained in prefix
  bool NString::IsPrefix (const NString &prefix)
  {
    t_size sl = prefix.Length();

    if (sl == 0)
      return FALSE;

    t_size pos = FindFirstOccurence (prefix);

    if (pos == tstring::npos)
      return FALSE;

    return (pos == 0);
  }

//! Return an NString without the character contained in suffix
  void NString::RemoveSuffix (const TCHAR &suffix)
  {
    if (IsSuffix (suffix) )
    {
      t_size pos = FindLastOccurence (suffix);
      *this = NString (m_string.substr (0, pos) );
    }
  }

//! Return an NString without the character string 'suffix'
  void NString::RemoveSuffix (const TCHAR *suffix)
  {
    if (IsSuffix (suffix) )
    {
      t_size pos = FindLastOccurence (suffix);
      *this = NString (m_string.substr (0, pos) );
    }
  }

//! Return an NString without the tstring 'suffix'
  void NString::RemoveSuffix (const tstring &suffix)
  {
    if (IsSuffix (suffix) )
    {
      t_size pos = FindLastOccurence (suffix);
      *this = NString (m_string.substr (0, pos) );
    }
  }

//! Return an NString without the NString 'suffix'
  void NString::RemoveSuffix (const NString &suffix)
  {
    if (IsSuffix (suffix) )
    {
      t_size pos = FindLastOccurence (suffix);
      *this = NString (m_string.substr (0, pos) );
    }
  }

//! Return an NString striped out of the prefix contained in prefix
  void NString::RemovePrefix (const TCHAR &prefix)
  {
    if (IsPrefix (prefix) )
    {
      *this = NString (m_string.substr (1) );
    }
  }

//! Return an NString striped out of the prefix contained in prefix
  void NString::RemovePrefix (const TCHAR *prefix)
  {
    if (IsPrefix (prefix) )
    {
      t_size l = StringLength (prefix);
      *this = NString (m_string.substr (l) );
    }
  }

//! Return an NString striped out of the prefix contained in prefix
  void NString::RemovePrefix (const tstring &prefix)
  {
    if (IsPrefix (prefix) )
    {
      t_size l = prefix.length();
      *this = NString (m_string.substr (l) );
    }
  }

//! Return an NString striped out of the prefix contained in prefix
  void NString::RemovePrefix (const NString &prefix)
  {
    if (IsPrefix (prefix) )
    {
      t_size l = prefix.Length();
      *this = NString (m_string.substr (l) );
    }
  }

  NString NString::GetSubString (t_size count) const
  {
    nuxAssert (count >= 0);
    return NString (m_string.substr (0, count) );
  }

  NString NString::GetSubString (t_size start, t_size count) const
  {
    nuxAssert (start >= 0);
    nuxAssert (count >= 0);
    return NString (m_string.substr (start, count) );
  }

  NString NString::Mid (t_size count) const
  {
    return GetSubString (count);
  }

  NString NString::Mid (t_size start, t_size count) const
  {
    return GetSubString (start, count);
  }

  NString NString::Left (t_size N) const
  {
    if (N >= Length() )
      return *this;

    return GetSubString (0, N);
  }

  NString NString::Right (t_size N) const
  {
    if (N >= Length() )
      return *this;

    return GetSubString (Length() - N, N);
  }

  NString NString::Trim() const
  {
    return TrimLeft().TrimRight();
  }

  NString NString::TrimLeft() const
  {
    t_size n = 0;
    t_size L = Length() - 1;

    while (n <= L)
    {
      if (IsWhitespaceChar (m_string[n]) )
      {
        n++;
      }
      else
      {
        break;
      }
    }

    return GetSubString (n, Length() - n);
  }

  NString NString::TrimRight() const
  {
    t_size L = Length() - 1;

    while (0 <= L)
    {
      if (IsWhitespaceChar (m_string[L]) )
      {
        L--;
      }
      else
      {
        break;
      }
    }

    return GetSubString (0, Length() + 1);
  }

  NString NString::TrimLeft (NString str) const
  {
    t_size n = 0;
    t_size L = Length();

    if (L == 0)
      return *this;

    while (n < L)
    {
      bool trim = false;
      t_size sz = str.Length();

      for (t_size i = 0; i < sz; i++)
      {
        if (m_string[n] == str[i])
        {
          trim = true;
          break;
        }
      }

      if (trim)
      {
        n++;
      }
      else
      {
        break;
      }
    }

    return GetSubString (n, Length() - n);
  }

  NString NString::TrimRight (NString str) const
  {
    t_size L = Length();

    if (L == 0)
      return *this;

    L = L - 1;

    while (0 <= L)
    {
      bool trim = false;
      t_size sz = str.Length();

      for (t_size i = 0; i < sz; i++)
      {
        if (m_string[L] == str[i])
        {
          trim = true;
          break;
        }
      }

      if (trim)
      {
        L--;
      }
      else
      {
        break;
      }
    }

    return GetSubString (0, L + 1);
  }

  const TCHAR *NString::operator () () const
  {
    if (Size() == 0)
      return 0;

    return GetTCharPtr();
  }

  const TCHAR *NString::operator * () const
  {
    if (Size() == 0)
      return 0;

    return GetTCharPtr();
  }

// NString::operator const TCHAR*() const
// {
//     return (const TCHAR*)GetTCharPtr();
// }

  NString &NString::operator += (const TCHAR &sufix)
  {
    m_string += sufix;
    return *this;
  }

  NString &NString::operator += (const TCHAR *sufix)
  {
    m_string += sufix;
    return *this;
  }

  NString &NString::operator += (const tstring sufix)
  {
    m_string += sufix;
    return *this;
  }

  NString &NString::operator += (const NString sufix)
  {
    m_string += sufix.m_string;
    return *this;
  }

  void NString::SplitAtFirstOccurenceOf (const TCHAR *SplitString, NString &Left, NString &Right)
  {
    t_size start = FindFirstOccurence (SplitString);

    if (start != tstring::npos)
    {
      t_size size = StringLength (SplitString);
      Left = GetSubString (0, start);
      Right = GetSubString (start + size, Length() - (start + size) );
    }
    else
    {
      Left = *this;
      Right = "";
    }
  }

  void NString::SplitAtFirstOccurenceOf (const TCHAR &SplitChar, NString &Left, NString &Right)
  {
    SplitAtFirstOccurenceOf (NString (SplitChar), Left, Right);
  }

  void NString::SplitAtFirstOccurenceOf (const NString &SplitString, NString &Left, NString &Right)
  {
    SplitAtFirstOccurenceOf (SplitString.GetTCharPtr(), Left, Right);
  }

  void NString::SplitAtLastOccurenceOf (const TCHAR *SplitString, NString &Left, NString &Right)
  {
    t_size start = FindLastOccurence (SplitString);

    if (start != tstring::npos)
    {
      t_size size = StringLength (SplitString);
      Left = GetSubString (0, start);
      Right = GetSubString (start + size, Length() - (start + size) );
    }
    else
    {
      Left = *this;
      Right = "";
    }
  }

  void NString::SplitAtLastOccurenceOf (const TCHAR &SplitChar, NString &Left, NString &Right)
  {
    SplitAtLastOccurenceOf (NString (SplitChar), Left, Right);
  }

  void NString::SplitAtLastOccurenceOf (const NString &SplitString, NString &Left, NString &Right)
  {
    SplitAtLastOccurenceOf (SplitString.GetTCharPtr(), Left, Right);
  }

  void NString::ParseToArray (std::vector<NString>& StringArray, const NString &delimiter)
  {
    NString Left;
    NString Right;
    NString Temp;
    SplitAtFirstOccurenceOf (delimiter, Left, Temp);

    if (Left.Size() )
    {
      Left = Left.Trim();
      StringArray.push_back (Left);
    }

    Right = Temp;

    while (Right.Size() )
    {
      Right.SplitAtFirstOccurenceOf (delimiter, Left, Temp);

      if (Left.Size() )
      {
        Left = Left.Trim();
        StringArray.push_back (Left);
      }

      Right = Temp;
    }
  }

  TCHAR NString::GetFirstChar()
  {
    if (IsEmpty() )
      return 0;

    return m_string[0];
  }

  TCHAR NString::GetLastChar()
  {
    if (IsEmpty() )
      return 0;

    return m_string[Size()-1];
  }

  bool operator != (const NString &left, const NString &right)
  {
    return left.m_string != right.m_string;
  }
  bool operator == (const NString &left, const NString &right)
  {
    return left.m_string == right.m_string;
  }
  bool operator <  (const NString &left, const NString &right)
  {
    return left.m_string < right.m_string;
  }
  bool operator <= (const NString &left, const NString &right)
  {
    return left.m_string <= right.m_string;
  }
  bool operator >  (const NString &left, const NString &right)
  {
    return left.m_string > right.m_string;
  }
  bool operator >= (const NString &left, const NString &right)
  {
    return left.m_string >= right.m_string;
  }

  NString operator+ (const NString &left, const NString &right)
  {
    return NString (left.m_string + right.m_string);
  }

  NString operator+ (const NString &left, const TCHAR *right)
  {
    return NString (left.m_string + right);
  }

  NString operator+ (const NString &left, const TCHAR right)
  {
    return NString (left.m_string + right);
  }

  NString operator+ (const TCHAR *left, const NString &right)
  {
    return NString (left + right.m_string);
  }

  NString operator+ (const TCHAR left, const NString &right)
  {
    return NString (left + right.m_string);
  }

  tostream &operator << (tostream &o, const NString &s)
  {
    return o << s.m_string;
  }

  VARARG_BODY (NString, NString::Printf, const TCHAR *, VARARG_NONE)
  {
    t_u32  BufferSize  = 1024;
    TCHAR *Buffer      = NULL;
    t_size Result      = tstring::npos;

    while (Result == tstring::npos)
    {
      Buffer = (TCHAR *) Realloc (Buffer, BufferSize * sizeof (TCHAR) );
      GET_VARARGS_RESULT (Buffer, BufferSize, BufferSize - 1, Fmt, Result);
      BufferSize *= 2;
    };

    Buffer[Result] = 0;

    NString ResultString (Buffer);

    free (Buffer);

    return ResultString;
  }

}
