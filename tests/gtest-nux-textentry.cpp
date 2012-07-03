#include <gmock/gmock.h>

#include "Nux/Nux.h"
#include "Nux/TextEntry.h"


using namespace testing;
using namespace nux;

namespace {

class MockTextEntry : public nux::TextEntry
{
public:
  MockTextEntry(const char* text) : nux::TextEntry(text)
  {}

  bool InspectKeyEvent(nux::Event const& event)
  {
    return nux::TextEntry::InspectKeyEvent(event);
  }
};

class TestTextEntry : public Test
{
public:
  virtual void SetUp()
  {
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("Nux Window", 300, 200,
                     nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL));

    text_entry = new MockTextEntry("");
    nux::GetWindowThread()->GetWindowCompositor().SetKeyFocusArea(text_entry.GetPointer());
  }

  std::unique_ptr<nux::WindowThread> wnd_thread;
  nux::ObjectPtr<MockTextEntry> text_entry;
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

#if defined(NUX_OS_LINUX)
class TestEvent : public nux::Event
{
public:
  TestEvent(nux::KeyModifier keymod, unsigned long keysym)
  {
    type = nux::NUX_KEYDOWN;
    key_modifiers = keymod;
    x11_keysym = keysym;
  }

  TestEvent(unsigned long keysym)
  {
    type = nux::NUX_KEYDOWN;
    x11_keysym = keysym;
  }
};

TEST_F(TestTextEntry, AltLinuxKeybindings)
{
  for (unsigned long keysym = 0; keysym < XK_VoidSymbol; ++keysym)
  {
    TestEvent event(KEY_MODIFIER_ALT, keysym);
    EXPECT_FALSE(text_entry->InspectKeyEvent(event));
  }
}

TEST_F(TestTextEntry, SuperLinuxKeybindings)
{
  for (unsigned long keysym = 0; keysym < XK_VoidSymbol; ++keysym)
  {
    TestEvent event(KEY_MODIFIER_SUPER, keysym);
    EXPECT_FALSE(text_entry->InspectKeyEvent(event));
  }
}
#endif

TEST_F(TestTextEntry, InvalidKeys)
{
  std::vector<std::string> invalid_chars = {"", "", "", "", "",
                                            "", "", "", "", "",
                                            "", "", ""};
  for (auto c : invalid_chars)
  {
    KeySym keysym = g_utf8_get_char(c.c_str());
    text_entry->DeleteText(0, std::numeric_limits<int>::max());
    text_entry->key_down.emit(NUX_KEYDOWN, keysym, 0, c.c_str(), 1);
    EXPECT_EQ(text_entry->GetText(), "");
  }
}

}
