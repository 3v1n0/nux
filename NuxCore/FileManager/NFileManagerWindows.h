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


#ifndef NFILEMANAGERWINDOWS_H
#define NFILEMANAGERWINDOWS_H

/*-----------------------------------------------------------------------------
File Manager.
-----------------------------------------------------------------------------*/

// File manager.
namespace nux
{
  class NWindowsSerialFileReader: public NSerializer
  {
  public:
    NWindowsSerialFileReader (HANDLE InHandle, LogOutputDevice &InError);
    ~NWindowsSerialFileReader();

    virtual bool Precache (t_int PrecacheOffset, t_int PrecacheSize);
    virtual t_s64 Seek (t_s64 InPos, NSerializer::SeekPos seekpos);
    virtual t_s64 Tell();
    virtual t_s64 GetFileSize();
    virtual bool Close();
    virtual void SerializeFinal (void *V, t_s64 Length);
    virtual bool isReader()
    {
      return true;
    }
    virtual bool isWriter()
    {
      return false;
    }

  protected:
    HANDLE              m_FileHandle;
    LogOutputDevice &m_Error;
    t_s64               m_FileSize;
    t_s64               m_FilePos;
    t_s64               m_BufferBase;
    t_int               m_BufferCount;
    BYTE               *m_Buffer;
    static const t_int  sBufferSize;
  };

  class NWindowsSerialFileWriter : public NSerializer
  {
  public:
    NWindowsSerialFileWriter (HANDLE InHandle, LogOutputDevice &InError);
    ~NWindowsSerialFileWriter();

    virtual t_s64 Seek (t_s64 InPos, NSerializer::SeekPos seekpos);
    virtual t_s64 Tell();
    virtual bool Close();
    virtual void SerializeFinal (void *V, t_s64 Length);
    virtual void Flush();
    virtual t_s64 GetFileSize();
    virtual bool isReader()
    {
      return false;
    }
    virtual bool isWriter()
    {
      return true;
    }

  protected:
    void _Flush();
    HANDLE              m_FileHandle;
    LogOutputDevice &m_Error;
    t_s64               m_Pos;
    t_int               m_BufferCount;
    BYTE               *m_Buffer;
    static const t_int  sBufferSize;
    NCriticalSection m_CriticalSection;
  };

  class NFileManagerWindows : public NFileManagerGeneric
  {
    NUX_DECLARE_GLOBAL_OBJECT (NFileManagerWindows, GlobalSingletonInitializer);
  public:

    // Flags is a combination of
    //  NSerializer::OutputErrorIfFail
    //  NSerializer::NoOverWrite
    //  NSerializer::OverWriteReadOnly
    //  NSerializer::Unbuffered
    //  NSerializer::Append
    //  NSerializer::Read
    virtual NSerializer *CreateFileReader (const TCHAR *Filename, DWORD Flags, LogOutputDevice &Error = GNullDevice);
    virtual NSerializer *CreateFileWriter (const TCHAR *Filename, DWORD Flags, LogOutputDevice &Error = GNullDevice);
    HANDLE CreateReadFileHandle (const TCHAR *Filename, DWORD Flags);
    HANDLE CreateWriteFileHandle (const TCHAR *Filename, DWORD Flags);

    /*!
        @return Size of the File. Return -1 if an error occurs.
    */
    t_s64 FileSize (const TCHAR *Filename);
    bool FileExist (const TCHAR *Filename);
    int Copy (const TCHAR *DestFile, const TCHAR *SrcFile, bool OverWriteExisting, bool OverWriteReadOnly, NFileTransferMonitor *Monitor);
    bool Move (const TCHAR *Dest, const TCHAR *Src, bool OverWriteExisting = true, bool OverWriteReadOnly = false, NFileTransferMonitor *Monitor = NULL);
    bool Delete (const TCHAR *Filename, bool OverWriteReadOnly = false);
    bool IsReadOnly (const TCHAR *Filename);
    bool IsDirectory (const TCHAR *DirectoryName);
    bool IsHidden (const TCHAR *Filename);
    /*!
        @return TRUE is the file exist.
    */
    bool GetFileAttribute (const TCHAR *Filename, bool &isDirectory, bool &isReadOnly, bool &isHidden, t_s64 &Size);
    bool MakeDirectory (const TCHAR *Path, bool CreateCompletePath = false);
    bool DeleteDirectory (const TCHAR *Path, bool DeleteContentFirst = false);


    void FindFiles (std::vector<NString>& Result, const TCHAR *Filename, bool Files, bool Directories);
    void ListFilesInDirectory (std::vector<NString>& Result, const TCHAR *DirName);
    double GetFileAgeSeconds (const TCHAR *Filename);
    time_t GetFileLastModified (const TCHAR *Filename);
    bool GetFileLastModified (const TCHAR *Filename, SYSTEMTIME &sysTime, bool bLocalTime);
    bool SetDefaultDirectory();
    NString GetCurrentDirectory();
    bool GetTimeStamp (const TCHAR *Filename, FileTimeStamp &Timestamp);
  };

}

#endif // NFILEMANAGERWINDOWS_H

