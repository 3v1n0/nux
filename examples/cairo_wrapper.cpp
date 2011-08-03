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
#include "Nux/CairoWrapper.h"
#include "Nux/VLayout.h"
#include "Nux/WindowThread.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "Nux/TimerProc.h"

nux::CairoWrapper* g_canvas  = NULL;
nux::TimerFunctor* g_timer   = NULL;
nux::TimerHandle   g_handler = NULL;

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

void
callback (nux::Geometry const& geom, cairo_t* cr)
{
  if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
    return;

  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
  cairo_scale (cr, 1.0, 1.0);
  cairo_set_source_rgba (cr, 1.0, 0.5, 0.25, 1.0);
  DrawRoundedRectangle (cr, 1.0, 5.0, 5.0, 10.0, 
                        (double) geom.width - 10.0,
                        (double) geom.height - 10.0);
  cairo_fill (cr);
}

void
terminate (void* data)
{
  nux::WindowThread* thread = NUX_STATIC_CAST (nux::WindowThread*, data);
  thread->TerminateThread ();
}

void
example (void* data)
{
  nux::Geometry geom = {0, 0, 200, 150};
  g_canvas = new nux::CairoWrapper (geom, sigc::ptr_fun (callback));
  std::string filename = "/tmp/cairo-wrapper-example.png"; 
  g_canvas->DumpToFile (filename);
}

void ThreadWidgetInit (nux::NThread* thread, void* initData)
{
  g_timer = new nux::TimerFunctor ();
  g_timer->OnTimerExpired.connect (sigc::ptr_fun (&example));
  g_handler = nux::GetTimer().AddTimerHandler (1000,
                                               g_timer,
                                               nux::GetWindowThread ());
}

int main (int    argc,
          char** argv)
{
  nux::NuxInitialize (0);
  nux::WindowThread* wt = NULL;

  wt = nux::CreateGUIThread (TEXT ("Cairo-Wrapper Example"),
                             400,
                             400,
                             0,
                             &ThreadWidgetInit,
                             0);
  wt->Run (NULL);
  delete wt;
  delete g_canvas;
  delete g_timer;

  return 0;
}
