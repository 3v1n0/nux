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


#include "NuxCore/NuxCore.h"
#include "NuxCore/FilePath.h"
#include "NuxCore/TinyXML/tinyxml.h"

#include "ImageSurface.h"
#include "NITX.h"
#include "NAnimatedTexture.h"

namespace nux
{

// XMLUIFile format:
//
//  <AnimatedTexture>
//      <frame Texture="0_macthrob2.png" time="41"/>
//      <frame Texture="1_macthrob2.png" time="41"/>
//  </PaintData>

  NBitmapData *LoadAnimatedTextureFromXML(const char *XMLFile)
  {
    FilePath m_FilePath;
    m_FilePath.AddSearchPath(""); // for case where fully qualified path is given
    m_FilePath.AddSearchPath(".");
    m_FilePath.AddSearchPath("./Data/AnimatedTextures");
    m_FilePath.AddSearchPath("../Data/AnimatedTextures");
    m_FilePath.AddSearchPath("../../Data/AnimatedTextures");

    std::string file_search = XMLFile;
    NString painter_filename = m_FilePath.GetFile(file_search.c_str());

    if (painter_filename == "")
    {
      printf("Unable to locate %s ...\n", file_search.c_str());
      return false;
    }

    TiXmlDocument doc(painter_filename.GetTCharPtr());
    doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *data = docHandle.FirstChild("AnimatedTexture").Element();

//     int frame_width, frame_height, num_frame;
//     data->Attribute("width", &frame_width);
//     data->Attribute("width", &frame_height);
//     data->Attribute("frame", &num_frame);

    std::vector<int> FrameTimeArray;
    std::vector<NBitmapData *> FrameArray;

    TiXmlElement *image = 0;

    for (image = data->FirstChildElement("frame"); image; image = image->NextSiblingElement("frame"))
    {
      int time_ms = 0;
      image->Attribute("time", &time_ms);

      const char *filename = image->Attribute("Texture");
      NString filepath = m_FilePath.GetFile(filename);

      if (filepath != "")
      {
        NBitmapData *pFrame = LoadImageFile(filepath.GetTCharPtr());

        if (pFrame)
        {
          FrameArray.push_back(pFrame);
          FrameTimeArray.push_back(time_ms);
        }
      }
    }

    NAnimatedTextureData *pAnimatedTexture = 0;
    pAnimatedTexture = new NAnimatedTextureData(FrameArray[0]->GetFormat(), FrameArray[0]->GetWidth(), FrameArray[0]->GetHeight(), FrameArray.size());

    for (unsigned int i = 0; i < FrameArray.size(); i++)
    {
      Memcpy(pAnimatedTexture->GetSurface(i).GetPtrRawData(),
              FrameArray[i]->GetSurface(0).GetPtrRawData(),
              FrameArray[i]->GetSurface(0).GetSize());
      pAnimatedTexture->AddFrameTime(FrameTimeArray[i]);
    }

    for (unsigned int i = 0; i < FrameArray.size(); i++)
    {
      delete FrameArray[i];
    }

    FrameArray.clear();
    FrameTimeArray.clear();

    return pAnimatedTexture;
  }

  bool SaveAnimatedTextureFile(NBitmapData *pAnimatedTexture, const TCHAR *Filename)
  {
    if (pAnimatedTexture == 0 || !pAnimatedTexture->IsAnimatedTextureData())
      return false;

    NSerializer *fileStream = GFileManager.CreateFileWriter(Filename, NSerializer::Read | NSerializer::Write);

    if (fileStream == 0)
      return false;

    fileStream->Seek(0, NSerializer::SeekStart);
    long long Offset = 0;

    unsigned int FileTag = NUX_ANIMATED_TEXTURE_TAG;
    unsigned int FileVersion = NUX_ANIMATED_TEXTURE_VERSION;

    fileStream->Serialize((char *) &FileTag,     sizeof(FileTag));;
    fileStream->Serialize((char *) &FileVersion,     sizeof(FileVersion));

    TextureArchiveAdd_ver_0_0_1(fileStream, pAnimatedTexture, "TestAnimatedTexture.iatx", Offset);

    fileStream->Close();

    return true;
  }

