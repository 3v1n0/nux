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
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 
#include "nux_automated_test_framework.h"

namespace
{
std::string RESET_KEY_FOCUS_AREA_MSG = "r";
std::string KEY_FOCUS_AREA_RESET_MSG = "d";
}

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
  void SendResetKeyFocusAreaMessageToProgram();


  nux::TextEntry* text_entry_;

  bool clicked_;
  bool activated_;
  bool cursor_moved_;

private:

  /* Handled inside the TextEntry WindowThread */
  void HandleProgramMessage(const std::string &);
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

void TextTextEntry::SendResetKeyFocusAreaMessageToProgram()
{
  SendMessageToProgram(RESET_KEY_FOCUS_AREA_MSG);
  WaitForMessageFromProgram(KEY_FOCUS_AREA_RESET_MSG);
}

void TextTextEntry::HandleProgramMessage(const std::string &m)
{
  if (m == RESET_KEY_FOCUS_AREA_MSG)
  {
    GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(NULL);
    GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(text_entry_);
    SendMessageToTest(KEY_FOCUS_AREA_RESET_MSG);
  }
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

void TextTextEntry::OnCursorMoved(int /* position */)
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

bool SetEngineActive (IBusBus* bus_, std::string engine)
{
    GList* engines = ibus_bus_list_active_engines(bus_);
    if (!engines)
      return false;

    GList* start = engines;
    bool found = false;
    gboolean global_flag = ibus_bus_get_use_global_engine(bus_);

    // Iterates through the active engines
    do
    {
      IBusEngineDesc *engine_desc = IBUS_ENGINE_DESC (engines->data);

      // Found Engine, make it active!
      if (engine == ibus_engine_desc_get_name(engine_desc))
      {
        found = true;

        // Set ibus to use global engines
        if (!global_flag)
          ibus_config_set_value(ibus_bus_get_config(bus_),
                                "general",
                                "use_global_engine",
                                g_variant_new_boolean(true));

        // Set and activate the engine
        ibus_bus_set_global_engine(bus_, engine.c_str());
      }
    } while ((engines = g_list_next(engines)) != NULL);

    // Restores the global setting back to what it was
    ibus_config_set_value(ibus_bus_get_config(bus_),
                          "general",
                          "use_global_engine",
                          g_variant_new_boolean(global_flag));

    g_list_free(start);
    return found;
}

TextTextEntry* test_textentry = NULL;

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

  test.ViewSendMouseClick(0, 1);

  // Type "Nux"
  // The cursor is at the end of the line
  // Unset/Set the focus on the text entry
  // Move the cursor
  {
    test.ViewSendString("Nux");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Nux", "Typed \"Nux\"");

    test_textentry->SendResetKeyFocusAreaMessageToProgram();

    test_textentry->ResetEvents();
    test.ViewSendLeft();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->cursor_moved_, "Cursor moved.");

    test.ViewSendCtrlA();   
    test.ViewSendDelete();
  }

  // Type "Nux"
  // The cursor is at the end of the line
  // Unset/Set the focus on the text entry
  // Press enter
  {
    test.ViewSendString("Nux");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Nux", "Typed \"Nux\"");

    test_textentry->SendResetKeyFocusAreaMessageToProgram();

    test_textentry->ResetEvents();
    test.ViewSendReturn();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->activated_, "Activated.");

    test.ViewSendCtrlA();   
    test.ViewSendDelete();
  }

  // Type "Nux"
  // The cursor is at the end of the line
  // Simulate CTRL+A to select the entire text
  // Simulate DELETE key to delete the text
  {
    test.ViewSendString("Nux");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Nux", "Typed \"Nux\"");

    test.ViewSendCtrlA();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetTextSelection() == "Nux", "Selection is \"Nux\"");

   
    test.ViewSendDelete();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "TextEntry is empty");
  }

  // Type "0123456789"
  // The cursor is at the end of the line
  // Simulate BACKSPACE key until the text is enpty
  {
    test.ViewSendString("Ubuntu");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Ubuntu", "Typed \"Ubuntu\"");

    test.ViewSendBackspace();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Ubunt", "Text is \"Ubunt\"");

    test.ViewSendBackspace();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Ubun", "Text is \"Ubun\"");

    test.ViewSendBackspace();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Ubu", "Text is \"Ubu\"");

    test.ViewSendBackspace();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Ub", "Text is \"Ub\"");

    test.ViewSendBackspace();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "U", "Text is \"U\"");

    test.ViewSendBackspace();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "TextEntry is empty");
  }

  // Type "0123456789"
  // Move cursor to start of line
  // Simulate DELETE key until the text is 6789
  // Simulate CTRL+A to select the entire text
  // Simulate DELETE key to delete the text
  {
    test.ViewSendString("0123456789");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "0123456789", "Typed \"0123456789\"");

    test_textentry->text_entry_->MoveCursorToLineStart();

    test.ViewSendDelete();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "123456789", "Text is \"123456789\"");

    test.ViewSendDelete();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "23456789", "Text is \"23456789\"");

    test.ViewSendDelete();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "3456789", "Text is \"3456789\"");

    test.ViewSendDelete();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "456789", "Text is \"456789\"");

    test.ViewSendDelete();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "56789", "Text is \"56789\"");

    test.ViewSendDelete();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "6789", "Text \"6789\"");


    test.ViewSendCtrlA();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetTextSelection() == "6789", "Selection is \"6789\"");

   
    test.ViewSendDelete();
    nux::SleepForMilliseconds(500);
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "TextEntry is empty");
  }

  // Send invalid keys
  {
    test.ViewSendKeyCombo(XK_Control_L, 0, 0, 'r');
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "Invalid key: TextEntry is empty");
    nux::SleepForMilliseconds(500);

    test.ViewSendKeyCombo(XK_Control_L, 0, 0, 'w');
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "Invalid key: TextEntry is empty");
    nux::SleepForMilliseconds(500);

    test.ViewSendKeyCombo(XK_Control_L, 0, 0, 'g');
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "Invalid key: TextEntry is empty");
    nux::SleepForMilliseconds(500);

    test.ViewSendKeyCombo(XK_Control_L, 0, 0, 'h');
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "Invalid key: TextEntry is empty");
    nux::SleepForMilliseconds(500);

    nux::SleepForMilliseconds(500);
    test.ViewSendCtrlA();
    test.ViewSendDelete();
  }

  // Toggle IBus
  // Type "qwerty"
  // Simulate key '1' to select the first IM option
  {
    // CTRL+Space to initiate iBus
    //test.ViewSendIBusToggle();
    
    IBusBus* bus_;
    ibus_init();
    bus_ = ibus_bus_new();
    bool active = false;
  
    // Test for ibus-pinyin 
    if (bus_ && SetEngineActive(bus_,"pinyin"))
    {
      // Type random stuff
      {
        test.ViewSendString("ninhao");
        nux::SleepForMilliseconds(500);
        test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "TextEntry is only Preedit");

        test.ViewSendChar('1');
        nux::SleepForMilliseconds(500);
        test.TestReportMsg(test_textentry->text_entry_->GetText() == "您好", "TextEntry is \"您好\"");

        test.ViewSendCtrlA();
        nux::SleepForMilliseconds(500);
        
        test.ViewSendDelete();
        nux::SleepForMilliseconds(500);
      }

      // Testing Cursor in the preedit window
      {
        test.ViewSendString("ming");
        nux::SleepForMilliseconds(500);
        test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "TextEntry is only Preedit");

        test.ViewSendLeft();
        nux::SleepForMilliseconds(500);

        test.ViewSendDelete();
        nux::SleepForMilliseconds(500);

        test.ViewSendChar('1');
        nux::SleepForMilliseconds(500);
        test.TestReportMsg(test_textentry->text_entry_->GetText() == "民", "TextEntry is \"民\"");

        test.ViewSendCtrlA();
        nux::SleepForMilliseconds(500);
        
        test.ViewSendDelete();
        nux::SleepForMilliseconds(500);
      }

      active = true;
    }

    // Test for ibus-hangul    
    if (bus_ && SetEngineActive(bus_,"hangul"))
    {   
      // Test for the the space in ibus-hangul working correctlly 
      {
        test.ViewSendString("asd abc ");
        nux::SleepForMilliseconds(500);
        test.TestReportMsg(test_textentry->text_entry_->GetText() == "ㅁㄴㅇ 뮻 ", "TextEntry is \"ㅁㄴㅇ 뮻 \"");

        test.ViewSendCtrlA();
        nux::SleepForMilliseconds(500);

        test.ViewSendDelete();
        nux::SleepForMilliseconds(500);
      }

      active = true;
    }

    // Checking for ibus-anthy - Japanese
    if (bus_ && SetEngineActive(bus_,"anthy"))
    {
      {
        test.ViewSendString("shisutemu ");
        nux::SleepForMilliseconds(500);
        test.TestReportMsg(test_textentry->text_entry_->GetText() == "", "TextEntry is only Preedit");

        // Ctrl + J commits for ibus-anthy
        test.ViewSendKeyCombo(XK_Control_L, 0, 0, 'j');
        nux::SleepForMilliseconds(500);
        test.TestReportMsg(test_textentry->text_entry_->GetText() == "システム", "TextEntry is \"システム\"");

        test.ViewSendCtrlA();
        nux::SleepForMilliseconds(500);

        test.ViewSendDelete();

        nux::SleepForMilliseconds(500);
      }

      active = true;
    }

    g_object_unref (bus_); 

    // CTRL+Space to deactivate iBus
    if (active)
      test.ViewSendIBusToggle();
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

