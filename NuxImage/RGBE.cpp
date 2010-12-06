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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "RGBE.h"


#include "NuxCore/NStreamBuffer.h"

#include "RGBE.h"
#include "BitmapFormats.h"
#include "ImageSurface.h"


/*
    This file contains code to read and write four byte rgbe file format
    developed by Greg Ward.  It handles the conversions between rgbe and
    pixels consisting of floats.  The data is assumed to be an array of floats.
    By default there are three floats per pixel in the order red, green, blue.
    (RGBE_DATA_??? values control this.)  Only the mimimal header reading and
    writing is implemented.  Each routine does error checking and will return
    a status value as defined below.  This code is intended as a skeleton so
    feel free to modify it to suit your needs.

    (Place notice here if you modified the code.)
    posted to http://www.graphics.cornell.edu/~bjw/
    written by Bruce Walter  (bjw@graphics.cornell.edu)  5/26/95
    based on code written by Greg Ward
*/

#ifdef _CPLUSPLUS
/* define if your compiler understands inline commands */
#define INLINE inline
#else
#define INLINE
#endif

/* offsets to red, green, and blue components in a data (float) pixel */
#define RGBE_DATA_RED    0
#define RGBE_DATA_GREEN  1
#define RGBE_DATA_BLUE   2
/* number of floats per pixel */
#define RGBE_DATA_SIZE   3

enum rgbe_error_codes
{
  rgbe_read_error,
  rgbe_write_error,
  rgbe_format_error,
  rgbe_memory_error,
};

namespace nux
{
  /* default error routine.  change this to change error handling */
  static int rgbe_error (int rgbe_error_code, const TCHAR *msg)
  {
    switch (rgbe_error_code)
    {
      case rgbe_read_error:
        nuxDebugMsg (TEXT ("RGBE read error") );
        break;
      case rgbe_write_error:
        nuxDebugMsg (TEXT ("RGBE write error") );
        break;
      case rgbe_format_error:
        nuxDebugMsg (TEXT ("RGBE bad file format: %s\n"), msg);
        break;
      default:
      case rgbe_memory_error:
        nuxDebugMsg (TEXT ("RGBE error: %s\n"), msg);
    }

    return RGBE_RETURN_FAILURE;
  }

  /* standard conversion from float pixels to rgbe pixels */
  /* note: you can remove the "inline"s if your compiler complains about it */
  static INLINE void FloatToRGBE (unsigned char rgbe[4], float red, float green, float blue)
  {
    float v;
    int e;

    v = red;

    if (green > v) v = green;

    if (blue > v) v = blue;

    if (v < 1e-32)
    {
      rgbe[0] = rgbe[1] = rgbe[2] = rgbe[3] = 0;
    }
    else
    {
      v = frexp (v, &e) * 256.0 / v;
      rgbe[0] = (unsigned char) (red * v);
      rgbe[1] = (unsigned char) (green * v);
      rgbe[2] = (unsigned char) (blue * v);
      rgbe[3] = (unsigned char) (e + 128);
    }
  }

  /* standard conversion from rgbe to float pixels */
  /* note: Ward uses ldexp(col+0.5,exp-(128+8)).  However we wanted pixels */
  /*       in the range [0,1] to map back into the range [0,1].            */
  static INLINE void
  RGBEToFloat (float *red, float *green, float *blue, unsigned char rgbe[4])
  {
    float f;

    if (rgbe[3])
    {
      /*nonzero pixel*/
      f = ldexp (1.0, rgbe[3] - (int) (128 + 8) );
      *red = rgbe[0] * f;
      *green = rgbe[1] * f;
      *blue = rgbe[2] * f;
    }
    else
      *red = *green = *blue = 0.0;
  }

