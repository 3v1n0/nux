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
#include "ImageSurface.h"

//#include "NTextureArchiveManager.h"
#include "NITX.h"

namespace nux
{

  bool TextureArchiveAdd_ver_0_0_1 (NSerializer *FileStream,  NBitmapData *BaseTexture, const TCHAR *InputTextureFile, NUX_OUT t_s64 &Offset)
  {
    nuxAssert (FileStream);
    nuxAssert (InputTextureFile);
    nuxAssert (BaseTexture);

    if (FileStream == 0 || InputTextureFile == 0 || BaseTexture == 0)
      return false;

//     NFileName Filename(SourceFolder);
//     if((SourceFolder != 0) && (SourceFolder != TEXT("")))
//     {
//         Filename = SourceFolder;
//         Filename.AddSlashAtEnd();
//     }
//     Filename += InputTextureFile;
//     NBitmapData *BaseTexture = LoadImageFile(Filename.GetTCharPtr());

//     if(!BaseTexture)
//         return false;

    if (!FileStream)
    {
      nuxAssertMsg (0, TEXT ("[TextureArchiveAdd_ver_0_0_1] Cannot add texture to archive. Archive stream is not opened.") );
      delete BaseTexture;
      return false;
    }

    // Set position at the end of the archive.
    Offset = FileStream->Seek ( 0, NSerializer::SeekEnd);

    char *TextureName = Strdup (TCHAR_TO_ANSI (InputTextureFile) );
    t_u32 TextureNameSize = (t_u32) strlen (TextureName) + 1;

    //  -------------------------------------------------------------------------------------------------------------------
    //  |Texture Name Size|Texture Name + (NULL character)|Offset to Next Texture|....BaseTexture....|Texture Name Size|....
    //  -------------------------------------------------------------------------------------------------------------------

    // Write the number of char (including the terminating 0) in the texture file name.
    FileStream->Serialize ( (char *) &TextureNameSize,         sizeof (TextureNameSize) );
    // Write the texture name including directory path
    FileStream->Serialize ( (char *) TextureName,         TextureNameSize);

    t_u32 TextureDataSize = 0;
    t_u32 TextureDataSizePosition = FileStream->Tell();
    FileStream->Serialize ( (char *) &TextureDataSize,         sizeof (TextureDataSize) );
    t_u32 TextureStartPosition = FileStream->Tell();

    if (BaseTexture->IsTextureData() )
    {
      NTextureData *Texture2DData = static_cast<NTextureData *> (BaseTexture);
      t_s32 type = ARCH_TEXTURE2D;
      t_s32 NumMip = Texture2DData->GetNumMipmap();
      t_s32 Width = Texture2DData->GetWidth();
      t_s32 Height = Texture2DData->GetHeight();
      t_s32 Format = Texture2DData->GetFormat();

      FileStream->Serialize ( (char *) &type,        sizeof (type) );
      FileStream->Serialize ( (char *) &Format,      sizeof (Format) );
      FileStream->Serialize ( (char *) &NumMip,      sizeof (NumMip) );
      FileStream->Serialize ( (char *) &Width,       sizeof (Width) );
      FileStream->Serialize ( (char *) &Height,      sizeof (Height) );

      for (t_s32 mip = 0; mip < NumMip; mip++)
      {
        t_s32 Pitch = Texture2DData->GetSurface (mip).GetPitch();
        t_s32 Size = Texture2DData->GetSurface (mip).GetSize();
        FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
        FileStream->Serialize ( (char *) &Size,     sizeof (Size) );
        FileStream->Serialize ( (char *) Texture2DData->GetSurface (mip).GetPtrRawData(),     Size);
      }
    }
    else if (BaseTexture->IsCubemapTextureData() )
    {
      NCubemapData *CubemapData = static_cast<NCubemapData *> (BaseTexture);
      t_s32 type = ARCH_CUBEMAP;
      t_s32 NumMip = CubemapData->GetNumMipmap();
      t_s32 Width = CubemapData->GetWidth();
      t_s32 Height = CubemapData->GetHeight();
      t_s32 Format = CubemapData->GetFormat();

      FileStream->Serialize ( (char *) &type,     sizeof (type) );
      FileStream->Serialize ( (char *) &Format,   sizeof (Format) );
      FileStream->Serialize ( (char *) &NumMip,   sizeof (NumMip) );
      FileStream->Serialize ( (char *) &Width,    sizeof (Width) );
      FileStream->Serialize ( (char *) &Height,   sizeof (Height) );

      for (t_s32 face = 0; face < 6; face++)
      {
        for (t_s32 mip = 0; mip < NumMip; mip++)
        {
          t_u32 Pitch = CubemapData->GetSurface (face, mip).GetPitch();
          t_u32 Size = CubemapData->GetSurface (face, mip).GetSize();
          FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
          FileStream->Serialize ( (char *) &Size,     sizeof (Size) );
          FileStream->Serialize ( (char *) CubemapData->GetSurface (face, mip).GetPtrRawData(),     Size);
        }
      }
    }
    else if (BaseTexture->IsVolumeTextureData() )
    {
      NVolumeData *VolumeData = static_cast<NVolumeData *> (BaseTexture);
      t_u32 type = ARCH_VOLUME;
      t_s32 NumMip = VolumeData->GetNumMipmap();
      t_s32 Width = VolumeData->GetWidth();
      t_s32 Height = VolumeData->GetHeight();
      t_s32 Depth = VolumeData->GetDepth();
      t_s32 Format = VolumeData->GetFormat();

      FileStream->Serialize ( (char *) &type,     sizeof (type) );
      FileStream->Serialize ( (char *) &Format,   sizeof (Format) );
      FileStream->Serialize ( (char *) &NumMip,   sizeof (NumMip) );
      FileStream->Serialize ( (char *) &Width,    sizeof (Width) );
      FileStream->Serialize ( (char *) &Height,   sizeof (Height) );
      FileStream->Serialize ( (char *) &Depth,    sizeof (Depth) );

      for (t_s32 mip = 0; mip < NumMip; mip++)
      {
        for (t_s32 s = 0; s < ImageSurface::GetLevelDim (BitmapFormat (Format), Depth, mip); s++)
        {
          t_s32 Pitch = VolumeData->GetSurface (mip, s).GetPitch();
          t_s32 Size = VolumeData->GetSurface (mip, s).GetSize();
          FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
          FileStream->Serialize ( (char *) &Size,     sizeof (Size) );
          FileStream->Serialize ( (char *) VolumeData->GetSurface (mip, s).GetPtrRawData(),     Size);
        }
      }
    }
    else if (BaseTexture->IsAnimatedTextureData() )
    {
      NAnimatedTextureData *AnimatedTextureData = static_cast<NAnimatedTextureData *> (BaseTexture);
      t_u32 type = ARCH_ANIMATEDTEXTURE;
      t_s32 NumMip = 1;;
      t_s32 Width = AnimatedTextureData->GetWidth();
      t_s32 Height = AnimatedTextureData->GetHeight();
      t_s32 Depth = AnimatedTextureData->GetDepth();
      t_s32 Format = AnimatedTextureData->GetFormat();

      FileStream->Serialize ( (char *) &type,     sizeof (type) );
      FileStream->Serialize ( (char *) &Format,   sizeof (Format) );
      FileStream->Serialize ( (char *) &NumMip,   sizeof (NumMip) );
      FileStream->Serialize ( (char *) &Width,    sizeof (Width) );
      FileStream->Serialize ( (char *) &Height,   sizeof (Height) );
      FileStream->Serialize ( (char *) &Depth,    sizeof (Depth) );

      for (t_s32 d = 0; d < Depth; d++)
      {
        t_s32 frame_time_ms = AnimatedTextureData->GetFrameTime (d);
        FileStream->Serialize ( (char *) &frame_time_ms, sizeof (t_u32) );
      }

      for (t_s32 mip = 0; mip < NumMip; mip++)
      {
        for (t_s32 s = 0; s < ImageSurface::GetLevelDim (BitmapFormat (Format), Depth, mip); s++)
        {
          t_u32 Pitch = AnimatedTextureData->GetSurface (s).GetPitch();
          t_u32 Size = AnimatedTextureData->GetSurface (s).GetSize();
          FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
          FileStream->Serialize ( (char *) &Size,     sizeof (Size) );
          FileStream->Serialize ( (char *) AnimatedTextureData->GetSurface (s).GetPtrRawData(),     Size);
        }
      }
    }
    else
    {
      nuxAssertMsg (0, TEXT ("[TextureArchiveAdd_ver_0_0_1] Texture type unknown.") );
    }

    t_u32 TextureEndPosition = FileStream->Tell();
    TextureDataSize = TextureEndPosition - TextureStartPosition;
    FileStream->Seek (TextureDataSizePosition, NSerializer::SeekStart);
    FileStream->Serialize ( (char *) &TextureDataSize, sizeof (TextureDataSize) );

    return true;
  }

