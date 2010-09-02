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


#ifndef NSYSTEM_H
#define NSYSTEM_H

#ifdef _WIN32
    #define INL_OS_WINDOWS
#elif __linux
    #define INL_OS_LINUX
#elif INL_PS3
    #define INL_OS_CELL
#elif __APPLE__ 
    #define INL_OS_MACOSX
#endif

#if __GNUC__
    #define INL_GNUC_COMPILER
    #if __GNUG__
        #define INL_GNUCPP_COMPILER
    #else  
        #error Support only g++.
    #endif

    // Compiler string.
    #define INL_COMPILER_STRING "GNU CPP Compiler"

    // Build string
    #ifdef _DEBUG
        #define INL_BUILD_STRING "Debug build compiled with " INL_COMPILER_STRING
    #else
        #define INL_BUILD_STRING "Compiled with " INL_COMPILER_STRING
    #endif
#endif

#if __APPLE_CC__
    #define INL_APPLE_COMPILER
#endif

#if defined(_M_X64) || defined(__amd64__) || defined(__ia64__)
    #define INL_ARCH_x64
#elif defined(_M_IX86) || defined(__i386__)
    #define INL_ARCH_i386
#elif defined(__arm__)
    #define INL_ARCH_arm
#elif defined(__cell)
    #define INL_ARCH_cell
#endif

#if _MSC_VER
    #define INL_MICROSOFT_COMPILER
#endif

// Compiler Macros:
// INL_GNUCPP_COMPILER
// INL_MICROSOFT_COMPILER

// OS Macros:
// INL_OS_WINDOWS
// INL_OS_LINUX
// INL_OS_CELL
// INL_OS_MACOSX

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

    #define VISUAL_STUDIO_2008_COMPILER     1500
    #define VISUAL_STUDIO_2005_COMPILER     1400
    #define VISUAL_STUDIO_2003_COMPILER     1310

    #if _MSC_VER >= 1500
        #define INL_VISUAL_STUDIO_2008
    #elif _MSC_VER >= 1400
        #define INL_VISUAL_STUDIO_2005
    #elif _MSC_VER >= 1310
        #define INL_VISUAL_STUDIO_2003
    #endif

    // Compiler string.
    #if (_MSC_VER >= VISUAL_STUDIO_2008_COMPILER)
        #define INL_COMPILER_STRING "Visual Studio 2008"
    #elif (_MSC_VER >= VISUAL_STUDIO_2005_COMPILER)
        #define INL_COMPILER_STRING "Visual Studio 2005"
    #elif (_MSC_VER >= VISUAL_STUDIO_2003_COMPILER)
        #define INL_COMPILER_STRING "Visual Studio 2003"
    #endif

    // Build String
    #ifdef _DEBUG
        #define INL_BUILD_STRING "Debug build compiled with " INL_COMPILER_STRING
    #else
        #define INL_BUILD_STRING "Compiled with " INL_COMPILER_STRING
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
#if defined(INL_OS_WINDOWS) && defined(_DEBUG)
    #define INL_ENABLE_ASSERT_MACROS
    #define INL_ENABLE_LOGGING
#elif defined(INL_OS_LINUX) && defined(_DEBUG)
    #define INL_ENABLE_ASSERT_MACROS
    #define INL_ENABLE_LOGGING
#endif

// NOP: no operation
// Specifies that a function should be ignored and the argument list 
// be parsed but no code be generated for the arguments. It is intended for use in global
// debug functions that take a variable number of arguments.

#if defined(INL_MICROSOFT_COMPILER)
    #define INL_COMPILER_SUPPORTS_NOOP
    #define INL_NOOP __noop
#elif defined(INL_GNUCPP_COMPILER)
    #define INL_COMPILER_SUPPORTS_NOOP
    #define INL_NOOP __asm__("nop")
#elif defined(INL_PS3)
    #define INL_COMPILER_SUPPORTS_NOOP
    #define INL_NOOP __asm__("nop")
#endif

// Pragma pack support
#if defined(INL_MICROSOFT_COMPILER) || defined(INL_GNUCPP_COMPILER) || defined(INL_PS3)
    #define INL_SUPPORTS_PRAGMA_PACK
#endif


// Define variadic macro support
#if defined(INL_MICROSOFT_COMPILER) && (defined(INL_VISUAL_STUDIO_2005) || defined(INL_VISUAL_STUDIO_2008))
    #define INL_VARIADIC_MACROS_SUPPORT
#elif defined(INL_GNUCPP_COMPILER)
    #define INL_VARIADIC_MACROS_SUPPORT
#elif defined(INL_PS3)
    #define INL_VARIADIC_MACROS_SUPPORT
#endif


/// DLL declaration macros
#if defined(INL_OS_WINDOWS)
    #ifdef INL_DLL
        #if (!defined(_WIN32)) && (!defined(_WIN64))
            #error("ERROR: Use INL_DLL is permitted only on win32 & win64 platforms")
        #endif

        #define INL_DECLSPEC_DLLIMPORT        __declspec(dllimport)
        #define INL_DECLSPEC_DLLEXPORT        __declspec(dllexport)
    #else
        #define INL_DECLSPEC_DLLIMPORT
        #define INL_DECLSPEC_DLLEXPORT
    #endif

    #ifdef INL_EXPORT_DLL
        #define INL_DECLSPEC_DLL               INL_DECLSPEC_DLLEXPORT
    #else
        #define INL_DECLSPEC_DLL               INL_DECLSPEC_DLLIMPORT
    #endif
#elif defined(INL_OS_LINUX)
    #if __GNUC__ >= 4
        #define INL_DECLSPEC_DLLIMPORT __attribute__ ((visibility("default")))
        #define INL_DECLSPEC_DLLEXPORT __attribute__ ((visibility("default")))
    #else
        #define INL_DECLSPEC_DLLIMPORT
        #define INL_DECLSPEC_DLLEXPORT
    #endif

    #ifdef INL_EXPORT_DLL
        #define INL_DECLSPEC_DLL               INL_DECLSPEC_DLLEXPORT
    #else
        #define INL_DECLSPEC_DLL               INL_DECLSPEC_DLLIMPORT
    #endif
#endif

#define INL_CHECK_PUREVIRTUALS 1

// Throwing exceptions:

// #ifdef _DEBUG
//     // if we are in Debug disable exceptions. What we want is to break were and error happens: ie INL_BREAK_ASM_INT3
//     #define INL_EXCEPTIONS_DISABLED 1
// #endif

#define INL_EXCEPTIONS_DISABLED 1

#define STL std

#endif // NSYSTEM_H

