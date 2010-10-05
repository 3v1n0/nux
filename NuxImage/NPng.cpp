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


#include "NuxCore/NKernel.h"

#include "png.h"
#include "NPng.h"
#include "BitmapFormats.h"


#include <stdio.h>
#if defined(_WIN32)
#include <windows.h>
#endif

namespace nux   //NUX_NAMESPACE_BEGIN
{

#define FAILMSGANDRETURN \
{\
    nuxAssertMsg(0, TEXT("[PNG Read Error] Cannot open %s for read."), FileNameAnsi); \
    return 0;\
}
#if defined(_WIN32)

#define PNG_FREERESOURCE if(hglobal) FreeResource(hglobal);
#define PNG_DECLARE_RESVAR \
    DWORD ressz;\
    HRSRC HRes  = NULL;\
    HGLOBAL hglobal = NULL;\
    void* resptr;
#define PNG_SETREADFN \
    if(!fp) png_set_read_fn(png_ptr, (void *)resptr, png_read_resource_fn);	 else
#define PNG_READHEADER \
    if(!fp)\
    {\
    memcpy(sig, resptr, 8);\
    resptr = (void*)((char*)resptr + 8);\
    } else

#else

#define PNG_FREERESOURCE
#define PNG_DECLARE_RESVAR
#define PNG_SETREADFN
#define PNG_READHEADER

#endif



  namespace
  {
#if defined(_WIN32)
    struct png_resource_access
    {
      png_resource_access() : hModule (NULL), res_type_id (0) {}
      HMODULE hModule;
      std::string res_type_name;
      unsigned long res_type_id;
    };
    png_resource_access png_resource;
#endif
  }

  void png_read_resource_fn (png_structp png_ptr, png_bytep data, png_size_t leng);

#if defined(_WIN32)
  void set_png_module_handle (unsigned long hM)
  {
    png_resource.hModule = (HMODULE) hM;
  }
  void set_png_module_restypename (const char *restypename)
  {
    if (HIWORD (restypename) )
    {
      png_resource.res_type_name = restypename;
      png_resource.res_type_id = 0;
    }
    else
    {
      //resource.res_type_name.clear(); should exist (cf STL Doc)
      png_resource.res_type_id = (unsigned long) restypename;
    }
  }
#endif


#if defined(_WIN32)
  HRSRC get_resource (const char *filename)
  {
    HRSRC HRes;
#ifdef _UNICODE
    unsigned int count = strlen (filename);
    wchar_t *wfilename = new wchar_t[count];
    mbstowcs (wfilename, filename, count);

    count = png_resource.res_type_name.size();
    wchar_t *wres_type_name = new wchar_t[count];
    mbstowcs (wres_type_name, png_resource.res_type_name.c_str(), count);

    HRes = FindResource (png_resource.hModule, wfilename,
                         png_resource.res_type_id ? (LPCWSTR) png_resource.res_type_id : wres_type_name);

    delete [] wfilename;
    delete [] wres_type_name;
#else
    HRes = FindResource (png_resource.hModule, filename,
                         png_resource.res_type_id ? (LPCSTR) png_resource.res_type_id : png_resource.res_type_name.c_str() );
#endif
    return HRes;
  }
#endif

