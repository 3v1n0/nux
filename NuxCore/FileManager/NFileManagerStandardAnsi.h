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


#ifndef NFILEMANAGERSTANDARDANSI_H
#define NFILEMANAGERSTANDARDANSI_H

/*-----------------------------------------------------------------------------
File Manager.
-----------------------------------------------------------------------------*/
NAMESPACE_BEGIN
// File manager.
class NSerializeFileReader : public NSerializer
{
public:
    NSerializeFileReader(FILE* InFile, NOutputDevice* InError, t_int InSize);
    ~NSerializeFileReader();

    virtual bool Precache(t_int PrecacheOffset, t_int PrecacheSize);
    virtual t_s64 Seek(t_s64 InPos, NSerializer::SeekPos seekpos);
    virtual t_s64 Tell();
    virtual t_s64 GetFileSize();
    virtual bool Close();
    virtual void SerializeFinal(void* Dest, t_u64 Length);
    virtual bool isReader() {return true;}
    virtual bool isWriter() {return false;}

protected:
    FILE*			m_File;
    NOutputDevice*	m_Error;
    t_int				m_FileSize;
    t_int				m_FilePos;
    t_int				BufferBase;
    t_int				BufferCount;
    BYTE*               m_PreCacheBuffer;
    static const t_int  sBufferSize;
};

class NSerializeFileWriter : public NSerializer
{
public:
    NSerializeFileWriter(FILE* InFile, NOutputDevice* InError);
    ~NSerializeFileWriter();

    virtual t_s64 Seek(t_s64 InPos, NSerializer::SeekPos seekpos);
    virtual t_s64 Tell();
    virtual t_s64 GetFileSize();
    virtual bool Close();
    virtual void SerializeFinal(void* Src, t_u64 Length);
    virtual void Flush();
    virtual bool isReader() {return false;}
    virtual bool isWriter() {return true;}

protected:
    FILE*               m_File;
    NOutputDevice*	m_Error;
    t_int               m_FilePos;
    t_int               m_CachePos;
    BYTE*               m_CacheBuffer;
    static const t_int  sBufferSize;
};

class NFileManagerAnsi : public NFileManagerGeneric
{
public:
    NSerializer* CreateFileReader(const TCHAR* Filename, DWORD Flags, NOutputDevice* Error)
    {
        FILE* File;
        #ifdef WIN32_SECURE
            ::_tfopen_s(&File, Filename,TEXT("rb"));
        #else
            File = ::fopen(Filename,TEXT("rb"));
        #endif
        if(!File)
        {
            if(Flags & NSerializer::OutputErrorIfFail)
            {
                nuxError(TEXT("Failed to read file: %s"), Filename);
            }
            return NULL;
        }
        ::fseek(File, 0, SEEK_END);
        return new NSerializeFileReader(File, Error, ftell(File));
    }

    NSerializer* CreateFileWriter(const TCHAR* Filename, DWORD Flags, NOutputDevice* Error)
    {
        if(!FileExist(Filename))
            return NULL;

        if(Flags & NSerializer::NoOverWrite)
        {
            return NULL;
        }

        if(Flags & NSerializer::OverWriteReadOnly)
        {
            ::_chmod(Filename, _S_IREAD | _S_IWRITE);
        }

        const TCHAR* Mode = (Flags & NSerializer::Append) ? TEXT("ab") : TEXT("wb"); 
        FILE* File;
        #ifdef WIN32_SECURE
            ::_tfopen_s(&File, Filename, Mode);
        #else
            File = ::fopen(Filename, Mode);
        #endif
        if(!File)
        {
            if(Flags & NSerializer::OutputErrorIfFail)
            {
                nuxError(TEXT("Failed to write: %s"), Filename);
            }
            return NULL;
        }
        if(Flags & NSerializer::Append)
        {
            ::fseek(File, 0, SEEK_END);
        }
//         if(Flags & NSerializer::Unbuffered)
//         {
//             ::setvbuf(File, 0, _IONBF, 0);
//         }
        return new NSerializeFileWriter(File, Error);
    }

    bool Delete(const TCHAR* Filename, bool OverWriteReadOnly = false)
    {
        if(OverWriteReadOnly)
            _chmod(Filename, _S_IREAD | _S_IWRITE);
        return (_unlink(Filename) == 0) || (errno == ENOENT);
    }

    bool FileExist(const TCHAR* Filename)
    {
        return ::_access(TCHAR_TO_ANSI(Filename), 00) == 0;
    }

    bool IsDirectory(const TCHAR* DirectoryName)
    {
        struct _stat FileInfo;
        if(_stat(TCHAR_TO_ANSI(DirectoryName), &FileInfo))
        {
            if(FileInfo.st_mode & _S_IFDIR)
                return TRUE;
        }
        return FALSE;
    }

    bool IsReadOnly(const TCHAR* Filename)
    {
        return ::_access(TCHAR_TO_ANSI(Filename), 04) == 0;
    }

