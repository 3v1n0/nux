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


#ifndef NPARSING_H
#define NPARSING_H

namespace nux
{

//! Parse a stream of characters and look for a string of TCHAR at the start of the stream.
  /*!
      If the string is found, Stream points to the next character after the string.
      Space and tab characters at the beginning of the stream are ignored.
      @param  Stream  Character stream to search.
      @param  Match   Token to match.
      @return TRUE if the character string is found at the start of the stream (excluding space and tab characters).
  */
  bool ParseCommand (const TCHAR **Stream, const TCHAR *Match);

//! Parse a stream of characters and look for a t_32 value after the token string.
  /*!
      Parse a unsigned int after the named token.

      @param  stream  Character stream to search.
      @param  token   Token to match.
      @return True if the character string is found at the start of the stream (excluding space and tab characters).
  */
  bool Parse_u32 (const TCHAR *stream, const TCHAR *Match, DWORD &value);

//! Parses a string of N character from a character stream after a named token.
  /*!
      Parses a string from a character stream.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param  Value      Buffer containing the parsed value.
      @param  Size        The size of the Value buffer, in characters.
      @param  MaxLen      Max number of character to return.
      @return TRUE    if the token is found inside the stream.
  */
  bool Parse_tchar (const TCHAR *stream, const TCHAR *Match, TCHAR *value, int size, int MaxLen);

//! Parse a BYTE after the named token.
  /*!
      Parse a BYTE after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param  Value      Buffer containing the parsed value.
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_u8 (const TCHAR *Stream, const TCHAR *Match, BYTE &Value);
//! Parse a signed BYTE after the named token.
  /*!
      Parse a signed BYTE after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param  Value parsed signed BYTE
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_s8 (const TCHAR *Stream, const TCHAR *Match, SBYTE &Value);
//! Parse a WORD after the named token.
  /*!
      Parse a WORD after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param Value parsed WORD
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_u16 (const TCHAR *Stream, const TCHAR *Match, WORD &Value);
//! Parse a signed WORD after the named token.
  /*!
      Parse a signed WORD after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param Value parsed signed WORD
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_s16 (const TCHAR *Stream, const TCHAR *Match, SWORD &Value);
//! Parse a floating point value after the named token.
  /*!
      Parse a floating point value after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param Value parsed floating point value
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_float (const TCHAR *Stream, const TCHAR *Match, float &Value);
//! Parse a double WORD after the named token.
  /*!
      Parse a double WORD after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param Value parsed double WORD
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_int (const TCHAR *Stream, const TCHAR *Match, int &Value);
//! Parse a std::string after the named token.
  /*!
      Parse a std::string after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param Value parsed std::string
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_string (const TCHAR *Stream, const TCHAR *Match, std::string &Value);
//! Parse a QUADWORD after the named token.
  /*!
      Parse a QUADWORD after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param Value parsed QUADWORD
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_u64 (const TCHAR *Stream, const TCHAR *Match, QWORD &Value);
//! Parse a SIGNED QUADWORD after the named token.
  /*!
      Parse a SIGNED QUADWORD after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param Value parsed SIGNED QUADWORD
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_s64 (const TCHAR *Stream, const TCHAR *Match, SQWORD &Value);
//! Parse a BOOLEAN after the named token.
  /*!
      Parse a BOOLEAN after the named token.
      @param  Stream      Character stream to search.
      @param  Match       Token to match.
      @param Value parsed BOOLEAN value
      @return TRUE if the token is found inside the stream.
  */
  bool Parse_bool (const TCHAR *Stream, const TCHAR *Match, bool &OnOff);

//! Extract a line of Stream (everything up to, but not including, CR/LF).
  /*!
      @param Stream Character stream to search.
      @param LineBuffer The first line in Stream.
      @param BufferSize Size of LineBuffer.
      @param GoToNextLine If true, advanced the pointer after the first CR/LF character at the end of the string.
                          If FALSE advanced the pointer past all the CR/LF character at the end of the string.
      @return True i a line was copied to LineString;
  */
  bool ParseLine (const TCHAR **Stream, TCHAR *LineBuffer, int BufferSize);

//! Extract a line of Stream (everything up to, but not including, CR/LF).
  /*!
      @param Stream Character stream to search.
      @param LineString The first line in Stream.
      @param GoToNextLine If true, advanced the pointer after the first CR/LF character at the end of the string.
                          If FALSE advanced the pointer past all the CR/LF character at the end of the string.
      @return True i a line was copied to LineString;
  */
  bool ParseLine (const TCHAR **Stream, std::string &LineString);

//! Parse the next space-delimited string from the input stream. If the next token starts with a quote, gets entire quoted string.
  /*!
      @param Str stream of characters to search.
      @param TokenBuffer The parsed token string.
      @param BufferSize Size of the TokenBuffer.
      @return True if a token was found.
  */
  bool ParseToken (const TCHAR *Str, TCHAR *TokenBuffer, int BufferSize);

//! Parse the next space-delimited string from the input stream. If the next token starts with a quote, gets entire quoted string.
  /*!
      @param Str stream of characters to search.
      @param TokenBuffer The parsed token string.
      @return True if a token was found.
  */
  bool ParseToken (const TCHAR *Str, std::string &TokenString);

//! Parse the next space-delimited string from the input stream. If the next token starts with a quote, gets entire quoted string.
  /*!
      @param Str stream of characters to parse.
      @return The next token in a std::string.
  */
  std::string ParseToken (const TCHAR *Str);

//! Go to the next token in the stream.
  /*!
      Skip tabs, and space at the beginning of each line.
      Skip everything on a line that follows the given comment char token
      If there is a valid token in the input, Stream will point to it.

      @param Stream stream of characters
      @param CommentChar Character representing the beginning of a comment on a line. ie ';' '#".
  */
  void ParseToNextLine (const TCHAR **Stream, TCHAR CommentChar);

//! Checks if a Token command-line parameter exists in the stream.
  /*!
      Checks if a Token command-line parameter exists in the stream
      @param Stream stream of characters to search
      @param token to match
      @return True if found.
  */
  bool ParseParam (const TCHAR *Stream, const TCHAR *Param);


}

#endif // NPARSING_H


