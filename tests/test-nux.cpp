/*
 * Copyright (C) 2010 Canonical Ltd
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
 * Authored by Gordon Allott <gord.allott@canonical.com>
 *
 */

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <Nux/Nux.h>
#include <glib-object.h>

void TestCanvasSuite();
void TestSystemSuite();

void test_timeline_create_suite ();

gint
main (gint argc, gchar *argv[])
{
#if G_ENCODE_VERSION (GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION) <= GLIB_VERSION_2_34
  g_type_init();
#endif
  g_test_init (&argc, &argv, NULL);
  nux::NuxInitialize(0);

  // Do not print Nux log messages to the console.
  GLogDevice.Disable ();

  /* First the Nux specific tests */
  TestSystemSuite();
  // TODO: fix the test
  // TestCanvasSuite();

  /* Please keep this alphabetically ordered */
  //test_timeline_create_suite ();

  return g_test_run ();
}