  NBitmapData *LoadAnimatedTextureFile(const TCHAR *Filename)
  {
    NSerializer *fileStream = GFileManager.CreateFileReader(Filename, NSerializer::Read | NSerializer::Write);

    if (fileStream == 0)
      return false;

    fileStream->Seek(0, NSerializer::SeekStart);
    long long Offset = 0;

    unsigned int FileTag;
    unsigned int FileVersion;

    fileStream->Serialize((char *) &FileTag,     sizeof(FileTag));;
    fileStream->Serialize((char *) &FileVersion,     sizeof(FileVersion));

    if (FileTag != NUX_ANIMATED_TEXTURE_TAG)
    {
      //nuxAssertMsg(0, "[LoadAnimatedTextureFile] File: %s format is incorrect.", Filename);
      return 0;
    }

    Offset = fileStream->Tell();
    NBitmapData *pAnimatedTexture = TextureArchiveLoad_ver_0_0_1(fileStream, Offset);

    fileStream->Close();
    return pAnimatedTexture;
  }


//
// bool LoadUIArchive(const TCHAR* ArchiveName, NArray<ArchiveTextureData*>& ArchTexArray)
// {
//     NSerializer* fileStream = GFileManager.CreateFileReader(ArchiveName, NSerializer::Read|NSerializer::Write);
//
//     fileStream->Seek(0, NSerializer::SeekStart);
//
//     int OFFSET = 0;
//     int numtexture = 0;
//
//     unsigned int FileTag;
//     unsigned int FileVersion;
//     fileStream->Serialize((char*) &FileTag, sizeof(FileTag));
//     fileStream->Serialize((char*) &FileVersion, sizeof(FileVersion));
//
//     if (FileTag != UITEXTUREARCHIVETAG)
//     {
//         nuxAssertMsg(0, "[ReadArchive] File: %s is not a texture archive.", ArchiveName);
//         return false;
//     }
//
//     fileStream->Serialize((char*) &numtexture, sizeof(numtexture));
//     OFFSET = fileStream->Tell();
//
//     char CharBuffer[2048];
//     for (int i = 0; i < numtexture; i++)
//     {
//         int c = 0;
//         do {fileStream->Serialize(&CharBuffer[c], 1); c++;} while (CharBuffer[c-1] != 0);
//         CharBuffer[c] = 0; // add null terminating char
//         NString Style = CharBuffer;
//
//         c = 0;
//         do {fileStream->Serialize(&CharBuffer[c], 1); c++;} while (CharBuffer[c-1] != 0);
//         CharBuffer[c] = 0; // add null terminating char
//         NString SourceFile = CharBuffer;
//
//         unsigned int border_left;
//         unsigned int border_right;
//         unsigned int border_top;
//         unsigned int border_bottom;
//
//         fileStream->Serialize((char*) &border_left,   sizeof(border_left));
//         fileStream->Serialize((char*) &border_right,  sizeof(border_right));
//         fileStream->Serialize((char*) &border_top,    sizeof(border_top));
//         fileStream->Serialize((char*) &border_bottom, sizeof(border_bottom));
//
//
//         unsigned int DataOffset;
//         fileStream->Serialize((char*) &DataOffset, sizeof(DataOffset));
//
//         int CurrentPos = fileStream->Tell();
//
//         //nuxDebugMsg("%s - %s", Style.GetTCharPtr(), SourceFile.GetTCharPtr());
//         NBitmapData* BaseTexture = TextureArchiveLoad_ver_0_0_1(fileStream, DataOffset);
//
//         ArchiveTextureData* pimage = new ArchiveTextureData;
//         pimage->SourceFile = SourceFile;
//         pimage->Style = Style;
//         pimage->BaseTexture = BaseTexture;
//         pimage->border_left = border_left;
//         pimage->border_right = border_right;
//         pimage->border_top = border_top;
//         pimage->border_bottom = border_bottom;
//
//         ArchTexArray.AddElement(pimage);
//
//         // Go back in the header section
//         fileStream->Seek(CurrentPos, NSerializer::SeekStart);
//     }
//
//     fileStream->Close();
//     return true;
// }

}
