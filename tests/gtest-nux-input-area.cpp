/*
 * Copyright 2010 Inalogic® Inc.
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
#include <boost/shared_ptr.hpp>
#include "Nux/Nux.h"

namespace {

struct TestInputArea : public testing::Test
{
  void SetUp()
  {
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("InputArea Test", 300, 200, nux::WINDOWSTYLE_NORMAL,
                                          NULL, false, NULL, NULL));
    area = new nux::InputArea("InputArea");
  }

  boost::shared_ptr<nux::WindowThread> wnd_thread;
  nux::ObjectPtr<nux::InputArea> area;
};

TEST_F(TestInputArea, GrabKeyboard)
{
  area->GrabKeyboard();
  EXPECT_TRUE(nux::GetWindowCompositor().IsInKeyboardGrabStack(area.GetPointer()));
}

TEST_F(TestInputArea, GrabPointer)
{
  area->GrabPointer();
  EXPECT_TRUE(nux::GetWindowCompositor().IsInPointerGrabStack(area.GetPointer()));
}

TEST_F(TestInputArea, UngrabsOnDestroy)
{
  nux::InputArea* area1 = new nux::InputArea("InputArea1");
  nux::InputArea* area2 = new nux::InputArea("InputArea2");

  // Adding some grabs
  area1->GrabKeyboard();
  area1->GrabPointer();
  area2->GrabPointer();
  area2->GrabKeyboard();
  area1->GrabKeyboard();

  ASSERT_TRUE(nux::GetWindowCompositor().IsInKeyboardGrabStack(area1));
  ASSERT_TRUE(nux::GetWindowCompositor().IsInPointerGrabStack(area1));
  ASSERT_TRUE(nux::GetWindowCompositor().IsInKeyboardGrabStack(area2));
  ASSERT_TRUE(nux::GetWindowCompositor().IsInPointerGrabStack(area2));

  // This should cleanup the references in the compositor
  area1->UnReference();

  EXPECT_FALSE(nux::GetWindowCompositor().IsInKeyboardGrabStack(area1));
  EXPECT_FALSE(nux::GetWindowCompositor().IsInPointerGrabStack(area1));

  area2->UnReference();

  EXPECT_FALSE(nux::GetWindowCompositor().IsInKeyboardGrabStack(area2));
  EXPECT_FALSE(nux::GetWindowCompositor().IsInPointerGrabStack(area2));
}

TEST_F(TestInputArea, IsTrackingChildMouseEvents)
{
  ASSERT_FALSE(area->IsTrackingChildMouseEvents());
  area->SetTrackChildMouseEvents(true);
  ASSERT_TRUE(area->IsTrackingChildMouseEvents());
  area->SetTrackChildMouseEvents(false);
  ASSERT_FALSE(area->IsTrackingChildMouseEvents());
}


}
