/*
 * Copyright 2011 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Andrea Azzarone <azzaronea@gmail.com>
 *
 */
 
#include "Nux/Nux.h"
#include "Nux/WindowThread.h"
#include "Nux/GridHLayout.h"
#include "Nux/TestFramework/NuxProgramTemplate.h"
#include "Nux/TestFramework/TestView.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_automated_test_framework.h"


// 0  1  2  3
// 4  5  6  7
// 8  9 10 

class HGridKeyNavigationTest: public NuxProgramTemplate
{
public:
  HGridKeyNavigationTest(const char* program_name, int window_width, int window_height, int program_life_span);
  ~HGridKeyNavigationTest();

  virtual void UserInterfaceSetup();
  
  TestView* CreateTile();
  void AddTile(TestView* tile);
  
  nux::GridHLayout* grid_layout_;
  TestView* tiles_[11];
};

HGridKeyNavigationTest::HGridKeyNavigationTest(const char* program_name,
                                               int window_width,
                                               int window_height,
                                               int program_life_span)
: NuxProgramTemplate(program_name, window_width, window_height, program_life_span)
{
}

HGridKeyNavigationTest::~HGridKeyNavigationTest()
{
}

TestView* HGridKeyNavigationTest::CreateTile()
{
  TestView* tile = new TestView();
  return tile;
}

void HGridKeyNavigationTest::AddTile(TestView* tile)
{
  grid_layout_->AddView(tile, 1);
}

void HGridKeyNavigationTest::UserInterfaceSetup()
{  
  grid_layout_ = new nux::GridHLayout(NUX_TRACKER_LOCATION);
  grid_layout_->ForceChildrenSize(true);
  grid_layout_->SetChildrenSize(100, 100);
  grid_layout_->EnablePartialVisibility(false);
  grid_layout_->SetPadding(20, 20);
  grid_layout_->SetSpaceBetweenChildren(10, 10);
  
  for (int i=0; i<11; ++i)
  {
    tiles_[i] = CreateTile();
    AddTile(tiles_[i]);
  }
  
  nux::GetWindowCompositor().SetKeyFocusArea(tiles_[0]);
  
  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(grid_layout_);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

HGridKeyNavigationTest* key_navigation_test = NULL;

void TestingThread(nux::NThread* thread, void* user_data)
{
  while (key_navigation_test->ReadyToGo() == false)
  {
    nuxDebugMsg("Waiting to start");
    nux::SleepForMilliseconds(300);
  }

  nux::SleepForMilliseconds(1000);

  nux::WindowThread* wnd_thread = static_cast<nux::WindowThread*>(user_data);

  NuxAutomatedTestFramework test(wnd_thread);

  test.Startup();

  // Set the mouse at coordinates (0, 0) (top-left corner) on the display
  test.PutMouseAt(0, 0);

  test.TestReportMsg(key_navigation_test->grid_layout_, "TestView created");
  test.TestReportMsg(key_navigation_test->tiles_[0]->has_focus_, "Top left tile has key focus");


  // Rigth key
  for (int i=0; i<10; ++i)
  {
    test.SendFakeKeyEvent(XK_Right, 0);
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(!key_navigation_test->tiles_[i]->has_focus_, "Right: key focus out");
    test.TestReportMsg(key_navigation_test->tiles_[i+1]->has_focus_, "Right: key focus in");
  }
  
  // Another right key, should do nothing
  test.SendFakeKeyEvent(XK_Right, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->tiles_[10]->has_focus_, "Right key, last element");
  
  // Left key
  for (int i=10; i>0; --i)
  {
    test.SendFakeKeyEvent(XK_Left, 0);
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(!key_navigation_test->tiles_[i]->has_focus_, "Left: key focus out");
    test.TestReportMsg(key_navigation_test->tiles_[i-1]->has_focus_, "Left: key focus in");
  }
  
  // Another left key, should do nothing
  test.SendFakeKeyEvent(XK_Left, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->tiles_[0]->has_focus_, "Left key, fist element");
  
  // Down key
  for (int i=0; i<2; ++i)
  {
    test.SendFakeKeyEvent(XK_Down, 0);
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(!key_navigation_test->tiles_[4*i]->has_focus_, "Down: key focus out");
    test.TestReportMsg(key_navigation_test->tiles_[4*(i+1)]->has_focus_, "Down: key focus in");
  }
  
  // Another down key, should do nothing
  test.SendFakeKeyEvent(XK_Down, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->tiles_[8]->has_focus_, "Down key, last row");
  
  // Up key
  for (int i=2; i>0; --i)
  {
    test.SendFakeKeyEvent(XK_Up, 0);
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(!key_navigation_test->tiles_[4*i]->has_focus_, "Up: key focus out");
    test.TestReportMsg(key_navigation_test->tiles_[4*(i-1)]->has_focus_, "Up: key focus in");
  }
  
  // Another down key, should do nothing
  test.SendFakeKeyEvent(XK_Up, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->tiles_[0]->has_focus_, "Up key, first row");

  if (test.WhenDoneTerminateProgram())
  {
    nux::SleepForMilliseconds(1000);
    wnd_thread->ExitMainLoop();
  }
  nuxDebugMsg("Exit testing thread");
}

int main(int argc, char** argv)
{
  int xstatus = XInitThreads();
  nuxAssertMsg(xstatus > 0, "XInitThreads has failed");

  key_navigation_test = new HGridKeyNavigationTest("Key navigation Test", 500, 400, 20000);
  key_navigation_test->Startup();
  key_navigation_test->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(key_navigation_test->GetWindowThread(), &TestingThread, key_navigation_test->GetWindowThread());

  test_thread->Start(key_navigation_test);

  key_navigation_test->Run();

  delete test_thread;
  delete key_navigation_test;

  return 0;
}