  /* default minimal header. modify if you want more information in header */
  int RGBEWriteHeader (std::ofstream &fileStream, int width, int height, rgbe_header_info *info)
  {
//    if(!GFileManager.FileExist(filename))
//    {
//        //nuxAssertMsg(0, TEXT("[RGBEWriteHeader] File not found: %s"), file_name);
//        return 1;
//    }
//    std::ofstream fileStream;
//    fileStream.open(filename, std::ofstream::binary | std::ofstream::binary);
//    if(!fileStream.is_open())
//    {
//        //nuxAssertMsg(0, TEXT("[RGBEWriteHeader] Can't open file: %s"), filename);
//        return 1;
//    }
    fileStream.seekp ( 0, std::ios::beg);

    char Buffer[256]; //= {0};
    const char *programtype = "RGBE";

    /* The #? is to identify file type, the programtype is optional. */
    if (info && (info->valid & RGBE_VALID_PROGRAMTYPE) )
      programtype = info->programtype;

    Snprintf (Buffer, 256, 256 - 1, "#?%s\n", (const char *) programtype);
    fileStream.write (Buffer, (std::streamsize) strlen (Buffer) );

    if (!fileStream.good() )
    {
      fileStream.close();
      return 1;
    }

    if (info && (info->valid & RGBE_VALID_GAMMA) )
    {
      Snprintf (Buffer, 256, 256 - 1, "GAMMA=%g\n", info->gamma);
      fileStream.write (Buffer, (std::streamsize) strlen (Buffer) );

      if (!fileStream.good() )
      {
        fileStream.close();
        return 1;
      }
    }

    if (info && (info->valid & RGBE_VALID_EXPOSURE) )
    {
      Snprintf (Buffer, 256, 256 - 1, "EXPOSURE=%g\n", info->exposure);
      fileStream.write (Buffer, (std::streamsize) strlen (Buffer) );

      if (!fileStream.good() )
      {
        fileStream.close();
        return 1;
      }
    }

    Snprintf (Buffer, 256, 256 - 1, "FORMAT=32-bit_rle_rgbe\n\n");
    fileStream.write (Buffer, (std::streamsize) strlen (Buffer) );

    if (!fileStream.good() )
    {
      fileStream.close();
      return 1;
    }

    Snprintf (Buffer, 256, 256 - 1, "-Y %d +X %d\n", height, width);
    fileStream.write (Buffer, (std::streamsize) strlen (Buffer) );

    if (!fileStream.good() )
    {
      fileStream.close();
      return 1;
    }

    return 0;
  }

