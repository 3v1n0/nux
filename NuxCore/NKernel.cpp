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


#include "NKernel.h"

namespace nux { //NUX_NAMESPACE_BEGIN

//  MessageBox resolves to:
//      MessageBoxA, if UNICODE is not defined
//      MessageBoxW, if UNICODE is defined

//  In all cases in the code, it is assumed that we have UNICODE and _UNICODE defined or not.
//  We have no support for cases of  (UNICODE, !_UNICODE) or (!UNICODE, _UNICODE)
//  TCHAR  resolves:
//      char, if _UNICODE is not defined
//      wchar_t, if _UNICODE is defined 


// Set to true to disable the popping of dialog box. The message will go to the log.
const t_bool GNoDialog          = false;

/*-----------------------------------------------------------------------------
Formatted printing and messages.
-----------------------------------------------------------------------------*/

t_u32 GetVariableArgs( TCHAR* Dest, t_u32 Size, t_u32 Count, const TCHAR*& Fmt, va_list ArgPtr )
{
    t_u32 Result = VSNTPRINTF_S(Dest, Size, Count, Fmt, ArgPtr);
    va_end(ArgPtr);
    return Result;
}
t_u32 GetVariableArgsAnsi( ANSICHAR* Dest, t_u32 Size, t_u32 Count, const ANSICHAR*& Fmt, va_list ArgPtr)
{
    t_u32 Result = VSNPRINTF_S(Dest, Size, Count, Fmt, ArgPtr);
    va_end(ArgPtr);
    return Result;
}

// Formats the text for inlOutputDebugString.
// This function can be used to print anything before the other output are initialized.
void inlOutputDebugString( const TCHAR *Format, ... )
{
    TCHAR TempStr[4096];
    GET_VARARGS( TempStr, 4096, NUX_ARRAY_COUNT(TempStr) - 1, Format );

#ifdef _WIN32
    OutputDebugString( TempStr );
#else
    printf("%s\n", TCHAR_TO_ANSI(TempStr));
#endif
}

void LogOutputAssertMessage( const ANSICHAR* File, int Line, const TCHAR* Format/*=TEXT("")*/, ... )
{
    TCHAR TempStr[4096];
    GET_VARARGS( TempStr, NUX_ARRAY_COUNT(TempStr), NUX_ARRAY_COUNT(TempStr) - 1, Format );
    // Logged to a file... Put "\r\n" at the end of each line.
    if(NOutputDeviceRedirector::Ready())
        GLogDevice.LogFunction( TEXT("Assertion failed: %s\r\n    [File:%s]\r\n    [Line: %i]\r\n"), (const TCHAR*)TempStr, ANSI_TO_TCHAR(File), Line);
}

void LogOutputErrorMessage( const ANSICHAR* File, int Line, const TCHAR* Format/*=TEXT("")*/, ... )
{
    TCHAR TempStr[4096];
    GET_VARARGS( TempStr, NUX_ARRAY_COUNT(TempStr), NUX_ARRAY_COUNT(TempStr) - 1, Format );
    if(NOutputDeviceRedirector::Ready())
        GLogDevice.LogFunction( TEXT("Error: %s\r\n    [File:%s]\r\n    [Line: %d]\r\n"), (const TCHAR*)TempStr, ANSI_TO_TCHAR(File), Line);
}

void LogOutputDebugMessage(const TCHAR* Format/*=TEXT("")*/, ... )
{
    TCHAR TempStr[4096];
    GET_VARARGS( TempStr, NUX_ARRAY_COUNT(TempStr), NUX_ARRAY_COUNT(TempStr) - 1, Format );
    if(NOutputDeviceRedirector::Ready())
        GLogDevice.LogFunction(TempStr);
}

bool inlOutputRedirectorReady()
{
    return NOutputDeviceRedirector::Ready();
}

} //NUX_NAMESPACE_END
