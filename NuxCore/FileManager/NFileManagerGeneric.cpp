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
#include "Math/MathUtility.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

#define COPYBLOCKSIZE	32768


  int NFileManagerGeneric::Copy (const TCHAR *InDestFile, const TCHAR *InSrcFile, bool OverWriteExisting, bool OverWriteReadOnly, NFileTransferMonitor *Monitor)
  {
    // Direct file copier.
    if (Monitor && !Monitor->Progress (0.0) )
    {
      return COPY_CANCELED;
    }

    int	    Result		= COPY_OK;
    NString SrcFile		= InSrcFile;
    NString DestFile	= InDestFile;

    NSerializer *Src = CreateFileReader (SrcFile.GetTCharPtr() );

    if (!Src)
    {
      Result = COPY_READFAIL;
    }
    else
    {
      t_u32 Size = Src->GetFileSize();
      NSerializer *Dest = CreateFileWriter (DestFile.GetTCharPtr(), (OverWriteExisting ? 0 : FILEWRITE_NOREPLACEEXISTING) | (OverWriteReadOnly ? FILEWRITE_EVENIFREADONLY : 0) );

      if (!Dest)
      {
        Result = COPY_WRITEFAIL;
      }
      else
      {
        t_u32 Percent = 0, NewPercent = 0;
        BYTE Buffer[COPYBLOCKSIZE];

        for (t_u32 Total = 0; Total < Size; Total += sizeof (Buffer) )
        {
          t_u32 Count = Min<t_u32> (Size - Total, (t_u32) sizeof (Buffer) );
          Src->Serialize (Buffer, Count);

          if (Src->IsError() )
          {
            Result = COPY_READFAIL;
            break;
          }

          Dest->Serialize (Buffer, Count);

          if (Dest->IsError() )
          {
            Result = COPY_WRITEFAIL;
            break;
          }

          NewPercent = Total * 100 / Size;

          if (Monitor && Percent != NewPercent && !Monitor->Progress ( (float) NewPercent / 100.f) )
          {
            Result = COPY_CANCELED;
            break;
          }

          Percent = NewPercent;
        }

        if (Result == COPY_OK)
        {
          if (!Dest->Close() )
          {
            Result = COPY_WRITEFAIL;
          }
        }

        delete Dest;

        if (Result != COPY_OK)
        {
          Delete (DestFile.GetTCharPtr() );
        }
      }

      if (Result == COPY_OK)
      {
        if (!Src->Close() )
        {
          Result = COPY_READFAIL;
        }
      }

      delete Src;
    }

    if (Monitor && Result == COPY_OK && !Monitor->Progress (1.0) )
    {
      Result = COPY_CANCELED;
    }

    return Result;
  }

  bool NFileManagerGeneric::IsDrive (const TCHAR *Path)
  {
    // Does Path refer to a drive letter or UNC path?
    // A UNC is a naming convention that permits you to use a network resource,
    // such as a network server, without formally connecting to the network resource
    // with a mapped drive. A UNC path uses the following syntax:
    //      \\<Server>\<Share>
    // The share is a drive: D:\Folder of ServerA = "\\ServerA\D\Folder"

    if (Stricmp (Path, TEXT ("") ) == 0)
      return 1;
    else if ( (ToUpperCase (Path[0]) != ToLowerCase (Path[0]) ) && (Path[1] == TEXT (':') ) && (Path[2] == 0) ) // look for "a:", "c:", "d:" ...
      return 1;
    else if (Stricmp (Path, TEXT ("\\") ) == 0) // look for "\"
      return 1;
    else if (Stricmp (Path, TEXT ("\\\\") ) == 0) // look for "\\"
      return 1;
    else if (Path[0] == TEXT ('\\') && Path[1] == TEXT ('\\') && !Strchr (Path + 2, TEXT ('\\') ) ) // look for "\\Server"
      return 1;
    else if (Path[0] == TEXT ('\\') && Path[1] == TEXT ('\\') && Strchr (Path + 2, TEXT ('\\') ) && !Strchr (Strchr (Path + 2, TEXT ('\\') ) + 1, TEXT ('\\') ) )
      // look for "\\Server\share"
      return 1;
    else
      return 0;
  }

  bool NFileManagerGeneric::MakeDirectory (const TCHAR *Path, bool CreateCompletePath)
  {
    // Support code for making a directory tree.
    nuxAssert (CreateCompletePath);
    t_u32 SlashCount = 0, CreateCount = 0;

    for (TCHAR Full[256] = TEXT (""), *Ptr = Full; ; *Ptr++ = *Path++)
    {
      if ( (*Path == NUX_BACKSLASH_CHAR) || (*Path == NUX_SLASH_CHAR) || (*Path == 0) )
      {
        if ( (SlashCount++ > 0) && !IsDrive (Full) )
        {
          *Ptr = 0;

          if (MakeDirectory (Full, 0) != NUX_OK)
            return 0;

          CreateCount++;
        }
      }

      if (*Path == 0)
        break;
    }

    return CreateCount != 0;
  }

  bool NFileManagerGeneric::DeleteDirectory (const TCHAR *Path, bool DeleteContentFirst)
  {
    nuxAssert (DeleteContentFirst);
    nuxAssert (Path != NULL);

    t_size PathLength = StringLength (Path);

    if (PathLength == 0)
      return false;

    NString WildcardPath = NString (Path);

    if ( (WildcardPath[PathLength - 1] != NUX_BACKSLASH_CHAR) && (WildcardPath[PathLength - 1] != NUX_SLASH_CHAR) )
      WildcardPath += NUX_BACKSLASH_CHAR;

    WildcardPath += TEXT ("*");

    std::vector<NString> List;
    FindFiles (List, *WildcardPath, 1, 0);

    for (t_u32 i = 0; i < List.size(); i++)
    {
      if (!Delete (* (NString (Path) + NUX_BACKSLASH_CHAR + List[i]), 1) )
        return 0;
    }

    List.clear();
    FindFiles (List, *WildcardPath, 0, 1);

    for (t_u32 i = 0; i < List.size(); i++)
    {
      if (!DeleteDirectory (* (NString (Path) + NUX_BACKSLASH_CHAR + List[i]), true) )
        return 0;
    }

    List.clear();
    return DeleteDirectory (Path, false);
  }

  bool NFileManagerGeneric::Move (const TCHAR *Dest, const TCHAR *Src, bool OverWriteExisting, bool OverWriteReadOnly, NFileTransferMonitor *Monitor)
  {
    // Move file manually.
    if (Copy (Dest, Src, OverWriteExisting, OverWriteReadOnly, NULL) != COPY_OK)
      return 0;

    Delete (Src, 1);
    return 1;
  }


  int NFileManagerGeneric::CreateUniqueFileName (const TCHAR *Filename, const TCHAR *Extension, NString &OutputFilename, unsigned int BaseIndex)
  {
    nuxAssert (Filename);
    nuxAssert (Extension);

    NString FullPath (Filename);
    const t_size IndexMarker = FullPath.Length();			// Marks location of the four-digit index.
    FullPath += TEXT ("0000.");
    FullPath += Extension;

    // Iterate over indices, searching for a file that doesn't exist.
    for (DWORD i = BaseIndex + 1 ; i < 10000 ; ++i)
    {
      FullPath[IndexMarker  ] = i / 1000     + TEXT ('0');
      FullPath[IndexMarker+1] = (i / 100) % 10 + TEXT ('0');
      FullPath[IndexMarker+2] = (i / 10)  % 10 + TEXT ('0');
      FullPath[IndexMarker+3] =   i     % 10 + TEXT ('0');

      if (GFileManager.FileSize (FullPath.GetTCharPtr() ) == -1)
      {
        // The file doesn't exist; output success.
        OutputFilename = FullPath;
        return static_cast<int> (i);
      }
    }

    // Can't find an empty filename slot with index in (StartVal, 9999].
    return -1;
  }

} //NUX_NAMESPACE_END

