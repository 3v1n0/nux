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
#include "Parsing.h"

#include <sstream>
#include <iomanip>

namespace nux
{

#if defined(NUX_DEBUG)
//! Create a backup copy of a file if it exist. The copy feature a timestamp in the filename.
  /*!
      @param Filename The name of the file to check.
  */
  static void CreateBackupCopy (const TCHAR *Filename)
  {
    if (GFileManager.FileSize (Filename) > 0)
    {
      // Create string with system time to create a unique filename.
      unsigned int Year, Month, Day, Hour, Min, Sec, MSec;
      GetLocalTime (Year, Month, Day, Hour, Min, Sec, MSec);
      std::string Name, Extension;
      std::string file = Filename;
      size_t pos = file.rfind('.');
      if (pos != std::string::npos)
      {
        Name = file.substr(0, pos);
        Extension = file.substr(pos + 1);
      }
      else
      {
        Name = file;
      }

      std::ostringstream s;
      s << std::setfill('0')
        << Name << "-backup-" << Year << "."
        << std::setw(2) << Month << "."
        << std::setw(2) << Day << "."
        << std::setw(2) << Hour << "."
        << std::setw(2) << Min << "."
        << std::setw(2) << Sec;
      if (Extension.size() )
      {
        s << "." << Extension;
      }

      std::string BackupFilename = s.str();

      GFileManager.Copy (BackupFilename.c_str(), Filename, true, true, NULL);
    }
  }
#endif

  NUX_IMPLEMENT_GLOBAL_OBJECT (NullOutput);
  NUX_IMPLEMENT_GLOBAL_OBJECT (LogOutputRedirector);
  NUX_IMPLEMENT_GLOBAL_OBJECT (LogFileOutput);
  NUX_IMPLEMENT_GLOBAL_OBJECT (VisualOutputConsole)
  NUX_IMPLEMENT_GLOBAL_OBJECT (PrintfOutputConsole)

  LogOutputDevice::LogOutputDevice()
  : _object_destroyed (false)
  {
    _enabled = true;
  }

  LogOutputDevice::~LogOutputDevice()
  {
    _object_destroyed = true;
  }

  void LogOutputDevice::Shutdown()
  {
    _object_destroyed = true;
  }

  void LogOutputDevice::Flush()
  {

  }

  void LogOutputDevice::Enable ()
  {
    _enabled = true;
  }

  void LogOutputDevice::Disable ()
  {
    _enabled = false;
  }


  VARARG_BODY ( void /*FuncRet*/, LogOutputDevice::LogFunction/*FuncName*/, const TCHAR* /*FmtType*/, VARARG_EXTRA (int severity) /*ExtraDecl*/)
  {
    if (_object_destroyed)
      return;

    int   BufferSize	= 1024;
    int   NewBufferSize	= 0;
    TCHAR  *Buffer		= NULL;
    int   Result		= -1;

    while (Result == -1)
    {
      if (NewBufferSize)
      {
        TCHAR *temp = new TCHAR[NewBufferSize];
        Memcpy (temp, Buffer, BufferSize);
        NUX_SAFE_DELETE_ARRAY (Buffer);
        Buffer = temp;
        BufferSize = NewBufferSize;
      }
      else
      {
        Buffer = new TCHAR[BufferSize];
      }

      GET_VARARGS_RESULT (Buffer, BufferSize, BufferSize - 1, Fmt, Result);

      if (Result == -1)
        NewBufferSize = 2 * BufferSize;
    };

    Buffer[Result] = 0;

    Serialize (Buffer, TEXT ("Nux"), severity);

    NUX_SAFE_DELETE_ARRAY (Buffer);
  }

