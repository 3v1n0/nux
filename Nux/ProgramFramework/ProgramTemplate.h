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
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/TextEntry.h"

#ifndef PROGRAMTEMPLATE_H
#define PROGRAMTEMPLATE_H

class ProgramTemplate
{
public:
  ProgramTemplate(const char* program_name, int window_width, int window_height, int program_life_span);
  virtual ~ProgramTemplate();

  virtual void Startup();
  virtual void UserInterfaceSetup();
  virtual void Run();

  bool ReadyToGo();

  //!< Use with caution
  //
  // This WindowThread here will be valid, but if you call it from
  // another thread, and the call has a side-effect of calling
  // nux::GetWindowThread, it will likely result in undefined behaviour
  // since nux::GetWindowThread depends on the same thread that
  // GetWindowThread () here provides being the currently running
  // thread, as nux::GetWindowThread operates in terms of thread
  // local storage.
  nux::WindowThread* GetWindowThread();

  //!< Tells the program to do something, depending on the defined protocol
  // implemented in HandleProgramMessage
  void SendMessageToProgram (const std::string &msg);
  void WaitForMessageFromProgram (const std::string &msg);

public:
  std::string program_name_;
  int program_life_span_;                  //!< The program will auto-terminate after a delay in milliseconds.
  nux::TimeOutSignal* timeout_signal_;
  nux::WindowThread* window_thread_;

  int window_width_;
  int window_height_;

protected:

  void SendMessageToTest(const std::string &msg);

private:

  void MessageReceivedFromTest();
  static void ThreadInitializer(nux::NThread *, void *);

  virtual void HandleProgramMessage(const std::string &);

  void ProgramExitCall(void* data);
  void WaitForConfigureEvent(int x, int y, int width, int height);
  bool ready_to_go_;
  int  program_pipe_[2];
  int  test_pipe_[2];
};

#endif // PROGRAMTEMPLATE_H

