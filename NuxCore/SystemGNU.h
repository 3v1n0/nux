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


#ifndef SYSTEMGNU_H
#define SYSTEMGNU_H

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

// If NUX_LOG_FILE_ANSI is set to 1, log files will be written in ASCII characters even when in UNICODE.
#define NUX_LOG_FILE_ANSI   1


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

#define NUX_INLINE          inline			                    // Force inline code
#define NUX_FORCEINLINE     inline //__attribute__ ((__always_inline))	// Force inline code
#define NUX_FORCENOINLINE   __attribute__ ((noinline))          // Force code to NOT be inline


// Unsigned base types.
typedef unsigned char           BOOL;       // 8-bit  unsigned.
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
typedef float                   FLOAT;      // 32-bit IEEE floating point.
typedef double                  DOUBLE;     // 64-bit IEEE double.
typedef unsigned long           SIZE_T;     // Corresponds to C SIZE_T.

///////////////////////////////////////////////
// UNICODE                                   //
///////////////////////////////////////////////
#ifdef NUX_UNICODE
typedef UNICHAR  TCHAR;
#undef TEXT
#define TEXT(s) L##s
#else
typedef ANSICHAR  TCHAR;
#undef TEXT
#define TEXT(s) s
#endif


#ifdef NUX_UNICODE
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

#ifndef NUX_UNICODE
#ifndef NUX_MBCS
#define NUX_TCHAR_ROUTINE(ansi, mbcs, unicode) ansi
#else
#define NUX_TCHAR_ROUTINE(ansi, mbcs, unicode) mbcs
#endif
#else
#define NUX_TCHAR_ROUTINE(ansi, mbcs, unicode) unicode
#endif


#define	NUX_UNIX_SYS_HOST_ROOT  TEXT("/")
#define	NUX_UNIX_SYS_HOME       TEXT("/home")

