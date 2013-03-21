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

class TextTextEntry: public ProgramTemplate
{
public:
  TextTextEntry(const char* program_name, int window_width, int window_height, int program_life_span);
  ~TextTextEntry();

  virtual void UserInterfaceSetup();

  void TextEntryClick(nux::TextEntry* text_entry);
  void ResetEvents();
  nux::TextEntry* text_entry_;

  bool clicked_;
};

TextTextEntry::TextTextEntry(const char* program_name,
  int window_width,
  int window_height,
  int program_life_span)
  : ProgramTemplate(program_name, window_width, window_height, program_life_span)
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

void TextTextEntry::TextEntryClick(nux::TextEntry* text_entry)
{
  if (text_entry_ == text_entry)
  {
    clicked_ = true;
  }
}

void TextTextEntry::UserInterfaceSetup()
{
  nux::VLayout* main_layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  text_entry_ = new nux::TextEntry("", NUX_TRACKER_LOCATION);
  text_entry_->SetFontSize(76);

  main_layout->AddView(text_entry_, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  main_layout->SetSpaceBetweenChildren(10);
  main_layout->SetContentDistribution(nux::MAJOR_POSITION_CENTER);

  static_cast<nux::WindowThread*>(window_thread_)->SetLayout(main_layout);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(window_thread_)->SetWindowBackgroundPaintLayer(&background);
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

  if (test.CompositionKeysSupported())
  {
    test.ViewSendCompositionKeys("`o");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "ò", "ò");

    test.ViewSendCompositionKeys("~o");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "òõ", "òõ");

    test.ViewSendCompositionKeys("^o");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "òõô", "òõô");

    test.ViewSendCompositionKeys("c=");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "òõô€", "òõô€");

    test.ViewSendCompositionKeys("oc");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "òõô€©", "òõô€©");

    test.ViewSendCompositionKeys("\"w");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "òõô€©ẅ", "òõô€©ẅ");

    test.ViewSendCompositionKeys("'S");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "òõô€©ẅŚ", "òõô€©ẅŚ");
    test.ViewSendCtrlA();
    test.ViewSendDelete();

    nux::SleepForMilliseconds(100);
    //exclamdown # INVERTED EXCLAMATION MARK
    test.ViewSendCompositionKeys("!!");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "¡", "¡");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //CENT SIGN
    test.ViewSendCompositionKeys("|c");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "¢", "¢");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //POUND SIGN
    test.ViewSendCompositionKeys("-L");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "£", "£");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //currency # CURRENCY SIGN
    test.ViewSendCompositionKeys("xo");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "¤", "¤");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //yen # YEN SIGN
    test.ViewSendCompositionKeys("Y=");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "¥", "¥");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

     //FEMININE ORDINAL INDICATOR
    test.ViewSendCompositionKeys("^_a");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "ª", "ª");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //guillemotleft # LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    test.ViewSendCompositionKeys("<<");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "«", "«");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

   //SUPERSCRIPT THREE
    test.ViewSendCompositionKeys("^3");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "³", "³");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //MIDDLE DOT
    test.ViewSendCompositionKeys("..");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "·", "·");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //cedilla # CEDILLA
    test.ViewSendCompositionKeys(", ");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "¸", "¸");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //VULGAR FRACTION THREE QUARTERS
    test.ViewSendCompositionKeys("34");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "¾", "¾");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //questiondown # INVERTED QUESTION MARK
    test.ViewSendCompositionKeys("??");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "¿", "¿");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN CAPITAL LETTER A WITH GRAVE
    test.ViewSendCompositionKeys("`A");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "À", "À");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN CAPITAL LETTER A WITH ACUTE
    test.ViewSendCompositionKeys("'A");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Á", "Á");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN CAPITAL LETTER A WITH CIRCUMFLEX
    test.ViewSendCompositionKeys("^A");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Â", "Â");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN CAPITAL LETTER A WITH TILDE
    test.ViewSendCompositionKeys("~A");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Ã", "Ã");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN CAPITAL LETTER A WITH DIAERESIS
    test.ViewSendCompositionKeys("\"A");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Ä", "Ä");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN CAPITAL LETTER A WITH RING ABOVE
    test.ViewSendCompositionKeys("oA");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Å", "Å");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //AE # LATIN CAPITAL LETTER AE
    test.ViewSendCompositionKeys("AE");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Æ", "Æ");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //ssharp # LATIN SMALL LETTER SHARP S
    test.ViewSendCompositionKeys("ss");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "ß", "ß");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN SMALL LETTER A WITH GRAVE
    test.ViewSendCompositionKeys("`a");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "à", "à");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN SMALL LETTER A WITH ACUTE
    test.ViewSendCompositionKeys("'a");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "á", "á");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN SMALL LETTER A WITH CIRCUMFLEX
    test.ViewSendCompositionKeys("^a");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "â", "â");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN SMALL LETTER A WITH TILDE
    test.ViewSendCompositionKeys("~a");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "ã", "ã");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN SMALL LETTER A WITH DIAERESIS
    test.ViewSendCompositionKeys("\"a");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "ä", "ä");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN SMALL LETTER A WITH RING ABOVE
    test.ViewSendCompositionKeys("oa");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "å", "å");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //ae # LATIN SMALL LETTER AE
    test.ViewSendCompositionKeys("ae");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "æ", "æ");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //DIVISION SIGN
    test.ViewSendCompositionKeys("-:");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "÷", "÷");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN CAPITAL LETTER A WITH OGONEK
    test.ViewSendCompositionKeys(";A");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Ą", "Ą");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN SMALL LETTER A WITH OGONEK
    test.ViewSendCompositionKeys(";a");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "ą", "ą");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);;

    //LATIN CAPITAL LETTER A WITH CARON
    test.ViewSendCompositionKeys("cA");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "Ǎ", "Ǎ");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);

    //EN DASH
    test.ViewSendCompositionKeys("--.");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "–", "–");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);

    //RIGHT SINGLE QUOTATION MARK
    test.ViewSendCompositionKeys(">'");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "’", "’");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);

    //LEFT DOUBLE QUOTATION MARK
    test.ViewSendCompositionKeys("<\"");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "“", "“");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);

    //RIGHT DOUBLE QUOTATION MARK
    test.ViewSendCompositionKeys("\">");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "”", "”");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);

    //DOUBLE LOW-9 QUOTATION MARK
    test.ViewSendCompositionKeys("\",");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "„", "„");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);

    //PER MILLE SIGN
    test.ViewSendCompositionKeys("%o");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "‰", "‰");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);

    //SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
    test.ViewSendCompositionKeys(".>");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "›", "›");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);

    //EuroSign # EURO SIGN
    test.ViewSendCompositionKeys("=E");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "€", "€");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);

    //REVERSED DOUBLE PRIME QUOTATION MARK
    test.ViewSendCompositionKeys("\"\\");
    test.TestReportMsg(test_textentry->text_entry_->GetText() == "〝", "〝");
    test.ViewSendCtrlA();
    test.ViewSendDelete();
    nux::SleepForMilliseconds(100);
  }
  else
  {
    test.WarnCompositionKeysNotSupported();
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

  test_textentry = new TextTextEntry("Text Entry: Composition Characters", 600, 200, 40000);
  test_textentry->Startup();
  test_textentry->UserInterfaceSetup();

  nux::SystemThread* test_thread = nux::CreateSystemThread(NULL, &TestingThread, test_textentry->GetWindowThread());

  test_thread->Start(test_textentry);

  test_textentry->Run();

  delete test_thread;
  delete test_textentry;

  return 0;
}

