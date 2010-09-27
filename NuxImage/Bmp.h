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


#ifndef BMP_H
#define BMP_H

#include "NuxCore/Error.h"
#include "ImageSurface.h"

namespace nux { //NUX_NAMESPACE_BEGIN

const unsigned int BITMAPFILEHEADER_SIZE      = 14;
const unsigned int BITMAPINFOHEADER_SIZE      = 40;

typedef struct
{
	unsigned short int      bfType; // must be "BM"
	unsigned int            bfSize;
    unsigned short int      bfReserved1;
    unsigned short int      bfReserved2;
	unsigned int            offset_to_data;
} bmp_fileheader;

typedef struct
{
	unsigned int          biSize;
	unsigned int          biWidth;
	         int          biHeight;  // height can be negative if the bmp is saved with "Flip Row Order" (see Photoshop). In this case, no need to flip at the end.
	unsigned short int    biPlanes;
	unsigned short int    biBitCount;
	unsigned int          biCompression;
	unsigned int          biSizeImage;
	unsigned int          biXPelsPerMeter;
	unsigned int          biYPelsPerMeter;
	unsigned int          biClrUsed;
	unsigned int          biClrImportant;
    unsigned int          colorpalette[256];
    unsigned int          redMask;
    unsigned int          greenMask;
    unsigned int          blueMask;
} bmp_infoheader;

typedef struct
{	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char reserved;
} bmp_colortable_element;

NBitmapData* read_bmp_file(const TCHAR* file_name);
HReport write_bmp_file(const TCHAR* file_name, NBitmapData *image);

ImageInfo get_bmp_info(const std::string& file_name);

} //NUX_NAMESPACE_END

#endif // BMP_H

/*
BITMAPFILEHEADER:
start size name             stdvalue    purpose 
1       2   bfType          19778       must always be set to 'BM' to declare that this is a .bmp-file. 
3       4   bfSize          ??          specifies the size of the file in bytes. 
7       2   bfReserved1     0           must always be set to zero. 
9       2   bfReserved2     0           must always be set to zero. 
11      4   bfOffBits       1078        specifies the offset from the beginning of the file to the bitmap data. 


BITMAPINFOHEADER:
start size  name                stdvalue    purpose 
15      4   biSize              40          specifies the size of the BITMAPINFOHEADER structure, in bytes. 
19      4   biWidth             100         specifies the width of the image, in pixels. 
23      4   biHeight            100         specifies the height of the image, in pixels. 
27      2   biPlanes            1           specifies the number of planes of the target device, must be set to zero. 
29      2   biBitCount          8           specifies the number of bits per pixel. 
31      4   biCompression       0           Specifies the type of compression, usually set to zero (no compression). 
35      4   biSizeImage         0           specifies the size of the image data, in bytes. If there is no compression, it is valid to set this member to zero. 
39      4   biXPelsPerMeter     0           specifies the the horizontal pixels per meter on the designated targer device, usually set to zero. 
43      4   biYPelsPerMeter     0           specifies the the vertical pixels per meter on the designated targer device, usually set to zero. 
47      4   biClrUsed           0           specifies the number of colors used in the bitmap, if set to zero the number of colors is calculated using the biBitCount member. 
51      4   biClrImportant      0           specifies the number of color that are 'important' for the bitmap, if set to zero, all colors are important. 
*/
