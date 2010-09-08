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


#ifndef NTEXTUREARCHIVEMANAGER_H
#define NTEXTUREARCHIVEMANAGER_H


#include "NuxCore/FileManager/NSerializer.h"

NAMESPACE_BEGIN

struct ArchiveTextureData
{
    int border_left;
    int border_right;
    int border_top;
    int border_bottom;
    bool draw_borders_only;
    unsigned int ArchiveOffset;
    nux::NBitmapData* TextureData;
    NString SourceFile;
    NString Style;
};

class NUITextureArchiver
{
public:
    NUITextureArchiver(const TCHAR* ArchiveFileName);
    ~NUITextureArchiver();
    nux::NBitmapData* ExtractTextureStyle(const TCHAR* Style);
    nux::NBitmapData* GetTexture(t_u32 index);
    int GetNumTexture();

private:
    std::vector<ArchiveTextureData*> m_ArchiveTextureArray;
};

bool LoadXMLUITextures(const char* XMLUIFile, std::vector<ArchiveTextureData*>& ArchTexArray);
bool LoadUIArchive(const TCHAR* ArchiveName, std::vector<ArchiveTextureData*>& ArchTexArray);
bool CreateUITextureArchive(const TCHAR* XMLUIFile, const TCHAR* ArchiveName);

// typedef enum
// {
//     ARCH_UNKNOWN=0,
//     ARCH_TEXTURE2D=1,
//     ARCH_CUBEMAP=2,
//     ARCH_VOLUME=3,
// };

#define UITEXTUREARCHIVETAG NUX_MAKEFOURCHARTAG('u','i','t','a')
#define UITEXTUREARCHIVEVERSION NUX_MAKEFOURCHARTAG('0','0','0','1')


typedef struct  
{
    unsigned int ArchiveTag;
    unsigned int ArchiveFormatVersion;
// Creation date
    unsigned int Year;
    unsigned int Month;
    unsigned int Day;
    unsigned int Hour;
    unsigned int Min;
    unsigned int Sec;
    unsigned int MSec;
} TextureAchiveHeader;

typedef struct  
{
    // Last modification date
    unsigned int Year;
    unsigned int Month;
    unsigned int Day;
    unsigned int Hour;
    unsigned int Min;
    unsigned int Sec;
    unsigned int MSec;

    unsigned int NumerOfTextures;
} TextureAchiveInfoHeader;

const unsigned int TEXTUREACHIVEHEADER_SIZE     = 36;
const unsigned int TEXTUREACHIVEINFOHEADER_SIZE = 32;

class NTextureArchiveManager
{
public:
    NTextureArchiveManager();
    ~NTextureArchiveManager();

    bool CreateTextureArchiveFile(const TCHAR* Filename);
    bool LoadTextureArchiveFile(const TCHAR* Filename);
    bool CloseTextureArchiveFile();

    bool PointToFirstTexture();
    void TraverseTextures(const TCHAR* TextureArchiveFilename);
    nux::NBitmapData* LoadBitmapArchive(const TCHAR* TextureArchiveFilename, const TCHAR* Filename);



    bool AddTextureToArchive(const TCHAR* SourceFolder, const TCHAR* Filename);
    bool RemoveTextureFromArchive(const TCHAR* Filename);
    bool FindTextureInArchive(const TCHAR* Filename);

    void PrintArchiveContent(const TCHAR* TextureArchiveFilename);

    bool m_isLoaded;
    nux::NSerializer* m_FileStream;
    unsigned int m_NumberOfTextures;
    NString m_CurrentTextureArchive;
};

NAMESPACE_END

#endif // NTEXTUREARCHIVEMANAGER_H