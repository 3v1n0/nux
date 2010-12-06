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


#include "NuxCore/NuxCore.h"
#include "BitmapFormats.h"
#include "CairoGraphics.h"

namespace nux
{

  CairoGraphics::CairoGraphics (cairo_format_t format, int width, int height)
    :   m_width (0)
    ,   m_height (0)
  {
    nuxAssert (width >= 0);
    nuxAssert (height >= 0);

    m_width = width;
    m_height = height;

    if (m_width <= 0)
      m_width = 1;

    if (m_height <= 0)
      m_height = 1;

    m_cairo_surface = cairo_image_surface_create (format, m_width, m_height);
    m_surface_format = format;
  }

  CairoGraphics::~CairoGraphics()
  {
    cairo_surface_destroy (m_cairo_surface);
  }

  cairo_t *CairoGraphics::GetContext()
  {
    cairo_t *cr = cairo_create (m_cairo_surface);

    if (cairo_status (cr) == CAIRO_STATUS_NO_MEMORY)
    {
      nuxAssertMsg (0, TEXT ("[CairoGraphics::GetContext] Cairo context error.") );
    }

    return cr;
  }

  cairo_surface_t* CairoGraphics::GetSurface ()
  {
    return m_cairo_surface;
  }

  NBitmapData *CairoGraphics::GetBitmap()
  {
    if ( (m_width <= 0) || (m_height <= 0) )
    {
      nuxDebugMsg (TEXT ("[CairoGraphics::GetBitmap] Invalid surface.") );
    }

    NUX_RETURN_VALUE_IF_NULL (m_width, 0);
    NUX_RETURN_VALUE_IF_NULL (m_height, 0);

    BitmapFormat bitmap_format = BITFMT_UNKNOWN;

    if (m_surface_format == CAIRO_FORMAT_ARGB32)
    {
      // Each pixel is a 32-bit quantity, with alpha in the upper 8 bits,
      // then red, then green, then blue. The 32-bit quantities are stored native-endian.
      // Pre-multiplied alpha is used. (That is, 50% transparent red is 0x80800000, not 0x80ff0000.)
      bitmap_format = BITFMT_B8G8R8A8;
    }

    if (m_surface_format == CAIRO_FORMAT_RGB24)
    {
      // Each pixel is a 32-bit quantity, with the upper 8 bits unused.
      // Red, Green, and Blue are stored in the remaining 24 bits in that order.
      bitmap_format = BITFMT_B8G8R8A8;
    }

    if (m_surface_format == CAIRO_FORMAT_A8)
    {
      // Each pixel is a 8-bit quantity holding an alpha value.
      bitmap_format = BITFMT_A8;
    }

    if (m_surface_format == CAIRO_FORMAT_A1)
      bitmap_format = BITFMT_A8;

    NTextureData *bitmap_data = new NTextureData (bitmap_format, m_width, m_height, 1);
    t_u8 *ptr = cairo_image_surface_get_data (m_cairo_surface);
    int stride = cairo_image_surface_get_stride (m_cairo_surface);

    if (m_surface_format == CAIRO_FORMAT_A1)
    {
      t_u8 *temp = new t_u8[bitmap_data->GetSurface (0).GetPitch() ];

      for (int j = 0; j < m_height; j++)
      {
        for (int i = 0; i < m_width; i++)
        {
          // Get the byte
          int a = ptr[j * stride + i/8];
          // Get the position in the byte
          int b = (i - 8 * (i / 8) );
          // Shift the byte and get the last bit
          int c = (a >> b) & 0x1;
          // If the last bit is set, put 1, otherwise put 0
          temp[i] = c ? 0xFF : 0x0;
        }

        Memcpy ( bitmap_data->GetSurface (0).GetPtrRawData() + j * bitmap_data->GetSurface (0).GetPitch(),
                 (const void *) (&temp[0]),
                 m_width);
      }
    }
    else
    {
      for (int j = 0; j < m_height; j++)
      {
        Memcpy (bitmap_data->GetSurface (0).GetPtrRawData() + j * bitmap_data->GetSurface (0).GetPitch(),
                (const void *) (&ptr[j * stride]),
                m_width * GPixelFormats[bitmap_format].NumComponents);
      }
    }

    return bitmap_data;
  }

}
