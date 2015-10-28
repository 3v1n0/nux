/*
* Copyright 2012 Inalogic Inc.
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
* Authored by: Brandon Schaefer <brandon.schaefer@canonical.com>
*
*/


#include <gmock/gmock.h>
using namespace testing;

#include "Nux/Nux.h"
#include "Nux/InputAreaProximity.h"
#include "Nux/WindowCompositor.h"
#include "Nux/ProgramFramework/TestView.h"

#include <stack>

namespace
{

const int VIEW_WIDTH = 500;
const int VIEW_HEIGHT = 500;

class TestInputAreaProximity : public Test
{
public:
  TestInputAreaProximity()
    : top_proximity_(0)
    , near_signal_recived_(false)
    , beyond_signal_recived_(false)
    , approaching_signal_recived_(false)
  {
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("View Test", 300, 200, nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL));
    MoveMouse(VIEW_WIDTH+200, VIEW_HEIGHT+200);
  }

  ~TestInputAreaProximity()
  {
    while (!prox_areas_.empty())
      RemoveInputAreaProximity();
  }

  void AddNInputAreaProximities(int n, int proximity = 10)
  {
    for (int i = 0; i < n; i++)
      AddInputAreaProximity(proximity);
  }

  void AddInputAreaProximity(int proximity = 10)
  {
    nux::TestView* test_view = new nux::TestView("");
    test_view->SetGeometry(nux::Geometry(0, 0, VIEW_WIDTH, VIEW_HEIGHT));
    top_proximity_ = proximity;

    auto prox_area = std::make_shared<nux::InputAreaProximity>(test_view, proximity);

    prox_area->mouse_near.connect([this] (const nux::Point&) {
      near_signal_recived_ = true;
    });

    prox_area->mouse_beyond.connect([this] (const nux::Point&) {
      beyond_signal_recived_ = true;
    });

    prox_area->mouse_approaching.connect([this] (const nux::Point&, const nux::Point& difference) {
      approaching_signal_recived_ = true;
      last_approaching_point_ = difference;
    });

    prox_areas_.push(std::make_pair(prox_area, test_view));
  }

  void RemoveInputAreaProximity()
  {
    if (!prox_areas_.empty())
    {
      auto prox_area = prox_areas_.top();
      prox_areas_.pop();

      prox_area.second->UnReference();
    }
  }

  void ResetSignalRecivedValues()
  {
    near_signal_recived_ = false;
    beyond_signal_recived_ = false;
    approaching_signal_recived_ = false;
  }

  bool NearSignalRecived() const
  {
    return near_signal_recived_;
  }

  bool BeyondSignalRecived() const
  {
    return beyond_signal_recived_;
  }

  bool ApproachingSignalRecived() const
  {
    return approaching_signal_recived_;
  }

  nux::Point GetLastApprochingPoint()
  {
    return last_approaching_point_;
  }

  void MoveMouseNear()
  {
    MoveMouse(VIEW_WIDTH+(top_proximity_-1), VIEW_HEIGHT+(top_proximity_-1));
  }

  void MoveMouseBeyond()
  {
    MoveMouse(VIEW_WIDTH+(top_proximity_+1), VIEW_HEIGHT+(top_proximity_+1));
  }

  void MoveMouse(int x, int y)
  {
    nux::Event event;
    event.type = nux::NUX_MOUSE_MOVE;
    event.x = x;
    event.y = y;
    nux::GetWindowCompositor().ProcessEvent(event);
  }

  void MoveMouseBeyondWindow()
  {
    nux::Event event;
    event.type = nux::NUX_WINDOW_MOUSELEAVE;
    event.x = VIEW_WIDTH + top_proximity_ + 10;
    event.y = VIEW_HEIGHT + top_proximity_ + 10;
    nux::GetWindowCompositor().ProcessEvent(event);
  }

  int GetProxListSize()
  {
    return nux::GetWindowCompositor().GetProximityListSize();
  }

private:
  std::unique_ptr<nux::WindowThread> wnd_thread;
  std::stack<std::pair<std::shared_ptr<nux::InputAreaProximity>, nux::TestView*> > prox_areas_;

  nux::Point last_approaching_point_;

  int top_proximity_;
  bool near_signal_recived_;
  bool beyond_signal_recived_;
  bool approaching_signal_recived_;
};


TEST_F(TestInputAreaProximity, TestAtemptToAddNullToList)
{
  const int prox_size = GetProxListSize();
  nux::InputAreaProximity* prox_area = new nux::InputAreaProximity(NULL, 10);

  MoveMouseNear();
  MoveMouseBeyond();

  delete prox_area;
  ASSERT_EQ(GetProxListSize(), prox_size);
}

