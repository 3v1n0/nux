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
#include "Nux/TextEntryIM.h"
#include "Nux/InputMethodIBus.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_test_framework.h"
#include "nux_automated_test_framework.h"

class TextTextEntry: public NuxTestFramework
{
public:
  TextTextEntry(const char *program_name, int window_width, int window_height, int program_life_span);
  ~TextTextEntry();

  virtual void UserInterfaceSetup();

  void TextEntryClick(nux::TextEntryIM *text_entry);
  void ResetEvents();
  nux::TextEntryIM *text_entry_;

  bool clicked_;
};

TextTextEntry::TextTextEntry(const char *program_name,
  int window_width,
  int window_height,
  int program_life_span)
  : NuxTestFramework(program_name, window_width, window_height, program_life_span)
{
  ResetEvents();
  text_entry_ = NULL;
}

TextTextEntry::~TextTextEntry()
{
  
}

void TextTextEntry::ResetEvents()
{
  clicked_ = false;
}

void TextTextEntry::TextEntryClick(nux::TextEntryIM *text_entry)
{
  if (text_entry_ == text_entry)
  {
    clicked_ = true;
  }
}

void TextTextEntry::UserInterfaceSetup()
{
  nux::VLayout *main_layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  text_entry_ = new nux::TextEntryIM("NUX", NUX_TRACKER_LOCATION);
  //text_entry_->SetLabelFontSize(76);

  main_layout->AddView(text_entry_, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  main_layout->SetSpaceBetweenChildren(10);
  main_layout->SetContentDistribution(nux::MAJOR_POSITION_CENTER);

  //text_entry_->click.connect(sigc::mem_fun(this, &TextTextEntry::ButtonClick));

  //nux::IMEContext* ime = nux::IMEContext::Create(text_entry_);

  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(main_layout);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

TextTextEntry *test_textentry = NULL;

void TestingThread(nux::NThread *thread, void *user_data)
{
  while (test_textentry->ReadyToGo() == false)
  {
    nuxDebugMsg("Waiting to start");
    nux::SleepForMilliseconds(300);
  }

  nux::SleepForMilliseconds(1300);

  nux::WindowThread *wnd_thread = static_cast<nux::WindowThread*>(user_data);

  NuxAutomatedTestFramework test(wnd_thread);

  test.Startup();

  test.TestReportMsg(test_textentry->text_entry_, "Button created");

  test_textentry->ResetEvents();
  test.ViewSendMouseMotionToCenter(test_textentry->text_entry_);
  test.ViewSendMouseClick(0, 1);

  nux::SleepForMilliseconds(1000);
  test.TestReportMsg(test_textentry->clicked_, "Button clicked");

  if (test.WhenDoneTerminateProgram())
  {
    wnd_thread->ExitMainLoop();
  }
  nuxDebugMsg("Exit testing thread");
}

int main(int argc, char **argv)
{
  int xstatus = XInitThreads();
  nuxAssertMsg(xstatus > 0, "XInitThreads has failed");

  test_textentry = new TextTextEntry("Button XTest", 300, 300, 0);
  test_textentry->Startup();
  test_textentry->UserInterfaceSetup();

  nux::SystemThread *test_thread = nux::CreateSystemThread(NULL, &TestingThread, test_textentry->GetWindowThread());

  //test_thread->Start(test_textentry);

  test_textentry->Run();

  delete test_thread;
  delete test_textentry;

  //nuxDebugMsg("Exit program");
  return 0;
}

