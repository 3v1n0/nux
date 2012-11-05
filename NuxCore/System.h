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


#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef _DEBUG
  #define NUX_DEBUG
#endif

#ifdef _WIN32
    #define NUX_OS_WINDOWS
#elif __arm__
    #define NUX_OS_LINUX
#elif __linux__
    #define NUX_OS_LINUX
#elif __APPLE__
    #define NUX_OS_MACOSX
#endif

#if __GNUC__
  #define NUX_GNUC_COMPILER
  #if __GNUG__
    #define NUX_GNUCPP_COMPILER
  #else
    #error Support only g++.
  #endif

  // Compiler string.
  #define NUX_COMPILER_STRING "GNU CPP Compiler"

  // Build string
  #ifdef NUX_DEBUG
    #define NUX_BUILD_STRING "Debug build compiled with " NUX_COMPILER_STRING
  #else
    #define NUX_BUILD_STRING "Compiled with " NUX_COMPILER_STRING
  #endif
#endif

#if __APPLE_CC__
  #define NUX_APPLE_COMPILER
#endif

#if defined(_MSC_VER)
  #define NUX_MICROSOFT_COMPILER
#endif

#if defined(_M_X64) || defined(__amd64__) || defined(__ia64__)
  #define NUX_ARCH_x64
#elif defined(_M_IX86) || defined(__i386__)
  #define NUX_ARCH_i386
#elif defined(__arm__)
  #define NUX_ARCH_ARM
#endif

// If we are compiling for linux, and neither USE_X11 or NO_X11 are set,
// then assume that we are compiling for X11
#if defined(NUX_OS_LINUX)
#  if defined(USE_X11)
#    if defined(NO_X11)
#      error Can not define both USE_X11 and NO_X11
#    endif
#  elif !defined(NO_X11)
#    define USE_X11
# endif
#elif defined(USE_X11)
#  error X11 not supported on non-linux systems
#endif

#if defined(USE_X11)
#  define DRAG_AND_DROP_SUPPORTED
#endif

// Right now, we only support minimal builds for ARM if no X.
#if (defined(NUX_ARCH_ARM) && defined(NO_X11) && !defined(NUX_MINIMAL))
#  define NUX_MINIMAL
#endif

// Compiler Macros:
// NUX_GNUCPP_COMPILER
// NUX_MICROSOFT_COMPILER

// OS Macros:
// NUX_OS_WINDOWS
// NUX_OS_LINUX
// NUX_OS_CELL
// NUX_OS_MACOSX

#ifdef _WIN32

//    Add lib  DbgHelp.lib for the following function
//    StackWalk64
//    SymFunctionTableAccess64
//    SymGetModuleBase64
//    SymGetModuleInfo64
//    SymGetLineFromAddr64
//    SymGetSymFromAddr64
//    SymInitialize
//    SymSetOptions
//    SymGetOptions
//    SymLoadModule64
#pragma comment(lib, "DbgHelp")

// _MSC_VER: Defines the major and minor versions of the compiler. For example, 1300 for Microsoft Visual C++ .NET. 1300 represents version 13
// and no point release. This represents the fact that there have been a total of 13 releases of the compiler.
// If you type cl /? at the command line, you will see the full version for the compiler you are using.

#ifndef _MSC_VER
#error Support only Visual Studio Compiler.
#endif

#define VISUAL_STUDIO_2010_COMPILER     1600
#define VISUAL_STUDIO_2008_COMPILER     1500
#define VISUAL_STUDIO_2005_COMPILER     1400
#define VISUAL_STUDIO_2003_COMPILER     1310

#if _MSC_VER >= 1600
  #define NUX_VISUAL_STUDIO_2010
#elif _MSC_VER >= 1500
  #define NUX_VISUAL_STUDIO_2008
#elif _MSC_VER >= 1400
  #define NUX_VISUAL_STUDIO_2005
#elif _MSC_VER >= 1310
  #define NUX_VISUAL_STUDIO_2003
#endif

// Compiler string.
#if (_MSC_VER >= VISUAL_STUDIO_2008_COMPILER)
  #define NUX_COMPILER_STRING "Visual Studio 2008"
#elif (_MSC_VER >= VISUAL_STUDIO_2005_COMPILER)
  #define NUX_COMPILER_STRING "Visual Studio 2005"
#elif (_MSC_VER >= VISUAL_STUDIO_2003_COMPILER)
  #define NUX_COMPILER_STRING "Visual Studio 2003"
#endif

// Build String
#ifdef NUX_DEBUG
#define NUX_BUILD_STRING "Debug build compiled with " NUX_COMPILER_STRING
#else
#define NUX_BUILD_STRING "Compiled with " NUX_COMPILER_STRING
#endif


// Define the proper values for _WIN32_WINNT and WINVER in the compiler response file "compiler_options.h"
//    Windows Vista 	    _WIN32_WINNT>=0x0600
//                          WINVER>=0x0600
//    Windows Server 2003   _WIN32_WINNT>=0x0502
//                          WINVER>=0x0502
//    Windows XP 	        _WIN32_WINNT>=0x0501
//                          WINVER>=0x0501
//    Windows 2000 	        _WIN32_WINNT>=0x0500
//                          WINVER>=0x0500
//    Windows NT 4.0 	    _WIN32_WINNT>=0x0400
//                          WINVER>=0x0400
//    Windows Me 	        _WIN32_WINDOWS=0x0500
//                          WINVER>=0x0500
//    Windows 98 	        _WIN32_WINDOWS>=0x0410
//                          WINVER>=0x0410
//    Windows 95 	        _WIN32_WINDOWS>=0x0400
//                          WINVER>=0x0400