TEST_F(TestInputAreaProximity, TestHandlesDeletedInputArea)
{
  const int prox_size = GetProxListSize();

  nux::TestView* test_view = new nux::TestView("");
  test_view->SetGeometry(nux::Geometry(0, 0, VIEW_WIDTH, VIEW_HEIGHT));

  nux::InputAreaProximity* prox_area = new nux::InputAreaProximity(test_view, 10);
  test_view->UnReference();

  MoveMouseNear();
  MoveMouseBeyond();

  delete prox_area;
  ASSERT_EQ(GetProxListSize(), prox_size);
}

TEST_F(TestInputAreaProximity, TestAddsNullProximity)
{
  const int prox_size = nux::GetWindowCompositor().GetProximityListSize();
  nux::GetWindowThread()->GetWindowCompositor().AddAreaInProximityList(NULL);

  MoveMouseNear();
  MoveMouseBeyond();

  nux::GetWindowThread()->GetWindowCompositor().RemoveAreaInProximityList(NULL);
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), prox_size);
}

TEST_F(TestInputAreaProximity, TestProximityAreaAddsToList)
{
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), 0);
  AddNInputAreaProximities(2);
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), 2);
}

TEST_F(TestInputAreaProximity, TestRemovesFromProximityList)
{
  int num_prox_areas = 10;
  AddNInputAreaProximities(num_prox_areas);
  ASSERT_EQ(GetProxListSize(), num_prox_areas);

  for (int i = 0; i < num_prox_areas; i++)
  {
    RemoveInputAreaProximity();
    ASSERT_EQ(GetProxListSize(), num_prox_areas - (i + 1));
  }
}

TEST_F(TestInputAreaProximity, TestNearSignal)
{
  AddInputAreaProximity();

  MoveMouseBeyond();
  ASSERT_FALSE(NearSignalRecived());

  MoveMouseNear();
  ASSERT_TRUE(NearSignalRecived());
}

TEST_F(TestInputAreaProximity, TestApproachingSignal)
{
  AddInputAreaProximity();

  MoveMouseBeyond();
  ASSERT_FALSE(ApproachingSignalRecived());

  MoveMouseNear();
  ASSERT_TRUE(ApproachingSignalRecived());
}

TEST_F(TestInputAreaProximity, TestApproachingDoesNotEmitInsideArea)
{
  AddInputAreaProximity();

  MoveMouse(VIEW_WIDTH/2, VIEW_HEIGHT/2);
  ResetSignalRecivedValues();

  MoveMouse(VIEW_WIDTH/4, VIEW_HEIGHT/4);
  ASSERT_FALSE(ApproachingSignalRecived());

  MoveMouseNear();
  ASSERT_TRUE(ApproachingSignalRecived());
}

TEST_F(TestInputAreaProximity, TestApproachingSignalHorizontal)
{
  int proximity = 10;
  AddInputAreaProximity(proximity);

  nux::Point difference;
  int expected_result;
  for (int i = 1; i <= proximity; i++)
  {
    MoveMouse(VIEW_WIDTH+(proximity-i), VIEW_HEIGHT+(proximity-1));

    difference = GetLastApprochingPoint();
    expected_result = -(proximity - i);
    ASSERT_EQ(expected_result, difference.x);
    ASSERT_EQ(-(proximity-1), difference.y);
  }
}

TEST_F(TestInputAreaProximity, TestApproachingSignalVertical)
{
  int proximity = 10;
  AddInputAreaProximity(proximity);

  nux::Point difference;
  int expected_result;
  for (int i = 1; i <= proximity; i++)
  {
    MoveMouse(VIEW_WIDTH+(proximity-1), VIEW_HEIGHT+(proximity-i));
    difference = GetLastApprochingPoint();
    expected_result = -(proximity - i);
    ASSERT_EQ(-(proximity-1), difference.x);
    ASSERT_EQ(expected_result, difference.y);
  }
}

TEST_F(TestInputAreaProximity, TestApproachingSignalDiagonal)
{
  int proximity = 10;
  AddInputAreaProximity(proximity);

  nux::Point difference;
  int expected_result;
  for (int i = 1; i <= proximity; i++)
  {
    MoveMouse(VIEW_WIDTH+(proximity-i), VIEW_HEIGHT+(proximity-i));
    difference = GetLastApprochingPoint();
    expected_result = -(proximity - i);
    ASSERT_EQ(expected_result, difference.x);
    ASSERT_EQ(expected_result, difference.y);
  }
}

TEST_F(TestInputAreaProximity, TestBeyondSignal)
{
  AddInputAreaProximity();

  MoveMouseNear();
  ASSERT_FALSE(BeyondSignalRecived());

  MoveMouseBeyond();
  ASSERT_TRUE(BeyondSignalRecived());
}

TEST_F(TestInputAreaProximity, TestBeyondSignalEmitWhenLeavingWindow)
{
  AddInputAreaProximity();

  MoveMouseNear();
  ASSERT_FALSE(BeyondSignalRecived());

  MoveMouseBeyondWindow();
  ASSERT_TRUE(BeyondSignalRecived());
}

}
