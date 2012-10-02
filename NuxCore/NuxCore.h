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

#ifndef NUXCORE_H
#define NUXCORE_H


#include "System.h"

#include <cassert>  // (assert.h)
#include <cctype>   // (ctype.h)
#include <cerrno>   // (errno.h)
#include <cfloat>   // (float.h)
#include <ciso646>  // (iso646.h)
#include <climits>  // (limits.h)
#include <clocale>  // (locale.h)
#include <cmath>    // (math.h)
//#include <csetjmp>  // (setjmp.h) conflict with libpng on GNU systems
//#include <csignal>  // (signal.h)
#include <cstdarg>  // (stdarg.h)
#include <cstddef>  // (stddef.h)
#include <cstdio>   // (stdio.h)
#include <cstdlib>  // (stdlib.h)
#include <cstring>  // (string.h)
#include <ctime>    // (time.h)
#include <cwchar>   // (wchar.h)
#include <cwctype>  // (wctype.h)
#include <exception>
#include <stdexcept>

#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <iomanip>
#include <map>
#include <vector>
#include <list>
#include <stack>
#include <queue>
#include <algorithm>
#include <new>
#include <set>

// WIN32_SECURE if define for the latest version of Visual Studio starting at VS 2005. We use it for security improvement.
#if (defined NUX_VISUAL_STUDIO_2005) || (defined NUX_VISUAL_STUDIO_2008) || (defined NUX_VISUAL_STUDIO_2010)
#define WIN32_SECURE
#endif

#define NUX_STATIC_CAST(a, b)       static_cast<a>(b)
#define NUX_REINTERPRET_CAST(a, b)  reinterpret_cast<a>(b)
#define NUX_CONST_CAST(a, b)        const_cast<a>(b)
#define NUX_DYNAMIC_CAST(a, b)      dynamic_cast<a>(b)

#define NUX_INVALID_INDEX           -1
#define NUX_INVALID_HANDLE          -1

#define NUX_IN
#define NUX_OUT

#define NUX_MAKEFOURCHARTAG(ch0, ch1, ch2, ch3)  \
    ((DWORD)(BYTE)(ch0) |               \
    ((DWORD)(BYTE)(ch1) << 8) |         \
    ((DWORD)(BYTE)(ch2) << 16) |        \
    ((DWORD)(BYTE)(ch3) << 24 ))


#define INLNEW new
#define INLDELETE delete
#define INLDELETEARRAY delete []

#define NUX_RUNTIME_ERROR(str, ...)               LogOutputErrorMessage(__FILE__, __LINE__, str, ##__VA_ARGS__);
#define NUX_ERROR_IF_NULL(test, str, ...)         if((test) == 0)   LogOutputErrorMessage(__FILE__, __LINE__, str, ##__VA_ARGS__);
#define NUX_ERROR_IF_TRUE(test, str, ...)         if(test)        LogOutputErrorMessage(__FILE__, __LINE__, str, ##__VA_ARGS__);
#define NUX_ERROR_IF_FALSE(test, str, ...)        if(!(test))     LogOutputErrorMessage(__FILE__, __LINE__, str, ##__VA_ARGS__);

#define NUX_RETURN_IF_NULL(test)                  if((test) == 0) return;
#define NUX_RETURN_IF_NOTNULL(test)               if((test) != 0) return;
#define NUX_RETURN_IF_TRUE(test)                  if(test)        return;
#define NUX_RETURN_IF_FALSE(test)                 if(!(test))     return;
#define NUX_RETURN_IF_FAIL(test)                  if(!(test))     return;

#define NUX_RETURN_VALUE_IF_NULL(test, value)     if((test) == 0) return value;
#define NUX_RETURN_VALUE_IF_NOTNULL(test, value)  if((test) != 0) return value;
#define NUX_RETURN_VALUE_IF_TRUE(test, value)     if(test)        return value;
#define NUX_RETURN_VALUE_IF_FALSE(test, value)    if(!(test))     return value;


// Structure Alignment
#if defined(NUX_MICROSOFT_COMPILER)
#define NUX_DATA_ALIGN(declaration, alignment) __declspec(align(alignment)) declaration
#elif defined (NUX_GNUCPP_COMPILER)
#define NUX_DATA_ALIGN(declaration, alignment) declaration __attribute__ ((aligned (alignment)))
#endif

// Sizeof is a compile time function. So array must be totally defined if sizeof is used on it.
// The number of elements in array must be a constant at compile time.
// Example: int array[10] is valid.
#define NUX_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

// Compiler specific include.
#if defined(NUX_OS_WINDOWS) && defined (NUX_MICROSOFT_COMPILER)
    #include "SystemWindows.h"
#elif defined (NUX_OS_LINUX) && defined (NUX_GNUCPP_COMPILER)
    #include "SystemGNU.h"
