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
#include "Nux/ProgramFramework/ProgramTemplate.h"
#include "Nux/ProgramFramework/TestView.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_automated_test_framework.h"

class FocusOnMouseDownTest: public ProgramTemplate
{
public:
  FocusOnMouseDownTest(const char* program_name, int window_width, int window_height, int program_life_span);
  ~FocusOnMouseDownTest();

  virtual void UserInterfaceSetup();
  
  nux::TestView* focus_view_;
  nux::TestView* no_focus_view_;
};

FocusOnMouseDownTest::FocusOnMouseDownTest(const char* program_name,
                                           int window_width,
                                           int window_height,
                                           int program_life_span)
: ProgramTemplate(program_name, window_width, window_height, program_life_span)
, focus_view_(nullptr)
, no_focus_view_(nullptr)
{
}

FocusOnMouseDownTest::~FocusOnMouseDownTest()
{
}

void FocusOnMouseDownTest::UserInterfaceSetup()
{
  nux::VLayout* main_layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  main_layout->SetSpaceBetweenChildren(10);
  main_layout->SetPadding(10, 10);
  
  focus_view_ = new nux::TestView(NUX_TRACKER_LOCATION);
  focus_view_->can_focus_ = true;
  focus_view_->SetAcceptKeyNavFocusOnMouseDown(true);
  main_layout->AddView(focus_view_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
    
  no_focus_view_ = new nux::TestView(NUX_TRACKER_LOCATION);
  no_focus_view_->can_focus_ = true;
  no_focus_view_->SetAcceptKeyNavFocusOnMouseDown(false);
  main_layout->AddView(no_focus_view_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);

  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(main_layout);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

FocusOnMouseDownTest* focus_on_mouse_down_test = nullptr;

void TestingThread(nux::NThread* /* thread */, void* user_data)
{
  while (focus_on_mouse_down_test->ReadyToGo() == false)
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

  test.TestReportMsg(focus_on_mouse_down_test->focus_view_, "Focus view created");
  test.TestReportMsg(focus_on_mouse_down_test->no_focus_view_, "No focus view created");

  // Move mouse to center of focus_view
  test.ViewSendMouseMotionToCenter(focus_on_mouse_down_test->focus_view_);

  // Mouse down/up on focus_view_
  test.ViewSendMouseClick(focus_on_mouse_down_test->focus_view_, 1);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(focus_on_mouse_down_test->focus_view_->has_focus_, "Mouse down: focus_view_ got the focus");
  test.ViewSendMouseUp(focus_on_mouse_down_test->focus_view_, 1);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(focus_on_mouse_down_test->focus_view_->has_focus_, "Mouse up: focus is still on focus_view_");
  
   // Move mouse to center of no_focus_view
  test.ViewSendMouseMotionToCenter(focus_on_mouse_down_test->no_focus_view_);

  // Mouse down/up on no_focus_view_
  test.ViewSendMouseDown(focus_on_mouse_down_test->no_focus_view_, 1);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(!focus_on_mouse_down_test->no_focus_view_->has_focus_, "Mouse down: no_focus_view_ did not take the focus");
  test.TestReportMsg(focus_on_mouse_down_test->focus_view_->has_focus_, "Mouse down: focus is still on focus_view_");
  test.ViewSendMouseUp(focus_on_mouse_down_test->no_focus_view_, 1);
  nux::SleepForMilliseconds(500);
  test.TestReportMsg(!focus_on_mouse_down_test->no_focus_view_->has_focus_, "Mouse up: no_focus_view_ still doesn't have the focus");
  test.TestReportMsg(focus_on_mouse_down_test->focus_view_->has_focus_, "Mouse up: focus is still on focus_view_");
  

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

  focus_on_mouse_down_test = new FocusOnMouseDownTest("Focus On Mouse Down Test", 300, 300, 15000);
  focus_on_mouse_down_test->Startup();
  focus_on_mouse_down_test->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(focus_on_mouse_down_test->GetWindowThread(), 
                                                           &TestingThread,
                                                           focus_on_mouse_down_test->GetWindowThread());

  test_thread->Start(focus_on_mouse_down_test);

  focus_on_mouse_down_test->Run();

  delete test_thread;
  delete focus_on_mouse_down_test;

  return 0;
}

