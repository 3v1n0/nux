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


#include "NuxCore/NuxCore.h"

#include "NuxCore/FilePath.h"
#include "NuxImage/Tga.h"
#include "NuxImage/ImageSurface.h"
#include "NuxImage/NITX.h"
#include "NTextureArchiveManager.h"

#include "NuxCore/TinyXML/tinyxml.h"

namespace nux
{

  bool IsUITextureArchive (const TCHAR *ArchiveFileName)
  {
    NSerializer *fileStream = GFileManager.CreateFileReader (ArchiveFileName, NSerializer::Read | NSerializer::Write);

    fileStream->Seek (0, NSerializer::SeekStart);

    unsigned int FileTag;
    fileStream->Serialize ( (char *) &FileTag, sizeof (FileTag) );
    fileStream->Close();

    if (FileTag != UITEXTUREARCHIVETAG)
    {
      return false;
    }

    return true;
  }

  NUITextureArchiver::NUITextureArchiver (const TCHAR *ArchiveFileName)
  {
    if (IsUITextureArchive (ArchiveFileName) )
      LoadUIArchive (ArchiveFileName, m_ArchiveTextureArray);
  }

  NUITextureArchiver::~NUITextureArchiver()
  {
    int num = m_ArchiveTextureArray.size();

    for (int n = 0; n < num; n++)
    {
      delete m_ArchiveTextureArray[n]->BaseTexture;
      delete m_ArchiveTextureArray[n];
    }

    m_ArchiveTextureArray.clear();
  }

  NBitmapData *NUITextureArchiver::ExtractTextureStyle (const TCHAR *Style)
  {
    int num = m_ArchiveTextureArray.size();

    for (int n = 0; n < num; n++)
    {
      if (Stricmp (m_ArchiveTextureArray[n]->Style.GetTCharPtr(), Style) == 0)
      {
        return m_ArchiveTextureArray[n]->BaseTexture;
      }
    }

    return 0;
  }

  NBitmapData *NUITextureArchiver::GetTexture (t_u32 index)
  {
    nuxAssert (index >= 0);
    nuxAssert (index < m_ArchiveTextureArray.size() );

    if ( (index < 0) || (index > m_ArchiveTextureArray.size() ) )
      return 0;

    return m_ArchiveTextureArray[index]->BaseTexture;
  }

  int NUITextureArchiver::GetNumTexture()
  {
    return m_ArchiveTextureArray.size();
  }

// XMLUIFile format:
//
//  <PaintData>
//      <Image Name="Stroke_10pxrad_2px.tga" type="Stroke" style="eSTROKE_CORNER_ROUND10" border_left="10" border_right="10" border_top="10" border_bottom="10" border_only="true" />
//      <Image Name="Stroke_4pxrad_2px.tga" type="Stroke" style="eSTROKE_CORNER_ROUND4" border_left="4" border_right="4" border_top="4" border_bottom="4" border_only="true" />
//  </PaintData>

