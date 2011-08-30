/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Authored by: Mirco Müller <mirco.mueller@canonical.com
 */


#include "CairoWrapper.h"

#include <iostream>

namespace nux
{
  CairoWrapper::CairoWrapper (Geometry const& geom, DrawCanvasCallback callback)
  {
    draw_canvas_callback_ = callback;
	geometry_.x           = geom.x;
	geometry_.y           = geom.y;
	geometry_.width       = geom.width;
	geometry_.height      = geom.height;
    cr_                   = NULL;
    surface_              = NULL;
    bitmap_               = NULL;
    texture_              = NULL;
	Recreate ();
  }

  CairoWrapper::~CairoWrapper ()
  {
    if (surface_)
      cairo_surface_destroy (surface_);

    if (cr_)
      cairo_destroy (cr_);

	if (bitmap_)
      delete bitmap_;

    if (texture_)
      texture_->UnReference ();
  }

  bool CairoWrapper::Recreate ()
  {
    if (surface_)
      cairo_surface_destroy (surface_);

    if (cr_)
      cairo_destroy (cr_);

    surface_ = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                           geometry_.width,
                                           geometry_.height);

    if (cairo_surface_status (surface_) != CAIRO_STATUS_SUCCESS)
	{
      g_debug ("Could not create image-surface!");
      return false;
    }

    cr_ = cairo_create (surface_);
    if (cairo_status (cr_) != CAIRO_STATUS_SUCCESS)
    {
      cairo_surface_destroy (surface_);
      g_debug ("Could not create cairo-context!");
      return false;
    }

    if (!draw_canvas_callback_)
      return false;

    draw_canvas_callback_ (geometry_, cr_);

    CreateBitmap ();
    NBitmapData* bitmap = GetBitmap ();
    if (texture_)
        texture_->UnReference ();

	if (GetGraphicsDisplay()->GetGraphicsEngine() == NULL)
	  return false;

    texture_ = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    texture_->Update (bitmap);

	return true;
  }

  bool CairoWrapper::CreateBitmap ()
  {
    if (geometry_.width < 1 || geometry_.height < 1)
    {
      g_debug ("Width or height invalid!");
      return false;
    }

    if (bitmap_)
    {
      delete bitmap_;
	  bitmap_ = NULL;
    }

    BitmapFormat bitmap_format = BITFMT_B8G8R8A8;
    bitmap_ = new NTextureData (bitmap_format,
                                geometry_.width,
                                geometry_.height,
                                1);
    t_u8* ptr = cairo_image_surface_get_data (surface_);
    int stride = cairo_image_surface_get_stride (surface_);

    if (ptr == NULL || stride == 0)
    {
      g_debug ("Invalid surface!");
      return false;
    }

    for (int j = 0; j < geometry_.height; j++)
    {
      Memcpy (bitmap_->GetSurface (0).GetPtrRawData() + j * bitmap_->GetSurface (0).GetPitch(),
                (const void *) (&ptr[j * stride]),
                geometry_.width * GPixelFormats[bitmap_format].NumComponents);
    }

    return true;
  }

  NBitmapData* CairoWrapper::GetBitmap () const
  {
    return bitmap_;
  }

  void CairoWrapper::SetDrawCanvasCallback (DrawCanvasCallback callback)
  {
    if (!callback)
      return;

    draw_canvas_callback_ = callback;

    Recreate (); 
  }

  bool CairoWrapper::DumpToFile (std::string const& filename)
  {
    cairo_surface_write_to_png (surface_, filename.c_str ());

    return true;
  }

  BaseTexture* CairoWrapper::GetTexture () const
  {
    return texture_;
  }

  cairo_surface_t* CairoWrapper::GetCairoSurface () const
  {
    return surface_;
  }

  cairo_t* CairoWrapper::GetCairoContext () const
  {
    return cr_;
  }

  bool CairoWrapper::Invalidate (Geometry const& geom)
  {
    if (geometry_.width  == geom.width &&
	    geometry_.height == geom.height)
      return false;

	geometry_.x      = geom.x;
	geometry_.y      = geom.y;
	geometry_.width  = geom.width;
	geometry_.height = geom.height;

    Recreate (); 

    return true;
  }
}