  void LogFileOutput::Constructor()
  {
    m_LogSerializer = NULL;
    m_Opened = false;
    m_Closed = false;

#if defined(NUX_DEBUG)
    // The Editor requires a fully qualified directory to not end up putting the log in various directories.
    m_Filename = GetProgramDirectory();

    if ( (m_Filename[m_Filename.size()-1] != NUX_SLASH_CHAR) || (m_Filename[m_Filename.size()-1] != NUX_BACKSLASH_CHAR) )
      m_Filename += (const TCHAR *) NUX_PATH_SEPARATOR_STRING;

    m_Filename += GetLogDirectory();

    // Create the directory tree where the Logs file will be stored.
    GFileManager.MakeDirectory (m_Filename.c_str(), 1);

    m_Filename += (const TCHAR *) NUX_PATH_SEPARATOR_STRING;
    m_Filename += TEXT ("nux");
    m_Filename += TEXT (".log");

    // if the file already exists, create a backup as we are going to overwrite it
    if (!m_Opened)
    {
      CreateBackupCopy (m_Filename.c_str() );
    }

    // Open log file.
    m_LogSerializer = GFileManager.CreateFileWriter (m_Filename.c_str(), NSerializer::Read | NSerializer::Write | NSerializer::OverWriteReadOnly | (m_Opened ? NSerializer::Append : 0) );

    if (m_LogSerializer)
    {
      m_Opened = true;
#if UNICODE && !NUX_LOG_FILE_ANSI
      m_LogSerializer->Serialize ( (void *) &NUX_UTF16_BE[1], NUX_UTF16_BE[0] /*size*/ );
#endif
      LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("Log file open, %s"), GetFormattedLocalTime().c_str());
    }
    else
    {
      m_Closed = true;
    }

#endif

  }

  void LogFileOutput::Destructor()
  {
    Shutdown();
  }

  /*!
      Closes output device and cleans up. This can't happen in the destructor
      as we have to call "delete" which cannot be done for static/ global objects.
  */
  void LogFileOutput::Shutdown()
  {
    if (m_LogSerializer)
    {
      LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("Log file closed, %s"), GetFormattedLocalTime().c_str());
      Flush();
      delete m_LogSerializer;
      m_LogSerializer = NULL;
    }

    m_Closed = true;
  }

  void LogFileOutput::Flush()
  {
    if (m_LogSerializer)
    {
      m_LogSerializer->Flush();
    }
  }


  //! Write data to the output log file.
  /*!
      @param  log_data    Text to log.
      @param  log_prefix	Prefix for the text
  */
  void LogFileOutput::Serialize (const TCHAR *log_data, const TCHAR *log_prefix, int /* severity */)
  {
    if (!_enabled)
      return;

    if (_object_destroyed)
      return;

    if (m_LogSerializer)
    {
#if UNICODE && NUX_LOG_FILE_ANSI
      ANSICHAR ACh[1024];
      INT DataOffset = 0;
      INT i;

      while (log_data[DataOffset])
      {
        for (i = 0; i < NUX_ARRAY_COUNT (ACh) && log_data[DataOffset]; i++, DataOffset++)
        {
          ACh[i] = ConvertTCHARToAnsiChar (log_data[DataOffset]);
        }

        // serialize chunks of 1024 characters
        m_LogSerializer->Serialize (ACh, i);
      };

      for (i = 0; NUX_LINE_TERMINATOR[i]; i++)
      {
        ACh[i] = NUX_LINE_TERMINATOR[i];
      }

      m_LogSerializer->Serialize (ACh, i);
#else
      std::string Raw = std::string (log_prefix) + std::string (TEXT (": ") ) + std::string (log_data) + std::string (NUX_LINE_TERMINATOR);
      SerializeRaw (Raw.c_str() );
#endif
    }
  }

  void LogFileOutput::SerializeRaw (const TCHAR *log_data)
  {
    unsigned int s = (unsigned int) StringLength (log_data) * sizeof (TCHAR);
    m_LogSerializer->Serialize (NUX_CONST_CAST (TCHAR *, log_data), s);
  }

  void LogOutputRedirector::Constructor()
  {

  }

  void LogOutputRedirector::Destructor()
  {
    Shutdown();
  }

  void LogOutputRedirector::AddOutputDevice (LogOutputDevice *OutputDevice)
  {
    if ( OutputDevice )
    {
      if (std::find (OutputDevices.begin(), OutputDevices.end(), OutputDevice) != OutputDevices.end() )
        return;

      OutputDevices.push_back (OutputDevice);
    }
  }

  void LogOutputRedirector::RemoveOutputDevice (LogOutputDevice *OutputDevice)
  {
    std::vector<LogOutputDevice *>::iterator it = std::find (OutputDevices.begin(), OutputDevices.end(), OutputDevice);
    OutputDevices.erase (it);
  }

  bool LogOutputRedirector::IsRedirectingTo (LogOutputDevice *OutputDevice)
  {
    if (std::find (OutputDevices.begin(), OutputDevices.end(), OutputDevice) != OutputDevices.end() )
      return true;

    return false;
  }

  void LogOutputRedirector::Serialize (const TCHAR *log_data, const TCHAR *log_prefix, int severity)
  {
    if (!_enabled)
      return;

    for (unsigned int OutputDeviceIndex = 0; OutputDeviceIndex < OutputDevices.size(); OutputDeviceIndex++)
    {
      OutputDevices[OutputDeviceIndex]->Serialize (log_data, log_prefix, severity);
    }
  }

  void LogOutputRedirector::Flush()
  {
    for (unsigned int OutputDeviceIndex = 0; OutputDeviceIndex < OutputDevices.size(); OutputDeviceIndex++)
    {
      OutputDevices[OutputDeviceIndex]->Flush();
    }
  }

  void LogOutputRedirector::Shutdown()
  {
    for (unsigned int OutputDeviceIndex = 0; OutputDeviceIndex < OutputDevices.size(); OutputDeviceIndex++)
    {
      OutputDevices[OutputDeviceIndex]->Shutdown();
      // do not delete the output device. This is the responsibility of the owwners.
    }

    OutputDevices.clear();
  }


  void VisualOutputConsole::Constructor() {}

  void VisualOutputConsole::Destructor() {}

