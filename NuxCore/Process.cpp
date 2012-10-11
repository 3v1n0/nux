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


#include "NuxCore.h"

namespace nux
{

#ifdef _WIN32
//
// Launch a uniform resource locator (i.e. http://www.yahoo.com/finance).
// This is expected to return immediately as the URL is launched by another
// task.
//
  void inlLaunchURL(const TCHAR *URL, const TCHAR *Parms, std::string Error)
  {
    nuxDebugMsg ( TEXT ("LaunchURL %s %s"), URL, Parms ? Parms : TEXT ("") );
    HINSTANCE Code = CALL_OS_TCHAR_FUNCTION (ShellExecuteW (NULL, TEXT ("open"), URL, Parms ? Parms : TEXT (""), TEXT (""), SW_SHOWNORMAL), ShellExecuteA (NULL, "open", TCHAR_TO_ANSI (URL), Parms ? TCHAR_TO_ANSI (Parms) : "", "", SW_SHOWNORMAL) );

    Error = (int) Code <= 32 ? TEXT ("UrlFailed") : TEXT ("");
  }

//
// Creates a new process and its primary thread. The new process runs the
// specified executable file in the security context of the calling process.
//
  void *inlCreateProc ( const TCHAR *URL, const TCHAR *Parms )
  {
    nuxDebugMsg (  TEXT ("CreateProc %s %s"), URL, Parms );

    TCHAR CommandLine[1024];
    Snprintf ( CommandLine, 1024, 1024 - 1, TEXT ("%s %s"), URL, Parms );

    PROCESS_INFORMATION ProcInfo;
    SECURITY_ATTRIBUTES Attr;
    Attr.nLength = sizeof (SECURITY_ATTRIBUTES);
    Attr.lpSecurityDescriptor = NULL;
    Attr.bInheritHandle = TRUE;

    STARTUPINFO StartupInfo = { sizeof (STARTUPINFO), NULL, NULL, NULL,
                                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                NULL, NULL, NULL, NULL, SW_HIDE, NULL, NULL,
                                NULL, NULL, NULL
                              };

    if ( !CreateProcess ( NULL, CommandLine, &Attr, &Attr, TRUE, DETACHED_PROCESS | REALTIME_PRIORITY_CLASS,
                          NULL, NULL, &StartupInfo, &ProcInfo ) )
      return NULL;

    return (void *) ProcInfo.hProcess;
  }

//
// Retrieves the termination status of the specified process.
//
  BOOL inlGetProcReturnCode ( void *ProcHandle, INT *ReturnCode )
  {
    return GetExitCodeProcess ( (HANDLE) ProcHandle, (DWORD *) ReturnCode ) && * ( (DWORD *) ReturnCode) != STILL_ACTIVE;
  }


  NUX_IMPLEMENT_GLOBAL_OBJECT (NProcess);

  void NProcess::Constructor()
  {
    m_ProcessID = GetCurrentProcessId();
    m_ProcessHandle = OpenProcess (PROCESS_ALL_ACCESS, FALSE, m_ProcessID);

    m_MainThreadID = GetCurrentThreadId();
    m_MainThreadHandle = OpenThread (THREAD_ALL_ACCESS, FALSE, m_MainThreadID);
  }

  void NProcess::Destructor()
  {
    CloseHandle (m_MainThreadHandle);
    CloseHandle (m_ProcessHandle);
  }

  HANDLE NProcess::GetProcessHandle()
  {
    return m_ProcessHandle;
  }

  DWORD NProcess::GetProcessID()
  {
    return m_ProcessID;
  }

  HANDLE NProcess::GetMainThreadHandle()
  {
    return m_MainThreadHandle;
  }

  DWORD NProcess::GetMainThreadID()
  {
    return m_MainThreadID;
  }

  HANDLE NProcess::GetCurrentThreadHandle()
  {
    DWORD ThreadID = GetCurrentThreadId();
    return OpenThread (THREAD_ALL_ACCESS, FALSE, ThreadID);
  }

  DWORD NProcess::GetCurrentThreadID()
  {
    return GetCurrentThreadId();
  }

#endif

}

