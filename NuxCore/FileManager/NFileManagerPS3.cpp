#include "NKernel.h"

NAMESPACE_BEGIN

// Choose the size so it is a power of 2. Example (size-1)= 11111111.
const t_int  NPS3SerialFileReader::sBufferSize = 1024;

NPS3SerialFileReader::NPS3SerialFileReader(t_int InFileDescriptor, NOutputDevice& InError, t_int InSize)
:   m_FileDescriptor    (InFileDescriptor)
,   m_Error             (InError)
,   m_FileSize          (InSize)
,   m_FilePos           (0)
,   m_BufferBase        (0)
,   m_BufferCount       (0)
{
    m_Buffer = new BYTE[sBufferSize];
}
NPS3SerialFileReader::~NPS3SerialFileReader()
{
    INL_SAFE_DELETE_ARRAY(m_Buffer);
    if(m_FileDescriptor)
    {
        Close();
    }
}

bool NPS3SerialFileReader::Precache(t_int PrecacheOffset, t_int PrecacheSize)
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
        cellFsRead(m_FileDescriptor, m_Buffer, m_BufferCount, &Count);
        if(Count != m_BufferCount)
        {
            m_ErrorCode = 1;
            m_Error.LogFunction(TEXT("ReadFile failed: Count=%i BufferCount=%i"), Count, m_BufferCount);
        }
    }
    return TRUE;
}

t_s64 NPS3SerialFileReader::Seek(t_s64 InPos, NSerializer::SeekPos seekpos)
{
    nuxAssert(InPos >= 0);
    nuxAssert(InPos <= m_FileSize);

    Flush();
    // Because we precache our reads, we must perform Seek accordingly.

    t_s64 pos = m_FilePos;
    t_u64 filepos = 0;

    // Set the file pointer to m_FilePos.
    if(cellFsLseek(m_FileDescriptor, pos, CELL_FS_SEEK_SET, &filepos) == 0)
    {
        m_ErrorCode = 1;
        m_Error.LogFunction(TEXT("[NPS3SerialFileReader::Seek] Seek to %i has failed."), InPos);
    }

    // Now the file pointer is current with what we have read so far.
    pos = InPos;
    filepos = 0;
    if(cellFsLseek(m_FileDescriptor, pos, (seekpos == SeekStart) ? CELL_FS_SEEK_SET : (seekpos == SeekCurrent) ? CELL_FS_SEEK_CUR : CELL_FS_SEEK_END, &filepos) == 0)
    {
        m_ErrorCode = 1;
        m_Error.LogFunction(TEXT("[NPS3SerialFileReader::Seek] Seek to %i has failed."), InPos);
    }
    m_FilePos   = filepos;
    m_BufferBase  = 0;
    m_BufferCount = 0;
    Precache(m_FilePos, sBufferSize);
    return filepos;
}

t_s64 NPS3SerialFileReader::Tell()
{
    //     Flush();
    //     LARGE_INTEGER pos;
    //     LARGE_INTEGER filepos;
    //     pos.QuadPart = 0;
    //     ::SetFilePointerEx(m_FileDescriptor, pos, &filepos, FILE_CURRENT);
    //     return filepos.QuadPart;

    return m_FilePos;
}

t_s64 NPS3SerialFileReader::GetFileSize()
{
    return m_FileSize;
}

bool NPS3SerialFileReader::Close()
{
    if(m_FileDescriptor)
    {
        cellFsClose(m_FileDescriptor);
    }
    m_FileDescriptor = 0;
    return !m_ErrorCode;
}