  bool LoadXMLUITextures (const char *XMLUIFile, std::vector<ArchiveTextureData *>& ArchTexArray)
  {
    FilePath m_FilePath;
    m_FilePath.AddSearchPath (TEXT ("") ); // for case where fully qualified path is given
    m_FilePath.AddSearchPath (TEXT (".") );

#ifdef NUX_OS_LINUX
    m_FilePath.AddSearchPath (TEXT (PKGDATADIR"/UITextures") );
#else
    m_FilePath.AddSearchPath (TEXT ("./Data/UITextures") );
    m_FilePath.AddSearchPath (TEXT ("../Data/UITextures") );
    m_FilePath.AddSearchPath (TEXT ("../../Data/UITextures") );
#endif
    std::string file_search = XMLUIFile;
    NString painter_filename = m_FilePath.GetFile (file_search.c_str() );

    if (painter_filename == "")
    {
      printf (TEXT ("Unable to locate %s ...\n"), file_search.c_str() );
      return false;
    }

    TiXmlDocument doc (painter_filename.GetTCharPtr() );
    doc.LoadFile();
    TiXmlHandle docHandle ( &doc );
    TiXmlElement *data = docHandle.FirstChild ("PaintData").Element();
    TiXmlElement *image = 0;

    for (image = data->FirstChildElement ("Image"); image; image = image->NextSiblingElement ("Image") )
    {
      ArchiveTextureData *pimage = new ArchiveTextureData;
      Memset (pimage, 0, sizeof (ArchiveTextureData) );

      const char *StyleName = image->Attribute ("style");

      pimage->Style = StyleName;

      // If the attributes border_left, border_right, border_top, border_bottom are not present, assume they are equal to 0;
      pimage->border_left = pimage->border_right = pimage->border_top = pimage->border_bottom = 0;

      image->Attribute ("border_left", &pimage->border_left);
      image->Attribute ("border_right", &pimage->border_right);
      image->Attribute ("border_top",  &pimage->border_top);
      image->Attribute ("border_bottom", &pimage->border_bottom);


      const char *draw_borders_only = image->Attribute ("border_only");

      if (draw_borders_only == 0)
      {
        pimage->draw_borders_only = true;
      }
      else
      {
        if (strcmp ("false", draw_borders_only) == 0)
        {
          pimage->draw_borders_only = false;
        }
        else
        {
          pimage->draw_borders_only = true;
        }
      }

      const char *filename = image->Attribute ("Name");

      NString filepath = m_FilePath.GetFile (filename);

      pimage->SourceFile = filename;

      if (filepath == TEXT ("") )
      {
        pimage->BaseTexture = 0;
      }
      else
      {
        pimage->BaseTexture = LoadImageFile (filepath.GetTCharPtr() );
      }

      pimage->ArchiveOffset = 0;
      ArchTexArray.push_back (pimage);
    }

    return true;
  }

// Archive only 2D texture. Do not archive Volumes or Cubemaps
  bool CreateUITextureArchive (const TCHAR *XMLUIFile, const TCHAR *ArchiveName)
  {
    std::vector<ArchiveTextureData *> ArchTexArray;

    if (!LoadXMLUITextures (XMLUIFile, ArchTexArray) )
      return false;

    NSerializer *fileStream = GFileManager.CreateFileWriter (ArchiveName, NSerializer::Read | NSerializer::Write | NSerializer::OverWriteReadOnly);

    fileStream->Seek (0, NSerializer::SeekStart);

    t_s64 OFFSET = 0;
    int numtexture = ArchTexArray.size();

    unsigned int FileTag = UITEXTUREARCHIVETAG;  // inalogic user interface texture archive
    unsigned int FileVersion = UITEXTUREARCHIVEVERSION;  // inalogic user interface texture archive

    // write tag
    fileStream->Serialize ( (char *) &FileTag,     sizeof (FileTag) );
    OFFSET += sizeof (FileTag);
    fileStream->Serialize ( (char *) &FileVersion,     sizeof (FileVersion) );
    OFFSET += sizeof (FileVersion);

    // write num textures
    fileStream->Serialize ( (char *) &numtexture,     sizeof (numtexture) );
    OFFSET += sizeof (numtexture);

    for (int i = 0; i < numtexture; i++)
    {
      fileStream->Serialize ( (char *) TCHAR_TO_ANSI (ArchTexArray[i]->Style.GetTCharPtr() ), (t_u32) ArchTexArray[i]->Style.Length() + 1);
      OFFSET += ArchTexArray[i]->Style.Length() + 1;
      fileStream->Serialize ( (char *) TCHAR_TO_ANSI (ArchTexArray[i]->SourceFile.GetTCharPtr() ), (t_u32) ArchTexArray[i]->SourceFile.Length() + 1);
      OFFSET += ArchTexArray[i]->SourceFile.Length() + 1;

      fileStream->Serialize ( (char *) & (ArchTexArray[i]->border_left),     sizeof (ArchTexArray[i]->border_left) );
      OFFSET += sizeof (ArchTexArray[i]->border_left);
      fileStream->Serialize ( (char *) & (ArchTexArray[i]->border_right),     sizeof (ArchTexArray[i]->border_right) );
      OFFSET += sizeof (ArchTexArray[i]->border_right);
      fileStream->Serialize ( (char *) & (ArchTexArray[i]->border_top),     sizeof (ArchTexArray[i]->border_top) );
      OFFSET += sizeof (ArchTexArray[i]->border_top);
      fileStream->Serialize ( (char *) & (ArchTexArray[i]->border_bottom),     sizeof (ArchTexArray[i]->border_bottom) );
      OFFSET += sizeof (ArchTexArray[i]->border_bottom);

      fileStream->Serialize ( (char *) & (ArchTexArray[i]->ArchiveOffset), sizeof (ArchTexArray[i]->ArchiveOffset) );
      OFFSET += sizeof (ArchTexArray[i]->ArchiveOffset);
    }

    unsigned int NumArchived = 0;

    for (int i = 0; i < numtexture; i++)
    {
#ifdef NUX_OS_LINUX
      NFileName Filename (TEXT (PKGDATADIR"/UITextures") );
#else
      NFileName Filename (TEXT ("../../Data/UITextures/") );
#endif
      Filename += ArchTexArray[i]->SourceFile.GetTCharPtr();
      NBitmapData *BaseTexture = LoadImageFile (Filename.GetTCharPtr() );

      if (BaseTexture)
      {
        TextureArchiveAdd_ver_0_0_1 (fileStream, BaseTexture, ArchTexArray[i]->SourceFile.GetTCharPtr(), OFFSET);
        ++NumArchived;
        ArchTexArray[i]->ArchiveOffset = OFFSET;
      }
      else
      {
        ArchTexArray[i]->ArchiveOffset = 0xFFFFFFFF;
      }
    }

    // Second pass with the header where we write the offset to the texture data
    fileStream->Seek (sizeof (FileTag) + sizeof (FileVersion), NSerializer::SeekStart);
    // Write the true number of textures that has been archived.
    fileStream->Serialize ( (char *) &NumArchived,     sizeof (NumArchived) );

    for (int i = 0; i < numtexture; i++)
    {
      fileStream->Serialize ( (char *) TCHAR_TO_ANSI (ArchTexArray[i]->Style.GetTCharPtr() ), (t_u32) ArchTexArray[i]->Style.Length() + 1);
      fileStream->Serialize ( (char *) TCHAR_TO_ANSI (ArchTexArray[i]->SourceFile.GetTCharPtr() ), (t_u32) ArchTexArray[i]->SourceFile.Length() + 1);

      fileStream->Serialize ( (char *) & (ArchTexArray[i]->border_left),   sizeof (ArchTexArray[i]->border_left) );
      fileStream->Serialize ( (char *) & (ArchTexArray[i]->border_right),  sizeof (ArchTexArray[i]->border_right) );
      fileStream->Serialize ( (char *) & (ArchTexArray[i]->border_top),    sizeof (ArchTexArray[i]->border_top) );
      fileStream->Serialize ( (char *) & (ArchTexArray[i]->border_bottom), sizeof (ArchTexArray[i]->border_bottom) );

      int ddd = ArchTexArray[i]->ArchiveOffset;
      fileStream->Serialize ( (char *) & (ArchTexArray[i]->ArchiveOffset), sizeof (ArchTexArray[i]->ArchiveOffset) );
      ddd = fileStream->Tell();
    }

    fileStream->Close();
    return true;
  }

