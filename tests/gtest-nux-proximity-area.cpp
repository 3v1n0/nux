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

#include "Nux/Nux.h"
#include "Nux/InputAreaProximity.h"
#include "Nux/WindowCompositor.h"
#include "Nux/ProgramFramework/TestView.h"

#include <stack>

namespace
{

const int VIEW_WIDTH = 500;
const int VIEW_HEIGHT = 500;

class MockProxyInstance
{
public:
  MockProxyInstance(int num_prox_areas = 0)
    : near_signal_recived_(false)
    , beyond_signal_recived_(false)
    , approaching_signal_recived_(false)
  {
    nux::NuxInitialize(0);
    wnd_thread = nux::CreateNuxWindow("View Test", 300, 200,
                 nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

    for (int i = 0; i < num_prox_areas; i++)
      AddInputAreaProximity();
  }

  ~MockProxyInstance()
  {
    while (!prox_areas_.empty())
      RemoveInputAreaProximity();

    delete wnd_thread;
  }

  void AddInputAreaProximity(int poximity = 10)
  {
    nux::TestView* test_view = new nux::TestView("");
    test_view->SetGeometry(nux::Geometry(0, 0, VIEW_WIDTH, VIEW_HEIGHT));

    nux::InputAreaProximity* prox_area = new nux::InputAreaProximity(test_view, poximity);

    prox_area->mouse_near.connect([&] (const nux::Point&) {
      near_signal_recived_ = true;
    });

    prox_area->mouse_beyond.connect([&] (const nux::Point&) {
      beyond_signal_recived_ = true;
    });

    prox_area->mouse_approaching.connect([&] (const nux::Point&, const nux::Point& difference) {
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

      delete prox_area.first;
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

  void MoveMouse(int x, int y)
  {
    nux::Event event;
    event.type = nux::NUX_MOUSE_MOVE;
    event.x = x;
    event.y = y;
    nux::GetWindowCompositor().ProcessEvent(event);
  }

private:
  nux::WindowThread* wnd_thread;
  std::stack<std::pair<nux::InputAreaProximity*, nux::TestView*> > prox_areas_;

  nux::Point last_approaching_point_;

  bool near_signal_recived_;
  bool beyond_signal_recived_;
  bool approaching_signal_recived_;
};

TEST(TestInputAreaProximity, TestProximityAreaAddsNullToList)
{
  MockProxyInstance mi;
  int prox_size = nux::GetWindowCompositor().GetProximityListSize();
  nux::InputAreaProximity* prox_area = new nux::InputAreaProximity(NULL, 10);

  mi.MoveMouse(500, 500);
  mi.MoveMouse(100, 100);

  delete prox_area;
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), prox_size);
}

TEST(TestInputAreaProximity, TestProximityAreaHandlesDeletedInputArea)
{
  MockProxyInstance mi;

  nux::TestView* test_view = new nux::TestView("");
  test_view->SetGeometry(nux::Geometry(0, 0, VIEW_WIDTH, VIEW_HEIGHT));

  const int prox_size = nux::GetWindowCompositor().GetProximityListSize();
  nux::InputAreaProximity* prox_area = new nux::InputAreaProximity(test_view, 10);
  test_view->UnReference();

  mi.MoveMouse(500, 500);
  mi.MoveMouse(100, 100);

  delete prox_area;
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), prox_size);
}

TEST(TestInputAreaProximity, TestProximityAreaAddsNullProximity)
{
  MockProxyInstance mi;
  const int prox_size = nux::GetWindowCompositor().GetProximityListSize();

  nux::GetWindowThread()->GetWindowCompositor().AddAreaInProximityList(NULL);
  nux::GetWindowThread()->GetWindowCompositor().AddAreaInProximityList(NULL);
  nux::GetWindowThread()->GetWindowCompositor().AddAreaInProximityList(NULL);
  nux::GetWindowThread()->GetWindowCompositor().AddAreaInProximityList(NULL);

  mi.MoveMouse(500, 500);
  mi.MoveMouse(100, 100);

  nux::GetWindowThread()->GetWindowCompositor().RemoveAreaInProximityList(NULL);
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), prox_size);
}

TEST(TestInputAreaProximity, TestProximityAreaAddsToList)
{
  MockProxyInstance mi;
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), 0);

  mi.AddInputAreaProximity();
  mi.AddInputAreaProximity();

  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), 2);
}

TEST(TestInputAreaProximity, TestProximityRemovesFromList)
{
  int num_prox_areas = 10;
  MockProxyInstance mi(num_prox_areas);
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), num_prox_areas);

  for (int i = 0; i < num_prox_areas; i++)
  {
    mi.RemoveInputAreaProximity();
    ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), num_prox_areas - (i + 1));
  }
}