  /* minimal header reading.  modify if you want to parse more information */
  int RGBEReadHeader (std::ifstream &fileStream, int &width, int &height, rgbe_header_info &info)
  {
//    if(!GFileManager.FileExist(filename))
//    {
//        //nuxAssertMsg(0, TEXT("[RGBEReadHeader] File not found: %s"), file_name);
//        return 1;
//    }
//    std::ifstream fileStream;
//    fileStream.open(filename, std::ifstream::binary);
//    if(!fileStream.is_open())
//    {
//        //nuxAssertMsg(0, TEXT("[RGBEReadHeader] Can't open file: %s"), filename);
//        return 1;
//    }
    fileStream.seekg ( 0, std::ios::beg);

    if (!fileStream.good() )
    {
      fileStream.close();
      return 1;
    }

    char buf[128];
    int found_format;
    float tempf;
    int i;

    found_format = 0;
    //if (info)
    {
      info.valid = 0;
      info.programtype[0] = 0;
      info.gamma = info.exposure = 1.0;
    }

    fileStream.getline (buf, sizeof (buf) / sizeof (buf[0]) );

    if (!fileStream.good() )
    {
      fileStream.close();
      return 1;
    }

    if ( (buf[0] != '#') || (buf[1] != '?') )
    {
      /* if you want to require the magic token then uncomment the next line */
      /*return rgbe_error(rgbe_format_error,"bad initial token"); */
      return 1;
    }
    else //if (info)
    {
      info.valid |= RGBE_VALID_PROGRAMTYPE;

      for (i = 0; i < (t_s32) sizeof (info.programtype) - 1; i++)
      {
        if ( (buf[i+2] == 0) || isspace (buf[i+2]) )
          break;

        info.programtype[i] = buf[i+2];
      }

      info.programtype[i] = 0;
      fileStream.getline (buf, sizeof (buf) / sizeof (buf[0]) );

      if (!fileStream.good() )
      {
        fileStream.close();
        return 1;
      }

    }

    for (;;)
    {
      if ( (buf[0] == 0) || (buf[0] == '\n') )
      {
        break;
      }
      else if (strcmp (buf, "FORMAT=32-bit_rle_rgbe") == 0)
        { }      /* format found so break out of loop */
      else if ( (SSCANF_S (buf, "GAMMA=%g", &tempf) == 1) )
      {
        info.gamma = tempf;
        info.valid |= RGBE_VALID_GAMMA;
      }
      else if ( (SSCANF_S (buf, "EXPOSURE=%g", &tempf) == 1) ) // do not use SSCANF_S
      {
        info.exposure = tempf;
        info.valid |= RGBE_VALID_EXPOSURE;
      }

      fileStream.getline (buf, sizeof (buf) / sizeof (buf[0]) );

      if (!fileStream.good() )
      {
        fileStream.close();
        return 1;
      }
    }

    fileStream.getline (buf, sizeof (buf) / sizeof (buf[0]) );

    if (!fileStream.good() )
    {
      fileStream.close();
      return 1;
    }

//     if (strcmp(buf,"") != 0)
//     {
//         nuxDebugMsg(TEXT("missing blank line after FORMAT specifier"));
//         return 1;
//     }
//
//     fileStream.getline(buf, sizeof(buf)/sizeof(buf[0]));
//     if(!fileStream.good()) {fileStream.close(); return 1;}

    if (SSCANF_S (buf, "-Y %d +X %d", &height, &width) < 2) // don ot use SSCANF_S here... it cannot read width. Why???
    {
      nuxDebugMsg (TEXT ("missing image size specifier") );
      return 1;
    }

    return 0;
  }

  /* simple write routine that does not use run length encoding */
  /* These routines can be made faster by allocating a larger buffer and
  fread-ing and fwrite-ing the data in larger chunks */
  int RGBEWritePixels (std::ofstream &fileStream, float *data, int numpixels)
  {
    unsigned char rgbe[4];

    while (numpixels-- > 0)
    {
      FloatToRGBE (rgbe, data[RGBE_DATA_RED], data[RGBE_DATA_GREEN], data[RGBE_DATA_BLUE]);
      data += RGBE_DATA_SIZE;

      if (fileStream.write ( (const char *) rgbe, sizeof (rgbe) ).bad() )
      {
        rgbe_error (rgbe_write_error, NULL);
        return 1;
      }
    }

    return RGBE_RETURN_SUCCESS;
  }

  /* simple read routine.  will not correctly handle run length encoding */
  int RGBEReadPixels (std::ifstream &fileStream, float *data, int numpixels)
  {
    unsigned char rgbe[4];

    while (numpixels-- > 0)
    {
      if (fileStream.read ( (char *) rgbe, sizeof (rgbe) ).bad() )
      {
        rgbe_error (rgbe_read_error, NULL);
        return 1;
      }

      RGBEToFloat (&data[RGBE_DATA_RED], &data[RGBE_DATA_GREEN], &data[RGBE_DATA_BLUE], rgbe);
      data += RGBE_DATA_SIZE;
    }

    return RGBE_RETURN_SUCCESS;
  }

  /* The code below is only needed for the run-length encoded files. */
  /* Run length encoding adds considerable complexity but does */
  /* save some space.  For each scanline, each channel (r,g,b,e) is */
  /* encoded separately for better compression. */

