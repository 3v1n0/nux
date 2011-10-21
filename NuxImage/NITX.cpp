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
#include "ImageSurface.h"

//#include "NTextureArchiveManager.h"
#include "NITX.h"

namespace nux
{

  bool TextureArchiveAdd_ver_0_0_1(NSerializer *FileStream,  NBitmapData *BaseTexture, const TCHAR *InputTextureFile, NUX_OUT long long &Offset)
  {
    nuxAssert(FileStream);
    nuxAssert(InputTextureFile);
    nuxAssert(BaseTexture);

    if (FileStream == 0 || InputTextureFile == 0 || BaseTexture == 0)
      return false;

//     NFileName Filename(SourceFolder);
//     if ((SourceFolder != 0) && (SourceFolder != ""))
//     {
//         Filename = SourceFolder;
//         Filename.AddSlashAtEnd();
//     }
//     Filename += InputTextureFile;
//     NBitmapData *BaseTexture = LoadImageFile(Filename.GetTCharPtr());

//     if (!BaseTexture)
//         return false;

    if (!FileStream)
    {
      nuxAssertMsg(0, "[TextureArchiveAdd_ver_0_0_1] Cannot add texture to archive. Archive stream is not opened.");
      delete BaseTexture;
      return false;
    }

    // Set position at the end of the archive.
    Offset = FileStream->Seek( 0, NSerializer::SeekEnd);

    char *TextureName = Strdup(TCHAR_TO_ANSI(InputTextureFile));
    unsigned int TextureNameSize = (unsigned int) strlen(TextureName) + 1;

    //  -------------------------------------------------------------------------------------------------------------------
    //  |Texture Name Size|Texture Name + (NULL character)|Offset to Next Texture|....BaseTexture....|Texture Name Size|....
    //  -------------------------------------------------------------------------------------------------------------------

    // Write the number of char(including the terminating 0) in the texture file name.
    FileStream->Serialize((char *) &TextureNameSize,         sizeof(TextureNameSize));
    // Write the texture name including directory path
    FileStream->Serialize((char *) TextureName,         TextureNameSize);

    unsigned int TextureDataSize = 0;
    unsigned int TextureDataSizePosition = FileStream->Tell();
    FileStream->Serialize((char *) &TextureDataSize,         sizeof(TextureDataSize));
    unsigned int TextureStartPosition = FileStream->Tell();

    if (BaseTexture->IsTextureData())
    {
      NTextureData *Texture2DData = static_cast<NTextureData *> (BaseTexture);
      int type = ARCH_TEXTURE2D;
      int NumMip = Texture2DData->GetNumMipmap();
      int Width = Texture2DData->GetWidth();
      int Height = Texture2DData->GetHeight();
      int Format = Texture2DData->GetFormat();

      FileStream->Serialize((char *) &type,        sizeof(type));
      FileStream->Serialize((char *) &Format,      sizeof(Format));
      FileStream->Serialize((char *) &NumMip,      sizeof(NumMip));
      FileStream->Serialize((char *) &Width,       sizeof(Width));
      FileStream->Serialize((char *) &Height,      sizeof(Height));

      for (int mip = 0; mip < NumMip; mip++)
      {
        int Pitch = Texture2DData->GetSurface(mip).GetPitch();
        int Size = Texture2DData->GetSurface(mip).GetSize();
        FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
        FileStream->Serialize((char *) &Size,     sizeof(Size));
        FileStream->Serialize((char *) Texture2DData->GetSurface(mip).GetPtrRawData(),     Size);
      }
    }
    else if (BaseTexture->IsCubemapTextureData())
    {
      NCubemapData *CubemapData = static_cast<NCubemapData *> (BaseTexture);
      int type = ARCH_CUBEMAP;
      int NumMip = CubemapData->GetNumMipmap();
      int Width = CubemapData->GetWidth();
      int Height = CubemapData->GetHeight();
      int Format = CubemapData->GetFormat();

      FileStream->Serialize((char *) &type,     sizeof(type));
      FileStream->Serialize((char *) &Format,   sizeof(Format));
      FileStream->Serialize((char *) &NumMip,   sizeof(NumMip));
      FileStream->Serialize((char *) &Width,    sizeof(Width));
      FileStream->Serialize((char *) &Height,   sizeof(Height));

      for (int face = 0; face < 6; face++)
      {
        for (int mip = 0; mip < NumMip; mip++)
        {
          unsigned int Pitch = CubemapData->GetSurface(face, mip).GetPitch();
          unsigned int Size = CubemapData->GetSurface(face, mip).GetSize();
          FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
          FileStream->Serialize((char *) &Size,     sizeof(Size));
          FileStream->Serialize((char *) CubemapData->GetSurface(face, mip).GetPtrRawData(),     Size);
        }
      }
    }
    else if (BaseTexture->IsVolumeTextureData())
    {
      NVolumeData *VolumeData = static_cast<NVolumeData *> (BaseTexture);
      unsigned int type = ARCH_VOLUME;
      int NumMip = VolumeData->GetNumMipmap();
      int Width = VolumeData->GetWidth();
      int Height = VolumeData->GetHeight();
      int Depth = VolumeData->GetDepth();
      int Format = VolumeData->GetFormat();

      FileStream->Serialize((char *) &type,     sizeof(type));
      FileStream->Serialize((char *) &Format,   sizeof(Format));
      FileStream->Serialize((char *) &NumMip,   sizeof(NumMip));
      FileStream->Serialize((char *) &Width,    sizeof(Width));
      FileStream->Serialize((char *) &Height,   sizeof(Height));
      FileStream->Serialize((char *) &Depth,    sizeof(Depth));

      for (int mip = 0; mip < NumMip; mip++)
      {
        for (int s = 0; s < ImageSurface::GetLevelDim(BitmapFormat(Format), Depth, mip); s++)
        {
          int Pitch = VolumeData->GetSurface(mip, s).GetPitch();
          int Size = VolumeData->GetSurface(mip, s).GetSize();
          FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
          FileStream->Serialize((char *) &Size,     sizeof(Size));
          FileStream->Serialize((char *) VolumeData->GetSurface(mip, s).GetPtrRawData(),     Size);
        }
      }
    }
    else if (BaseTexture->IsAnimatedTextureData())
    {
      NAnimatedTextureData *AnimatedTextureData = static_cast<NAnimatedTextureData *> (BaseTexture);
      unsigned int type = ARCH_ANIMATEDTEXTURE;
      int NumMip = 1;;
      int Width = AnimatedTextureData->GetWidth();
      int Height = AnimatedTextureData->GetHeight();
      int Depth = AnimatedTextureData->GetDepth();
      int Format = AnimatedTextureData->GetFormat();

      FileStream->Serialize((char *) &type,     sizeof(type));
      FileStream->Serialize((char *) &Format,   sizeof(Format));
      FileStream->Serialize((char *) &NumMip,   sizeof(NumMip));
      FileStream->Serialize((char *) &Width,    sizeof(Width));
      FileStream->Serialize((char *) &Height,   sizeof(Height));
      FileStream->Serialize((char *) &Depth,    sizeof(Depth));

      for (int d = 0; d < Depth; d++)
      {
        int frame_time_ms = AnimatedTextureData->GetFrameTime(d);
        FileStream->Serialize((char *) &frame_time_ms, sizeof(unsigned int));
      }

      for (int mip = 0; mip < NumMip; mip++)
      {
        for (int s = 0; s < ImageSurface::GetLevelDim(BitmapFormat(Format), Depth, mip); s++)
        {
          unsigned int Pitch = AnimatedTextureData->GetSurface(s).GetPitch();
          unsigned int Size = AnimatedTextureData->GetSurface(s).GetSize();
          FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
          FileStream->Serialize((char *) &Size,     sizeof(Size));
          FileStream->Serialize((char *) AnimatedTextureData->GetSurface(s).GetPtrRawData(),     Size);
        }
      }
    }
    else
    {
      nuxAssertMsg(0, "[TextureArchiveAdd_ver_0_0_1] Texture type unknown.");
    }

    unsigned int TextureEndPosition = FileStream->Tell();
    TextureDataSize = TextureEndPosition - TextureStartPosition;
    FileStream->Seek(TextureDataSizePosition, NSerializer::SeekStart);
    FileStream->Serialize((char *) &TextureDataSize, sizeof(TextureDataSize));

    return true;
  }

