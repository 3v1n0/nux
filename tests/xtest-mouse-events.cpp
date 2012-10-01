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


class EventsTest: public ProgramTemplate
{
public:
  EventsTest(const char* program_name, int window_width, int window_height, int program_life_span);
  ~EventsTest();

  virtual void UserInterfaceSetup();
  
  void ResetEvents();
  nux::TestView* test_view_;
};

EventsTest::EventsTest(const char* program_name,
  int window_width,
  int window_height,
  int program_life_span)
: ProgramTemplate(program_name, window_width, window_height, program_life_span)
{
  ResetEvents();
  test_view_ = NULL;
}

EventsTest::~EventsTest()
{
  
}

void EventsTest::ResetEvents()
{
  if (test_view_)
    test_view_->ResetEvents();
}

void EventsTest::UserInterfaceSetup()
{
  nux::VLayout* main_layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  test_view_ = new nux::TestView(NUX_TRACKER_LOCATION);

  main_layout->AddView(test_view_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  main_layout->SetPadding(10, 10);

  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(main_layout);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

EventsTest* event_test = NULL;

void TestingThread(nux::NThread* /* thread */, void* user_data)
{
  while (event_test->ReadyToGo() == false)
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

  test.TestReportMsg(event_test->test_view_, "TestView created");

  event_test->ResetEvents();
  test.ViewSendMouseMotionToCenter(event_test->test_view_);

  test.TestReportMsg(event_test->test_view_->registered_mouse_move_, "Mouse move event");
  test.TestReportMsg(event_test->test_view_->registered_mouse_enter_, "Mouse enter event");
  test.ViewSendMouseClick(0, 1);

  test.TestReportMsg(event_test->test_view_->registered_mouse_down_, "Mouse down event");
  test.TestReportMsg(event_test->test_view_->registered_mouse_up_, "Mouse up event");
  test.TestReportMsg(event_test->test_view_->registered_mouse_click_, "Mouse click event");

  test.ViewSendMouseDrag(event_test->test_view_, 1, 0, 0, 50, 50);
  test.TestReportMsg(event_test->test_view_->registered_mouse_click_, "Mouse drag event");

  test.ViewSendMouseMotionTo(event_test->test_view_, -5, -5);
  test.TestReportMsg(event_test->test_view_->registered_mouse_leave_, "Mouse leave event");

  event_test->test_view_->EnableDoubleClick(true);
  test.ViewSendMouseDoubleClick(event_test->test_view_, 1);
  test.TestReportMsg(event_test->test_view_->registered_mouse_double_click_, "Mouse double click event");

  event_test->ResetEvents();
  // Deactivate double click and test if the view still receives it.
  event_test->test_view_->EnableDoubleClick(false);
  test.ViewSendMouseDoubleClick(event_test->test_view_, 1);
  test.TestReportMsg(!event_test->test_view_->registered_mouse_double_click_, "Mouse double click de-activated");

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

  event_test = new EventsTest("Events Test", 300, 300, 20000);
  event_test->Startup();
  event_test->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(event_test->GetWindowThread(), &TestingThread, event_test->GetWindowThread());

  test_thread->Start(event_test);

  event_test->Run();

  delete test_thread;
  delete event_test;

  return 0;
}

