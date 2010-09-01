#ifndef NPROCESS_H
#define NPROCESS_H

NAMESPACE_BEGIN

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
    INL_DECLARE_GLOBAL_OBJECT(NProcess, NGlobalSingletonInitializer);
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

NAMESPACE_END

#endif // NPROCESS_H