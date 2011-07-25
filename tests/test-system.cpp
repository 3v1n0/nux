/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include <glib.h>
#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/Button.h"

//static void TestMainWindow(void);
//static void TestMainWindowStatus(void);
//static void TestMainWindowSize(void);

void TestSystemSuite (void)
{
#define TESTDOMAIN "/System/"

//  g_test_add_func (TESTDOMAIN"/TestMainWindow",       TestMainWindow);
//  g_test_add_func (TESTDOMAIN"/TestMainWindowStatus", TestMainWindowStatus);
//  g_test_add_func (TESTDOMAIN"/TestMainWindowSize",   TestMainWindowSize);
}

/*
static void TestMainWindow (void)
{
  // Warning: on windows the window can not be physically smaller than ~120x100
  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("TestMainWindow"), 120, 100, 0, 0, 0);

  g_assert (wt != 0);
  g_assert (nux::GetGraphicsThread () == wt);

  delete wt;
}


static void TestMainWindowStatus (void)
{
  // Warning: on windows the window can not be physically smaller than ~120x100
  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("TestMainWindowStatus"), 120, 100, 0, 0, 0);

  g_assert (wt != 0);
  g_assert (nux::GetGraphicsThread () == wt);

  //g_assert (nux::GetWindow ().IsWindowVisible () == false);

  //nux::GetWindow ().ShowWindow ();

  //g_assert (nux::GetWindow ().IsWindowVisible () == true);

  //nux::GetWindow ().HideWindow ();

  //g_assert (nux::GetWindow ().IsWindowVisible () == false);

  delete wt;
}

static void TestMainWindowSize (void)
{
  // Warning: on windows the window can not be physically smaller than ~120x100
  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("TestMainWindowSize"), 120, 100, 0, 0, 0);

  nux::GetWindow ().ShowWindow ();

  g_assert (wt != 0);
  g_assert (nux::GetGraphicsThread () == wt);
  g_assert (nux::GetWindow ().GetWindowWidth () == 120);
  g_assert (nux::GetWindow ().GetWindowHeight () == 100);

  delete wt;
}*/
