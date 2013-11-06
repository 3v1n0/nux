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


#ifndef TEXTSTRING_H
#define TEXTSTRING_H

#include "Math/MathUtility.h"

namespace nux
{

  class TCharToUpperCase
  {
    TCHAR *UpperCaseString;
    TCharToUpperCase();

  public:
    /*!
        Converts the data by using the Convert() method on the base class.
    */
    explicit inline TCharToUpperCase (const TCHAR *Source)
    {
      if (Source != NULL)
      {
        UpperCaseString = _tcsdup (Source); // Duplicate string. Allocated with malloc.

        size_t i = 0;

        while (UpperCaseString[i])
        {
          UpperCaseString[i] = _totupper (UpperCaseString[i]); // Conversion is done in place.
          ++i;
        }
      }
      else
      {
        UpperCaseString = NULL;
      }
    }

    inline ~TCharToUpperCase()
    {
      if (UpperCaseString != NULL)
      {
        std::free (UpperCaseString);
      }
    }

    inline operator const TCHAR* () const
    {
      return UpperCaseString;
    }

    inline operator TCHAR* () const
    {
      return UpperCaseString;
    }
  };

  class TCharToLowerCase
  {
    TCHAR *LowerCaseString;
    TCharToLowerCase();

  public:
    /*!
    Converts the data by using the Convert() method on the base class.
    */
    explicit inline TCharToLowerCase (const TCHAR *Source)
    {
      if (Source != NULL)
      {
        LowerCaseString = _tcsdup (Source); // Duplicate string. Allocated with malloc.

        size_t i = 0;

        while (LowerCaseString[i])
        {
          LowerCaseString[i] = _totlower (LowerCaseString[i]); // Conversion is done in place.
          ++i;
        }
      }
      else
      {
        LowerCaseString = NULL;
      }
    }

    inline ~TCharToLowerCase()
    {
      if (LowerCaseString != NULL)
      {
        std::free (LowerCaseString);
      }
    }

    inline operator const TCHAR* () const
    {
      return LowerCaseString;
    }

    inline operator TCHAR* () const
    {
      return LowerCaseString;
    }
  };

////////////////////////////////////////////////////////////////////////////////////

  template<typename T>
  T ToLowerCase (T c)
  {
    // Valid for Basic Latin characters in ASCII, UTF8 or Unicode.
    if (c >= 'A' && c <= 'Z')
      c += 32;

    return c;
  }

  template<typename T>
  struct ToLowerCaseFunctor
  {
    T operator() (T value)
    {
      return ToLower (value);
    }
  };

  template<typename T>
  T ToUpperCase (T c)
  {
    // Valid for Basic Latin characters in ASCII, UTF8 or Unicode.
    if (c >= 'a' && c <= 'z')
      c -= 32;

    return c;
  }

  template<typename T>
  struct ToUpperCaseFunctor
  {
    T operator() (T value)
    {
      return ToUpperCase (value);
    }
  };

  /*!
      Compare two char or TCHAR.
      @return 0 if equal, -1 if(lhs<rhs), +1 if(lhs>rhs)
  */
  template<typename T>
  struct LexicographicCompare
  {
    unsigned int operator() (const T &lhs, const T &rhs)
    {
      if (lhs < rhs) return -1;

      if (rhs < lhs) return 1;

      return 0;
    }
  };

  /*!
      Compare two char or TCHAR. Apply the conversion functor before the comparison.
      @return 0 if equal, -1 if(lhs<rhs), +1 if(lhs>rhs)
  */
  template<typename T, typename ConversionFunctor>
  struct LexicographicCompareWithConversion
  {
    LexicographicCompareWithConversion() {}
    unsigned int operator() (const T &lhs, const T &rhs)
    {
      T newlhs = m_ConversionFunctor (lhs);
      T newrhs = m_ConversionFunctor (rhs);

      if (newlhs < newrhs) return -1;

      if (newrhs < newlhs) return 1;

      return 0;
    }
    ConversionFunctor m_ConversionFunctor;
  };

  /*!
      Compare two strings of char or TCHAR. Comparison is dictated by the functor parameter.
      @param lhs String input
      @param rhs String input
      @param functor The comparison operator
      @return 0, -1, +1 according to the comparison functor
  */
  template<typename T, typename U>
  static int StringCompareWithOperator (const T *lhs, const T *rhs, U functor)
  {
    nuxAssert (lhs);
    nuxAssert (rhs);
    int result = 0;

    while ( (*lhs || *rhs) && !result)
    {
      result = functor (* (lhs++), * (rhs++) );
    }

    return result;
  }

