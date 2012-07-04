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

#include "Nux.h"
#include "VLayout.h"
#include "HLayout.h"
#include "WindowThread.h"
#include "TextEntry.h"
#include "ProgramTemplate.h"


ProgramTemplate::ProgramTemplate(const char* program_name,
  int window_width,
  int window_height,
  int program_life_span)
{
  ready_to_go_ = false;
  window_width_ = window_width;
  window_height_ = window_height;

  if (window_width_ < 100)
    window_width_ = 100;

  if (window_height_ < 100)
    window_height_ = 100;

  timeout_signal_ = NULL;
  window_thread_ = NULL;
  program_name_ = program_name;
  program_life_span_ = program_life_span;

  if (program_life_span_ > 0 && program_life_span_ < 1000)
  {
    // Minimum life span is 1 second.
    program_life_span_ = 1000;
  }
}

ProgramTemplate::~ProgramTemplate()
{
  if (window_thread_)
    delete window_thread_;
}

void ProgramTemplate::Startup()
{
  nux::NuxInitialize(0);
  window_thread_ = nux::CreateGUIThread(program_name_.c_str(), window_width_, window_height_, NULL, NULL, NULL);

  window_thread_->window_configuration.connect(sigc::mem_fun(this, &ProgramTemplate::WaitForConfigureEvent));
}

void ProgramTemplate::UserInterfaceSetup()
{
  // nux::VLayout *MainVLayout = new nux::VLayout(NUX_TRACKER_LOCATION);
  // nux::TextEntry *text_entry_0 = new nux::TextEntry(TEXT("0123456789abcdefghij"), NUX_TRACKER_LOCATION);

  // MainVLayout->AddView(text_entry_0, 0, nux::eCenter, nux::eFull);
  // MainVLayout->SetVerticalInternalMargin(10);
  // MainVLayout->SetContentDistribution(nux::eStackCenter);

  // nux::GetWindowThread()->SetLayout(MainVLayout);
  // nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  // window_thread_->SetWindowBackgroundPaintLayer(&background);
}

void ProgramTemplate::Run()
{
  if (window_thread_ == NULL)
    return;

  if (program_life_span_ > 0)
  {
    timeout_signal_ = new nux::TimeOutSignal();
    timeout_signal_->tick.connect(sigc::mem_fun(this, &ProgramTemplate::ProgramExitCall));
    window_thread_->GetTimerHandler().AddOneShotTimer(program_life_span_, timeout_signal_, NULL, NULL);
  }

  window_thread_->Run(NULL);
}

bool ProgramTemplate::ReadyToGo()
{
  return window_thread_;
}

nux::WindowThread* ProgramTemplate::GetWindowThread()
{
  return window_thread_;
}

void ProgramTemplate::ProgramExitCall(void *data)
{
  if (window_thread_)
    window_thread_->ExitMainLoop();
}

void ProgramTemplate::WaitForConfigureEvent(int x, int y, int width, int height)
{
  ready_to_go_ = true;
}


// int main(int argc, char **argv)
// {
//     ProgramTemplate test("Text Entry", 300, 300, 3000);
//     test.Startup();
//     test.UserInterfaceSetup();
//     test.Run();

//     return 0;
// }
