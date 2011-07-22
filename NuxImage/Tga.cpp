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
#include <memory>

#include "NuxCore/NuxCore.h"

#include "Tga.h"
#include "BitmapFormats.h"

namespace nux
{
  /* Load up a TGA file
  *
  *Only recognizes the following types of Targas:
  *		Type 02 - Uncompressed RGB, 16, 24 or 32 bits
  *		Type 03 - Uncompressed grayscale, 8 bits
  *		Type 10 - RLE-compressed RGB, 16, 24 or 32 bits
  *		Type 11 - RLE-compressed grayscale, 8 bits
  * There are several other Targa formats unsupported by this function
  */


  NBitmapData *read_tga_file (const TCHAR *file_name)
  {
    std::ifstream fileStream;
    BYTE *tga_buffer, *tga_data_pointer;
    t_u32 datasize;

    tga_header header;
    tga_footer footer;
    t_u32 rle, footer_present;

    if (!GFileManager.FileExist (file_name) )
    {
      //nuxAssertMsg(0, TEXT("[read_tga_file] File not found: %s"), file_name);
      return 0;
    }

    fileStream.open (file_name, std::ifstream::binary);       // open as binary

    if (!fileStream.is_open() )
    {
      //nuxAssertMsg(0, TEXT("[read_tga_file] Can't open file: %s"), file_name);
      return 0;
    }

    fileStream.seekg ( 0, std::ios::end);
    t_u32 filesize = fileStream.tellg();
    fileStream.seekg ( 0, std::ios::beg);


    // Read the footer first
    fileStream.seekg (- (TGA_FOOTER_SIZE), std::ios::end);
    fileStream.read ( (char *) (&footer.extensionAreaOffset), 4);
    fileStream.read ( (char *) (&footer.developerDirectoryOffset), 4);
    fileStream.read ( (char *) (&footer.signature), 16);
    fileStream.read ( (char *) (&footer.dot), 1);
    fileStream.read ( (char *) (&footer.null), 1);

    if (!fileStream.good() )
    {
      fileStream.close();
      //nuxAssertMsg(0, TEXT("[read_tga_file] Error while reading the TGA footer: %s"), file_name);
      return 0;
    }

    // Check the footer to see if we have a v2.0 TGA file
    footer_present = 0;

    if (Memcmp (footer.signature, TGA_SIGNATURE, TGA_SIGNATURE_SIZE) == 0)
      footer_present = 1;

    if (!footer_present)
    {
      // The footer is not present in TGA format version 1. this FILE supports only TGA format version 2.0
      // Here we also eliminate every file that is not really a tga format.
      //nuxAssertMsg(0, TEXT("[read_tga_file] This is not a TGA 2.0 file: %s"), file_name);
      fileStream.close();
      return 0;
    }

    // Read the header
    fileStream.seekg (0, std::ios::beg);
    fileStream.read ( (char *) (&header.id_length), 1);
    fileStream.read ( (char *) (&header.colormap_type), 1);
    fileStream.read ( (char *) (&header.image_type), 1);
    fileStream.read ( (char *) (&header.colormap_index_lo), 1);
    fileStream.read ( (char *) (&header.colormap_index_hi), 1);
    fileStream.read ( (char *) (&header.colormap_length_lo), 1);
    fileStream.read ( (char *) (&header.colormap_length_hi), 1);
    fileStream.read ( (char *) (&header.colormap_size), 1);
    fileStream.read ( (char *) (&header.x_origin_lo), 1);
    fileStream.read ( (char *) (&header.x_origin_hi), 1);
    fileStream.read ( (char *) (&header.y_origin_lo), 1);
    fileStream.read ( (char *) (&header.y_origin_hi), 1);
    fileStream.read ( (char *) (&header.width_lo), 1);
    fileStream.read ( (char *) (&header.width_hi), 1);
    fileStream.read ( (char *) (&header.height_lo), 1);
    fileStream.read ( (char *) (&header.height_hi), 1);
    fileStream.read ( (char *) (&header.bpp), 1);
    fileStream.read ( (char *) (&header.descriptor), 1);

    if (!fileStream.good() )
    {
      fileStream.close();
      //nuxAssertMsg(0, TEXT("[read_tga_file] Error while reading the TGA header: %s"), file_name);
      return 0;
    }

    switch (header.image_type)
    {
      case TGA_COLORMAP:
      case TGA_COLOR:
      case TGA_GRAYSCALE:
        rle = 0;
        break;

      case TGA_RLE_COLORMAP:
      case TGA_RLE_COLOR:
      case TGA_RLE_GRAYSCALE:
        rle = 1;
        break;

      default:
        fileStream.close();
        //nuxAssertMsg(0, TEXT("[read_tga_file] Couldn't find the image type: %s"), file_name);
        return 0;
    }


    if ( ( (header.image_type == TGA_COLORMAP) || (header.image_type == TGA_RLE_COLORMAP) ) && header.colormap_type == 0)
    {
      // the image is pseudo color but there is no colormap
      // problem in file
      fileStream.close();
      //nuxAssertMsg(0, TEXT("[read_tga_file] Couldn't find the colormap: %s"), file_name);
      return 0;
    }

    if (header.image_type == TGA_COLORMAP || header.image_type == TGA_RLE_COLORMAP)
    {
      // Not supported
      fileStream.close();
      //nuxAssertMsg(0, TEXT("[read_tga_file] TGA colormap image is not supported: %s"), file_name);
      return 0;
    }

    t_u32 width, height;
    width  = header.width_hi << 8 |  header.width_lo;
    height = header.height_hi << 8 | header.height_lo;

    // Size of image data = file size - header size - id length - colormap size
    datasize = filesize - (TGA_HEADER_SIZE + header.id_length +
                           (header.colormap_length_hi << 8) + header.colormap_length_lo + (footer_present ? TGA_FOOTER_SIZE : 0) );

    // Prepare the finale image
    BitmapFormat bitmap_format;

    if (header.bpp == 8)
      bitmap_format = BITFMT_A8;

    if (header.bpp == 16)
      bitmap_format = BITFMT_R5G6B5;

    if (header.bpp == 24)
      bitmap_format = BITFMT_R8G8B8;

    if (header.bpp == 32)
      bitmap_format = BITFMT_R8G8B8A8;

    std::scoped_ptr<NTextureData> TextureObjectData(new NTextureData (bitmap_format, (header.width_hi << 8) | (header.width_lo), (header.height_hi << 8) | (header.height_lo), 1));

    // Allocate memory for a temporary buffer
    tga_buffer = new BYTE[datasize];
    //memset(tga_buffer, 0, datasize);

    // Set the position to read image data
    fileStream.seekg (long (TGA_HEADER_SIZE + header.id_length + (header.colormap_length_hi << 8) + header.colormap_length_lo),
                      std::ios::beg);

    // Read image data
    fileStream.read ( (char *) tga_buffer, datasize);

    if (!fileStream.good() )
    {
      fileStream.close();
      delete [] tga_buffer;
      return 0;
    }

    tga_data_pointer = tga_buffer;

    t_u32 image_width = TextureObjectData->GetSurface (0).GetWidth();
    t_u32 image_height = TextureObjectData->GetSurface (0).GetHeight();

    if (rle == 0)
    {
      t_u32 j, i;
      t_u32 position = 0;
      t_u32 value;

      for (j = 0; j < image_height; j++)
      {
        for (i = 0; i < image_width; i++)
        {
          // In TGA the MSB is RED and the LSB is BLUE
          switch (bitmap_format)
          {
              // 32 bits RGBA pixels
            case BITFMT_R8G8B8A8:
              position = (j * image_width + i);
              // store ARGB
              value = (* ( (BYTE *) tga_data_pointer + 3) << 24) |	// a
                      (* ( (BYTE *) tga_data_pointer + 0) << 16) |		// b
                      (* ( (BYTE *) tga_data_pointer + 1) << 8) |		 // g
                      * ( (BYTE *) tga_data_pointer + 2);				 // r

              TextureObjectData->GetSurface (0).Write32b (i, j, value);
              tga_data_pointer += 4;
              break;

              // 24 bits RGB pixels
            case BITFMT_R8G8B8:
              position = (j * image_width + i);
              // store 0xff_RGB
              value = (* ( (BYTE *) tga_data_pointer + 0) << 16) | // b
                      (* ( (BYTE *) tga_data_pointer + 1) << 8) |		 // g
                      * ( (BYTE *) tga_data_pointer + 2);				 // r

              TextureObjectData->GetSurface (0).Write24b (i, j, value);
              tga_data_pointer += 3;
              break;

              // 16 bits RGB pixels
            case BITFMT_R5G6B5:
              // FIXME: it seem the format is R5G5B5 and not R5G6B5
              position = (j * image_width + i);

              BYTE r, g, b;
              value = ( (BYTE) * ( (BYTE *) (tga_data_pointer + 1) ) << 8) | ( (BYTE) * ( (BYTE *) (tga_data_pointer + 0) ) );

              // Photoshop saves a 16 bits BMP with 5 bits for each red, green and blue.
              // We need to expand the green component to 6 bits.
              b = (BYTE) (value & 0x1f);
              g = (BYTE) ( float ( (value & 0x3e0) >> 5) * 63.0f / 31.0f); // expand green component from 5 bits to 6 bits
              r = (BYTE) ( (value & 0x7c00) >> 10);

              value = (r << 11) | (g << 5) | b;

              TextureObjectData->GetSurface (0).Write16b (i, j, value);
              tga_data_pointer += 2;
              break;

              // 8 bits black & white (grayscale)
            case BITFMT_A8:
              position = (j * image_width + i);
              value = * ( (BYTE *) tga_data_pointer + 0);
              TextureObjectData->GetSurface (0).Write8b (i, j, value);
              tga_data_pointer += 1;
              break;

            default:
              break;
          }
        }
      }
    }
    else
    {
      UINT rep;
      UINT i = 0;
      UINT j = 0;
      UINT position = 0;
      UINT value;
      //unsigned BYTE *last_byte = buf_dest + 4*image->height*image->width; // last byte of destination image

      t_u32 size = TextureObjectData->GetSurface (0).GetWidth() * TextureObjectData->GetSurface (0).GetHeight();

      while (position < size)
      {
        t_u32 repetition_count = (*tga_data_pointer & 0x7f) + 1;

        if (*tga_data_pointer & 0x80) // RLE Packet
        {
          tga_data_pointer++;  // tga_data_pointer point the pixel value

          // In TGA the MSB is RED and the LSB is BLUE
          switch (bitmap_format)
          {
              // 32 bits RGBA pixels
            case BITFMT_R8G8B8A8:

              for (rep = 0; rep < repetition_count; rep++)
              {
                // store ARGB
                value = (* ( (BYTE *) tga_data_pointer + 3) << 24) |	// a
                        (* ( (BYTE *) tga_data_pointer + 0) << 16) |		// b
                        (* ( (BYTE *) tga_data_pointer + 1) << 8) |		 // g
                        * ( (BYTE *) tga_data_pointer + 2);				 // r
                TextureObjectData->GetSurface (0).Write32b (i, j, value);
                i++;

                if (i >= image_width)
                {
                  j++;
                  i = 0;
                }

                position++;
              }

              tga_data_pointer += 4;
              break;

              // 24 bits RGB pixels
            case BITFMT_R8G8B8:

              for (rep = 0; rep < repetition_count; rep++)
              {
                value = (0xff << 24) |
                        (* ( (BYTE *) tga_data_pointer + 0) << 16) |	 // b
                        (* ( (BYTE *) tga_data_pointer + 1) << 8) |		 // g
                        * ( (BYTE *) tga_data_pointer + 2);				 // r
                TextureObjectData->GetSurface (0).Write24b (i, j, value);
                i++;

                if (i >= image_width)
                {
                  j++;
                  i = 0;
                }

                position++;
              }

              tga_data_pointer += 3;
              break;

              // 16 bits RGB pixels
            case BITFMT_R5G6B5:

              for (rep = 0; rep < repetition_count; rep++)
              {
                // FIXME: it seem the format is R5G5B5 and not R5G6B5
                BYTE r, g, b;
                value = (* (tga_data_pointer + 1) << 8) | (* (tga_data_pointer + 0) );

                // Photoshop saves a 16 bits BMP with 5 bits for each red, green and blue.
                // We need to expand the green component to 6 bits.
                b = (BYTE) (value & 0x1f);
                g = (BYTE) ( float ( (value & 0x3e0) >> 5) * 63.0f / 31.0f); // expand green component from 5 bits to 6 bits
                r = (BYTE) ( (value & 0x7c00) >> 10);
                value = (r << 11) | (g << 5) | b;

                TextureObjectData->GetSurface (0).Write16b (i, j, value);
                i++;

                if (i >= image_width)
                {
                  j++;
                  i = 0;
                }

                position++;
              }

              tga_data_pointer += 2;
              break;

              // 8 bits grayscale pixels
            case BITFMT_A8:

              for (rep = 0; rep < repetition_count; rep++)
              {
                value = * ( (BYTE *) tga_data_pointer + 0);
                TextureObjectData->GetSurface (0).Write8b (i, j, value);
                i++;

                if (i >= image_width)
                {
                  j++;
                  i = 0;
                }

                position++;
              }

              tga_data_pointer += 1;
              break;

            default:
              break;
          }
        }
        else  // raw packet
        {
          tga_data_pointer++;	// tga_data_pointer point the pixel value

          for (rep = 0; rep < repetition_count; rep++)
          {
            switch (bitmap_format)
            {
                // 32 bits RGBA pixels
              case BITFMT_R8G8B8A8:
                value = (* ( (BYTE *) tga_data_pointer + 3) << 24) |		//a
                        (* ( (BYTE *) tga_data_pointer + 0) << 16) |			//r
                        (* ( (BYTE *) tga_data_pointer + 1) << 8) |			//g
                        * ( (BYTE *) tga_data_pointer + 2);					//b
                TextureObjectData->GetSurface (0).Write32b (i, j, value);
                i++;

                if (i >= image_width)
                {
                  j++;
                  i = 0;
                }

                position++;
                tga_data_pointer += 4;
                break;

                // 24 bits RGB pixels
              case BITFMT_R8G8B8:
                value = (* ( (BYTE *) tga_data_pointer + 0) << 16) |	//r
                        (* ( (BYTE *) tga_data_pointer + 1) << 8) |		//g
                        * ( (BYTE *) tga_data_pointer + 2);				//b
                TextureObjectData->GetSurface (0).Write24b (i, j, value);
                i++;

                if (i >= image_width)
                {
                  j++;
                  i = 0;
                }

                position++;
                tga_data_pointer += 3;
                break;

                // 16 bits RGB pixels
              case BITFMT_R5G6B5:
                // FIXME: it seem the format is R5G5B5 and not R5G6B5
                BYTE r, g, b;
                value = (* (tga_data_pointer + 1) << 8) | (* (tga_data_pointer + 0) );

                // Photoshop saves a 16 bits BMP with 5 bits for each red, green and blue.
                // We need to expand the green component to 6 bits.
                b = (BYTE) (value & 0x1f);
                g = (BYTE) ( float ( (value & 0x3e0) >> 5) * 63.0f / 31.0f); // expand green component from 5 bits to 6 bits
                r = (BYTE) ( (value & 0x7c00) >> 10);
                value = (r << 11) | (g << 5) | b;

                TextureObjectData->GetSurface (0).Write16b (i, j, value);
                i++;

                if (i >= image_width)
                {
                  j++;
                  i = 0;
                }

                position++;
                tga_data_pointer += 2;
                break;

                // 8 bits grayscale pixels
              case BITFMT_A8:
                value = * ( (BYTE *) tga_data_pointer + 0);
                TextureObjectData->GetSurface (0).Write8b (i, j, value);
                i++;

                if (i >= image_width)
                {
                  j++;
                  i = 0;
                }

                position++;
                tga_data_pointer += 1;
                break;

              default:
                break;
            }
          }
        }
      }
    }

    unsigned char flip = (header.descriptor & 0x30);

    switch (flip)
    {
      case 0x00:  // bottom left
        TextureObjectData->GetSurface (0).FlipVertical();
        break;
      case 0x10:  // bottom right
        TextureObjectData->GetSurface (0).FlipHorizontal();
        TextureObjectData->GetSurface (0).FlipVertical();
        break;
      case 0x20:  // top left
        // Ok
        break;
      case 0x30:  // top right
        TextureObjectData->GetSurface (0).FlipHorizontal();
        break;
    }

    delete [] tga_buffer;
    fileStream.close();

    return TextureObjectData;
  }

