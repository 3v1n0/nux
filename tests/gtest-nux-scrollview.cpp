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
#include "Nux/HScrollBar.h"
#include "Nux/VScrollBar.h"

using namespace testing;

namespace {

class MockScrollView : public nux::ScrollView
{
  NUX_DECLARE_OBJECT_TYPE(MockScrollView, ScrollView);
public:
  MockScrollView(NUX_FILE_LINE_PROTO)
  : nux::ScrollView(NUX_FILE_LINE_PARAM)
  {

  }
  MOCK_METHOD0(QueueDraw, void());

  void FakeMouseWheelSignal(int x, int y, int wheel_delta, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
     EmitMouseWheelSignal(x, y, wheel_delta, mouse_button_state, special_keys_state);
  }

  nux::Area* FindAreaUnderMouse(const nux::Point& mouse_position, nux::NuxEventType event_type)
  {
    return ScrollView::FindAreaUnderMouse(mouse_position, event_type);
  }

  nux::HScrollBar* GetHScrollbar() const { return _hscrollbar; }
  nux::VScrollBar* GetVScrollbar() const { return _vscrollbar; }
};
NUX_IMPLEMENT_OBJECT_TYPE(MockScrollView);

class TestScrollView : public ::testing::Test
{
public:
  virtual void SetUp()
  {
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("ScrollView test", 300, 200, nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL));

    scrollview.Adopt(new MockScrollView(NUX_TRACKER_LOCATION));
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


TEST_F(TestScrollView, TestFindAreaUnderMouseScrollbars)
{
  scrollview->m_ViewContentHeight = 500;
  scrollview->m_ViewHeight = 400;
  scrollview->EnableVerticalScrollBar(true);
  scrollview->EnableHorizontalScrollBar(true);

  nux::Area* scroll_area = scrollview.GetPointer();
  nux::Area* vscrollbar = scrollview->GetVScrollbar();
  nux::Area* hscrollbar = scrollview->GetHScrollbar();

  EXPECT_CALL(*scrollview, QueueDraw())
    .Times(0);

  nux::Geometry const& geo_scrollview = scrollview->GetAbsoluteGeometry();
  nux::Geometry const& geo_vbar = vscrollbar->GetAbsoluteGeometry();
  nux::Geometry const& geo_hbar = hscrollbar->GetAbsoluteGeometry();

  nux::Area* middle_area = scrollview->FindAreaUnderMouse(geo_scrollview.GetCenter(), nux::NUX_MOUSE_MOVE);
  EXPECT_EQ(middle_area, scroll_area);

  nux::Area* right_area = scrollview->FindAreaUnderMouse(geo_vbar.GetCenter(), nux::NUX_MOUSE_MOVE);
  EXPECT_TRUE(right_area == vscrollbar || right_area->IsChildOf(vscrollbar));

  nux::Area* bottom_area = scrollview->FindAreaUnderMouse(geo_hbar.GetCenter(), nux::NUX_MOUSE_MOVE);
  EXPECT_TRUE(bottom_area == hscrollbar || bottom_area->IsChildOf(hscrollbar));
}


TEST_F(TestScrollView, TestFindAreaUnderMouseNoScrollbars)
{
  scrollview->m_ViewContentHeight = 500;
  scrollview->m_ViewHeight = 400;
  scrollview->EnableVerticalScrollBar(true);
  scrollview->EnableHorizontalScrollBar(true);

  nux::Area* scroll_area = scrollview.GetPointer();
  nux::Area* vscrollbar = scrollview->GetVScrollbar();
  nux::Area* hscrollbar = scrollview->GetHScrollbar();

  EXPECT_CALL(*scrollview, QueueDraw())
    .Times(0);

  nux::Geometry const& geo_scrollview = scrollview->GetAbsoluteGeometry();
  nux::Geometry const& geo_vbar = vscrollbar->GetAbsoluteGeometry();
  nux::Geometry const& geo_hbar = hscrollbar->GetAbsoluteGeometry();

  // Now, disable the scrollbars.
  scrollview->EnableVerticalScrollBar(false);
  scrollview->EnableHorizontalScrollBar(false);

  nux::Area* middle_area = scrollview->FindAreaUnderMouse(geo_scrollview.GetCenter(), nux::NUX_MOUSE_MOVE);
  EXPECT_EQ(middle_area, scroll_area);

  nux::Area* right_area = scrollview->FindAreaUnderMouse(geo_vbar.GetCenter(), nux::NUX_MOUSE_MOVE);
  EXPECT_EQ(right_area, scroll_area);

  nux::Area* bottom_area = scrollview->FindAreaUnderMouse(geo_hbar.GetCenter(), nux::NUX_MOUSE_MOVE);
  EXPECT_EQ(bottom_area, scroll_area);
}

}