  NBitmapData *TextureArchiveLoad_ver_0_0_1 (NSerializer *FileStream, t_u32 Offset)
  {
    nuxAssert (FileStream != 0);
    NBitmapData *BitmapData = 0;

    if (!FileStream)
    {
      nuxAssertMsg (0, TEXT ("[TextureArchiveAdd_ver_0_0_1] Cannot Load texture to archive. Invalid Archive stream.") );
      return 0;
    }

    // Set position at the end of the archive.
    FileStream->Seek (Offset, NSerializer::SeekStart);

    t_u32 TextureNameSize = 0;

    //  -------------------------------------------------------------------------------------------------------------------
    //  |Texture Name Size|Texture Name + (NULL character)|Offset to Next Texture|....BaseTexture....|Texture Name Size|....
    //  -------------------------------------------------------------------------------------------------------------------

    // Read the number of char (including the terminating 0) in the texture file name.
    FileStream->Serialize ( (char *) &TextureNameSize,         sizeof (TextureNameSize) );

    // Read the texture name including directory path
    char *TextureName = new char[TextureNameSize];
    FileStream->Serialize ( (char *) TextureName,         TextureNameSize);

    t_u32 TextureDataSize = 0;
    FileStream->Serialize ( (char *) &TextureDataSize,         sizeof (TextureDataSize) );

    t_u32 TextureDataPosition = FileStream->Tell();
    t_u32 TextureType;
    FileStream->Serialize ( (char *) &TextureType,     sizeof (TextureType) );

    if (TextureType == ARCH_TEXTURE2D)
    {
      t_s32 NumMip = 0;
      t_s32 Width = 0;
      t_s32 Height = 0;
      t_s32 Format = 0;

      FileStream->Serialize ( (char *) &Format,      sizeof (Format) );
      FileStream->Serialize ( (char *) &NumMip,      sizeof (NumMip) );
      FileStream->Serialize ( (char *) &Width,       sizeof (Width) );
      FileStream->Serialize ( (char *) &Height,      sizeof (Height) );

      BitmapData = new NTextureData ( (BitmapFormat) Format, Width, Height, NumMip);

      for (t_s32 mip = 0; mip < NumMip; mip++)
      {
        t_s32 Pitch = 0;;
        t_s32 Size = 0;
        FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
        FileStream->Serialize ( (char *) &Size,     sizeof (Size) );

        BYTE *PixelLineBuffer = new BYTE[Size];
        FileStream->Serialize ( (char *) PixelLineBuffer,   Size);

        BYTE	            *Dest        = BitmapData->GetSurface (mip).GetPtrRawData();
        BitmapFormat   Format      = BitmapData->GetSurface (mip).GetFormat();
        t_s32                 Width       = Align ( BitmapData->GetSurface (mip).GetWidth(),    GPixelFormats[Format].BlockSizeX );
        t_s32                 Height      = Align ( BitmapData->GetSurface (mip).GetHeight(),   GPixelFormats[Format].BlockSizeY );
        t_s32                 RowByteSize = Align ( (Width / GPixelFormats[Format].BlockSizeX) * GPixelFormats[Format].BlockBytes, GPixelFormats[Format].RowMemoryAlignment);;
        t_s32                 NumRows     = (Height / GPixelFormats[Format].BlockSizeY);

        for ( t_s32 Y = 0; Y < NumRows; Y++ )
        {
          Memcpy (Dest + Y * RowByteSize, &PixelLineBuffer[Y * Pitch], Min (Pitch, RowByteSize) );
        }
      }
    }
    else if (TextureType == ARCH_CUBEMAP)
    {
      t_s32 NumMip = 0;
      t_s32 Width = 0;
      t_s32 Height = 0;
      t_s32 Format = 0;

      FileStream->Serialize ( (char *) &Format,      sizeof (Format) );
      FileStream->Serialize ( (char *) &NumMip,      sizeof (NumMip) );
      FileStream->Serialize ( (char *) &Width,       sizeof (Width) );
      FileStream->Serialize ( (char *) &Height,      sizeof (Height) );

      BitmapData = new NCubemapData ( (BitmapFormat) Format, Width, Height, NumMip);

      for (t_s32 face = 0; face < 6; face++)
      {
        for (t_s32 mip = 0; mip < NumMip; mip++)
        {
          t_s32 Pitch = 0;;
          t_s32 Size = 0;
          FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
          FileStream->Serialize ( (char *) &Size,     sizeof (Size) );

          BYTE *PixelLineBuffer = new BYTE[Size];
          FileStream->Serialize ( (char *) PixelLineBuffer,   Size);

          BYTE	            *Dest        = BitmapData->GetSurface (face, mip).GetPtrRawData();
          BitmapFormat   Format      = BitmapData->GetSurface (face, mip).GetFormat();
          t_s32                 Width       = Align ( BitmapData->GetSurface (face, mip).GetWidth(),    GPixelFormats[Format].BlockSizeX );
          t_s32                 Height      = Align ( BitmapData->GetSurface (face, mip).GetHeight(),   GPixelFormats[Format].BlockSizeY );
          t_s32                 RowByteSize = Align ( (Width / GPixelFormats[Format].BlockSizeX) * GPixelFormats[Format].BlockBytes, GPixelFormats[Format].RowMemoryAlignment);;
          t_s32                 NumRows     = (Height / GPixelFormats[Format].BlockSizeY);

          for ( t_s32 Y = 0; Y < NumRows; Y++ )
          {
            Memcpy (Dest + Y * RowByteSize, &PixelLineBuffer[Y * Pitch], Min (Pitch, RowByteSize) );
          }
        }
      }
    }
    else if (TextureType == ARCH_VOLUME)
    {
      t_s32 NumMip = 0;
      t_s32 Width = 0;
      t_s32 Height = 0;
      t_s32 Format = 0;
      t_s32 Depth = 0;

      FileStream->Serialize ( (char *) &Format,      sizeof (Format) );
      FileStream->Serialize ( (char *) &NumMip,      sizeof (NumMip) );
      FileStream->Serialize ( (char *) &Width,       sizeof (Width) );
      FileStream->Serialize ( (char *) &Height,      sizeof (Height) );
      FileStream->Serialize ( (char *) &Depth,    sizeof (Depth) );

      BitmapData = new NVolumeData ( (BitmapFormat) Format, Width, Height, Depth, NumMip);

      for (t_s32 mip = 0; mip < NumMip; mip++)
      {
        for (t_s32 s = 0; s < ImageSurface::GetLevelDim (BitmapFormat (Format), Depth, mip); s++)
        {
          t_s32 Pitch = 0;;
          t_s32 Size = 0;
          FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
          FileStream->Serialize ( (char *) &Size,     sizeof (Size) );

          BYTE *PixelLineBuffer = new BYTE[Size];
          FileStream->Serialize ( (char *) PixelLineBuffer,   Size);

          BYTE	            *Dest        = BitmapData->GetSurface (mip, s).GetPtrRawData();
          BitmapFormat   Format      = BitmapData->GetSurface (mip, s).GetFormat();
          t_s32                 Width       = Align ( BitmapData->GetSurface (mip, s).GetWidth(),    GPixelFormats[Format].BlockSizeX );
          t_s32                 Height      = Align ( BitmapData->GetSurface (mip, s).GetHeight(),   GPixelFormats[Format].BlockSizeY );
          t_s32                 RowByteSize = Align ( (Width / GPixelFormats[Format].BlockSizeX) * GPixelFormats[Format].BlockBytes, GPixelFormats[Format].RowMemoryAlignment);;
          t_s32                 NumRows     = (Height / GPixelFormats[Format].BlockSizeY);

          for ( t_s32 Y = 0; Y < NumRows; Y++ )
          {
            Memcpy (Dest + Y * RowByteSize, &PixelLineBuffer[Y * Pitch], Min (Pitch, RowByteSize) );
          }
        }
      }
    }
    else if (TextureType == ARCH_ANIMATEDTEXTURE)
    {
      t_s32 NumMip = 0;
      t_s32 Width = 0;
      t_s32 Height = 0;
      t_s32 Format = 0;
      t_s32 Depth = 0;

      FileStream->Serialize ( (char *) &Format,      sizeof (Format) );
      FileStream->Serialize ( (char *) &NumMip,      sizeof (NumMip) );
      FileStream->Serialize ( (char *) &Width,       sizeof (Width) );
      FileStream->Serialize ( (char *) &Height,      sizeof (Height) );
      FileStream->Serialize ( (char *) &Depth,    sizeof (Depth) );

      nuxAssert (NumMip == 1);
      NAnimatedTextureData *AnimatedTextureData = new NAnimatedTextureData ( (BitmapFormat) Format, Width, Height, Depth);
      BitmapData = NUX_STATIC_CAST (NBitmapData *, AnimatedTextureData);

      for (t_s32 d = 0; d < Depth; d++)
      {
        t_s32 frame_time_ms;
        FileStream->Serialize ( (char *) &frame_time_ms,    sizeof (t_u32) );
        AnimatedTextureData->AddFrameTime (frame_time_ms);
      }

      for (t_s32 mip = 0; mip < NumMip; mip++)
      {
        for (t_s32 s = 0; s < ImageSurface::GetLevelDim (BitmapFormat (Format), Depth, mip); s++)
        {
          t_s32 Pitch = 0;;
          t_s32 Size = 0;
          FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
          FileStream->Serialize ( (char *) &Size,     sizeof (Size) );

          BYTE *PixelLineBuffer = new BYTE[Size];
          FileStream->Serialize ( (char *) PixelLineBuffer,   Size);

          BYTE	            *Dest        = BitmapData->GetSurface (s).GetPtrRawData();
          BitmapFormat   Format      = BitmapData->GetSurface (s).GetFormat();
          t_s32                 Width       = Align ( BitmapData->GetSurface (s).GetWidth(),    GPixelFormats[Format].BlockSizeX );
          t_s32                 Height      = Align ( BitmapData->GetSurface (s).GetHeight(),   GPixelFormats[Format].BlockSizeY );
          t_s32                 RowByteSize = Align ( (Width / GPixelFormats[Format].BlockSizeX) * GPixelFormats[Format].BlockBytes, GPixelFormats[Format].RowMemoryAlignment);;
          t_s32                 NumRows     = (Height / GPixelFormats[Format].BlockSizeY);

          for ( t_s32 Y = 0; Y < NumRows; Y++ )
          {
            Memcpy (Dest + Y * RowByteSize, &PixelLineBuffer[Y * Pitch], Min (Pitch, RowByteSize) );
          }
        }
      }
    }
    else
    {
      nuxAssertMsg (0, TEXT ("[TextureArchiveAdd_ver_0_0_1] Texture type unknown.") );
      return 0;
    }

    t_u32 TextureEndPosition = FileStream->Tell();

    if (TextureEndPosition != TextureDataPosition + TextureDataSize)
    {
      nuxAssertMsg (0, TEXT ("[TextureArchiveLoad_ver_0_0_1] Loading error.") );
    }

    return BitmapData;
  }

