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
#include "Math/MathUtility.h"

namespace nux
{

// Choose the size so it is a power of 2. Example (size-1)= 11111111.
  const int  NWindowsSerialFileReader::sBufferSize = 1024;

  NWindowsSerialFileReader::NWindowsSerialFileReader (HANDLE InHandle, LogOutputDevice &InError)
    :   m_FileHandle    (InHandle)
    ,   m_Error         (InError)
    ,   m_FilePos       (0)
    ,   m_BufferBase    (0)
    ,   m_BufferCount   (0)
  {
    m_Buffer = new BYTE[sBufferSize];
    m_FileSize = GetFileSize();
  }
  NWindowsSerialFileReader::~NWindowsSerialFileReader()
  {
    NUX_SAFE_DELETE_ARRAY (m_Buffer);

    if (m_FileHandle)
    {
      Close();
    }
  }

  bool NWindowsSerialFileReader::Precache (int PrecacheOffset, int PrecacheSize)
  {
    // Only pre-cache at current position and avoid work if pre-caching same offset twice.
    if ( (m_FilePos == PrecacheOffset) && (!m_BufferBase || !m_BufferCount || m_BufferBase != m_FilePos) )
    {
      m_BufferBase = m_FilePos;
      // (sBufferSize - 1) contains only '1', i.e 1111111111.
      // So (m_FilePos & (sBufferSize-1)) is equal to m_FilePos if m_FilePos <= (sBufferSize-1).
      m_BufferCount = Min<long long> (Min<long long> (PrecacheSize, (int) (sBufferSize - (m_FilePos & (sBufferSize - 1) ) ) ), m_FileSize - m_FilePos);
      unsigned int Count = 0;
      //GTotalBytesReadViaFileManager += m_BufferCount;
      ::ReadFile (m_FileHandle, m_Buffer, m_BufferCount, NUX_REINTERPRET_CAST (DWORD *, &Count), NULL);

      if (Count != m_BufferCount)
      {
        m_ErrorCode = 1;
        m_Error.LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("ReadFile failed: Count=%i BufferCount=%i Error=%s"), Count, m_BufferCount, inlGetSystemErrorMessage() );
      }
    }

    return TRUE;
  }

  long long NWindowsSerialFileReader::Seek (long long InPos, NSerializer::SeekPos seekpos)
  {
    nuxAssert (InPos >= 0);
    nuxAssert (InPos <= m_FileSize);

    Flush();
    // Because we precache our reads, we must perform Seek accordingly.

    LARGE_INTEGER pos;
    pos.QuadPart = m_FilePos;
    LARGE_INTEGER filepos;
    filepos.QuadPart = 0;

    // Set the file pointer to m_FilePos.
    if (::SetFilePointerEx (m_FileHandle, pos, &filepos, FILE_BEGIN) == 0)
    {
      m_ErrorCode = 1;
      m_Error.LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("SetFilePointer Failed %i/%i: %i %s"), InPos, m_FileSize, m_FilePos, inlGetSystemErrorMessage() );
    }

    // Now the file pointer is current with what we have read so far.
    pos.QuadPart = InPos;
    filepos.QuadPart = 0;

    if (::SetFilePointerEx (m_FileHandle, pos, &filepos, (seekpos == SeekStart) ? FILE_BEGIN : (seekpos == SeekCurrent) ? FILE_CURRENT : FILE_END) == 0)
    {
      m_ErrorCode = 1;
      m_Error.LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("SetFilePointer Failed %i/%i: %i %s"), InPos, m_FileSize, m_FilePos, inlGetSystemErrorMessage() );
    }

    m_FilePos   = filepos.QuadPart;
    m_BufferBase  = 0;
    m_BufferCount = 0;
    Precache (m_FilePos, sBufferSize);
    return filepos.QuadPart;
  }

  long long NWindowsSerialFileReader::Tell()
  {
//     Flush();
//     LARGE_INTEGER pos;
//     LARGE_INTEGER filepos;
//     pos.QuadPart = 0;
//     ::SetFilePointerEx(m_FileHandle, pos, &filepos, FILE_CURRENT);
//     return filepos.QuadPart;

    return m_FilePos;
  }

  long long NWindowsSerialFileReader::GetFileSize()
  {
    nuxAssert (m_FileHandle);

    if (m_FileHandle == NULL)
      return -1;

    long long Size = 0;

    if (::GetFileSizeEx (m_FileHandle, NUX_REINTERPRET_CAST (PLARGE_INTEGER, &Size) ) == 0)
    {
      Size = -1;
    }

    m_FileSize = Size > 0 ? Size : 0;
    return Size;
  }

  bool NWindowsSerialFileReader::Close()
  {
    if (m_FileHandle)
    {
      CloseHandle (m_FileHandle);
    }

    m_FileHandle = NULL;
    return !m_ErrorCode;
  }

  void NWindowsSerialFileReader::SerializeFinal (void* Dest, long long Length)
  {
    nuxAssert (Dest);

    while (Length > 0)
    {
      int DataSize = Min<long long> (Length, m_BufferBase + m_BufferCount - m_FilePos);

      if (DataSize == 0)
      {
        if (Length >= sBufferSize)
        {
          int Count = 0;
          //GTotalBytesReadViaFileManager += Length;
          ReadFile (m_FileHandle, Dest, Length, (DWORD*) &Count, NULL);

          if (Count != Length)
          {
            m_ErrorCode = 1;
            m_Error.LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("ReadFile failed: Count=%i Length=%i Error=%s"), Count, Length, inlGetSystemErrorMessage() );
          }

          m_FilePos += Length;
          m_BufferBase += Length;
          return;
        }

        Precache (m_FilePos, 0x7FFFFFFF);
        DataSize = Min<long long> (Length, m_BufferBase + m_BufferCount - m_FilePos);

        if (DataSize <= 0)
        {
          m_ErrorCode = 1;
          m_Error.LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("ReadFile beyond EOF %i+%i/%i"), m_FilePos, Length, m_FileSize);
        }

        if (m_ErrorCode)
          return;
      }

      Memcpy (Dest, m_Buffer + m_FilePos - m_BufferBase, DataSize);
      m_FilePos   += DataSize;
      Length      -= DataSize;
      Dest        = (BYTE *) Dest + DataSize;
    }
  }
