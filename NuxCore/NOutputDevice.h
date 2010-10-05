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


#ifndef NOUTPUTDEVICE_H
#define NOUTPUTDEVICE_H

namespace nux   //NUX_NAMESPACE_BEGIN
{

  class NSerializer;

  class LogOutputDevice
  {
  public:
    LogOutputDevice()
      : m_ObjectDestroyed (FALSE)
    { }

    virtual ~LogOutputDevice()
    {
      m_ObjectDestroyed = TRUE;
    }

    BOOL m_terminated;
    virtual void Serialize (const TCHAR *V, const TCHAR *LogPrefix, int Severity) = 0;
    virtual void Flush();
    virtual void Shutdown();

    VARARG_DECL ( void/*FuncRet*/, void/*StaticFuncRet*/, {}/*Return*/, LogFunction/*FuncName*/, VARARG_NONE/*Pure*/, const TCHAR*/*FmtType*/, VARARG_EXTRA (int Severity) /*ExtraDecl*/, VARARG_EXTRA (Severity) /*ExtraParam*/ );

  protected:
    BOOL m_ObjectDestroyed;
  };

//! Output to null device.
  class NNullOutput : public LogOutputDevice
  {
    NUX_DECLARE_GLOBAL_OBJECT (NNullOutput, NGlobalSingletonInitializer);
  public:
    void Serialize ( const TCHAR *V, const TCHAR *LogPrefix, int Severity) {}
  };

//! Output to log file.
  class NOutputLogFile : public LogOutputDevice
  {
    NUX_DECLARE_GLOBAL_OBJECT (NOutputLogFile, NGlobalSingletonInitializer);
    //NOutputLogFile( const TCHAR* InFilename);

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

        @param Data         Stream characters to write.
        @param LogPrefix    A string to write before the input stream of characters.
    */
    void Serialize (const TCHAR *Data, const TCHAR *LogPrefix, int Severity);

  private:
    NSerializer    *m_LogSerializer;
    NString         m_Filename;
    bool            m_Opened;
    bool            m_Closed;

    /*!
        Serialize data directly to the log file without any type of conversion or preprocessing.

        @param Data     String of char to write to the output file.
    */
    void SerializeRaw (const TCHAR *Data);
  };

//! Output to microsoft visual console.
  class NOutputVisualDebugConsole : public LogOutputDevice
  {
    NUX_DECLARE_GLOBAL_OBJECT (NOutputVisualDebugConsole, NGlobalSingletonInitializer);
  public:

    //! Write data to visual studio output debug console.
    /*!
        @param  Data        Text to log.
        @param  LogPrefix	Prefix for the text
    */
    void Serialize (const TCHAR *Data, const TCHAR *LogPrefix, int Severity);
  };

  class LogOutputRedirector : public LogOutputDevice
  {
    NUX_DECLARE_GLOBAL_OBJECT (LogOutputRedirector, NGlobalSingletonInitializer);
  public:
    virtual void AddOutputDevice (LogOutputDevice *OutputDevice);
    virtual void RemoveOutputDevice (LogOutputDevice *OutputDevice);
    virtual bool IsRedirectingTo (LogOutputDevice *OutputDevice);
    void Serialize (const TCHAR *Data, const TCHAR *LogPrefix, int Severity);
    void Flush();

    void Shutdown();

  private:
    std::vector<LogOutputDevice *> OutputDevices;
  };

} //NUX_NAMESPACE_END

#endif // NOUTPUTDEVICE_H
