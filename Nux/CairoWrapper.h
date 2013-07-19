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

#ifndef CAIRO_WRAPPER_H
#define CAIRO_WRAPPER_H

#include <cairo.h>

#include "Nux/Nux.h"

namespace nux
{
  typedef std::function<void(Geometry const&, cairo_t*)> DrawCanvasCallback;

  class CairoWrapper
  {
    public:
      CairoWrapper (Geometry const& geom, DrawCanvasCallback callback);
      ~CairoWrapper ();

      bool               Invalidate (Geometry const& geom);
      void               SetDrawCanvasCallback (DrawCanvasCallback callback);
      cairo_surface_t*   GetCairoSurface () const;
      cairo_t*           GetCairoContext () const;
      bool               DumpToFile (std::string const& filename);
      BaseTexture*       GetTexture () const;
      NBitmapData*       GetBitmap () const;
      bool               Recreate ();

    private:
      struct Impl;
      Impl* pimpl;
  };
}

#endif // CAIRO_WRAPPER_H
