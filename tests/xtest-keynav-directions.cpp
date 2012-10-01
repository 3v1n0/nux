/*
 * Copyright 2012 Inalogic Inc.
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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */
 
#include "Nux/Nux.h"
#include "Nux/WindowThread.h"
#include "Nux/VLayout.h"
#include "Nux/ProgramFramework/ProgramTemplate.h"
#include "Nux/ProgramFramework/TestView.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_automated_test_framework.h"

class KeyNavDirectionTest: public ProgramTemplate
{
public:
  KeyNavDirectionTest(const char *program_name, int window_width, int window_height, int program_life_span);
  ~KeyNavDirectionTest();

  virtual void UserInterfaceSetup();
  
  nux::TestView* view0_;
  nux::TestView* view1_;
  nux::TestView* view2_;
  nux::TestView* view3_;

};

KeyNavDirectionTest::KeyNavDirectionTest(const char* program_name,
                                           int window_width,
                                           int window_height,
                                           int program_life_span)
  : ProgramTemplate(program_name, window_width, window_height, program_life_span)
  , view0_(NULL)
  , view1_(NULL)
  , view2_(NULL)
  , view3_(NULL)  
{
}

KeyNavDirectionTest::~KeyNavDirectionTest()
{
}

void KeyNavDirectionTest::UserInterfaceSetup()
{
  nux::HLayout* main_layout = new nux::HLayout(NUX_TRACKER_LOCATION);
  main_layout->SetSpaceBetweenChildren(10);
  main_layout->SetPadding(10, 10);
  
  nux::VLayout* layout0 = new nux::VLayout(NUX_TRACKER_LOCATION);
  nux::VLayout* layout1 = new nux::VLayout(NUX_TRACKER_LOCATION);
  layout0->SetSpaceBetweenChildren(10);
  layout1->SetSpaceBetweenChildren(10);

  view0_ = new nux::TestView(NUX_TRACKER_LOCATION);
  view0_->can_focus_ = true;
  view0_->SetAcceptKeyNavFocusOnMouseDown(true);
  layout0->AddView(view0_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
    
  view1_ = new nux::TestView(NUX_TRACKER_LOCATION);
  view1_->can_focus_ = true;
  view1_->SetAcceptKeyNavFocusOnMouseDown(true);
  layout0->AddView(view1_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);

  view2_ = new nux::TestView(NUX_TRACKER_LOCATION);
  view2_->can_focus_ = true;
  view2_->SetAcceptKeyNavFocusOnMouseDown(true);
  layout1->AddView(view2_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
    
  view3_ = new nux::TestView(NUX_TRACKER_LOCATION);
  view3_->can_focus_ = true;
  view3_->SetAcceptKeyNavFocusOnMouseDown(true);
  layout1->AddView(view3_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);

  main_layout->AddLayout(layout0, 1);
  main_layout->AddLayout(layout1, 1);

  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(main_layout);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

KeyNavDirectionTest* keynav_direction_test = NULL;

void TestingThread(nux::NThread* /* thread */, void* user_data)
{
  while (keynav_direction_test->ReadyToGo() == false)
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

  // Move mouse to center of view0_
  test.ViewSendMouseMotionToCenter(keynav_direction_test->view0_);

  // Mouse down/up on view0_
  test.ViewSendMouseClick(keynav_direction_test->view0_, 1);
  
  nux::SleepForMilliseconds(500);
  test.SendFakeKeyEvent(XK_Down, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(keynav_direction_test->view1_->has_focus_ && (keynav_direction_test->view1_->key_nav_direction_ == nux::KEY_NAV_DOWN),
   "View1 received key nav from top");
  
  test.SendFakeKeyEvent(XK_Up, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(keynav_direction_test->view0_->has_focus_ && (keynav_direction_test->view0_->key_nav_direction_ == nux::KEY_NAV_UP),
   "View0 received key nav from bottom");
    
  test.SendFakeKeyEvent(XK_Right, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(keynav_direction_test->view2_->has_focus_ && (keynav_direction_test->view0_->key_nav_direction_ == nux::KEY_NAV_RIGHT),
   "View2 received key nav from left");
   
  test.SendFakeKeyEvent(XK_Down, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(keynav_direction_test->view3_->has_focus_ && (keynav_direction_test->view3_->key_nav_direction_ == nux::KEY_NAV_DOWN),
   "View3 received key nav from top");
   
  test.SendFakeKeyEvent(XK_Up, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(keynav_direction_test->view2_->has_focus_ && (keynav_direction_test->view2_->key_nav_direction_ == nux::KEY_NAV_UP),
   "View2 received key nav from bottom");
   
  test.SendFakeKeyEvent(XK_Left, 0);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(keynav_direction_test->view0_->has_focus_ && (keynav_direction_test->view0_->key_nav_direction_ == nux::KEY_NAV_LEFT),
   "View0 received key nav from right");
                
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

  keynav_direction_test = new KeyNavDirectionTest("Automated: KeyNav Directions", 300, 300, 15000);
  keynav_direction_test->Startup();
  keynav_direction_test->UserInterfaceSetup();

  nux::SystemThread *test_thread = nux::CreateSystemThread(keynav_direction_test->GetWindowThread(), 
                                                           &TestingThread,
                                                           keynav_direction_test->GetWindowThread());

  test_thread->Start(keynav_direction_test);

  keynav_direction_test->Run();

  delete test_thread;
  delete keynav_direction_test;

  return 0;
}

