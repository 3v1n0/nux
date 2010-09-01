#include "NKernel.h"

#pragma pack(push,8)
#include <string.h>
#include <TlHelp32.h>				// For module info.
#include <DbgHelp.h>				// For stack walker.
#include <psapi.h>
#pragma pack(pop)


NAMESPACE_BEGIN

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

NAMESPACE_END