  void TextureArchiveGetInfo_ver_0_0_1 (NSerializer *FileStream, TextureArchiveInfo_ver_0_0_1 &TextureArchiveInfo)
  {
    Memset (&TextureArchiveInfo, 0, sizeof (TextureArchiveInfo_ver_0_0_1) );

    t_u32 texturenamesize = 0;
    char TextureFilename[1024];

    TextureArchiveInfo.FilePointer = FileStream->Tell();
    FileStream->Serialize ( (char *) &texturenamesize,     sizeof (texturenamesize) );
    FileStream->Serialize ( (char *) TextureFilename,     texturenamesize);
    TextureArchiveInfo.Name = NFileName (TextureFilename);
    t_u32 texturedatasize = 0;
    FileStream->Serialize ( (char *) &texturedatasize,     sizeof (texturedatasize) );
    //     // Jump to next texture position (or end of this texture).
    //     m_FileStream->Seek(texturedatasize, NSerializer::SeekCurrent);

    FileStream->Serialize ( (char *) &TextureArchiveInfo.Type, sizeof (TextureArchiveInfo.Type) );

    if (TextureArchiveInfo.Type == ARCH_TEXTURE2D)
    {
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Format, sizeof (TextureArchiveInfo.Format) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.NumMipmap, sizeof (TextureArchiveInfo.NumMipmap) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Width, sizeof (TextureArchiveInfo.Width) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Height, sizeof (TextureArchiveInfo.Height) );

      TextureArchiveInfo.Size = 0;

      for (t_s32 mip = 0; mip < TextureArchiveInfo.NumMipmap; mip++)
      {
        t_s32 Pitch = 0;
        t_s32 Size = 0;
        FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
        FileStream->Serialize ( (char *) &Size,     sizeof (Size) );
        nuxAssertMsg (Size == Pitch * ImageSurface::GetLevelBlockHeight (BitmapFormat (TextureArchiveInfo.Format), TextureArchiveInfo.Height, mip), TEXT ("[TextureArchiveGetInfo_ver_0_0_1] Level size is inconsistant.") );
        TextureArchiveInfo.Size += Size;
        // Jump to next texture mip.
        FileStream->Seek (Size, NSerializer::SeekCurrent);
      }
    }
    else if (TextureArchiveInfo.Type == ARCH_CUBEMAP)
    {
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Format, sizeof (TextureArchiveInfo.Format) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.NumMipmap, sizeof (TextureArchiveInfo.NumMipmap) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Width, sizeof (TextureArchiveInfo.Width) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Height, sizeof (TextureArchiveInfo.Height) );

