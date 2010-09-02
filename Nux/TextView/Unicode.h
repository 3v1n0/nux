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


#ifndef UNICODE_H
#define UNICODE_H

NAMESPACE_BEGIN_GUI

// Define the basic types for storing Unicode 
// typedef unsigned long	t_UTF32;	
// typedef unsigned short	t_UTF16;	
// typedef unsigned char	t_UTF8;

// Some fundamental constants 
#define UNI_REPLACEMENT_CHAR (t_UTF32)0x0000FFFD
#define UNI_MAX_BMP			 (t_UTF32)0x0000FFFF
#define UNI_MAX_UTF16		 (t_UTF32)0x0010FFFF
#define UNI_MAX_UTF32		 (t_UTF32)0x7FFFFFFF
#define UNI_MAX_LEGAL_UTF32  (t_UTF32)0x0010FFFF

#define UNI_SUR_HIGH_START   (t_UTF32)0xD800
#define UNI_SUR_HIGH_END     (t_UTF32)0xDBFF
#define UNI_SUR_LOW_START    (t_UTF32)0xDC00
#define UNI_SUR_LOW_END      (t_UTF32)0xDFFF

#define SWAPWORD(val) (((t_UTF16)(val) << 8) | ((t_UTF16)(val) >> 8))

//
//	Conversions between UTF-8 and a single UTF-32 value
//
size_t	utf8_to_utf32(t_UTF8 *utf8str, size_t utf8len, t_UTF32 *pcp32);
size_t  utf32_to_utf8(t_UTF8 *utf8str, size_t utf8len, t_UTF32 ch32);

//
//	Conversions between UTF-16 and UTF-8 strings
//
size_t	utf8_to_utf16(t_UTF8 *utf8str,   size_t utf8len, t_UTF16 *utf16str, size_t *utf16len);
size_t  utf16_to_utf8(t_UTF16 *utf16str, size_t utf16len, t_UTF8 *utf8str,  size_t *utf8len);

//
//	Conversions between UTF-16 and UTF-32 strings
//
size_t  utf16_to_utf32(t_UTF16 *utf16str,   size_t utf16len, t_UTF32 *utf32str, size_t *utf32len);
size_t  utf32_to_utf16(t_UTF32 *utf32str,   size_t utf32len, t_UTF16 *utf16str, size_t *utf16len);
size_t	utf16be_to_utf32(t_UTF16 *utf16str, size_t utf16len, t_UTF32 *utf32str, size_t *utf32len);


//
//	Conversions between ASCII/ANSI and UTF-16 strings
//
size_t	ascii_to_utf16(t_UTF8 *asciistr,  size_t asciilen, t_UTF16 *utf16str, size_t *utf16len);
size_t  utf16_to_ascii(t_UTF16 *utf16str, size_t utf16len, t_UTF8  *asciistr, size_t *asciilen);

//
//	Miscallaneaous
//
size_t	copy_utf8(t_UTF8 *src, size_t srclen, t_UTF8 *dest, size_t *destlen);
size_t	copy_utf16(t_UTF16 *src, size_t srclen, t_UTF16 *dest, size_t *destlen);
size_t	swap_utf16(t_UTF16 *src, size_t srclen, t_UTF16 *dest, size_t *destlen);

NAMESPACE_END_GUI

#endif // UNICODE_H
