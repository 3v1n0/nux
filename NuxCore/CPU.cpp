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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore.h"

#ifdef _WIN32
#include <intrin.h>

namespace nux
{

  NUX_IMPLEMENT_GLOBAL_OBJECT (NCPU);

//============================================================================
// Calculate and log the amount of RAM in the machine
//============================================================================
  static t_u64 GetTotalPhysRAM_MB()
  {
    // Get memory status
    MEMORYSTATUSEX theStatus;
    ZeroMemory (&theStatus, sizeof (theStatus) );
    theStatus.dwLength = sizeof (theStatus);
    GlobalMemoryStatusEx (&theStatus);

    return theStatus.ullTotalPhys / (1024 * 1024);
  }

//============================================================================
// Read the CPU speed from the registry
//============================================================================
  static DWORD GetCPUSpeedFromRegistry (DWORD dwCPU)
  {
    HKEY hKey;
    DWORD dwSpeed;

    // Get the key name
    TCHAR szKey[256];
    _sntprintf_s (szKey, 256, sizeof (szKey) / sizeof (wchar_t), TEXT ("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d\\"), dwCPU);

    // Open the key
    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, szKey, 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
    {
      return 0;
    }

    // Read the value
    DWORD dwLen = 4;

    if (RegQueryValueEx (hKey, TEXT ("~MHz"), NULL, NULL, (LPBYTE) &dwSpeed, &dwLen) != ERROR_SUCCESS)
    {
      RegCloseKey (hKey);
      return 0;
    }

    // Cleanup and return
    RegCloseKey (hKey);
    return dwSpeed;
  }

