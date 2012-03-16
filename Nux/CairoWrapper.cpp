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
  struct CairoWrapper::Impl
  {
    Impl(CairoWrapper* parent, Geometry const& geo, DrawCanvasCallback callback);
    ~Impl();

    bool CreateBitmap ();
    bool Invalidate (Geometry const& geom);
    void SetDrawCanvasCallback (DrawCanvasCallback callback);
    bool DumpToFile (std::string const& filename);
    bool Recreate ();
    void DeleteResources();

    CairoWrapper*      parent_;
    Geometry           geometry_;
    DrawCanvasCallback draw_canvas_callback_;
    cairo_t*           cr_;
    cairo_surface_t*   surface_;
    NBitmapData*       bitmap_;
    BaseTexture*       texture_;
  };

  CairoWrapper::Impl::Impl(CairoWrapper* parent, Geometry const& geo, DrawCanvasCallback callback)
    : parent_(parent)
    , geometry_(geo)
    , draw_canvas_callback_(callback)
    , cr_(0)
    , surface_(0)
    , bitmap_(0)
    , texture_(0)
  {
  }

  CairoWrapper::Impl::~Impl()
  {
    DeleteResources();
  }

  void CairoWrapper::Impl::DeleteResources()
  {
    if (surface_)
    {
      cairo_surface_destroy (surface_);
      surface_ = 0;
    }

    if (cr_)
    {
      cairo_destroy (cr_);
      cr_ = 0;
    }

    if (bitmap_)
    {
      delete bitmap_;
      bitmap_ = 0;
    }

    if (texture_)
    {
      texture_->UnReference ();
      texture_ = 0;
    }
  }

  bool CairoWrapper::Impl::Recreate ()
  {
    DeleteResources();

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
    NBitmapData* bitmap = parent_->GetBitmap ();
    if (texture_)
        texture_->UnReference ();

    if (GetGraphicsDisplay()->GetGraphicsEngine() == 0)
      return false;

    texture_ = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    texture_->Update (bitmap);

    return true;
  }

  bool CairoWrapper::Impl::CreateBitmap ()
  {
    if (geometry_.width < 1 || geometry_.height < 1)
    {
      g_debug ("Width or height invalid!");
      return false;
    }

    if (bitmap_)
    {
      delete bitmap_;
      bitmap_ = 0;
    }

    BitmapFormat bitmap_format = BITFMT_B8G8R8A8;
    bitmap_ = new NTextureData (bitmap_format,
                                geometry_.width,
                                geometry_.height,
                                1);
    unsigned char* ptr = cairo_image_surface_get_data (surface_);
    int stride = cairo_image_surface_get_stride (surface_);

    if (ptr == 0 || stride == 0)
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

  void CairoWrapper::Impl::SetDrawCanvasCallback (DrawCanvasCallback callback)
  {
    if (!callback)
      return;

    draw_canvas_callback_ = callback;
    Recreate (); 
  }

  bool CairoWrapper::Impl::DumpToFile (std::string const& filename)
  {
    if (!surface_)
      Recreate();

    cairo_surface_write_to_png (surface_, filename.c_str ());

    return true;
  }

  bool CairoWrapper::Impl::Invalidate (Geometry const& geom)
  {
    if (geometry_.width  == geom.width && geometry_.height == geom.height)
      return false;

    geometry_.x      = geom.x;
    geometry_.y      = geom.y;
    geometry_.width  = geom.width;
    geometry_.height = geom.height;

    DeleteResources(); 

    return true;
  }

  CairoWrapper::CairoWrapper (Geometry const& geom, DrawCanvasCallback callback)
    : pimpl(new Impl(this, geom, callback))
  {
    Recreate ();
  }

  CairoWrapper::~CairoWrapper ()
  {
    delete pimpl;
  }

  bool CairoWrapper::Invalidate (Geometry const& geom)
  {
    return pimpl->Invalidate(geom);
  }

  void CairoWrapper::SetDrawCanvasCallback (DrawCanvasCallback callback)
  {
    pimpl->SetDrawCanvasCallback(callback);
  }

  bool CairoWrapper::DumpToFile (std::string const& filename)
  {
    return pimpl->DumpToFile(filename);
  }

  bool CairoWrapper::Recreate ()
  {
    return pimpl->Recreate();
  }

  NBitmapData* CairoWrapper::GetBitmap () const
  {
    if (!pimpl->bitmap_)
      pimpl->Recreate();

    return pimpl->bitmap_;
  }

  BaseTexture* CairoWrapper::GetTexture () const
  {
    if (!pimpl->texture_)
      pimpl->Recreate();

    return pimpl->texture_;
  }

  cairo_surface_t* CairoWrapper::GetCairoSurface () const
  {
    if (!pimpl->surface_)
      pimpl->Recreate();

    return pimpl->surface_;
  }

  cairo_t* CairoWrapper::GetCairoContext () const
  {
    if (!pimpl->cr_)
      pimpl->Recreate();

    return pimpl->cr_;
  }
}
