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


#ifndef NOUTPUTDEVICE_H
#define NOUTPUTDEVICE_H

namespace nux
{

  class NSerializer;

  class LogOutputDevice
  {
  public:
    LogOutputDevice ();
    virtual ~LogOutputDevice ();

    BOOL m_terminated;
    virtual void Serialize (const TCHAR *log_data, const TCHAR *log_prefix, int severity) = 0;
    virtual void Flush ();
    virtual void Shutdown ();

    VARARG_DECL (void/*FuncRet*/, void/*StaticFuncRet*/, {}/*Return*/, LogFunction/*FuncName*/, VARARG_NONE/*Pure*/, const TCHAR*/*FmtType*/, VARARG_EXTRA (int severity) /*ExtraDecl*/, VARARG_EXTRA (severity) /*ExtraParam*/ );

    void Enable ();
    void Disable ();

  protected:
    bool _object_destroyed;
    bool _enabled;
  };

//! Output to null device.
  class NullOutput : public LogOutputDevice
  {
    NUX_DECLARE_GLOBAL_OBJECT (NullOutput, GlobalSingletonInitializer);
  public:
    void Serialize ( const TCHAR * /* V */, const TCHAR * /* LogPrefix */, int /* severity */) {}
  };

//! Output to log file.
  class LogFileOutput : public LogOutputDevice
  {
    NUX_DECLARE_GLOBAL_OBJECT (LogFileOutput, GlobalSingletonInitializer);
    //LogFileOutput( const TCHAR* InFilename);

  public:
    /*!
        Closes output device and cleans up.
    */
    void Shutdown();

    /*!
        Flush the write cache so the output to the file isn't truncated.
    */
    void Flush();

    /*!
        Write a stream to the log file.

        @param log_data     Stream characters to write.
        @param LogPrefix    A string to write before the input stream of characters.
    */
    void Serialize (const TCHAR *log_data, const TCHAR *LogPrefix, int severity);

  private:
    NSerializer    *m_LogSerializer;
    std::string         m_Filename;
    bool            m_Opened;
    bool            m_Closed;

    /*!
        Serialize data directly to the log file without any type of conversion or preprocessing.

        @param log_data     String of char to write to the output file.
    */
    void SerializeRaw (const TCHAR *log_data);
  };

  //! Output to microsoft visual console.
  class VisualOutputConsole : public LogOutputDevice
  {
    NUX_DECLARE_GLOBAL_OBJECT (VisualOutputConsole, GlobalSingletonInitializer);
  public:

    //! Write data to visual studio output debug console.
    /*!
        @param text       Text to log.
        @param log_prefix	Prefix for the text.
        @param severity   Importance of the message.
    */
    void Serialize (const TCHAR *text, const TCHAR *log_prefix, int severity);
  };

    //! Standard printf output console.
  class PrintfOutputConsole : public LogOutputDevice
  {
    NUX_DECLARE_GLOBAL_OBJECT (PrintfOutputConsole, GlobalSingletonInitializer);
  public:

    //! Write text to the standard printf output debug console.
    /*!
        @param text       Text to log.
        @param log_prefix	Prefix for the text.
        @param severity   Importance of the message.
    */
    void Serialize (const TCHAR *text, const TCHAR *log_prefix, int severity);
  };

  class LogOutputRedirector : public LogOutputDevice
  {
    NUX_DECLARE_GLOBAL_OBJECT (LogOutputRedirector, GlobalSingletonInitializer);
  public:
    virtual void AddOutputDevice (LogOutputDevice *OutputDevice);
    virtual void RemoveOutputDevice (LogOutputDevice *OutputDevice);
    virtual bool IsRedirectingTo (LogOutputDevice *OutputDevice);
    void Serialize (const TCHAR *log_data, const TCHAR *log_prefix, int severity);
    void Flush();

    void Shutdown();

  private:
    std::vector<LogOutputDevice *> OutputDevices;
  };

}

#endif // NOUTPUTDEVICE_H
