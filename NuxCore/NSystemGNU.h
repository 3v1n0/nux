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


#ifndef SYSTEMPS3_H
#define SYSTEMPS3_H

#include <cassert>  // (assert.h)
#include <cctype>   // (ctype.h)
#include <cerrno>   // (errno.h)
#include <cfloat>   // (float.h)
#include <ciso646>  // (iso646.h)
#include <climits>  // (limits.h)
#include <clocale>  // (locale.h)
#include <cmath>    // (math.h)
//#include <csetjmp>  // (setjmp.h) conflicts with libpng on linux.
#include <csignal>  // (signal.h)
#include <cstdarg>  // (stdarg.h)
#include <cstddef>  // (stddef.h)
#include <cstdio>   // (stdio.h)
#include <cstdlib>  // (stdlib.h)
#include <cstring>  // (string.h)
#include <ctime>    // (time.h)
#include <cwchar>   // (wchar.h)
#include <cwctype>  // (wctype.h)

#include <utime.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sched.h>
#include <inttypes.h>
#include <fcntl.h>

#include <glib.h>
#include <pthread.h>

// If INL_LOG_FILE_ANSI is set to 1, log files will be written in ASCII characters even when in UNICODE.
#define INL_LOG_FILE_ANSI   1


#define INL_VARARGS     __cdecl					// Functions with variable arguments

// Calling Convention
// This is the default calling convention for C and C++ programs.
// Because the stack is cleaned up by the caller, it can do vararg functions.
// Argument-passing order: Right to left
#define INL_CDECL	    __cdecl
// The __stdcall calling convention is used to call Win32 API functions.
// The callee cleans the stack, so the compiler makes vararg functions __cdecl.
// Argument-passing order: Right to left
#define INL_STDCALL		__stdcall
// The __fastcall calling convention specifies that arguments to functions are to be passed in registers, when possible.
#define INL_FASTCALL    __fastcall
// This is the default calling convention used by C++ member functions that do not use variable arguments.
// Under thiscall, the callee cleans the stack, which is impossible for vararg functions.
#define INL_THISCALL    thiscall                //

#define INL_INLINE          inline			                    // Force inline code
#define INL_FORCEINLINE     inline //__attribute__ ((__always_inline))	// Force inline code
#define INL_FORCENOINLINE   __attribute__ ((noinline))          // Force code to NOT be inline


// Unsigned base types.
typedef unsigned char           BYTE;       // 8-bit  unsigned.
typedef unsigned short          WORD;       // 16-bit unsigned.
typedef unsigned int            UINT;       // 32-bit unsigned.
typedef unsigned long           DWORD;      // 32-bit unsigned.
typedef          uint64_t       QWORD;      // 64-bit unsigned.

// Signed base types.
typedef	signed char             SBYTE;      // 8-bit  signed.
typedef signed short            SWORD;      // 16-bit signed.
typedef signed int              INT;        // 32-bit signed.
typedef        int64_t          SQWORD;     // 64-bit signed.


// Character types.
typedef char                    ANSICHAR;   // An ANSI character.
typedef unsigned char           ANSIUCHAR;  // An ANSI character.
typedef wchar_t                 UNICHAR;    // A unicode character.

// Other base types.
typedef unsigned char           BOOL;       // Boolean 0 (false) or 1 (true).
typedef long                    UBOOL;      // Boolean 0 (false) or 1 (true).
typedef float                   FLOAT;      // 32-bit IEEE floating point.
typedef double                  DOUBLE;     // 64-bit IEEE double.
typedef unsigned long           SIZE_T;     // Corresponds to C SIZE_T.

///////////////////////////////////////////////
// UNICODE                                   //
///////////////////////////////////////////////
#ifdef INL_UNICODE
    typedef UNICHAR  TCHAR;
    #undef TEXT
    #define TEXT(s) L##s
#else
    typedef ANSICHAR  TCHAR;
    #undef TEXT
    #define TEXT(s) s
#endif


#ifdef INL_UNICODE
    #define WINE_tchar_true(a) (1)
    #define WINE_tchar_false(a) (0)
    #define WINE_tchar_tclen(a) (1)
    #define WINE_tchar_tccpy(a,b) do { *(a)=*(b); } while (0)
#else
    #define WINE_tchar_true(a) (1)
    #define WINE_tchar_false(a) (0)
    #define WINE_tchar_tclen(a) (1)
    #define WINE_tchar_tccpy(a,b) do { *(a)=*(b); } while (0)
