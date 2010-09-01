#ifndef NPLATFORM_H
#define NPLATFORM_H

NAMESPACE_BEGIN

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
    INL_DECLARE_GLOBAL_OBJECT(NGlobalData, NGlobalSingletonInitializer);
public:

    void Initialize(const TCHAR* CommandLine);

    t_u32 m_RandomSeed;
    NString m_UserName;
    NString m_ComputerName;
    NString m_ProgramName;
    NString m_ProgramDirectory;
    NString m_CommandLine;
};

NAMESPACE_END

#endif // NPLATFORM_H