  HReport write_tga_file (const TCHAR *file_name, NBitmapData *image)
  {
    std::fstream fileStream;

    t_u32 datasize;
    tga_header header;
    tga_footer footer;

    if (file_name == 0)
    {
      nuxAssertMsg (0, TEXT ("[write_tga_file] Incorrect file name.") );
      return HR_INVALID_ARG;
    }

    /*if(image->get_format() != BITFMT_R8G8B8)
    {
        DEBUG_WARNING("Unsupported format");
        return HR_INVALID_ARG;
    }*/

    fileStream.open (file_name, std::fstream::out | std::fstream::binary);                 // open as binary

    if (!fileStream.is_open() )
    {
      fprintf (stderr, "Open file %s failed", file_name);
      DEBUG_WARNING ("cannot open file");
      return HR_FILE_IO_FAIL;
    }

    t_u32 BPP = 3;

    if (image->GetSurface (0).GetFormat() == BITFMT_R8G8B8 ||
        image->GetSurface (0).GetFormat() == BITFMT_B8G8R8)
    {
      datasize = image->GetSurface (0).GetWidth() * image->GetSurface (0).GetHeight() * 3;
      BPP = 3;
    }

    if (image->GetSurface (0).GetFormat() == BITFMT_A8R8G8B8 ||
        image->GetSurface (0).GetFormat() == BITFMT_B8G8R8A8 ||
        image->GetSurface (0).GetFormat() == BITFMT_R8G8B8A8 ||
        image->GetSurface (0).GetFormat() == BITFMT_A8B8G8R8)
    {
      datasize = image->GetSurface (0).GetWidth() * image->GetSurface (0).GetHeight() * 4;
      BPP = 4;
    }


    header.id_length = 0;
    header.colormap_type = 0;
    header.image_type = 2;                  // uncompressed True-Color
    header.colormap_index_lo = 0;
    header.colormap_index_hi = 0;
    header.colormap_length_lo = 0;
    header.colormap_length_hi = 0;
    header.colormap_size = 0;
    header.x_origin_lo = 0;
    header.x_origin_hi = 0;
    header.y_origin_lo = 0;
    header.y_origin_hi = 0;
    header.width_lo = image->GetSurface (0).GetWidth() & 0xff;
    header.width_hi = (image->GetSurface (0).GetWidth() & 0xff00) >> 8;
    header.height_lo = image->GetSurface (0).GetHeight() & 0xff;;
    header.height_hi = (image->GetSurface (0).GetHeight() & 0xff00) >> 8;
    header.bpp = BPP * 8;                          // 16, 24 or 32 bpp
    header.descriptor = 2 << 4;               // top left


    footer.extensionAreaOffset = 0;
    footer.developerDirectoryOffset = 0;
    footer.signature[0]  = 'T';
    footer.signature[1]  = 'R';
    footer.signature[2]  = 'U';
    footer.signature[3]  = 'E';
    footer.signature[4]  = 'V';
    footer.signature[5]  = 'I';
    footer.signature[6]  = 'S';
    footer.signature[7]  = 'I';
    footer.signature[8]  = 'O';
    footer.signature[9]  = 'N';
    footer.signature[10] = '-';
    footer.signature[11] = 'X';
    footer.signature[12] = 'F';
    footer.signature[13] = 'I';
    footer.signature[14] = 'L';
    footer.signature[15] = 'E';
    footer.dot = '.';
    footer.null = 0;

    fileStream.seekg (0, std::ios::beg);
    fileStream.write ( (char *) &header.id_length,           sizeof (header.id_length) );
    fileStream.write ( (char *) &header.colormap_type,       sizeof (header.colormap_type) );
    fileStream.write ( (char *) &header.image_type,          sizeof (header.image_type) );
    fileStream.write ( (char *) &header.colormap_index_lo,   sizeof (header.colormap_index_lo) );
    fileStream.write ( (char *) &header.colormap_index_hi,   sizeof (header.colormap_index_hi) );
    fileStream.write ( (char *) &header.colormap_length_lo,  sizeof (header.colormap_length_lo) );
    fileStream.write ( (char *) &header.colormap_length_hi,  sizeof (header.colormap_length_hi) );
    fileStream.write ( (char *) &header.colormap_size,       sizeof (header.colormap_size) );
    fileStream.write ( (char *) &header.x_origin_lo,         sizeof (header.x_origin_lo) );
    fileStream.write ( (char *) &header.x_origin_hi,         sizeof (header.x_origin_hi) );
    fileStream.write ( (char *) &header.y_origin_lo,         sizeof (header.y_origin_lo) );
    fileStream.write ( (char *) &header.y_origin_hi,         sizeof (header.y_origin_hi) );
    fileStream.write ( (char *) &header.width_lo,            sizeof (header.width_lo) );
    fileStream.write ( (char *) &header.width_hi,            sizeof (header.width_hi) );
    fileStream.write ( (char *) &header.height_lo,           sizeof (header.height_lo) );
    fileStream.write ( (char *) &header.height_hi,           sizeof (header.height_hi) );
    fileStream.write ( (char *) &header.bpp,                 sizeof (header.bpp) );
    fileStream.write ( (char *) &header.descriptor,          sizeof (header.descriptor) );

    t_s32 i, j;

    for (j = 0; j < image->GetSurface (0).GetHeight(); j++)
    {
      for (i = 0; i < image->GetSurface (0).GetWidth(); i++)
      {
        t_u32 gba = image->GetSurface (0).Read (i, j);
        fileStream.write ( (char *) &gba, BPP);
      }
    }

    if (!fileStream.good() )
    {
      fileStream.close();
      DEBUG_WARNING ("error while writing TGA file");
      return HR_FILE_IO_FAIL;
    }

    fileStream.write ( (char *) &footer.extensionAreaOffset,         sizeof (footer.extensionAreaOffset) );
    fileStream.write ( (char *) &footer.developerDirectoryOffset,    sizeof (footer.developerDirectoryOffset) );
    fileStream.write ( (char *) &footer.signature,                   sizeof (footer.signature) );
    fileStream.write ( (char *) &footer.dot,                         sizeof (footer.dot) );
    fileStream.write ( (char *) &footer.null,                        sizeof (footer.null) );


    fileStream.close();
    return HR_SUCCESS;
  }


}