#elif defined (NUX_OS_MACOSX) && defined (NUX_GNUCPP_COMPILER)
    #error Unknown Compiler
#else
    #error Unknown Compiler
#endif


namespace nux
{

// Variable arguments.
  unsigned int GetVariableArgs (TCHAR *Dest, unsigned int Size, unsigned int Count, const TCHAR*& Fmt, va_list ArgPtr);
  unsigned int GetVariableArgsAnsi (ANSICHAR *Dest, unsigned int Size, unsigned int Count, const ANSICHAR*& Fmt, va_list ArgPtr);


#define GET_VARARGS(msg, size, len, fmt)            \
{                                                   \
    va_list arg_list;                               \
    va_start(arg_list,fmt);                         \
    VSNTPRINTF_S( msg, size, len, fmt, arg_list );  \
    va_end( arg_list );                             \
}
#define GET_VARARGS_ANSI(msg, size, len, fmt)       \
{                                                   \
    va_list arg_list;                               \
    va_start(arg_list,fmt);                         \
    VSNPRINTF_S( msg, size, len, fmt, arg_list );   \
    va_end( arg_list );                             \
}
#define GET_VARARGS_RESULT(msg, size, len, fmt, result)         \
{                                                               \
    va_list arg_list;                                           \
    va_start(arg_list, fmt);                                    \
    result = GetVariableArgs(msg, size, len, fmt, arg_list);    \
    va_end(arg_list);                                           \
}

//////////////////////////////////////////////////////////////////////////
//  Check macros for assertions.                                        //
//////////////////////////////////////////////////////////////////////////
  typedef enum
  {
    NUX_MSG_SEVERITY_CRITICAL   = 0,
    NUX_MSG_SEVERITY_ALERT      = 1,
    NUX_MSG_SEVERITY_WARNING    = 2,
    NUX_MSG_SEVERITY_INFO       = 3,
    NUX_MSG_SEVERITY_NONE       = 4,

  } MessageSeverity;

#define nuxOkMsg(str, ...)        { nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_INFO, str, ##__VA_ARGS__);}
#define nuxWarningMsg(str, ...)   { nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_WARNING, str, ##__VA_ARGS__);}
#define nuxAlertMsg(str, ...)     { nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_ALERT, str, ##__VA_ARGS__);}
#define nuxCriticalMsg(str, ...)  { nux::LogOutputSeverityMessage(nux::NUX_MSG_SEVERITY_CRITICAL, str, ##__VA_ARGS__);}

#ifdef NUX_ENABLE_ASSERT_MACROS
#define nuxAssert(expr)             { if(!(expr)) nuxFailAssert(TEXT(#expr)); }
  // Expression is always evaluated no matter if NUX_ENABLE_ASSERT_MACROS is enabled. nuxFailAssert is called if enabled.
#define nuxVerifyExpr(expr)         { if(!(expr)) nuxFailAssert(TEXT(#expr)); }

#define DEBUGTRACE(str, ...)    nuxDebugMsg(str, ##__VA_ARGS__)

#ifdef NUX_VARIADIC_MACROS_SUPPORT
#define nuxFailAssert(str, ...)         { if(nuxIsDebuggerPresent()){nux::LogOutputAssertMessage(__FILE__, __LINE__, str, ##__VA_ARGS__);} inlDebugBreak();}
#define nuxError(str, ...)              { if(nuxIsDebuggerPresent()){nux::LogOutputErrorMessage(__FILE__, __LINE__, str, ##__VA_ARGS__);} inlDebugBreak();}
#define nuxDebugMsg(str, ...)           { if(nuxIsDebuggerPresent()) nux::LogOutputDebugMessage(str, ##__VA_ARGS__);}

#define nuxAssertMsg(expr, a, ...)      { if(!(expr)) nuxFailAssert( TEXT(#expr) TEXT(" : ") a, ##__VA_ARGS__); }
#define nuxVerifyExprMsg(expr, a, ...)  { if(!(expr)) nuxFailAssert( TEXT(#expr) TEXT(" : ") a, ##__VA_ARGS__); }   // Expression is always evaluated. nuxFailAssert is called if enabled.
#else
#define nuxFailAssert(a,b,c,d,e,f,g,h,i,j,k,l)          { if(nuxIsDebuggerPresent()){nux::LogOutputAssertMessage(__FILE__,__LINE__,VARG(a),VARG(b),VARG(c),VARG(d),VARG(e),VARG(f),VARG(g),VARG(h),VARG(i),VARG(j),VARG(k),VARG(l));} inlDebugBreak();}
#define nuxError(a,b,c,d,e,f,g,h,i,j,k,l)               { if(nuxIsDebuggerPresent()) {nux::LogOutputErrorMessage(__FILE__,__LINE__,VARG(a),VARG(b),VARG(c),VARG(d),VARG(e),VARG(f),VARG(g),VARG(h),VARG(i),VARG(j),VARG(k),VARG(l));} inlDebugBreak();}
#define nuxAssertMsg(expr,a,b,c,d,e,f,g,h,i,j,k,l)      { if(!(expr)) { nuxFailAssert( TEXT(#expr) TEXT(" : ") a,b,c,d,e,f,g,h,i,j,k,l); } }
#define nuxVerifyExprMsg(expr,a,b,c,d,e,f,g,h,i,j,k,l)  { if(!(expr)) { nuxFailAssert( TEXT(#expr) TEXT(" : ") a,b,c,d,e,f,g,h,i,j,k,l); } }    // Expression is always evaluated. nuxFailAssert is called if enabled.
#define nuxDebugMsg(a,b,c,d,e,f,g,h,i,j,k,l)            { if(nuxIsDebuggerPresent() && LogOutputRedirector::Ready()) GLogDevice.LogFunction(a,b,c,d,e,f,g,h,i,j,k,l); }
#endif

