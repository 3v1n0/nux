/*
 * Copyright 2010 InalogicÂ® Inc.
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

  typedef unsigned short          t_u16, t_uint16, t_ushort, t_word;  // 0 to 65,535
  typedef short                   t_s16, t_int16, t_short;            // –32,768 to 32,767

  typedef unsigned int            t_u32, t_uint32, t_uint, t_dword;   // 0 to 4,294,967,295
  typedef int                     t_s32, t_int32, t_int;              // –2,147,483,648 to 2,147,483,647

  typedef unsigned long           t_ulong;                            // 0 to 4,294,967,295
  typedef long                    t_long;                             // –2,147,483,648 to 2,147,483,647

  typedef unsigned long long      t_u64, t_uint64, t_qword, t_ulonglong;       // 0 to 18,446,744,073,709,551,615
  typedef long long               t_s64, t_int64, t_sqword, t_slonglong;       // –9,223,372,036,854,775,808 to 9,223,372,036,854,775,807

  typedef float                   t_float, t_f32;                     // 3.4E +/- 38 (7 digits)
  typedef double                  t_double, t_f64;                    // 1.7E +/- 308 (15 digits)

  typedef char                    *t_pchar;
  typedef wchar_t                 t_wchar;
  typedef unsigned short          t_unicodechar, *t_punicodechar;
  typedef bool                    t_bool;

  typedef void                    t_void, *t_pvoid;

  typedef unsigned char           t_UTF8;
  typedef unsigned int            t_UTF32;
#ifdef _WIN32
  typedef wchar_t             t_UTF16;
#elif defined(__linux__)
  typedef wchar_t             t_UTF16;
#elif defined (__APPLE__)
  typedef wchar_t             t_UTF16;
#else
#error t_UTF16 is undefined for this platform.
#endif

  typedef std::size_t                  t_size;

// Compile Time
#if defined(__LP64__) || defined(_LP64) || defined(_WIN64)
  typedef long long           t_saddress;
  typedef unsigned long long  t_uaddress;
  typedef t_u64               NUX_POINTER;
  typedef long long           t_integer;
#else
  typedef int                 t_saddress;
  typedef unsigned int        t_uaddress;
  typedef t_uint32            NUX_POINTER;
  typedef long                t_integer;
#endif

  typedef unsigned long            t_addressu32;
  typedef unsigned long long       t_addressu64;

#define NUX_ADDRESS_TO_PTR(addr) (void*)(addr)
#define NUX_PTR_TO_ADDRESS(ptr)  (t_uaddress)(ptr)

  const t_s8  t_s8_min          =  -128;
  const t_s8  t_s8_max          =  127;
  const t_u8  t_u8_min          =  0;
  const t_u8  t_u8_max          =  255;
  const t_s16 t_s16_min         =  -32768;
  const t_s16 t_s16_max         =  32767;
  const t_u16 t_u16_min         =  0;
  const t_u16 t_u16_max         =  65535;
  const t_s32 t_s32_min         =  0x80000000;
  const t_s32 t_s32_max         =  0x7FFFFFFF;
  const t_u32 t_u32_min         =  0x00000000;
  const t_u32 t_u32_max         =  0xFFFFFFFF;
  const t_s64 t_s64_min         =  0x8000000000000000LL;
  const t_s64 t_s64_max         =  0x7FFFFFFFFFFFFFFFLL;
  const t_u64 t_u64_min         =  0x0000000000000000ULL;
  const t_u64 t_u64_max         =  0xFFFFFFFFFFFFFFFFULL;

  const t_f32 t_f32_min       =  1.175494351E-38F;
  const t_f32 t_f32_max     	=  3.402823466E+38F;
  const t_f64 t_f64_min       =  2.2250738585072014E-308;
  const t_f64 t_f64_max     	=  1.7976931348623158E+308;

  const t_f32 MinFloat        =  1.175494351E-38F;
  const t_f32 MaxFloat     	=  3.402823466E+38F;
  const t_f64 MinDouble       =  2.2250738585072014E-308;
  const t_f64 MaxDouble     	=  1.7976931348623158E+308;

//             ILP32	LP64	LLP64	ILP64
// char	        8       8       8       8
// short	    16	    16      16      16
// int	        32	    32      32      64
// long	        32      64      32      64
// long long	64	    64      64      64
// pointer	    32	    64      64      64

// Window 64-bit supports LLP64
// Linux 64 bit  supports LP64
// Mac OS 64 bit  supports LP64

// The size of  a long integer in particular depends on the operating system and the targeted architecture as follows:
//
// OS           arch           size
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

// Polymorphic Types
#if defined(_WIN32) || defined(_WIN64)
    #define NUX_PTRSIZE_INT     INT_PTR
    #define NUX_PTRSIZE_UINT    UINT_PTR
    #define NUX_PTRSIZE_DWORD   DWORD_PTR
    #define NUX_PTRSIZE_LONG    LONG_PTR
    #define NUX_PTRSIZE_ULONG   ULONG_PTR
    #define NUX_SIZE_T          SIZE_T
    #define NUX_SSIZE_T         SSIZE_T
#else
    #define NUX_PTRSIZE_INT     int
    #define NUX_PTRSIZE_UINT    unsigned int
    #define NUX_PTRSIZE_DWORD   unsigned int
    #define NUX_PTRSIZE_LONG    long
    #define NUX_PTRSIZE_ULONG   unsigned long
    #define NUX_SIZE_T          size_t
    #define NUX_SSIZE_T         int
#endif



// #ifdef _WIN32
//     #define NUX_HARDWARE_BREAK __asm {int 3}
//     #define NUX_BREAK_ASM_INT3 __asm {int 3}
// #else
//     #define NUX_HARDWARE_BREAK
//     #define NUX_BREAK_ASM_INT3
// #endif

#define DEGTORAD(d) (d) * 3.1415927f / 180.0f
#define RADTODEG(d) (d) * 180.0f / 3.1415927f


}
#endif // SYSTEMTYPES_H

/*

 C/C++ recognizes the types shown in the table below.

Type Name               Bytes               Other Names				Range of Values
________________________________________________________________________________________________________________________________
int                     *                   signed,
                                            signed int                          System dependent

unsigned int            *                   unsigned                            System dependent

__int8                  1                   char,
                                            signed char                         -128 to 127

__int16                 2                   short,
                                            short int,
                                            signed short int                    -32768 to 32767

__int32                 4                   signed,
                                            signed int                          -2,147,483,648 to 2,147,483,647

__int64                 8                   none                                -9,223,372,036,854,775,808
                                                                                 to 9,223,372,036,854,775,807

char                    1                   signed char                         -128 to 127

unsigned char           1                   none                                0 to 255

short                   2                   short int,
                                            signed short int                    -32,768 to 32,767

unsigned short          2                   unsigned short int                  0 to 65,535

long                    4                   long int,
                                            signed long int                     -2,147,483,648 to 2,147,483,647

unsigned long           4                   unsigned long int                   0 to 4,294,967,295

enum                    *                   none                                Same as int

float                   4                   none                                3.4E +/- 38 (7 digits)

double                  8                   none                                1.7E +/- 308 (15 digits)

long double             10                  none                                1.2E +/- 4932 (19 digits)



*: OS dependant

*/
