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


#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include "NuxCore/NuxCore.h"
#include "BitmapFormats.h"

#include "Bmp.h"

#if defined(NUX_OS_LINUX)
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
#define BI_JPEG       4L
#define BI_PNG        5L
#endif

namespace nux
{

  ImageInfo get_bmp_info(const std::string &file_name)
  {
    ImageInfo info;
    info.isDelegate = false;
    info.width = 0;
    info.height = 0;
    info.bytes_per_pixel = 0;
    info.planes = 0;
    info.format = "";

    std::ifstream file;
#if defined(_WIN32)
    struct _stat file_info;
#else
    struct stat file_info;
#endif

    bmp_fileheader fileheader;
    bmp_infoheader infoheader;

    // System call: check if the file exist
#if defined(_WIN32)

    if (_stat(file_name.c_str(), &file_info) != 0)
#else
    if (stat(file_name.c_str(), &file_info) != 0)
#endif
    {
      DEBUG_WARNING("file not found");
      return info;
    }

    file.open(file_name.c_str(), std::ifstream::binary);                 // open as binary

    if (!file.is_open())
    {
      //fprintf(stderr, "open file %s failed", file_name);
      //DEBUG_WARNING("Can't open file");
      return info;
    }

    // Read the footer first
    file.seekg(0, std::ios::beg);
    file.read((char *) (&fileheader.bfType),              2);
    file.read((char *) (&fileheader.bfSize),              4);
    file.read((char *) (&fileheader.bfReserved1),         2);
    file.read((char *) (&fileheader.bfReserved2),         2);
    file.read((char *) (&fileheader.offset_to_data),      4);

    if (!file.good())
    {
      file.close();
      //DEBUG_WARNING("error while reading the BMP file header");
      return info;
    }

    file.seekg(BITMAPFILEHEADER_SIZE, std::ios::beg);
    file.read((char *) (&infoheader.biSize),              4);
    file.read((char *) (&infoheader.biWidth),             4);
    file.read((char *) (&infoheader.biHeight),            4);
    file.read((char *) (&infoheader.biPlanes),            2);
    file.read((char *) (&infoheader.biBitCount),          2);
    file.read((char *) (&infoheader.biCompression),       4);
    file.read((char *) (&infoheader.biSizeImage),         4);
    file.read((char *) (&infoheader.biXPelsPerMeter),     4);
    file.read((char *) (&infoheader.biYPelsPerMeter),     4);
    file.read((char *) (&infoheader.biClrUsed),           4);
    file.read((char *) (&infoheader.biClrImportant),      4);

    if (!file.good())
    {
      file.close();
      //DEBUG_WARNING("error while reading the BMP info header");
      return info;
    }

    bool FLIP_ROW_ORDER = true;

    if (infoheader.biHeight < 0)
    {
      infoheader.biHeight = -infoheader.biHeight;
      FLIP_ROW_ORDER = false;
    }

    info.width = infoheader.biWidth;
    info.height = infoheader.biHeight;
    info.bytes_per_pixel = infoheader.biBitCount / 8;
    info.planes = 3;

    if (fileheader.bfType == 19778)
    {
      info.isDelegate = true;
    }

    file.close();
    return info;
  }