  void NCPU::Constructor()
  {
    unsigned nHighestFeature;
    unsigned nHighestFeatureEx;
    int nBuff[4];
    char szMan[13];

    Memset (&m_CPUInfo, 0, sizeof (m_CPUInfo) );
    Memset (&m_CPUFeatures, 0, sizeof (m_CPUFeatures) );

    // Get CPU manufacturer and highest CPUID
    __cpuid (nBuff, 0);

    nHighestFeature = (unsigned) nBuff[0];
    * (int *) &szMan[0] = nBuff[1];
    * (int *) &szMan[4] = nBuff[3];
    * (int *) &szMan[8] = nBuff[2];
    szMan[12] = 0;

    if (strcmp (szMan, "AuthenticAMD") == 0)
    {
      m_CPUInfo.m_CPUType = CPU_AMD;
    }
    else if (strcmp (szMan, "GenuineIntel") == 0)
    {
      m_CPUInfo.m_CPUType = CPU_INTEL;
    }
    else
    {
      m_CPUInfo.m_CPUType = CPU_UNKNOWN;
    }

    m_CPUString = szMan;

    // Get highest extended feature
    __cpuid (nBuff, 0x80000000);
    nHighestFeatureEx = (unsigned) nBuff[0];

    // Get processor brand name
    if (nHighestFeatureEx >= 0x80000004)
    {
      char szCPUName[49];
      szCPUName[0] = 0;
      __cpuid ( (int *) &szCPUName[0], 0x80000002);
      __cpuid ( (int *) &szCPUName[16], 0x80000003);
      __cpuid ( (int *) &szCPUName[32], 0x80000004);
      szCPUName[48] = 0;

      m_CPUBrandString = szCPUName;
//         for(int i=(int)strlen(szCPUName)-1; i>=0; --i)
//         {
//             if(szCPUName[i] == ' ')
//                 szCPUName[i] = '\0';
//             else
//                 break;
//         }
    }

    // Get CPU features
    if (nHighestFeature >= 1)
    {
      __cpuid (nBuff, 1);


      m_CPUInfo.m_CPUSteppingID = nBuff[0] & 0xf;
      m_CPUInfo.m_CPUModel = (nBuff[0] >> 4) & 0xf;
      m_CPUInfo.m_CPUFamily = (nBuff[0] >> 8) & 0xf;
      //m_CPUInfo.nProcessorType = (nBuff[0] >> 12) & 0x3;
      m_CPUInfo.m_CPUExtModel = (nBuff[0] >> 16) & 0xf;
      m_CPUInfo.m_CPUExtFamily = (nBuff[0] >> 20) & 0xff;
      //nBrandIndex = nBuff[1] & 0xff;
      m_CPUInfo.m_CPUNumLogicalProcessors = ( (nBuff[1] >> 16) & 0xff);




      if (nBuff[3] & 1 << 0)
        m_CPUFeatures.m_HasFPU = true;

      if (nBuff[3] & 1 << 23)
        m_CPUFeatures.m_HasMMX = true;

      if (nBuff[3] & 1 << 25)
        m_CPUFeatures.m_HasSSE = true;

      if (nBuff[3] & 1 << 26)
        m_CPUFeatures.m_HasSSE2 = true;

      if (nBuff[2] & 1 << 0)
        m_CPUFeatures.m_HasSSE3 = true;

      if (nBuff[2] & 1 << 19)
        m_CPUFeatures.m_HasSSE41 = true;

      if (nBuff[2] & 1 << 20)
        m_CPUFeatures.m_HasSSE42 = true;

      // Intel specific:
      if (m_CPUInfo.m_CPUType == CPU_INTEL)
      {
        if (nBuff[2] & 1 << 9)
          m_CPUFeatures.m_HasSSSE3 = true;

        if (nBuff[2] & 1 << 7)
          m_CPUFeatures.m_HasEST = true;
      }

      if (nBuff[3] & 1 << 28)
        m_CPUFeatures.m_HasHTT = true;
    }

    // AMD specific:
    if (m_CPUInfo.m_CPUType == CPU_AMD)
    {
      // Get extended features
      __cpuid (nBuff, 0x80000000);

      if (nHighestFeatureEx >= 0x80000001)
      {
        __cpuid (nBuff, 0x80000001);

        if (nBuff[3] & 1 << 31)
          m_CPUFeatures.m_Has3DNow = true;

        if (nBuff[3] & 1 << 30)
          m_CPUFeatures.m_Has3DNowExt = true;

        if (nBuff[3] & 1 << 22)
          m_CPUFeatures.m_HasAMDMMX = true;
      }

//         // Get level 1 cache size
//         if(nHighestFeatureEx >= 0x80000005)
//         {
//             __cpuid(nBuff, 0x80000005);
//             ELog::Get().SystemFormat(L"PERF    : L1 cache size: %dK\n", ((unsigned)nBuff[2])>>24);
//         }
    }

    if (nHighestFeatureEx >= 0x80000001)
    {
      __cpuid (nBuff, 0x80000001);

      if (nBuff[3] & 1 << 29)
        m_CPUFeatures.m_Hasx64 = true;

    }

    m_CPUInfo.m_CPUSpeed = GetCPUSpeedFromRegistry (0);
    m_MemInfo.m_PhysicalMemorySize = GetTotalPhysRAM_MB();
    GetOS();

    unsigned int dw = 0x00000001;
    m_IsLittleEndian = (* ( (char *) &dw ) ? true : false);

    //m_MemInfo
//     // Get cache size
//     if(nHighestFeatureEx >= 0x80000006)
//     {
//         __cpuid(nBuff, 0x80000006);
//         ELog::Get().SystemFormat(L"PERF    : L2 cache size: %dK\n", ((unsigned)nBuff[2])>>16);
//     }
//
//     // Log features
//     ELog::Get().SystemFormat(L"PERF    : CPU Features: %S\n", szFeatures);
//
//     // Get misc system info
//     SYSTEM_INFO theInfo;
//     GetSystemInfo(&theInfo);
//
//     // Log number of CPUs and speeds
//     ELog::Get().SystemFormat(L"PERF    : Number of CPUs: %d\n", theInfo.dwNumberOfProcessors);
//     for(DWORD i=0; i<theInfo.dwNumberOfProcessors; ++i)
//     {
//         DWORD dwCPUSpeed = ReadCPUSpeedFromRegistry(i);
//         ELog::Get().SystemFormat(L"PERF    : * CPU %d speed: ~%dMHz\n", i, dwCPUSpeed);
//     }
  }


  void NCPU::Destructor()
  {

  }

  typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
  LPFN_ISWOW64PROCESS fnIsWow64Process;


//============================================================================
// Determine OS type
//============================================================================
  void NCPU::GetOS()
  {
    void *ptr = 0;
    int PointerSize = sizeof (ptr);
    OSVERSIONINFOEX osInfo;
    SYSTEM_INFO sysInfo;

    // Get system info
    ZeroMemory (&sysInfo, sizeof (sysInfo) );
    GetSystemInfo (&sysInfo);

    // Get OS version
    ZeroMemory (&osInfo, sizeof (osInfo) );
    osInfo.dwOSVersionInfoSize = sizeof (osInfo);

    if (!GetVersionEx ( (OSVERSIONINFO *) &osInfo) )
    {
      ZeroMemory (&osInfo, sizeof (osInfo) );
      osInfo.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
      GetVersionEx ( (OSVERSIONINFO *) &osInfo);
    }

    // Win 9x
    if (osInfo.dwPlatformId == 1)
    {
      if ( (osInfo.dwMajorVersion == 4) && (osInfo.dwMinorVersion == 0) )
        m_OSInfo.m_OSType = OS_WIN95;
      else if ( (osInfo.dwMajorVersion == 4) && (osInfo.dwMinorVersion == 10) )
        m_OSInfo.m_OSType = OS_WIN98;
      else if ( (osInfo.dwMajorVersion == 4) && (osInfo.dwMinorVersion == 90) )
        m_OSInfo.m_OSType = OS_WINME;
      else
        m_OSInfo.m_OSType = OS_UNKNOWN;
    }

    // Win NT
    else if (osInfo.dwPlatformId == 2)
    {
      if ( (osInfo.dwMajorVersion == 4) && (osInfo.dwMinorVersion == 0) )
      {
        m_OSInfo.m_OSType = OS_WINNT40;
      }
      else if ( (osInfo.dwMajorVersion == 5) && (osInfo.dwMinorVersion == 0) )
      {
        m_OSInfo.m_OSType = OS_WIN2000;
      }
      else if ( (osInfo.dwMajorVersion == 5) && (osInfo.dwMinorVersion == 1) )
      {
        m_OSInfo.m_OSType = OS_WINXP;
      }
      else if ( (osInfo.dwMajorVersion == 5) && (osInfo.dwMinorVersion == 2) )
      {
        if (GetSystemMetrics (89) )	// SM_SERVERR2 == 89
        {
          m_OSInfo.m_OSType = OS_WINSERVER2003R2;
        }
        else if ( (osInfo.wProductType == VER_NT_WORKSTATION) &&
                  (sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64) )
        {
          m_OSInfo.m_OSType = OS_WINXP64;
        }
        else
        {
          m_OSInfo.m_OSType = OS_WINSERVER2003;
        }
      }
      else if ( (osInfo.dwMajorVersion == 6) && (osInfo.dwMinorVersion == 0) )
      {
        m_OSInfo.m_OSType = OS_WINVISTA;
        BOOL bIsWow64 = FALSE;
        fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress (GetModuleHandle (TEXT ("kernel32") ), "IsWow64Process");

        if (fnIsWow64Process != NULL)
        {
          if (!fnIsWow64Process (GetCurrentProcess(), &bIsWow64) )
          {
            // handle error
            nuxCriticalMsg(TEXT("[NCPU::GetOS] Failed to getOS info."));
            return;
          }
        }

        if ( (PointerSize == 4) && bIsWow64)
        {
          // This is Vista 64 bits
          m_OSInfo.m_OSType = OS_WINVISTA64;
        }

        if (PointerSize == 8)
        {
          // This is Vista 64 bits
          m_OSInfo.m_OSType = OS_WINVISTA64;
        }
      }
      else
      {
        m_OSInfo.m_OSType = OS_UNKNOWN;
      }
    }
    else
    {
      m_OSInfo.m_OSType = OS_UNKNOWN;
    }
  }

  bool NCPU::IsBigEndian()
  {
    return !m_IsLittleEndian;
  }

  bool NCPU::IsLittleEndian()
  {
    return m_IsLittleEndian;
  }

}

#endif // _WIN32

