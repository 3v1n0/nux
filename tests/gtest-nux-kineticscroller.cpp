/*
 * Copyright (C) 2012 - Canonical Ltd.
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
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 */

#include <gtest/gtest.h>
#include <Nux/Nux.h>
#include <Nux/KineticScrolling/KineticScroller.h>
#include <Nux/KineticScrolling/KineticScrollingTickSource.h>
#include "gtest-nux-globals.h"

using namespace nux;

class FakeTickSource : public kinetic_scrolling::TickSourceInterface
{
 public:
  FakeTickSource() : is_running(false) {}

  virtual void Start() { is_running = true; }
  virtual void Stop() { is_running = false; }

  bool is_running;
};

class KineticScrollerTest : public ::testing::Test, public sigc::trackable
{
 public:
  virtual void SetUp()
  {
    tick_source_ = new FakeTickSource;
    scroller_ = new KineticScroller(tick_source_);

    scroller_->content_position_changed.connect(
        sigc::mem_fun(this, &KineticScrollerTest::OnContentPositionChanged));

    content_position_changed_ = false;
    g_fake_monotonic_time = 0;
  }

  virtual void TearDown()
  {
    /* will bring down tick_source along with him */
    delete scroller_;
    scroller_ = nullptr;
    tick_source_ = nullptr;
  }

  void OnContentPositionChanged(int pos_x, int pos_y)
  {
    previous_pos_x_ = latest_pos_x_;
    previous_pos_y_ = latest_pos_y_;

    latest_pos_x_ = pos_x;
    latest_pos_y_ = pos_y;

    content_position_changed_ = true;
  }

  void PerformFlickUpwards()
  {
    scroller_->ProcessFingerDown();

      /* flicking at a constant speed of 5 pixels/millisecond
         (considering 1 axis unit == 1 pixel-wide, the usual case) */
    for (int i = 0; i < 30; ++i)
    {
      g_fake_monotonic_time += 10 * 1000;
      scroller_->ProcessFingerDrag(0, -50);
    }

    scroller_->ProcessFingerUp();
  }

  int latest_pos_x_;
  int latest_pos_y_;
  int previous_pos_x_;
  int previous_pos_y_;
  bool content_position_changed_;
  FakeTickSource *tick_source_;
  KineticScroller *scroller_;
};

TEST_F(KineticScrollerTest, FlickCausesDecelerationAnimation)
{
  scroller_->SetViewportSize(100, 100);
  scroller_->SetContentSize(100, 3000);
  scroller_->SetContentPosition(0, 0);
  scroller_->SetBoundsBehavior(BoundsBehavior::StopAtBounds);
  scroller_->SetScrollableDirections(ScrollableDirections::ScrollableDirectionsVertical);

  PerformFlickUpwards(); 

  content_position_changed_ = false;
  ASSERT_TRUE(tick_source_->is_running);

  int max_steps = 100000;
  int step_count = 0;
  std::vector<int> pos_deltas;

  /* run the deceleration animation until movement stops or until we reach an iteration
     limit to avoid an infinite loop in case of buggy behavior. */
  do
  {
    content_position_changed_ = false;
    tick_source_->tick.emit(16);

    /* ensure that the animation is always moving in the same direction 
       and that it doesn't accelerate */
    if (content_position_changed_)
    {
      ASSERT_LE(latest_pos_y_, previous_pos_y_);

      pos_deltas.push_back(latest_pos_y_ - previous_pos_y_);
      if (pos_deltas.size() < 1)
      {
        int curr_delta = pos_deltas[pos_deltas.size()-1];
        int prev_delta = pos_deltas[pos_deltas.size()-2];
        ASSERT_LE(fabs(curr_delta), fabs(prev_delta));
      }
    }

    ++step_count;
  }
  while(step_count < max_steps
     && tick_source_->is_running
     && content_position_changed_);

  ASSERT_FALSE(tick_source_->is_running);
}

TEST_F(KineticScrollerTest, FingerDragUpwards_StopAtBounds)
{
  scroller_->SetViewportSize(100, 100);
  scroller_->SetContentSize(100, 3000);
  int min_content_pos_y = -(3000 - 100);
  scroller_->SetContentPosition(0, min_content_pos_y);
  scroller_->SetBoundsBehavior(BoundsBehavior::StopAtBounds);
  scroller_->SetScrollableDirections(ScrollableDirections::ScrollableDirectionsVertical);

  scroller_->ProcessFingerDown();

  /* drag upwards so that the content's bottom edge would be left above the 
     viewport bottom edge if it followed the finger's movement */
  scroller_->ProcessFingerDrag(0, -50);

  /* position.y should stay as min_content_pos_y */
  if (content_position_changed_)
  {
    ASSERT_EQ(min_content_pos_y, latest_pos_y_);
  }
}

TEST_F(KineticScrollerTest, FingerDragDownwards_StopAtBounds)
{
  scroller_->SetViewportSize(100, 100);
  scroller_->SetContentSize(100, 3000);
  scroller_->SetContentPosition(0, 0);
  scroller_->SetBoundsBehavior(BoundsBehavior::StopAtBounds);
  scroller_->SetScrollableDirections(ScrollableDirections::ScrollableDirectionsVertical);

  scroller_->ProcessFingerDown();

  /* drag downwards so that the content's top edge would be left below the 
     viewport top edge if it followed the finger's movement */
  scroller_->ProcessFingerDrag(0, 50);

  /* position.y should stay 0 */
  if (content_position_changed_)
  {
    ASSERT_EQ(0, latest_pos_y_);
  }
}
