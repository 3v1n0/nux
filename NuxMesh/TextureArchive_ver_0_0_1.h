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


// #ifndef TEXTUREARCHIVE_VER_0_0_1_H
// #define TEXTUREARCHIVE_VER_0_0_1_H
// 
// NAMESPACE_BEGIN
// 
// #pragma pack(1)
// typedef struct  
// {
//     nux::t_s64 FilePointer;
//     NFileName Name;
//     unsigned int Type;
//     unsigned int Format;
//     unsigned int Width;
//     unsigned int Height;
//     unsigned int Depth;
//     unsigned int NumMipmap;
//     unsigned int Size;
// 
// } TextureArchiveInfo_ver_0_0_1;
// #pragma pack()
// 
// bool TextureArchiveAdd_ver_0_0_1(nux::NSerializer* FileStream, const TCHAR* SourceFolder, const TCHAR* InputTextureFile, INL_OUT nux::t_s64& Offset);
// nux::NBitmapData* TextureArchiveLoad_ver_0_0_1(nux::NSerializer* FileStream, unsigned int Offset);
// void TextureArchiveGetInfo_ver_0_0_1(nux::NSerializer* FileStream, TextureArchiveInfo_ver_0_0_1& TextureArchiveInfo);
// 
// NAMESPACE_END
// 
// #endif // TEXTUREARCHIVE_VER_0_0_1_H