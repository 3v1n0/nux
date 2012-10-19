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
#include "Nux/HLayout.h"
#include "Nux/ProgramFramework/ProgramTemplate.h"
#include "Nux/ProgramFramework/TestView.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_automated_test_framework.h"

unsigned int const NUM_VIEWS = 5;
unsigned int const ID_UNFOCUSABLE_VIEW = 2; // Please don't make this the last/first one :)

class HLayoutKeyNavigationTest : public ProgramTemplate
{
public:
  HLayoutKeyNavigationTest(const char* program_name, int window_width, int window_height, int program_life_span);
  ~HLayoutKeyNavigationTest();

  virtual void UserInterfaceSetup();
  
  nux::TestView* CreateView();
  void AddView(nux::TestView* view);
  
  nux::HLayout* hlayout_;
  nux::TestView* views_[NUM_VIEWS];
};

HLayoutKeyNavigationTest::HLayoutKeyNavigationTest(const char* program_name,
                                                   int window_width,
                                                   int window_height,
                                                   int program_life_span)
: ProgramTemplate(program_name, window_width, window_height, program_life_span)
{
}

HLayoutKeyNavigationTest::~HLayoutKeyNavigationTest()
{
}

nux::TestView* HLayoutKeyNavigationTest::CreateView()
{
  nux::TestView* view = new nux::TestView();
  return view;
}

void HLayoutKeyNavigationTest::AddView(nux::TestView* view)
{
  hlayout_->AddView(view, 1);
}

void HLayoutKeyNavigationTest::UserInterfaceSetup()
{  
  hlayout_ = new nux::HLayout(NUX_TRACKER_LOCATION);
  hlayout_->SetPadding(20, 20);
  hlayout_->SetSpaceBetweenChildren(10);
  
  for (unsigned int i=0; i<NUM_VIEWS; ++i)
  {
    views_[i] = CreateView();
    AddView(views_[i]);
  }
  
  views_[ID_UNFOCUSABLE_VIEW]->can_focus_ = false;
  nux::GetWindowCompositor().SetKeyFocusArea(views_[0]);
  
  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(hlayout_);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

HLayoutKeyNavigationTest* key_navigation_test = NULL;

void TestingThread(nux::NThread* /* thread */, void* user_data)
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

  test.TestReportMsg(key_navigation_test->hlayout_, "TestView created");
  test.TestReportMsg(key_navigation_test->views_[0]->has_focus_, "Top left tile has key focus");
  
  // Right key
  for (unsigned int i=0; i<NUM_VIEWS-1; ++i)
  {
    test.SendFakeKeyEvent(XK_Right, 0);
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(!key_navigation_test->views_[i]->has_focus_, "Right: key focus out");
    if ((i+1) == ID_UNFOCUSABLE_VIEW)
    {
      test.TestReportMsg(!key_navigation_test->views_[i+1]->has_focus_, "Right: key focus skipped");
      test.TestReportMsg(key_navigation_test->views_[i+2]->has_focus_, "Right: key focus in");
      ++i;
    }
    else
    {
      test.TestReportMsg(key_navigation_test->views_[i+1]->has_focus_, "Right: key focus in");
    }
      
  }
  
  // Another right key, should do nothing
  test.SendFakeKeyEvent(XK_Right, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->views_[NUM_VIEWS-1]->has_focus_, "Rigth key, last element");
  
  // Left key
  for (unsigned int i=NUM_VIEWS-1; i>0; --i)
  {
    test.SendFakeKeyEvent(XK_Left, 0);
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(!key_navigation_test->views_[i]->has_focus_, "Left: key focus out");
    if ((i-1) == ID_UNFOCUSABLE_VIEW)
    {
      test.TestReportMsg(!key_navigation_test->views_[i-1]->has_focus_, "Left: key focus skipped");
      test.TestReportMsg(key_navigation_test->views_[i-2]->has_focus_, "Left: key focus in");
      --i;
    }
    else
    {
      test.TestReportMsg(key_navigation_test->views_[i-1]->has_focus_, "Left: key focus in");
    }
  }
  
  // Another Left key, should do nothing
  test.SendFakeKeyEvent(XK_Left, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->views_[0]->has_focus_, "Left key, fist element");
  
   // Up key, should do nothing
  test.SendFakeKeyEvent(XK_Up, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->views_[0]->has_focus_, "Up key");
  
   // Down key, should do nothing
  test.SendFakeKeyEvent(XK_Down, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(key_navigation_test->views_[0]->has_focus_, "Down key");


  if (test.WhenDoneTerminateProgram())
  {
    nux::SleepForMilliseconds(1000);
    wnd_thread->ExitMainLoop();
  }
  nuxDebugMsg("Exit testing thread");
}

int main()
{
  XInitThreads();

  key_navigation_test = new HLayoutKeyNavigationTest("Key navigation Test", 500, 400, 8000);
  key_navigation_test->Startup();
  key_navigation_test->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(key_navigation_test->GetWindowThread(), &TestingThread, key_navigation_test->GetWindowThread());

  test_thread->Start(key_navigation_test);

  key_navigation_test->Run();

  delete test_thread;
  delete key_navigation_test;

  return 0;
}

