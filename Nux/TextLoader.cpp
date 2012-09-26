// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Jason Smith <jason.smith@canonical.com>
 */

#include "TextLoader.h"
#include "NuxGraphics/CairoGraphics.h"

#include <cairo/cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>

namespace nux {

struct TextLoader::Impl
{
  Impl(TextLoader* parent);
  ~Impl();

  void RasterizeText(void* cairo_context, Color color);
  Size ComputeTextSize();
  std::string GetPangoFontName();

  TextLoader* parent_;
  float padding_x_;
  float padding_y_;
};

TextLoader::Impl::Impl(TextLoader* parent)
  : parent_(parent)
  , padding_x_(0)
  , padding_y_(0)
{

}

TextLoader::Impl::~Impl()
{

}

std::string TextLoader::Impl::GetPangoFontName()
{
  std::ostringstream os;
  os << parent_->font_name() << " " << parent_->font_size();
  return std::string(os.str());
}

Size TextLoader::Impl::ComputeTextSize()
{
  cairo_surface_t*      pango_surface = NULL;
  cairo_t*              cairo_ctx     = NULL;
  PangoLayout*          pango_layout  = NULL;
  PangoFontDescription* font_desc     = NULL;
  PangoContext*         pango_ctx     = NULL;
  PangoRectangle        ink_rect      = {0, 0, 0, 0};
  PangoRectangle        logical_rect  = {0, 0, 0, 0};
  int                   dpi           = 96;

  std::string pango_font_name = GetPangoFontName();

  // Create Cairo surface.
  pango_surface = cairo_image_surface_create(CAIRO_FORMAT_A1, 1, 1);
  
  // Create Cairo context.
  cairo_ctx = cairo_create(pango_surface);
  
  // Create layout.
  pango_layout = pango_cairo_create_layout(cairo_ctx);
  {
    pango_layout_set_wrap     (pango_layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_ellipsize(pango_layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_markup   (pango_layout, parent_->text().c_str(), -1);
    pango_layout_set_height   (pango_layout, -parent_->lines());

    // Sets the width to which the lines of the PangoLayout should wrap or ellipsized. The default value is -1: no width set.
    pango_layout_set_width(pango_layout, parent_->width * PANGO_SCALE);
  }

  // Create font description: "[FAMILY-LIST] [STYLE-OPTIONS] [SIZE]"
  font_desc = pango_font_description_from_string(pango_font_name.c_str());
  {
    pango_font_description_set_weight(font_desc, PANGO_WEIGHT_NORMAL);
    pango_layout_set_font_description(pango_layout, font_desc);
  }

  // Get Pango context
  pango_ctx = pango_layout_get_context(pango_layout); // is not ref'ed

  // Set font options
  CairoFontOptions font_options;
  {
    cairo_font_options_set_antialias      (font_options, CAIRO_ANTIALIAS_DEFAULT);
    cairo_font_options_set_subpixel_order(font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
    cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
    cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);
    cairo_set_font_options(cairo_ctx, font_options);

    pango_cairo_context_set_font_options(pango_ctx, font_options);
  }

  // use some default DPI-value
  pango_cairo_context_set_resolution(pango_ctx, dpi);

  pango_layout_context_changed(pango_layout);
  pango_layout_get_extents(pango_layout, &ink_rect, &logical_rect);

  int text_width = std::ceil((float)logical_rect.width / PANGO_SCALE);
  int text_height = std::ceil((float)logical_rect.height / PANGO_SCALE);

  padding_x_ = text_width - logical_rect.width / PANGO_SCALE;
  padding_y_ = text_height - logical_rect.height / PANGO_SCALE;

  text_width = std::min<int>(text_width, parent_->width());

  // clean up
  pango_font_description_free(font_desc);
  g_object_unref(pango_layout);
  cairo_destroy(cairo_ctx);
  cairo_surface_destroy(pango_surface);

  return Size(text_width, text_height);
}

void TextLoader::Impl::RasterizeText(void* cairo_context, Color /* color */)
{
  cairo_t* cairo_ctx = (cairo_t*) cairo_context;

  PangoLayout*          pango_layout  = NULL;
  PangoFontDescription* font_desc     = NULL;
  PangoContext*         pango_ctx     = NULL;
  int                   dpi            = 96;

  // Create layout.
  pango_layout = pango_cairo_create_layout(cairo_ctx);
  {
    pango_layout_set_wrap     (pango_layout, PANGO_WRAP_WORD_CHAR);
    pango_layout_set_ellipsize(pango_layout, PANGO_ELLIPSIZE_END);
    pango_layout_set_alignment(pango_layout, (PangoAlignment)parent_->alignment());
    pango_layout_set_markup   (pango_layout, parent_->text().c_str(), -1);
    pango_layout_set_height   (pango_layout, -parent_->lines());

    // Sets the width to which the lines of the PangoLayout should wrap or ellipsized. The default value is -1: no width set.
    pango_layout_set_width(pango_layout, parent_->width * PANGO_SCALE);
  }

  // Create font description: "[FAMILY-LIST] [STYLE-OPTIONS] [SIZE]"
  font_desc = pango_font_description_from_string(GetPangoFontName().c_str());
  {
    pango_font_description_set_weight(font_desc, PANGO_WEIGHT_NORMAL);
    pango_layout_set_font_description(pango_layout, font_desc);
  }

  // Get Pango context
  pango_ctx = pango_layout_get_context(pango_layout); // is not ref'ed

  // Set font options
  CairoFontOptions font_options;
  {
    cairo_font_options_set_antialias      (font_options, CAIRO_ANTIALIAS_DEFAULT);
    cairo_font_options_set_subpixel_order (font_options, CAIRO_SUBPIXEL_ORDER_DEFAULT);
    cairo_font_options_set_hint_style     (font_options, CAIRO_HINT_STYLE_DEFAULT);
    cairo_font_options_set_hint_metrics   (font_options, CAIRO_HINT_METRICS_ON);
    cairo_set_font_options(cairo_ctx, font_options);

    pango_cairo_context_set_font_options(pango_ctx, font_options);
  }

  pango_cairo_context_set_resolution(pango_ctx, dpi);

  cairo_set_source_rgba(cairo_ctx, 1.0, 1.0, 1.0, 1.0);

  pango_layout_context_changed(pango_layout);

  cairo_move_to(cairo_ctx, padding_x_, padding_y_);
  pango_cairo_show_layout(cairo_ctx, pango_layout);

  // clean up
  pango_font_description_free(font_desc);
  g_object_unref(pango_layout);
}

TextLoader::TextLoader()
  : alignment(TextAlignment::ALIGN_CENTER)
  , font_name("Ubuntu")
  , font_size(10)
  , width(-1)
  , minimum_width(0)
  , lines(1)
  , pimpl(new TextLoader::Impl(this))
{

}

TextLoader::~TextLoader()
{
  delete pimpl;
}

ObjectPtr<BaseTexture> TextLoader::CreateTexture()
{
  ObjectPtr<BaseTexture> result;
  Size sz = pimpl->ComputeTextSize();

  if (sz.width == 0 || sz.height == 0)
    return result;

  CairoGraphics* cairo_graphics = new CairoGraphics(CAIRO_FORMAT_ARGB32, std::max<int>(sz.width, minimum_width()), sz.height);
  cairo_t* cairo_ctx = cairo_graphics->GetContext();
  cairo_set_operator(cairo_ctx, CAIRO_OPERATOR_CLEAR);
  cairo_paint(cairo_ctx);
  cairo_set_operator(cairo_ctx, CAIRO_OPERATOR_OVER);

  pimpl->RasterizeText(cairo_ctx, color);

  NBitmapData* bitmap = cairo_graphics->GetBitmap();

  // NTexture2D is the high level representation of an image that is backed by
  // an actual opengl texture.

  BaseTexture* rasterized_text_texture = NULL;

  rasterized_text_texture = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
  rasterized_text_texture->Update(bitmap);

  result = rasterized_text_texture;
  rasterized_text_texture->UnReference(); // get rid of our ref so the owner is the only one who gets it
  rasterized_text_texture = NULL;

  delete bitmap;
  cairo_destroy(cairo_ctx);
  delete cairo_graphics;
  cairo_graphics = NULL;

  return result;
}

}
