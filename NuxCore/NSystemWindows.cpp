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

#pragma pack(push,8)
#include <string.h>
#include <TlHelp32.h>
#include <DbgHelp.h>
#include <psapi.h>
#pragma pack(pop)


namespace nux { //NUX_NAMESPACE_BEGIN

void* GetDllHandle( const TCHAR* Filename )
{
    nuxAssert(Filename);	
    return CALL_OS_TCHAR_FUNCTION(LoadLibraryW(Filename),LoadLibraryA(TCHAR_TO_ANSI(Filename)));
}

//
// Free a DLL.
//
void FreeDllHandleappFreeDllHandle( void* DllHandle )
{
    nuxAssert(DllHandle);
    FreeLibrary( (HMODULE)DllHandle );
}

//
// Lookup the address of a DLL function.
//
void* GetDllExport( void* DllHandle, const TCHAR* ProcName )
{
    nuxAssert(DllHandle);
    nuxAssert(ProcName);
    return (void*)GetProcAddress( (HMODULE)DllHandle, TCHAR_TO_ANSI(ProcName) );
}

} //NUX_NAMESPACE_END