  NBitmapData *read_bmp_file(const TCHAR *file_name)
  {
    std::ifstream file;
#if defined(_WIN32)
    struct _stat file_info;
#else
    struct stat file_info;
#endif


    unsigned char *bmp_buffer, *bmp_data_pointer;
    int datasize;
    bmp_fileheader fileheader;
    bmp_infoheader infoheader;

    // System call: check if the file exist
#if defined(_WIN32)

    if (_stat(file_name, &file_info) != 0)
#else
    if (stat(file_name, &file_info) != 0)
#endif
    {
      DEBUG_WARNING("file not found");
      return 0;
    }

    file.open(file_name, std::ifstream::binary);                 // open as binary

    if (!file.is_open())
    {
      file.close();
      fprintf(stderr, "open file %s failed", file_name);
      DEBUG_WARNING("cannot open file");
      return 0;
    }

//        00- 01 $00 -$01 ASCII 2-byte "BM" bitmap identifier.
//        02- 05 $02 -$05 Total length of bitmap file in bytes.
//                        Four byte integer, LSB first.
//        06-09 $06 -$09  Reserved, possibly for image id or revision.
//                        Four byte integer, LSB first.
//        10-13 $0A-$0D   Offset to start of actual pixel data.
//                        Four byte integer, LSB first.
//        14-17 $0A-$11   Size of data header, usually 40 bytes.
//                        Four byte integer, LSB first.
//        18-21 $12 -$15  Width of bitmap in pixels.
//                        Four byte integer, LSB first.
//        22-25 $16 -$19  Height of bitmap in pixels.
//                        Four byte integer, LSB first.
//        26-27 $1A-$1B   Number of color planes. Usually 01
//                        Two byte integer, LSB first.
//        28-29 $1C -$1D  Number of bits per pixel. Sets color mode.
//                        Two byte integer, LSB first.
//                            1 - Monochrome
//                            4 - 16 lookup colors
//                            8 - 256 lookup colors
//                            16 - 65,536 lookup colors
//                            24 - 16,777,216 RGB colors
//                            32 - 16,777,216 RGB colors + alpha
//        30-33 $1E -$21  Non-lossy compression mode in use
//                        Four byte integer, LSB first.
//                          0 - None
//                          1 - 8-bit run length encoded
//                          1 - 4-bit run length encoded
//        34-37 $22 -$25  Size of stored pixel data
//                        Four byte integer, LSB first.
//        38-41 $26 -$29  Width resolution in pixels per meter
//                        Four byte integer, LSB first.
//        42-45 $2A-$2D   Height resolution in pixels per meter
//                        Four byte integer, LSB first.
//        46-49 $2E -$31  Number of colors actually used.
//                        Four byte integer, LSB first.
//        50-53 $32 -$35  Number of important colors
//                        Four byte integer, LSB first.

    // Read the footer first
    file.seekg(0, std::ios::beg);
    file.read((char *) (&fileheader.bfType),            2);
    file.read((char *) (&fileheader.bfSize),            4);
    file.read((char *) (&fileheader.bfReserved1),       2);
    file.read((char *) (&fileheader.bfReserved2),       2);
    file.read((char *) (&fileheader.offset_to_data),    4);

    if (!file.good())
    {
      file.close();
      DEBUG_WARNING("error while reading the BMP file header");
      return 0;
    }

    if (fileheader.bfType != 0x4D42) // Should be "BM"
    {
      file.close();
      return 0;
    }

    file.seekg(BITMAPFILEHEADER_SIZE, std::ios::beg);
    file.read((char *) (&infoheader.biSize),    4);
    file.read((char *) (&infoheader.biWidth),              4);
    file.read((char *) (&infoheader.biHeight),             4);
    file.read((char *) (&infoheader.biPlanes),             2);
    file.read((char *) (&infoheader.biBitCount),          2);
    file.read((char *) (&infoheader.biCompression),        4);
    file.read((char *) (&infoheader.biSizeImage),         4);
    file.read((char *) (&infoheader.biXPelsPerMeter),       4);
    file.read((char *) (&infoheader.biYPelsPerMeter),       4);
    file.read((char *) (&infoheader.biClrUsed),         4);
    file.read((char *) (&infoheader.biClrImportant),    4);

    if (!file.good())
    {
      file.close();
      DEBUG_WARNING("error while reading the BMP info header");
      return 0;
    }

//    int COMPRESSION_MODE = infoheader.biCompression;
//    if (infoheader.biCompression)
//    {
//        // No support for compression
//        file.close();
//        DEBUG_WARNING("BMP compressed data not supported");
//        return HR_UNSUPPORTED_FORMAT;
//    }

    if ((infoheader.biBitCount == 1) ||
         (infoheader.biBitCount == 4))
    {
      file.close();
      nuxAssertMsg(0, "[read_bmp_file] No support for 1 or 4 bit BMP");
      return 0;
    }

    if ((infoheader.biCompression == BI_RLE8) &&
         (infoheader.biBitCount == 8))
    {
      int PaletteSize = (infoheader.biClrUsed == 0) ? PowerOfTwo(infoheader.biBitCount) : infoheader.biClrUsed;

      for (int pal = 0; pal < PaletteSize; pal++)
      {
        file.read((char *) (&infoheader.colorpalette[pal]),    4);
      }
    }

    if (infoheader.biCompression == BI_BITFIELDS)
    {
      file.read((char *) (&infoheader.redMask),    4);
      file.read((char *) (&infoheader.greenMask),    4);
      file.read((char *) (&infoheader.blueMask),    4);

      if (infoheader.biBitCount == 32)
      {
        nuxDebugMsg("[read_bmp_file] Warning: 32 Bits BMP with BITFIELDS has not been implemented");
      }
    }

    bool FLIP_ROW_ORDER = true;

    if (infoheader.biHeight < 0)
    {
      infoheader.biHeight = -infoheader.biHeight;
      FLIP_ROW_ORDER = false;
    }

    if ((infoheader.biCompression == BI_RLE4) && (infoheader.biCompression != BI_BITFIELDS))
    {
      // data is compressed. not supported.
      file.close();
      nuxAssertMsg(0, "[read_bmp_file] No support for 1 or 4 bit BMP");
      DEBUG_WARNING("BMP compressed data not supported");
      return 0;
    }

    if (! (infoheader.biBitCount == 8 ||
           infoheader.biBitCount == 16 ||
           infoheader.biBitCount == 24 ||
           infoheader.biBitCount == 32))
    {
      file.close();
      nuxAssertMsg(0, "[read_bmp_file] Unsupported bit count");
      return 0;
    }


    BitmapFormat bitmap_format;

    if (infoheader.biBitCount == 8)
      bitmap_format = BITFMT_A8;

    if (infoheader.biBitCount == 16)
      bitmap_format = BITFMT_R5G6B5;

    if (infoheader.biBitCount == 24)
      bitmap_format = BITFMT_R8G8B8;

    if (infoheader.biBitCount == 32)
      bitmap_format = BITFMT_R8G8B8A8;

    NTextureData *TextureObjectData = new NTextureData(bitmap_format, infoheader.biWidth, infoheader.biHeight, 1);

    // raster data size
    datasize = file_info.st_size - fileheader.offset_to_data;
    bmp_buffer = new unsigned char[datasize];

    // Set the position to read image data
    file.seekg(fileheader.offset_to_data, std::ios::beg);

    // Read image data
    file.read((char *) bmp_buffer, datasize);

    if (!file.good())
    {
      file.close();
      delete bmp_buffer;
      nuxAssertMsg(0, "[read_bmp_file] Error while reading the BMP data");
      delete TextureObjectData;
      return 0;
    }


    bmp_data_pointer = bmp_buffer;
    t_u32 image_width = TextureObjectData->GetSurface(0).GetWidth();
    t_u32 image_height = TextureObjectData->GetSurface(0).GetHeight();
    t_u32 i, j;
    t_u32 value;

    // 32 bits RGBA pixels
    if (infoheader.biBitCount == 32)
    {
      for (j = 0; j < image_height; j++)
      {
        for (i = 0; i < image_width; i++)
        {
          value = (* (bmp_data_pointer + 3) << 24) |  // a
                  (* (bmp_data_pointer + 0) << 16) |      // b
                  (* (bmp_data_pointer + 1) << 8) |       // g
                  * (bmp_data_pointer + 2);               // r

          TextureObjectData->GetSurface(0).Write32b(i, j, value);
          bmp_data_pointer += 4;
        }
      }
    }

    // 24 bits RGBA pixels
    //The bits in the array are packed together, but each scan line must be zero-padded to end on a 4 bytes boundary.
    if (infoheader.biBitCount == 24)
    {
      int BMP_PITCH = ((image_width * 3 + 3) >> 2) << 2;

      for (j = 0; j < image_height; j++)
      {
        bmp_data_pointer = bmp_buffer + j * BMP_PITCH;

        for (i = 0; i < image_width; i++)
        {
          value = (* (bmp_data_pointer + 0) << 16) |  // b
                  (* (bmp_data_pointer + 1) << 8) |       // g
                  * (bmp_data_pointer + 2);               // r

          TextureObjectData->GetSurface(0).Write24b(i, j, value);
          bmp_data_pointer += 3;
        }
      }
    }


    // 8 bits RGBA pixels
    //The bits in the array are packed together, but each scan line must be zero-padded to end on a 4 bytes boundary.
    if (infoheader.biBitCount == 8)
    {
      if (infoheader.biCompression != BI_RLE8)
      {
        int BMP_PITCH = ((image_width + 3) >> 2) << 2;

        for (j = 0; j < image_height; j++)
        {
          bmp_data_pointer = bmp_buffer + j * BMP_PITCH;

          for (i = 0; i < image_width; i++)
          {
            value = * (bmp_data_pointer);

            TextureObjectData->GetSurface(0).Write8b(i, j, value);
            bmp_data_pointer += 1;
          }
        }
      }
      else
      {
        t_u32 ix, iy = image_height - 1;
        t_u32 val, valS, skip;
        t_u32 index = 0;

        while (iy >= 0)
        {
          ix = 0;

          while (ix < image_width)
          {
            val = bmp_buffer[index++];

            if (0 != val)
            {
              valS = bmp_buffer[index++];

              while (val--)
              {
                TextureObjectData->GetSurface(0).Write8b(ix, iy, infoheader.colorpalette[valS]);
                ix++;
              }
            }
            else
            {
              /* Special modes... */
              val = bmp_buffer[index++];

              switch(val)
              {
                case 0: /* End-Of-Line detected */
                  ix = image_width;
                  iy--;
                  break;
                case 1: /* End-Of-Picture detected */
                  ix = image_width;
                  iy = -1;
                  break;
                case 2: /* Position change detected */
                  val = bmp_buffer[index++];
                  ix += val;
                  val = bmp_buffer[index++];
                  iy -= val;
                  break;
                default: /* Transparent(not compressed) sequence detected */

                  if (val & 1)
                    skip = 1;
                  else
                    skip = 0;

                  while (val--)
                  {
                    valS = bmp_buffer[index++];
                    TextureObjectData->GetSurface(0).Write8b(ix, iy, infoheader.colorpalette[valS]);
                    ix++;
                  }

                  break;
              }
            }
          }
        }
      }
    }

    if (infoheader.biBitCount == 16)
    {
      if (infoheader.biCompression != BI_BITFIELDS)
      {
        int BMP_PITCH = ((image_width * 2 + 3) >> 2) << 2;

        for (j = 0; j < image_height; j++)
        {
          // Photoshop saves a 16 bits BMP with 5 bits for each red, green and blue.
          // We need to expand the green component to 6 bits.
          bmp_data_pointer = bmp_buffer + j * BMP_PITCH;

          for (i = 0; i < image_width; i++)
          {
            value = (* (bmp_data_pointer + 1) << 8) |
                    (* (bmp_data_pointer + 0)) ;

            BYTE b = (BYTE) (value & 0x1f);
            BYTE g = (BYTE) ( float((value & 0x3e0) >> 5) * 63.0f / 31.0f); // expand green component from 5 bits to 6 bits
            BYTE r = (BYTE) ((value & 0x7c00) >> 10);

            value = (r << 11) | (g << 5) | b;

            TextureObjectData->GetSurface(0).Write16b(i, j, value);
            bmp_data_pointer += 2;
          }
        }
      }
      else
      {
        int BMP_PITCH = ((image_width * 2 + 3) >> 2) << 2;

        for (j = 0; j < image_height; j++)
        {
          // Photoshop saves a 16 bits BMP with 5 bits for each red, green and blue.
          // We need to expand the green component to 6 bits.
          bmp_data_pointer = bmp_buffer + j * BMP_PITCH;

          for (i = 0; i < image_width; i++)
          {
            value = (* (bmp_data_pointer + 1) << 8) |
                    (* (bmp_data_pointer + 0)) ;

            BYTE b;
            BYTE g;
            BYTE r;

            if (Hak32_CountNumBits(infoheader.greenMask) == 5)
            {
              b = (BYTE) (value & infoheader.blueMask);
              g = (BYTE) ( float((value & infoheader.greenMask) >> 5) * 63.0f / 31.0f); // expand green component from 5 bits to 6 bits
              r = (BYTE) ((value & infoheader.redMask) >> 10);
            }
            else
            {
              b = (BYTE) (value & infoheader.blueMask);
              g = (BYTE) ((value & infoheader.greenMask) >> 5);
              r = (BYTE) ((value & infoheader.redMask) >> 11);
            }

            value = (r << 11) | (g << 5) | b;

            TextureObjectData->GetSurface(0).Write16b(i, j, value);
            bmp_data_pointer += 2;
          }
        }
      }
    }

//    //load_16(Biobuf *b, long width, long height, Rgb* buf, Rgb* clut)
//    {
//        uchar c[2];
//        long ix, iy, i = 0, step_up = 0;
//
//        if (height > 0) {	/* bottom-up */
//            i = (height - 1) * width;
//            step_up = -2 * width;
//        } else
//            height = -height;
//
//        if (clut) {
//            unsigned mask_blue =  (unsigned)clut[0].blue +
//                ((unsigned)clut[0].green << 8);
//            unsigned mask_green =  (unsigned)clut[1].blue +
//                ((unsigned)clut[1].green << 8);
//            unsigned mask_red =  (unsigned)clut[2].blue +
//                ((unsigned)clut[2].green << 8);
//            int shft_blue = msb((ulong)mask_blue) - 8;
//            int shft_green = msb((ulong)mask_green) - 8;
//            int shft_red = msb((ulong)mask_red) - 8;
//
//            for (iy = height; iy; iy--, i += step_up)
//                for (ix = 0; ix < width; ix++, i++) {
//                    unsigned val;
//                    Bread(b, c, sizeof(c));
//                    val = (unsigned)c[0] + ((unsigned)c[1] << 8);
//
//                    buf[i].alpha = 0;
//                    if (shft_blue >= 0)
//                        buf[i].blue = (uchar)((val & mask_blue) >> shft_blue);
//                    else
//                        buf[i].blue = (uchar)((val & mask_blue) << -shft_blue);
//                    if (shft_green >= 0)
//                        buf[i].green = (uchar)((val & mask_green) >> shft_green);
//                    else
//                        buf[i].green = (uchar)((val & mask_green) << -shft_green);
//                    if (shft_red >= 0)
//                        buf[i].red = (uchar)((val & mask_red) >> shft_red);
//                    else
//                        buf[i].red = (uchar)((val & mask_red) << -shft_red);
//                }
//        } else
//            for (iy = height; iy; iy--, i += step_up)
//                for (ix = 0; ix < width; ix++, i++) {
//                    Bread(b, c, sizeof(c));
//                    buf[i].blue = (uchar)((c[0] << 3) & 0xf8);
//                    buf[i].green = (uchar)(((((unsigned)c[1] << 6) +
//                        (((unsigned)c[0]) >> 2))) & 0xf8);
//                    buf[i].red = (uchar)((c[1] << 1) & 0xf8);
//                }
//                return 0;
//    }


    // correct orientation;
    // BMP are stored up-side down. The first pixel is the bottom left.
    if (FLIP_ROW_ORDER)
      TextureObjectData->GetSurface(0).FlipVertical();

    delete [] bmp_buffer;
    file.close();

    return TextureObjectData;
  }

