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


#include <sys/stat.h>
#include <iostream>
#include <fstream>

#include "NuxCore/NKernel.h"
#include "BitmapFormats.h"
#include "DDS.h"

const char *result_string_pointer = "SOIL initialized";

// compressed texture types
static const unsigned long FOURCC_DXT1 = 0x31545844l; //(MAKEFOURCC('D','X','T','1'))
static const unsigned long FOURCC_DXT3 = 0x33545844l; //(MAKEFOURCC('D','X','T','3'))
static const unsigned long FOURCC_DXT5 = 0x35545844l; //(MAKEFOURCC('D','X','T','5'))


NAMESPACE_BEGIN

NBitmapData* Load_DDS_File(const TCHAR *filename)
{
    std::ifstream file;

#if defined(_WIN32)
    struct _stat file_info;
#else
    struct stat file_info;
#endif

    // System call: check if the file exist
#if defined(_WIN32)
    if(_stat(filename, &file_info) != 0)
#else
    if(stat(filename, &file_info) != 0)
#endif
    {
        nuxAssertMsg(0, TEXT("[Load_DDS_File] File not found."));
        return 0;
    }

    file.open(filename, std::ifstream::binary);        // open as binary
    if(!file.is_open())
    {
        nuxAssertMsg(0, TEXT("[Load_DDS_File] Cannot open file."));
        return 0;
    }

    BYTE* buffer;
    int buffer_length;

    file.seekg(0, std::ios_base::end);
    buffer_length = file.tellg();
    file.seekg(0, std::ios_base::beg);
    buffer = new BYTE[buffer_length];
    file.read((char*)buffer, buffer_length);
    if(file.fail() || file.eof())
    {
        nuxAssertMsg(0, TEXT("[Load_DDS_File] Cannot read the expected content size."));
        return 0;
    }

    /*	variables	*/
    DDS_header header;
    unsigned int buffer_index = 0;
    /*	file reading variables	*/
    BitmapFormat DDSFormat = BITFMT_UNKNOWN;
    unsigned int DDS_main_size;
    unsigned int DDS_full_size;
    unsigned int width, height;
    int mipmaps = 0;
    int uncompressed;
    int block_size = 16;
    bool isCubemap = false;
    bool isVolume = false;
    unsigned int flag;
    unsigned int cf_target;
    unsigned int ogl_target_start = 0;
    unsigned int ogl_target_end = 0;
    int VolumeDepth = 0;

    NBitmapData* BitmapData = 0;

    /*	1st off, does the filename even exist?	*/
    if( NULL == buffer )
    {
        /*	we can't do it!	*/
        result_string_pointer = "NULL buffer";
        return 0;
    }
    if(buffer_length < (int) sizeof(DDS_header))
    {
        /*	we can't do it!	*/
        result_string_pointer = "DDS file was too small to contain the DDS header";
        return 0;
    }
    /*	try reading in the header	*/
    memcpy ( (void*)(&header), (const void *)buffer, sizeof( DDS_header ) );
    buffer_index = sizeof( DDS_header );
    /*	guilty until proven innocent	*/
    result_string_pointer = "Failed to read a known DDS header";
    /*	validate the header (warning, "goto"'s ahead, shield your eyes!!)	*/
    flag = ('D'<<0)|('D'<<8)|('S'<<16)|(' '<<24);
    if( header.dwMagic != flag ) {goto quick_exit;}
    if( header.dwSize != 124 ) {goto quick_exit;}

    /*	I need all of these	*/
    flag = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
    if( (header.dwFlags & flag) != flag ) {goto quick_exit;}
    /*	According to the MSDN spec, the dwFlags should contain
    DDSD_LINEARSIZE if it's compressed, or DDSD_PITCH if
    uncompressed.  Some DDS writers do not conform to the
    spec, so I need to make my reader more tolerant	*/
    /*	I need one of these	*/

    // DDPF_RGB - Texture contains uncompressed RGB data; dwRGBBitCount and the RGB masks (dwRBitMask, dwRBitMask, dwRBitMask) contain valid data.
    // DDPF_FOURCC - Texture contains compressed RGB data; dwFourCC contains valid data.
    flag = DDPF_FOURCC | DDPF_RGB;
    if( (header.sPixelFormat.dwFlags & flag) == 0 ) {goto quick_exit;}
    if( (header.sPixelFormat.dwRGBAlphaBitMask & DDPF_ALPHAPIXELS))
    {
        // texture has valid alpha data.
        // check dwRGBAlphaBitMask for alpha mask
    }
    if( header.sPixelFormat.dwSize != 32 ) {goto quick_exit;}
    if( (header.sCaps.dwCaps1 & DDSCAPS_TEXTURE) == 0 ) {goto quick_exit;}
    /*	make sure it is a type we can upload */
    if( (header.sPixelFormat.dwFlags & DDPF_FOURCC) &&
        !(
        (header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('1'<<24))) ||
        (header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('2'<<24))) ||
        (header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('3'<<24))) ||
        (header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('4'<<24))) ||
        (header.sPixelFormat.dwFourCC == (('D'<<0)|('X'<<8)|('T'<<16)|('5'<<24)))
        ) )
    {
        goto quick_exit;
    }
    /*	OK, validated the header, let's load the image data	*/
    result_string_pointer = "DDS header loaded and validated";
    width = header.dwWidth;
    height = header.dwHeight;
    //uncompressed = 1 - (header.sPixelFormat.dwFlags & DDPF_FOURCC) / DDPF_FOURCC;
    if(header.sPixelFormat.dwFlags & DDPF_FOURCC)
        uncompressed = 0;
    else
        uncompressed = 1;
    
    isCubemap = (header.sCaps.dwCaps2 & DDSCAPS2_CUBEMAP) / DDSCAPS2_CUBEMAP;
    isVolume = (header.sCaps.dwCaps2 & DDSCAPS2_VOLUME);

    if( uncompressed )
    {
        if( header.sPixelFormat.dwRGBBitCount == 32 )
        {
            if((header.sPixelFormat.dwRBitMask          & 0x00FF0000) &&
                (header.sPixelFormat.dwGBitMask         & 0x0000FF00) &&
                (header.sPixelFormat.dwBBitMask         & 0x000000FF) &&
                (header.sPixelFormat.dwRGBAlphaBitMask  & 0xFF000000))
                DDSFormat = BITFMT_B8G8R8A8;

            else if((header.sPixelFormat.dwRBitMask     & 0x000000FF) &&
                (header.sPixelFormat.dwGBitMask         & 0x0000FF00) &&
                (header.sPixelFormat.dwBBitMask         & 0x00FF0000) &&
                (header.sPixelFormat.dwRGBAlphaBitMask  & 0xFF000000))
                DDSFormat = BITFMT_R8G8B8A8;

            else if((header.sPixelFormat.dwRBitMask     & 0xFF000000) &&
                (header.sPixelFormat.dwGBitMask         & 0x00FF0000) &&
                (header.sPixelFormat.dwBBitMask         & 0x0000FF00) &&
                (header.sPixelFormat.dwRGBAlphaBitMask  & 0x000000FF))
                DDSFormat = BITFMT_A8B8G8R8;

            else if((header.sPixelFormat.dwRBitMask     & 0x0000FF00) &&
                (header.sPixelFormat.dwGBitMask         & 0x00FF0000) &&
                (header.sPixelFormat.dwBBitMask         & 0xFF000000) &&
                (header.sPixelFormat.dwRGBAlphaBitMask  & 0x000000FF))
                DDSFormat = BITFMT_A8R8G8B8;
            
            else if((header.sPixelFormat.dwRBitMask     & 0x00FF0000) &&
                (header.sPixelFormat.dwGBitMask         & 0x0000FF00) &&
                (header.sPixelFormat.dwBBitMask         & 0x000000FF))
                DDSFormat = BITFMT_B8G8R8A8;       // DirectX X8R8G8B8

            else if((header.sPixelFormat.dwRBitMask     & 0x000000FF) &&
                (header.sPixelFormat.dwGBitMask         & 0x0000FF00) &&
                (header.sPixelFormat.dwBBitMask         & 0x00FF0000))
                DDSFormat = BITFMT_R8G8B8A8;       // DirexctX X8B8G8R8

            block_size = 4;
        }
        else if( header.sPixelFormat.dwRGBBitCount == 24 )
        {
            if((header.sPixelFormat.dwRBitMask          & 0x00FF0000) &&
                (header.sPixelFormat.dwGBitMask         & 0x0000FF00) &&
                (header.sPixelFormat.dwBBitMask         & 0x000000FF) )
                DDSFormat = BITFMT_B8G8R8;

            else if((header.sPixelFormat.dwRBitMask     & 0x000000FF) &&
                (header.sPixelFormat.dwGBitMask         & 0x0000FF00) &&
                (header.sPixelFormat.dwBBitMask         & 0x00FF0000) )
                DDSFormat = BITFMT_R8G8B8;

            block_size = 3;
        }
        else if( header.sPixelFormat.dwRGBBitCount == 16 )
        {
            DDSFormat = BITFMT_R5G6B5;
            block_size = 2;
        }
        else if( header.sPixelFormat.dwRGBBitCount == 16 )
        {
            DDSFormat = BITFMT_A8;
            block_size = 1;
        }
        DDS_main_size = width * height * block_size;
    }
    else
    {
        /*	well, we know it is DXT1/3/5, because we checked above	*/
        switch( (header.sPixelFormat.dwFourCC >> 24) - '0' )
        {
        case 1:
            DDSFormat = BITFMT_DXT1;
            block_size = 8;
            break;
        case 2:
            DDSFormat = BITFMT_DXT2;
            block_size = 16;
            break;
        case 3:
            DDSFormat = BITFMT_DXT3;
            block_size = 16;
            break;
        case 4:
            DDSFormat = BITFMT_DXT4;
            block_size = 16;
            break;
        case 5:
            DDSFormat = BITFMT_DXT5;
            block_size = 16;
            break;
        }
        DDS_main_size = ((width+3)>>2)*((height+3)>>2)*block_size;
    }

    if(isCubemap)
    {
        ogl_target_start = 0;
        ogl_target_end =   6;
        BitmapData = (NBitmapData*) new NCubemapData(DDSFormat, width, height, (header.dwMipMapCount? header.dwMipMapCount:1));
    }
    else if(isVolume)
    {
        int slice = header.dwDepth;
        VolumeDepth =   slice;
        BitmapData = (NBitmapData*) new NVolumeData(DDSFormat, width, height, slice, (header.dwMipMapCount? header.dwMipMapCount:1));
    }
    else
    {
        ogl_target_start = 0;
        ogl_target_end =   1;
        BitmapData = (NBitmapData*) new NTextureData(DDSFormat, width, height, (header.dwMipMapCount? header.dwMipMapCount:1));
    }

    if( (header.sCaps.dwCaps1 & DDSCAPS_MIPMAP) && (header.dwMipMapCount > 1) )
    {
        mipmaps = header.dwMipMapCount;
    }
    else if(header.dwMipMapCount == 1)
    {
        mipmaps = header.dwMipMapCount;
        DDS_full_size = DDS_main_size;
    }
    else if(header.dwMipMapCount == 0)
    {
        mipmaps = 1;
        DDS_full_size = DDS_main_size;
    }  

    if(isVolume)
    {
        for(t_s32 mip = 0; mip < mipmaps; ++mip )
        {
            t_s32 pitch = ImageSurface::GetLevelPitchNoMemAlignment(DDSFormat, width, height, mip);
            t_s32 blockheight = ImageSurface::GetLevelBlockHeight(DDSFormat, height, mip);

            for(t_s32 s = 0; s < ImageSurface::GetLevelDim(DDSFormat, VolumeDepth, mip); s++ )
            {
                for(t_s32 b = 0; b < blockheight; b++)
                {
                    Memcpy( BitmapData->GetSurface(mip, s).GetPtrRawData() + b * pitch,
                        (const void*)(&buffer[buffer_index + b * pitch]),
                        pitch);
                }

                //BitmapData->GetSurface(mip, s).FlipVertical();
                buffer_index += blockheight * pitch;
            }
        }
    }
    else
    {
        for( cf_target = ogl_target_start; cf_target < ogl_target_end; ++cf_target )
        {
            for( int mip = 0; mip < mipmaps; ++mip )
            {
                int pitch = ImageSurface::GetLevelPitchNoMemAlignment(DDSFormat, width, height, mip);
                int blockheight = ImageSurface::GetLevelBlockHeight(DDSFormat, height, mip);

                for(int b = 0; b < blockheight; b++)
                {
                    Memcpy( BitmapData->GetSurface(cf_target, mip).GetPtrRawData() + b * pitch,
                        (const void*)(&buffer[buffer_index + b * pitch]),
                        pitch);
                }

                //BitmapData->GetSurface(cf_target, mip).FlipVertical();
                buffer_index += blockheight * pitch;
            }
        }
    }

quick_exit:
    /*	report success or failure	*/

    return BitmapData;
}

NAMESPACE_END