      TextureArchiveInfo.Size = 0;

      for (t_s32 face = 0; face < 6; face++)
      {
        for (t_s32 mip = 0; mip < TextureArchiveInfo.NumMipmap; mip++)
        {
          t_s32 Pitch = 0;
          t_s32 Size = 0;
          FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
          FileStream->Serialize ( (char *) &Size,     sizeof (Size) );
          nuxAssertMsg (Size == Pitch * ImageSurface::GetLevelBlockHeight (BitmapFormat (TextureArchiveInfo.Format), TextureArchiveInfo.Height, mip), TEXT ("[TextureArchiveGetInfo_ver_0_0_1] Level size is inconsistant.") );
          TextureArchiveInfo.Size += Size;
          // Jump to next texture mip in the face.
          FileStream->Seek (Size, NSerializer::SeekCurrent);
        }
      }
    }
    else if (TextureArchiveInfo.Type == ARCH_VOLUME)
    {
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Format, sizeof (TextureArchiveInfo.Format) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.NumMipmap, sizeof (TextureArchiveInfo.NumMipmap) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Width, sizeof (TextureArchiveInfo.Width) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Height, sizeof (TextureArchiveInfo.Height) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Depth, sizeof (TextureArchiveInfo.Depth) );

      TextureArchiveInfo.Size = 0;

      for (t_s32 mip = 0; mip < TextureArchiveInfo.NumMipmap; mip++)
      {
        for (t_s32 s = 0; s < ImageSurface::GetLevelDim (BitmapFormat (TextureArchiveInfo.Format), TextureArchiveInfo.Depth, mip); s++)
        {
          t_s32 Pitch = 0;
          t_s32 Size = 0;
          FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
          FileStream->Serialize ( (char *) &Size,     sizeof (Size) );
          nuxAssertMsg (Size == Pitch * ImageSurface::GetLevelBlockHeight (BitmapFormat (TextureArchiveInfo.Format), TextureArchiveInfo.Height, mip), TEXT ("[TextureArchiveGetInfo_ver_0_0_1] Level size is inconsistant.") );
          TextureArchiveInfo.Size += Size;
          // Jump to next texture depth.
          FileStream->Seek (Size, NSerializer::SeekCurrent);
        }
      }
    }
    else if (TextureArchiveInfo.Type == ARCH_ANIMATEDTEXTURE)
    {
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Format, sizeof (TextureArchiveInfo.Format) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.NumMipmap, sizeof (TextureArchiveInfo.NumMipmap) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Width, sizeof (TextureArchiveInfo.Width) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Height, sizeof (TextureArchiveInfo.Height) );
      FileStream->Serialize ( (char *) &TextureArchiveInfo.Depth, sizeof (TextureArchiveInfo.Depth) );

      for (t_s32 d = 0; d < TextureArchiveInfo.Depth; d++)
      {
        t_s32 frame_time_ms;
        FileStream->Serialize ( (char *) &frame_time_ms,    sizeof (t_u32) );
      }

      TextureArchiveInfo.Size = 0;

      for (t_s32 mip = 0; mip < TextureArchiveInfo.NumMipmap; mip++)
      {
        for (t_s32 s = 0; s < ImageSurface::GetLevelDim (BitmapFormat (TextureArchiveInfo.Format), TextureArchiveInfo.Depth, mip); s++)
        {
          t_s32 Pitch = 0;
          t_s32 Size = 0;
          FileStream->Serialize ( (char *) &Pitch,    sizeof (Pitch) );
          FileStream->Serialize ( (char *) &Size,     sizeof (Size) );
          nuxAssertMsg (Size == Pitch * ImageSurface::GetLevelBlockHeight (BitmapFormat (TextureArchiveInfo.Format), TextureArchiveInfo.Height, mip), TEXT ("[TextureArchiveGetInfo_ver_0_0_1] Level size is inconsistant.") );
          TextureArchiveInfo.Size += Size;
          // Jump to next texture depth.
          FileStream->Seek (Size, NSerializer::SeekCurrent);
        }
      }
    }
  }