TEST(TestInputAreaProximity, TestProximityNearSignal)
{
  MockProxyInstance mi;
  int proximity = 10;

  mi.MoveMouse(VIEW_WIDTH+200, VIEW_HEIGHT+200);
  mi.AddInputAreaProximity(proximity);

  mi.MoveMouse(VIEW_WIDTH+(proximity+1), VIEW_HEIGHT+(proximity+1));
  ASSERT_FALSE(mi.NearSignalRecived());

  mi.MoveMouse(VIEW_WIDTH+(proximity-1), VIEW_HEIGHT+(proximity-1));
  ASSERT_TRUE(mi.NearSignalRecived());
}

TEST(TestInputAreaProximity, TestProximityApproachingSignal)
{
  MockProxyInstance mi;
  int proximity = 10;

  mi.MoveMouse(VIEW_WIDTH+200, VIEW_HEIGHT+200);
  mi.AddInputAreaProximity(proximity);

  mi.MoveMouse(VIEW_WIDTH+(proximity+1), VIEW_HEIGHT+(proximity+1));
  ASSERT_FALSE(mi.ApproachingSignalRecived());

  mi.MoveMouse(VIEW_WIDTH+(proximity/2), VIEW_HEIGHT+(proximity/2));
  ASSERT_TRUE(mi.ApproachingSignalRecived());
}

TEST(TestInputAreaProximity, TestProximityApproachingDoesNotEmitInsideArea)
{
  MockProxyInstance mi;
  int proximity = 10;

  mi.AddInputAreaProximity(proximity);

  mi.MoveMouse(VIEW_WIDTH/2, VIEW_HEIGHT/2);
  mi.ResetSignalRecivedValues();

  mi.MoveMouse(VIEW_WIDTH/4, VIEW_HEIGHT/4);
  ASSERT_FALSE(mi.ApproachingSignalRecived());

  mi.MoveMouse(VIEW_WIDTH+proximity/2, VIEW_HEIGHT+proximity/2);
  ASSERT_TRUE(mi.ApproachingSignalRecived());
}

TEST(TestInputAreaProximity, TestProximityApproachingSignalHorizontal)
{
  MockProxyInstance mi;
  int proximity = 10;

  mi.MoveMouse(VIEW_WIDTH+200, VIEW_HEIGHT+200);
  mi.AddInputAreaProximity(proximity);

  nux::Point difference;
  int expected_result;
  for (int i = 1; i <= proximity; i++)
  {
    mi.MoveMouse(VIEW_WIDTH+(proximity-i), VIEW_HEIGHT+(proximity-1));

    difference = mi.GetLastApprochingPoint();
    expected_result = -(proximity - i);
    ASSERT_EQ(expected_result, difference.x);
    ASSERT_EQ(-(proximity-1), difference.y);
  }
}

TEST(TestInputAreaProximity, TestProximityApproachingSignalVertical)
{
  MockProxyInstance mi;
  int proximity = 10;

  mi.MoveMouse(VIEW_WIDTH+200, VIEW_HEIGHT+200);
  mi.AddInputAreaProximity(proximity);

  nux::Point difference;
  int expected_result;
  for (int i = 1; i <= proximity; i++)
  {
    mi.MoveMouse(VIEW_WIDTH+(proximity-1), VIEW_HEIGHT+(proximity-i));
    difference = mi.GetLastApprochingPoint();
    expected_result = -(proximity - i);
    ASSERT_EQ(-(proximity-1), difference.x);
    ASSERT_EQ(expected_result, difference.y);
  }
}

TEST(TestInputAreaProximity, TestProximityApproachingSignalDiagonal)
{
  MockProxyInstance mi;
  int proximity = 10;

  mi.MoveMouse(VIEW_WIDTH+200, VIEW_HEIGHT+200);
  mi.AddInputAreaProximity(proximity);

  nux::Point difference;
  int expected_result;
  for (int i = 1; i <= proximity; i++)
  {
    mi.MoveMouse(VIEW_WIDTH+(proximity-i), VIEW_HEIGHT+(proximity-i));
    difference = mi.GetLastApprochingPoint();
    expected_result = -(proximity - i);
    ASSERT_EQ(expected_result, difference.x);
    ASSERT_EQ(expected_result, difference.y);
  }
}

TEST(TestInputAreaProximity, TestProximityBeyondSignal)
{
  MockProxyInstance mi;
  int proximity = 10;

  mi.MoveMouse(VIEW_WIDTH+200, VIEW_HEIGHT+200);
  mi.AddInputAreaProximity(proximity);

  mi.MoveMouse(VIEW_WIDTH+(proximity-1), VIEW_HEIGHT+(proximity-1));
  ASSERT_FALSE(mi.BeyondSignalRecived());

  mi.MoveMouse(VIEW_WIDTH+(proximity+1), VIEW_HEIGHT+(proximity+1));
  ASSERT_TRUE(mi.BeyondSignalRecived());
}

}
