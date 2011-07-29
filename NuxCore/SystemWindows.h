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


#ifndef SYSTEMWIN32_H
#define SYSTEMWIN32_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif


#include <windows.h>

#include <tchar.h>
#include <sys/utime.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <winbase.h>
#include <commdlg.h>
#include <shellapi.h>

#include <io.h>
#include <direct.h>
#include <memory>
// If NUX_LOG_FILE_ANSI is set to 1, log files will be written in ASCII characters even when in UNICODE.
#define NUX_LOG_FILE_ANSI   1

// // Undo any Windows defines.
// #undef BYTE
// #undef WORD
// #undef DWORD
// #undef INT
// #undef FLOAT
// #undef MAXBYTE
// #undef MAXWORD
// #undef MAXDWORD
// #undef MAXINT
// #undef CDECL
// #undef BOOL
// #undef TCHAR
// 
// // Undef Windows min and max. Conflict with std::min, std::max.
// #undef min
// #undef max
// 


#define NUX_VARARGS     __cdecl					// Functions with variable arguments

// Calling Convention
// This is the default calling convention for C and C++ programs.
// Because the stack is cleaned up by the caller, it can do vararg functions.
// Argument-passing order: Right to left
#define NUX_CDECL	    __cdecl
// The __stdcall calling convention is used to call Win32 API functions.
// The callee cleans the stack, so the compiler makes vararg functions __cdecl.
// Argument-passing order: Right to left
#define NUX_STDCALL		__stdcall
// The __fastcall calling convention specifies that arguments to functions are to be passed in registers, when possible.
#define NUX_FASTCALL    __fastcall
// This is the default calling convention used by C++ member functions that do not use variable arguments.
// Under thiscall, the callee cleans the stack, which is impossible for vararg functions.
#define NUX_THISCALL    thiscall                //

#define NUX_INLINE          inline			        // Force inline code
#define NUX_FORCEINLINE     __forceinline			// Force inline code
#define NUX_FORCENOINLINE   __declspec(noinline)	// Force code to NOT be inline


// Unsigned base types.
typedef unsigned char           BYTE;       // 8-bit  unsigned.
typedef unsigned short          WORD;       // 16-bit unsigned.
typedef unsigned int            UINT;       // 32-bit unsigned.
typedef unsigned long           DWORD;		// 32-bit unsigned.
typedef unsigned __int64        QWORD;		// 64-bit unsigned.

// Signed base types.
typedef	signed char             SBYTE;		// 8-bit  signed.
typedef signed short            SWORD;		// 16-bit signed.
typedef signed int              INT;		// 32-bit signed.
typedef signed __int64          SQWORD;		// 64-bit signed.


// Character types.
typedef char                    ANSICHAR;   // An ANSI character.
typedef unsigned char           ANSIUCHAR;  // An ANSI character.
typedef wchar_t                 UNICHAR;    // A unicode character. L"Hello" is of type wchar_t and should not be confuse with type unsigned short on windows.

// Other base types.
typedef float                   FLOAT;      // 32-bit IEEE floating point.
typedef double                  DOUBLE;     // 64-bit IEEE double.

///////////////////////////////////////////////
// UNICODE                                   //
///////////////////////////////////////////////
#ifdef _UNICODE
#ifndef _TCHAR_DEFINED
typedef wchar_t  TCHAR;
#endif

#ifndef _TEXT_DEFINED
#undef TEXT
#define TEXT(s) L##s
#endif

#else
#ifndef _TCHAR_DEFINED
typedef ANSICHAR  TCHAR;
#endif

#undef TEXT
#define TEXT(s) s
#endif

