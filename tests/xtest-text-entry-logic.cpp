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
#include "Nux/TextEntry.h"
#include "Nux/InputMethodIBus.h"
#include "Nux/ProgramFramework/ProgramTemplate.h"
#include "Nux/ProgramFramework/TestView.h"
#include "Nux/ProgramFramework/TestTextEntry.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_automated_test_framework.h"

class TestTextEntry: public ProgramTemplate
{
public:
  TestTextEntry(const char* program_name, int window_width, int window_height, int program_life_span);
  ~TestTextEntry();

  virtual void UserInterfaceSetup();

  void ResetEvents();

  nux::TestTextEntry* text_entry_;

};

TestTextEntry::TestTextEntry(const char* program_name,
  int window_width,
  int window_height,
  int program_life_span)
  : ProgramTemplate(program_name, window_width, window_height, program_life_span)
  , text_entry_(NULL)
{
  ResetEvents();
}

TestTextEntry::~TestTextEntry()
{
  
}

void TestTextEntry::ResetEvents()
{
  if (text_entry_)
    text_entry_->ResetEvents();
}

void TestTextEntry::UserInterfaceSetup()
{
  nux::VLayout* main_layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  text_entry_ = new nux::TestTextEntry(NUX_TRACKER_LOCATION);
  text_entry_->SetFontSize(76);

  main_layout->AddView(text_entry_, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  main_layout->SetSpaceBetweenChildren(10);
  main_layout->SetContentDistribution(nux::MAJOR_POSITION_CENTER);

  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(main_layout);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

TestTextEntry* test_textentry = NULL;

void TestingThread(nux::NThread* /* thread */, void* user_data)
{
  while (test_textentry->ReadyToGo() == false)
  {
    nuxDebugMsg("Waiting to start");
    nux::SleepForMilliseconds(300);
  }

  nux::SleepForMilliseconds(1300);

  nux::WindowThread* wnd_thread = static_cast<nux::WindowThread*>(user_data);

  NuxAutomatedTestFramework test(wnd_thread);

  test.Startup();

  test.TestReportMsg(test_textentry->text_entry_, "TextEntry created");

  test_textentry->ResetEvents();
  test.ViewSendMouseMotionToCenter(test_textentry->text_entry_);

  test.TestReportMsg(test_textentry->text_entry_->HasKeyFocus() == false, "Focus test");
  test.TestReportMsg(test_textentry->text_entry_->IsInTextInputMode() == false, "Text input mode test");
  test.ViewSendMouseClick(0, 1);
  test.TestReportMsg(test_textentry->text_entry_->HasKeyFocus() == true, "Focus test");
  test.TestReportMsg(test_textentry->text_entry_->IsInTextInputMode() == false, "Text input mode test");

  // Type "Nux"
  // The cursor is at the end of the line
  // Simulate CTRL+A to select the entire text
  // Simulate DELETE key to delete the text
  {
    test.ViewSendString("Nux");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Nux", "Typed \"Nux\"");

    test.TestReportMsg(test_textentry->text_entry_->IsInTextInputMode() == true, "Text input mode test");

    test.ViewSendCtrlA();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetTextSelection() == "Nux", "Selection is \"Nux\"");

    test.ViewSendDelete();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "TextEntry is empty");
  }

  if (test.WhenDoneTerminateProgram())
  {
    wnd_thread->ExitMainLoop();
  }
  nuxDebugMsg("Exit testing thread");
}

int main()
{
  XInitThreads();

  test_textentry = new TestTextEntry("Text Entry", 600, 200, 10000);
  test_textentry->Startup();
  test_textentry->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(NULL, &TestingThread, test_textentry->GetWindowThread());

  test_thread->Start(test_textentry);

  test_textentry->Run();

  delete test_thread;
  delete test_textentry;

  return 0;
}

