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


#ifndef NPROCESS_H
#define NPROCESS_H

namespace nux { //NUX_NAMESPACE_BEGIN

#ifdef _WIN32
/**
* Launches a uniform resource locator (i.e. http://www.yahoo.com/finance).
* This is expected to return immediately as the URL is launched by another task.
*/
void inlLaunchURL( const TCHAR* URL, const TCHAR* Parms=NULL, NString* Error=NULL );
void* inlCreateProc( const TCHAR* URL, const TCHAR* Parms );
//! Get process termination code of a process
BOOL inlGetProcReturnCode( void* ProcessHandle, INT* ReturnCode );

#endif

class NProcess
{
    NUX_DECLARE_GLOBAL_OBJECT(NProcess, NGlobalSingletonInitializer);
public:
    
    DWORD GetProcessID();
    DWORD GetMainThreadID();
    DWORD GetCurrentThreadID();
private:

#ifdef _WIN32
    HANDLE GetProcessHandle();
    HANDLE GetMainThreadHandle();
    HANDLE GetCurrentThreadHandle();

    HANDLE m_ProcessHandle;
    DWORD m_ProcessID;
    HANDLE m_MainThreadHandle;
    DWORD m_MainThreadID;
#endif

};

} //NUX_NAMESPACE_END

#endif // NPROCESS_H