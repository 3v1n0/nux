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


#ifndef NPARSING_H
#define NPARSING_H

NAMESPACE_BEGIN

//! Sequentially parse a stream of characters for token.
/*!
    Sequentially parse a stream of chracters for token.
    If Stream starts with the named token, the pointer is advanced past the end of the token in the stream.
    and the function return TRUE. If not, the function return FALSE. If there are SPACE or TAB characters  
    at the start of the stream, they are simply ignored.

    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.

    @return TRUE if the token is found at the start of the stream (excluding space and tab characters)
*/
BOOL ParseCommand( const TCHAR** Stream, const TCHAR* Match );

//! Parse a DWORD after the named token.
/*!
    Parse a DWORD after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseDWORD( const TCHAR* Stream, const TCHAR* Match, DWORD& Value );

//! Parses a string of N character from a character stream after a named token.
/*!
    Parses a string from a character stream.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param  Value   [OUT]   Buffer containing the parsed value.
    @param  Size    [IN]    The size of the Value buffer, in characters.
    @param  MaxLen  [IN]    Max number of character to return.
    @return TRUE    if the token is found inside the stream.
*/
BOOL ParseTCHAR( const TCHAR* Stream, const TCHAR* Match, TCHAR* Value, INT Size, INT MaxLen );

//! Parse a BYTE after the named token.
/*!
    Parse a BYTE after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param  Value   [OUT]   Buffer containing the parsed value.
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseBYTE( const TCHAR* Stream, const TCHAR* Match, BYTE& Value );
//! Parse a signed BYTE after the named token.
/*!
    Parse a signed BYTE after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param  Value parsed signed BYTE
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseSBYTE( const TCHAR* Stream, const TCHAR* Match, SBYTE& Value );
//! Parse a WORD after the named token.
/*!
    Parse a WORD after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param Value parsed WORD
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseWORD( const TCHAR* Stream, const TCHAR* Match, WORD& Value );
//! Parse a signed WORD after the named token.
/*!
    Parse a signed WORD after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param Value parsed signed WORD
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseSWORD( const TCHAR* Stream, const TCHAR* Match, SWORD& Value );
//! Parse a floating point value after the named token.
/*!
    Parse a floating point value after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param Value parsed floating point value
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseFLOAT( const TCHAR* Stream, const TCHAR* Match, float& Value );
//! Parse a double WORD after the named token.
/*!
    Parse a double WORD after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param Value parsed double WORD
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseINT( const TCHAR* Stream, const TCHAR* Match, INT& Value );
//! Parse a NString after the named token.
/*!
    Parse a NString after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param Value parsed NString
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseFString( const TCHAR* Stream, const TCHAR* Match, NString& Value );
//! Parse a QUADWORD after the named token.
/*!
    Parse a QUADWORD after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param Value parsed QUADWORD
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseQWORD( const TCHAR* Stream, const TCHAR* Match, QWORD& Value );
//! Parse a SIGNED QUADWORD after the named token.
/*!
    Parse a SIGNED QUADWORD after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param Value parsed SIGNED QUADWORD
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseSQWORD( const TCHAR* Stream, const TCHAR* Match, SQWORD& Value );
//! Parse a BOOLEAN after the named token.
/*!
    Parse a BOOLEAN after the named token.
    @param  Stream  [IN]    Character stream to search.
    @param  Match   [IN]    Token to match.
    @param Value parsed BOOLEAN value
    @return TRUE if the token is found inside the stream.
*/
BOOL ParseUBOOL( const TCHAR* Stream, const TCHAR* Match, BOOL& OnOff );

//! Parse a line of N characters and advance the pointer.
/*!
    Parse a line of MaxLen characters or up to but not including CR/LF.
    @param  Stream  [IN]    Character stream to search.
    @param Result parsed characters
    @param MaxLen maximum number of characters to parse
    @param Exact    If true, advanced the pointer after the first CR/LF character at the end of the string.
                    If FALSE advanced the pointer past all the CR/LF character at the end of the string.
    @return 0 if Ok, nonzero if at the end of stream and returning a 0-length string
*/

BOOL ParseLine( const TCHAR** Stream, TCHAR* Result, INT MaxLen, BOOL Exact=0 );
//! Parse a NString and advance the pointer.
/*!
    Parse a NString.
    @param  Stream  [IN]    Character stream to search.
    @param Result parsed NString
    @param Exact    If true, advanced the pointer after the first CR/LF character at the end of the string.
                    If FALSE advanced the pointer past all the CR/LF character at the end of the string.
    @return 0 if Ok, nonzero if at the end of stream and returning a 0-length string
*/
BOOL ParseLine( const TCHAR** Stream, NString& Resultd, BOOL Exact=0 );
//! Parse the next space-delimited string from the input stream. If quoted, gets entire quoted string.
/*!
    Parse the next space-delimited string from the input stream.
    @param Str stream of characters to search
    @param Result parsed stream of characters
    @param MaxLen maximum number of characters to parse
    @param UseEscape
    @return 0 if Ok, nonzero otherwise
*/
BOOL ParseToken( const TCHAR*& Str, TCHAR* Result, INT MaxLen, BOOL UseEscape );
//! Parse the next space-delimited string from the input stream. If quoted, gets entire quoted string.
/*!
    Parse the next space-delimited string from the input stream.
    @param Str stream of characters to search
    @param NString parsed stream of characters
    @param UseEscape 
    @return 0 if Ok, nonzero otherwise
*/
BOOL ParseToken( const TCHAR*& Str, NString& Arg, BOOL UseEscape );
//! Parse the next space-delimited string from the input stream. If quoted, gets entire quoted string.
/*!
    Parse the next space-delimited string from the input stream.
    @param Str stream of characters to search
    @param UseEscape 
    @return 0 if Ok, nonzero otherwise
*/
NString ParseToken( const TCHAR*& Str, BOOL UseEscape );

//! Get to next token, skipping comments and CR.
/*!
    Get to next token, skipping comments and CR.
    @param Stream stream of characters
*/
void ParseNext( const TCHAR** Stream );

//! Checks if a Token command-line parameter exists in the stream.
/*!
    Checks if a Token command-line parameter exists in the stream
    @param Stream stream of characters to search
    @param token to match
    @return True if found.
*/
BOOL ParseParam( const TCHAR* Stream, const TCHAR* Param );


NAMESPACE_END

#endif // NPARSING_H