  /*!
      Compare the first N characters of two strings of char or TCHAR (case insensitive). Comparison is dictated by the functor parameter.
      @param lhs String input
      @param rhs String input
      @param numCharToCompare Number of characters to compare
      @param functor The comparison operator
      @return 0, -1, +1 according to the comparison functor
  */
  template<typename T, typename U>
  static int StringCompareWithOperator (const T *lhs, const T *rhs, unsigned int numCharToCompare, U functor)
  {
    nuxAssert (lhs);
    nuxAssert (rhs);

    int result = 0;

    while ( (*lhs || *rhs) && !result && numCharToCompare)
    {
      result = functor (* (lhs++), * (rhs++) );
      --numCharToCompare;
    }

    return result;
  }

  /*!
      Compare two strings of char or TCHAR for equality.
      @param lhs String input
      @param rhs String input
      @return 0 if equal, -1 if(lhs<rhs), +1 if(lhs>rhs)
  */
  template<typename T>
  static int StringCompare (const T *lhs, const T *rhs)
  {
    return StringCompareWithOperator (lhs, rhs, LexicographicCompare<T>() );
  }

  /*!
      Compare the first N characters of two strings of char or TCHAR for equality.
      @param lhs String input
      @param rhs String input
      @return 0 if equal, -1 if(lhs<rhs), +1 if(lhs>rhs)
  */
  template<class T>
  static int StringCompare (const T *lhs, const T *rhs, unsigned int len)
  {
    return StringCompareWithOperator (lhs, rhs, len, LexicographicCompare<T>() );
  }


  /*!
      Compare two strings of char or TCHAR for equality (case insensitive).
      @param lhs String input
      @param rhs String input
      @return 0 if equal, -1 if(lhs<rhs), +1 if(lhs>rhs)
  */
  template<typename T>
  static int StringCompareCaseInsensitive (const T *lhs, const T *rhs)
  {
    return StringCompareWithOperator (lhs, rhs, LexicographicCompareWithConversion<T, ToLowerCaseFunctor<T> >() );
  }


  /*!
  Copy one string to another without controls of buffer size for the destination. DANGEROUS!
  @param src Source string
  @param dest Destination string
  @return The number of char copied
  */
  template<typename T>
  static unsigned int StringCopy (T *dest, const T *src)
  {
    nuxAssert (src);
    nuxAssert (dest);

    const T *origin = src;

    while (*src)
    {
      * (dest++) = * (src++);
    }

    *dest = 0;
    return (unsigned int) (src - origin);
  }

  /*!
  Copy one string to another with controls of buffer size for the destination.
  @param src Source string
  @param dest Destination string
  @return The number of char copied
  */
  template<typename T>
  static unsigned int StringCopy (T *dest, unsigned int bufferSize, const T *src, unsigned int lengthToCopy = 0xFFFFFFFF)
  {
    nuxAssert (src);
    nuxAssert (dest);
    nuxAssert (bufferSize);

    const T *origin = src;
    lengthToCopy = Min (lengthToCopy, bufferSize - 1);
    T *MaxNullCharPosition = dest + bufferSize - 1;

    // Copy src to dst
    while (*src && lengthToCopy)
    {
      * (dest++) = * (src++);
      --lengthToCopy;
    }

    // Terminate string with Null char
    unsigned int NumCharCopied = (unsigned int) (src - origin);

    if (dest < MaxNullCharPosition)
    {
      *dest = 0;
    }
    else
    {
      *MaxNullCharPosition = 0;

      if (NumCharCopied)
        NumCharCopied = NumCharCopied - 1;
    }

    return NumCharCopied;
  }

  /*!
  Find a string inside a char or TCHAR string. The search is done according to the functor operator U
  @param src Search inside this string of characters
  @param str The piece to search for
  @param U The comparison functor
  @return A pointer to the beginning of the searched string. Null otherwise.
  */
  template<typename T, typename U>
  static T *FindString (const T *src, const T *str, U functor)
  {
    const T *original = str;

    nuxAssert (src);
    nuxAssert (str);

    while (*src)
    {
      if (functor (*src, *str) )
      {
        if (* (++str) == 0)
        {
          return (T *) src - (str - original) + 1;
        }
      }
      else
      {
        str = original;
      }

      ++src;
    }

    return 0;
  }

  template<typename T>
  struct EqualFunctor
  {
    bool operator() (const T &lhs, const T &rhs)
    {
      return lhs == rhs;
    }
  };

  template<typename T>
  static T *FindString (const T *src, const T *str)
  {
    return FindString (src, str, EqualFunctor<T>() );
  }

//! Returns a TCHAR length.
  template<class T>
  static size_t StringLength (const T *s)
  {
    nuxAssert (s);
    const T *end = s;

    while (*end)
    {
      ++end;
    }

    return (size_t) (end - s);
  }