//////////////////////////////////////////////////////////////////////////
// Choose the size so it is a power of 2. Example (size-1)= 11111111.
  const int  NWindowsSerialFileWriter::sBufferSize = 32;
//NCriticalSection NWindowsSerialFileWriter::m_CriticalSection;

  NWindowsSerialFileWriter::NWindowsSerialFileWriter (HANDLE InHandle, LogOutputDevice &InError)
    :   m_FileHandle  (InHandle)
    ,   m_Error       (InError)
    ,   m_BufferCount (0)
  {
    m_Pos = Tell();
    m_Buffer = new BYTE[sBufferSize];
  }

  NWindowsSerialFileWriter::~NWindowsSerialFileWriter()
  {
    NUX_SAFE_DELETE_ARRAY (m_Buffer);

    if (m_FileHandle)
      Close();

    m_FileHandle = NULL;
  }

  long long NWindowsSerialFileWriter::Seek (long long InPos, NSerializer::SeekPos seekpos)
  {
    NScopeLock Scope (&m_CriticalSection);
    nuxAssert (m_FileHandle);

    if (m_FileHandle == NULL)
      return -1;

    _Flush();
    LARGE_INTEGER pos;
    pos.QuadPart = InPos;
    LARGE_INTEGER filepos;
    filepos.QuadPart = 0;

    if (::SetFilePointerEx (m_FileHandle, pos, &filepos, (seekpos == SeekStart) ? FILE_BEGIN : (seekpos == SeekCurrent) ? FILE_CURRENT : FILE_END) == 0)
    {
      m_ErrorCode = 1;
      m_Error.LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("SeekFailed") );
    }

    m_Pos = filepos.QuadPart;

    return filepos.QuadPart;
  }

  long long NWindowsSerialFileWriter::Tell()
  {
    NScopeLock Scope (&m_CriticalSection);
    nuxAssert (m_FileHandle);

    if (m_FileHandle == NULL)
      return -1;

    _Flush();
    LARGE_INTEGER pos;
    LARGE_INTEGER filepos;
    filepos.QuadPart = 0;
    pos.QuadPart = 0;
    ::SetFilePointerEx (m_FileHandle, pos, &filepos, FILE_CURRENT);
    return filepos.QuadPart;
  }

  bool NWindowsSerialFileWriter::Close()
  {
    NScopeLock Scope (&m_CriticalSection);
    nuxAssert (m_FileHandle);

    if (m_FileHandle == NULL)
      return true;

    _Flush();

    if (m_FileHandle && !CloseHandle (m_FileHandle) )
    {
      m_ErrorCode = 1;
      m_Error.LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("WriteFailed") );
    }

    m_FileHandle = NULL;
    return !m_ErrorCode;
  }

  long long NWindowsSerialFileWriter::GetFileSize()
  {
    nuxAssert (m_FileHandle);

    if (m_FileHandle == NULL)
      return -1;

    long long Size = 0;

    if (::GetFileSizeEx (m_FileHandle, NUX_REINTERPRET_CAST (PLARGE_INTEGER, &Size) ) == 0)
    {
      Size = -1;
    }

    return Size;
  }

  void NWindowsSerialFileWriter::SerializeFinal (void *V, long long Length)
  {
    // This method is not re-entrant by itself. It relies on m_Buffer and other variables
    // that belong to this object. Therefore, it is not thread safe. We add a critical section
    // to make it thread safe.

    NScopeLock Scope (&m_CriticalSection);
    nuxAssert (m_FileHandle);
    nuxAssert (V);

    NUX_RETURN_IF_NULL (m_FileHandle);

    m_Pos += Length;
    int FreeSpace;

    while (Length > (FreeSpace = sBufferSize - m_BufferCount) )
    {
      // m_Buffer is Full. Write it to the file.
      Memcpy (m_Buffer + m_BufferCount, V, FreeSpace);
      m_BufferCount   += FreeSpace;
      Length          -= FreeSpace;
      V               = (BYTE *) V + FreeSpace;
      _Flush();
    }

    if (Length)
    {
      Memcpy (m_Buffer + m_BufferCount, V, Length);
      m_BufferCount += Length; // Count the number of Characters stored in m_Buffer.
    }
  }

  void NWindowsSerialFileWriter::Flush()
  {
    NScopeLock Scope (&m_CriticalSection);
    nuxAssert (m_FileHandle);

    if (m_FileHandle == NULL)
      return;

    _Flush();
  }

  void NWindowsSerialFileWriter::_Flush()
  {
    //GTotalBytesWrittenViaFileManager += m_BufferCount;
    if (m_BufferCount)
    {
      int Result = 0;

      if (!WriteFile (m_FileHandle, m_Buffer, m_BufferCount, (DWORD *) &Result, NULL) )
      {
        m_ErrorCode = 1;
        m_Error.LogFunction (NUX_MSG_SEVERITY_NONE, TEXT ("[NWindowsSerialFileWriter::_Flush] Write failed") );
      }
    }

    m_BufferCount = 0;
  }

