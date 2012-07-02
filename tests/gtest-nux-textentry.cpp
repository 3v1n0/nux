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

class TestTextEntry : public Test
{
public:
  virtual void SetUp()
  {
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("Nux Window", 300, 200,
                     nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL));

    text_entry = new nux::TextEntry("");
    nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(text_entry.GetPointer());
  }

  std::unique_ptr<nux::WindowThread> wnd_thread;
  nux::ObjectPtr<nux::TextEntry> text_entry;
};


TEST_F(TestTextEntry, TestSetText)
{
  EXPECT_EQ(text_entry->IsInTextInputMode(), false);

  text_entry->SetText("Nux");
  EXPECT_EQ(text_entry->GetText() == std::string("Nux"), true);

  EXPECT_EQ(text_entry->IsInTextInputMode(), true);
}

TEST_F(TestTextEntry, TestEnterText)
{
  EXPECT_EQ(text_entry->IsInTextInputMode(), false);

  text_entry->EnterText("Nux");

  EXPECT_EQ(text_entry->GetText() == std::string("Nux"), true);

  EXPECT_EQ(text_entry->IsInTextInputMode(), true);
}

TEST_F(TestTextEntry, TestDeleteText)
{
  EXPECT_EQ(text_entry->IsInTextInputMode(), false);

  text_entry->EnterText("Nux");

  EXPECT_EQ(text_entry->GetText() == std::string("Nux"), true);

  EXPECT_EQ(text_entry->IsInTextInputMode(), true);

  nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(NULL);

  nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(text_entry.GetPointer());

  EXPECT_EQ(text_entry->IsInTextInputMode(), false);

  text_entry->DeleteText(0, 3);

  EXPECT_EQ(text_entry->GetText() == std::string(""), true);

  EXPECT_EQ(text_entry->IsInTextInputMode(), true);
}

}
