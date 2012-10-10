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
#include "Nux/Button.h"
#include "Nux/ProgramFramework/ProgramTemplate.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_automated_test_framework.h"

class TestButton: public ProgramTemplate
{
public:
  TestButton(const char* program_name, int window_width, int window_height, int program_life_span);
  ~TestButton();

  virtual void UserInterfaceSetup();

  void ButtonClick(nux::Button* button);
  void ResetEvents();
  nux::Button* button_;

  bool clicked_;
};

TestButton::TestButton(const char* program_name,
  int window_width,
  int window_height,
  int program_life_span)
: ProgramTemplate(program_name, window_width, window_height, program_life_span)
{
  ResetEvents();
  button_ = NULL;
}

TestButton::~TestButton()
{
  
}

void TestButton::ResetEvents()
{
  clicked_ = false;
}

void TestButton::ButtonClick(nux::Button* button)
{
  if (button_ == button)
  {
    clicked_ = true;
  }
}

void TestButton::UserInterfaceSetup()
{
  nux::VLayout* main_layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  button_ = new nux::Button("NUX", NUX_TRACKER_LOCATION);
  button_->SetLabelFontSize(76);

  main_layout->AddView(button_, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
  main_layout->SetVerticalInternalMargin(10);
  main_layout->SetContentDistribution(nux::MAJOR_POSITION_CENTER);

  button_->click.connect(sigc::mem_fun(this, &TestButton::ButtonClick));

  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(main_layout);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

TestButton* test_button = NULL;

void TestingThread(nux::NThread* /* thread */, void* user_data)
{
  while (test_button->ReadyToGo() == false)
  {
    nuxDebugMsg("Waiting to start");
    nux::SleepForMilliseconds(300);
  }

  nux::SleepForMilliseconds(1300);

  nux::WindowThread* wnd_thread = static_cast<nux::WindowThread*>(user_data);

  NuxAutomatedTestFramework test(wnd_thread);

  test.Startup();

  test.TestReportMsg(test_button->button_, "Button created");

  test_button->ResetEvents();
  test.ViewSendMouseMotionToCenter(test_button->button_);
  test.ViewSendMouseClick(0, 1);

  nux::SleepForMilliseconds(1000);
  test.TestReportMsg(test_button->clicked_, "Button clicked");

  if (test.WhenDoneTerminateProgram())
  {
    wnd_thread->ExitMainLoop();
  }
  nuxDebugMsg("Exit testing thread");
}

int main()
{
  XInitThreads();

  test_button = new TestButton("Button XTest", 300, 300, 10000);
  test_button->Startup();
  test_button->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(NULL, &TestingThread, test_button->GetWindowThread());

  test_thread->Start(test_button);

  test_button->Run();

  delete test_thread;
  delete test_button;

  //nuxDebugMsg("Exit program");
  return 0;
}

