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

#ifndef CANVAS_H
#define CANVAS_H

#include <cairo.h>

#include "Nux/Nux.h"
#include "Nux/View.h"

namespace nux
{
  class Canvas : public View
  {
    public:
      Canvas (NUX_FILE_LINE_DECL);
      ~Canvas ();

    protected:
      void Draw (GraphicsEngine& graphics_engine, bool forceDraw);
      void DrawContent (GraphicsEngine& graphics_engine, bool forceDraw);
      virtual void Paint ();
      void Invalidate ();

      cairo_surface_t* GetCairoSurface ();
      cairo_t*         GetCairoContext ();
      int              GetLastWidth ();
      int              GetLastHeight ();

    private:
      void             PreLayoutManagement ();
      long             PostLayoutManagement (long layoutResult);
      void             Recreate ();
      NBitmapData*     GetBitmap ();

    private:
      cairo_t*         _cr;
      cairo_surface_t* _surface;
      int              _last_width;
      int              _last_height;
      bool             _invalid;
      BaseTexture*     _texture;
  };
}

#endif // CANVAS_H
