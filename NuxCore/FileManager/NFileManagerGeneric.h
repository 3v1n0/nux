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


#ifndef NFILEMANAGERGENERIC_H
#define NFILEMANAGERGENERIC_H

namespace nux
{

  class NFileManagerWindows;

  class NFileTransferMonitor
  {
  public:
    NFileTransferMonitor()
    {
      m_bCancel = false;
    }
    virtual ~NFileTransferMonitor() {}
    void Cancel()
    {
      m_bCancel = true;
    }
    virtual bool Progress (float Fraction) = 0;

#ifdef _WIN32
    static DWORD CALLBACK CopyProgressRoutine (
      NUX_IN                 LARGE_INTEGER TotalFileSize,
      NUX_IN                 LARGE_INTEGER TotalBytesTransferred,
      NUX_IN                 LARGE_INTEGER StreamSize,
      NUX_IN                 LARGE_INTEGER StreamBytesTransferred,
      NUX_IN                 DWORD dwStreamNumber,
      NUX_IN                 DWORD dwCallbackReason,
      NUX_IN                 HANDLE hSourceFile,
      NUX_IN                 HANDLE hDestinationFile,
      NUX_IN                 LPVOID lpData
    )
    {
      NFileTransferMonitor *filetransfer = NUX_STATIC_CAST (NFileTransferMonitor *, lpData);

      if (filetransfer)
      {
        if (filetransfer->Progress (100.0 * double (TotalBytesTransferred.QuadPart) / double (TotalFileSize.QuadPart) ) == false)
        {
          return PROGRESS_CANCEL;
        }
      }

      return PROGRESS_CONTINUE;
    }

//private:
#endif
    BOOL m_bCancel;
    friend class NFileManagerWindows;
  };

  class NFileManager
  {
  public:

    NFileManager() {}
    virtual ~NFileManager() {}
    /** Timestamp structure */
    struct FileTimeStamp
    {
      // Time is in UTC
      INT     Year;           /* year                             */
      INT     Month;          /* months since January - [0,11]    */
      INT     Day;            /* day of the month - [1,31]        */
      INT     Hour;           /* hours since midnight - [0,23]    */
      INT     Minute;         /* minutes after the hour - [0,59]  */
      INT     Second;         /* seconds after the minute - [0,59]*/
      INT     DayOfWeek;      /* days since Sunday - [0,6]        */
      INT     DayOfYear;      /* days since January 1 - [0,365]   */

      INT     GetJulian     ( void )         const;
      INT     GetSecondOfDay ( void )         const;
      bool   operator == ( FileTimeStamp &Other ) const;
      bool   operator != ( FileTimeStamp &Other ) const;
      bool   operator < ( FileTimeStamp &Other ) const;
      bool   operator > ( FileTimeStamp &Other ) const;
      bool   operator >= ( FileTimeStamp &Other ) const;
      bool   operator <= ( FileTimeStamp &Other ) const;
    };

    virtual void Init (bool /* Startup */) {}
    virtual NSerializer *CreateFileReader ( const TCHAR *Filename, DWORD ReadFlags = 0, LogOutputDevice &Error = GNullDevice ) = 0;
    virtual NSerializer *CreateFileWriter ( const TCHAR *Filename, DWORD WriteFlags = 0, LogOutputDevice &Error = GNullDevice ) = 0;
    //! Return TRUE if the file exist.
    /*!
        Return TRUE if the file exist.
        @param Filename the full path of the file to search.
        @return TRUE if the file exist.
    */
    virtual long long FileSize (const TCHAR *Filename) = 0;   // Max file size is 16 terabytes minus 64 KB on NTFS. 4 gigabytes on Fat32.
    virtual bool FileExist (const TCHAR *Filename) = 0;
    virtual int Copy (const TCHAR *Dest, const TCHAR *Src, bool OverWriteExisting = true, bool OverWriteReadOnly = false, NFileTransferMonitor *Progress = NULL) = 0;
    virtual bool Move (const TCHAR *Dest, const TCHAR *Src, bool OverWriteExisting = true, bool OverWriteReadOnly = false, NFileTransferMonitor *Monitor = NULL) = 0;
    virtual bool Delete (const TCHAR *Filename, bool OverWriteReadOnly = false) = 0;
    virtual bool IsReadOnly (const TCHAR *Filename) = 0;
    virtual bool IsDirectory (const TCHAR *DirectoryName) = 0;
    virtual bool IsHidden (const TCHAR *Filename) = 0;

    virtual bool GetFileAttribute (const TCHAR *Filename, bool &isDirectory, bool &IsReadOnly, bool &IsHidden, long long &Size) = 0;
    virtual bool MakeDirectory (const TCHAR *Path, bool CreateCompletePath = false) = 0;
    //! Delete directory
    /*!
        Delete a Directory. If DeleteContent is true, The content of the directory is deleted before the directory itself;

        @param Path Path of the directory
        @param DeleteContentFirst Delete the content of the directory before deleting the directory itself.
        @return TRUE if the directory was deleted.
    */
    virtual bool DeleteDirectory (const TCHAR *Path, bool DeleteContentFirst = false) = 0;

    /*!
        Creates a unique file name. The format of the name is "DirectoryPath/BaseName####.Extension" where #### is a 4-digit number in [0, 9999].
        The new name is unique and does not exist in the path directory.
        The function returns the value of the index created for the new file name or -1 if none could be found.
        The return value can be saved and passed the he next call to CreateUniqueFileName in order to speed up the search.
        Example usage:
            Create a new file name for of form DirectoryPath/Filename####.ext
            CreateUniqueFileName(TEXT("DirectoryPath/Filename"), TEXT("ext"), Output);

        @param	Filename		Filename with optional path.
        @param	Extension		Extension.
        @param	OutputFilename	New filename.
        @param	BaseIndex		Base for index search.
        @return					Index of the new file. -1 if the file couldn't be created The index has to be in the range [0, 9999].
    */
    virtual int CreateUniqueFileName (const TCHAR *Filename, const TCHAR *Extension, std::string &OutputFilename, unsigned int BaseIndex = 0xffffffff) = 0;


    virtual void FindFiles ( std::vector<std::string>& FileNames, const TCHAR *Filename, bool Files, bool Directories ) = 0;
    virtual void ListFilesInDirectory ( std::vector<std::string>& Result, const TCHAR *DirName) = 0;
    virtual time_t GetFileLastModified (const TCHAR *Filename) = 0;
    virtual double GetFileAgeSeconds (const TCHAR *Filename) = 0;
    virtual bool SetDefaultDirectory() = 0;
    virtual std::string GetCurrentDirectory() = 0;
    virtual bool GetTimeStamp ( const TCHAR *Path, FileTimeStamp &Timestamp ) = 0;


  protected:

  };

  class NFileManagerGeneric : public NFileManager
  {
  public:
    int Copy (const TCHAR *InDestFile, const TCHAR *InSrcFile, bool OverWriteExisting, bool OverWriteReadOnly, NFileTransferMonitor *Monitor);
    bool MakeDirectory (const TCHAR *Path, bool CreateCompletePath = false);
    bool DeleteDirectory (const TCHAR *Path, bool DeleteContentFirst = false);
    bool Move (const TCHAR *Dest, const TCHAR *Src, bool OverWriteExisting = true, bool OverWriteReadOnly = false, NFileTransferMonitor *Monitor = NULL);
    int CreateUniqueFileName (const TCHAR *Filename, const TCHAR *Extension, std::string &OutputFilename, unsigned int BaseIndex = 0xffffffff);

    bool IsDrive (const TCHAR *Path);
  };

}

#endif // NFILEMANAGERGENERIC_H

