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
 * Authored by: Brandon Schaefer <brandon.schaefer@canonical.com>
 *
 */

#include "Nux/Nux.h"
#include "Nux/WindowThread.h"
#include "Nux/VLayout.h"
#include "Nux/TextEntry.h"
#include "Nux/InputMethodIBus.h"
#include "Nux/ProgramFramework/ProgramTemplate.h"
#include "Nux/ProgramFramework/TestView.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include "nux_automated_test_framework.h"

#include <sys/types.h>
#include <signal.h>
#include <stdio.h>

#include <fstream>
#include <queue>

class TextTextEntry: public ProgramTemplate
{
public:
  TextTextEntry(const char* program_name, int window_width, int window_height, int program_life_span);
  ~TextTextEntry();

  virtual void UserInterfaceSetup();

  void TextEntryClick(nux::TextEntry* text_entry);
  void OnActivated();
  void OnCursorMoved(int);
  void ResetEvents();

  nux::TextEntry* text_entry_;

  bool clicked_;
  bool activated_;
  bool cursor_moved_;
};

TextTextEntry::TextTextEntry(const char* program_name,
  int window_width,
  int window_height,
  int program_life_span)
  : ProgramTemplate(program_name, window_width, window_height, program_life_span)
  , clicked_(false)
  , activated_(false)
  , cursor_moved_(false)
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
  activated_ = false;
  cursor_moved_ = false;
}

void TextTextEntry::TextEntryClick(nux::TextEntry* text_entry)
{
  if (text_entry_ == text_entry)
  {
    clicked_ = true;
  }
}

void TextTextEntry::OnActivated()
{
  activated_ = true;
}

void TextTextEntry::OnCursorMoved(int position)
{
  cursor_moved_ = true;
}

