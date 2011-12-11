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
#include "Nux/VLayout.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_test_framework.h"
#include "nux_automated_test_framework.h"
#include "test-view.h"

class VLayoutKeyNavigationTest : public NuxTestFramework
{
public:
  VLayoutKeyNavigationTest(const char* program_name, int window_width, int window_height, int program_life_span);
  ~VLayoutKeyNavigationTest();

  virtual void UserInterfaceSetup();
  
  TestView* CreateView();
  void AddView(TestView* view);
  
  nux::VLayout* vlayout_;
  TestView* views_[11];
};

VLayoutKeyNavigationTest::VLayoutKeyNavigationTest(const char *program_name,
                                                   int window_width,
                                                   int window_height,
                                                   int program_life_span)
  : NuxTestFramework(program_name, window_width, window_height, program_life_span)
{
}

VLayoutKeyNavigationTest::~VLayoutKeyNavigationTest()
{
}

TestView* VLayoutKeyNavigationTest::CreateView()
{
  TestView* view = new TestView();
  return view;
}

void VLayoutKeyNavigationTest::AddView(TestView* view)
{
  vlayout_->AddView(view, 1);
}

void VLayoutKeyNavigationTest::UserInterfaceSetup()
{  
  vlayout_ = new nux::VLayout(NUX_TRACKER_LOCATION);
  //grid_layout_->ForceChildrenSize(true);
  //grid_layout_->SetChildrenSize(100, 100);
  //grid_layout_->EnablePartialVisibility(false);
  vlayout_->SetPadding(20, 20);
  vlayout_->SetSpaceBetweenChildren(10);
  
  for (int i=0; i<4; ++i)
  {
    views_[i] = CreateView();
    AddView(views_[i]);
  }
  
  nux::GetWindowCompositor().SetKeyFocusArea(views_[0]);
  
  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(vlayout_);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

VLayoutKeyNavigationTest *key_navigation_test = NULL;

void TestingThread(nux::NThread *thread, void *user_data)
{
  while (key_navigation_test->ReadyToGo() == false)
  {
    nuxDebugMsg("Waiting to start");
    nux::SleepForMilliseconds(300);
  }

  nux::SleepForMilliseconds(1000);

  nux::WindowThread *wnd_thread = static_cast<nux::WindowThread*>(user_data);

  NuxAutomatedTestFramework test(wnd_thread);

  test.Startup();

  // Set the mouse at coordinates (0, 0) (top-left corner) on the display
  test.PutMouseAt(0, 0);

  test.TestReportMsg(key_navigation_test->vlayout_, "TestView created");
  test.TestReportMsg(key_navigation_test->views_[0]->has_focus_, "Top left tile has key focus");
  
  // Down key
  for (int i=0; i<3; ++i)
  {
    test.SendFakeKeyEvent(XK_Down, 0);
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(!key_navigation_test->views_[i]->has_focus_, "Down: key focus out");
    test.TestReportMsg(key_navigation_test->views_[i+1]->has_focus_, "Down: key focus in");
  }
  
  // Another down key, should do nothing
  test.SendFakeKeyEvent(XK_Down, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->views_[3]->has_focus_, "Down key, last element");
  
  // Up key
  for (int i=3; i>0; --i)
  {
    test.SendFakeKeyEvent(XK_Up, 0);
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(!key_navigation_test->views_[i]->has_focus_, "Up: key focus out");
    test.TestReportMsg(key_navigation_test->views_[i-1]->has_focus_, "Up: key focus in");
  }
  
  // Another up key, should do nothing
  test.SendFakeKeyEvent(XK_Up, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->views_[0]->has_focus_, "Up key, fist element");
  
   // Keft key, should do nothing
  test.SendFakeKeyEvent(XK_Left, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->views_[0]->has_focus_, "Left key");
  
   // Right key, should do nothing
  test.SendFakeKeyEvent(XK_Right, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->views_[0]->has_focus_, "Right key");


  if (test.WhenDoneTerminateProgram())
  {
    nux::SleepForMilliseconds(1000);
    wnd_thread->NuxMainLoopQuit();
  }
  nuxDebugMsg("Exit testing thread");
}

int main(int argc, char **argv)
{
  int xstatus = XInitThreads();
  nuxAssertMsg(xstatus > 0, "XInitThreads has failed");

  key_navigation_test = new VLayoutKeyNavigationTest("Key navigation Test", 500, 400, 8000);
  key_navigation_test->Startup();
  key_navigation_test->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(key_navigation_test->GetWindowThread(), &TestingThread, key_navigation_test->GetWindowThread());

  test_thread->Start(key_navigation_test);

  key_navigation_test->Run();

  delete test_thread;
  delete key_navigation_test;

  return 0;
}