// #ifdef WIN32_SECURE
//     #define STRNCPY_S(strDest, numberOfElements, strSource, count) strncpy_s(strDest, numberOfElements, strSource, count)
//     #define WCSNCPY_S(strDest, numberOfElements, strSource, count) wcsncpy_s(strDest, numberOfElements, strSource, count)
//     #define MBSNCPY_S(strDest, numberOfElements, strSource, count) _mbsncpy_s(strDest, numberOfElements, strSource, count)
//     #define _TCSNCPY_S(strDest, numberOfElements, strSource, count) _tcsncpy_s(strDest, numberOfElements, strSource, count)
//
//     #define STRCPY_S(strDest, numberOfElements, strSource) strcpy_s(strDest, numberOfElements, strSource)
//     #define WCSCPY_S(strDest, numberOfElements, strSource) wcscpy_s(strDest, numberOfElements, strSource)
//     #define MBSCPY_S(strDest, numberOfElements, strSource) _mbscpy_s(strDest, numberOfElements, strSource)
//     #define _TCSCPY_S(strDest, numberOfElements, strSource) _tcscpy_s(strDest, numberOfElements, strSource)
//
//     #define STRCAT_S(strDest, numberOfElements, strSource) strcat_s(strDest, numberOfElements, strSource)
//     #define WCSCAT_S(strDest, numberOfElements, strSource) wcscat_s(strDest, numberOfElements, strSource)
//     #define MBSCAT_S(strDest, numberOfElements, strSource) _mbscat_s(strDest, numberOfElements, strSource)
//
//     #define _VSNPRINTF_S(strDest, numberOfElements, Count, Format, VA_Arg_List) _vsnprintf_s(strDest, numberOfElements, Count, Format, VA_Arg_List)
//     #define _VSNTPRINTF_S(strDest, numberOfElements, Count, Format, VA_Arg_List) _vsntprintf_s(strDest, numberOfElements, Count, Format, VA_Arg_List)
//     #define SPRINTF_S(strDest, numberOfElements, Format, ...) sprintf_s(strDest, numberOfElements, Format, ##__VA_ARGS__)
//     #define _SNPRINTF_S(strDest, numberOfElements, Count, Format, ...) _snprintf_s(strDest, numberOfElements, Count, Format, ##__VA_ARGS__)
//
//     #define STRDATE_S(strDest, numberOfElements) _strdate_s(strDest, numberOfElements)
//     #define _TSTRDATE_S(strDest, numberOfElements) _tstrdate_s(strDest, numberOfElements)
//     #define _STRTIME_S(strDest, numberOfElements) _strtime_s(strDest, numberOfElements)
//     #define _TSTRTIME_S(strDest, numberOfElements) _tstrtime_s(strDest, numberOfElements)
//
//     #define FOPEN_S(file, filename, mode)   fopen_s(file, filename, mode)
//
//     #define _TCSNLEN(str, numberOfElements) _tcsnlen(str)
//
//     #define _TSPLITPATH(path, Drive, DriveNumElements, Dir, DirNumElements, Filename, FileNumElements, Extension, ExtNumElements) _tsplitpath_s(path, Drive, DriveNumElements, Dir, DirNumElements, Filename, FileNumElements, Extension, ExtNumElements)
//     #define _TMAKEPATH_S(path, numberOfElements, Drive, Dir, Filename, Extension) _tmakepath_s(path, numberOfElements, Drive, Dir, Filename, Extension)
//
//     #define SSCANF_S(buffer, Format, ...) sscanf_s(buffer, Format, ##__VA_ARGS__)
//     #define SWSCANF_S(buffer, Format, ...) swscanf_s(buffer, Format, ##__VA_ARGS__)
//
// #else
//     #define STRNCPY_S(strDest, numberOfElements, strSource, count) strncpy(strDest, strSource, count)
//     #define WCSNCPY_S(strDest, numberOfElements, strSource, count) wcsncpy(strDest, strSource, count)
//     #define MBSNCPY_S(strDest, numberOfElements, strSource, count) _mbsncpy(strDest, strSource, count)
//     #define _TCSNCPY_S(strDest, numberOfElements, strSource, count) _tcsncpy(strDest, strSource, count)
//
//     #define STRCPY_S(strDest, numberOfElements, strSource) strcpy(strDest, strSource)
//     #define WCSCPY_S(strDest, numberOfElements, strSource) wcscpy(strDest, strSource)
//     #define MBSCPY_S(strDest, numberOfElements, strSource) _mbscpy(strDest, strSource)
//     #define _TCSCPY_S(strDest, numberOfElements, strSource) _tcscpy(strDest, strSource)
//
//     #define STRCAT_S(strDest, numberOfElements, strSource) strcat(strDest, strSource)
//     #define WCSCAT_S(strDest, numberOfElements, strSource) wcscat(strDest, strSource)
//     #define MBSCAT_S(strDest, numberOfElements, strSource) _mbscat(strDest, strSource)
//
//     #define _VSNPRINTF_S(strDest, numberOfElements, Count, Format, VA_Arg_List) _vsnprintf(strDest, Count, Format, VA_Arg_List)
//     #define _VSNTPRINTF_S(strDest, numberOfElements, Count, Format, VA_Arg_List) _vsntprintf(strDest, Count, Format, VA_Arg_List)
//     #define SPRINTF_S(strDest, numberOfElements, Format, a,b,c,d,e,f,g,h,i,j,k,l) sprintf(strDest, Format, VARG(a),VARG(b),VARG(c),VARG(d),VARG(e),VARG(f),VARG(g),VARG(h),VARG(i),VARG(j),VARG(k),VARG(l))
//     #define _SNPRINTF_S(strDest, numberOfElements, Count, Format, a,b,c,d,e,f,g,h,i,j,k,l) _snprintf(strDest, Count, Format, VARG(a),VARG(b),VARG(c),VARG(d),VARG(e),VARG(f),VARG(g),VARG(h),VARG(i),VARG(j),VARG(k),VARG(l))
//
//     #define STRDATE_S(strDest, numberOfElements) _strdate(strDest)
//     #define _TSTRDATE_S(strDest, numberOfElements) _tstrdate(strDest)
//     #define _STRTIME_S(strDest, numberOfElements) _strtime(strDest)
//     #define _TSTRTIME_S(strDest, numberOfElements) _tstrtime(strDest)
//
//     #define FOPEN_S(file, filename, mode)   (file=fopen(filename, mode))
//
//     #define _TCSNLEN(str, numberOfElements)     _tcsclen(str)
//
//     #define _TSPLITPATH(path, Drive, DriveNumElements, Dir, DirNumElements, Filename, FileNumElements, Extension, ExtNumElements) _tsplitpath(path, Drive, Dir, Filename, Extension)
//     #define _TMAKEPATH_S(path, numberOfElements, Drive, Dir, Filename, Extension) _makepath(path, Drive, Dir, Filename, Extension)
//
//     #define SSCANF_S(buffer, Format, VA_Arg_List) sscanf(buffer, Format, VA_Arg_List)
//     #define SWSCANF_S(buffer, Format, VA_Arg_List) swscanf(buffer, Format, VA_Arg_List)
// #endif

