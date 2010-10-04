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


#include "NKernel.h"
#include "NParsing.h"

namespace nux { //NUX_NAMESPACE_BEGIN

#if (defined DEBUG) || (defined _DEBUG) || (defined Debug)
//! Create a backup copy of a file if it exist. The copy feature a timestamp in the filename.
/*!
    @param Filename The name of the file to check.
*/
static void CreateBackupCopy(const TCHAR* Filename)
{
    if(GFileManager.FileSize(Filename) > 0)
    {
        // Create string with system time to create a unique filename.
        unsigned int Year, Month, Day, Hour, Min, Sec, MSec;
        GetLocalTime(Year, Month, Day, Hour, Min, Sec, MSec);
        NString Name, Extension;
        NString(Filename).SplitAtLastOccurenceOf(TEXT("."), Name, Extension);
        NString BackupFilename;
        if(Extension.Size())
        {
            BackupFilename = NString::Printf(TEXT("%s-backup-%i.%02i.%02i-%02i.%02i.%02i.%s"), Name.GetTCharPtr(), Year, Month, Day, Hour, Min, Sec, Extension.GetTCharPtr());
        }
        else
        {
            BackupFilename = NString::Printf(TEXT("%s-backup-%i.%02i.%02i-%02i.%02i.%02i"), Name.GetTCharPtr(), Year, Month, Day, Hour, Min, Sec);
        }
        GFileManager.Copy(BackupFilename.GetTCharPtr(), Filename, true, true, NULL);
    }
}
#endif

NUX_IMPLEMENT_GLOBAL_OBJECT(NNullOutput);
NUX_IMPLEMENT_GLOBAL_OBJECT(LogOutputRedirector);
NUX_IMPLEMENT_GLOBAL_OBJECT(NOutputLogFile);
NUX_IMPLEMENT_GLOBAL_OBJECT(NOutputVisualDebugConsole)

void LogOutputDevice::Shutdown()
{
    m_ObjectDestroyed = TRUE;
}

void LogOutputDevice::Flush()
{

}

VARARG_BODY( void /*FuncRet*/, LogOutputDevice::LogFunction/*FuncName*/, const TCHAR* /*FmtType*/, VARARG_EXTRA(int Severity)/*ExtraDecl*/)
{
    if(m_ObjectDestroyed)
        return;

    t_int   BufferSize	= 1024;
    t_int   NewBufferSize	= 0;
    TCHAR*  Buffer		= NULL;
    t_int   Result		= -1;

    while(Result == -1)
    {
        if(NewBufferSize)
        {
            TCHAR* temp = new TCHAR[NewBufferSize];
            Memcpy (temp, Buffer, BufferSize);
            NUX_SAFE_DELETE_ARRAY(Buffer);
            Buffer = temp;
            BufferSize = NewBufferSize;
        }
        else
        {
            Buffer = new TCHAR[BufferSize];
        }
        GET_VARARGS_RESULT(Buffer, BufferSize, BufferSize-1, Fmt, Result);
        if(Result == -1)
            NewBufferSize = 2 * BufferSize;
    };
    Buffer[Result] = 0;
    Serialize(Buffer, TEXT("Log"), Severity);

    NUX_SAFE_DELETE_ARRAY(Buffer);
}

void NOutputLogFile::Constructor()
{
    m_LogSerializer = NULL;
    m_Opened = false;
    m_Closed = false;

#if (defined DEBUG) || (defined _DEBUG) || (defined Debug)
    // The Editor requires a fully qualified directory to not end up putting the log in various directories.
    m_Filename = GetProgramDirectory();
    if((m_Filename[m_Filename.Size()-1] != NUX_SLASH_CHAR) || (m_Filename[m_Filename.Size()-1] != NUX_BACKSLASH_CHAR))
        m_Filename += (const TCHAR*)NUX_PATH_SEPARATOR_STRING;
    m_Filename += GetLogDirectory();

    // Create the directory tree where the Logs file will be stored.
    GFileManager.MakeDirectory(m_Filename.GetTCharPtr(), 1);

    m_Filename += (const TCHAR*)NUX_PATH_SEPARATOR_STRING;
    m_Filename += TEXT("inalogic");
    m_Filename += TEXT(".log");

    // if the file already exists, create a backup as we are going to overwrite it
    if (!m_Opened)
    {
        CreateBackupCopy(m_Filename.GetTCharPtr());
    }

    // Open log file.
    m_LogSerializer = GFileManager.CreateFileWriter(m_Filename.GetTCharPtr(), NSerializer::Read|NSerializer::Write|NSerializer::OverWriteReadOnly|(m_Opened ? NSerializer::Append : 0));

    if(m_LogSerializer)
    {
        m_Opened = true;
#if UNICODE && !NUX_LOG_FILE_ANSI
        m_LogSerializer->Serialize( (void*)&NUX_UTF16_BE[1], NUX_UTF16_BE[0] /*size*/ );
#endif
        LogFunction(NUX_MSG_SEVERITY_NONE, TEXT("Log file open, %s"), GetFormattedLocalTime());
    }
    else
    {
        m_Closed = true;
    }
#endif

}

void NOutputLogFile::Destructor()
{
    Shutdown();
}

/*!
    Closes output device and cleans up. This can't happen in the destructor
    as we have to call "delete" which cannot be done for static/ global objects.
*/
void NOutputLogFile::Shutdown()
{
    if(m_LogSerializer)
    {
        LogFunction(NUX_MSG_SEVERITY_NONE, TEXT("Log file closed, %s"), GetFormattedLocalTime());
        Flush();
        delete m_LogSerializer;
        m_LogSerializer = NULL;
    }
    m_Closed = true;
}

void NOutputLogFile::Flush()
{
    if(m_LogSerializer)
    {
        m_LogSerializer->Flush();
    }
}


//! Write data to the output log file.
/*!
    @param  Data        Text to log.
    @param  LogPrefix	Prefix for the text
*/
void NOutputLogFile::Serialize(const TCHAR* Data, const TCHAR* LogPrefix, int Severity)
{
    if(m_ObjectDestroyed)
        return;

    if(m_LogSerializer)
    {
#if UNICODE && NUX_LOG_FILE_ANSI
        ANSICHAR ACh[1024];
        INT DataOffset = 0;
        INT i;
        while(Data[DataOffset])
        {
            for(i = 0;i < NUX_ARRAY_COUNT(ACh) && Data[DataOffset];i++,DataOffset++)
            {
                ACh[i] = ConvertTCHARToAnsiChar(Data[DataOffset]);
            }
            // serialize chunks of 1024 characters
            m_LogSerializer->Serialize(ACh,i);
        };

        for(i = 0; NUX_LINE_TERMINATOR[i]; i++)
        {
            ACh[i] = NUX_LINE_TERMINATOR[i];
        }

        m_LogSerializer->Serialize(ACh, i);
#else
        NString Raw = NString(LogPrefix) + NString(TEXT(": ")) + NString(Data) + NString(NUX_LINE_TERMINATOR);
        SerializeRaw(Raw.GetTCharPtr());
#endif
    }
}

void NOutputLogFile::SerializeRaw(const TCHAR* Data)
{
    t_u32 s = (t_u32)StringLength(Data)*sizeof(TCHAR);
    m_LogSerializer->Serialize(NUX_CONST_CAST(TCHAR*, Data), s);
}

void LogOutputRedirector::Constructor()
{

}

void LogOutputRedirector::Destructor()
{
    Shutdown();
}

void LogOutputRedirector::AddOutputDevice(LogOutputDevice* OutputDevice)
{
    if( OutputDevice )
    {
        if(std::find(OutputDevices.begin(), OutputDevices.end(), OutputDevice) != OutputDevices.end())
            return;
        OutputDevices.push_back(OutputDevice);
    }
}

void LogOutputRedirector::RemoveOutputDevice(LogOutputDevice* OutputDevice)
{
    std::vector<LogOutputDevice*>::iterator it = std::find(OutputDevices.begin(), OutputDevices.end(), OutputDevice);
    OutputDevices.erase(it);
}

bool LogOutputRedirector::IsRedirectingTo(LogOutputDevice* OutputDevice)
{
    if(std::find(OutputDevices.begin(), OutputDevices.end(), OutputDevice) != OutputDevices.end())
        return true;
    return false;
}

void LogOutputRedirector::Serialize(const TCHAR* Data, const TCHAR* LogPrefix, int Severity)
{
    for(t_u32 OutputDeviceIndex = 0; OutputDeviceIndex < OutputDevices.size(); OutputDeviceIndex++)
    {
        OutputDevices[OutputDeviceIndex]->Serialize(Data, LogPrefix, Severity);
    }
}

void LogOutputRedirector::Flush()
{
    for(t_u32 OutputDeviceIndex=0; OutputDeviceIndex < OutputDevices.size(); OutputDeviceIndex++)
    {
        OutputDevices[OutputDeviceIndex]->Flush();
    }
}

void LogOutputRedirector::Shutdown()
{
    for(t_u32 OutputDeviceIndex = 0; OutputDeviceIndex < OutputDevices.size(); OutputDeviceIndex++)
    {
        OutputDevices[OutputDeviceIndex]->Shutdown();
        // do not delete the output device. This is the responsibility of the owwners.
    }
    OutputDevices.clear();
}


void NOutputVisualDebugConsole::Constructor(){}

void NOutputVisualDebugConsole::Destructor(){}

//! Write data to visual studio output debug console.
/*!
    @param  Data        Text to log.
    @param  LogPrefix	Prefix for the text
*/
void NOutputVisualDebugConsole::Serialize(const TCHAR* Data, const TCHAR* LogPrefix, int Severity)
{
    TCHAR Temp[4096];
    

#if defined (NUX_OS_WINDOWS)
    Snprintf(Temp, 4096, 4096 - 1, TEXT("%s: %s%s"), LogPrefix, Data, NUX_LINE_TERMINATOR);
    OutputDebugString(Temp);
#elif defined (NUX_OS_LINUX)
//     {attr} is one of following
// 
//     0	Reset All Attributes (return to normal mode)
//     1	Bright (Usually turns on BOLD)
//     2 	Dim
//     3	Underline
//     5	Blink
//     7 	Reverse
//     8	Hidden
// 
//     {fg} is one of the following
// 
//     30   Black
//     31   Red
//     32   Green
//     33   Yellow
//     34   Blue
//     35   Magenta
//     36   Cyan
//     37   White (greyish)
//     38   White
// 
//     {bg} is one of the following
// 
//     40   Black
//     41   Red
//     42   Green
//     43   Yellow
//     44   Blue
//     45   Magenta
//     46   Cyan
//     47   White (greyish)
//     48   White

    int Foreground = 38;
    int Background = 48;
    int Bold = 0;

    if(Severity == NUX_MSG_SEVERITY_CRITICAL)
    {
      Foreground = 31;
      Background = 44;
      Bold = 1;
    }
    else if(Severity == NUX_MSG_SEVERITY_ALERT)
    {
      Foreground = 31;
      Bold = 1;
    }
    else if(Severity == NUX_MSG_SEVERITY_WARNING)
    {
      Foreground = 33;
      Bold = 1;
    }
    else if(Severity == NUX_MSG_SEVERITY_INFO)
    {
      Foreground = 32;
      Bold = 1;
    }
    else if(Severity == NUX_MSG_SEVERITY_NONE)
    {
      Foreground = 38;
      Bold = 0;
    }


    Snprintf(Temp, 4096, 4096 - 1, TEXT("%c[%d;%d;%dm%s: %s%c[%d;%d;%dm%s"), 0x1B, Bold, Foreground, Background, LogPrefix, Data, 0x1B, 0, 38, 48, NUX_LINE_TERMINATOR);
    printf("%s", &Temp[0]);

#else
    Snprintf(Temp, 4096, 4096 - 1, TEXT("%s: %s%s"), LogPrefix, Data, NUX_LINE_TERMINATOR);
    printf("%s", &Temp[0]);
#endif
}

void NNullOutput::Constructor(){}

void NNullOutput::Destructor(){}


} //NUX_NAMESPACE_END

