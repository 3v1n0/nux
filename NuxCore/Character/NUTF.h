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


#ifndef NUTF_H
#define NUTF_H

// http://en.wikipedia.org/wiki/UTF-16

// In computing, UTF-16 (16-bit Unicode Transformation Format) is a variable-length character encoding
// for Unicode, capable of encoding the entire Unicode repertoire. The encoding form maps code points
// (characters) into a sequence of 16-bit words, called code units. For characters in the Basic
// Multilingual Plane (BMP) the resulting encoding is a single 16-bit word. For characters in the other
// planes, the encoding will result in a pair of 16-bit words, together called a surrogate pair. All possible
// code points from U+0000 through U+10FFFF, except for the surrogate code points U+D800–U+DFFF
// (which are not characters), are uniquely mapped by UTF-16 regardless of the code point's current or
// future character assignment or use.
//
// As many uses in computing require units of bytes (octets) there are three related encoding schemes
// which map to octet sequences instead of words: namely UTF-16, UTF-16BE, and UTF-16LE. They
// differ only in the byte order chosen to represent each 16-bit unit and whether they make use of a
// Byte Order Mark. All of the schemes will result in either a 2 or 4-byte sequence for any given character.
//
// UTF-16 is officially defined in Annex Q of the international standard ISO/IEC 10646-1. It is also
// described in The Unicode Standard version 3.0 and higher, as well as in the IETF's RFC 2781.
//
// UCS-2 (2-byte Universal Character Set) is an obsolete character encoding which is a predecessor
// to UTF-16. The UCS-2 encoding form is nearly identical to that of UTF-16, except that it does not
// support surrogate pairs and therefore can only encode characters in the BMP range U+0000 through
// U+FFFF. As a consequence it is a fixed-length encoding that always encodes characters into a
// single 16-bit value. As with UTF-16, there are three related encoding schemes (UCS-2, UCS-2BE, UCS-2LE)
// that map characters to a specific byte sequence.
//
// Because of the technical similarities and upwards compatibility from UCS-2 to UTF-16, the two
// encodings are often erroneously conflated and used as if interchangeable, so that strings encoded
// in UTF-16 are sometimes misidentified as being encoded in UCS-2.

namespace nux
{

//! Convert UTF-16 to UTF-8
  class NUTF8
  {
    // UTF-8 encoded characters may theoretically be up to six bytes long, however 16-bit BMP characters are only up to three bytes long.
  public:
    explicit NUTF8 (const UNICHAR *Source);
    explicit NUTF8 (const std::wstring &Source);
    ~NUTF8();

    operator const char* ();

  private:
    void Convert (const UNICHAR *);
    //void Convert(const t_UTF32*);
    char *utf8;

  };

//! Convert UTF-8 to UTF-16
  class NUTF16
  {
  public:
    explicit NUTF16 (const char *Source);
    explicit NUTF16 (const std::string &Source);
    ~NUTF16();

    operator const UNICHAR* ();

  private:
    void Convert (const char *);
    UNICHAR *unicode;

  };

}

#endif // NUTF_H