///////////////////////////////////////////////
// Variable argument functions               //
///////////////////////////////////////////////

//@note: Currently xutility breaks with this enabled 2005 fixes it
#pragma warning(disable : 4619) // #pragma warning : there is no warning number 'number'   
//#pragma warning(disable : 4267) // #pragma warning : conversion from 'size_t' to 'int', possible loss of data


// Unwanted VC++ level 4 warnings to disable.
#pragma warning(disable : 4100) // unreferenced formal parameter										
#pragma warning(disable : 4127) // Conditional expression is constant									
#pragma warning(disable : 4200) // Zero-length array item at end of structure, a VC-specific extension	
#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union					
#pragma warning(disable : 4244) // conversion to float, possible loss of data						
#pragma warning(disable : 4245) // conversion from 'enum ' to 'unsigned long', signed/unsigned mismatch 
#pragma warning(disable : 4251) // needs to have dll-interface to be used by clients of class 'ULinker' 
#pragma warning(disable : 4275) // non dll-interface class used as base for dll-interface class          
#pragma warning(disable : 4291) // typedef-name '' used as synonym for class-name ''                    
#pragma warning(disable : 4324) // structure was padded due to __declspec(align())						
#pragma warning(disable : 4355) // this used in base initializer list                                   
#pragma warning(disable : 4389) // signed/unsigned mismatch                                             
#pragma warning(disable : 4511) // copy constructor could not be generated                              
#pragma warning(disable : 4512) // assignment operator could not be generated                           
#pragma warning(disable : 4514) // unreferenced inline function has been removed						
#pragma warning(disable : 4699) // creating precompiled header											
#pragma warning(disable : 4702) // unreachable code in inline expanded function							
#pragma warning(disable : 4710) // inline function not expanded											
#pragma warning(disable : 4711) // function selected for autmatic inlining								
#pragma warning(disable : 4714) // __forceinline function not expanded									


namespace nux
{

  void *GetDllHandle ( const TCHAR *DllName );

  void FreeDllHandle ( void *DllHandle );

  void *GetDllExport ( void *DllHandle, const TCHAR *ExportName );


}

#endif // SYSTEMWIN32_H


