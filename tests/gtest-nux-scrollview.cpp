// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2012 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Andrea Azzarone <azzaronea@gmail.com>
 *
 */

#include <gmock/gmock.h>

#include "Nux/Nux.h"
#include "Nux/ScrollView.h"
#include "Nux/VLayout.h"

using namespace testing;

namespace {

class MockScrollView : public nux::ScrollView
{
public:
  MOCK_METHOD0(QueueDraw, void());

  void FakeMouseWheelSignal(int x, int y, int wheel_delta, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
     EmitMouseWheelSignal(x, y, wheel_delta, mouse_button_state, special_keys_state);
  }
};

class TestScrollView : public ::testing::Test
{
public:
  virtual void SetUp()
  {
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("ScrollView test", 300, 200, nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL));

    scrollview.Adopt(new MockScrollView());
    scrollview->SetLayout(new nux::VLayout());
  }

  std::unique_ptr<nux::WindowThread> wnd_thread;
  nux::ObjectPtr<MockScrollView> scrollview;
};

TEST_F(TestScrollView, TestQueueDrawScrollDownNoScrollbars)
{
  EXPECT_CALL(*scrollview, QueueDraw())
    .Times(0);

  scrollview->FakeMouseWheelSignal(0, 0, -NUX_MOUSEWHEEL_DELTA, 0, 0);
}


TEST_F(TestScrollView, TestQueueDrawScrollUpNoScrollbars)
{
  EXPECT_CALL(*scrollview, QueueDraw())
    .Times(0);

  scrollview->FakeMouseWheelSignal(0, 0, NUX_MOUSEWHEEL_DELTA, 0, 0);
}


TEST_F(TestScrollView, TestQueueDrawScrollDown)
{
  scrollview->m_ViewContentHeight = 500;
  scrollview->m_ViewHeight = 400;

  EXPECT_CALL(*scrollview, QueueDraw())
    .Times(1);

  scrollview->FakeMouseWheelSignal(0, 0, -NUX_MOUSEWHEEL_DELTA, 0, 0);
}


TEST_F(TestScrollView, TestQueueDrawScrollUp)
{
  scrollview->m_ViewContentHeight = 500;
  scrollview->m_ViewHeight = 400;
  scrollview->ScrollDown(1, 10);

  EXPECT_CALL(*scrollview, QueueDraw())
    .Times(1);

  scrollview->FakeMouseWheelSignal(0, 0, NUX_MOUSEWHEEL_DELTA, 0, 0);
}


TEST_F(TestScrollView, TestQueueDrawScrollDownEnd)
{
  scrollview->m_ViewContentHeight = 500;
  scrollview->m_ViewHeight = 400;
  scrollview->ScrollDown(1, 100);

  EXPECT_CALL(*scrollview, QueueDraw())
    .Times(0);

  scrollview->FakeMouseWheelSignal(0, 0, -NUX_MOUSEWHEEL_DELTA, 0, 0);
}


TEST_F(TestScrollView, TestQueueDrawScrollUpStart)
{
  scrollview->m_ViewContentHeight = 500;
  scrollview->m_ViewHeight = 400;

  EXPECT_CALL(*scrollview, QueueDraw())
    .Times(0);

  scrollview->FakeMouseWheelSignal(0, 0, NUX_MOUSEWHEEL_DELTA, 0, 0);
}

}
