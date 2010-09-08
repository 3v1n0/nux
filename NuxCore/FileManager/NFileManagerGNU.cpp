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

NAMESPACE_BEGIN

// Choose the size so it is a power of 2. Example (size-1)= 11111111.
const t_int  NGNUSerialFileReader::sBufferSize = 1024;

NGNUSerialFileReader::NGNUSerialFileReader(t_int InFileDescriptor, NOutputDevice& InError, t_int InSize)
:   m_FileDescriptor    (InFileDescriptor)
,   m_Error             (InError)
,   m_FileSize          (InSize)
,   m_FilePos           (0)
,   m_BufferBase        (0)
,   m_BufferCount       (0)
{
    m_Buffer = new BYTE[sBufferSize];
}
NGNUSerialFileReader::~NGNUSerialFileReader()
{
    NUX_SAFE_DELETE_ARRAY(m_Buffer);
    if(m_FileDescriptor)
    {
        Close();
    }
}

bool NGNUSerialFileReader::Precache(t_int PrecacheOffset, t_int PrecacheSize)
{
    // Only pre-cache at current position and avoid work if pre-caching same offset twice.
    if((m_FilePos == PrecacheOffset) && (!m_BufferBase || !m_BufferCount || m_BufferBase != m_FilePos))
    {
        m_BufferBase = m_FilePos;
        // (sBufferSize - 1) contains only '1', i.e 1111111111. 
        // So (m_FilePos & (sBufferSize-1)) is equal to m_FilePos if m_FilePos <= (sBufferSize-1).
        m_BufferCount = Min<t_s64>(Min<t_s64>(PrecacheSize, (t_int)(sBufferSize - (m_FilePos & (sBufferSize-1)))), m_FileSize - m_FilePos);
        t_u64 Count = 0;
        //GTotalBytesReadViaFileManager += m_BufferCount;
        Count = read(m_FileDescriptor, m_Buffer, m_BufferCount);
        if(Count == 0)
        {
            m_ErrorCode = 1;
            m_Error.LogFunction(TEXT("[NGNUSerialFileReader::Precache] Reached end of file while attempting to read %i bytes"), m_BufferCount);
        }
        if(Count == -1)
        {
            m_ErrorCode = 1;
            m_Error.LogFunction(TEXT("[NGNUSerialFileReader::Precache]  Read error while attempting to read file: ???"));
        }
    }
    return TRUE;
}

t_s64 NGNUSerialFileReader::Seek(t_s64 InPos, NSerializer::SeekPos seekpos)
{
    nuxAssert(InPos >= 0);
    nuxAssert(InPos <= m_FileSize);

    Flush();
    // Because we precache our reads, we must perform Seek accordingly.

    t_s64 pos = m_FilePos;
    t_u64 filepos = 0;

    // Set the file pointer to m_FilePos.
    filepos = lseek(m_FileDescriptor, pos, SEEK_SET);
    if(filepos == -1)
    {
        m_ErrorCode = 1;
        m_Error.LogFunction(TEXT("[NGNUSerialFileReader::Seek] Seek to %i has failed."), InPos);
    }

    // Now the file pointer is current with what we have read so far.
    pos = InPos;
    filepos = lseek(m_FileDescriptor, pos, (seekpos == SeekStart) ? SEEK_SET : (seekpos == SeekCurrent) ? SEEK_CUR : SEEK_END);
    if( filepos == -1)
    {
        m_ErrorCode = 1;
        m_Error.LogFunction(TEXT("[NGNUSerialFileReader::Seek] Seek to %i has failed."), InPos);
    }
    m_FilePos       = filepos;
    m_BufferBase    = 0;
    m_BufferCount   = 0;
    Precache(m_FilePos, sBufferSize);
    return filepos;
}

t_s64 NGNUSerialFileReader::Tell()
{
    //     Flush();
    //     LARGE_INTEGER pos;
    //     LARGE_INTEGER filepos;
    //     pos.QuadPart = 0;
    //     ::SetFilePointerEx(m_FileDescriptor, pos, &filepos, FILE_CURRENT);
    //     return filepos.QuadPart;

    return m_FilePos;
}

t_s64 NGNUSerialFileReader::GetFileSize()
{
    return m_FileSize;
}

