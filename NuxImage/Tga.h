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


#ifndef TGA_H
#define TGA_H

#include "NuxCore/Error.h"
#include "ImageSurface.h"

NAMESPACE_BEGIN

#define TGA_SIGNATURE "TRUEVISION-XFILE" 
#define TGA_COLORMAP        1
#define TGA_COLOR           2
#define TGA_GRAYSCALE       3
#define TGA_RLE_COLORMAP    9
#define TGA_RLE_COLOR       10
#define TGA_RLE_GRAYSCALE   11

#define TGA_HEADER_SIZE     18
#define TGA_FOOTER_SIZE     26
#define TGA_SIGNATURE_SIZE  16


const unsigned int TGA_ORI_TOPLEFT  =   0x20;
const unsigned int TGA_ORI_TOPRIGHT =   0x30;
const unsigned int TGA_ORI_BOTLEFT  =   0x00;
const unsigned int TGA_ORI_BOTRIGHT =   0x10;

typedef struct
{
  unsigned char id_length;
  unsigned char colormap_type;
  unsigned char image_type;
  unsigned char colormap_index_lo, colormap_index_hi;
  unsigned char colormap_length_lo, colormap_length_hi;
  unsigned char colormap_size;
  unsigned char x_origin_lo, x_origin_hi;
  unsigned char y_origin_lo, y_origin_hi;
  unsigned char width_lo, width_hi;
  unsigned char height_lo, height_hi;
  unsigned char bpp;
  unsigned char descriptor; 
} tga_header;

typedef struct 
{ 
  unsigned int extensionAreaOffset; 
  unsigned int developerDirectoryOffset; 
  char signature[16]; 
  char dot; 
  char null; 
} tga_footer; 

NBitmapData* read_tga_file(const TCHAR* file_name);
HReport write_tga_file(const TCHAR* file_name, NBitmapData *image);

NAMESPACE_END

#endif // TGA_H