  NBitmapData *TextureArchiveLoad_ver_0_0_1(NSerializer *FileStream, unsigned int Offset)
  {
    nuxAssert(FileStream != 0);
    NBitmapData *BitmapData = 0;

    if (!FileStream)
    {
      nuxAssertMsg(0, "[TextureArchiveAdd_ver_0_0_1] Cannot Load texture to archive. Invalid Archive stream.");
      return 0;
    }

    // Set position at the end of the archive.
    FileStream->Seek(Offset, NSerializer::SeekStart);

    unsigned int TextureNameSize = 0;

    //  -------------------------------------------------------------------------------------------------------------------
    //  |Texture Name Size|Texture Name + (NULL character)|Offset to Next Texture|....BaseTexture....|Texture Name Size|....
    //  -------------------------------------------------------------------------------------------------------------------

    // Read the number of char(including the terminating 0) in the texture file name.
    FileStream->Serialize((char *) &TextureNameSize,         sizeof(TextureNameSize));

    // Read the texture name including directory path
    char *TextureName = new char[TextureNameSize];
    FileStream->Serialize((char *) TextureName,         TextureNameSize);

    unsigned int TextureDataSize = 0;
    FileStream->Serialize((char *) &TextureDataSize,         sizeof(TextureDataSize));

    unsigned int TextureDataPosition = FileStream->Tell();
    unsigned int TextureType;
    FileStream->Serialize((char *) &TextureType,     sizeof(TextureType));

    if (TextureType == ARCH_TEXTURE2D)
    {
      int NumMip = 0;
      int Width = 0;
      int Height = 0;
      int Format = 0;

      FileStream->Serialize((char *) &Format,      sizeof(Format));
      FileStream->Serialize((char *) &NumMip,      sizeof(NumMip));
      FileStream->Serialize((char *) &Width,       sizeof(Width));
      FileStream->Serialize((char *) &Height,      sizeof(Height));

      BitmapData = new NTextureData((BitmapFormat) Format, Width, Height, NumMip);

      for (int mip = 0; mip < NumMip; mip++)
      {
        int Pitch = 0;;
        int Size = 0;
        FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
        FileStream->Serialize((char *) &Size,     sizeof(Size));

        BYTE *PixelLineBuffer = new BYTE[Size];
        FileStream->Serialize((char *) PixelLineBuffer,   Size);

        BYTE	            *Dest        = BitmapData->GetSurface(mip).GetPtrRawData();
        BitmapFormat   Format      = BitmapData->GetSurface(mip).GetFormat();
        int                 Width       = Align( BitmapData->GetSurface(mip).GetWidth(),    GPixelFormats[Format].BlockSizeX );
        int                 Height      = Align( BitmapData->GetSurface(mip).GetHeight(),   GPixelFormats[Format].BlockSizeY );
        int                 RowByteSize = Align((Width / GPixelFormats[Format].BlockSizeX) * GPixelFormats[Format].BlockBytes, GPixelFormats[Format].RowMemoryAlignment);;
        int                 NumRows     = (Height / GPixelFormats[Format].BlockSizeY);

        for ( int Y = 0; Y < NumRows; Y++ )
        {
          Memcpy(Dest + Y * RowByteSize, &PixelLineBuffer[Y * Pitch], Min(Pitch, RowByteSize));
        }
      }
    }
    else if (TextureType == ARCH_CUBEMAP)
    {
      int NumMip = 0;
      int Width = 0;
      int Height = 0;
      int Format = 0;

      FileStream->Serialize((char *) &Format,      sizeof(Format));
      FileStream->Serialize((char *) &NumMip,      sizeof(NumMip));
      FileStream->Serialize((char *) &Width,       sizeof(Width));
      FileStream->Serialize((char *) &Height,      sizeof(Height));

      BitmapData = new NCubemapData((BitmapFormat) Format, Width, Height, NumMip);

      for (int face = 0; face < 6; face++)
      {
        for (int mip = 0; mip < NumMip; mip++)
        {
          int Pitch = 0;;
          int Size = 0;
          FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
          FileStream->Serialize((char *) &Size,     sizeof(Size));

          BYTE *PixelLineBuffer = new BYTE[Size];
          FileStream->Serialize((char *) PixelLineBuffer,   Size);

          BYTE	            *Dest        = BitmapData->GetSurface(face, mip).GetPtrRawData();
          BitmapFormat   Format      = BitmapData->GetSurface(face, mip).GetFormat();
          int                 Width       = Align( BitmapData->GetSurface(face, mip).GetWidth(),    GPixelFormats[Format].BlockSizeX );
          int                 Height      = Align( BitmapData->GetSurface(face, mip).GetHeight(),   GPixelFormats[Format].BlockSizeY );
          int                 RowByteSize = Align((Width / GPixelFormats[Format].BlockSizeX) * GPixelFormats[Format].BlockBytes, GPixelFormats[Format].RowMemoryAlignment);;
          int                 NumRows     = (Height / GPixelFormats[Format].BlockSizeY);

          for ( int Y = 0; Y < NumRows; Y++ )
          {
            Memcpy(Dest + Y * RowByteSize, &PixelLineBuffer[Y * Pitch], Min(Pitch, RowByteSize));
          }
        }
      }
    }
    else if (TextureType == ARCH_VOLUME)
    {
      int NumMip = 0;
      int Width = 0;
      int Height = 0;
      int Format = 0;
      int Depth = 0;

      FileStream->Serialize((char *) &Format,      sizeof(Format));
      FileStream->Serialize((char *) &NumMip,      sizeof(NumMip));
      FileStream->Serialize((char *) &Width,       sizeof(Width));
      FileStream->Serialize((char *) &Height,      sizeof(Height));
      FileStream->Serialize((char *) &Depth,    sizeof(Depth));

      BitmapData = new NVolumeData((BitmapFormat) Format, Width, Height, Depth, NumMip);

      for (int mip = 0; mip < NumMip; mip++)
      {
        for (int s = 0; s < ImageSurface::GetLevelDim(BitmapFormat(Format), Depth, mip); s++)
        {
          int Pitch = 0;;
          int Size = 0;
          FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
          FileStream->Serialize((char *) &Size,     sizeof(Size));

          BYTE *PixelLineBuffer = new BYTE[Size];
          FileStream->Serialize((char *) PixelLineBuffer,   Size);

          BYTE	            *Dest        = BitmapData->GetSurface(mip, s).GetPtrRawData();
          BitmapFormat   Format      = BitmapData->GetSurface(mip, s).GetFormat();
          int                 Width       = Align( BitmapData->GetSurface(mip, s).GetWidth(),    GPixelFormats[Format].BlockSizeX );
          int                 Height      = Align( BitmapData->GetSurface(mip, s).GetHeight(),   GPixelFormats[Format].BlockSizeY );
          int                 RowByteSize = Align((Width / GPixelFormats[Format].BlockSizeX) * GPixelFormats[Format].BlockBytes, GPixelFormats[Format].RowMemoryAlignment);;
          int                 NumRows     = (Height / GPixelFormats[Format].BlockSizeY);

          for ( int Y = 0; Y < NumRows; Y++ )
          {
            Memcpy(Dest + Y * RowByteSize, &PixelLineBuffer[Y * Pitch], Min(Pitch, RowByteSize));
          }
        }
      }
    }
    else if (TextureType == ARCH_ANIMATEDTEXTURE)
    {
      int NumMip = 0;
      int Width = 0;
      int Height = 0;
      int Format = 0;
      int Depth = 0;

      FileStream->Serialize((char *) &Format,      sizeof(Format));
      FileStream->Serialize((char *) &NumMip,      sizeof(NumMip));
      FileStream->Serialize((char *) &Width,       sizeof(Width));
      FileStream->Serialize((char *) &Height,      sizeof(Height));
      FileStream->Serialize((char *) &Depth,    sizeof(Depth));

      nuxAssert(NumMip == 1);
      NAnimatedTextureData *AnimatedTextureData = new NAnimatedTextureData((BitmapFormat) Format, Width, Height, Depth);
      BitmapData = NUX_STATIC_CAST(NBitmapData *, AnimatedTextureData);

      for (int d = 0; d < Depth; d++)
      {
        int frame_time_ms;
        FileStream->Serialize((char *) &frame_time_ms,    sizeof(unsigned int));
        AnimatedTextureData->AddFrameTime(frame_time_ms);
      }

      for (int mip = 0; mip < NumMip; mip++)
      {
        for (int s = 0; s < ImageSurface::GetLevelDim(BitmapFormat(Format), Depth, mip); s++)
        {
          int Pitch = 0;;
          int Size = 0;
          FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
          FileStream->Serialize((char *) &Size,     sizeof(Size));

          BYTE *PixelLineBuffer = new BYTE[Size];
          FileStream->Serialize((char *) PixelLineBuffer,   Size);

          BYTE	            *Dest        = BitmapData->GetSurface(s).GetPtrRawData();
          BitmapFormat   Format      = BitmapData->GetSurface(s).GetFormat();
          int                 Width       = Align( BitmapData->GetSurface(s).GetWidth(),    GPixelFormats[Format].BlockSizeX );
          int                 Height      = Align( BitmapData->GetSurface(s).GetHeight(),   GPixelFormats[Format].BlockSizeY );
          int                 RowByteSize = Align((Width / GPixelFormats[Format].BlockSizeX) * GPixelFormats[Format].BlockBytes, GPixelFormats[Format].RowMemoryAlignment);;
          int                 NumRows     = (Height / GPixelFormats[Format].BlockSizeY);

          for ( int Y = 0; Y < NumRows; Y++ )
          {
            Memcpy(Dest + Y * RowByteSize, &PixelLineBuffer[Y * Pitch], Min(Pitch, RowByteSize));
          }
        }
      }
    }
    else
    {
      nuxAssertMsg(0, "[TextureArchiveAdd_ver_0_0_1] Texture type unknown.");
      return 0;
    }

    unsigned int TextureEndPosition = FileStream->Tell();

    if (TextureEndPosition != TextureDataPosition + TextureDataSize)
    {
      nuxAssertMsg(0, "[TextureArchiveLoad_ver_0_0_1] Loading error.");
    }

    return BitmapData;
  }