void NPS3SerialFileReader::Serialize(void* Dest, t_u64 Length)
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
                cellFsRead(m_FileDescriptor, Dest, Length, &Count);
                if(Count!=Length)
                {
                    m_ErrorCode = 1;
                    m_Error.LogFunction(TEXT("[NPS3SerialFileReader::Serialize] ReadFile failed: Read Requested=%i Read Count=%i"), Length, Count);
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
const t_int  NPS3SerialFileWriter::sBufferSize = 4096;

NPS3SerialFileWriter::NPS3SerialFileWriter(t_int InFileDescriptor, NOutputDevice& InError, t_int InPos)
:   m_FileDescriptor(InFileDescriptor)
,   m_Error         (InError)
,   m_Pos           (InPos)
,   m_BufferCount   (0)
{
    m_Buffer = new BYTE[sBufferSize];
}

NPS3SerialFileWriter::~NPS3SerialFileWriter()
{
    INL_SAFE_DELETE_ARRAY(m_Buffer);
    if(m_FileDescriptor)
        Close();
    m_FileDescriptor = 0;
}

t_s64 NPS3SerialFileWriter::Seek(t_s64 InPos, NSerializer::SeekPos seekpos)
{
    Flush();
    t_s64 pos = InPos;
    t_u64 filepos = 0;
    if(cellFsLseek(m_FileDescriptor, pos, (seekpos == SeekStart) ? CELL_FS_SEEK_SET : (seekpos == SeekCurrent) ? CELL_FS_SEEK_CUR : CELL_FS_SEEK_END, &filepos) == 0)
    {
        m_ErrorCode = 1;
        m_Error.LogFunction(TEXT("SeekFailed"));
    }
    m_Pos = filepos;
    return filepos;
}

t_s64 NPS3SerialFileWriter::Tell()
{
    Flush();
    t_s64 pos = 0;
    t_u64 filepos = 0;
    cellFsLseek(m_FileDescriptor, pos, CELL_FS_SEEK_CUR, &filepos);
    return filepos;
}

bool NPS3SerialFileWriter::Close()
{
    Flush();
    if(m_FileDescriptor && !cellFsClose(m_FileDescriptor))
    {
        m_ErrorCode = 1;
        m_Error.LogFunction(TEXT("WriteFailed"));
    }
    m_FileDescriptor = 0;
    return !m_ErrorCode;
}

t_s64 NPS3SerialFileWriter::GetFileSize()
{
    CellFsStat sb;
    if(cellFsFstat(m_FileDescriptor, &sb) != CELL_FS_SUCCEEDED)
    {
        m_Error.LogFunction(TEXT("[NPS3SerialFileWriter::GetFileSize] Failed."));
        return -1;
    }
    return sb.st_size;
}

void NPS3SerialFileWriter::Serialize(void* V, t_u64 Length)
{
    nuxAssert(V);

    m_Pos += Length;
    t_int FreeSpace;
    while(Length > (FreeSpace = sBufferSize - m_BufferCount))
    {
        // m_Buffer is Full. Write it to the file.
        Memcpy(m_Buffer + m_BufferCount, V, FreeSpace);
        m_BufferCount   += FreeSpace;
        Length          -= FreeSpace;
        V               = (BYTE*)V + FreeSpace;
        Flush();
    }
    if(Length)
    {
        Memcpy(m_Buffer + m_BufferCount, V, Length);
        m_BufferCount += Length; // Count the number of Characters stored in m_Buffer.
    }
}
void NPS3SerialFileWriter::Flush()
{
    //GTotalBytesWrittenViaFileManager += m_BufferCount;
    if(m_BufferCount)
    {
        t_u64 Result = 0;
        CellFsErrno error = cellFsWrite(m_FileDescriptor, m_Buffer, m_BufferCount, &Result);
        if(error != CELL_FS_SUCCEEDED)
        {
            m_ErrorCode = 1;
            m_Error.LogFunction(TEXT("[NPS3SerialFileWriter::Flush] Write error."));
        }
    }
    m_BufferCount = 0;
}
//////////////////////////////////////////////////////////////////////////
INL_IMPLEMENT_GLOBAL_OBJECT(NFileManagerPS3);

void NFileManagerPS3::Constructor()
{
}

void NFileManagerPS3::Destructor()
{
}

NSerializer* NFileManagerPS3::CreateFileReader(const TCHAR* Filename, DWORD Flags, NOutputDevice& Error)
{
    t_int FileDesc;
    if(cellFsOpen(TCHAR_TO_ANSI(Filename), CELL_FS_O_RDWR, &FileDesc, NULL, 0) != CELL_FS_SUCCEEDED)
    {
        if(Flags & NSerializer::OutputErrorIfFail)
        {
            nuxError(TEXT("Failed to read file: %s"), Filename);
        }
        return NULL;
    }

    CellFsStat sb;
    if(cellFsFstat(FileDesc, &sb) != CELL_FS_SUCCEEDED)
    {
        cellFsClose(FileDesc);
        return NULL;
    }
    return new NPS3SerialFileReader(FileDesc, Error, sb.st_size);
}

NSerializer* NFileManagerPS3::CreateFileWriter(const TCHAR* Filename,
                                               DWORD Flags,
                                               NOutputDevice& Error)
{
    if(Flags & NSerializer::OverWriteReadOnly)
    {
        cellFsChmod(TCHAR_TO_ANSI(Filename), CELL_FS_S_IRUSR|CELL_FS_S_IWUSR);
    }

    DWORD  ModeFlags  = 0;
    if((Flags & NSerializer::Read) && (Flags & NSerializer::Write))
    {
        ModeFlags |= CELL_FS_O_RDWR;
    }
    else if(Flags & NSerializer::Read)
    {
        ModeFlags |= CELL_FS_O_RDONLY;
    }
    else if(Flags & NSerializer::Write)
    {
        ModeFlags |= CELL_FS_O_WRONLY;
    }

    ModeFlags |= (Flags & NSerializer::Append) ? CELL_FS_O_APPEND : 0;
    ModeFlags |= (Flags & NSerializer::NoOverWrite) ? CELL_FS_O_EXCL /*fail if the file already exist*/: CELL_FS_O_CREAT /*create the file if it does not exist*/;

    t_int FileDesc = 0;
    if(cellFsOpen(TCHAR_TO_ANSI(Filename), ModeFlags, &FileDesc, NULL, 0) != CELL_FS_SUCCEEDED)
    {
        if(Flags & NSerializer::OutputErrorIfFail)
        {
            nuxError(TEXT("[NFileManagerPS3::CreateFileWriter] Failed to create file %s."), Filename);
        }
        return NULL;
    }
    t_u64 Pos = 0;
    if(Flags & NSerializer::Append)
    {
        cellFsLseek(FileDesc, 0, CELL_FS_SEEK_END, &Pos);
    }
    CellFsStat sb;
    if(cellFsFstat(FileDesc, &sb) != CELL_FS_SUCCEEDED)
    {
        cellFsClose(FileDesc);
        return NULL;
    }
    return new NPS3SerialFileWriter(FileDesc, Error, sb.st_size);
}

t_s64 NFileManagerPS3::FileSize(const TCHAR* Filename)
{
    CellFsStat sb;
    if(cellFsStat(TCHAR_TO_ANSI(Filename), &sb) != CELL_FS_SUCCEEDED)
    {
        nuxDebugMsg(TEXT("[NFileManagerPS3::FileSize] Can't get file size"));
        return 0;
    }
    if(sb.st_mode & CELL_FS_S_IFDIR)
    {
        // This is a directory
        return 0;
    }
    if(sb.st_mode & CELL_FS_S_IFWHT)
    {
        // The type is unknown
        //return 0;
    }
    if(sb.st_mode & CELL_FS_S_IFLNK)
    {
        // This is a symbolic link
        //return 0;
    }
    return sb.st_size;
}

bool NFileManagerPS3::FileExist(const TCHAR* Filename)
{
    CellFsStat sb;
    if(cellFsStat(TCHAR_TO_ANSI(Filename), &sb) != CELL_FS_SUCCEEDED)
    {
        return false;
    }
    return true;
}

int NFileManagerPS3::Copy(const TCHAR* DestFile,
                                const TCHAR* SrcFile,
                                bool OverWriteExisting,
                                bool OverWriteReadOnly,
                                NFileTransferMonitor* Monitor)
{
    nuxAssert(0);
    return false;
}

bool NFileManagerPS3::Delete(const TCHAR* Filename, bool OverWriteReadOnly)
{
    if(OverWriteReadOnly)
    {
        cellFsChmod(TCHAR_TO_ANSI(Filename), CELL_FS_S_IRUSR|CELL_FS_S_IWUSR);
    }
    if(cellFsUnlink(TCHAR_TO_ANSI(Filename)) != CELL_FS_SUCCEEDED)
    {
        nuxDebugMsg(TEXT("[NFileManagerPS3::Delete] Error deleting file '%s'."), Filename);
        return false;
    }
    return true;
}

bool NFileManagerPS3::IsReadOnly(const TCHAR* Filename)
{
    CellFsStat sb;
    if(cellFsStat(TCHAR_TO_ANSI(Filename), &sb) != CELL_FS_SUCCEEDED)
    {
        nuxDebugMsg(TEXT("[NFileManagerPS3::IsReadOnly] Error reading file status '%s'."), Filename);
        return false;
    }
    if((sb.st_mode & CELL_FS_S_IRUSR) && !(sb.st_mode & CELL_FS_S_IWUSR))
    {
        return true;
    }
    return false;
}

bool NFileManagerPS3::IsDirectory(const TCHAR* DirectoryName)
{
    CellFsStat sb;
    if(cellFsStat(TCHAR_TO_ANSI(DirectoryName), &sb) != CELL_FS_SUCCEEDED)
    {
        nuxDebugMsg(TEXT("[NFileManagerPS3::IsDirectory] Error reading file status '%s'."), DirectoryName);
        return false;
    }
    if(sb.st_mode & CELL_FS_S_IFDIR)
    {
        return true;
    }
    return false;
}

bool NFileManagerPS3::IsHidden(const TCHAR* Filename)
{
    return false;
}

/*!
    @return TRUE is the file exist.
*/
bool NFileManagerPS3::GetFileAttribute(const TCHAR* Filename, bool& isDirectory, bool& isReadOnly, bool& isHidden, t_s64& Size)
{
    isDirectory = false;
    isReadOnly = false;
    isHidden = false;
    Size = 0;

    CellFsStat sb;
    if(cellFsStat(TCHAR_TO_ANSI(Filename), &sb) != CELL_FS_SUCCEEDED)
    {
        return false;
    }

    if(sb.st_mode & CELL_FS_S_IFDIR)
    {
        isDirectory = true;
    }
    if((sb.st_mode & CELL_FS_S_IRUSR) && !(sb.st_mode & CELL_FS_S_IWUSR))
    {
        isReadOnly = true;
    }
    Size = sb.st_mode;
    return true;
}

bool NFileManagerPS3::Move(const TCHAR* Dest, const TCHAR* Src, bool OverWriteExisting, bool OverWriteReadOnly, NFileTransferMonitor* Monitor)
{
    nuxAssert(0);
    return false;
}

bool NFileManagerPS3::MakeDirectory(const TCHAR* Path, bool CreateCompletePath)
{
    if(CreateCompletePath)
    {
        return NFileManagerGeneric::MakeDirectory(Path, CreateCompletePath);
    }

    CellFsErrno error = cellFsMkdir(TCHAR_TO_ANSI(Path), 
        CELL_FS_S_IRUSR|CELL_FS_S_IWUSR|CELL_FS_S_IXUSR|
        CELL_FS_S_IRGRP|CELL_FS_S_IWGRP|CELL_FS_S_IXGRP|
        CELL_FS_S_IROTH|CELL_FS_S_IWOTH|CELL_FS_S_IXOTH);

    // CELL_FS_EEXIST = -2147418092 = 0x80010014
    if((error != CELL_FS_SUCCEEDED) && (error != CELL_FS_EEXIST))
    {
        nuxDebugMsg(TEXT("[NFileManagerPS3::MakeDirectory] Error creating directory '%s'."), Path);
        return INL_ERROR;
    }
    return INL_OK;
}

bool NFileManagerPS3::DeleteDirectory(const TCHAR* Path, bool DeleteContentFirst)
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