  bool LoadUIArchive (const TCHAR *ArchiveName, std::vector<ArchiveTextureData *>& ArchTexArray)
  {
    NSerializer *fileStream = GFileManager.CreateFileReader (TCHAR_TO_ANSI (ArchiveName), NSerializer::Read | NSerializer::Write);

    fileStream->Seek (0, NSerializer::SeekStart);

    int OFFSET = 0;
    int numtexture = 0;

    unsigned int FileTag;
    unsigned int FileVersion;
    fileStream->Serialize ( (char *) &FileTag, sizeof (FileTag) );
    fileStream->Serialize ( (char *) &FileVersion, sizeof (FileVersion) );

    if (FileTag != UITEXTUREARCHIVETAG)
    {
      nuxAssertMsg (0, TEXT ("[ReadArchive] File: %s is not a texture archive."), ArchiveName);
      return false;
    }

    fileStream->Serialize ( (char *) &numtexture, sizeof (numtexture) );
    OFFSET = fileStream->Tell();

    char CharBuffer[2048];

    for (int i = 0; i < numtexture; i++)
    {
      int c = 0;

      do
      {
        fileStream->Serialize (&CharBuffer[c], 1);
        c++;
      }
      while (CharBuffer[c-1] != 0);

      CharBuffer[c] = 0; // add null terminating char
      NString Style = CharBuffer;

      c = 0;

      do
      {
        fileStream->Serialize (&CharBuffer[c], 1);
        c++;
      }
      while (CharBuffer[c-1] != 0);

      CharBuffer[c] = 0; // add null terminating char
      NString SourceFile = CharBuffer;

      unsigned int border_left;
      unsigned int border_right;
      unsigned int border_top;
      unsigned int border_bottom;

      fileStream->Serialize ( (char *) &border_left,   sizeof (border_left) );
      fileStream->Serialize ( (char *) &border_right,  sizeof (border_right) );
      fileStream->Serialize ( (char *) &border_top,    sizeof (border_top) );
      fileStream->Serialize ( (char *) &border_bottom, sizeof (border_bottom) );


      unsigned int DataOffset;
      fileStream->Serialize ( (char *) &DataOffset, sizeof (DataOffset) );

      int CurrentPos = fileStream->Tell();

      //nuxDebugMsg(TEXT("%s - %s"), Style.GetTCharPtr(), SourceFile.GetTCharPtr());
      if (DataOffset == 0xFFFFFFFF)
      {
        nuxDebugMsg (TEXT ("[LoadUIArchive] Can't find file %s in UI archive %s. Skipping..."), SourceFile.GetTCharPtr(), ArchiveName);
      }

      NBitmapData *BaseTexture = TextureArchiveLoad_ver_0_0_1 (fileStream, DataOffset);

      ArchiveTextureData *pimage = new ArchiveTextureData;
      pimage->SourceFile = SourceFile;
      pimage->Style = Style;
      pimage->BaseTexture = BaseTexture;
      pimage->border_left = border_left;
      pimage->border_right = border_right;
      pimage->border_top = border_top;
      pimage->border_bottom = border_bottom;

      ArchTexArray.push_back (pimage);

      // Go back in the header section
      fileStream->Seek (CurrentPos, NSerializer::SeekStart);
    }

    fileStream->Close();
    return true;
  }

//NTextureArchiveManager ////////////////////////////////////////////