bool NGNUSerialFileReader::Close()
{
    if(m_FileDescriptor)
    {
        int ret = close(m_FileDescriptor);
        if(ret == -1)
        {
            m_ErrorCode = 1;
            m_Error.LogFunction(TEXT("[NGNUSerialFileReader::Close] Error while closing file"));
        }
    }
    m_FileDescriptor = 0;
    return !m_ErrorCode;
}

void NGNUSerialFileReader::SerializeFinal(void* Dest, t_u64 Length)
{
    nuxAssert(Dest);
    while(Length > 0)
    {
        t_int DataSize = Min<t_s64>(Length, m_BufferBase + m_BufferCount - m_FilePos);
        if(DataSize == 0)
        {
            if(Length >= sBufferSize)
            {
                t_u64 Count=0;
                //GTotalBytesReadViaFileManager += Length;		
                Count = read(m_FileDescriptor, Dest, Length);
                if(Count == 0)
                {
                    m_ErrorCode = 1;
                    m_Error.LogFunction(TEXT("[NGNUSerialFileReader::Serialize] Reached end of file while attempting to read %i bytes"), Length);
                }
                if(Count == -1)
                {
                    m_ErrorCode = 1;
                    m_Error.LogFunction(TEXT("[NGNUSerialFileReader::Serialize]  Read error while attempting to read file: ???"));
                }

                m_FilePos += Length;
                m_BufferBase += Length;
                return;
            }
            Precache(m_FilePos, t_s32_max);
            DataSize = Min<t_s64>(Length, m_BufferBase + m_BufferCount - m_FilePos);
            if(DataSize <= 0)
            {
                m_ErrorCode = 1;
                m_Error.LogFunction(TEXT("ReadFile beyond EOF %i+%i/%i"), m_FilePos, Length, m_FileSize);
            }
            if(m_ErrorCode)
                return;
        }
        Memcpy(Dest, m_Buffer + m_FilePos - m_BufferBase, DataSize);
        m_FilePos   += DataSize;
        Length      -= DataSize;
        Dest        = (BYTE*)Dest + DataSize;
    }
}
//////////////////////////////////////////////////////////////////////////
// Choose the size so it is a power of 2. Example (size-1)= 11111111.
const t_int  NGNUSerialFileWriter::sBufferSize = 32;

NGNUSerialFileWriter::NGNUSerialFileWriter(t_int InFileDescriptor, NOutputDevice& InError, t_int InPos)
:   m_FileDescriptor(InFileDescriptor)
,   m_Error         (InError)
,   m_Pos           (InPos)
,   m_BufferCount   (0)
{
    m_Pos = Tell();
    m_Buffer = new BYTE[sBufferSize];
}

NGNUSerialFileWriter::~NGNUSerialFileWriter()
{
    NUX_SAFE_DELETE_ARRAY(m_Buffer);
    if(m_FileDescriptor)
        Close();
    m_FileDescriptor = 0;
}

t_s64 NGNUSerialFileWriter::Seek(t_s64 InPos, NSerializer::SeekPos seekpos)
{
    NScopeLock Scope(&m_CriticalSection);
    nuxAssert(m_FileDescriptor);
    if(m_FileDescriptor == 0)
        return -1;

    _Flush();
    t_s64 pos = InPos;
    t_u64 filepos = 0;
    filepos = lseek(m_FileDescriptor, pos, (seekpos == SeekStart) ? SEEK_SET : (seekpos == SeekCurrent) ? SEEK_CUR : SEEK_END);
    if(filepos == -1)
    {
        m_ErrorCode = 1;
        m_Error.LogFunction(TEXT("[NGNUSerialFileWriter::Seek] Seek to %i has failed."), InPos);
    }
    m_Pos = filepos;
    return filepos;
}

t_s64 NGNUSerialFileWriter::Tell()
{
    NScopeLock Scope(&m_CriticalSection);
    nuxAssert(m_FileDescriptor);
    if(m_FileDescriptor == 0)
        return -1;

    _Flush();
    t_s64 pos = 0;
    t_u64 filepos = 0;
    filepos = lseek(m_FileDescriptor, pos, SEEK_CUR);
    if(filepos == -1)
    {
        m_ErrorCode = 1;
        m_Error.LogFunction(TEXT("[NGNUSerialFileWriter::Tell] Seek to %i has failed."), pos);
    }
    return filepos;
}