  NBitmapData *read_png_rgba (const TCHAR *filename)
  {
    //--------- Resource stuff
    PNG_DECLARE_RESVAR
    //---------
    FILE *fp = NULL;
    png_byte sig[8];
    int bit_depth, color_type;
    double              gamma;
    png_uint_32 channels, row_bytes;
    png_structp png_ptr = 0;
    png_infop info_ptr = 0;

    ANSICHAR *FileNameAnsi = (ANSICHAR *) TCHAR_TO_ANSI (filename);

    if (!FileNameAnsi)
    {
      nuxAssertMsg (0, TEXT ("[read_png_rgba] Incorrect file name: %s."), filename);
      return 0;
    }


#if defined(_WIN32)
    struct _stat file_info;
#else
    struct stat file_info;
#endif


    // System call: check if the file exist
#if defined(_WIN32)

    if (_stat (FileNameAnsi, &file_info) != 0)
#else
    if (stat (FileNameAnsi, &file_info) != 0)
#endif
    {
      nuxAssert (TEXT ("[read_png_rgba] File not found: %s.") );
      return 0;
    }

#if (defined NUX_VISUAL_STUDIO_2005) || (defined NUX_VISUAL_STUDIO_2008)
    fopen_s (&fp, FileNameAnsi, "rb");
#else
    fp = fopen (FileNameAnsi, "rb");
#endif

    if (!fp)
#if defined(_WIN32)
    {
      // Try resource access
      HRes = get_resource (FileNameAnsi);

      if (!HRes)
        FAILMSGANDRETURN
        hglobal	=	LoadResource (png_resource.hModule, HRes);

      if (!hglobal)
        FAILMSGANDRETURN
        ressz = SizeofResource (png_resource.hModule, HRes);

      resptr =	(char *) LockResource (hglobal);

      if (!resptr)
        FAILMSGANDRETURN
      }

#else
      FAILMSGANDRETURN
#endif

    // first check the eight byte PNG signature
    t_size r = fread (sig, 1, 8, fp);

    if (r == 0)
    {
      nuxAssertMsg (0, TEXT ("[read_png_rgba] Png read error.") );
    }

    if (!png_check_sig (sig, 8) )
    {
      if (fp) fclose (fp);

      PNG_FREERESOURCE
      return 0;
    }


    // start back here!!!!

    // create the two png(-info) structures

    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, 0, 0, 0);

    if (!png_ptr)
    {
      if (fp) fclose (fp);

      PNG_FREERESOURCE
      return 0;
    }

    info_ptr = png_create_info_struct (png_ptr);

    if (!info_ptr)
    {
      png_destroy_read_struct (&png_ptr, 0, 0);

      if (fp) fclose (fp);

      PNG_FREERESOURCE
    }

    // initialize the png structure
    PNG_SETREADFN png_init_io (png_ptr, fp);

    png_set_sig_bytes (png_ptr, 8);

    // read all PNG info up to image data
    png_read_info (png_ptr, info_ptr);