  NTextureArchiveManager::NTextureArchiveManager()
    :   m_isLoaded (false)
    ,   m_FileStream (0)
    ,   m_NumberOfTextures (0)
  {

  }

  NTextureArchiveManager::~NTextureArchiveManager()
  {
    if (m_FileStream)
    {
      m_FileStream->Close();
      delete m_FileStream;
    }
  }

  bool NTextureArchiveManager::CreateTextureArchiveFile (const TCHAR *TextureArchiveFilename)
  {
#define CHECK_ITX_WRITE_ERROR /*if(m_FileStream.bad()) {m_FileStream->close(); nuxAssertMsg(0, TEXT("[CreateTextureArchiveFile] Write Error.")); return false;}*/

    m_FileStream = GFileManager.CreateFileWriter (TCHAR_TO_ANSI (TextureArchiveFilename), NSerializer::Read | NSerializer::Write);

    if (!m_FileStream)
    {
      nuxAssertMsg (0, TEXT ("[CreateTextureArchiveFile] Can't open file: %s"), TextureArchiveFilename);
      m_isLoaded = false;
      return false;
    }

    m_FileStream->Seek (0, NSerializer::SeekStart);
    unsigned int ArchiveTag = TEXTUREARCHIVETAG;
    unsigned int ArchiveFormatVersion = TEXTUREARCHIVEVERSION;
    m_NumberOfTextures = 0;
    NTimeStamp TimeStamp;
    TimeStamp.GetTime();

    m_FileStream->Serialize ( (char *) &ArchiveTag,            sizeof (ArchiveTag) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &ArchiveFormatVersion,  sizeof (ArchiveFormatVersion) );
    CHECK_ITX_WRITE_ERROR;

    // Date Created
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    CHECK_ITX_WRITE_ERROR;

    // Date Modified
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    CHECK_ITX_WRITE_ERROR;
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    CHECK_ITX_WRITE_ERROR;

    // Number of Textures in the archives
    m_FileStream->Serialize ( (char *) &m_NumberOfTextures,        sizeof (m_NumberOfTextures) );
    CHECK_ITX_WRITE_ERROR;


    m_CurrentTextureArchive = TextureArchiveFilename;
    m_isLoaded = true;
#undef CHECK_ITX_WRITE_ERROR
    return true;
  }

