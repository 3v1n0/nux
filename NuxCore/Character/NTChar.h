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


#ifndef NTCHAR_H
#define NTCHAR_H

namespace nux { //NUX_NAMESPACE_BEGIN

inline TCHAR ToUpperCase( TCHAR c )
{
    return (c<TEXT('a') || c>TEXT('z')) ? (c) : (c+TEXT('A')-TEXT('a'));
}
inline TCHAR ToLowerCase( TCHAR c )
{
    return (c<TEXT('A') || c>TEXT('Z')) ? (c) : (c+TEXT('a')-TEXT('A'));
}
inline bool IsUpperCase( TCHAR c )
{
    return (c>=TEXT('A') && c<=TEXT('Z'));
}
inline bool IsLowerCase( TCHAR c )
{
    return (c>=TEXT('a') && c<=TEXT('z'));
}
inline bool IsAlphaChar( TCHAR c )
{
    return (c>=TEXT('a') && c<=TEXT('z')) || (c>=TEXT('A') && c<=TEXT('Z'));
}
inline bool IsDigitChar( TCHAR c )
{
    return c>=TEXT('0') && c<=TEXT('9');
}
inline bool IsAlphanumericChar( TCHAR c )
{
    return (c>=TEXT('a') && c<=TEXT('z')) || (c>=TEXT('A') && c<=TEXT('Z')) || (c>=TEXT('0') && c<=TEXT('9'));
}
inline bool IsWhitespaceChar( TCHAR c )
{
    return c == TEXT(' ') || c == TEXT('\t');
}
inline bool IsLinebreakChar( TCHAR c )
{
    //@todo - support for language-specific line break characters
    return c == TEXT('\n');
}

/** Returns nonzero if character is a space character. */
inline bool IsSpaceChar( TCHAR c )
{
#ifdef NUX_UNICODE
    return( std::iswspace(c) != 0 );
#else
    return( std::isspace(c) != 0 );
#endif
}

} //NUX_NAMESPACE_END

#endif // NTCHAR_H
