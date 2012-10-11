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


#ifndef NFILEMANAGERGNU_H
#define NFILEMANAGERGNU_H

#include <time.h>

/*-----------------------------------------------------------------------------
File Manager.
-----------------------------------------------------------------------------*/
namespace nux
{
// File manager.
  class NGNUSerialFileReader : public NSerializer
  {
  public:
    NGNUSerialFileReader (int InFileDescriptor, LogOutputDevice &InError, int InSize);
    ~NGNUSerialFileReader();

    virtual bool Precache (int PrecacheOffset, int PrecacheSize);
    virtual long long Seek (long long InPos, NSerializer::SeekPos seekpos);
    virtual long long Tell();
    virtual long long GetFileSize();
    virtual bool Close();
    virtual void SerializeFinal (void *V, long long Length);
    virtual bool isReader()
    {
      return true;
    }
    virtual bool isWriter()
    {
      return false;
    }

  protected:
    int               m_FileDescriptor;
    LogOutputDevice &m_Error;
    long long               m_FileSize;
    long long               m_FilePos;
    long long               m_BufferBase;
    int               m_BufferCount;
    BYTE               *m_Buffer;
    static const int  sBufferSize;
  };

  class NGNUSerialFileWriter : public NSerializer
  {
  public:
    NGNUSerialFileWriter (int InFileDescriptor, LogOutputDevice &InError, int InPos);
    ~NGNUSerialFileWriter();

    virtual long long Seek (long long InPos, NSerializer::SeekPos seekpos);
    virtual long long Tell();
    virtual bool Close();
    virtual void SerializeFinal (void *V, long long Length);
    virtual void Flush();
    virtual long long GetFileSize();
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
    int               m_FileDescriptor;
    LogOutputDevice &m_Error;
    long long               m_Pos;
    int               m_BufferCount;
    BYTE               *m_Buffer;
    static const int  sBufferSize;
    NCriticalSection m_CriticalSection;
  };

  class NFileManagerGNU : public NFileManagerGeneric
  {
    NUX_DECLARE_GLOBAL_OBJECT (NFileManagerGNU, GlobalSingletonInitializer);
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
    /*!
    @return Size of the File. Return -1 if an error occurs.
    */
    long long FileSize (const TCHAR *Filename);
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
    bool GetFileAttribute (const TCHAR *Filename, bool &isDirectory, bool &isReadOnly, bool &isHidden, long long &Size);
    bool MakeDirectory (const TCHAR *Path, bool CreateCompletePath = false);
    bool DeleteDirectory (const TCHAR *Path, bool DeleteContentFirst = false);


    void FindFiles(std::vector<std::string>& /* Result */, const TCHAR* /* Filename */, bool /* Files */, bool /* Directories */) {};
    void ListFilesInDirectory (std::vector<std::string>& /* Result */, const TCHAR* /* DirName */) {};
    double GetFileAgeSeconds (const TCHAR* /* Filename */)
    {
      return 0;
    };
    time_t GetFileLastModified (const TCHAR * /* Filename */)
    {
      return 0;
    };
    bool SetDefaultDirectory()
    {
      return false;
    };
    std::string GetCurrentDirectory()
    {
      return std::string();
    };
    bool GetTimeStamp (const TCHAR * /* Filename */, FileTimeStamp & /* Timestamp */)
    {
      return false;
    };
  };


}

#endif // NFILEMANAGERGNU_H