  bool NTextureArchiveManager::AddTextureToArchive (const TCHAR *SourceFolder, const TCHAR *TextureFilename)
  {
    if (!m_FileStream)
    {
      nuxAssertMsg (0, TEXT ("[NTextureArchiveManager::AddTextureToArchive] Cannot add texture to archive. Load texture archive first.") );
      return false;
    }

    NFileName Filename;

    if ( (SourceFolder != 0) && (NString (SourceFolder) != NString (TEXT ("") ) ) )
    {
      Filename = SourceFolder;
      Filename.RemoveBackSlashAtEnd();
      Filename.RemoveSlashAtEnd();
      Filename.AddSlashAtEnd();
    }

    Filename += TextureFilename;
    NBitmapData *BaseTexture = LoadImageFile (Filename.GetTCharPtr() );

    if (BaseTexture == 0)
      return false;

    t_s64 Offset = 0;
    TextureArchiveAdd_ver_0_0_1 (m_FileStream, BaseTexture, TextureFilename, Offset);
    m_NumberOfTextures++;

    // Update the "last modified" date and the number of textures in the archive.
    m_FileStream->Seek (TEXTUREACHIVEHEADER_SIZE, NSerializer::SeekStart);

    NTimeStamp TimeStamp;
    TimeStamp.GetTime();

    // Date Modified
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );

    // Number of Textures in the archives
    m_FileStream->Serialize ( (char *) &m_NumberOfTextures,        sizeof (m_NumberOfTextures) );

