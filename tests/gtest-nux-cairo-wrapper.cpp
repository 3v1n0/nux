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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Nux/CairoWrapper.h"

#include <string>
#include <fstream>

#include <iostream>

typedef void (*TestFunc)(void);

nux::TimerFunctor* g_timer   = NULL;
nux::TimerHandle   g_handler = NULL;

void
callback_one (nux::Geometry const& geom, cairo_t* cr)
{
  if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
    return;

  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_OVER);
  cairo_scale (cr, 1.0, 1.0);
  cairo_set_source_rgba (cr, 1.0, 0.5, 0.25, 1.0);
  cairo_rectangle (cr,
                   5.0,
                   5.0,
                   (double) geom.width - 10.0,
                   (double) geom.height - 10.0);
  cairo_fill (cr);
}

void
callback_two (nux::Geometry const& geom, cairo_t* cr)
{
  if (cairo_status (cr) != CAIRO_STATUS_SUCCESS)
    return;

  cairo_set_operator (cr, CAIRO_OPERATOR_CLEAR);
  cairo_paint (cr);
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_scale (cr, 1.0, 1.0);
  cairo_set_source_rgba (cr, 0.25, 0.5, 1.0, 1.0);
  cairo_rectangle (cr,
                   2.0,
                   2.0,
                   (double) geom.width - 4.0,
                   (double) geom.height - 4.0);
  cairo_fill (cr);
}

void
terminate (void* data)
{
  nux::WindowThread* thread = NUX_STATIC_CAST (nux::WindowThread*, data);
  thread->ExitMainLoop();
}

void
init (nux::NThread* /* thread */, void* data)
{
  TestFunc func = (TestFunc) data;

  (func) ();

  g_timer = new nux::TimerFunctor ();
  g_timer->tick.connect (sigc::ptr_fun (&terminate));
  g_handler = nux::GetTimer().AddOneShotTimer  (100,
                                               g_timer,
                                               nux::GetWindowThread ());
}

void
run_test (TestFunc func)
{
  nux::WindowThread* wt = NULL;

  wt = nux::CreateGUIThread (TEXT ("Canvas Test"), 400, 400, 0, &init, (void*) func);
  wt->Run (NULL);
  delete wt;
  delete g_timer;
  g_timer = NULL;
}

void
test_construction ()
{
  nux::Geometry geom_one = {0, 0, 100, 100};
  nux::Geometry geom_two = {0, 0, 200, 200};
  nux::CairoWrapper wrapper (geom_one, sigc::ptr_fun (callback_one));

  EXPECT_FALSE (wrapper.Invalidate (geom_one));
  EXPECT_TRUE (wrapper.Invalidate (geom_two));
}

void
test_invalidate ()
{
  nux::Geometry geom_one = {0, 0, 100, 100};
  nux::Geometry geom_two = {0, 0, 150, 75};
  nux::CairoWrapper wrapper (geom_one, sigc::ptr_fun (callback_one));

  EXPECT_FALSE (wrapper.Invalidate (geom_one));
  EXPECT_TRUE (wrapper.Invalidate (geom_two));
}

void
test_get_cairo_surface ()
{
  nux::Geometry geom = {0, 0, 100, 100};
  nux::CairoWrapper wrapper (geom, sigc::ptr_fun (callback_one));

  cairo_surface_t* surf = wrapper.GetCairoSurface ();
  EXPECT_FALSE (surf == NULL);
}

void
test_get_cairo_context ()
{
  nux::Geometry geom = {0, 0, 100, 100};
  nux::CairoWrapper wrapper (geom, sigc::ptr_fun (callback_one));

  EXPECT_TRUE (wrapper.GetCairoContext ());
}

void
test_dump_to_file ()
{
  nux::Geometry geom = {0, 0, 100, 100};
  nux::CairoWrapper wrapper (geom, sigc::ptr_fun (callback_one));

  EXPECT_TRUE (wrapper.DumpToFile ("/tmp/dump.png"));
}

void
test_get_texture ()
{
  nux::Geometry geom = {0, 0, 100, 100};
  nux::CairoWrapper wrapper (geom, sigc::ptr_fun (callback_one));

  EXPECT_TRUE (wrapper.GetTexture ());
}

void
test_get_bitmap ()
{
  nux::Geometry geom = {0, 0, 100, 100};
  nux::CairoWrapper wrapper (geom, sigc::ptr_fun (callback_one));

  EXPECT_TRUE (wrapper.GetBitmap ());
}

namespace {

TEST (DISABLED_TestCairoWrapper, TestConstruction) {
  run_test (test_construction);
}

TEST (DISABLED_TestCairoWrapper, TestInvalidate) {
  run_test (test_invalidate);
}

TEST (DISABLED_TestCairoWrapper, TestGetCairoSurface) {
  run_test (test_get_cairo_surface);
}

TEST (DISABLED_TestCairoWrapper, TestGetCairoContext) {
  run_test (test_get_cairo_context);
}

TEST (DISABLED_TestCairoWrapper, TestDumpToFile) {
  run_test (test_dump_to_file);
}

TEST (DISABLED_TestCairoWrapper, TestGetTexture) {
  run_test (test_get_texture);
}

TEST (DISABLED_TestCairoWrapper, TestGetBitmap) {
  run_test (test_get_bitmap);
}
}
