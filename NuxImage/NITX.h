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


#ifndef NITX_H
#define NITX_H

NAMESPACE_BEGIN

enum
{
    ARCH_UNKNOWN            = 0,
    ARCH_TEXTURE2D          = 1,
    ARCH_CUBEMAP            = 2,
    ARCH_VOLUME             = 3,
    ARCH_ANIMATEDTEXTURE    = 4,
};

#pragma pack(1)
typedef struct  
{
    t_s64 FilePointer;
    NFileName Name;
    unsigned int Type;
    unsigned int Format;
    unsigned int Width;
    unsigned int Height;
    unsigned int Depth;
    unsigned int NumMipmap;
    unsigned int Size;

} TextureArchiveInfo_ver_0_0_1;
#pragma pack()

bool TextureArchiveAdd_ver_0_0_1(NSerializer* FileStream, NBitmapData* BitmapData, const TCHAR* InputTextureFile, NUX_OUT t_s64& Offset);
NBitmapData* TextureArchiveLoad_ver_0_0_1(NSerializer* FileStream, unsigned int Offset);
void TextureArchiveGetInfo_ver_0_0_1(NSerializer* FileStream, TextureArchiveInfo_ver_0_0_1& TextureArchiveInfo);


#define TEXTUREARCHIVETAG NUX_MAKEFOURCHARTAG('i','t','x','t')
#define TEXTUREARCHIVEVERSION NUX_MAKEFOURCHARTAG('0','0','0','1')

#define NUX_ANIMATED_TEXTURE_TAG NUX_MAKEFOURCHARTAG('i','a','t', 'x')
#define NUX_ANIMATED_TEXTURE_VERSION NUX_MAKEFOURCHARTAG('0','0','0','1')


bool CreateITXFile(const TCHAR* InputTextureFile, const TCHAR* OutputTextureFile = 0);
NBitmapData* LoadITXFile(const TCHAR* ITXFilename);

NAMESPACE_END

#endif // NITX_H