  static int RGBEWriteBytes_RLE (std::ofstream &fileStream, unsigned char *data, int numbytes)
  {
#define MINRUNLENGTH 4
    int cur, beg_run, run_count, old_run_count, nonrun_count;
    unsigned char buf[2];

    cur = 0;

    while (cur < numbytes)
    {
      beg_run = cur;
      /* find next run of length at least 4 if one exists */
      run_count = old_run_count = 0;

      while ( (run_count < MINRUNLENGTH) && (beg_run < numbytes) )
      {
        beg_run += run_count;
        old_run_count = run_count;
        run_count = 1;

        while ( (data[beg_run] == data[beg_run + run_count]) && (beg_run + run_count < numbytes) && (run_count < 127) )
          run_count++;
      }

      /* if data before next big run is a short run then write it as such */
      if ( (old_run_count > 1) && (old_run_count == beg_run - cur) )
      {
        buf[0] = 128 + old_run_count;   /*write short run*/
        buf[1] = data[cur];

        if (fileStream.write ( (const char *) buf, sizeof (buf[0]) * 2).bad() )
        {
          rgbe_error (rgbe_write_error, NULL);
          return 1;
        }

        cur = beg_run;
      }

      /* write out bytes until we reach the start of the next run */
      while (cur < beg_run)
      {
        nonrun_count = beg_run - cur;

        if (nonrun_count > 128)
          nonrun_count = 128;

        buf[0] = nonrun_count;

        if (fileStream.write ( (const char *) buf, sizeof (buf[0]) ).bad() )
        {
          rgbe_error (rgbe_write_error, NULL);
          return 1;
        }

        if (fileStream.write ( (const char *) &data[cur], sizeof (data[0]) *nonrun_count).bad() )
        {
          rgbe_error (rgbe_write_error, NULL);
          return 1;
        }

        cur += nonrun_count;
      }

      /* write out next run if one was found */
      if (run_count >= MINRUNLENGTH)
      {
        buf[0] = 128 + run_count;
        buf[1] = data[beg_run];

        if (fileStream.write ( (const char *) buf, sizeof (buf[0]) * 2).bad() )
        {
          rgbe_error (rgbe_write_error, NULL);
          return 1;
        }

        cur += run_count;
      }
    }

    return RGBE_RETURN_SUCCESS;
#undef MINRUNLENGTH
  }

  int RGBEWritePixels_RLE (const TCHAR *filename, float *data, int scanline_width, int num_scanlines)
  {
    if (!GFileManager.FileExist (filename) )
    {
      //nuxAssertMsg(0, TEXT("[RGBEWritePixels_RLE] File not found: %s"), file_name);
      return 1;
    }

    std::ofstream fileStream;
    fileStream.open (filename, std::ofstream::binary | std::ofstream::binary);

    if (!fileStream.is_open() )
    {
      //nuxAssertMsg(0, TEXT("[RGBEWritePixels_RLE] Can't open file: %s"), filename);
      return 1;
    }

    fileStream.seekp ( 0, std::ios::beg);

    unsigned char rgbe[4];
    unsigned char *buffer;
    int i, err;

    if ( (scanline_width < 8) || (scanline_width > 0x7fff) )
    {
      // run length encoding is not allowed so write flat
      return RGBEWritePixels (fileStream, data, scanline_width * num_scanlines);
    }

    buffer = (unsigned char *) malloc (sizeof (unsigned char) * 4 * scanline_width);

    if (buffer == NULL)
    {
      // no buffer space so write flat
      return RGBEWritePixels (fileStream, data, scanline_width * num_scanlines);
    }

    while (num_scanlines-- > 0)
    {
      rgbe[0] = 2;
      rgbe[1] = 2;
      rgbe[2] = scanline_width >> 8;
      rgbe[3] = scanline_width & 0xFF;

      if (fileStream.write ( (const char *) rgbe, sizeof (rgbe) ).bad() )
      {
        free (buffer);
        rgbe_error (rgbe_write_error, NULL);
        fileStream.close();
        return 1;
      }

      if (fileStream.write ( (const char *) rgbe, sizeof (rgbe) ).bad() )
      {
        free (buffer);
        fileStream.close();
        return rgbe_error (rgbe_write_error, NULL);
      }

      for ( i = 0; i < scanline_width; i++ )
      {
        FloatToRGBE (rgbe, data[RGBE_DATA_RED], data[RGBE_DATA_GREEN], data[RGBE_DATA_BLUE]);
        buffer[i] = rgbe[0];
        buffer[i+scanline_width] = rgbe[1];
        buffer[i+2*scanline_width] = rgbe[2];
        buffer[i+3*scanline_width] = rgbe[3];
        data += RGBE_DATA_SIZE;
      }

      /* write out each of the four channels separately run length encoded */
      /* first red, then green, then blue, then exponent */
      for ( i = 0; i < 4; i++ )
      {
        if ( (err = RGBEWriteBytes_RLE (fileStream, &buffer[i*scanline_width], scanline_width) ) != RGBE_RETURN_SUCCESS)
        {
          free (buffer);
          fileStream.close();
          return err;
        }
      }
    }

    free (buffer);
    fileStream.close();
    return RGBE_RETURN_SUCCESS;
  }

