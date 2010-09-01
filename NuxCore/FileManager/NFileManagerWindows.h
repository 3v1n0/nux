#ifndef NFILEMANAGERWINDOWS_H
#define NFILEMANAGERWINDOWS_H

/*-----------------------------------------------------------------------------
File Manager.
-----------------------------------------------------------------------------*/

// File manager.
NAMESPACE_BEGIN
class NWindowsSerialFileReader: public NSerializer
{
public:
    NWindowsSerialFileReader(HANDLE InHandle, NOutputDevice& InError);
    ~NWindowsSerialFileReader();

    virtual bool Precache(t_int PrecacheOffset, t_int PrecacheSize);
    virtual t_s64 Seek(t_s64 InPos, NSerializer::SeekPos seekpos);
    virtual t_s64 Tell();
    virtual t_s64 GetFileSize();
    virtual bool Close();
    virtual void SerializeFinal(void* V, t_u64 Length);
    virtual bool isReader() {return true;}
    virtual bool isWriter() {return false;}

protected:
    HANDLE              m_FileHandle;
    NOutputDevice& m_Error;
    t_s64               m_FileSize;
    t_s64               m_FilePos;
    t_s64               m_BufferBase;
    t_int               m_BufferCount;
    BYTE*               m_Buffer;
    static const t_int  sBufferSize;
};

class NWindowsSerialFileWriter : public NSerializer
{
public:
    NWindowsSerialFileWriter(HANDLE InHandle, NOutputDevice& InError);
    ~NWindowsSerialFileWriter();

    virtual t_s64 Seek(t_s64 InPos, NSerializer::SeekPos seekpos);
    virtual t_s64 Tell();
    virtual bool Close();
    virtual void SerializeFinal(void* V, t_u64 Length);
    virtual void Flush();
    virtual t_s64 GetFileSize();
    virtual bool isReader() {return false;}
    virtual bool isWriter() {return true;}

protected:
    void _Flush();
    HANDLE              m_FileHandle;
    NOutputDevice& m_Error;
    t_s64               m_Pos;
    t_int               m_BufferCount;
    BYTE*               m_Buffer;
    static const t_int  sBufferSize;
    NCriticalSection m_CriticalSection;
};

class NFileManagerWindows : public NFileManagerGeneric
{
    INL_DECLARE_GLOBAL_OBJECT(NFileManagerWindows, NGlobalSingletonInitializer);
public:

    // Flags is a combination of
    //  NSerializer::OutputErrorIfFail
    //  NSerializer::NoOverWrite
    //  NSerializer::OverWriteReadOnly
    //  NSerializer::Unbuffered
    //  NSerializer::Append
    //  NSerializer::Read
    virtual NSerializer* CreateFileReader(const TCHAR* Filename, DWORD Flags, NOutputDevice& Error = GNullDevice);
    virtual NSerializer* CreateFileWriter(const TCHAR* Filename, DWORD Flags, NOutputDevice& Error = GNullDevice);
    HANDLE CreateReadFileHandle(const TCHAR* Filename, DWORD Flags);
    HANDLE CreateWriteFileHandle(const TCHAR* Filename, DWORD Flags);

    /*!
        @return Size of the File. Return -1 if an error occurs.
    */
    t_s64 FileSize(const TCHAR* Filename);
    bool FileExist(const TCHAR* Filename);
    int Copy(const TCHAR* DestFile, const TCHAR* SrcFile, bool OverWriteExisting, bool OverWriteReadOnly, NFileTransferMonitor* Monitor);
    bool Move(const TCHAR* Dest, const TCHAR* Src, bool OverWriteExisting = true, bool OverWriteReadOnly = false, NFileTransferMonitor* Monitor = NULL);
    bool Delete(const TCHAR* Filename, bool OverWriteReadOnly = false);
    bool IsReadOnly(const TCHAR* Filename);
    bool IsDirectory(const TCHAR* DirectoryName);
    bool IsHidden(const TCHAR* Filename);
    /*!
        @return TRUE is the file exist.
    */
    bool GetFileAttribute(const TCHAR* Filename, bool& isDirectory, bool& isReadOnly, bool& isHidden, t_s64& Size);
    bool MakeDirectory(const TCHAR* Path, bool CreateCompletePath = false);
    bool DeleteDirectory(const TCHAR* Path, bool DeleteContentFirst = false);


    void FindFiles(std::vector<NString>& Result, const TCHAR* Filename, bool Files, bool Directories);
    void ListFilesInDirectory(std::vector<NString>& Result, const TCHAR* DirName);
    double GetFileAgeSeconds(const TCHAR* Filename);
    time_t GetFileLastModified(const TCHAR* Filename);
    bool GetFileLastModified(const TCHAR* Filename, SYSTEMTIME& sysTime, bool bLocalTime);
    bool SetDefaultDirectory();
    NString GetCurrentDirectory();
    bool GetTimeStamp(const TCHAR* Filename, FileTimeStamp& Timestamp);
};

NAMESPACE_END

#endif // NFILEMANAGERWINDOWS_H