///////////////////////////////////////////////
#define __targv       NUX_TCHAR_ROUTINE(__argv,          __argv,      __wargv)
#define _fgettc       NUX_TCHAR_ROUTINE(fgetc,           fgetc,       fgetwc)
#define _fgettchar    NUX_TCHAR_ROUTINE(fgetchar,        fgetchar,    _fgetwchar)
#define _fgetts       NUX_TCHAR_ROUTINE(fgets,           fgets,       fgetws)
#define _fputtc       NUX_TCHAR_ROUTINE(fputc,           fputc,       fputwc)
#define _fputtchar    NUX_TCHAR_ROUTINE(fputchar,        fputchar,    _fputwchar)
#define _fputts       NUX_TCHAR_ROUTINE(fputs,           fputs,       fputws)
#define _ftprintf     NUX_TCHAR_ROUTINE(fprintf,         fprintf,     fwprintf)
#define _ftscanf      NUX_TCHAR_ROUTINE(fscanf,          fscanf,      fwscanf)
#define _gettc        NUX_TCHAR_ROUTINE(getc,            getc,        getwc)
#define _gettchar     NUX_TCHAR_ROUTINE(getchar,         getchar,     getwchar)
#define _getts        NUX_TCHAR_ROUTINE(gets,            gets,        getws)
#define _isalnum      NUX_TCHAR_ROUTINE(isalnum,         _ismbcalnum, iswalnum)
#define _istalpha     NUX_TCHAR_ROUTINE(isalpha,         _ismbcalpha, iswalpha)
#define _istascii     NUX_TCHAR_ROUTINE(isascii,         __isascii,   iswascii)
#define _istcntrl     NUX_TCHAR_ROUTINE(iscntrl,         iscntrl,     iswcntrl)
#define _istdigit     NUX_TCHAR_ROUTINE(isdigit,         _ismbcdigit, iswdigit)
#define _istgraph     NUX_TCHAR_ROUTINE(isgraph,         _ismbcgraph, iswgraph)
#define _istlead      NUX_TCHAR_ROUTINE(WINE_tchar_false,_ismbblead,  WINE_tchar_false)
#define _istleadbyte  NUX_TCHAR_ROUTINE(WINE_tchar_false,isleadbyte,  WINE_tchar_false)
#define _istlegal     NUX_TCHAR_ROUTINE(WINE_tchar_true, _ismbclegal, WINE_tchar_true)
#define _istlower     NUX_TCHAR_ROUTINE(islower,         _ismbcslower,iswlower)
#define _istprint     NUX_TCHAR_ROUTINE(isprint,         _ismbcprint, iswprint)
#define _istpunct     NUX_TCHAR_ROUTINE(ispunct,         _ismbcpunct, iswpunct)
#define _istspace     NUX_TCHAR_ROUTINE(isspace,         _ismbcspace, iswspace)
#define _istupper     NUX_TCHAR_ROUTINE(isupper,         _ismbcupper, iswupper)
#define _istxdigit    NUX_TCHAR_ROUTINE(isxdigit,        isxdigit,    iswxdigit)
#define _itot         NUX_TCHAR_ROUTINE(_itoa,           _itoa,       _itow)
#define _ltot         NUX_TCHAR_ROUTINE(_ltoa,           _ltoa,       _ltow)
#define _puttc        NUX_TCHAR_ROUTINE(putc,            putc,        putwc)
#define _puttchar     NUX_TCHAR_ROUTINE(putchar,         putchar,     putwchar)
#define _putts        NUX_TCHAR_ROUTINE(puts,            puts,        putws)
#define _sntprintf    NUX_TCHAR_ROUTINE(sprintf,         sprintf,     swprintf)
#define _stprintf     NUX_TCHAR_ROUTINE(sprintf,         sprintf,     swprintf)
#define _stscanf      NUX_TCHAR_ROUTINE(sscanf,          sscanf,      swscanf)
#define _taccess      NUX_TCHAR_ROUTINE(access,          _access,     _waccess)
#define _tasctime     NUX_TCHAR_ROUTINE(asctime,         asctime,     _wasctime)
#define _tccpy        NUX_TCHAR_ROUTINE(WINE_tchar_tccpy,_mbccpy,     WINE_tchar_tccpy)
#define _tchdir       NUX_TCHAR_ROUTINE(chdir,           _chdir,      _wchdir)
#define _tclen        NUX_TCHAR_ROUTINE(WINE_tchar_tclen,_mbclen,     WINE_tchar_tclen)
#define _tchmod       NUX_TCHAR_ROUTINE(chmod,           _chmod,      _wchmod)
#define _tcreat       NUX_TCHAR_ROUTINE(creat,           _creat,      _wcreat)
#define _tcscat       NUX_TCHAR_ROUTINE(strcat,          _mbscat,     wcscat)
#define _tcschr       NUX_TCHAR_ROUTINE(strchr,          _mbschr,     wcschr)
#define _tcsclen      NUX_TCHAR_ROUTINE(strlen,          _mbslen,     wcslen)
#define _tcscmp       NUX_TCHAR_ROUTINE(strcmp,          _mbscmp,     wcscmp)
#define _tcscoll      NUX_TCHAR_ROUTINE(strcoll,         _mbscoll,    wcscoll)
#define _tcscpy       NUX_TCHAR_ROUTINE(strcpy,          _mbscpy,     wcscpy)
#define _tcscspn      NUX_TCHAR_ROUTINE(strcspn,         _mbscspn,    wcscspn)
#define _tcsdec       NUX_TCHAR_ROUTINE(_strdec,         _mbsdec,     _wcsdec)
#define _tcsdup       NUX_TCHAR_ROUTINE(strdup,          _mbsdup,     _wcsdup)
#define _tcsftime     NUX_TCHAR_ROUTINE(strftime,        strftime,    wcsftime)
#define _tcsicmp      NUX_TCHAR_ROUTINE(strcasecmp,      _mbsicmp,    _wcsicmp)
#define _tcsicoll     NUX_TCHAR_ROUTINE(_stricoll,       _stricoll,   _wcsicoll)
#define _tcsinc       NUX_TCHAR_ROUTINE(_strinc,         _mbsinc,     _wcsinc)
#define _tcslen       NUX_TCHAR_ROUTINE(strlen,          strlen,      wcslen)
#define _tcslwr       NUX_TCHAR_ROUTINE(strlwr,          _mbslwr,     wcslwr)
#define _tcsnbcnt     NUX_TCHAR_ROUTINE(_strncnt,        _mbsnbcnt,   _wcnscnt)
#define _tcsncat      NUX_TCHAR_ROUTINE(strncat,         _mbsnbcat,   wcsncat)
#define _tcsnccat     NUX_TCHAR_ROUTINE(strncat,         _mbsncat,    wcsncat)
#define _tcsncmp      NUX_TCHAR_ROUTINE(strncmp,         _mbsnbcmp,   wcsncmp)
#define _tcsnccmp     NUX_TCHAR_ROUTINE(strncmp,         _mbsncmp,    wcsncmp)
#define _tcsnccnt     NUX_TCHAR_ROUTINE(_strncnt,        _mbsnccnt,   _wcsncnt)
#define _tcsnccpy     NUX_TCHAR_ROUTINE(strncpy,         _mbsncpy,    wcsncpy)
#define _tcsncicmp    NUX_TCHAR_ROUTINE(_strnicmp,       _mbsnicmp,   _wcsnicmp)
#define _tcsncpy      NUX_TCHAR_ROUTINE(strncpy,         _mbsnbcpy,   wcsncpy)
#define _tcsncset     NUX_TCHAR_ROUTINE(_strnset,        _mbsnset,    _wcsnset)
#define _tcsnextc     NUX_TCHAR_ROUTINE(_strnextc,       _mbsnextc,   _wcsnextc)
#define _tcsnicmp     NUX_TCHAR_ROUTINE(_strnicmp,       _mbsnicmp,   _wcsnicmp)
#define _tcsnicoll    NUX_TCHAR_ROUTINE(_strnicoll,      _strnicoll   _wcsnicoll)
#define _tcsninc      NUX_TCHAR_ROUTINE(_strninc,        _mbsninc,    _wcsninc)
#define _tcsnccnt     NUX_TCHAR_ROUTINE(_strncnt,        _mbsnccnt,   _wcsncnt)
#define _tcsnset      NUX_TCHAR_ROUTINE(_strnset,        _mbsnbset,   _wcsnset)
#define _tcspbrk      NUX_TCHAR_ROUTINE(strpbrk,         _mbspbrk,    wcspbrk)
#define _tcsspnp      NUX_TCHAR_ROUTINE(_strspnp,        _mbsspnp,    _wcsspnp)
#define _tcsrchr      NUX_TCHAR_ROUTINE(strrchr,         _mbsrchr,    wcsrchr)
#define _tcsrev       NUX_TCHAR_ROUTINE(_strrev,         _mbsrev,     _wcsrev)
#define _tcsset       NUX_TCHAR_ROUTINE(_strset,         _mbsset,     _wcsset)
#define _tcsspn       NUX_TCHAR_ROUTINE(strspn,          _mbsspn,     wcsspn)
#define _tcsstr       NUX_TCHAR_ROUTINE(strstr,          _mbsstr,     wcsstr)
#define _tcstod       NUX_TCHAR_ROUTINE(strtod,          strtod,      wcstod)
#define _tcstok       NUX_TCHAR_ROUTINE(strtok,          _mbstok,     wcstok)
#define _tcstol       NUX_TCHAR_ROUTINE(strtol,          strtol,      wcstol)
#define _tcstoul      NUX_TCHAR_ROUTINE(std::strtoul,    strtoul,     std::wcstoul)
#define _tcsupr       NUX_TCHAR_ROUTINE(strupr,          _mbsupr,     wcsupr)
#define _tcsxfrm      NUX_TCHAR_ROUTINE(strxfrm,         strxfrm,     wcsxfrm)
#define _tctime       NUX_TCHAR_ROUTINE(ctime,           ctime,       _wctime)
#define _tenviron     NUX_TCHAR_ROUTINE(_environ,        _environ,    _wenviron)
#define _texecl       NUX_TCHAR_ROUTINE(execl,           _execl,      _wexecl)
#define _texecle      NUX_TCHAR_ROUTINE(execle,          _execle,     _wexecle)
#define _texeclp      NUX_TCHAR_ROUTINE(execlp,          _execlp,     _wexeclp)
#define _texeclpe     NUX_TCHAR_ROUTINE(execlpe,         _execlpe,    _wexeclpe)
#define _texecv       NUX_TCHAR_ROUTINE(execv,           _execv,      _wexecv)
#define _texecve      NUX_TCHAR_ROUTINE(execve,          _execve,     _wexecve)
#define _texecvp      NUX_TCHAR_ROUTINE(execvp,          _execvp,     _wexecvp)
#define _texecvpe     NUX_TCHAR_ROUTINE(execvpe,         _execvpe,    _wexecvpe)
#define _tfdopen      NUX_TCHAR_ROUTINE(fdopen,          _fdopen,     _wfdopen)
#define _tfinddata_t  NUX_TCHAR_ROUTINE(_finddata_t,     _finddata_t, _wfinddata_t)
#define _tfinddatai64_t NUX_TCHAR_ROUTINE(_finddatai64_t,_finddatai64_t,_wfinddatai64_t)
#define _tfindfirst   NUX_TCHAR_ROUTINE(_findfirst,      _findfirst,  _wfindfirst)
#define _tfindnext    NUX_TCHAR_ROUTINE(_findnext,       _findnext,   _wfindnext)
#define _tfopen       NUX_TCHAR_ROUTINE(fopen,           fopen,       _wfopen)
#define _tfreopen     NUX_TCHAR_ROUTINE(freopen,         freopen,     _wfreopen)
#define _tfsopen      NUX_TCHAR_ROUTINE(_fsopen,         _fsopen,     _wfsopen)
#define _tfullpath    NUX_TCHAR_ROUTINE(_fullpath,       _fullpath,   _wfullpath)
#define _tgetcwd      NUX_TCHAR_ROUTINE(getcwd,          _getcwd,     _wgetcwd)
#define _tgetenv      NUX_TCHAR_ROUTINE(getenv,          getenv,      _wgetenv)
#define _tmain        NUX_TCHAR_ROUTINE(main,            main,        wmain)
#define _tmakepath    NUX_TCHAR_ROUTINE(_makepath,       _makepath,   _wmakepath)
#define _tmkdir       NUX_TCHAR_ROUTINE(mkdir,           _mkdir,      _wmkdir)
#define _tmktemp      NUX_TCHAR_ROUTINE(mktemp,          _mktemp,     _wmktemp)
#define _tperror      NUX_TCHAR_ROUTINE(perror,          perror,      _wperror)
#define _topen        NUX_TCHAR_ROUTINE(open,            _open,       _wopen)
#define _totlower     NUX_TCHAR_ROUTINE(std::tolower,    _mbctolower, towlower)
#define _totupper     NUX_TCHAR_ROUTINE(std::toupper,    _mbctoupper, towupper)
#define _tpopen       NUX_TCHAR_ROUTINE(popen,           _popen,      _wpopen)
#define _tprintf      NUX_TCHAR_ROUTINE(printf,          printf,      wprintf)
#define _tremove      NUX_TCHAR_ROUTINE(remove,          remove,      _wremove)
#define _trename      NUX_TCHAR_ROUTINE(rename,          rename,      _wrename)
#define _trmdir       NUX_TCHAR_ROUTINE(rmdir,           _rmdir,      _wrmdir)
#define _tsearchenv   NUX_TCHAR_ROUTINE(_searchenv,      _searchenv,  _wsearchenv)
#define _tscanf       NUX_TCHAR_ROUTINE(scanf,           scanf,       wscanf)
#define _tsetlocale   NUX_TCHAR_ROUTINE(setlocale,       setlocale,   _wsetlocale)
#define _tsopen       NUX_TCHAR_ROUTINE(_sopen,          _sopen,      _wsopen)
#define _tspawnl      NUX_TCHAR_ROUTINE(_spawnl,         _spawnl,     _wspawnl)
#define _tspawnle     NUX_TCHAR_ROUTINE(_spawnle,        _spawnle,    _wspawnle)
#define _tspawnlp     NUX_TCHAR_ROUTINE(_spawnlp,        _spawnlp,    _wspawnlp)
#define _tspawnlpe    NUX_TCHAR_ROUTINE(_spawnlpe,       _spawnlpe,   _wspawnlpe)
#define _tspawnv      NUX_TCHAR_ROUTINE(_spawnv,         _spawnv,     _wspawnv)
#define _tspawnve     NUX_TCHAR_ROUTINE(_spawnve,        _spawnve,    _wspawnve)
#define _tspawnvp     NUX_TCHAR_ROUTINE(_spawnvp,        _spawnvp,    _tspawnvp)
#define _tspawnvpe    NUX_TCHAR_ROUTINE(_spawnvpe,       _spawnvpe,   _tspawnvpe)
#define _tsplitpath   NUX_TCHAR_ROUTINE(_splitpath,      _splitpath,  _wsplitpath)
#define _tstat        NUX_TCHAR_ROUTINE(_stat,           _stat,       _wstat)
#define _tstrdate     NUX_TCHAR_ROUTINE(_strdate,        _strdate,    _wstrdate)
#define _tstrtime     NUX_TCHAR_ROUTINE(_strtime,        _strtime,    _wstrtime)
#define _tsystem      NUX_TCHAR_ROUTINE(system,          system,      _wsystem)
#define _ttempnam     NUX_TCHAR_ROUTINE(tempnam,         _tempnam,    _wtempnam)
#define _ttmpnam      NUX_TCHAR_ROUTINE(tmpnam,          tmpnam,      _wtmpnam)
#define _ttoi         NUX_TCHAR_ROUTINE(atoi,            atoi,        _wtoi)
#define _ttol         NUX_TCHAR_ROUTINE(atol,            atol,        _wtol)
#define _tutime       NUX_TCHAR_ROUTINE(utime,           _utime,      _wutime)
#define _tWinMain     NUX_TCHAR_ROUTINE(WinMain,         WinMain,     wWinMain)
#define _ultot        NUX_TCHAR_ROUTINE(_ultoa,          _ultoa,      _ultow)
#define _ungettc      NUX_TCHAR_ROUTINE(ungetc,          ungetc,      ungetwc)
#define _vftprintf    NUX_TCHAR_ROUTINE(vfprintf,        vfprintf,    vfwprintf)
#define _vsntprintf   NUX_TCHAR_ROUTINE(vsnprintf,       _vsnprintf,  _vsnwprintf)
#define _vstprintf    NUX_TCHAR_ROUTINE(vsprintf,        vsprintf,    vswprintf)
#define _vtprintf     NUX_TCHAR_ROUTINE(vprintf,         vprintf,     vwprintf)
#define _TEOF         NUX_TCHAR_ROUTINE(EOF,             EOF,         WEOF)



#endif // SYSTEMGNU_H


