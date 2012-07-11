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
#include "GdiImageLoader.h"

namespace nux
{
  NBitmapData* GdiLoadImageFile(const TCHAR* filename)
  {
    ULONG_PTR token;
    Gdiplus::GdiplusStartupInput input;
    Gdiplus::GdiplusStartup(&token, &input, NULL);

    Gdiplus::Bitmap* bitmap = new Gdiplus::Bitmap(TCHAR_TO_UNICHAR(filename));
    if (bitmap == 0 || (bitmap->GetType() == Gdiplus::ImageTypeUnknown) || (bitmap->GetWidth() == 0) || (bitmap->GetHeight() == 0))
    {
      Gdiplus::GdiplusShutdown(token);
      return 0;
    }

    Gdiplus::BitmapData* bitmap_data = new Gdiplus::BitmapData;
    if (bitmap_data == 0)
    {
      delete bitmap;
      Gdiplus::GdiplusShutdown(token);
      return 0;
    }

    Gdiplus::PixelFormat pixel_format = bitmap->GetPixelFormat();
    int channels = 4;
    BitmapFormat bitmap_format = BITFMT_UNKNOWN;

    if (pixel_format == PixelFormat24bppRGB)
    {
      bitmap_format = BITFMT_B8G8R8;
      channels = 3;
    }
    else if ((pixel_format == PixelFormat32bppARGB) ||
      (pixel_format == PixelFormat32bppPARGB) ||
      (pixel_format == PixelFormat32bppRGB))
    {
      if (pixel_format == PixelFormat32bppARGB)
        pixel_format = PixelFormat32bppPARGB; // request conversion to pre-multiplied RGBA

      bitmap_format = BITFMT_B8G8R8A8;
      channels = 4;
    }
    else if ((pixel_format == PixelFormat16bppARGB1555) ||
      (pixel_format == PixelFormat16bppGrayScale) ||
      (pixel_format == PixelFormat16bppRGB555) ||
      (pixel_format == PixelFormat16bppRGB565))
    {
      pixel_format = PixelFormat24bppRGB;   // request conversion to RGB
      bitmap_format = BITFMT_B8G8R8;
      channels = 3;
    }
    else if ((pixel_format == PixelFormat1bppIndexed) ||
      (pixel_format == PixelFormat4bppIndexed) ||
      (pixel_format == PixelFormat8bppIndexed))
    {
      pixel_format = PixelFormat32bppARGB;  // request conversion to RGBA
      bitmap_format = BITFMT_B8G8R8A8;
      channels = 4;
    }
    else if (pixel_format == PixelFormat48bppRGB)
    {
      pixel_format = PixelFormat24bppRGB;  // request conversion to RGBA
      bitmap_format = BITFMT_B8G8R8;
      channels = 3;
    }
    else if ((pixel_format == PixelFormat64bppARGB) ||
      (pixel_format == PixelFormat64bppPARGB))
    {
      pixel_format = PixelFormat32bppARGB;  // request conversion to RGBA
      bitmap_format = BITFMT_B8G8R8A8;
      channels = 4;
    }

    Gdiplus::Rect rect(0, 0, bitmap->GetWidth(), bitmap->GetHeight());

    Gdiplus::Status status = bitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, pixel_format, bitmap_data);
  
    if (status != Gdiplus::Ok)
    {
      delete bitmap_data;
      delete bitmap;
      Gdiplus::GdiplusShutdown(token);
      return 0;
    }
    
    unsigned int width = bitmap->GetWidth();
    unsigned int height = bitmap->GetHeight();

    NTextureData *Texture = NULL;
    Texture = new NTextureData(bitmap_format, width, height, 1);
    
    if (Texture == 0)
    {
      bitmap->UnlockBits(bitmap_data);
      delete bitmap_data;
      delete bitmap;
      Gdiplus::GdiplusShutdown(token);
      return 0;
    }

    ImageSurface &image_surface = Texture->GetSurface(0);
    unsigned char* dest = image_surface.GetPtrRawData();
    int dest_pitch = image_surface.GetPitch();

    unsigned char* dst = dest;
    unsigned char *src = (unsigned char*) bitmap_data->Scan0;
    unsigned int src_pitch = bitmap_data->Stride;

    for (unsigned int i = 0; i < height; i++)
    {
      Memcpy(dst, src + i*src_pitch, width*channels);
      dst += dest_pitch;
    }

    bitmap->UnlockBits(bitmap_data);
    delete bitmap_data;
    delete bitmap;

    Gdiplus::GdiplusShutdown(token);

    return Texture;
  }
}
