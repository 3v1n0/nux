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
    raw_file = new char[size];
    file.seekg (0, std::ios::beg);
    file.read(raw_file, size);
    file.close();
  }
  return raw_file;
}


/* Takes the raw file and replaces the space after the number
 * with a null ('\0') and also at the end of the line of a command
 * this makes it so we don't have to do any copying as we use that
 * raw input as the tokens.
*/
// It could be more benifitcal if we actually split the commands into
// tokens instead of just putting NULLs to split the word....
void TokenizeCommands(char* raw_commands, std::queue<std::string>& token_queue)
{
  char *cur_ptr, *lk_ptr = NULL;
  cur_ptr = raw_commands;

  // Simple Tokenizer
  // Split on the first space
  while (*cur_ptr != '\0')// && *cur_ptr != '\n')
  {
    // Skip whitespace or a random new line
    while (*cur_ptr == '\n' || *cur_ptr == ' ') {cur_ptr++;}

    // Comment, skip line, restart rules
    if (*cur_ptr == '/' && *(cur_ptr+1) == '/')
    {
      cur_ptr = (strchr(cur_ptr, '\n')+1);
      continue;
    }

    // Find the first space and replace with a null
    if((lk_ptr = strchr(cur_ptr, ' ')) == NULL)
    {
      fprintf(stderr, "ERROR: %s\n", cur_ptr);
    }
    *lk_ptr = '\0';
    std::string tmp1(cur_ptr);
    token_queue.push(tmp1);

    // Find the end of the line and replace it with a Null
    if ((cur_ptr = strchr(lk_ptr+1, '\n')) == NULL)
    {
      fprintf(stderr, "ERROR: %s\n", lk_ptr);
    }
    *cur_ptr = '\0';
    cur_ptr++;
    std::string tmp2(lk_ptr+1);
    token_queue.push(tmp2);
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
      // Make sure its dead
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
  KeyStroks = 1 <key> | 1 <key+key> | 1 <key+key+key> | 1 <key+key+key+key>
  Input     = 2 <input>
  Expect    = 3 <cjk>
  Halt      = 4 halt

  0 -> popen(im_name)
  1 -> RunKeyStrokes(keys)
  2 -> TypeInput(input)
  3 -> CheckInput(cjk)
  4 -> pclose(im_name)
*/

bool RunKeyStrokes(const char* keystrokes, NuxAutomatedTestFramework* test)
{
  return true;
}

bool TypeInput(const char* text, NuxAutomatedTestFramework* test)
{
  return true;
}

bool CheckInput(const std::string cjk, NuxAutomatedTestFramework* test)
{
  std::string message("Test is: " + cjk);
  printf("equals %s\n", cjk.c_str());
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
    printf("cur_cmd: %s\n", cur_cmd.c_str());
    printf("next: %s\n", next_cmd.c_str());
    if (cur_cmd == "0")
    {
      if ((start_im = popen(next_cmd.c_str(), "r")) == NULL)
      {
        // Move on to the next test
        while (cur_cmd != "4")
        {
          cur_cmd = next_token(tokens);
        }
        next_cmd = next_token(tokens);
      }
      else
      {
        im_name = next_cmd.c_str();
        nux::SleepForMilliseconds(500);
      }
    }
    else if (cur_cmd == "1")
    {
      //RunKeyStrokes(next_cmd, test);
      test->ViewSendKeys(next_cmd);
      nux::SleepForMilliseconds(500);
    }
    else if (cur_cmd == "2")
    {
      //TypeInput(next_cmd, test);
      test->ViewSendString(next_cmd);
      nux::SleepForMilliseconds(500);
    }
    else if (cur_cmd == "3")
    {
      CheckInput(next_cmd, test);
      nux::SleepForMilliseconds(500);
    }
    else if (cur_cmd == "4")
    {
      KillInputMethod(im_name.c_str());
      pclose(start_im);
      nux::SleepForMilliseconds(500);
    }
    else
    {
      fprintf(stderr, "Something bad happened....\n");
      return false;
    }
    cur_cmd = next_token(tokens);
    printf("%i\n", tokens.size());
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