  void TextureArchiveGetInfo_ver_0_0_1(NSerializer *FileStream, TextureArchiveInfo_ver_0_0_1 &TextureArchiveInfo)
  {
    Memset(&TextureArchiveInfo, 0, sizeof(TextureArchiveInfo_ver_0_0_1));

    unsigned int texturenamesize = 0;
    char TextureFilename[1024];

    TextureArchiveInfo.FilePointer = FileStream->Tell();
    FileStream->Serialize((char *) &texturenamesize,     sizeof(texturenamesize));
    FileStream->Serialize((char *) TextureFilename,     texturenamesize);
    TextureArchiveInfo.Name = NFileName(TextureFilename);
    unsigned int texturedatasize = 0;
    FileStream->Serialize((char *) &texturedatasize,     sizeof(texturedatasize));
    //     // Jump to next texture position(or end of this texture).
    //     m_FileStream->Seek(texturedatasize, NSerializer::SeekCurrent);

    FileStream->Serialize((char *) &TextureArchiveInfo.Type, sizeof(TextureArchiveInfo.Type));

    if (TextureArchiveInfo.Type == ARCH_TEXTURE2D)
    {
      FileStream->Serialize((char *) &TextureArchiveInfo.Format, sizeof(TextureArchiveInfo.Format));
      FileStream->Serialize((char *) &TextureArchiveInfo.NumMipmap, sizeof(TextureArchiveInfo.NumMipmap));
      FileStream->Serialize((char *) &TextureArchiveInfo.Width, sizeof(TextureArchiveInfo.Width));
      FileStream->Serialize((char *) &TextureArchiveInfo.Height, sizeof(TextureArchiveInfo.Height));

      TextureArchiveInfo.Size = 0;

      for (int mip = 0; mip < TextureArchiveInfo.NumMipmap; mip++)
      {
        int Pitch = 0;
        int Size = 0;
        FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
        FileStream->Serialize((char *) &Size,     sizeof(Size));
        nuxAssertMsg(Size == Pitch * ImageSurface::GetLevelBlockHeight(BitmapFormat(TextureArchiveInfo.Format), TextureArchiveInfo.Height, mip), "[TextureArchiveGetInfo_ver_0_0_1] Level size is inconsistant.");
        TextureArchiveInfo.Size += Size;
        // Jump to next texture mip.
        FileStream->Seek(Size, NSerializer::SeekCurrent);
      }
    }
    else if (TextureArchiveInfo.Type == ARCH_CUBEMAP)
    {
      FileStream->Serialize((char *) &TextureArchiveInfo.Format, sizeof(TextureArchiveInfo.Format));
      FileStream->Serialize((char *) &TextureArchiveInfo.NumMipmap, sizeof(TextureArchiveInfo.NumMipmap));
      FileStream->Serialize((char *) &TextureArchiveInfo.Width, sizeof(TextureArchiveInfo.Width));
      FileStream->Serialize((char *) &TextureArchiveInfo.Height, sizeof(TextureArchiveInfo.Height));

      TextureArchiveInfo.Size = 0;

      for (int face = 0; face < 6; face++)
      {
        for (int mip = 0; mip < TextureArchiveInfo.NumMipmap; mip++)
        {
          int Pitch = 0;
          int Size = 0;
          FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
          FileStream->Serialize((char *) &Size,     sizeof(Size));
          nuxAssertMsg(Size == Pitch * ImageSurface::GetLevelBlockHeight(BitmapFormat(TextureArchiveInfo.Format), TextureArchiveInfo.Height, mip), "[TextureArchiveGetInfo_ver_0_0_1] Level size is inconsistant.");
          TextureArchiveInfo.Size += Size;
          // Jump to next texture mip in the face.
          FileStream->Seek(Size, NSerializer::SeekCurrent);
        }
      }
    }
    else if (TextureArchiveInfo.Type == ARCH_VOLUME)
    {
      FileStream->Serialize((char *) &TextureArchiveInfo.Format, sizeof(TextureArchiveInfo.Format));
      FileStream->Serialize((char *) &TextureArchiveInfo.NumMipmap, sizeof(TextureArchiveInfo.NumMipmap));
      FileStream->Serialize((char *) &TextureArchiveInfo.Width, sizeof(TextureArchiveInfo.Width));
      FileStream->Serialize((char *) &TextureArchiveInfo.Height, sizeof(TextureArchiveInfo.Height));
      FileStream->Serialize((char *) &TextureArchiveInfo.Depth, sizeof(TextureArchiveInfo.Depth));

      TextureArchiveInfo.Size = 0;

      for (int mip = 0; mip < TextureArchiveInfo.NumMipmap; mip++)
      {
        for (int s = 0; s < ImageSurface::GetLevelDim(BitmapFormat(TextureArchiveInfo.Format), TextureArchiveInfo.Depth, mip); s++)
        {
          int Pitch = 0;
          int Size = 0;
          FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
          FileStream->Serialize((char *) &Size,     sizeof(Size));
          nuxAssertMsg(Size == Pitch * ImageSurface::GetLevelBlockHeight(BitmapFormat(TextureArchiveInfo.Format), TextureArchiveInfo.Height, mip), "[TextureArchiveGetInfo_ver_0_0_1] Level size is inconsistant.");
          TextureArchiveInfo.Size += Size;
          // Jump to next texture depth.
          FileStream->Seek(Size, NSerializer::SeekCurrent);
        }
      }
    }
    else if (TextureArchiveInfo.Type == ARCH_ANIMATEDTEXTURE)
    {
      FileStream->Serialize((char *) &TextureArchiveInfo.Format, sizeof(TextureArchiveInfo.Format));
      FileStream->Serialize((char *) &TextureArchiveInfo.NumMipmap, sizeof(TextureArchiveInfo.NumMipmap));
      FileStream->Serialize((char *) &TextureArchiveInfo.Width, sizeof(TextureArchiveInfo.Width));
      FileStream->Serialize((char *) &TextureArchiveInfo.Height, sizeof(TextureArchiveInfo.Height));
      FileStream->Serialize((char *) &TextureArchiveInfo.Depth, sizeof(TextureArchiveInfo.Depth));

      for (int d = 0; d < TextureArchiveInfo.Depth; d++)
      {
        int frame_time_ms;
        FileStream->Serialize((char *) &frame_time_ms,    sizeof(unsigned int));
      }

      TextureArchiveInfo.Size = 0;

      for (int mip = 0; mip < TextureArchiveInfo.NumMipmap; mip++)
      {
        for (int s = 0; s < ImageSurface::GetLevelDim(BitmapFormat(TextureArchiveInfo.Format), TextureArchiveInfo.Depth, mip); s++)
        {
          int Pitch = 0;
          int Size = 0;
          FileStream->Serialize((char *) &Pitch,    sizeof(Pitch));
          FileStream->Serialize((char *) &Size,     sizeof(Size));
          nuxAssertMsg(Size == Pitch * ImageSurface::GetLevelBlockHeight(BitmapFormat(TextureArchiveInfo.Format), TextureArchiveInfo.Height, mip), "[TextureArchiveGetInfo_ver_0_0_1] Level size is inconsistant.");
          TextureArchiveInfo.Size += Size;
          // Jump to next texture depth.
          FileStream->Seek(Size, NSerializer::SeekCurrent);
        }
      }
    }
  }


//////////////////