  template<class T>
  size_t ToCharString (char *buffer, size_t bufferlen, const char *format, T value)
  {
    nuxAssert (bufferlen);
    nuxAssert (buffer);

#if defined(NUX_OS_WINDOWS)
    int res = _snprintf_s (buffer, bufferlen, bufferlen - 1, format, value);
#elif defined(NUX_OS_LINUX)
    int res = snprintf (buffer, bufferlen - 1, format, value);
#endif

    nuxAssert (res > 0);
    nuxAssert (res < (int) bufferlen);
    return res;
  }

  template<class T>
  int ToTCharString (TCHAR *buffer, size_t bufferlen, const TCHAR *format, T value)
  {
    nuxAssert (bufferlen);
    nuxAssert (buffer);

#if defined(NUX_OS_WINDOWS)
    int res = _snwprintf_s (buffer, bufferlen, bufferlen - 1, format, value);
#elif defined(NUX_OS_LINUX)
    int res = snwprintf (buffer, bufferlen - 1, format, value);
#endif

    nuxAssert (res > 0);
    nuxAssert (res < (int) bufferlen);
    return res;
  }

  size_t ValueToLiteralString (char *buffer, size_t len, unsigned short     value);
  size_t ValueToLiteralString (char *buffer, size_t len, short     value);
  size_t ValueToLiteralString (char *buffer, size_t len, unsigned int     value);
  size_t ValueToLiteralString (char *buffer, size_t len, int     value);
  size_t ValueToLiteralString (char *buffer, size_t len, unsigned long   value);
  size_t ValueToLiteralString (char *buffer, size_t len, long    value);
  size_t ValueToLiteralString (char *buffer, size_t len, unsigned long long     value);
  size_t ValueToLiteralString (char *buffer, size_t len, long long     value);
  size_t ValueToLiteralString (char *buffer, size_t len, float   value);
  size_t ValueToLiteralString (char *buffer, size_t len, double  value);
  size_t ValueToLiteralString (char *buffer, size_t len, unsigned char      value);
  size_t ValueToLiteralString (char *buffer, size_t len, char      value);
  size_t ValueToLiteralString (char *buffer, size_t len, char   value);

  template<class T>
  bool FromCharString (const char *buffer, size_t bufferlen, const char *format, T &value)
  {
    nuxAssert (buffer);
    nuxAssert (bufferlen);

#if defined(NUX_OS_WINDOWS)
    size_t res = _snscanf_s (buffer, bufferlen, format, &value);
#elif defined(NUX_OS_LINUX)
    bufferlen = 0; // Stop unused param warning in release mode.
    bufferlen = bufferlen + (1 - 1); // Stop unused param warning in release mode.
    size_t res = sscanf (buffer, format, &value);
#endif

    nuxAssert (res != 0);
    return res != 0;
  }

  template<class T>
  bool FromTCharString (const TCHAR *buffer, size_t bufferlen, const TCHAR *format, T &value)
  {
    nuxAssert (buffer);
    nuxAssert (bufferlen);

#if defined(NUX_OS_WINDOWS)
    size_t res = _snwscanf_s (buffer, bufferlen, format, &value);
#elif defined(NUX_OS_LINUX)
    size_t res = swscanf (buffer, format, &value);
#endif

    nuxAssert (res != 0);
    return res != 0;
  }

  bool ValueFromLiteralString (const char *buffer, size_t len, unsigned short &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, short &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, unsigned int &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, int &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, unsigned long &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, long &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, unsigned long long &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, long long &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, float &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, double &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, unsigned char &value);
  bool ValueFromLiteralString (const char *buffer, size_t len, char &value);


  /*-----------------------------------------------------------------------------
  String functions.
  -----------------------------------------------------------------------------*/
// Copy Src into Dest. Check length of Dest. Dest is NULL terminated.
  TCHAR *Strncpy ( TCHAR *Dest, size_t Size, const TCHAR *Src, size_t Max);
  /** Concatenate a string with length checking. */
  TCHAR *Strncat ( TCHAR *Dest, size_t Size, const TCHAR *Src, size_t Max);

// Search a string inside a string. Return a pointer to the beginning of the searched string if it is found.
// Else, return NULL;
  const TCHAR *Strfind (const TCHAR *Str, const TCHAR *Find);



#ifdef WIN32_SECURE
  //! Copy a string. Return 0 if successful
  inline int inlTCharStringCopy ( TCHAR *Dest, size_t numberOfElements, const TCHAR *Src )
  {
    return _tcscpy_s ( Dest, numberOfElements, Src );
  }
  inline int inlCharStringCopy ( char *Dest, size_t numberOfElements, const char *Src )
  {
    return strcpy_s ( Dest, numberOfElements, Src );
  }