    // get width, height, bit-depth and color-type
    unsigned long width, height;
    png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);

    // expand images of all color-type and bit-depth to 3x8 bit RGB images
    // let the library process things like alpha, transparency, background

    if (bit_depth == 16) png_set_strip_16 (png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand (png_ptr);

    if (bit_depth < 8) png_set_expand (png_ptr);

    if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS) ) png_set_expand (png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb (png_ptr);

    // if required set gamma conversion
    if (png_get_gAMA (png_ptr, info_ptr, &gamma) ) png_set_gamma (png_ptr, (double) 2.2, gamma);

    // after the transformations have been registered update info_ptr data
    png_read_update_info (png_ptr, info_ptr);

    // get again width, height and the new bit-depth and color-type
    png_get_IHDR (png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);


    // row_bytes is the width x number of channels
    row_bytes = png_get_rowbytes (png_ptr, info_ptr);
    channels = png_get_channels (png_ptr, info_ptr);

    if ( channels == 3 )
    {
      //nuxAssertMsg(0, TEXT("[read_png_rgba] Unable to open image of type RGB %s using read_png_rgba()."), filename);
      //nuxAssertMsg(0, TEXT("[read_png_rgba] Try using read_png_rgb() instead!"));
      if (fp) fclose (fp);

      return 0;
    }

    // now we can allocate memory to store the image
    png_byte *img = new png_byte[row_bytes * height];
    // and allocate memory for an array of row-pointers
    png_byte **row = new png_byte * [height];


    // set the individual row-pointers to point at the correct offsets
    for (unsigned int i = 0; i < height; i++)
      row[i] = img + i * row_bytes;

    // now we can go ahead and just read the whole image
    png_read_image (png_ptr, row);

    // read the additional chunks in the PNG file (not really needed)
    png_read_end (png_ptr, NULL);

    //image = array2<vec4ub>(w, h);
    NTextureData *TextureObjectData = new NTextureData (BITFMT_R8G8B8A8, width, height, 1);
    {
      for (unsigned int i = 0; i < width; i++)
        for (unsigned int j = 0; j < height; j++)
        {
          BYTE *png_data_pointer = img + ( (height - j - 1) * row_bytes + i * 4);
          UINT value =
            (* (png_data_pointer + 3) << 24) |  // a
            (* (png_data_pointer + 2) << 16) |  // b
            (* (png_data_pointer + 1) << 8)  |  // g
            * (png_data_pointer + 0);           // r

          TextureObjectData->GetSurface (0).Write32b (i, j, value); // = vec4ub(img + ((h-j-1)*row_bytes + i * 4));
        }
    }

    delete [] row;
    delete [] img;

    png_destroy_read_struct (&png_ptr, &info_ptr, 0);

    if (TextureObjectData)
      TextureObjectData->GetSurface (0).FlipVertical();

    if (fp) fclose (fp);

    PNG_FREERESOURCE

    return TextureObjectData;
  }

  NBitmapData *read_png_rgb (const TCHAR *filename)
  {
    //--------- Resource stuff
    PNG_DECLARE_RESVAR
    //----------
    FILE *fp;
    png_byte sig[8];
    int bit_depth, color_type;
    double              gamma;
    png_uint_32 channels, row_bytes;
    png_structp png_ptr = 0;
    png_infop info_ptr = 0;

    // open the PNG input file
    ANSICHAR *FileNameAnsi = (ANSICHAR *) TCHAR_TO_ANSI (filename);

    if (!FileNameAnsi)
    {
      nuxAssertMsg (0, TEXT ("[read_png_rgb] Incorrect file name: %s."), filename);
      return 0;
    }

#if defined(_WIN32)
    struct _stat file_info;
#else
    struct stat file_info;
#endif


    // System call: check if the file exist
#if defined(_WIN32)

    if (_stat (FileNameAnsi, &file_info) != 0)
#else
    if (stat (FileNameAnsi, &file_info) != 0)
#endif
    {
      nuxAssert (TEXT ("[read_png_rgb] File not found: %s.") );
      return 0;
    }

#if (defined NUX_VISUAL_STUDIO_2005) || (defined NUX_VISUAL_STUDIO_2008)
    fopen_s (&fp, FileNameAnsi, "rb");
#else
    fp = fopen (FileNameAnsi, "rb");
#endif

    if (!fp)
#if defined(_WIN32)
    {
      // Try resource access
      HRes = get_resource (FileNameAnsi);

      if (!HRes)
        FAILMSGANDRETURN
        hglobal	=	LoadResource (png_resource.hModule, HRes);

      if (!hglobal)
        FAILMSGANDRETURN
        ressz = SizeofResource (png_resource.hModule, HRes);

      resptr =	(char *) LockResource (hglobal);

      if (!resptr)
        FAILMSGANDRETURN
      }

#else
      FAILMSGANDRETURN
#endif

    // first check the eight byte PNG signature
    t_size r = fread (sig, 1, 8, fp);

    if (r == 0)
    {
      nuxAssertMsg (0, TEXT ("[read_png_rgba] Png read error.") );
    }

    if (!png_check_sig (sig, 8) )
    {
      if (fp) fclose (fp);

      PNG_FREERESOURCE;
      return 0;
    }

    // start back here!!!!

    // create the two png(-info) structures

    png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, 0, 0, 0);

    if (!png_ptr)
    {
      if (fp) fclose (fp);

      PNG_FREERESOURCE;
      return 0;
    }

    info_ptr = png_create_info_struct (png_ptr);

    if (!info_ptr)
    {
      png_destroy_read_struct (&png_ptr, 0, 0);

      if (fp) fclose (fp);

      PNG_FREERESOURCE
    }

    // initialize the png structure
    PNG_SETREADFN png_init_io (png_ptr, fp);

    png_set_sig_bytes (png_ptr, 8);

    // read all PNG info up to image data
    png_read_info (png_ptr, info_ptr);

    // get width, height, bit-depth and color-type
    unsigned long w, h;
    png_get_IHDR (png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);

    // expand images of all color-type and bit-depth to 3x8 bit RGB images
    // let the library process things like alpha, transparency, background

    if (bit_depth == 16) png_set_strip_16 (png_ptr);

    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand (png_ptr);

    if (bit_depth < 8) png_set_expand (png_ptr);

    if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS) ) png_set_expand (png_ptr);

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
      png_set_gray_to_rgb (png_ptr);

    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA)
      png_set_strip_alpha (png_ptr);

    // if required set gamma conversion
    if (png_get_gAMA (png_ptr, info_ptr, &gamma) ) png_set_gamma (png_ptr, (double) 2.2, gamma);

    // after the transformations have been registered update info_ptr data
    png_read_update_info (png_ptr, info_ptr);

    // get again width, height and the new bit-depth and color-type
    png_get_IHDR (png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);


    // row_bytes is the width x number of channels
    row_bytes = png_get_rowbytes (png_ptr, info_ptr);
    channels = png_get_channels (png_ptr, info_ptr);

    if ( channels == 4 )
    {
      //nuxAssertMsg(0, TEXT("[read_png_rgba] Unable to open image of type RGBA %s using read_png_rgb()."), filename);
      //nuxAssertMsg(0, TEXT("[read_png_rgba] Try using read_png_rgba() instead!"));
      if (fp) fclose (fp);

      return 0;
    }

    // now we can allocate memory to store the image

    png_byte *img = new png_byte[row_bytes * h];

    // and allocate memory for an array of row-pointers

    png_byte **row = new png_byte * [h];


    // set the individual row-pointers to point at the correct offsets

    for (unsigned int i = 0; i < h; i++)
      row[i] = img + i * row_bytes;

    // now we can go ahead and just read the whole image

    png_read_image (png_ptr, row);

    // read the additional chunks in the PNG file (not really needed)

    png_read_end (png_ptr, NULL);

    //image = array2<vec3ub>(w, h);
    NTextureData *TextureObjectData = new NTextureData (BITFMT_R8G8B8, w, h, 1);
    {
      for (unsigned int i = 0; i < w; i++)
        for (unsigned int j = 0; j < h; j++)
        {
          BYTE *png_data_pointer = img + ( (h - j - 1) * row_bytes + i * 3);

          UINT value =
            (* (png_data_pointer + 2) << 16) |  //r
            (* (png_data_pointer + 1) << 8)  |  //g
            * (png_data_pointer + 0);           //b

          TextureObjectData->GetSurface (0).Write24b (i, j, value);  // vec3ub(img + ((h-j-1)*row_bytes + i * 3));
        }
    }


    delete [] row;
    delete [] img;

    png_destroy_read_struct (&png_ptr, &info_ptr, 0);

    if (TextureObjectData)
      TextureObjectData->GetSurface (0).FlipVertical();

    if (fp) fclose (fp);

    PNG_FREERESOURCE
    return TextureObjectData;
  }


