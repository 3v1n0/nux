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
#include "NuxCore/Rect.h"
#include "BitmapFormats.h"
#include "CairoGraphics.h"

namespace nux
{

  CairoGraphics::CairoGraphics(cairo_format_t format, int width, int height)
    : m_surface_format(format)
    , _width(0)
    , _height(0)
    , _zoom(1.0f)
    , _opacity(1.0f)
  {
    nuxAssert(width >= 0);
    nuxAssert(height >= 0);

    _width = width;
    _height = height;

    if (_width <= 0)
      _width = 1;

    if (_height <= 0)
      _height = 1;

    _cairo_surface = cairo_image_surface_create(m_surface_format, _width, _height);
    _cr = cairo_create(_cairo_surface);

    if (cairo_status(_cr) == CAIRO_STATUS_NO_MEMORY)
    {
      // If memory cannot be allocated, a special cairo_t object will be returned
      // on which cairo_status() returns CAIRO_STATUS_NO_MEMORY.
      // You can use this object normally, but no drawing will be done. 
      nuxAssertMsg(0, "[CairoGraphics::GetContext] Cairo context error.");
    }
  }

  CairoGraphics::~CairoGraphics()
  {
    if (_cr)
    {
      cairo_destroy(_cr);
    }
    cairo_surface_destroy(_cairo_surface);
  }

  cairo_t *CairoGraphics::GetContext()
  {
    cairo_t *cr = cairo_create(_cairo_surface);
    if (cairo_status(cr) == CAIRO_STATUS_NO_MEMORY)
    {
      // If memory cannot be allocated, a special cairo_t object will be returned
      // on which cairo_status() returns CAIRO_STATUS_NO_MEMORY.
      // You can use this object normally, but no drawing will be done. 
      nuxAssertMsg(0, "[CairoGraphics::GetContext] Cairo context error.");
    }
    return cr;
  }

  cairo_t *CairoGraphics::GetInternalContext()
  {
    return _cr;
  }

  cairo_surface_t* CairoGraphics::GetSurface()
  {
    return _cairo_surface;
  }

  NBitmapData *CairoGraphics::GetBitmap()
  {
    if ((_width <= 0) || (_height <= 0))
    {
      nuxDebugMsg("[CairoGraphics::GetBitmap] Invalid surface.");
    }

    NUX_RETURN_VALUE_IF_NULL(_width, 0);
    NUX_RETURN_VALUE_IF_NULL(_height, 0);

    cairo_surface_flush(_cairo_surface);

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

    NTextureData *bitmap_data = new NTextureData(bitmap_format, _width, _height, 1);
    unsigned char *ptr = cairo_image_surface_get_data(_cairo_surface);
    int stride = cairo_image_surface_get_stride(_cairo_surface);

    if (ptr == NULL || stride == 0)
    {
      // _cairo_surface is not a valid surface
      nuxError("[CairoGraphics::GetBitmap] Invalid surface");
      return bitmap_data; // just returns because we will segfault otherwise
    }

    unsigned char* dest_u8 = bitmap_data->GetSurface(0).GetPtrRawData();
    const int pitch = bitmap_data->GetSurface(0).GetPitch();

    if (m_surface_format == CAIRO_FORMAT_A1)
    {
      std::vector<unsigned char> temp(pitch);

      for (int j = 0; j < _height; ++j)
      {
        for (int i = 0; i < _width; ++i)
        {
          // Get the byte
          int a = ptr[j * stride + i/8];
          // Get the position in the byte
          int b = (i - 8 * (i / 8));
          // Shift the byte and get the last bit
          int c = (a >> b) & 0x1;
          // If the last bit is set, put 1, otherwise put 0
          temp[i] = c ? 0xFF : 0x0;
        }

        Memcpy(dest_u8 + j * pitch, temp.data(), _width);
      }
    }
    else
    {
      for (int j = 0; j < _height; ++j)
      {
        Memcpy(dest_u8 + j * pitch,
                (const void *) (&ptr[j * stride]),
                _width * GPixelFormats[bitmap_format].NumComponents);
      }
    }

    return bitmap_data;
  }

  int CairoGraphics::GetWidth() const
  {
    return _width;
  }

  int CairoGraphics::GetHeight() const
  {
    return _height;
  }

  bool CairoGraphics::PushState()
  {
    nuxAssert(_cr);
    _opacity_stack.push(_opacity);
    cairo_save(_cr);
    return true;
  }

  bool CairoGraphics::PopState()
  {
    nuxAssert(_cr);
    if (_opacity_stack.empty())
    {
      return false;
    }

    _opacity = _opacity_stack.top();
    _opacity_stack.pop();
    cairo_restore(_cr);
    return true;
  }

  bool CairoGraphics::ClearCanvas()
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

    cairo_restore(_cr);
    cairo_save(_cr);

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