  //! Append a string. Return 0 if successful.
  inline int inlTCharStringConcat ( TCHAR *Dest, size_t numberOfElements, const TCHAR *Src )
  {
    return _tcscat_s ( Dest, numberOfElements, Src );
  }
  inline int inlCharStringConcat ( char *Dest, size_t numberOfElements, const char *Src )
  {
    return strcat_s ( Dest, numberOfElements, Src );
  }
  //! Convert a string to uppercase.  Returns a pointer to the altered string. Return 0 if not successful.
  inline int inlStrupr (TCHAR *String, size_t numberOfElements)
  {
    return _tcsupr_s ( String, numberOfElements );
  }
#else
  //! Copy a string.
  inline TCHAR *inlStringCopy (TCHAR *Dest, size_t /* numberOfElements */, const TCHAR *Src)
  {
    return _tcscpy ( Dest, Src );
  }
  //! Append a string.
  inline TCHAR *inlTCharStringConcat (TCHAR *Dest, size_t /* numberOfElements */, const TCHAR *Src)
  {
    return _tcscat ( Dest, Src );
  }
  //! Convert a string to uppercase. Returns a pointer to the altered string. Return 0 if not successful.
  inline TCHAR *inlStrupr (TCHAR *String, size_t /* numberOfElements */)
  {
    nuxAssert (String);

    if (String == 0)
      return NULL;

    size_t i = 0;

    while (String[i])
    {
      String[i] = _totupper (String[i]); // Conversion is done in place.
      ++i;
    }

    return String;
  }
#endif

//! Return a pointer to the first occurrence of a search string in a string.
  inline TCHAR *Strstr ( const TCHAR *String, const TCHAR *Find )
  {
    return (TCHAR *) _tcsstr ( String, Find );
  }
//! Find the first occurrence a character in a string. Returns a pointer to that character in the string or NULL if it is not found.
  inline TCHAR *Strchr ( const TCHAR *String, int c )
  {
    return (TCHAR *) _tcschr ( String, c );
  }
//! Find the last occurrence a character in a string. Returns a pointer to that character in the string or NULL if it is not found.
  inline TCHAR *Strrchr ( const TCHAR *String, int c )
  {
    return (TCHAR *) _tcsrchr ( String, c );
  }

//! Performs a case-sensitive comparison. The return value indicates the lexicographic relation of String1 to String2.
  inline int TCharStringCompare ( const TCHAR *String1, const TCHAR *String2 )
  {
    return _tcscmp ( String1, String2 );
  }
//! Performs a case-insensitive comparison. The return value indicates the lexicographic relation of String1 to String2.
  inline int Stricmp ( const TCHAR *String1, const TCHAR *String2 )
  {
    return _tcscmp ( (TCHAR *) TCharToLowerCase (String1), (TCHAR *) TCharToLowerCase (String2) );
  }


  inline int Strtoi ( const TCHAR *Start, TCHAR **End, int Base )
  {
    return _tcstoul ( Start, End, Base );
  }
  inline int TCharStringNCompare ( const TCHAR *A, const TCHAR *B, size_t Count )
  {
    return _tcsncmp ( A, B, Count );
  }
  inline int TCharStringNICompare ( const TCHAR *A, const TCHAR *B, size_t Count )
  {
    return _tcsncmp ( (TCHAR *) TCharToLowerCase (A), (TCHAR *) TCharToLowerCase (B), Count );
  }

//! Duplicate a null terminated string using new[]. The resulting string is NULL terminated. Use delete[] to destroy.
  TCHAR *Strdup (const TCHAR *str);

//! Duplicate a null terminated ANSICHAR string using new[]. The resulting string is NULL terminated. Use delete[] to destroy.
  ANSICHAR *StrdupA ( const ANSICHAR *str);

//! Duplicate a null terminated UNICHAR string using new[]. The resulting string is NULL terminated. Use delete[] to destroy.
  UNICHAR *StrdupU ( const UNICHAR *str);

//! Formatted Printf.
  VARARG_DECL ( int, static int, return, Snprintf, VARARG_NONE, const TCHAR *, VARARG_EXTRA (TCHAR *Dest) VARARG_EXTRA (int Size) VARARG_EXTRA (int Count), VARARG_EXTRA (Dest) VARARG_EXTRA (Size) VARARG_EXTRA (Count) );

  bool IsLastChar (const TCHAR *CharString, const TCHAR Chr);

}

#endif // TEXTSTRING_H