//////////////////

  bool CreateITXFile (const TCHAR *InputTextureFile, const TCHAR *OutputTextureFile)
  {
    nuxAssert (InputTextureFile);

    if (InputTextureFile == 0)
      return false;

    NFileName Filename = InputTextureFile;
    NBitmapData *BaseTexture = LoadImageFile (Filename.GetTCharPtr() );

    if (BaseTexture == 0)
      return false;

    NFileName OutputFilename;

    if ( (OutputTextureFile == 0) || (NString (OutputTextureFile) == NString (TEXT ("") ) ) )
    {
      OutputFilename = InputTextureFile;
      OutputFilename.ChangeFileExtension (TEXT ("itx") );
    }
    else
      OutputFilename = OutputTextureFile;

    NSerializer *fileStream = GFileManager.CreateFileWriter (OutputFilename.GetTCharPtr(), NSerializer::Read | NSerializer::Write);
    fileStream->Seek (0, NSerializer::SeekStart);
    t_s64 Offset = 0;

    t_u32 FileTag = TEXTUREARCHIVETAG;
    t_u32 FileVersion = TEXTUREARCHIVEVERSION;

    fileStream->Serialize ( (char *) &FileTag,     sizeof (FileTag) );;
    fileStream->Serialize ( (char *) &FileVersion,     sizeof (FileVersion) );

    TextureArchiveAdd_ver_0_0_1 (fileStream, BaseTexture, InputTextureFile, Offset);

    fileStream->Close();
    return true;
  }

  NBitmapData *LoadITXFile (const TCHAR *ITXFilename)
  {
    nuxAssert (ITXFilename);

    if (ITXFilename == 0)
      return false;

    NSerializer *fileStream = GFileManager.CreateFileReader (ITXFilename, NSerializer::Read | NSerializer::Write);
    fileStream->Seek (0, NSerializer::SeekStart);
    t_s64 Offset = 0;

    t_u32 FileTag;
    t_u32 FileVersion;
    fileStream->Serialize ( (char *) &FileTag, sizeof (FileTag) );
    fileStream->Serialize ( (char *) &FileVersion, sizeof (FileVersion) );

    if (FileTag != TEXTUREARCHIVETAG)
    {
      //nuxAssertMsg(0, TEXT("[LoadITXFile] File: %s format is incorrect."), ITXFilename);
      return 0;
    }

    Offset = fileStream->Tell();
    NBitmapData *data = TextureArchiveLoad_ver_0_0_1 (fileStream, Offset);

    fileStream->Close();
    return data;
  }

}

