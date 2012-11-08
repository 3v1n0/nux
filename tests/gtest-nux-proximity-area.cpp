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
* Authored by: <brandon.schaefer@canonical.com>
*
*/


#include <gmock/gmock.h>

#include "Nux/Nux.h"
#include "Nux/ProximityArea.h"
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
  {
    nux::NuxInitialize(0);
    wnd_thread = nux::CreateNuxWindow("View Test", 300, 200,
                 nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

    for (int i = 0; i < num_prox_areas; i++)
      AddProximityArea();
  }

  ~MockProxyInstance()
  {
    while (!prox_areas_.empty())
      RemoveProximityArea();

    delete wnd_thread;
  }

  void AddProximityArea(int poximity = 10)
  {
    nux::TestView* test_view = new nux::TestView("");
    test_view->SetGeometry(nux::Geometry(0, 0, VIEW_WIDTH, VIEW_HEIGHT));

    nux::ProximityArea* prox_area = new nux::ProximityArea(test_view, poximity);

    prox_area->mouse_near.connect([&] (nux::Point) {
      near_signal_recived_ = true;
    });

    prox_area->mouse_beyond.connect([&] (nux::Point) {
      beyond_signal_recived_ = true;
    });

    prox_areas_.push(std::make_pair(prox_area, test_view));
  }

  void RemoveProximityArea()
  {
    if (!prox_areas_.empty())
    {
      auto prox_area = prox_areas_.top();
      prox_areas_.pop();

      delete prox_area.first;
      prox_area.second->UnReference();
    }
  }

  bool NearSignalRecived() const
  {
    return near_signal_recived_;
  }

  bool BeyondSignalRecived() const
  {
    return beyond_signal_recived_;
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
  std::stack<std::pair<nux::ProximityArea*, nux::TestView*> > prox_areas_;

  bool near_signal_recived_;
  bool beyond_signal_recived_;
};

TEST(TestProximityArea, TestProximityAreaAddsToList)
{
  MockProxyInstance mi;
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), 0);

  mi.AddProximityArea();
  mi.AddProximityArea();

  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), 2);
}

TEST(TestProximityArea, TestProximityRemovesFromList)
{
  int num_prox_areas = 10;
  MockProxyInstance mi(num_prox_areas);
  ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), num_prox_areas);

  for (int i = 0; i < num_prox_areas; i++)
  {
    mi.RemoveProximityArea();
    ASSERT_EQ(nux::GetWindowCompositor().GetProximityListSize(), num_prox_areas - (i + 1));
  }
}

TEST(TestProximityArea, TestProximityNearSignal)
{
  MockProxyInstance mi;
  int proximity = 10;

  mi.MoveMouse(VIEW_WIDTH+200, VIEW_HEIGHT+200);
  mi.AddProximityArea(proximity);

  mi.MoveMouse(VIEW_WIDTH+(proximity+1), VIEW_HEIGHT+(proximity+1));
  ASSERT_FALSE(mi.NearSignalRecived());

  mi.MoveMouse(VIEW_WIDTH+(proximity-1), VIEW_HEIGHT+(proximity-1));
  ASSERT_TRUE(mi.NearSignalRecived());
}

TEST(TestProximityArea, TestProximityBeyondSignal)
{
  MockProxyInstance mi;
  int proximity = 10;

  mi.MoveMouse(VIEW_WIDTH+200, VIEW_HEIGHT+200);
  mi.AddProximityArea(proximity);

  mi.MoveMouse(VIEW_WIDTH+(proximity-1), VIEW_HEIGHT+(proximity-1));
  ASSERT_FALSE(mi.BeyondSignalRecived());

  mi.MoveMouse(VIEW_WIDTH+(proximity+1), VIEW_HEIGHT+(proximity+1));
  ASSERT_TRUE(mi.BeyondSignalRecived());
}

}
