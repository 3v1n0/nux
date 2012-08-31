/*
 * Copyright 2012 Canonical Ltd.
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
 * Authored by: Marco Trevisan <marco.trevisan@canonical.com>
 */

#include <gmock/gmock.h>
#include "Nux/Nux.h"

namespace {

TEST(TestInputArea, UngrabsOnDestroy)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Area Test", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::InputArea* area = new nux::InputArea("");

  // Adding some grabs
  area->GrabKeyboard();
  area->GrabPointer();
  area->GrabPointer();

  // This should cleanup the references in the compositor
  area->UnReference();

  EXPECT_FALSE(nux::GetWindowCompositor().IsInKeyboardGrabStack(area));
  EXPECT_FALSE(nux::GetWindowCompositor().IsInPointerGrabStack(area));
  delete wnd_thread;
}

}
