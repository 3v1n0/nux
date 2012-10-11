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

#if defined(NUX_OS_LINUX)
#include <pwd.h>
#endif

namespace nux
{

  NUX_IMPLEMENT_GLOBAL_OBJECT (NGlobalData);

  void NuxCoreInitialize (const TCHAR *CommandLine)
  {
    static bool sInitialized = false;

    // Avoid initializing multiple times.
    if (sInitialized)
      return;

    sInitialized = true;

    NUX_GLOBAL_OBJECT_INSTANCE (NGlobalData).Initialize (CommandLine);

    NThreadLocalStorage::Initialize();
  }


  void ExitSystem()
  {
    //SystemShutdown();
  }

  void inlInitRandomGenerator()
  {
#if _WIN32
    std::srand ( time (NULL) );
#endif

  }

  static std::string _GetProgramDirectory()
  {
#if defined(NUX_OS_WINDOWS)
    TCHAR RootDirectory[NUX_MAX_FILEPATH_SIZE] = TEXT ("");

    if (!RootDirectory[0])
    {
      DWORD Result = ::GetModuleFileName (NULL, RootDirectory, NUX_MAX_FILEPATH_SIZE);
      nuxAssertMsg (Result, TEXT ("[GetProgramDirectory] Can't get program's directory path.") );

      if (Result == 0)
        std::string (TEXT ("Unknown Program Directory") );

      unsigned int i;

      // Skip the program name
      for (i = (unsigned int) StringLength (RootDirectory) - 1; i > 0; i--)
      {
        if ( (RootDirectory[i - 1] == NUX_BACKSLASH_CHAR) || (RootDirectory[i-1] == TEXT ('/') ) )
          break;
      }

      RootDirectory[i] = 0;
    }

    return std::string (RootDirectory);

#elif defined(NUX_OS_LINUX)

    TCHAR RootDirectory[NUX_MAX_FILEPATH_SIZE] = TEXT ("");

    if (!RootDirectory[0])
    {
      char *Result = getcwd (RootDirectory, NUX_MAX_FILEPATH_SIZE);
      nuxAssertMsg (Result, TEXT ("[GetProgramDirectory] Can't get program's directory path.") );

      if (Result == 0)
        std::string (TEXT ("Unknown Program Directory") );

    }

    nuxDebugMsg (TEXT ("[GetProgramDirectory] Program directory path: %s"), RootDirectory);
    return std::string (RootDirectory);

#else
    return std::string (TEXT ("Unknown Program Directory") );
#endif
  }

  static std::string _GetComputerName()
  {
#if defined(NUX_OS_WINDOWS)
    TCHAR ComputerName[NUX_MAX_FILEPATH_SIZE] = TEXT ("");

    if (!ComputerName[0])
    {
      DWORD Size = NUX_ARRAY_COUNT (ComputerName);
      ::GetComputerName (ComputerName, &Size);

      TCHAR *c, *d;

      for (c = ComputerName, d = ComputerName; *c != 0; c++)
      {
        if (IsAlphanumericChar (*c) )
          *d++ = *c;
      }

      *d++ = 0;
    }

    return std::string (ComputerName);

#elif defined(NUX_OS_LINUX)
    char Buffer[NUX_MAX_FILEPATH_SIZE];
    size_t BufferSize = NUX_ARRAY_COUNT (Buffer);

    if (gethostname (Buffer, BufferSize) != -1)
    {
      return std::string (Buffer);
    }

    return std::string (TEXT ("Unknown Computer Name") );

#else
    return std::string (TEXT ("Unknown Computer Name") );
#endif
  }

// Get user name.  NOTE: Only one return value is valid at a time!
  static std::string _GetUserName()
  {
#if defined(NUX_OS_WINDOWS)
    TCHAR UserName[256] = TEXT ("");

    if ( !UserName[0] )
    {
      DWORD Size = NUX_ARRAY_COUNT (UserName);
      ::GetUserName (UserName, &Size);
      TCHAR *c, *d;

      for (c = UserName, d = UserName; *c != 0; c++)
        if (IsAlphanumericChar (*c) )
          *d++ = *c;

      *d++ = 0;
    }

    return std::string (UserName);

#elif defined(NUX_OS_LINUX)
    struct passwd *userinfo;
    userinfo = getpwuid (getuid() );

    if (userinfo == 0)
      return std::string (TEXT ("Unknown User") );

    return std::string (userinfo->pw_name);

#else
    return return std::string (TEXT ("Unknown User") );
#endif
  }

  void NGlobalData::Initialize (const TCHAR * /* CommandLine */)
  {

  }

  void NGlobalData::Constructor()
  {
    m_ComputerName      = _GetComputerName();
    m_ProgramDirectory  = _GetProgramDirectory();
    m_UserName          = _GetUserName();
    m_RandomSeed        = 0x5A7CF91E;   // arbitrary

    std::srand (m_RandomSeed);
  }

  void NGlobalData::Destructor()
  {

  }

  std::string GetComputerName()
  {
    return NUX_GLOBAL_OBJECT_INSTANCE (NGlobalData).m_ComputerName;
  }

  std::string GetProgramDirectory()
  {
    return NUX_GLOBAL_OBJECT_INSTANCE (NGlobalData).m_ProgramDirectory;
  }

  std::string GetUserName()
  {
    return NUX_GLOBAL_OBJECT_INSTANCE (NGlobalData).m_UserName;
  }

  std::string GetCmdLine()
  {
    return NUX_GLOBAL_OBJECT_INSTANCE (NGlobalData).m_CommandLine;
  }

  std::string GetLogDirectory()
  {
    return TEXT ("Logs");
  }

}