    return true;
  }

  bool NTextureArchiveManager::LoadTextureArchiveFile (const TCHAR *Filename)
  {
    return true;
  }

  bool NTextureArchiveManager::CloseTextureArchiveFile()
  {
    if (m_FileStream)
      m_FileStream->Close();

    NUX_SAFE_DELETE (m_FileStream);
    m_CurrentTextureArchive = TEXT ("");
    m_isLoaded = false;
    return true;
  }

  bool NTextureArchiveManager::PointToFirstTexture()
  {
    nuxAssert (m_FileStream);

    if (m_FileStream == 0)
      return false;

    m_FileStream->Seek (0, NSerializer::SeekStart);
    unsigned int ArchiveTag = 0;
    unsigned int ArchiveFormatVersion = 0;
    //unsigned int NumTextures = 0;
    NTimeStamp TimeStamp;


    m_FileStream->Serialize ( (char *) &ArchiveTag,            sizeof (ArchiveTag) ); // should be 0x20787469
    m_FileStream->Serialize ( (char *) &ArchiveFormatVersion,  sizeof (ArchiveFormatVersion) );
    nuxDebugMsg (TEXT ("File Tag:         %c%c%c%c"),
                 (ArchiveTag & 0xFF),
                 (ArchiveTag & 0xFF00) >> 8,
                 (ArchiveTag & 0xFF0000) >> 16,
                 (ArchiveTag & 0xFF000000) >> 24);
    nuxDebugMsg (TEXT ("File Version:     %c.%c.%c.%c"),
                 (ArchiveFormatVersion & 0xFF),
                 (ArchiveFormatVersion & 0xFF00) >> 8,
                 (ArchiveFormatVersion & 0xFF0000) >> 16,
                 (ArchiveFormatVersion & 0xFF000000) >> 24);

    // Date Created
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    nuxDebugMsg (TEXT ("File created on:  %d/%d/%d at %d:%d:%d"), TimeStamp.m_Year, TimeStamp.m_Month, TimeStamp.m_Day, TimeStamp.m_Hour, TimeStamp.m_Minute, TimeStamp.m_Second);

    // Date Modified
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Year,   sizeof (TimeStamp.m_Year) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Month,  sizeof (TimeStamp.m_Month) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Day,    sizeof (TimeStamp.m_Day) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Hour,   sizeof (TimeStamp.m_Hour) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Minute, sizeof (TimeStamp.m_Minute) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_Second, sizeof (TimeStamp.m_Second) );
    m_FileStream->Serialize ( (char *) &TimeStamp.m_MicroSecond, sizeof (TimeStamp.m_MicroSecond) );
    nuxDebugMsg (TEXT ("Last modified on: %d/%d/%d at %d:%d:%d"), TimeStamp.m_Year, TimeStamp.m_Month, TimeStamp.m_Day, TimeStamp.m_Hour, TimeStamp.m_Minute, TimeStamp.m_Second);

    // Number of Textures in the archives
    m_FileStream->Serialize ( (char *) &m_NumberOfTextures,        sizeof (m_NumberOfTextures) );

    return true;
  }

  void NTextureArchiveManager::PrintArchiveContent (const TCHAR *TextureArchiveFilename)
  {
    if (m_FileStream)
      m_FileStream->Close();

    m_FileStream = GFileManager.CreateFileReader (TextureArchiveFilename, NSerializer::NoOverWrite | NSerializer::Read | NSerializer::Write);


    PointToFirstTexture();

    for (unsigned int i = 0; i < m_NumberOfTextures; i++)
    {
      TextureArchiveInfo_ver_0_0_1 TextureArchiveInfo;
      TextureArchiveGetInfo_ver_0_0_1 (m_FileStream, TextureArchiveInfo);
      nuxDebugMsg (TEXT ("File:     %s"), ANSI_TO_TCHAR (TextureArchiveInfo.Name.GetTCharPtr() ) );
      nuxDebugMsg (TEXT ("---------------------------------------------") );
      nuxDebugMsg (TEXT ("      Type:       %s"), (TextureArchiveInfo.Type == ARCH_TEXTURE2D) ? TEXT ("Texture2D") :
                   (TextureArchiveInfo.Type == ARCH_CUBEMAP) ? TEXT ("Cubemap Texture") :
                   (TextureArchiveInfo.Type == ARCH_VOLUME) ? TEXT ("Volume Texture") :
                   TEXT ("Unknown Texture")  );
      nuxDebugMsg (TEXT ("      Format:     %s"), GPixelFormats[TextureArchiveInfo.Format].Name);
      nuxDebugMsg (TEXT ("      Width:      %d"), TextureArchiveInfo.Width);
      nuxDebugMsg (TEXT ("      Height:     %d"), TextureArchiveInfo.Height);
      nuxDebugMsg (TEXT ("      Depth:      %d"), TextureArchiveInfo.Depth);
      nuxDebugMsg (TEXT ("      NumMipmap:  %d"), TextureArchiveInfo.NumMipmap);
      nuxDebugMsg (TEXT ("      Size:       %d"), TextureArchiveInfo.Size);
    }

    CloseTextureArchiveFile();
  }


  void NTextureArchiveManager::TraverseTextures (const TCHAR *TextureArchiveFilename)
  {
    if (m_FileStream)
      m_FileStream->Close();

    m_FileStream = GFileManager.CreateFileReader (TextureArchiveFilename, NSerializer::NoOverWrite | NSerializer::Read | NSerializer::Write);

    PointToFirstTexture();

    for (unsigned int i = 0; i < m_NumberOfTextures; i++)
    {
      unsigned int Offset = m_FileStream->Tell();
      TextureArchiveLoad_ver_0_0_1 (m_FileStream, Offset);
    }

    CloseTextureArchiveFile();
  }

  NBitmapData *NTextureArchiveManager::LoadBitmapArchive (const TCHAR *TextureArchiveFilename, const TCHAR *Filename)
  {
    if (m_FileStream)
      m_FileStream->Close();

    m_FileStream = GFileManager.CreateFileReader (TextureArchiveFilename, NSerializer::NoOverWrite | NSerializer::Read | NSerializer::Write);

    PointToFirstTexture();

    for (unsigned int i = 0; i < m_NumberOfTextures; i++)
    {
      TextureArchiveInfo_ver_0_0_1 TextureArchiveInfo;
      TextureArchiveGetInfo_ver_0_0_1 (m_FileStream, TextureArchiveInfo);

      if (Stricmp (Filename, TextureArchiveInfo.Name.GetTCharPtr() ) == 0)
      {
        return TextureArchiveLoad_ver_0_0_1 (m_FileStream, TextureArchiveInfo.FilePointer);
      }
    }

    CloseTextureArchiveFile();
    return 0;
  }


}
