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

#include "Nux/Nux.h"
#include "Nux/Canvas.h"
#include "Nux/View.h"
#include "Nux/VLayout.h"
#include "Nux/WindowThread.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "Nux/Canvas.h"

void
DrawRoundedRectangle (cairo_t* cr,
                      double   aspect,
                      double   x,
                      double   y,
                      double   cornerRadius,
                      double   width,
                      double   height)
{
  double radius = cornerRadius / aspect;

  // top-left, right of the corner
  cairo_move_to (cr, x + radius, y);

  // top-right, left of the corner
  cairo_line_to (cr, x + width - radius, y);

  // top-right, below the corner
  cairo_arc (cr,
             x + width - radius,
             y + radius,
             radius,
             -90.0f * G_PI / 180.0f,
             0.0f * G_PI / 180.0f);

  // bottom-right, above the corner
  cairo_line_to (cr, x + width, y + height - radius);

  // bottom-right, left of the corner
  cairo_arc (cr,
             x + width - radius,
             y + height - radius,
             radius,
             0.0f * G_PI / 180.0f,
             90.0f * G_PI / 180.0f);

  // bottom-left, right of the corner
  cairo_line_to (cr, x + radius, y + height);

  // bottom-left, above the corner
  cairo_arc (cr,
             x + radius,
             y + height - radius,
             radius,
             90.0f * G_PI / 180.0f,
             180.0f * G_PI / 180.0f);

  // top-left, right of the corner
  cairo_arc (cr,
             x + radius,
             y + radius,
             radius,
             180.0f * G_PI / 180.0f,
             270.0f * G_PI / 180.0f);
}

class FooCanvas : public nux::Canvas
{
  public:
    FooCanvas (NUX_FILE_LINE_DECL);
    ~FooCanvas ();

    void Paint ();
};

FooCanvas::FooCanvas (NUX_FILE_LINE_DECL) : Canvas (NUX_FILE_LINE_PARAM)
{
}

FooCanvas::~FooCanvas ()
{
}

void
FooCanvas::Paint ()
{
  cairo_t* cr = GetCairoContext ();

  if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
    return;

  cairo_scale (cr, 1.0, 1.0);
  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
  DrawRoundedRectangle (cr,
                        1.0,
                        4.0,
                        4.0,
                        10.0,
                        (double) GetLastWidth () - 8.0,
                        (double) GetLastHeight () - 8.0);
  cairo_set_source_rgba (cr, 1.0f, 0.0f, 0.0f, 1.0f);
  cairo_fill_preserve (cr);
  cairo_set_source_rgba (cr, 0.0f, 1.0f, 0.0f, 1.0f);
  cairo_stroke (cr);
  //cairo_surface_write_to_png (GetCairoSurface (), "/tmp/foocanvas.png");
}

class BarCanvas : public nux::Canvas
{
  public:
    BarCanvas (NUX_FILE_LINE_DECL);
    ~BarCanvas ();

    void Paint ();
};

BarCanvas::BarCanvas (NUX_FILE_LINE_DECL) : Canvas (NUX_FILE_LINE_PARAM)
{
}

BarCanvas::~BarCanvas ()
{
}

void
BarCanvas::Paint ()
{
  cairo_t* cr = GetCairoContext ();

  if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
    return;

  cairo_scale (cr, 1.0, 1.0);
  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
  DrawRoundedRectangle (cr,
                        1.0,
                        4.0,
                        4.0,
                        10.0,
                        (double) GetLastWidth () - 8.0,
                        (double) GetLastHeight () - 8.0);
  cairo_set_source_rgba (cr, 0.0f, 0.0f, 1.0f, 1.0f);
  cairo_fill_preserve (cr);
  cairo_set_source_rgba (cr, 1.0f, 1.0f, 0.0f, 1.0f);
  cairo_stroke (cr);
  //cairo_surface_write_to_png (GetCairoSurface (), "/tmp/barcanvas.png");
}

void ThreadWidgetInit (nux::NThread* thread, void* initData)
{
  nux::VLayout* layout = new nux::VLayout (TEXT(""), NUX_TRACKER_LOCATION);

  FooCanvas* foocanvas = new FooCanvas (NUX_TRACKER_LOCATION);
  BarCanvas* barcanvas = new BarCanvas (NUX_TRACKER_LOCATION);

  layout->AddView (foocanvas, 1, nux::eCenter, nux::eFull);
  layout->AddView (barcanvas, 1, nux::eCenter, nux::eFull);
  layout->SetContentDistribution (nux::eStackCenter);
    
  nux::GetWindowThread()->SetLayout (layout);
}

int main (int    argc,
          char** argv)
{
  nux::NuxInitialize (0);
  nux::WindowThread* wt = NULL;

  wt = nux::CreateGUIThread (TEXT ("Canvas Example"),
                             400,
                             400,
                             0,
                             &ThreadWidgetInit,
                             0);
  wt->Run (NULL);
  delete wt;

  return 0;
}