  // Break if codepaths should never be reached.
#define nuxAssertNoEntry()           { nuxFailAssert( TEXT("This section of code should not be executed.") ); }
  // Break if codepaths should not be executed more than once.
#define nuxAssertNoReEntry() \
    { \
        static bool s_inlRuntimeHasBeenHere##__LINE__ = false; \
        nuxAssertMsg( !s_inlRuntimeHasBeenHere##__LINE__, TEXT("This section of code has already been called.") ); \
        s_inlRuntimeHasBeenHere##__LINE__ = true; \
    }

  class NRecursionScopeCounter
  {
  public:
    NRecursionScopeCounter (WORD &InCounter) : Counter ( InCounter )
    {
      ++Counter;
    }
    ~NRecursionScopeCounter()
    {
      --Counter;
    }
  private:
    WORD &Counter;
  };

  // Break if codepaths should never be called recursively.
#define nuxAssertNoRecursion()  \
        static WORD RecursionCounter##__LINE__ = 0; \
        nuxAssertMsg( RecursionCounter##__LINE__ == 0, TEXT("This section of code was entered recursively.") ); \
        const NRecursionScopeCounter ScopeMarker##__LINE__( RecursionCounter##__LINE__ )

  // Compile time assertion. Break if the assertion fails.
  // @param expr  Must be evaluated at compile time.
#define nuxAssertAtCompileTime(expr)  typedef BYTE CompileTimeCheckType##__LINE__[(expr) ? 1 : -1]
#else
#ifdef NUX_MICROSOFT_COMPILER
#define nuxAssert(expr)                     NUX_NOOP
#define nuxVerifyExpr(expr)                 { if(!(expr)) {} }
#define nuxDebugMsg(a, ...)                 NUX_NOOP
#ifdef NUX_VARIADIC_MACROS_SUPPORT
#define nuxAssertMsg(expr, a, ...)      NUX_NOOP
#define nuxVerifyExprMsg(expr, a, ...)  { if(!(expr)) {} }
#define nuxError(a, ...)                NUX_NOOP
#else
#define nuxAssertMsg(expr,a,b,c,d,e,f,g,h,i,j,k,l)      NUX_NOOP
#define nuxVerifyExprMsg(expr,a,b,c,d,e,f,g,h,i,j,k,l)  { if(!(expr)) {} }
#define nuxError(a,b,c,d,e,f,g,h,i,j,k,l)               NUX_NOOP
#endif
#define nuxAssertNoEntry()              NUX_NOOP
#define nuxAssertNoReentry()            NUX_NOOP
#define nuxAssertNoRecursion()          NUX_NOOP
#define nuxAssertAtCompileTime(expr)    NUX_NOOP
#else
#define nuxDebugMsg(a, ...)
#define nuxError(a, ...)                {}
#define nuxAssert(expr)                 {}
#define nuxVerifyExpr(expr)             { if(!(expr)) {} }
#define nuxAssertMsg(expr,msg, ...)     {}
#define nuxVerifyExprMsg(expr, a, ...)  { if(!(expr)) {} }
#define nuxAssertNoEntry()              {}
#define nuxAssertNoReentry()            {}
#define nuxAssertNoRecursion()          {}
#define nuxAssertAtCompileTime(expr)    {}
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// String conversion classes                                            //
//////////////////////////////////////////////////////////////////////////

#ifndef _UNICODE
#define CALL_OS_TCHAR_FUNCTION(funcW,funcA) funcA
#define TCHAR_TO_ANSI(str) str
#define ANSI_TO_TCHAR(str) (const TCHAR*)((const ANSICHAR*)str)

#define UTF8ToTCHAR(str) str
#define TCHARToUTF8(str) str
#define UTF16ToTCHAR(str) (const char*)NUTF8(str)
#define TCHARToUTF16(str) (const wchar_t*)NUTF16(str)
#else
#define CALL_OS_TCHAR_FUNCTION(funcW,funcA) funcW

  /*!
  NOTE: Theses macros creates objects with very short lifespan. They are
  meant to be used as parameters to functions. Do not assign a variable to the content
  of the converted string as the object will go out of scope and the string released.

  Usage:
  SomeApi(TCHAR_TO_ANSI(SomeUnicodeString));
  const char* SomePointer = TCHAR_TO_ANSI(SomeUnicodeString); <--- Bad!!!
  */
#define TCHAR_TO_ANSI(str)  (ANSICHAR*)typedef NCharacterConversion<ANSICHAR, TCHAR, TCharToAnsiConvertion>((const TCHAR*)str)
#define ANSI_TO_TCHAR(str)  (TCHAR*)NCharacterConversion<TCHAR, ANSICHAR, AnsiToTCharConversion>((const ANSICHAR*)str)

#define UTF8ToTCHAR(str) (const wchar_t*)NUTF16(str)
#define TCHARToUTF8(str) (const char*)NUTF8(str)
#define UTF16ToTCHAR(str) str
#define TCHARToUTF16(str) str
#endif

#define inlUTF16ToUTF8(s) (const char*)nux::NUTF8(s)
#define inlUTF8ToUTF16(s) (const wchar_t*)nux::NUTF16(s)

#define ANSICHAR_TO_UNICHAR(str)  (UNICHAR*)  nux::NCharacterConversion <UNICHAR,   ANSICHAR, nux::AnsicharToUnicharConvertion>((const ANSICHAR*)str)
#define UNICHAR_TO_ANSICHAR(str)  (ANSICHAR*) nux::NCharacterConversion <ANSICHAR,  UNICHAR,  nux::UnicharToAnsicharConvertion>((const UNICHAR*)str)
#define ANSICHAR_TO_TCHAR(str)    (UNICHAR*)  nux::NCharacterConversion <TCHAR,     ANSICHAR, nux::AnsiToTCharConversion>((const ANSICHAR*)str)
#define TCHAR_TO_ANSICHAR(str)    (ANSICHAR*) nux::NCharacterConversion <ANSICHAR,  TCHAR,    nux::TCharToAnsiConvertion>((const TCHAR*)str)
#define TCHAR_TO_UNICHAR(str)     (UNICHAR*)  nux::NCharacterConversion <UNICHAR,   TCHAR,    nux::TCharToUnicharConvertion>((const TCHAR*)str)

#define NUX_WIN32_LINE_TERMINATOR   TEXT("\r\n")
#define NUX_UNIX_LINE_TERMINATOR    TEXT("\n")
#define NUX_MACOSX_LINE_TERMINATOR  TEXT("\n")

#if defined(NUX_OS_WINDOWS)
    #define NUX_LINE_TERMINATOR NUX_WIN32_LINE_TERMINATOR
#elif defined(NUX_OS_LINUX) || defined(NUX_OS_MACOSX)
    #define NUX_LINE_TERMINATOR NUX_UNIX_LINE_TERMINATOR
#endif


#if defined(NUX_OS_WINDOWS)
    #define NUX_PATH_SEPARATOR_STRING   NUX_BACKSLASH_STRING
    #define NUX_PATH_SEPARATOR_CHAR     NUX_BACKSLASH_CHAR
#elif defined(NUX_OS_LINUX) || defined(NUX_OS_MACOSX)
    #define NUX_PATH_SEPARATOR_STRING   NUX_SLASH_STRING
    #define NUX_PATH_SEPARATOR_CHAR     NUX_SLASH_CHAR
#endif

#define NUX_BACKSLASH_CHAR      TEXT('\\')
#define NUX_BACKSLASH_STRING    TEXT("\\")
#define NUX_SLASH_CHAR          TEXT('/')
#define NUX_SLASH_STRING        TEXT("/")

#define NUX_MAX_FILEPATH_SIZE   1024
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if (defined _WIN32) && (defined WIN32_SECURE)
#define WCSNCPY_S(strDest, numberOfElements, strSource, count)  wcsncpy_s (strDest, numberOfElements, strSource, count)
#define STRNCPY_S(strDest, numberOfElements, strSource, count)  _tcsncpy_s(strDest, numberOfElements, strSource, count)
#define STRCPY_S(strDest, numberOfElements, strSource)          _tcscpy_s(strDest, numberOfElements, strSource)
#define STRCAT_S(strDest, numberOfElements, strSource)          _tcscat_s(strDest, numberOfElements, strSource)

#define VSNPRINTF_S(strDest, numberOfElements, Count, format, VA_Arg_List)  vsnprintf_s(strDest, numberOfElements, Count, format, VA_Arg_List)
#define VSNTPRINTF_S(strDest, numberOfElements, Count, format, VA_Arg_List) _vsntprintf_s(strDest, numberOfElements, Count, format, VA_Arg_List)
#define SPRINTF_S(strDest, numberOfElements, format, ...)                   _stprintf_s(strDest, numberOfElements, format, ##__VA_ARGS__)
#define SNPRINTF_S(strDest, numberOfElements, Count, format, ...)           _sntprintf_s(strDest, numberOfElements, Count, format, ##__VA_ARGS__)

#define STRDATE_S(strDest, numberOfElements) _tstrdate_s(strDest, numberOfElements)
#define STRTIME_S(strDest, numberOfElements)       _tstrtime_s(strDest, numberOfElements)

#define FOPEN_S(file, filename, mode)           _tfopen_s(file, filename, mode)

#define STRLEN_S(str, numberOfElements)         _tcsnlen(str, numberOfElements)

#define SPLITPATH_S(path, Drive, DriveNumElements, Dir, DirNumElements, Filename, FileNumElements, Extension, ExtNumElements) _tsplitpath_s(path, Drive, DriveNumElements, Dir, DirNumElements, Filename, FileNumElements, Extension, ExtNumElements)
#define MAKEPATH_S(path, numberOfElements, Drive, Dir, Filename, Extension) _tmakepath_s(path, numberOfElements, Drive, Dir, Filename, Extension)

#define SSCANF_S(buffer, format, ...)           _stscanf_s(buffer, format, ##__VA_ARGS__)
#define SNSCANF_S(input, length, format, ...)   _sntscanf_s(input, length, format, ##__VA_ARGS__)
#else
#define WCSNCPY_S(strDest, numberOfElements, strSource, count)  wcsncpy (strDest, strSource, count)

// The weird numElmt - numElmt in the expansion is to stop the compiler from complaining about unused params.
#define STRNCPY_S(strDest, numElmt, strSource, count)  _tcsncpy(strDest, strSource, count + numElmt - numElmt)
#define STRCPY_S(strDest, numberOfElements, strSource)          _tcscpy(strDest, strSource)
#define STRCAT_S(strDest, numberOfElements, strSource)          _tcscat(strDest, strSource)

#define VSNPRINTF_S(strDest, numberOfElements, Count, format, VA_Arg_List)              vsnprintf(strDest, Count, format, VA_Arg_List)
#define VSNTPRINTF_S(strDest, numberOfElements, Count, format, VA_Arg_List)             _vsntprintf(strDest, Count, format, VA_Arg_List)
#define SPRINTF_S(strDest, numberOfElements, format, ...)                               _stprintf(strDest, format, ##__VA_ARGS__)
#define SNPRINTF_S(strDest, numberOfElements, Count, format, ...)                       _sntprintf(strDest, Count, format, ##__VA_ARGS__)

#define STRDATE_S(strDest, numberOfElements)        _tstrdate(strDest)
#define STRTIME_S(strDest, numberOfElements)        _tstrtime(strDest)

#define FOPEN_S(file, filename, mode)               (file = _tfopen(filename, mode))

#define STRLEN_S(str, numberOfElements)             _tcslen(str)

#define SPLITPATH_S(path, Drive, DriveNumElements, Dir, DirNumElements, Filename, FileNumElements, Extension, ExtNumElements) _tsplitpath(path, Drive, Dir, Filename, Extension)
#define MAKEPATH_S(path, numberOfElements, Drive, Dir, Filename, Extension) _makepath(path, Drive, Dir, Filename, Extension)

#define SSCANF_S(buffer, format, ...)           _stscanf(buffer, format, ##__VA_ARGS__)
#define SNSCANF_S(input, length, format, ...)   _sntscanf(input, length, format, ##__VA_ARGS__)
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  extern const bool GNoDialog;         // Set to true to disable the popping of dialog box. The message will go to the log.

#ifdef NUX_VISUAL_STUDIO_2003
  //Visual Studio C++ 2003 doesn't support it, but there is a workaround:
#pragma warning(disable: 4002)    // Warning: too many actual parameters for macro 'ident'
#pragma warning(disable: 4003)    // Warning: not enough actual parameters for macro 'ident'
  template <typename T>
  inline const T   &VARG ( const T &t )
  {
    return t;
  }
  inline const TCHAR  *VARG( )
  {
    return TEXT ("");
  }
#endif

#ifdef _UNICODE
#define tstring std::wstring
#define tostream std::wostream
#define tistream std::wistream
#define tiostream std::wiostream
#define tofstream std::wofstream
#define tfstream std::wfstream
#else
#define tstring std::string
#define tostream std::ostream
#define tistream std::istream
#define tiostream std::iostream
#define tofstream std::ofstream
#define tfstream std::fstream
#endif

// // UTF-16 is the primary encoding mechanism used by Microsoft Windows 2000, Windows 2000 Server, Windows XP and Windows 2003 Server.
// // Unicode Byte Order Mark (BOM)
// enum {UNICODE_UTF32_BE   = 0x0000FEFF };
// enum {UNICODE_UTF32_LE   = 0xFFFE0000 };
// enum {UNICODE_UTF16_BE   = 0xFEFF };
// enum {UNICODE_UTF16_LE   = 0xFFFE };
// enum {UNICODE_UTF8       = 0xEFBBBF };

  const BYTE NUX_UTF32_BE[]   = {0x04 /*size*/, 0x00, 0x00, 0xFE, 0xFF };
  const BYTE NUX_UTF32_LE[]   = {0x04 /*size*/, 0xFF, 0xFE, 0x00, 0x00 };
  const BYTE NUX_UTF16_BE[]   = {0x02 /*size*/, 0xFE, 0xFF };
  const BYTE NUX_UTF16_LE[]   = {0x02 /*size*/, 0xFF, 0xFE };
  const BYTE NUX_UTF8[]       = {0x03 /*size*/, 0xEF, 0xBB, 0xBF };

// enum {UNICODE_BOM   = 0xfeff     };


  class LogOutputDevice;
  class NFileManager;


#define GNullDevice         NUX_GLOBAL_OBJECT_INSTANCE(nux::NullOutput)
#define GLogDevice          NUX_GLOBAL_OBJECT_INSTANCE(nux::LogOutputRedirector)

#if (defined NUX_OS_WINDOWS)
    #define GFileManager    NUX_GLOBAL_OBJECT_INSTANCE(nux::NFileManagerWindows)
#elif (defined NUX_OS_LINUX)
    #define GFileManager    NUX_GLOBAL_OBJECT_INSTANCE(nux::NFileManagerGNU)
#endif

// Define architecture specific asm statements for hardware breakpoint
#if defined(NUX_GNUC_COMPILER)
    #if (defined __i386__) || (defined __x86_64__)
        #define ARCH_HARDWARE_BREAK std::abort ()
    #elif defined (__arm__) || (defined __ppc__)
        #define ARCH_HARDWARE_BREAK do {} while(0)
    #else
        #define ARCH_HARDWARE_BREAK do {} while(0)
    #endif
#endif


// Breaks into the debugger.  Forces a GPF in non-debug builds.
#if (defined NUX_DEBUG) && (defined NUX_MICROSOFT_COMPILER)
    #define nuxIsDebuggerPresent()  IsDebuggerPresent()
    #define inlDebugBreak()         ( IsDebuggerPresent() ? (DebugBreak(),1) : 1 )
#elif (defined _WIN32)
    #define nuxIsDebuggerPresent()  IsDebuggerPresent()
    #define inlDebugBreak()         std::abort ()
#elif (defined NUX_DEBUG) && (defined NUX_GNUCPP_COMPILER)
    #define nuxIsDebuggerPresent()  1
    #define inlDebugBreak()         ARCH_HARDWARE_BREAK
#else
    #define nuxIsDebuggerPresent()  0
    #define inlDebugBreak()
#endif

#if defined(NUX_MICROSOFT_COMPILER)
    #define NUX_HARDWARE_BREAK      {__debugbreak();}
    #define NUX_BREAK_ASM_INT3      {__debugbreak();}
#elif defined(NUX_GNUC_COMPILER)
    #define NUX_HARDWARE_BREAK     ARCH_HARDWARE_BREAK
    #define NUX_BREAK_ASM_INT3     ARCH_HARDWARE_BREAK
#else
    #define NUX_HARDWARE_BREAK
    #define NUX_BREAK_ASM_INT3
#endif


//////////////////////////////////////////////////////////////////////////
//      Variadic function prototypes.
//////////////////////////////////////////////////////////////////////////

#define VARARG_EXTRA(A) A,
#define VARARG_NONE
#define VARARG_PURE =0

#if _MSC_VER

  static inline DWORD         VAType (DWORD dw)
  {
    return dw;
  }
  static inline unsigned char        VAType (unsigned char b)
  {
    return b;
  }
  static inline unsigned int        VAType (unsigned int ui)
  {
    return ui;
  }
  static inline int         VAType (int i)
  {
    return i;
  }
  static inline unsigned long long         VAType (unsigned long long qw)
  {
    return qw;  // possible conflict with size_t when compiling in 64 bits
  }
  static inline long long         VAType (long long sqw)
  {
    return sqw;
  }
  static inline double        VAType (double d)
  {
    return d;
  }
  static inline TCHAR         VAType (TCHAR c)
  {
    return c;
  }
  static inline ANSICHAR     *VAType (ANSICHAR *s)
  {
    return s;
  }
  static inline UNICHAR      *VAType (UNICHAR *s)
  {
    return s;
  }
  template<class T> T        *VAType (T *p)
  {
    return p;
  }
  template<class T> const T  *VAType (const T *p)
  {
    return p;
  }

  //  Declaration of prototypes with lots of arguments
  //  If(the function return nothing)
  //  {
  //      Return = {}
  //      StaticFuncRet = void
  //  }
  //  else
  //  {
  //      Return = return
  //      StaticFuncRet = type0
  //      FuncRet = type1
  //  }
  //
  //  If this is a pure virtual function then PURE is equal to: ==0
  //  ExtraParamDecl is declaration for additional parameters: VARARG_EXTRA(TCHAR* Dest) VARARG_EXTRA(INT Size) VARARG_EXTRA(INT Count)
  //  ExtraParam is the parameters presented in ExtraParamDecl: VARARG_EXTRA(Dest) VARARG_EXTRA(Size) VARARG_EXTRA(Count)
#define VARARG_DECL( FuncRet, StaticFuncRet, Return, FuncName, Pure, FmtType, ExtraParamDecl, ExtraParam ) \
    FuncRet FuncName##__VA(ExtraParamDecl FmtType Fmt, ... ) Pure;  \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt) {Return FuncName##__VA(ExtraParam (Fmt));} \
    template<class T1> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1) {T1 v1=VAType(V1);Return FuncName##__VA(ExtraParam (Fmt),(v1));} \
    template<class T1,class T2> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2) {T1 v1=VAType(V1);T2 v2=VAType(V2);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2));} \
    template<class T1,class T2,class T3> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3));} \
    template<class T1,class T2,class T3,class T4> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4));} \
    template<class T1,class T2,class T3,class T4,class T5> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10,T11 V11) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);T11 v11=VAType(V11);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10),(v11));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10,T11 V11,T12 V12) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);T11 v11=VAType(V11);T12 v12=VAType(V12);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10),(v11),(v12));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10,T11 V11,T12 V12,T13 V13) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);T11 v11=VAType(V11);T12 v12=VAType(V12);T13 v13=VAType(V13);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10),(v11),(v12),(v13));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10,T11 V11,T12 V12,T13 V13,T14 V14) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);T11 v11=VAType(V11);T12 v12=VAType(V12);T13 v13=VAType(V13);T14 v14=VAType(V14);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10),(v11),(v12),(v13),(v14));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10,T11 V11,T12 V12,T13 V13,T14 V14,T15 V15) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);T11 v11=VAType(V11);T12 v12=VAType(V12);T13 v13=VAType(V13);T14 v14=VAType(V14);T15 v15=VAType(V15);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10),(v11),(v12),(v13),(v14),(v15));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10,T11 V11,T12 V12,T13 V13,T14 V14,T15 V15,T16 V16) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);T11 v11=VAType(V11);T12 v12=VAType(V12);T13 v13=VAType(V13);T14 v14=VAType(V14);T15 v15=VAType(V15);T16 v16=VAType(V16);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10),(v11),(v12),(v13),(v14),(v15),(v16));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10,T11 V11,T12 V12,T13 V13,T14 V14,T15 V15,T16 V16,T17 V17) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);T11 v11=VAType(V11);T12 v12=VAType(V12);T13 v13=VAType(V13);T14 v14=VAType(V14);T15 v15=VAType(V15);T16 v16=VAType(V16);T17 v17=VAType(V17);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10),(v11),(v12),(v13),(v14),(v15),(v16),(v17));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10,T11 V11,T12 V12,T13 V13,T14 V14,T15 V15,T16 V16,T17 V17,T18 V18) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);T11 v11=VAType(V11);T12 v12=VAType(V12);T13 v13=VAType(V13);T14 v14=VAType(V14);T15 v15=VAType(V15);T16 v16=VAType(V16);T17 v17=VAType(V17);T18 v18=VAType(V18);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10),(v11),(v12),(v13),(v14),(v15),(v16),(v17),(v18));} \
    template<class T1,class T2,class T3,class T4,class T5,class T6,class T7,class T8,class T9,class T10,class T11,class T12,class T13,class T14,class T15,class T16,class T17,class T18,class T19> \
    StaticFuncRet FuncName(ExtraParamDecl FmtType Fmt,T1 V1,T2 V2,T3 V3,T4 V4,T5 V5,T6 V6,T7 V7,T8 V8,T9 V9,T10 V10,T11 V11,T12 V12,T13 V13,T14 V14,T15 V15,T16 V16,T17 V17,T18 V18,T19 V19) {T1 v1=VAType(V1);T2 v2=VAType(V2);T3 v3=VAType(V3);T4 v4=VAType(V4);T5 v5=VAType(V5);T6 v6=VAType(V6);T7 v7=VAType(V7);T8 v8=VAType(V8);T9 v9=VAType(V9);T10 v10=VAType(V10);T11 v11=VAType(V11);T12 v12=VAType(V12);T13 v13=VAType(V13);T14 v14=VAType(V14);T15 v15=VAType(V15);T16 v16=VAType(V16);T17 v17=VAType(V17);T18 v18=VAType(V18);T19 v19=VAType(V19);Return FuncName##__VA(ExtraParam (Fmt),(v1),(v2),(v3),(v4),(v5),(v6),(v7),(v8),(v9),(v10),(v11),(v12),(v13),(v14),(v15),(v16),(v17),(v18),(v19));}

#define VARARG_BODY( FuncRet, FuncName, FmtType, ExtraParamDecl ) \
    FuncRet FuncName##__VA( ExtraParamDecl  FmtType Fmt, ... )

#else  // !_MSC_VER

#define VARARG_DECL( FuncRet, StaticFuncRet, Return, FuncName, Pure, FmtType, ExtraParamDecl, ExtraParam ) \
    FuncRet FuncName( ExtraParamDecl FmtType Fmt, ... ) Pure
#define VARARG_BODY( FuncRet, FuncName, FmtType, ExtraParamDecl ) \
    FuncRet FuncName( ExtraParamDecl FmtType Fmt, ... )

#endif // _MSC_VER


//! Log an outpout message to console or visual studio output. To be used while the log redirector is not initialized.
  void PrintOutputDebugString (const TCHAR *Format, ...);

//! Log an assertion failure to registered output.
  void LogOutputAssertMessage (const ANSICHAR *File, int Line, const TCHAR *Format = TEXT (""), ...);

//! Log an error message to registered output.
  void LogOutputErrorMessage (const ANSICHAR *File, int Line, const TCHAR *Format = TEXT (""), ...);

//! Log and output message with a severity factor to registered output. Print colored output in XTerm.
  void LogOutputDebugMessage (const TCHAR *Format, ...);

//! Log and output message with a severity factor to registered output. Print colored output in XTerm.
  void LogOutputSeverityMessage (int Severity, const TCHAR *Format/*=TEXT("")*/, ...);

// Returns true is the output redirector is ready
  bool OutputRedirectorReady();


  enum EFileWrite
  {
    FILEWRITE_NOFAIL            = 0x01,
    FILEWRITE_NOREPLACEEXISTING = 0x02,
    FILEWRITE_EVENIFREADONLY    = 0x04,
    FILEWRITE_UNBUFFERED        = 0x08,
    FILEWRITE_APPEND            = 0x10,
    FILEWRITE_ALLOWREAD         = 0x20,
  };

  enum ECopyResult
  {
    COPY_OK                     = 0x00,
    COPY_MISCFAIL               = 0x01,
    COPY_READFAIL               = 0x02,
    COPY_WRITEFAIL              = 0x03,
    COPY_CANCELED               = 0x06,
  };

  enum NUX_STATUS
  {
    NUX_OK,
    NUX_ERROR,
    NUX_FILENOTFOUND,
    NUX_COPYFILE_ERROR,
    NUX_DELETEFILE_ERROR,
  };

}


