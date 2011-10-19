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


#ifndef RGBE_H
#define RGBE_H

/* THIS CODE CARRIES NO GUARANTEE OF USABILITY OR FITNESS FOR ANY PURPOSE.
* WHILE THE AUTHORS HAVE TRIED TO ENSURE THE PROGRAM WORKS CORRECTLY,
* IT IS STRICTLY USE AT YOUR OWN RISK.  */

/* utility for reading and writing Ward's rgbe image format.
See rgbe.txt file for more details.
*/

#include "NuxCore/NuxCore.h"
#include "ImageSurface.h"

#include <iostream>
#include <fstream>
#include <stdio.h>

namespace nux
{
  typedef struct
  {
    int valid;            /* indicate which fields are valid */
    char programtype[16]; /* listed at beginning of file to identify it
                          * after "#?".  defaults to "RGBE" */
    float gamma;          /* image has already been gamma corrected with
                          * given gamma.  defaults to 1.0(no correction) */
    float exposure;       /* a value of 1.0 in an image corresponds to
                          * <exposure> watts/steradian/m^2.
                          * defaults to 1.0 */
  } rgbe_header_info;

  /* flags indicating which fields in an rgbe_header_info are valid */
#define RGBE_VALID_PROGRAMTYPE 0x01
#define RGBE_VALID_GAMMA       0x02
#define RGBE_VALID_EXPOSURE    0x04

  /* return codes for rgbe routines */
#define RGBE_RETURN_SUCCESS 0
#define RGBE_RETURN_FAILURE -1

  /* read or write headers */
  /* you may set rgbe_header_info to null if you want to */
//int RGBEWriteHeader(FILE *fp, int width, int height, rgbe_header_info *info);
  int RGBEWriteHeader(std::ofstream &fileStream, int width, int height, rgbe_header_info *info);
  int RGBEReadHeader(std::ifstream &fileStream, int &width, int &height, rgbe_header_info &info);

  /* read or write pixels */
  /* can read or write pixels in chunks of any size including single pixels*/
  int RGBEWritePixels(FILE *fp, float *data, int numpixels);
  int RGBEReadPixels(FILE *fp, float *data, int numpixels);

  /* read or write run length encoded files */
  /* must be called to read or write whole scanlines */
  int RGBEWritePixels_RLE(FILE *fp, float *data, int scanline_width,
                           int num_scanlines);
  NBitmapData *LoadRGBE(const TCHAR *filename);

}
#endif // RGBE_H