    bool MakeDirectory(const TCHAR* Path, bool CreateCompletePath = false)
    {
        if(CreateCompletePath)
        {
            return NFileManagerGeneric::MakeDirectory(Path, CreateCompletePath);
        }
        if((::_mkdir(TCHAR_TO_ANSI(Path)) != 0) && (errno != EEXIST))
        {
            nuxDebugMsg(TEXT("[NFileManagerAnsi::MakeDirectory] Error creating directory"));
            return NUX_ERROR;
        }
        return NUX_OK;
    }

    bool DeleteDirectory(const TCHAR* Path, bool DeleteContentFirst = false)
    {
        if(DeleteContentFirst)
        {
            return NFileManagerGeneric::DeleteDirectory(Path, DeleteContentFirst);
        }
        if((::_rmdir(TCHAR_TO_ANSI(Path)) != 0) && (errno != EEXIST))
        {
            nuxDebugMsg(TEXT("[NFileManagerAnsi::DeleteDirectory] Error deleting directory"));
            return NUX_ERROR;
        }
        return NUX_OK;
    }

    void FindFiles(std::vector<NString>& Result, const TCHAR* Filename, bool Files, bool Directories)
    {
        _finddata_t Found;
        long Handle = ::_findfirst(TCHAR_TO_ANSI(Filename), &Found);
        if(Handle != -1)
        {
            do
            {
                const TCHAR* Name = ANSI_TO_TCHAR(Found.name);
                if(TCharStringCompare(ANSI_TO_TCHAR(Found.name), TEXT(".")) &&
                    TCharStringCompare(ANSI_TO_TCHAR(Found.name), TEXT("..")) &&
                    !(Found.attrib & _A_SYSTEM) &&
                    !(Found.attrib & _A_HIDDEN))
                {
                    if((Found.attrib & _A_SUBDIR) ? Directories : Files)
                        Result.push_back(NString(Name));
                }
            }
            while(::_findnext(Handle, &Found) != -1);
        }
        ::_findclose(Handle);
    }

    void ListFilesInDirectory(std::vector<NString>& Result, const TCHAR* DirName)
    {

    }

    double GetFileAgeSeconds(const TCHAR* Filename)
    {
        struct stat FileInfo;
        if(stat(TCHAR_TO_ANSI(Filename), &FileInfo))
        {
            time_t	CurrentTime, FileTime;	
            FileTime = FileInfo.st_mtime;
            time(&CurrentTime);

            return ::difftime(CurrentTime, FileTime);
        }
        return -1.0;
    }

    time_t GetFileLastModified(const TCHAR* Filename)
    {
        struct stat FileInfo;
        if(stat(TCHAR_TO_ANSI(Filename), &FileInfo))
        {
            time_t FileTime;	
            FileTime = FileInfo.st_mtime;

            return FileTime;
        }
        return -1;
    }

    bool SetDefaultDirectory()
    {
        #ifdef WIN32_SECURE
            return _tchdir(GetProgramDirectory().GetTCharPtr()) == 0;
        #else
            // chdir has been deprecated in Visual 2005 for _chdir.
            return ::chdir(TCHAR_TO_ANSI(GetProgramDirectory())) == 0;
        #endif
    }

    NString GetCurrentDirectory()
    {
        char Buffer[1024]= "";
        ::_tgetcwd(Buffer, NUX_ARRAY_COUNT(Buffer));
        return NString(Buffer);
    }

    bool GetTimeStamp(const TCHAR* Filename, FileTimeStamp& Timestamp)
    {
        Memzero(&Timestamp, sizeof(Timestamp));

#if (defined NUX_VISUAL_STUDIO_2005) || (defined NUX_VISUAL_STUDIO_2008)
        struct __stat64 FileInfo;
        if(_tstat64(Filename, &FileInfo) == 0)
        {
            time_t	FileTime;	
            FileTime = FileInfo.st_mtime;
            tm* pTime = 0;
            ::_gmtime64_s(pTime, &FileTime);

            Timestamp.Day       = pTime->tm_mday;
            Timestamp.DayOfWeek = pTime->tm_wday;
            Timestamp.DayOfYear = pTime->tm_yday;
            Timestamp.Hour      = pTime->tm_hour;
            Timestamp.Minute    = pTime->tm_min;
            Timestamp.Second    = pTime->tm_sec;
            Timestamp.Year      = pTime->tm_year + 1900;
            return TRUE;
        }
#else
        struct stat FileInfo;
        if(stat(Filename, &FileInfo) == 0)
        {
            time_t	FileTime;	
            FileTime = FileInfo.st_mtime;
            tm* pTime = ::gmtime(&FileTime);

            Timestamp.Day       = pTime->tm_mday;
            Timestamp.DayOfWeek = pTime->tm_wday;
            Timestamp.DayOfYear = pTime->tm_yday;
            Timestamp.Hour      = pTime->tm_hour;
            Timestamp.Minute    = pTime->tm_min;
            Timestamp.Second    = pTime->tm_sec;
            Timestamp.Year      = pTime->tm_year + 1900;
            return TRUE;
        }
#endif
        return FALSE;
    }
};

NAMESPACE_END

#endif // NFILEMANAGERSTANDARDANSI_H