//! Write data to visual studio output debug console.
  /*!
      @param  log_data        Text to log.
      @param  log_prefix	Prefix for the text
  */
#if defined(NUX_OS_WINDOWS)
  void VisualOutputConsole::Serialize(const TCHAR* text, const TCHAR* log_prefix, int severity)
  {
    if (!_enabled)
      return;

    TCHAR Temp[4096];

    Snprintf (Temp, 4096, 4096 - 1, TEXT ("%s: %s%s"), log_prefix, text, NUX_LINE_TERMINATOR);
    OutputDebugString (Temp);
  }
#else
  void VisualOutputConsole::Serialize(const TCHAR * /* text */, const TCHAR * /* log_prefix */, int /* severity */)
  {
  }
#endif

  void PrintfOutputConsole::Constructor() {}

  void PrintfOutputConsole::Destructor() {}

//! Write data to visual studio output debug console.
  /*!
      @param  log_data        Text to log.
      @param  log_prefix	Prefix for the text
  */
  void PrintfOutputConsole::Serialize (const TCHAR *text, const TCHAR *log_prefix, int severity)
  {
    if (!_enabled)
      return;

    TCHAR Temp[4096];
#if defined (NUX_OS_WINDOWS)
    Snprintf (Temp, 4096, 4096 - 1, TEXT ("%s: %s%s"), log_prefix, text, NUX_LINE_TERMINATOR);

    HANDLE hConsole;
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    

    if (severity == NUX_MSG_SEVERITY_CRITICAL)
    {
      SetConsoleTextAttribute(hConsole, FOREGROUND_RED|FOREGROUND_INTENSITY|BACKGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_INTENSITY);
    }
    else if (severity == NUX_MSG_SEVERITY_ALERT)
    {
      SetConsoleTextAttribute(hConsole, FOREGROUND_RED|FOREGROUND_INTENSITY);
    }
    else if (severity == NUX_MSG_SEVERITY_WARNING)
    {
      SetConsoleTextAttribute(hConsole, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
    }
    else if (severity == NUX_MSG_SEVERITY_INFO)
    {
      SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN|FOREGROUND_INTENSITY);
    }
    else if (severity == NUX_MSG_SEVERITY_NONE)
    {
      SetConsoleTextAttribute(hConsole, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
    }

    printf ("%s", &Temp[0]);

    SetConsoleTextAttribute(hConsole, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);

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

    if (severity == NUX_MSG_SEVERITY_CRITICAL)
    {
      Foreground = 31;
      Background = 44;
      Bold = 1;
    }
    else if (severity == NUX_MSG_SEVERITY_ALERT)
    {
      Foreground = 31;
      Bold = 1;
    }
    else if (severity == NUX_MSG_SEVERITY_WARNING)
    {
      Foreground = 33;
      Bold = 1;
    }
    else if (severity == NUX_MSG_SEVERITY_INFO)
    {
      Foreground = 32;
      Bold = 1;
    }
    else if (severity == NUX_MSG_SEVERITY_NONE)
    {
      Foreground = 38;
      Bold = 0;
    }


    Snprintf (Temp, 4096, 4096 - 1, TEXT ("%c[%d;%d;%dm%s: %s%c[%d;%d;%dm%s"), 0x1B, Bold, Foreground, Background, log_prefix, text, 0x1B, 0, 38, 48, NUX_LINE_TERMINATOR);
    printf ("%s", &Temp[0]);

#else
    Snprintf (Temp, 4096, 4096 - 1, TEXT ("%s: %s%s"), log_prefix, text, NUX_LINE_TERMINATOR);
    printf ("%s", &Temp[0]);
#endif
  }


  void NullOutput::Constructor() {}

  void NullOutput::Destructor() {}


}