#include "Macros.h"
#include "Memory.h"

#include "Character/NUni.h"

#if defined(NUX_OS_WINDOWS)
    #include "Character/NUnicode.h"
#elif defined(NUX_OS_LINUX)
    #include "Character/NUnicode.h"
#endif

#include "Template.h"
#include "NumberConversion.h"

#include "TextString.h"

#if defined(NUX_OS_WINDOWS)
    #include "ThreadWin.h"
#elif defined(NUX_OS_LINUX)
    #include "ThreadGNU.h"
#endif

/*#include "Memory/NMemoryAllocatorInterface.h"
#include "Memory/NDefaultMemoryAllocator.h"
#include "Memory/NMemoryHook.h"
#include "Memory/NMemoryAllocator.h"
*/

#include "NUniqueIndex.h"

//#include "GlobalInitializer.h"

#ifdef NUX_OS_WINDOWS
#include "Win32Dialogs/NWin32MessageBox.h"
#endif

#include "Character/NTChar.h"

#include "TimeFunctions.h"
#include "Platform.h"
#include "FileManager/NSerializer.h"
#include "Process.h"

#include "OutputDevice.h"
#include "FileManager/NFileManagerGeneric.h"

#ifdef NUX_OS_WINDOWS
    #include "FileManager/NFileManagerWindows.h"
#elif defined NUX_OS_LINUX
    #include "FileManager/NFileManagerGNU.h"
#endif

#include "FileIO.h"
#include "ObjectType.h"
#include "FileName.h"
#include "Color.h"
#include "Colors.h"
#include "Object.h"

#ifdef NUX_OS_WINDOWS
  #include "Win32Dialogs/NWin32CustomDialog.h"
  #include "Win32Dialogs/NWin32Clipboard.h"
#endif


#include "GlobalInitializer.h"

#endif // NUXCORE_H