  bool CairoGraphics::DrawLine(double x0, double y0, double x1, double y1,
                               double width, const Color &c)
  {
    nuxAssert(_cr);
    if (width < 0.0)
    {
      return false;
    }

    cairo_set_line_width(_cr, width);
    cairo_set_source_rgba(_cr, c.red, c.green, c.blue, _opacity);
    cairo_move_to(_cr, x0, y0);
    cairo_line_to(_cr, x1, y1);
    cairo_stroke(_cr);

    return true;
  }

  void CairoGraphics::TranslateCoordinates(double tx, double ty)
  {
    nuxAssert(_cr);
    cairo_translate(_cr, tx, ty);
  }

  bool CairoGraphics::DrawFilledRect(double x, double y, double w, double h,
                                     const Color &c)
  {
    nuxAssert(_cr);
    if (w <= 0.0 || h <= 0.0) {
      return false;
    }

    cairo_set_source_rgba(_cr, c.red, c.green, c.blue, _opacity);
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

  static inline double
  _align(double val)
  {
    double fract = val - (int) val;

    if (fract != 0.5f)
      return(double) ((int) val + 0.5f);
    else
      return val;
  }

  bool
  CairoGraphics::DrawRoundedRectangle(cairo_t* cr,
                                       double   aspect,
                                       double   x,
                                       double   y,
                                       double   cornerRadius,
                                       double   width,
                                       double   height,
                                       bool     align)
  {
    double radius = cornerRadius / aspect;

    if (align)
    {
      // top-left, right of the corner
      cairo_move_to(cr, _align(x + radius), _align(y));

      // top-right, left of the corner
      cairo_line_to(cr, _align(x + width - radius), _align(y));

      // top-right, below the corner
      cairo_arc(cr,
                 _align(x + width - radius),
                 _align(y + radius),
                 radius,
                 -90.0f * G_PI / 180.0f,
                 0.0f * G_PI / 180.0f);

      // bottom-right, above the corner
      cairo_line_to(cr, _align(x + width), _align(y + height - radius));

      // bottom-right, left of the corner
      cairo_arc(cr,
                 _align(x + width - radius),
                 _align(y + height - radius),
                 radius,
                 0.0f * G_PI / 180.0f,
                 90.0f * G_PI / 180.0f);

      // bottom-left, right of the corner
      cairo_line_to(cr, _align(x + radius), _align(y + height));

      // bottom-left, above the corner
      cairo_arc(cr,
                 _align(x + radius),
                 _align(y + height - radius),
                 radius,
                 90.0f * G_PI / 180.0f,
                 180.0f * G_PI / 180.0f);

      // top-left, right of the corner
      cairo_arc(cr,
                 _align(x + radius),
                 _align(y + radius),
                 radius,
                 180.0f * G_PI / 180.0f,
                 270.0f * G_PI / 180.0f);
    }
    else
    {
      // top-left, right of the corner
      cairo_move_to(cr, x + radius, y);

      // top-right, left of the corner
      cairo_line_to(cr, x + width - radius, y);

      // top-right, below the corner
      cairo_arc(cr,
                 x + width - radius,
                 y + radius,
                 radius,
                 -90.0f * G_PI / 180.0f,
                 0.0f * G_PI / 180.0f);

      // bottom-right, above the corner
      cairo_line_to(cr, x + width, y + height - radius);

      // bottom-right, left of the corner
      cairo_arc(cr,
                 x + width - radius,
                 y + height - radius,
                 radius,
                 0.0f * G_PI / 180.0f,
                 90.0f * G_PI / 180.0f);

      // bottom-left, right of the corner
      cairo_line_to(cr, x + radius, y + height);

      // bottom-left, above the corner
      cairo_arc(cr,
                 x + radius,
                 y + height - radius,
                 radius,
                 90.0f * G_PI / 180.0f,
                 180.0f * G_PI / 180.0f);

      // top-left, right of the corner
      cairo_arc(cr,
                 x + radius,
                 y + radius,
                 radius,
                 180.0f * G_PI / 180.0f,
                 270.0f * G_PI / 180.0f);
    }

    return true;
  }

  static inline void _blurinner(guchar* pixel,
                                 gint*   zR,
                                 gint*   zG,
                                 gint*   zB,
                                 gint*   zA,
                                 gint    alpha,
                                 gint    aprec,
                                 gint    zprec)
  {
    gint R;
    gint G;
    gint B;
    guchar A;

    R = *pixel;
    G = *(pixel + 1);
    B = *(pixel + 2);
    A = *(pixel + 3);

    *zR += (alpha * ((R << zprec) - *zR)) >> aprec;
    *zG += (alpha * ((G << zprec) - *zG)) >> aprec;
    *zB += (alpha * ((B << zprec) - *zB)) >> aprec;
    *zA += (alpha * ((A << zprec) - *zA)) >> aprec;

    *pixel       = *zR >> zprec;
    *(pixel + 1) = *zG >> zprec;
    *(pixel + 2) = *zB >> zprec;
    *(pixel + 3) = *zA >> zprec;
  }

  static inline void _blurrow(guchar* pixels,
                               gint    width,
                               gint    /* height */,  // TODO: This seems very strange. Why is height not used as it is in _blurcol() ?
                               gint    channels,
                               gint    line,
                               gint    alpha,
                               gint    aprec,
                               gint    zprec)
  {
    gint    zR;
    gint    zG;
    gint    zB;
    gint    zA;
    gint    index;
    guchar* scanline;

    scanline = &(pixels[line * width * channels]);

    zR = *scanline << zprec;
    zG = *(scanline + 1) << zprec;
    zB = *(scanline + 2) << zprec;
    zA = *(scanline + 3) << zprec;

    for (index = 0; index < width; index ++)
      _blurinner(&scanline[index * channels], &zR, &zG, &zB, &zA, alpha, aprec,
      zprec);

    for (index = width - 2; index >= 0; index--)
      _blurinner(&scanline[index * channels], &zR, &zG, &zB, &zA, alpha, aprec,
      zprec);
  }

  static inline void _blurcol(guchar* pixels,
                               gint    width,
                               gint    height,
                               gint    channels,
                               gint    x,
                               gint    alpha,
                               gint    aprec,
                               gint    zprec)
  {
    gint zR;
    gint zG;
    gint zB;
    gint zA;
    gint index;
    guchar* ptr;

    ptr = pixels;

    ptr += x * channels;

    zR = *((guchar*) ptr    ) << zprec;
    zG = *((guchar*) ptr + 1) << zprec;
    zB = *((guchar*) ptr + 2) << zprec;
    zA = *((guchar*) ptr + 3) << zprec;

    for (index = width; index < (height - 1) * width; index += width)
      _blurinner((guchar*) &ptr[index * channels], &zR, &zG, &zB, &zA, alpha,
      aprec, zprec);

    for (index = (height - 2) * width; index >= 0; index -= width)
      _blurinner((guchar*) &ptr[index * channels], &zR, &zG, &zB, &zA, alpha,
      aprec, zprec);
  }

  //
  // pixels   image-data
  // width    image-width
  // height   image-height
  // channels image-channels
  //
  // in-place blur of image 'img' with kernel of approximate radius 'radius'
  //
  // blurs with two sided exponential impulse response
  //
  // aprec = precision of alpha parameter in fixed-point format 0.aprec
  //
  // zprec = precision of state parameters zR,zG,zB and zA in fp format 8.zprec
  //
  void _expblur(guchar* pixels,
                 gint    width,
                 gint    height,
                 gint    channels,
                 gint    radius,
                 gint    aprec,
                 gint    zprec)
  {
    gint alpha;
    gint row = 0;
    gint col = 0;

    if (radius < 1)
      return;

    // calculate the alpha such that 90% of 
    // the kernel is within the radius.
    // (Kernel extends to infinity)
    alpha = (gint) ((1 << aprec) * (1.0f - expf(-2.3f / (radius + 1.f))));

    for (; row < height; row++)
      _blurrow(pixels, width, height, channels, row, alpha, aprec, zprec);

    for (; col < width; col++)
      _blurcol(pixels, width, height, channels, col, alpha, aprec, zprec);

    return;
  }

  // if called like BlurSurface(radius) or BlurSurface(radius, NULL) it will
  // try to blur the image-surface of the internal cairo-context
  bool CairoGraphics::BlurSurface(unsigned int radius, cairo_surface_t* surf)
  {
    cairo_surface_t* surface;
    guchar*          pixels;
    guint            width;
    guint            height;
    cairo_format_t   format;

    if (surf)
      surface = surf;
    else
      surface = cairo_get_target(_cr);

    // don't do anything if we're not dealing with an image-surface
      if (cairo_surface_get_type(surface) != CAIRO_SURFACE_TYPE_IMAGE)
      return false;

    // before we mess with the surface execute any pending drawing
    cairo_surface_flush(surface);

    pixels = cairo_image_surface_get_data(surface);
    width  = cairo_image_surface_get_width(surface);
    height = cairo_image_surface_get_height(surface);
    format = cairo_image_surface_get_format(surface);

    switch(format)
    {
      case CAIRO_FORMAT_ARGB32:
        _expblur(pixels, width, height, 4, radius, 16, 7);
      break;

      case CAIRO_FORMAT_RGB24:
        _expblur(pixels, width, height, 3, radius, 16, 7);
      break;

      case CAIRO_FORMAT_A8:
        _expblur(pixels, width, height, 1, radius, 16, 7);
      break;

      default :
        // do nothing
      break;
    }

    // inform cairo we altered the surfaces contents
    cairo_surface_mark_dirty(surface);

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
    bool do_clip = false;
    cairo_antialias_t pre = cairo_get_antialias(_cr);
    cairo_set_antialias(_cr, CAIRO_ANTIALIAS_NONE);
    
    std::list<Rect>::iterator it;
    for (it = region.begin(); it != region.end(); ++it)
    {
      Rect rect = (*it);

      if (!rect.IsNull())
      {
        cairo_rectangle(_cr, rect.x, rect.y, rect.width, rect.height);
        do_clip = true;
      }
    }

    if (do_clip)
    {
      cairo_clip(_cr);
    }

    cairo_set_antialias(_cr, pre);
    return true;
  }
}