bool NGNUSerialFileWriter::Close()
{
    NScopeLock Scope(&m_CriticalSection);
    nuxAssert(m_FileDescriptor);
    if(m_FileDescriptor == 0)
        return true;

    _Flush();
    int ret = close(m_FileDescriptor);
    if(ret == -1)
    {
        m_ErrorCode = 1;
        m_Error.LogFunction(TEXT("[NGNUSerialFileWriter::Close] Error while closing file"));
    }
    m_FileDescriptor = 0;
    return !m_ErrorCode;
}

t_s64 NGNUSerialFileWriter::GetFileSize()
{
    NScopeLock Scope(&m_CriticalSection);
    nuxAssert(m_FileDescriptor);
    if(m_FileDescriptor == 0)
        return -1;

    struct stat sb;
    if(fstat(m_FileDescriptor, &sb) != 0)
    {
        m_Error.LogFunction(TEXT("[NGNUSerialFileWriter::GetFileSize] Can't get file size."));
        return -1;
    }
    return sb.st_size;
}

void NGNUSerialFileWriter::SerializeFinal(void* V, t_u64 Length)
{
    // This method is not re-entrant by itself. It relies on m_Buffer and other variables
    // that belong to this object. Therefore, it is not thread safe. We add a critical section
    // to make it thread safe.

    NScopeLock Scope(&m_CriticalSection);
    nuxAssert(m_FileDescriptor);
    nuxAssert(V);

    NUX_RETURN_IF_NULL(m_FileDescriptor);

    m_Pos += Length;
    t_int FreeSpace;
    while(Length > (FreeSpace = sBufferSize - m_BufferCount))
    {
        // m_Buffer is Full. Write it to the file.
        Memcpy(m_Buffer + m_BufferCount, V, FreeSpace);
        m_BufferCount   += FreeSpace;
        Length          -= FreeSpace;
        V               = (BYTE*)V + FreeSpace;
        _Flush();
    }
    if(Length)
    {
        Memcpy(m_Buffer + m_BufferCount, V, Length);
        m_BufferCount += Length; // Count the number of Characters stored in m_Buffer.
    }
}

void NGNUSerialFileWriter::Flush()
{
    NScopeLock Scope(&m_CriticalSection);
    nuxAssert(m_FileDescriptor);
    if(m_FileDescriptor == 0)
        return;
    _Flush();
}

void NGNUSerialFileWriter::_Flush()
{
    //GTotalBytesWrittenViaFileManager += m_BufferCount;
    if(m_BufferCount)
    {
        t_u64 Result = 0;
        Result = write(m_FileDescriptor, m_Buffer, m_BufferCount);
        if(Result == -1)
        {
            m_ErrorCode = 1;
            m_Error.LogFunction(TEXT("[NGNUSerialFileWriter::Flush] Write error."));
        }
    }
    m_BufferCount = 0;
}
//////////////////////////////////////////////////////////////////////////
NUX_IMPLEMENT_GLOBAL_OBJECT(NFileManagerGNU);

void NFileManagerGNU::Constructor()
{
}

void NFileManagerGNU::Destructor()
{
}

NSerializer* NFileManagerGNU::CreateFileReader(const TCHAR* Filename, DWORD Flags, NOutputDevice& Error)
{
    t_int FileDesc = open(TCHAR_TO_ANSI(Filename), O_RDONLY);
    if(FileDesc == -1)
    {
        nuxDebugMsg(TEXT("[NFileManagerGNU::CreateFileReader] Can't create file reade for: %s"), Filename);
        if(Flags & NSerializer::OutputErrorIfFail)
        {
            nuxError(TEXT("[NFileManagerGNU::CreateFileReader] Can't open file: %s"), Filename);
        }
        return NULL;
    }

    struct stat sb;
    if(fstat(FileDesc, &sb) != 0)
    {
        int ret = close(FileDesc);
        if(ret == -1)
        {
            Error.LogFunction(TEXT("[NFileManagerGNU::CreateFileReader] Error while closing file descriptor: %s"), Filename);
        }
        nuxDebugMsg(TEXT("[NFileManagerGNU::CreateFileReader] Can't get file descriptor: %s"), Filename);
        Error.LogFunction(TEXT("[NFileManagerGNU::CreateFileReader] Can't get file descriptor: %s"), Filename);
        return NULL;
    }
    return new NGNUSerialFileReader(FileDesc, Error, sb.st_size);
}

