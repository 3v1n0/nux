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
#include "test-scrollview.h"

class ScrollBarTest: public ProgramTemplate
{
public:
  ScrollBarTest(const char* program_name, int window_width, int window_height, int program_life_span);
  ~ScrollBarTest();

  virtual void UserInterfaceSetup();
  
  nux::VLayout* layout_;
  TestScrollView* scroll_view_;
  nux::VLayout* in_layout_;
  nux::TestView* test_view_;
};

ScrollBarTest::ScrollBarTest(const char* program_name,
                             int window_width,
                             int window_height,
                             int program_life_span)
: ProgramTemplate(program_name, window_width, window_height, program_life_span)
{
}

ScrollBarTest::~ScrollBarTest()
{
}

void ScrollBarTest::UserInterfaceSetup()
{  
  layout_ = new nux::VLayout(NUX_TRACKER_LOCATION);
  layout_->SetPadding(20, 20);

  scroll_view_ = new TestScrollView();
  layout_->AddView(scroll_view_);
  
  in_layout_ = new nux::VLayout(NUX_TRACKER_LOCATION);
  scroll_view_->SetLayout(in_layout_);
  
  test_view_ = new nux::TestView();
  test_view_->SetMinimumHeight(1000);
  in_layout_->AddView(test_view_);
  
  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(layout_);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

ScrollBarTest* scrollbar_test;

void TestingThread(nux::NThread* /* thread */, void* user_data)
{
  while (scrollbar_test->ReadyToGo() == false)
  {
    nuxDebugMsg("Waiting to start");
    nux::SleepForMilliseconds(300);
  }
  nux::SleepForMilliseconds(1000);

  nux::WindowThread* wnd_thread = static_cast<nux::WindowThread*>(user_data);
  NuxAutomatedTestFramework test(wnd_thread);
  test.Startup();

  test.TestReportMsg(scrollbar_test->scroll_view_, "TestScrollView created");
  
  scrollbar_test->scroll_view_->ResetEvents();
  test.ViewSendMouseMotionToCenter(scrollbar_test->scroll_view_);
  test.ViewSendMouseClick(scrollbar_test->scroll_view_, 4);
  nux::SleepForMilliseconds(1000);
  test.TestReportMsg(scrollbar_test->scroll_view_->registered_scroll_, "Scroll on the center of the scrollview");
  
  
  scrollbar_test->scroll_view_->ResetEvents();
  test.ViewSendMouseMotionToCenter(scrollbar_test->scroll_view_->GetVScrollbar());
  test.ViewSendMouseClick(scrollbar_test->scroll_view_->GetVScrollbar(), 4);
  nux::SleepForMilliseconds(1000);
  test.TestReportMsg(scrollbar_test->scroll_view_->registered_scroll_, "Scroll on the scrollbar");

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

  scrollbar_test = new ScrollBarTest("Scrollbar Test", 500, 400, 13000);
  scrollbar_test->Startup();
  scrollbar_test->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(scrollbar_test->GetWindowThread(), &TestingThread, scrollbar_test->GetWindowThread());

  test_thread->Start(scrollbar_test);

  scrollbar_test->Run();

  delete test_thread;
  delete scrollbar_test;

  return 0;
}

