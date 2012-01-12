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


#ifndef SYSTEMTYPES_H
#define SYSTEMTYPES_H

#include <cstddef>

namespace nux
{
// Note: do not use long: long is 64 bits in LP64 while it remains 32 bits on LLP64

  typedef unsigned char           t_u8, t_byte, t_uchar;              // 0 to 255
  typedef char                    t_char;                       // –128 to 127
// signed char is a distinct type. See ANSI C Draft Standard and the keyword "signed"
  typedef signed char             t_s8, t_schar;                            // –128 to 127

  typedef void                    t_void, *t_pvoid;

  typedef unsigned char           t_UTF8;
  typedef unsigned int            t_UTF32;

  typedef unsigned int            t_u32; // TODO: to be removed soon.

#ifdef _WIN32
  typedef wchar_t             t_UTF16;
#elif defined(__linux__)
  typedef wchar_t             t_UTF16;
#elif defined (__APPLE__)
  typedef wchar_t             t_UTF16;
#else
#error t_UTF16 is undefined for this platform.
#endif

  const t_s8  t_s8_min                =  -128;
  const t_s8  t_s8_max                =  127;
  const t_u8  t_u8_min                =  0;
  const t_u8  t_u8_max                =  255;
  const short t_s16_min               =  -32768;
  const short t_s16_max               =  32767;
  const unsigned short t_u16_min      =  0;
  const unsigned short t_u16_max      =  65535;
  const int t_s32_min                 =  0x80000000;
  const int t_s32_max                 =  0x7FFFFFFF;
  const unsigned int t_u32_min        =  0x00000000;
  const unsigned int t_u32_max        =  0xFFFFFFFF;
  const long long t_s64_min           =  0x8000000000000000LL;
  const long long t_s64_max           =  0x7FFFFFFFFFFFFFFFLL;
  const unsigned long long t_u64_min  =  0x0000000000000000ULL;
  const unsigned long long t_u64_max  =  0xFFFFFFFFFFFFFFFFULL;

  const float t_f32_min               =  1.175494351E-38F;
  const float t_f32_max               =  3.402823466E+38F;
  const double t_f64_min              =  2.2250738585072014E-308;
  const double t_f64_max              =  1.7976931348623158E+308;

  const float MinFloat                =  1.175494351E-38F;
  const float MaxFloat                =  3.402823466E+38F;
  const double MinDouble              =  2.2250738585072014E-308;
  const double MaxDouble              =  1.7976931348623158E+308;

//            ILP32 LP64    LLP64   ILP64
// char       8     8       8       8
// short      16    16      16      16
// int        32    32      32      64
// long       32    64      32      64
// long long  64    64      64      64
// pointer    32    64      64      64

// Window 64-bit supports LLP64
// Linux 64 bit  supports LP64
// Mac OS 64 bit  supports LP64

// The size of  a long integer in particular depends on the operating system and the targeted architecture as follows:
//
// OS            arch           size
// Windows       IA-32        4 bytes
// Windows       Intel 64     4 bytes
// Windows       IA-64        4 bytes
// Linux         IA-32        4 bytes
// Linux         Intel 64     8 bytes
// Linux         IA-64        8 bytes
// Mac OS X      IA-32        4 bytes
// Mac OS X      Intel 64     8 bytes

#define NUX_BYTE_SIZE   1
#define NUX_WORD_SIZE   2
#define NUX_FLOAT_SIZE  4
#define NUX_INT_SIZE    4
#define NUX_DOUBLE_SIZE 8

#define DEGTORAD(d) (d) * 3.1415927f / 180.0f
#define RADTODEG(d) (d) * 180.0f / 3.1415927f


}

#endif // SYSTEMTYPES_H
