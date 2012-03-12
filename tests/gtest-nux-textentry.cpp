#include <string>
#include <fstream>

#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>

#include "Nux/Nux.h"
#include "Nux/TextEntry.h"


using namespace testing;

namespace {

TEST(TestTextEntry, TestSetText)
{
  nux::NuxInitialize(0);
  nux::WindowThread* wnd_thread = nux::CreateNuxWindow("Nux Window", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::TextEntry* text_entry = new nux::TextEntry("");
  
  nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(text_entry);

  EXPECT_EQ(text_entry->IsInTextInputMode(), false);

  text_entry->SetText("Nux");
  EXPECT_EQ(text_entry->GetText() == std::string("Nux"), true);

  EXPECT_EQ(text_entry->IsInTextInputMode(), true);

  text_entry->UnReference();
  delete wnd_thread;
}

TEST(TestTextEntry, TestEnterText)
{
  nux::NuxInitialize(0);
  nux::WindowThread* wnd_thread = nux::CreateNuxWindow("Nux Window", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::TextEntry* text_entry = new nux::TextEntry("");
  
  nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(text_entry);

  EXPECT_EQ(text_entry->IsInTextInputMode(), false);

  text_entry->EnterText("Nux");

  EXPECT_EQ(text_entry->GetText() == std::string("Nux"), true);

  EXPECT_EQ(text_entry->IsInTextInputMode(), true);

  text_entry->UnReference();
  delete wnd_thread;
}

TEST(TestTextEntry, TestDeleteText)
{
  nux::NuxInitialize(0);
  nux::WindowThread* wnd_thread = nux::CreateNuxWindow("Nux Window", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::TextEntry* text_entry = new nux::TextEntry("");
  
  nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(text_entry);

  EXPECT_EQ(text_entry->IsInTextInputMode(), false);

  text_entry->EnterText("Nux");

  EXPECT_EQ(text_entry->GetText() == std::string("Nux"), true);

  EXPECT_EQ(text_entry->IsInTextInputMode(), true);

  nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(NULL);

  nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(text_entry);

  EXPECT_EQ(text_entry->IsInTextInputMode(), false);

  text_entry->DeleteText(0, 3);

  EXPECT_EQ(text_entry->GetText() == std::string(""), true);

  EXPECT_EQ(text_entry->IsInTextInputMode(), true);


  text_entry->UnReference();
  delete wnd_thread;
}

}
