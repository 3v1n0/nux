/*
 * Copyright 2010 Inalogic Inc.
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
#include "Nux/Layout.h"
#include "Nux/ProgramFramework/ProgramTemplate.h"
#include "Nux/ProgramFramework/TestView.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_automated_test_framework.h"

class ButtonEventsTest: public ProgramTemplate
{
public:
  ButtonEventsTest(const char* program_name, int window_width, int window_height, int program_life_span);
  ~ButtonEventsTest();

  virtual void UserInterfaceSetup();
  
  void ResetEvents();
  nux::TestView* test_view_;
};

ButtonEventsTest::ButtonEventsTest(const char* program_name,
  int window_width,
  int window_height,
  int program_life_span)
: ProgramTemplate(program_name, window_width, window_height, program_life_span)
{
  ResetEvents();
  test_view_ = NULL;
}

ButtonEventsTest::~ButtonEventsTest()
{
  
}

void ButtonEventsTest::ResetEvents()
{
  if (test_view_)
    test_view_->ResetEvents();
}

void ButtonEventsTest::UserInterfaceSetup()
{
  nux::VLayout* main_layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  test_view_ = new nux::TestView(NUX_TRACKER_LOCATION);

  main_layout->AddView(test_view_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  main_layout->SetPadding(10, 10);

  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(main_layout);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

ButtonEventsTest* button_event_test = NULL;

void TestingThread(nux::NThread* /* thread */, void* user_data)
{
  while (button_event_test->ReadyToGo() == false)
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

  test.TestReportMsg(button_event_test->test_view_, "TestView created");

  
  // Move mouse to center of the view
  test.ViewSendMouseMotionToCenter(button_event_test->test_view_);

  // Mouse click with button 1
  button_event_test->ResetEvents();
  test.ViewSendMouseClick(button_event_test->test_view_, 1);
  test.TestReportMsg(button_event_test->test_view_->registered_mouse_down_, "Mouse button 1 down");
  test.TestReportMsg(button_event_test->test_view_->registered_mouse_up_, "Mouse button 1 up");

  // Mouse click with button 2
  button_event_test->ResetEvents();
  test.ViewSendMouseClick(button_event_test->test_view_, 2);
  test.TestReportMsg(button_event_test->test_view_->registered_mouse_down_, "Mouse button 2 down");
  test.TestReportMsg(button_event_test->test_view_->registered_mouse_up_, "Mouse button 2 up");

  // Mouse click with button 3
  button_event_test->ResetEvents();
  test.ViewSendMouseClick(button_event_test->test_view_, 3);
  test.TestReportMsg(button_event_test->test_view_->registered_mouse_down_, "Mouse button 3 down");
  test.TestReportMsg(button_event_test->test_view_->registered_mouse_up_, "Mouse button 3 up");

  // Mouse click with button 4
  button_event_test->ResetEvents();
  test.ViewSendMouseClick(button_event_test->test_view_, 4);
  test.TestReportMsg(!button_event_test->test_view_->registered_mouse_down_, "Mouse button 4 down");
  test.TestReportMsg(!button_event_test->test_view_->registered_mouse_up_, "Mouse button 4 up");

  // Mouse click with button 5
  button_event_test->ResetEvents();
  test.ViewSendMouseClick(button_event_test->test_view_, 5);
  test.TestReportMsg(!button_event_test->test_view_->registered_mouse_down_, "Mouse button 5 down");
  test.TestReportMsg(!button_event_test->test_view_->registered_mouse_up_, "Mouse button 5 up");

  // Mouse click with button 6
  button_event_test->ResetEvents();
  test.ViewSendMouseClick(button_event_test->test_view_, 6);
  test.TestReportMsg(!button_event_test->test_view_->registered_mouse_down_, "Mouse button 6 down");
  test.TestReportMsg(!button_event_test->test_view_->registered_mouse_up_, "Mouse button 6 up");

  // Mouse click with button 7
  button_event_test->ResetEvents();
  test.ViewSendMouseClick(button_event_test->test_view_, 7);
  test.TestReportMsg(!button_event_test->test_view_->registered_mouse_down_, "Mouse button 7 down");
  test.TestReportMsg(!button_event_test->test_view_->registered_mouse_up_, "Mouse button 7 up");

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

  button_event_test = new ButtonEventsTest("Events Test", 300, 300, 30000);
  button_event_test->Startup();
  button_event_test->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(button_event_test->GetWindowThread(), &TestingThread, button_event_test->GetWindowThread());

  test_thread->Start(button_event_test);

  button_event_test->Run();

  delete test_thread;
  delete button_event_test;

  return 0;
}