  static NBitmapData *CreateRGBEBitmapData (float *data, BitmapFormat format, int width, int height)
  {
    NBitmapData *BitmapData = (NBitmapData *) new NTextureData (
                                BITFMT_RGB32F,
                                width,
                                height, 1);

    int pitch = ImageSurface::GetLevelPitchNoMemAlignment (format, width, height, 0);
    int blockheight = ImageSurface::GetLevelBlockHeight (format, height, 0);

    BYTE *buffer = (BYTE *) data;

    for (int b = 0; b < blockheight; b++)
    {
      Memcpy ( BitmapData->GetSurface (0).GetPtrRawData() + b * pitch,
               (const void *) (&buffer[b * pitch]),
               pitch);
    }

    return BitmapData;
  }

  NBitmapData *LoadRGBE (const TCHAR *filename)
  {
    if (!GFileManager.FileExist (filename) )
    {
      //nuxAssertMsg(0, TEXT("[RGBEReadPixels_RLE] File not found: %s"), file_name);
      return 0;
    }

    std::ifstream fileStream;
    fileStream.open (filename, std::ofstream::binary);

    if (!fileStream.is_open() )
    {
      //nuxAssertMsg(0, TEXT("[RGBEReadPixels_RLE] Can't open file: %s"), filename);
      return 0;
    }

    fileStream.seekg ( 0, std::ios::beg );

    int scanline_width, num_scanlines;
    rgbe_header_info rgbe_header;
    Memset (&rgbe_header, 0, sizeof (rgbe_header_info) );

    if (RGBEReadHeader (fileStream, scanline_width, num_scanlines, rgbe_header) )
    {
      fileStream.close();
      return 0;
    }

    float *floatdata = new float[scanline_width*num_scanlines* (RGBE_DATA_SIZE) ];
    float *converteddata = floatdata;

    unsigned char rgbe[4], *scanline_buffer, *ptr, *ptr_end;
    int i, count;
    unsigned char buf[2];
    int width = scanline_width;
    int height = num_scanlines;

    //1. RLE scanline length must be between 8 and 0x7fff (32767)
    if ( (scanline_width < 8) || (scanline_width > 0x7fff) )
    {
      // run length encoding is not allowed so read flat
      RGBEReadPixels (fileStream, floatdata, scanline_width * num_scanlines);
      fileStream.close();
      return CreateRGBEBitmapData (floatdata, BITFMT_RGB32F, width, height);
    }

    scanline_buffer = NULL;

    // read in each successive scanline
    while (num_scanlines > 0)
    {
      if (fileStream.read ( (char *) rgbe, sizeof (rgbe) ).bad() )
      {
        free (scanline_buffer);
        fileStream.close();
        rgbe_error (rgbe_read_error, NULL);
        return 0;
      }

      //2. RLE begins with 2 bytes equal to 2
      //3. Since 3rd btye is upper byte of scanline length, it must be >0 (signed)
      if ( (rgbe[0] != 2) || (rgbe[1] != 2) || (rgbe[2] & 0x80) )
      {
        /* this file is not run length encoded */
        RGBEToFloat (&floatdata[0], &floatdata[1], &floatdata[2], rgbe);
        floatdata += RGBE_DATA_SIZE;
        free (scanline_buffer);
        RGBEReadPixels (fileStream, floatdata, width * height - 1);
        fileStream.close();

        return CreateRGBEBitmapData (floatdata, BITFMT_RGB32F, width, height);
      }

      //4. 3rd byte and 4th byte equals scanline length
      if ( ( ( (int) rgbe[2]) << 8 | rgbe[3]) != scanline_width)
      {
        free (scanline_buffer);
        fileStream.close();
        rgbe_error (rgbe_format_error, "wrong scanline width");
        return 0;
      }

      if (scanline_buffer == NULL)
        scanline_buffer = (unsigned char *) std::malloc (sizeof (unsigned char) * 4 * scanline_width);

      if (scanline_buffer == NULL)
      {
        fileStream.close();
        rgbe_error (rgbe_memory_error, "unable to allocate buffer space");
        return 0;
      }

      ptr = &scanline_buffer[0];

      // read each of the four channels for the scanline into the buffer
      for (i = 0; i < 4; i++)
      {
        ptr_end = &scanline_buffer[ (i+1) *scanline_width];

        while (ptr < ptr_end)
        {
          // read 2 bytes
          if (fileStream.read ( (char *) buf, 2).bad() )
          {
            free (scanline_buffer);
            fileStream.close();
            rgbe_error (rgbe_read_error, NULL);
            return 0;
          }

          if (buf[0] > 128)
          {
            /* a run of the same value */
            count = buf[0] - 128;

            if ( (count == 0) || (count > ptr_end - ptr) )
            {
              free (scanline_buffer);
              fileStream.close();
              rgbe_error (rgbe_format_error, "bad scanline data");
              return 0;
            }

            while (count-- > 0)
              *ptr++ = buf[1];
          }
          else
          {
            /* a non-run */
            count = buf[0];

            if ( (count == 0) || (count > ptr_end - ptr) )
            {
              free (scanline_buffer);
              fileStream.close();
              rgbe_error (rgbe_format_error, "bad scanline data");
              return 0;
            }

            *ptr++ = buf[1];

            if (--count > 0)
            {
              if (fileStream.read ( (char *) ptr, sizeof (*ptr) *count).bad() )
              {
                free (scanline_buffer);
                fileStream.close();
                rgbe_error (rgbe_read_error, NULL);
                return 0;
              }

              ptr += count;
            }
          }
        }
      }

      // now convert data from buffer into floats
      for (i = 0; i < scanline_width; i++)
      {
        rgbe[0] = scanline_buffer[i];
        rgbe[1] = scanline_buffer[i+scanline_width];
        rgbe[2] = scanline_buffer[i+2*scanline_width];
        rgbe[3] = scanline_buffer[i+3*scanline_width];
        RGBEToFloat (&floatdata[RGBE_DATA_RED], &floatdata[RGBE_DATA_GREEN], &floatdata[RGBE_DATA_BLUE], rgbe);
        floatdata += RGBE_DATA_SIZE;
      }

      num_scanlines--;
    }

    NBitmapData *BitmapData = CreateRGBEBitmapData (converteddata, BITFMT_RGB32F, width, height);

    delete [] converteddata;
    free (scanline_buffer);
    fileStream.close();

    return BitmapData;
  }

}