  bool CreateITXFile(const TCHAR *InputTextureFile, const TCHAR *OutputTextureFile)
  {
    nuxAssert(InputTextureFile);

    if (InputTextureFile == 0)
      return false;

    NFileName Filename = InputTextureFile;
    NBitmapData *BaseTexture = LoadImageFile(Filename.GetTCharPtr());

    if (BaseTexture == 0)
      return false;

    NFileName OutputFilename;

    if ((OutputTextureFile == 0) || (NString(OutputTextureFile) == NString("")))
    {
      OutputFilename = InputTextureFile;
      OutputFilename.ChangeFileExtension("itx");
    }
    else
      OutputFilename = OutputTextureFile;

    NSerializer *fileStream = GFileManager.CreateFileWriter(OutputFilename.GetTCharPtr(), NSerializer::Read | NSerializer::Write);
    fileStream->Seek(0, NSerializer::SeekStart);
    long long Offset = 0;

    unsigned int FileTag = TEXTUREARCHIVETAG;
    unsigned int FileVersion = TEXTUREARCHIVEVERSION;

    fileStream->Serialize((char *) &FileTag,     sizeof(FileTag));;
    fileStream->Serialize((char *) &FileVersion,     sizeof(FileVersion));

    TextureArchiveAdd_ver_0_0_1(fileStream, BaseTexture, InputTextureFile, Offset);

    fileStream->Close();
    return true;
  }

  NBitmapData *LoadITXFile(const TCHAR *ITXFilename)
  {
    nuxAssert(ITXFilename);

    if (ITXFilename == 0)
      return false;

    NSerializer *fileStream = GFileManager.CreateFileReader(ITXFilename, NSerializer::Read | NSerializer::Write);
    fileStream->Seek(0, NSerializer::SeekStart);
    long long Offset = 0;

    unsigned int FileTag;
    unsigned int FileVersion;
    fileStream->Serialize((char *) &FileTag, sizeof(FileTag));
    fileStream->Serialize((char *) &FileVersion, sizeof(FileVersion));

    if (FileTag != TEXTUREARCHIVETAG)
    {
      //nuxAssertMsg(0, "[LoadITXFile] File: %s format is incorrect.", ITXFilename);
      return 0;
    }

    Offset = fileStream->Tell();
    NBitmapData *data = TextureArchiveLoad_ver_0_0_1(fileStream, Offset);

    fileStream->Close();
    return data;
  }

}