  HReport write_bmp_file(const TCHAR *file_name, NBitmapData *image)
  {
    std::fstream file;

    int datasize = 0;
    bmp_fileheader fileheader;
    bmp_infoheader infoheader;

    if (file_name == 0)
    {
      DEBUG_WARNING("Cannot create file");
      return HR_INVALID_ARG;
    }

    if (image->GetSurface(0).GetFormat() != BITFMT_R8G8B8)
    {
      DEBUG_WARNING("Unsupported format");
      return HR_INVALID_ARG;
    }

    file.open(file_name, std::fstream::out | std::fstream::binary);                 // open as binary

    if (!file.is_open())
    {
      fprintf(stderr, "Open file %s failed", file_name);
      DEBUG_WARNING("cannot open file");
      return HR_FILE_IO_FAIL;
    }

    if (image->GetSurface(0).GetFormat() == BITFMT_R8G8B8)
    {
      datasize = image->GetSurface(0).GetWidth() * image->GetSurface(0).GetHeight() * 3;
    }

    fileheader.bfType = 19778;
    fileheader.bfSize = datasize + BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE;
    fileheader.bfReserved1  = 0;
    fileheader.bfReserved2  = 0;
    fileheader.offset_to_data = BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE;

    infoheader.biSize = BITMAPINFOHEADER_SIZE;
    infoheader.biWidth = image->GetSurface(0).GetWidth();
    infoheader.biHeight = image->GetSurface(0).GetHeight();
    infoheader.biPlanes = 1;
    infoheader.biBitCount = 24;
    infoheader.biCompression = 0; // 0 = no compression, 1 = 8bit RLE encoding, 2 = 4bit RLE encoding
    infoheader.biSizeImage = datasize;
    infoheader.biXPelsPerMeter = 0;
    infoheader.biYPelsPerMeter = 0;
    infoheader.biClrUsed = 0;
    infoheader.biClrImportant = 0;

    file.seekg(0, std::ios::beg);

    file.write((char *) &fileheader.bfType,       sizeof(fileheader.bfType));
    file.write((char *) &fileheader.bfSize,       sizeof(fileheader.bfSize));
    file.write((char *) &fileheader.bfReserved1,        sizeof(fileheader.bfReserved1));
    file.write((char *) &fileheader.bfReserved2,        sizeof(fileheader.bfReserved2));
    file.write((char *) &fileheader.offset_to_data,  sizeof(fileheader.offset_to_data));

    file.write((char *) &infoheader.biSize, sizeof(infoheader.biSize));
    file.write((char *) &infoheader.biWidth,           sizeof(infoheader.biWidth));
    file.write((char *) &infoheader.biHeight,          sizeof(infoheader.biHeight));
    file.write((char *) &infoheader.biPlanes,          sizeof(infoheader.biPlanes));
    file.write((char *) &infoheader.biBitCount,       sizeof(infoheader.biBitCount));
    file.write((char *) &infoheader.biCompression,     sizeof(infoheader.biCompression));
    file.write((char *) &infoheader.biSizeImage,      sizeof(infoheader.biSizeImage));
    file.write((char *) &infoheader.biXPelsPerMeter,    sizeof(infoheader.biXPelsPerMeter));
    file.write((char *) &infoheader.biYPelsPerMeter,    sizeof(infoheader.biYPelsPerMeter));
    file.write((char *) &infoheader.biClrUsed,      sizeof(infoheader.biClrUsed));
    file.write((char *) &infoheader.biClrImportant, sizeof(infoheader.biClrImportant));

    if (!file.good())
    {
      file.close();
      DEBUG_WARNING("error while writing BMP file");
      return HR_FILE_IO_FAIL;
    }


    t_s32 i, j;

    for (j = 0; j < image->GetSurface(0).GetHeight(); j++)
    {
      for (i = 0; i < image->GetSurface(0).GetWidth(); i++)
      {
        t_u32 gba = image->GetSurface(0).Read(i, image->GetSurface(0).GetHeight() - 1 - j);
        file.write((char *) &gba, 3);
      }
    }

    if (!file.good())
    {
      file.close();
      DEBUG_WARNING("error while writing BMP file");
      return HR_FILE_IO_FAIL;
    }

    file.close();
    return HR_SUCCESS;
  }

}