//    Minimum system required 	Macros to define
//    Windows Vista 	NTDDI_VERSION >=NTDDI_LONGHORN
//    Windows Server 2003 SP1 	NTDDI_VERSION >=NTDDI_WS03SP1
//    Windows Server 2003 	NTDDI_VERSION >=NTDDI_WS03
//    Windows XP SP2 	NTDDI_VERSION >=NTDDI_WINXPSP2
//    Windows XP SP1 	NTDDI_VERSION >=NTDDI_WINXPSP1
//    Windows XP 	NTDDI_VERSION >=NTDDI_WINXP
//    Windows 2000 SP4 	NTDDI_VERSION >=NTDDI_WIN2KSP4
//    Windows 2000 SP3 	NTDDI_VERSION >=NTDDI_WIN2KSP3
//    Windows 2000 SP2 	NTDDI_VERSION >=NTDDI_WIN2KSP2
//    Windows 2000 SP1 	NTDDI_VERSION >=NTDDI_WIN2KSP1
//    Windows 2000 	NTDDI_VERSION >=NTDDI_WIN2K

#define WIN32_LEAN_AND_MEAN 1
#endif  // _WIN32




// Logging
#if defined(NUX_OS_WINDOWS) && defined(NUX_DEBUG)
  #define NUX_ENABLE_ASSERT_MACROS
  #define NUX_ENABLE_LOGGING
#elif defined(NUX_OS_LINUX) && defined(NUX_DEBUG)
  #define NUX_ENABLE_ASSERT_MACROS
  #define NUX_ENABLE_LOGGING
#endif

// NOP: no operation
// Specifies that a function should be ignored and the argument list
// be parsed but no code be generated for the arguments. It is intended for use in global
// debug functions that take a variable number of arguments.

#if defined(NUX_MICROSOFT_COMPILER)
  #define NUX_COMPILER_SUPPORTS_NOOP
  #define NUX_NOOP __noop
#elif defined(NUX_GNUCPP_COMPILER)
  #define NUX_COMPILER_SUPPORTS_NOOP
  #define NUX_NOOP __asm__("nop")
#endif

// Pragma pack support
#if defined(NUX_MICROSOFT_COMPILER) || defined(NUX_GNUCPP_COMPILER)
  #define NUX_SUPPORTS_PRAGMA_PACK
#endif


// Define variadic macro support
#if defined(NUX_MICROSOFT_COMPILER) && (defined(NUX_VISUAL_STUDIO_2005) || defined(NUX_VISUAL_STUDIO_2008) || defined(NUX_VISUAL_STUDIO_2010))
    #define NUX_VARIADIC_MACROS_SUPPORT
#elif defined(NUX_GNUCPP_COMPILER)
    #define NUX_VARIADIC_MACROS_SUPPORT
#endif


/// DLL declaration macros
#if defined(NUX_OS_WINDOWS)
    #ifdef NUX_DLL
        #if (!defined(_WIN32)) && (!defined(_WIN64))
            #error("ERROR: Use NUX_DLL is permitted only on win32 & win64 platforms")
        #endif

        #define NUX_DECLSPEC_DLLIMPORT        __declspec(dllimport)
        #define NUX_DECLSPEC_DLLEXPORT        __declspec(dllexport)
    #else
        #define NUX_DECLSPEC_DLLIMPORT
        #define NUX_DECLSPEC_DLLEXPORT
    #endif

    #ifdef NUX_EXPORT_DLL
        #define NUX_DECLSPEC_DLL               NUX_DECLSPEC_DLLEXPORT
    #else
        #define NUX_DECLSPEC_DLL               NUX_DECLSPEC_DLLIMPORT
    #endif
#elif defined(NUX_OS_LINUX)
    #if __GNUC__ >= 4
        #define NUX_DECLSPEC_DLLIMPORT __attribute__ ((visibility("default")))
        #define NUX_DECLSPEC_DLLEXPORT __attribute__ ((visibility("default")))
    #else
        #define NUX_DECLSPEC_DLLIMPORT
        #define NUX_DECLSPEC_DLLEXPORT
    #endif

    #ifdef NUX_EXPORT_DLL
        #define NUX_DECLSPEC_DLL               NUX_DECLSPEC_DLLEXPORT
    #else
        #define NUX_DECLSPEC_DLL               NUX_DECLSPEC_DLLIMPORT
    #endif
#endif

#define NUX_CHECK_PUREVIRTUALS 1

// Throwing exceptions:

// #ifdef NUX_DEBUG
//     // if we are in Debug disable exceptions. What we want is to break were and error happens: ie NUX_BREAK_ASM_INT3
//     #define NUX_EXCEPTIONS_DISABLED 1
// #endif

#define NUX_EXCEPTIONS_DISABLED 1

#define STL std

#endif // SYSTEM_H