#endif

#ifndef INL_UNICODE
    #ifndef INL_MBCS
        #define INL_TCHAR_ROUTINE(ansi, mbcs, unicode) ansi
    #else
        #define INL_TCHAR_ROUTINE(ansi, mbcs, unicode) mbcs
    #endif
#else
    #define INL_TCHAR_ROUTINE(ansi, mbcs, unicode) unicode
#endif


#define	INL_UNIX_SYS_HOST_ROOT  TEXT("/")
#define	INL_UNIX_SYS_HOME       TEXT("/home")

///////////////////////////////////////////////
#define __targv       INL_TCHAR_ROUTINE(__argv,          __argv,      __wargv)
#define _fgettc       INL_TCHAR_ROUTINE(fgetc,           fgetc,       fgetwc)
#define _fgettchar    INL_TCHAR_ROUTINE(fgetchar,        fgetchar,    _fgetwchar)
#define _fgetts       INL_TCHAR_ROUTINE(fgets,           fgets,       fgetws)
#define _fputtc       INL_TCHAR_ROUTINE(fputc,           fputc,       fputwc)
#define _fputtchar    INL_TCHAR_ROUTINE(fputchar,        fputchar,    _fputwchar)
#define _fputts       INL_TCHAR_ROUTINE(fputs,           fputs,       fputws)
#define _ftprintf     INL_TCHAR_ROUTINE(fprintf,         fprintf,     fwprintf)
#define _ftscanf      INL_TCHAR_ROUTINE(fscanf,          fscanf,      fwscanf)
#define _gettc        INL_TCHAR_ROUTINE(getc,            getc,        getwc)
#define _gettchar     INL_TCHAR_ROUTINE(getchar,         getchar,     getwchar)
#define _getts        INL_TCHAR_ROUTINE(gets,            gets,        getws)
#define _isalnum      INL_TCHAR_ROUTINE(isalnum,         _ismbcalnum, iswalnum)
#define _istalpha     INL_TCHAR_ROUTINE(isalpha,         _ismbcalpha, iswalpha)
#define _istascii     INL_TCHAR_ROUTINE(isascii,         __isascii,   iswascii)
#define _istcntrl     INL_TCHAR_ROUTINE(iscntrl,         iscntrl,     iswcntrl)
#define _istdigit     INL_TCHAR_ROUTINE(isdigit,         _ismbcdigit, iswdigit)
#define _istgraph     INL_TCHAR_ROUTINE(isgraph,         _ismbcgraph, iswgraph)
#define _istlead      INL_TCHAR_ROUTINE(WINE_tchar_false,_ismbblead,  WINE_tchar_false)
#define _istleadbyte  INL_TCHAR_ROUTINE(WINE_tchar_false,isleadbyte,  WINE_tchar_false)
#define _istlegal     INL_TCHAR_ROUTINE(WINE_tchar_true, _ismbclegal, WINE_tchar_true)
#define _istlower     INL_TCHAR_ROUTINE(islower,         _ismbcslower,iswlower)
#define _istprint     INL_TCHAR_ROUTINE(isprint,         _ismbcprint, iswprint)
#define _istpunct     INL_TCHAR_ROUTINE(ispunct,         _ismbcpunct, iswpunct)
#define _istspace     INL_TCHAR_ROUTINE(isspace,         _ismbcspace, iswspace)
#define _istupper     INL_TCHAR_ROUTINE(isupper,         _ismbcupper, iswupper)
#define _istxdigit    INL_TCHAR_ROUTINE(isxdigit,        isxdigit,    iswxdigit)
#define _itot         INL_TCHAR_ROUTINE(_itoa,           _itoa,       _itow)
#define _ltot         INL_TCHAR_ROUTINE(_ltoa,           _ltoa,       _ltow)
#define _puttc        INL_TCHAR_ROUTINE(putc,            putc,        putwc)
#define _puttchar     INL_TCHAR_ROUTINE(putchar,         putchar,     putwchar)
#define _putts        INL_TCHAR_ROUTINE(puts,            puts,        putws)
#define _sntprintf    INL_TCHAR_ROUTINE(sprintf,         sprintf,     swprintf)
#define _stprintf     INL_TCHAR_ROUTINE(sprintf,         sprintf,     swprintf)
#define _stscanf      INL_TCHAR_ROUTINE(sscanf,          sscanf,      swscanf)
#define _taccess      INL_TCHAR_ROUTINE(access,          _access,     _waccess)
#define _tasctime     INL_TCHAR_ROUTINE(asctime,         asctime,     _wasctime)
#define _tccpy        INL_TCHAR_ROUTINE(WINE_tchar_tccpy,_mbccpy,     WINE_tchar_tccpy)
#define _tchdir       INL_TCHAR_ROUTINE(chdir,           _chdir,      _wchdir)
#define _tclen        INL_TCHAR_ROUTINE(WINE_tchar_tclen,_mbclen,     WINE_tchar_tclen)
#define _tchmod       INL_TCHAR_ROUTINE(chmod,           _chmod,      _wchmod)
#define _tcreat       INL_TCHAR_ROUTINE(creat,           _creat,      _wcreat)
#define _tcscat       INL_TCHAR_ROUTINE(strcat,          _mbscat,     wcscat)
#define _tcschr       INL_TCHAR_ROUTINE(strchr,          _mbschr,     wcschr)
#define _tcsclen      INL_TCHAR_ROUTINE(strlen,          _mbslen,     wcslen)
#define _tcscmp       INL_TCHAR_ROUTINE(strcmp,          _mbscmp,     wcscmp)
#define _tcscoll      INL_TCHAR_ROUTINE(strcoll,         _mbscoll,    wcscoll)
#define _tcscpy       INL_TCHAR_ROUTINE(strcpy,          _mbscpy,     wcscpy)
#define _tcscspn      INL_TCHAR_ROUTINE(strcspn,         _mbscspn,    wcscspn)
#define _tcsdec       INL_TCHAR_ROUTINE(_strdec,         _mbsdec,     _wcsdec)
#define _tcsdup       INL_TCHAR_ROUTINE(strdup,          _mbsdup,     _wcsdup)
#define _tcsftime     INL_TCHAR_ROUTINE(strftime,        strftime,    wcsftime)
#define _tcsicmp      INL_TCHAR_ROUTINE(strcasecmp,      _mbsicmp,    _wcsicmp)
#define _tcsicoll     INL_TCHAR_ROUTINE(_stricoll,       _stricoll,   _wcsicoll)
#define _tcsinc       INL_TCHAR_ROUTINE(_strinc,         _mbsinc,     _wcsinc)
#define _tcslen       INL_TCHAR_ROUTINE(strlen,          strlen,      wcslen)
#define _tcslwr       INL_TCHAR_ROUTINE(strlwr,          _mbslwr,     wcslwr)
#define _tcsnbcnt     INL_TCHAR_ROUTINE(_strncnt,        _mbsnbcnt,   _wcnscnt)
#define _tcsncat      INL_TCHAR_ROUTINE(strncat,         _mbsnbcat,   wcsncat)
#define _tcsnccat     INL_TCHAR_ROUTINE(strncat,         _mbsncat,    wcsncat)
#define _tcsncmp      INL_TCHAR_ROUTINE(strncmp,         _mbsnbcmp,   wcsncmp)
#define _tcsnccmp     INL_TCHAR_ROUTINE(strncmp,         _mbsncmp,    wcsncmp)
#define _tcsnccnt     INL_TCHAR_ROUTINE(_strncnt,        _mbsnccnt,   _wcsncnt)
#define _tcsnccpy     INL_TCHAR_ROUTINE(strncpy,         _mbsncpy,    wcsncpy)
#define _tcsncicmp    INL_TCHAR_ROUTINE(_strnicmp,       _mbsnicmp,   _wcsnicmp)
#define _tcsncpy      INL_TCHAR_ROUTINE(strncpy,         _mbsnbcpy,   wcsncpy)
#define _tcsncset     INL_TCHAR_ROUTINE(_strnset,        _mbsnset,    _wcsnset)
#define _tcsnextc     INL_TCHAR_ROUTINE(_strnextc,       _mbsnextc,   _wcsnextc)
#define _tcsnicmp     INL_TCHAR_ROUTINE(_strnicmp,       _mbsnicmp,   _wcsnicmp)
#define _tcsnicoll    INL_TCHAR_ROUTINE(_strnicoll,      _strnicoll   _wcsnicoll)
#define _tcsninc      INL_TCHAR_ROUTINE(_strninc,        _mbsninc,    _wcsninc)
#define _tcsnccnt     INL_TCHAR_ROUTINE(_strncnt,        _mbsnccnt,   _wcsncnt)
#define _tcsnset      INL_TCHAR_ROUTINE(_strnset,        _mbsnbset,   _wcsnset)
#define _tcspbrk      INL_TCHAR_ROUTINE(strpbrk,         _mbspbrk,    wcspbrk)
#define _tcsspnp      INL_TCHAR_ROUTINE(_strspnp,        _mbsspnp,    _wcsspnp)
#define _tcsrchr      INL_TCHAR_ROUTINE(strrchr,         _mbsrchr,    wcsrchr)
#define _tcsrev       INL_TCHAR_ROUTINE(_strrev,         _mbsrev,     _wcsrev)
#define _tcsset       INL_TCHAR_ROUTINE(_strset,         _mbsset,     _wcsset)
#define _tcsspn       INL_TCHAR_ROUTINE(strspn,          _mbsspn,     wcsspn)
#define _tcsstr       INL_TCHAR_ROUTINE(strstr,          _mbsstr,     wcsstr)
#define _tcstod       INL_TCHAR_ROUTINE(strtod,          strtod,      wcstod)
#define _tcstok       INL_TCHAR_ROUTINE(strtok,          _mbstok,     wcstok)
#define _tcstol       INL_TCHAR_ROUTINE(strtol,          strtol,      wcstol)
#define _tcstoul      INL_TCHAR_ROUTINE(std::strtoul,    strtoul,     std::wcstoul)
#define _tcsupr       INL_TCHAR_ROUTINE(strupr,          _mbsupr,     wcsupr)
#define _tcsxfrm      INL_TCHAR_ROUTINE(strxfrm,         strxfrm,     wcsxfrm)
#define _tctime       INL_TCHAR_ROUTINE(ctime,           ctime,       _wctime)
#define _tenviron     INL_TCHAR_ROUTINE(_environ,        _environ,    _wenviron)
#define _texecl       INL_TCHAR_ROUTINE(execl,           _execl,      _wexecl)
#define _texecle      INL_TCHAR_ROUTINE(execle,          _execle,     _wexecle)
#define _texeclp      INL_TCHAR_ROUTINE(execlp,          _execlp,     _wexeclp)
#define _texeclpe     INL_TCHAR_ROUTINE(execlpe,         _execlpe,    _wexeclpe)
#define _texecv       INL_TCHAR_ROUTINE(execv,           _execv,      _wexecv)
#define _texecve      INL_TCHAR_ROUTINE(execve,          _execve,     _wexecve)
#define _texecvp      INL_TCHAR_ROUTINE(execvp,          _execvp,     _wexecvp)
#define _texecvpe     INL_TCHAR_ROUTINE(execvpe,         _execvpe,    _wexecvpe)
#define _tfdopen      INL_TCHAR_ROUTINE(fdopen,          _fdopen,     _wfdopen)
#define _tfinddata_t  INL_TCHAR_ROUTINE(_finddata_t,     _finddata_t, _wfinddata_t)
#define _tfinddatai64_t INL_TCHAR_ROUTINE(_finddatai64_t,_finddatai64_t,_wfinddatai64_t)
#define _tfindfirst   INL_TCHAR_ROUTINE(_findfirst,      _findfirst,  _wfindfirst)
#define _tfindnext    INL_TCHAR_ROUTINE(_findnext,       _findnext,   _wfindnext)
#define _tfopen       INL_TCHAR_ROUTINE(fopen,           fopen,       _wfopen)
#define _tfreopen     INL_TCHAR_ROUTINE(freopen,         freopen,     _wfreopen)
#define _tfsopen      INL_TCHAR_ROUTINE(_fsopen,         _fsopen,     _wfsopen)
#define _tfullpath    INL_TCHAR_ROUTINE(_fullpath,       _fullpath,   _wfullpath)
#define _tgetcwd      INL_TCHAR_ROUTINE(getcwd,          _getcwd,     _wgetcwd)
#define _tgetenv      INL_TCHAR_ROUTINE(getenv,          getenv,      _wgetenv)
#define _tmain        INL_TCHAR_ROUTINE(main,            main,        wmain)
#define _tmakepath    INL_TCHAR_ROUTINE(_makepath,       _makepath,   _wmakepath)
#define _tmkdir       INL_TCHAR_ROUTINE(mkdir,           _mkdir,      _wmkdir)
#define _tmktemp      INL_TCHAR_ROUTINE(mktemp,          _mktemp,     _wmktemp)
#define _tperror      INL_TCHAR_ROUTINE(perror,          perror,      _wperror)
#define _topen        INL_TCHAR_ROUTINE(open,            _open,       _wopen)
#define _totlower     INL_TCHAR_ROUTINE(std::tolower,    _mbctolower, towlower)
#define _totupper     INL_TCHAR_ROUTINE(std::toupper,    _mbctoupper, towupper)
#define _tpopen       INL_TCHAR_ROUTINE(popen,           _popen,      _wpopen)
#define _tprintf      INL_TCHAR_ROUTINE(printf,          printf,      wprintf)
#define _tremove      INL_TCHAR_ROUTINE(remove,          remove,      _wremove)
#define _trename      INL_TCHAR_ROUTINE(rename,          rename,      _wrename)
#define _trmdir       INL_TCHAR_ROUTINE(rmdir,           _rmdir,      _wrmdir)
#define _tsearchenv   INL_TCHAR_ROUTINE(_searchenv,      _searchenv,  _wsearchenv)
#define _tscanf       INL_TCHAR_ROUTINE(scanf,           scanf,       wscanf)
#define _tsetlocale   INL_TCHAR_ROUTINE(setlocale,       setlocale,   _wsetlocale)
#define _tsopen       INL_TCHAR_ROUTINE(_sopen,          _sopen,      _wsopen)
#define _tspawnl      INL_TCHAR_ROUTINE(_spawnl,         _spawnl,     _wspawnl)
#define _tspawnle     INL_TCHAR_ROUTINE(_spawnle,        _spawnle,    _wspawnle)
#define _tspawnlp     INL_TCHAR_ROUTINE(_spawnlp,        _spawnlp,    _wspawnlp)
#define _tspawnlpe    INL_TCHAR_ROUTINE(_spawnlpe,       _spawnlpe,   _wspawnlpe)
#define _tspawnv      INL_TCHAR_ROUTINE(_spawnv,         _spawnv,     _wspawnv)
#define _tspawnve     INL_TCHAR_ROUTINE(_spawnve,        _spawnve,    _wspawnve)
#define _tspawnvp     INL_TCHAR_ROUTINE(_spawnvp,        _spawnvp,    _tspawnvp)
#define _tspawnvpe    INL_TCHAR_ROUTINE(_spawnvpe,       _spawnvpe,   _tspawnvpe)
#define _tsplitpath   INL_TCHAR_ROUTINE(_splitpath,      _splitpath,  _wsplitpath)
#define _tstat        INL_TCHAR_ROUTINE(_stat,           _stat,       _wstat)
#define _tstrdate     INL_TCHAR_ROUTINE(_strdate,        _strdate,    _wstrdate)
#define _tstrtime     INL_TCHAR_ROUTINE(_strtime,        _strtime,    _wstrtime)
#define _tsystem      INL_TCHAR_ROUTINE(system,          system,      _wsystem)
#define _ttempnam     INL_TCHAR_ROUTINE(tempnam,         _tempnam,    _wtempnam)
#define _ttmpnam      INL_TCHAR_ROUTINE(tmpnam,          tmpnam,      _wtmpnam)
#define _ttoi         INL_TCHAR_ROUTINE(atoi,            atoi,        _wtoi)
#define _ttol         INL_TCHAR_ROUTINE(atol,            atol,        _wtol)
#define _tutime       INL_TCHAR_ROUTINE(utime,           _utime,      _wutime)
#define _tWinMain     INL_TCHAR_ROUTINE(WinMain,         WinMain,     wWinMain)
#define _ultot        INL_TCHAR_ROUTINE(_ultoa,          _ultoa,      _ultow)
#define _ungettc      INL_TCHAR_ROUTINE(ungetc,          ungetc,      ungetwc)
#define _vftprintf    INL_TCHAR_ROUTINE(vfprintf,        vfprintf,    vfwprintf)
#define _vsntprintf   INL_TCHAR_ROUTINE(vsnprintf,       _vsnprintf,  _vsnwprintf)
#define _vstprintf    INL_TCHAR_ROUTINE(vsprintf,        vsprintf,    vswprintf)
#define _vtprintf     INL_TCHAR_ROUTINE(vprintf,         vprintf,     vwprintf)
#define _TEOF         INL_TCHAR_ROUTINE(EOF,             EOF,         WEOF)



#endif // SYSTEMPS3_H