//////////////////////////////////////////////////////////////////////////
  NUX_IMPLEMENT_GLOBAL_OBJECT (NFileManagerWindows);

  void NFileManagerWindows::Constructor()
  {
  }

  void NFileManagerWindows::Destructor()
  {
  }

  HANDLE NFileManagerWindows::CreateReadFileHandle (const TCHAR *Filename, DWORD Flags)
  {
    DWORD  Access    = GENERIC_READ;
    DWORD  Create    = OPEN_EXISTING;

    DWORD  SharedModeFlags  = 0;
    SharedModeFlags |= (Flags & NSerializer::Read) ? FILE_SHARE_READ : 0;
    SharedModeFlags |= (Flags & NSerializer::Write) ? FILE_SHARE_WRITE : 0;

    HANDLE Handle    = ::CreateFile (Filename, Access, SharedModeFlags, NULL, Create, FILE_ATTRIBUTE_NORMAL, NULL);

    if (Handle == INVALID_HANDLE_VALUE)
    {
      if (Flags & NSerializer::OutputErrorIfFail)
        nuxError (TEXT ("Failed to read file: %s"), Filename);
    }

    return Handle;
  }

  NSerializer *NFileManagerWindows::CreateFileReader (const TCHAR *Filename, DWORD Flags, LogOutputDevice &Error)
  {
    HANDLE Handle = CreateReadFileHandle (Filename, Flags);

    if (Handle == INVALID_HANDLE_VALUE)
    {
      return 0;
    }

    return new NWindowsSerialFileReader (Handle, Error);
  }

  HANDLE NFileManagerWindows::CreateWriteFileHandle (const TCHAR *Filename, DWORD Flags)
  {
    if (Flags & NSerializer::OverWriteReadOnly)
    {
      ::SetFileAttributes (Filename, 0);
    }

    DWORD  Access    = GENERIC_WRITE;
    DWORD  SharedModeFlags  = 0;
    SharedModeFlags |= (Flags & NSerializer::Read) ? FILE_SHARE_READ : 0;
    SharedModeFlags |= (Flags & NSerializer::Write) ? FILE_SHARE_WRITE : 0;

    DWORD  Create    = 0;
    Create          |= (Flags & NSerializer::Append) ? OPEN_ALWAYS : 0;
    Create          |= (Flags & NSerializer::NoOverWrite) ? CREATE_NEW /*fail if the file already exist*/ : CREATE_ALWAYS /*create the file if it does not exist*/;
    HANDLE Handle    = ::CreateFile (Filename, Access, SharedModeFlags, NULL, Create, FILE_ATTRIBUTE_NORMAL, NULL);

    if (Handle == INVALID_HANDLE_VALUE)
    {
      if (Flags & NSerializer::OutputErrorIfFail)
      {
        nuxError (TEXT ("[NFileManagerWindows::CreateFileWriter] Failed to create file %s (GetLastError: %d)"), Filename, ::GetLastError() );
      }
    }

    if ( (Flags & NSerializer::Append) && (Handle != INVALID_HANDLE_VALUE) )
    {
      int Pos = ::SetFilePointer (Handle, 0, 0, FILE_END);
    }

    return Handle;
  }

  NSerializer *NFileManagerWindows::CreateFileWriter (const TCHAR *Filename,
      DWORD Flags,
      LogOutputDevice &Error)
  {
    HANDLE Handle = CreateWriteFileHandle (Filename, Flags);

    if (Handle == INVALID_HANDLE_VALUE)
    {
      return 0;
    }

    return new NWindowsSerialFileWriter (Handle, Error);
  }

  /*!
      @return Size of the File. Return -1 if an error occurs.
  */
  long long NFileManagerWindows::FileSize (const TCHAR *Filename)
  {
    HANDLE Handle = ::CreateFile (Filename, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (Handle == INVALID_HANDLE_VALUE)
    {
      LPVOID lpMsgBuf = 0;
      ::FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       ::GetLastError(),
                       MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR) &lpMsgBuf,
                       0,
                       NULL);

      nuxDebugMsg (TEXT ("[NFileManagerWindows::FileSize] Failed to open file %s for attribute read (GetLastError: %d - %s)"), Filename, ::GetLastError(), lpMsgBuf);

      ::LocalFree (lpMsgBuf);
      return -1;
    }

    long long Size = 0;

    if (::GetFileSizeEx (Handle, NUX_REINTERPRET_CAST (PLARGE_INTEGER, &Size) ) == 0)
    {
      Size = -1;
    }

    CloseHandle (Handle);
    return Size;
  }

  bool NFileManagerWindows::FileExist (const TCHAR *Filename)
  {
    WIN32_FILE_ATTRIBUTE_DATA FileAttrData;

    if (::GetFileAttributesEx (Filename, GetFileExInfoStandard, NUX_STATIC_CAST (void *, &FileAttrData) ) )
    {
      return true;
    }

    return false;
  }

  int NFileManagerWindows::Copy (const TCHAR *DestFile,
                                 const TCHAR *SrcFile,
                                 bool OverWriteExisting,
                                 bool OverWriteReadOnly,
                                 NFileTransferMonitor *Monitor)
  {
    // In case file exist and OverWriteReadOnly is true, Remove the ReadOnly attribute from the file.
    if (OverWriteReadOnly)
    {
      ::SetFileAttributes (DestFile, FILE_ATTRIBUTE_NORMAL);
    }

    DWORD Flags = (OverWriteExisting ? 0 : COPY_FILE_FAIL_IF_EXISTS);
    BOOL *pCancel = NULL;

    if (Monitor)
      pCancel = & (Monitor->m_bCancel);

    if (::CopyFileEx (SrcFile, DestFile, NFileTransferMonitor::CopyProgressRoutine, NUX_REINTERPRET_CAST (void *, Monitor), pCancel, Flags) == 0)
    {
      LPVOID lpMsgBuf = 0;
      ::FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       ::GetLastError(),
                       MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR) &lpMsgBuf,
                       0,
                       NULL);

      nuxDebugMsg (TEXT ("[NFileManagerWindows::Copy] Error copying file from '%s' to '%s'(GetLastError: %d - %s)"), SrcFile, DestFile, ::GetLastError(), lpMsgBuf);
      ::LocalFree (lpMsgBuf);
      return false;
    }

    ::SetFileAttributes (DestFile, FILE_ATTRIBUTE_NORMAL);
    return true;
  }

  bool NFileManagerWindows::Delete (const TCHAR *Filename, bool OverWriteReadOnly)
  {
    if (OverWriteReadOnly)
    {
      ::SetFileAttributes (Filename, FILE_ATTRIBUTE_NORMAL);
    }

    if ( (::DeleteFile (Filename) == 0) && (GetLastError() != ERROR_FILE_NOT_FOUND) )
    {
      LPVOID lpMsgBuf = 0;
      ::FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL,
                       ::GetLastError(),
                       MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPTSTR) &lpMsgBuf,
                       0,
                       NULL);

      nuxDebugMsg (TEXT ("[NFileManagerWindows::Delete] Error deleting file '%s' (GetLastError: %d - %s)"), Filename, ::GetLastError(), lpMsgBuf);
      ::LocalFree (lpMsgBuf);
      return false;
    }

    return true;
  }

  bool NFileManagerWindows::IsReadOnly (const TCHAR *Filename)
  {
    WIN32_FILE_ATTRIBUTE_DATA FileAttrData;

    if (::GetFileAttributesEx (Filename, GetFileExInfoStandard, NUX_STATIC_CAST (void *, &FileAttrData) ) )
    {
      return ( (FileAttrData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0);
    }
    else
    {
      nuxDebugMsg (TEXT ("[NFileManagerWindows::IsReadOnly]: Error reading attributes for file '%s'"), Filename);
    }

    return false;
  }

  bool NFileManagerWindows::IsDirectory (const TCHAR *DirectoryName)
  {
    WIN32_FILE_ATTRIBUTE_DATA FileAttrData;

    if (::GetFileAttributesEx (DirectoryName, GetFileExInfoStandard, NUX_STATIC_CAST (void *, &FileAttrData) ) )
    {
      return ( (FileAttrData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    }
    else
    {
      nuxDebugMsg (TEXT ("[NFileManagerWindows::IsDirectory]: Error reading attributes for directory '%s'"), DirectoryName);
    }

    return false;
  }

  bool NFileManagerWindows::IsHidden (const TCHAR *Filename)
  {
    WIN32_FILE_ATTRIBUTE_DATA FileAttrData;

    if (::GetFileAttributesEx (Filename, GetFileExInfoStandard, NUX_STATIC_CAST (void *, &FileAttrData) ) )
    {
      return ( (FileAttrData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0);
    }
    else
    {
      nuxDebugMsg (TEXT ("[NFileManagerWindows::IsHidden]: Error reading attributes for file '%s'"), Filename);
    }

    return false;
  }

  /*!
      @return TRUE is the file exist.
  */
  bool NFileManagerWindows::GetFileAttribute (const TCHAR *Filename, bool &isDirectory, bool &isReadOnly, bool &isHidden, long long &Size)
  {
    isDirectory = false;
    isReadOnly = false;
    isHidden = false;
    Size = 0;
    WIN32_FILE_ATTRIBUTE_DATA FileAttrData;

    if (::GetFileAttributesEx (Filename, GetFileExInfoStandard, (void *) &FileAttrData) )
    {
      isDirectory = ( (FileAttrData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
      isReadOnly = ( (FileAttrData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0);
      isHidden = ( (FileAttrData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0);
      Size = FileAttrData.nFileSizeLow | ( (long long) (FileAttrData.nFileSizeHigh) << 32);
    }
    else
    {
      nuxDebugMsg (TEXT ("[NFileManagerWindows::GetFileAttribute]: Error reading attributes for file '%s'"), Filename);
      return (0);
    }

    return TRUE;
  }

  bool NFileManagerWindows::Move (const TCHAR *Dest,
                                  const TCHAR *Src,
                                  bool Replace,
                                  bool EvenIfReadOnly,
                                  NFileTransferMonitor *Monitor)
  {
    DWORD Flags = MOVEFILE_COPY_ALLOWED | MOVEFILE_FAIL_IF_NOT_TRACKABLE;
    Flags |= (EvenIfReadOnly ? MOVEFILE_REPLACE_EXISTING : 0);

    BOOL *pCancel = NULL;

    if (Monitor)
      pCancel = & (Monitor->m_bCancel);

    if (::MoveFileWithProgress (Src, Dest, NFileTransferMonitor::CopyProgressRoutine, NUX_REINTERPRET_CAST (void *, Monitor), Flags) != 0)
    {
      nuxDebugMsg (TEXT ("[NFileManagerWindows::Move] Error moving file '%s' to '%s' (GetLastError: %d)"), Src, Dest, ::GetLastError() );
      return false;
    }

    return true;
  }

  bool NFileManagerWindows::MakeDirectory (const TCHAR *Path, bool CreateCompletePath)
  {
    if (CreateCompletePath)
    {
      return NFileManagerGeneric::MakeDirectory (Path, CreateCompletePath);
    }

    if ( (::CreateDirectory (Path, NULL) == 0) && (::GetLastError() != ERROR_ALREADY_EXISTS) )
    {
      nuxDebugMsg (TEXT ("[NFileManagerWindows::MakeDirectory] Error creating directory '%s' (GetLastError: %d)"), Path, ::GetLastError() );
      return NUX_ERROR;
    }

    return NUX_OK;
  }

  bool NFileManagerWindows::DeleteDirectory (const TCHAR *Path, bool DeleteContentFirst)
  {
    if (DeleteContentFirst)
    {
      return NFileManagerGeneric::DeleteDirectory (Path, DeleteContentFirst);
    }

    if ( (::RemoveDirectory (Path) == 0) && (::GetLastError() != ERROR_FILE_NOT_FOUND) )
    {
      nuxDebugMsg (TEXT ("[NFileManagerWindows::DeleteDirectory] Error deleting directory '%s' (GetLastError: %d)"), Path, ::GetLastError() );
      return false;
    }

    return true;
  }

  void NFileManagerWindows::FindFiles (std::vector<std::string>& Result, const TCHAR *Filename, bool Files, bool Directories)
  {
    HANDLE Handle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA SearchData;
    Handle = ::FindFirstFile (Filename, &SearchData);

    if (Handle != INVALID_HANDLE_VALUE)
    {
      do
      {
        if (Stricmp (SearchData.cFileName, TEXT (".") ) &&
            Stricmp (SearchData.cFileName, TEXT ("..") ) &&
            ! (SearchData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
            ! (SearchData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) )

        {
          if ( (SearchData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? Directories : Files)
            Result.push_back (std::string(SearchData.cFileName) );
        }
      }
      while (::FindNextFile (Handle, &SearchData) );
    }

    if (Handle != INVALID_HANDLE_VALUE)
      ::FindClose (Handle);
  }

  void NFileManagerWindows::ListFilesInDirectory (std::vector<std::string>& Result, const TCHAR *DirName)
  {
    WIN32_FIND_DATA SearchData;
    HANDLE Handle = INVALID_HANDLE_VALUE;
    std::string DirectoryName = DirName;
    DirectoryName += TEXT ("\\*");
    Handle = ::FindFirstFile (DirectoryName.c_str(), &SearchData);

    if (Handle != INVALID_HANDLE_VALUE)
    {
      // List all the other files in the directory.
      do
      {
        if (Stricmp (SearchData.cFileName, TEXT (".") ) &&
            Stricmp (SearchData.cFileName, TEXT ("..") ) &&
            ! (SearchData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) &&
            ! (SearchData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) )
        {
          Result.push_back(std::string(SearchData.cFileName));
        }
      }
      while (::FindNextFile (Handle, &SearchData) );
    }

    if (Handle != INVALID_HANDLE_VALUE)
      ::FindClose (Handle);
  }

  double NFileManagerWindows::GetFileAgeSeconds (const TCHAR *Filename)
  {
    struct _stat FileInfo;

    if (_tstat (Filename, &FileInfo) == 0)
    {
      time_t  CurrentTime,
              FileTime;
      FileTime = FileInfo.st_mtime;
      time (&CurrentTime);

      return difftime (CurrentTime, FileTime);
    }

    return -1.0;
  }

  time_t NFileManagerWindows::GetFileLastModified (const TCHAR *Filename)
  {
    struct _stat FileInfo;

    if (_tstat (Filename, &FileInfo) == 0)
    {
      time_t FileTime;
      FileTime = FileInfo.st_mtime;

      return FileTime;
    }

    return -1;
  }

// bool GetFileLastModified(const TCHAR* Filename, SYSTEMTIME& sysTime, bool bLocalTime)
// {
//     ZeroMemory(&sysTime, sizeof(SYSTEMTIME));
//
//     DWORD dwAttr = ::GetFileAttributes(Filename);
//
//     // files only
//     if (dwAttr == 0xFFFFFFFF)
//         return false;
//
//     WIN32_FIND_DATA findFileData;
//     HANDLE hFind = ::FindFirstFile((LPTSTR)Filename, &findFileData);
//
//     if (hFind == INVALID_HANDLE_VALUE)
//         return FALSE;
//
//     ::FindClose(hFind);
//
//     FILETIME ft = findFileData.ftLastWriteTime;
//
//     if (bLocalTime)
//         ::FileTimeToLocalFileTime(&findFileData.ftLastWriteTime, &ft);
//
//     ::FileTimeToSystemTime(&ft, &sysTime);
//     return true;
// }

  bool NFileManagerWindows::SetDefaultDirectory()
  {
    return CALL_OS_TCHAR_FUNCTION(SetCurrentDirectoryW(GetProgramDirectory()), SetCurrentDirectoryA(TCHAR_TO_ANSI(GetProgramDirectory().c_str()))) != 0;
  }

  std::string NFileManagerWindows::GetCurrentDirectory()
  {
#if UNICODE
    TCHAR Buffer[1024] = TEXT ("");
    ::GetCurrentDirectoryW (NUX_ARRAY_COUNT (Buffer), Buffer);
    return std::string(Buffer);
#else
    ANSICHAR Buffer[1024] = "";
    ::GetCurrentDirectoryA (NUX_ARRAY_COUNT (Buffer), Buffer);
    return std::string(Buffer);
#endif
  }

  bool NFileManagerWindows::GetTimeStamp (const TCHAR *Filename, FileTimeStamp &Timestamp)
  {
    Memzero (&Timestamp, sizeof (Timestamp) );
    struct _stat FileInfo;

    if (_tstat (Filename, &FileInfo) == 0)
    {
#ifdef WIN32_SECURE
      __time64_t FileTime;
      // FileTime represents seconds elapsed since midnight (00:00:00), January 1, 1970, coordinated universal time (UTC).
      FileTime = FileInfo.st_mtime;
      tm pTime;
      // _gmtime64_s can express time up to 23:59:59, December 31, 3000, UTC
      _gmtime64_s (&pTime, &FileTime);

      Timestamp.Day       = pTime.tm_mday;
      Timestamp.DayOfWeek = pTime.tm_wday;
      Timestamp.DayOfYear = pTime.tm_yday;
      Timestamp.Hour      = pTime.tm_hour;
      Timestamp.Minute    = pTime.tm_min;
      Timestamp.Second    = pTime.tm_sec;
      Timestamp.Year      = pTime.tm_year + 1900;
#else
      time_t  FileTime;
      // FileTime represents seconds elapsed since midnight (00:00:00), January 1, 1970, coordinated universal time (UTC).
      FileTime = FileInfo.st_mtime;
      // gmtime can express time up to 03:14:07 January 19, 2038, UTC
      tm *pTime = gmtime (&FileTime);

      Timestamp.Day       = pTime->tm_mday;
      Timestamp.DayOfWeek = pTime->tm_wday;
      Timestamp.DayOfYear = pTime->tm_yday;
      Timestamp.Hour      = pTime->tm_hour;
      Timestamp.Minute    = pTime->tm_min;
      Timestamp.Second    = pTime->tm_sec;
      Timestamp.Year      = pTime->tm_year + 1900;
#endif
      return TRUE;
    }

    return FALSE;
  }

}

