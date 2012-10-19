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


#ifndef NPLATFORM_H
#define NPLATFORM_H

namespace nux
{

  void NuxCoreInitialize (const TCHAR *CommandLine);
  void inlPreInitializePlatform();
  void inlInitializePlatform();
  void inlPostInitializePlatform();
  void inlExitSystem();

  void inlInitRandomGenerator();

  /*-----------------------------------------------------------------------------
  Command line.
  -----------------------------------------------------------------------------*/

//! Get the program command line
  /*!
      Get the program command line.
      @return The program command line.
  */
  std::string GetCmdLine();

//! Get startup directory.
  /*!
      Get startup directory.
      @return The computer name.
  */
  std::string GetProgramDirectory();

//! Get computer name.
  /*!
      Get computer name.
      @return The computer name.
  */
  std::string GetComputerName();

//! Get user name.
  /*!
      Get the user name.
      @return The user name.
  */
  std::string GetUserName();


//! Return the logs directory
  /*!
      Returns the directory where logs files are output.
      This cannot be in a .ini files as the system may start logging before the .ini files are read.

      @return The logs directory
  */
  std::string GetLogDirectory();

  class NGlobalData
  {
    NUX_DECLARE_GLOBAL_OBJECT (NGlobalData, GlobalSingletonInitializer);
  public:

    void Initialize (const TCHAR *CommandLine);

    unsigned int m_RandomSeed;
    std::string m_UserName;
    std::string m_ComputerName;
    std::string m_ProgramName;
    std::string m_ProgramDirectory;
    std::string m_CommandLine;
  };

}

#endif // NPLATFORM_H
