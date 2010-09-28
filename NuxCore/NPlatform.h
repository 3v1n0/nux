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


#ifndef NPLATFORM_H
#define NPLATFORM_H

namespace nux { //NUX_NAMESPACE_BEGIN

void NuxCoreInitialize(const TCHAR* CommandLine);
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
NString GetCmdLine();

//! Get startup directory.
/*!
    Get startup directory.
    @return The computer name.
*/
NString GetProgramDirectory();

//! Get computer name.
/*!
    Get computer name.
    @return The computer name.
*/
NString GetComputerName();

//! Get user name.
/*!
    Get the user name.
    @return The user name.
*/
NString GetUserName();


//! Return the logs directory
/*! 
    Returns the directory where logs files are output.
    This cannot be in a .ini files as the system may start logging before the .ini files are read.

    @return The logs directory
*/
NString GetLogDirectory();

class NGlobalData
{
    NUX_DECLARE_GLOBAL_OBJECT(NGlobalData, NGlobalSingletonInitializer);
public:

    void Initialize(const TCHAR* CommandLine);

    t_u32 m_RandomSeed;
    NString m_UserName;
    NString m_ComputerName;
    NString m_ProgramName;
    NString m_ProgramDirectory;
    NString m_CommandLine;
};

} //NUX_NAMESPACE_END

#endif // NPLATFORM_H