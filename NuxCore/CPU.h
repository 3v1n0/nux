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


#ifndef NCPU_H
#define NCPU_H

namespace nux
{

  class NCPU
  {
    NUX_DECLARE_GLOBAL_OBJECT (NCPU, GlobalSingletonInitializer);
  public:

    enum
    {
      CPU_UNKNOWN = 0,
      CPU_INTEL,
      CPU_AMD
    };

    enum
    {
      OS_UNKNOWN = 0,
      OS_WIN95,
      OS_WIN98,
      OS_WINME,
      OS_WINNT40,
      OS_WIN2000,
      OS_WINXP,
      OS_WINXP64,
      OS_WINSERVER2003,
      OS_WINSERVER2003R2,
      OS_WINVISTA,
      OS_WINVISTA64,
      OS_MACOS,
      OS_LINUX,
    };

    NString m_CPUString;  // GenuineIntel or AuthenticAMD
    NString m_CPUBrandString;

    struct CPUInfo
    {
      t_u32 m_CPUType;
      t_char m_CPUModel;
      t_char m_CPUExtModel;
      t_u32 m_CPUFamily;
      t_u32 m_CPUExtFamily;
      t_char m_CPUSteppingID;
      t_u32 m_CPUNumLogicalProcessors;
      t_u32 m_CPUSpeed;
    };
    struct CPUInfo m_CPUInfo;

    struct CPUFeatures
    {
      bool m_HasFPU;
      bool m_HasMMX;
      bool m_HasSSE;
      bool m_HasSSE2;
      bool m_HasSSE3;
      bool m_HasSSSE3;
      bool m_HasSSE41;
      bool m_HasSSE42;
      bool m_HasEST;          // Enhanced Intel SpeedStep Technology (Intel)
      bool m_HasHTT;          // Multithreading
      bool m_Has3DNow;        // 3Dnow! instructions (AMD)
      bool m_Has3DNowExt;     // 3DnowExt (AMD)
      bool m_HasAMDMMX;       // Extensions to MMX instructions (AMD)
      bool m_Hasx64;

    };
    struct CPUFeatures m_CPUFeatures;

    struct MemInfo
    {
      t_u32 m_PhysicalMemorySize;
    };
    struct MemInfo m_MemInfo;

    struct OSInfo
    {
      t_u32 m_OSType;
    };

    struct OSInfo m_OSInfo;

    bool IsBigEndian();
    bool IsLittleEndian();
  protected:
    bool m_IsLittleEndian;
    void GetOS();

  };

}

#endif // NCPU_H