////FIXME: LIBPNG expands to RGB and only R is fetched...
//bool read_png_grey(const char * filename, glh::array2<unsigned char> & image)
//{
//    //--------- Resource stuff
//    PNG_DECLARE_RESVAR
//        //---------
//        FILE * fp;
//    png_byte sig[8];
//    int bit_depth, color_type;
//    double              gamma;
//    png_uint_32 channels, row_bytes;
//    png_structp png_ptr = 0;
//    png_infop info_ptr = 0;
//
//
//    // This is the SDK default path...
//    if(path.path.size() < 1)
//    {
//        path.path.push_back(""); // added by Ashu, for case where fully qualified path is given
//        path.path.push_back(".");
//        path.path.push_back("../../../MEDIA/textures/1D");
//        path.path.push_back("../../../../MEDIA/textures/1D");
//        path.path.push_back("../../../../../../../MEDIA/textures/1D");
//        path.path.push_back("../../../MEDIA/textures/2D");
//        path.path.push_back("../../../../MEDIA/textures/2D");
//        path.path.push_back("../../../../../../../MEDIA/textures/2D");
//        path.path.push_back("../../../MEDIA/textures/rectangles");
//        path.path.push_back("../../../../MEDIA/textures/rectangles");
//        path.path.push_back("../../../../../../../MEDIA/textures/rectangles");
//        path.path.push_back("../../../MEDIA/textures/cubemaps");
//        path.path.push_back("../../../../MEDIA/textures/cubemaps");
//        path.path.push_back("../../../../../../../MEDIA/textures/cubemaps");
//        path.path.push_back("../../../MEDIA/textures/3D");
//        path.path.push_back("../../../../MEDIA/textures/3D");
//        path.path.push_back("../../../../../../../MEDIA/textures/3D");
//    }
//
//    // open the PNG input file
//    if (!filename) return false;
//
//    if (!(fp = path.fopen(filename)))
//#if defined(_WIN32)
//    {
//        // Try resource access
//        HRes = get_resource(filename);
//
//        if(!HRes)
//            FAILMSGANDRETURN
//            hglobal	=	LoadResource(png_resource.hModule, HRes);
//        if(!hglobal)
//            FAILMSGANDRETURN
//            ressz = SizeofResource(png_resource.hModule, HRes);
//        resptr =	(char*)LockResource(hglobal);
//        if(!resptr)
//            FAILMSGANDRETURN
//    }
//#else
//        FAILMSGANDRETURN
//#endif
//
//        // first check the eight byte PNG signature
//        PNG_READHEADER fread(sig, 1, 8, fp);
//    if (!png_check_sig(sig, 8)) {
//        if(fp) fclose(fp);
//        PNG_FREERESOURCE
//            return false;
//    }
//
//
//    // start back here!!!!
//
//    // create the two png(-info) structures
//
//    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
//    if (!png_ptr) {
//        if(fp) fclose(fp);
//        PNG_FREERESOURCE
//            return false;
//    }
//
//    info_ptr = png_create_info_struct(png_ptr);
//    if (!info_ptr)
//    {
//        png_destroy_read_struct(&png_ptr, 0, 0);
//        if(fp) fclose(fp);
//        PNG_FREERESOURCE
//    }
//
//    // initialize the png structure
//    PNG_SETREADFN png_init_io(png_ptr, fp);
//
//    png_set_sig_bytes(png_ptr, 8);
//
//    // read all PNG info up to image data
//    png_read_info(png_ptr, info_ptr);
//
//    // get width, height, bit-depth and color-type
//    unsigned long w, h;
//    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);
//
//    // expand images of all color-type and bit-depth to 3x8 bit RGB images
//    // let the library process things like alpha, transparency, background
//
//    if (bit_depth == 16) png_set_strip_16(png_ptr);
//    if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
//    if (bit_depth < 8) png_set_expand(png_ptr);
//    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) png_set_expand(png_ptr);
//    if (color_type == PNG_COLOR_TYPE_GRAY ||
//        color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
//        png_set_gray_to_rgb(png_ptr);
//
//    // if required set gamma conversion
//    if (png_get_gAMA(png_ptr, info_ptr, &gamma)) png_set_gamma(png_ptr, (double) 2.2, gamma);
//
//    // after the transformations have been registered update info_ptr data
//    png_read_update_info(png_ptr, info_ptr);
//
//    // get again width, height and the new bit-depth and color-type
//    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type, 0, 0, 0);
//
//
//    // row_bytes is the width x number of channels
//    row_bytes = png_get_rowbytes(png_ptr, info_ptr);
//    channels = png_get_channels(png_ptr, info_ptr);
//
//    // now we can allocate memory to store the image
//
//    png_byte * img = new png_byte[row_bytes * h];
//
//    // and allocate memory for an array of row-pointers
//
//    png_byte ** row = new png_byte * [h];
//
//
//    // set the individual row-pointers to point at the correct offsets
//
//    for (unsigned int i = 0; i < h; i++)
//        row[i] = img + i * row_bytes;
//
//    // now we can go ahead and just read the whole image
//
//    png_read_image(png_ptr, row);
//
//    // read the additional chunks in the PNG file (not really needed)
//
//    png_read_end(png_ptr, NULL);
//
//    image = array2<unsigned char>(w, h);
//
//    {
//        for(unsigned int i=0; i < w; i++)
//            for(unsigned int j=0; j < h; j++)
//            { image(i,j) = *(img + ((h-j-1)*row_bytes + i * 3)); }
//    }
//
//    delete [] row;
//    delete [] img;
//
//    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
//
//    if(fp) fclose (fp);
//    PNG_FREERESOURCE
//
//        return true;
//}

  void png_read_resource_fn (png_structp png_ptr, png_bytep data, png_size_t leng)
  {
    png_bytep src = (png_bytep) png_ptr->io_ptr;

    for (unsigned int i = 0; i < leng; i++)
      data[i] = *src++;

    png_ptr->io_ptr = (void *) src;
  }

} //NUX_NAMESPACE_END
