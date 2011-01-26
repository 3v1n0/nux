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
#include "NuxCore/Rect.h"
#include "BitmapFormats.h"
#include "CairoGraphics.h"

namespace nux
{

  CairoGraphics::CairoGraphics (cairo_format_t format, int width, int height)
    :   _width (0)
    ,   _height (0)
  {
    nuxAssert (width >= 0);
    nuxAssert (height >= 0);

    _width = width;
    _height = height;

    if (_width <= 0)
      _width = 1;

    if (_height <= 0)
      _height = 1;

    _cairo_surface = cairo_image_surface_create (format, _width, _height);
    m_surface_format = format;

    _cr = cairo_create (_cairo_surface);
    if (cairo_status (_cr) == CAIRO_STATUS_NO_MEMORY)
    {
      nuxAssertMsg (0, TEXT ("[CairoGraphics::GetContext] Cairo context error.") );
    }

    _opacity = 1.0f;
    _zoom = 1.0;
  }

  CairoGraphics::~CairoGraphics()
  {
    cairo_destroy (_cr);
    cairo_surface_destroy (_cairo_surface);
  }

  cairo_t *CairoGraphics::GetContext()
  { 
    cairo_t *cr = cairo_create (_cairo_surface);
    if (cairo_status (cr) == CAIRO_STATUS_NO_MEMORY)
    {
      nuxAssertMsg (0, TEXT ("[CairoGraphics::GetContext] Cairo context error.") );
    }
    return cr;
  }

  cairo_surface_t* CairoGraphics::GetSurface ()
  {
    return _cairo_surface;
  }

  NBitmapData *CairoGraphics::GetBitmap()
  {
    if ( (_width <= 0) || (_height <= 0) )
    {
      nuxDebugMsg (TEXT ("[CairoGraphics::GetBitmap] Invalid surface.") );
    }

    NUX_RETURN_VALUE_IF_NULL (_width, 0);
    NUX_RETURN_VALUE_IF_NULL (_height, 0);

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

    NTextureData *bitmap_data = new NTextureData (bitmap_format, _width, _height, 1);
    t_u8 *ptr = cairo_image_surface_get_data (_cairo_surface);
    int stride = cairo_image_surface_get_stride (_cairo_surface);

    if (m_surface_format == CAIRO_FORMAT_A1)
    {
      t_u8 *temp = new t_u8[bitmap_data->GetSurface (0).GetPitch() ];

      for (int j = 0; j < _height; j++)
      {
        for (int i = 0; i < _width; i++)
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
                 _width);
      }
    }
    else
    {
      for (int j = 0; j < _height; j++)
      {
        Memcpy (bitmap_data->GetSurface (0).GetPtrRawData() + j * bitmap_data->GetSurface (0).GetPitch(),
                (const void *) (&ptr[j * stride]),
                _width * GPixelFormats[bitmap_format].NumComponents);
      }
    }

    return bitmap_data;
  }

  int CairoGraphics::GetWidth () const
  {
    return _width;
  }

  int CairoGraphics::GetHeight () const
  {
    return _height;
  }

  bool CairoGraphics::PushState ()
  {
    return true;
  }

  bool CairoGraphics::PopState ()
  {
    return true;
  }

  bool CairoGraphics::ClearCanvas ()
  {
    // Clear the surface.
    nuxAssert(_cr);
    cairo_operator_t op = cairo_get_operator(_cr);
    cairo_set_operator(_cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(_cr);
    cairo_set_operator(_cr, op);

    // Set the clip region to an infinitely large shape containing the target.
    cairo_reset_clip(_cr);

    _opacity = 1.0f;
    _opacity_stack = std::stack<float>();

    //cairo_restore(_cr);
    //cairo_save(_cr);

    return true;
  }

  bool CairoGraphics::ClearRect(double x, double y, double w, double h)
  {
    nuxAssert(_cr);
    cairo_rectangle(_cr, x, y, w, h);
    cairo_operator_t op = cairo_get_operator(_cr);
    cairo_set_operator(_cr, CAIRO_OPERATOR_CLEAR);
    cairo_fill(_cr);
    cairo_set_operator(_cr, op);
    return true;
  }

  bool CairoGraphics::DrawLine(double x0, double y0, double x1, double y1, double width, const Color &c)
  {
    nuxAssert(_cr);
    if (width < 0.0)
    {
      return false;
    }

    cairo_set_line_width(_cr, width);
    cairo_set_source_rgba(_cr, c.R(), c.G(), c.B(), _opacity);
    cairo_move_to(_cr, x0, y0);
    cairo_line_to(_cr, x1, y1);
    cairo_stroke(_cr);

    return true;
  }

  bool CairoGraphics::DrawFilledRect(double x, double y, double w, double h, const Color &c)
  {
    nuxAssert(_cr);
    if (w <= 0.0 || h <= 0.0) {
      return false;
    }

    cairo_set_source_rgba(_cr, c.R(), c.G(), c.B(), _opacity);
    cairo_rectangle(_cr, x, y, w, h);
    cairo_fill(_cr);
    return true;
  }

  bool CairoGraphics::DrawCanvas(double x, double y, CairoGraphics *cg)
  {
    if (cg == 0) return false;

    cairo_surface_t *s = cg->GetSurface();
    double src_zoom = cg->_zoom;
    double inv_zoom = 1.0 / src_zoom;

    cairo_save(_cr);

    IntersectRectClipRegion(x, y, cg->GetWidth(), cg->GetHeight());

    cairo_scale(_cr, inv_zoom, inv_zoom);
    cairo_set_source_surface(_cr, s, x * src_zoom, y * src_zoom);

    cairo_pattern_set_extend(cairo_get_source(_cr), CAIRO_EXTEND_PAD);

    cairo_paint_with_alpha(_cr, _opacity);
    cairo_restore(_cr);

    return true;
  }

  bool CairoGraphics::IntersectRectClipRegion(double x, double y, double w, double h)
  {
    if (w <= 0.0 || h <= 0.0) {
      return false;
    }

    cairo_antialias_t pre = cairo_get_antialias(_cr);
    cairo_set_antialias(_cr, CAIRO_ANTIALIAS_NONE);
    cairo_rectangle(_cr, x, y, w, h);
    cairo_clip(_cr);
    cairo_set_antialias(_cr, pre);
    return true;
  }

  bool CairoGraphics::IntersectGeneralClipRegion(std::list<Rect> &region)
  {
    cairo_antialias_t pre = cairo_get_antialias(_cr);
    cairo_set_antialias(_cr, CAIRO_ANTIALIAS_NONE);
    
    std::list<Rect>::iterator it;
    for (it = region.begin(); it != region.end(); it++)
    {
      float x = (*it).x;
      float y = (*it).y;
      float w = (*it).width;
      float h = (*it).height;

      cairo_rectangle(_cr, x, y, w, h);
    }
    cairo_set_antialias(_cr, pre);
    return true;
  }


}