void TextTextEntry::UserInterfaceSetup()
{
  nux::VLayout* main_layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  text_entry_ = new nux::TextEntry("", NUX_TRACKER_LOCATION);
  text_entry_->activated.connect(sigc::mem_fun(this, &TextTextEntry::OnActivated));
  text_entry_->cursor_moved.connect(sigc::mem_fun(this, &TextTextEntry::OnCursorMoved));
  text_entry_->SetFontSize(76);

  main_layout->AddView(text_entry_, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  main_layout->SetSpaceBetweenChildren(10);
  main_layout->SetContentDistribution(nux::MAJOR_POSITION_CENTER);

  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(main_layout);

  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
}

TextTextEntry* test_textentry = NULL;

// Returns dynamic memory, delete me!
char* ReadInCommands(const char* path)
{
  char* raw_file = NULL;
  long size;
  std::ifstream file(path, std::ios::in|std::ios::ate);

  if (file.is_open())
  {
    size = file.tellg();
    raw_file = new char[size+1];
    file.seekg (0, std::ios::beg);
    file.read(raw_file, size);
    file.close();
  }
  return raw_file;
}

void TokenizeCommands(char* raw_cmds, std::queue<std::string>& token_queue)
{
  int i = 0;
  int size = strlen(raw_cmds);

  while (i < size)
  {
    if (raw_cmds[i] == '/' && raw_cmds[i+1] == '/')
    {
      while (raw_cmds[i] != '\n' && i < size)
      {
        i++;
      }
      i++;
    }
    else if (raw_cmds[i] >= '0' && raw_cmds[i] <= '9')
    {
      std::string num = "";
      while (raw_cmds[i] != ' ' && i < size)
      {
        num += raw_cmds[i];
        i++;
      }
      token_queue.push(num);

      while (raw_cmds[i] == ' ') {i++;}

      std::string cmd = "";
      while (raw_cmds[i] != '\n' && raw_cmds[i] != '\0' && i < size)
      {
        cmd += raw_cmds[i];
        i++;
      }
      token_queue.push(cmd);
    }
    else
    {
      i++;
    }
  }
}

void KillInputMethod(const char* im_name)
{
  FILE* f_ptr;
  char pgrep[128] = "pgrep ";
  char pid[128];
  char* last = NULL;
  int cur_pid = 0;

  strcat(pgrep, im_name);
  if ((f_ptr = popen(pgrep, "r")) == NULL)
    return;

  while(fgets(pid, 128, f_ptr) != NULL)
  {
    cur_pid = strtol(pid, &last, 10);
    if (*last == '\0' || *last == '\n')
    {
      // Make sure it's dead
      kill(cur_pid, SIGKILL);
      kill(cur_pid, SIGSTOP);
    }
  }
  pclose(f_ptr);
}

void KillCurrentInputMethod()
{
  const char* xmodifier;
  xmodifier = getenv("XMODIFIERS");
  if (!xmodifier)
    return;

  KillInputMethod(strchr(xmodifier, '=')+1);
}

/*
  0 - Name of Input method
  1 - what keys we should press
  2 - what to type
  3 - what to expect
  4 - we are done with this IM

  CFG (If someone complains that we need to check the input)
  **Note I cheated with Key, Letter and CJK...can be filled in
  S -> 0 L | 1 K | 2 L | 3 C | 4 <halt>
  K -> K+K | <Key>
  L -> LL | <Letter>
  C -> CC | <CJK>

  example:
  0 fcitx
  1 ctrl+space
  2 ninhao <CJK>

  Abstract Syntax
  IM        = 0 <name>
  KeyStrokes= 1 <key> | 1 <key+key> | 1 <key+key+key> | 1 <key+key+key+key>
  Input     = 2 <input>
  Expect    = 3 <cjk>
  Halt      = 4 halt

  0 -> popen(im_name)
  1 -> RunKeyStrokes(keys)
  2 -> TypeInput(input)
  3 -> CheckInput(cjk)
  4 -> pclose(im_name)
*/

std::vector<std::string>& GetSplitEnvPath()
{
  static std::vector<std::string> split_path;

  if (split_path.empty())
  {
    std::string tmp;
    size_t colon_ptr = 0, start_ptr = 0;

    const std::string env_path(getenv("PATH"));

    if (!env_path.empty())
    {
      // Splits the env path var up on (":")
      while (colon_ptr != std::string::npos)
      {
        colon_ptr = env_path.find(":", start_ptr);
        tmp = env_path.substr(start_ptr, (colon_ptr-start_ptr));
        start_ptr = colon_ptr+1;
        split_path.push_back(tmp);
      }
    }
  }
  return split_path;
}

bool CheckIMExist(const char* im_name)
{
  std::vector<std::string> split_path = GetSplitEnvPath();
  std::string command("ls ");
  char buf [128];
  FILE* path = NULL;

  for (unsigned int i = 0; i < split_path.size(); i++)
  {
    if ((path = popen((command + split_path[i]).c_str(),"r")) == NULL)
      return false;

    while (fgets(buf, 128, path) != NULL)
    {
      if (strstr(buf, im_name))
        return true;
    }
  }
  return false;
}

FILE* OpenIM(const char* im_name)
{
  FILE* open_file = NULL;

  if (CheckIMExist(im_name))
  {
    open_file = popen(im_name,"r");
    nux::SleepForMilliseconds(500);
  }
  return open_file;
}

bool CheckInput(const std::string cjk, NuxAutomatedTestFramework* test)
{
  std::string message("Test is: " + cjk);
  test->TestReportMsg(test_textentry->text_entry_->GetText() == cjk, message.c_str());

  test->ViewSendCtrlA();
  nux::SleepForMilliseconds(500);

  test->ViewSendDelete();
  nux::SleepForMilliseconds(500);
  return true;
}

std::string next_token (std::queue<std::string>& tokens)
{
  std::string next_token;
  if (!tokens.empty())
  {
    next_token = tokens.front();
    tokens.pop();
  }
  return next_token;
}

bool RunCommands(std::queue<std::string>& tokens, NuxAutomatedTestFramework* test)
{
  std::string next_cmd;
  std::string im_name;
  std::string cur_cmd = next_token(tokens);
  FILE* start_im = NULL;

  while (!tokens.empty())
  {
    next_cmd = next_token(tokens);
    if (cur_cmd == "0")
    {
      if ((start_im = OpenIM(next_cmd.c_str())) != NULL)
      {
        im_name = next_cmd.c_str();
        nux::SleepForMilliseconds(500);
      }
      else
      {
        fprintf(stderr, "Command not found: %s\n", next_cmd.c_str());

        // Skip the test
        while (cur_cmd != "4")
        {
          cur_cmd = next_token(tokens);
        }
        next_cmd = next_token(tokens);
      }
    }
    else if (cur_cmd == "1")
    {
      test->ViewSendKeys(next_cmd);
    }
    else if (cur_cmd == "2")
    {
      test->ViewSendString(next_cmd);
    }
    else if (cur_cmd == "3")
    {
      CheckInput(next_cmd, test);
    }
    else if (cur_cmd == "4")
    {
      KillInputMethod(im_name.c_str());
      pclose(start_im);
    }
    else
    {
      fprintf(stderr, "Error - Incorrect command: %s\n", cur_cmd.c_str());
      return false;
    }
    nux::SleepForMilliseconds(500);
    cur_cmd = next_token(tokens);
  }

  return true;
}

void TestingThread(nux::NThread* thread, void* user_data)
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

  test.ViewSendMouseClick(0,1);

  std::queue<std::string> tokens;
  char* raw_commands;

  // Just kills what should be the current IM (if there is one)
  KillCurrentInputMethod();

  raw_commands = ReadInCommands("xim-test-commands.txt");
  if (raw_commands)
  {
    TokenizeCommands(raw_commands, tokens);
    delete raw_commands;
    RunCommands(tokens, &test);
  }

  if (test.WhenDoneTerminateProgram())
  {
    wnd_thread->ExitMainLoop();
  }
  nuxDebugMsg("Exit testing thread");
}

int main(int argc, char** argv)
{
  int xstatus = XInitThreads();
  nuxAssertMsg(xstatus > 0, "XInitThreads has failed");

  test_textentry = new TextTextEntry("Text Entry", 600, 200, 40000);
  test_textentry->Startup();
  test_textentry->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(NULL, &TestingThread, test_textentry->GetWindowThread());

  test_thread->Start(test_textentry);

  test_textentry->Run();

  delete test_thread;
  delete test_textentry;

  return 0;
}