NSerializer* NFileManagerGNU::CreateFileWriter(const TCHAR* Filename,
                                               DWORD Flags,
                                               NOutputDevice& Error)
{
    if(FileExist(Filename) && (Flags & NSerializer::OverWriteReadOnly))
    {
        int ret = chmod(TCHAR_TO_ANSI(Filename), S_IRUSR|S_IWUSR);
        if(ret == -1)
        {
            Error.LogFunction(TEXT("[NFileManagerGNU::CreateFileWriter] Can't change file mode"));
        }
    }

    DWORD  ModeFlags  = 0;
    if((Flags & NSerializer::Read) && (Flags & NSerializer::Write))
    {
        ModeFlags |= O_RDWR;
    }
    else if(Flags & NSerializer::Read)
    {
        ModeFlags |= O_RDONLY;
    }
    else if(Flags & NSerializer::Write)
    {
        ModeFlags |= O_WRONLY;
    }

    ModeFlags |= (Flags & NSerializer::Append) ? O_APPEND : O_TRUNC;
    ModeFlags |= (Flags & NSerializer::NoOverWrite) ? (O_CREAT|O_EXCL) /*fail if the file already exist*/: O_CREAT /*create the file if it does not exist*/;

    t_int FileDesc = open(TCHAR_TO_ANSI(Filename), ModeFlags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if(FileDesc == -1)
    {
        if(Flags & NSerializer::OutputErrorIfFail)
        {
            nuxError(TEXT("[NFileManagerGNU::CreateFileWriter] Failed to create file %s."), Filename);
        }
        return NULL;
    }
    
    t_u64 Pos = 0;
    if(Flags & NSerializer::Append)
    {
        Pos = lseek(FileDesc, Pos, SEEK_END);
        if(Pos == -1)
        {
            Error.LogFunction(TEXT("[NGNUSerialFileReader::Seek] Seek to %i has failed."), Pos);
        }
    }

    struct stat sb;
    if(fstat(FileDesc, &sb) != 0)
    {
        int ret = close(FileDesc);
        if(ret == -1)
        {
            Error.LogFunction(TEXT("[NFileManagerGNU::CreateFileWriter] Error while closing file"));
        }
        Error.LogFunction(TEXT("[NFileManagerGNU::CreateFileWriter] Can't create file reader."));
        return NULL;
    }
    return new NGNUSerialFileWriter(FileDesc, Error, sb.st_size);
}

t_s64 NFileManagerGNU::FileSize(const TCHAR* Filename)
{
    struct stat sb;
    if(stat(TCHAR_TO_ANSI(Filename), &sb) != 0)
    {
        nuxDebugMsg(TEXT("[NFileManagerGNU::FileSize] Can't get file size"));
        return 0;
    }

    if(sb.st_mode & S_IFDIR)
    {
        // This is a directory
        return 0;
    }
    return sb.st_size;
}

bool NFileManagerGNU::FileExist(const TCHAR* Filename)
{
    struct stat sb;
    if(stat(TCHAR_TO_ANSI(Filename), &sb) != 0)
    {
        return false;
    }
    return true;
}

int NFileManagerGNU::Copy(const TCHAR* DestFile,
                                const TCHAR* SrcFile,
                                bool OverWriteExisting,
                                bool OverWriteReadOnly,
                                NFileTransferMonitor* Monitor)
{
    size_t nmemb;
    //int nmemb;
    FILE *ifp, *ofp;
    char buf[BUFSIZ];

    if (OverWriteExisting)
    {
        if (access(DestFile, F_OK) == 0)
        {
            //OUTLOG((FUNC, TRWRN, "file %s already exists\n", DestFile));
            return false;
        }
        else if (errno != ENOENT) {
            //OUTLOG((FUNC, TRERR, "access(%s, F_OK) failed\n", DestFile));
            return false;
        }
    }

    if ((ifp = fopen(SrcFile, "r")) == NULL)
    {
        //OUTLOG((FUNC, TRERR, "%s doesn't exist\n", SrcFile));
        return false;
    }

    if ((ofp = fopen(DestFile, "w+")) == NULL)
    {
        //OUTLOG((FUNC, TRERR, "can't create %s\n", DestFile));
        fclose(ifp);
        return false;
    }

    while ((nmemb = fread(buf, 1, sizeof(buf), ifp)) > 0)
    {
        if (fwrite(buf, 1, nmemb, ofp) != nmemb)
        {
            //OUTLOG((FUNC, TRERR, "fwrite failed\n"));
            fclose(ifp);
            fclose(ofp);
            return false;
        }
    }

    fclose(ifp);
    fclose(ofp);
    return true;
}

bool NFileManagerGNU::Delete(const TCHAR* Filename, bool OverWriteReadOnly)
{
    if(OverWriteReadOnly)
    {
        chmod(TCHAR_TO_ANSI(Filename), S_IRUSR|S_IWUSR);
    }
    if(unlink(TCHAR_TO_ANSI(Filename)) != 0)
    {
        nuxDebugMsg(TEXT("[NFileManagerGNU::Delete] Error deleting file '%s'."), Filename);
        return false;
    }
    return true;
}

bool NFileManagerGNU::IsReadOnly(const TCHAR* Filename)
{
    struct stat sb;
    if(stat(TCHAR_TO_ANSI(Filename), &sb) != 0)
    {
        nuxDebugMsg(TEXT("[NFileManagerGNU::IsReadOnly] Error reading file status '%s'."), Filename);
        return false;
    }
    if((sb.st_mode & S_IRUSR) && !(sb.st_mode & S_IWUSR))
    {
        return true;
    }
    return false;
}

bool NFileManagerGNU::IsDirectory(const TCHAR* DirectoryName)
{
    struct stat sb;
    if(stat(TCHAR_TO_ANSI(DirectoryName), &sb) != 0)
    {
        nuxDebugMsg(TEXT("[NFileManagerGNU::IsDirectory] Error reading file status '%s'."), DirectoryName);
        return false;
    }
    if(sb.st_mode & S_IFDIR)
    {
        return true;
    }
    return false;
}

bool NFileManagerGNU::IsHidden(const TCHAR* Filename)
{
    return false;
}

/*!
    @return TRUE is the file exist.
*/
bool NFileManagerGNU::GetFileAttribute(const TCHAR* Filename, bool& isDirectory, bool& isReadOnly, bool& isHidden, t_s64& Size)
{
    isDirectory = false;
    isReadOnly = false;
    isHidden = false;
    Size = 0;

    struct stat sb;
    if(stat(TCHAR_TO_ANSI(Filename), &sb) != 0)
    {
        return false;
    }

    if(sb.st_mode & S_IFDIR)
    {
        isDirectory = true;
    }
    if((sb.st_mode & S_IRUSR) && !(sb.st_mode & S_IWUSR))
    {
        isReadOnly = true;
    }
    Size = sb.st_mode;
    return true;
}

bool NFileManagerGNU::Move(const TCHAR* Dest, const TCHAR* Src, bool OverWriteExisting, bool OverWriteReadOnly, NFileTransferMonitor* Monitor)
{
    nuxAssert(0);
    return false;
}

bool NFileManagerGNU::MakeDirectory(const TCHAR* Path, bool CreateCompletePath)
{
    if(CreateCompletePath)
    {
        return NFileManagerGeneric::MakeDirectory(Path, CreateCompletePath);
    }

    int error = mkdir(TCHAR_TO_ANSI(Path),
        S_IRUSR|S_IWUSR|S_IXUSR|
        S_IRGRP|S_IWGRP|S_IXGRP|
        S_IROTH|S_IWOTH|S_IXOTH);

    // EEXIST = -2147418092 = 0x80010014
//     if((errno != 0) && (errno != EEXIST))
//     {
//         nuxDebugMsg(TEXT("[NFileManagerGNU::MakeDirectory] Error creating directory '%s'."), Path);
//         return NUX_ERROR;
//     }
    return NUX_OK;
}

bool NFileManagerGNU::DeleteDirectory(const TCHAR* Path, bool DeleteContentFirst)
{
//     if(DeleteContentFirst)
//     {
//         return NFileManagerGeneric::DeleteDirectory(Path, DeleteContentFirst);
//     }
//     if((::RemoveDirectory(Path) == 0) && (::GetLastError() != ERROR_FILE_NOT_FOUND))
//     {
//         nuxDebugMsg(TEXT("[NFileManagerWindows::DeleteDirectory] Error deleting directory '%s' (GetLastError: %d)"), Path, ::GetLastError());
//         return false;
//     }
    return true;
}

NAMESPACE_END

