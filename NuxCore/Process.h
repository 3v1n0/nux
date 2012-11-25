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


#ifndef NPROCESS_H
#define NPROCESS_H

namespace nux
{

#ifdef _WIN32
  /**
  * Launches a uniform resource locator (i.e. http://www.yahoo.com/finance).
  * This is expected to return immediately as the URL is launched by another task.
  */
  void inlLaunchURL ( const TCHAR* URL, const TCHAR* Parms, std::string Error);
  void *inlCreateProc ( const TCHAR* URL, const TCHAR* Parms );
//! Get process termination code of a process
  BOOL inlGetProcReturnCode ( void *ProcessHandle, INT *ReturnCode );

#endif

  class NProcess
  {
    NUX_DECLARE_GLOBAL_OBJECT (NProcess, GlobalSingletonInitializer);
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

}

#endif // NPROCESS_H
