/*
 * Copyright 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Marco Trevisan <marco.trevisan@canonical.com>
 *
 */

#include "gtest-nux-utils.h"

#include <gmock/gmock.h>

#include "Nux/Nux.h"
#include "Nux/TextEntry.h"
#include "Nux/HLayout.h"
#if defined(NUX_OS_LINUX)
#include "Nux/InputMethodIBus.h"
#include "Nux/TextEntryComposeSeqs.h"
#endif

using namespace testing;
using namespace nux;

namespace {

class MockTextEntry : public TextEntry
{
public:
  MockTextEntry() : TextEntry("")
  {}

  bool InspectKeyEvent(Event const& event)
  {
    return TextEntry::InspectKeyEvent(event);
  }

  bool GetSelectionBounds(int* start, int* end) const
  {
    return TextEntry::GetSelectionBounds(start, end);
  }

  int GetCursor() const
  {
    return cursor_;
  }

  void SetCursor(int cursor)
  {
    TextEntry::SetCursor(cursor);
  }

  bool InCompositionMode() const
  {
    return TextEntry::IsInCompositionMode();
  }

  bool HandleComposition(unsigned long keysym)
  {
    return TextEntry::HandleComposition(keysym);
  }

  void ClearText()
  {
    TextEntry::DeleteText(0, std::numeric_limits<int>::max());
    ASSERT_TRUE(GetText().empty());
  }

  enum class CompositionResult
  {
    NO_MATCH,
    PARTIAL,
    MATCH
  };

  CompositionResult GetCompositionForList(std::vector<unsigned long> const& input, std::string& composition)
  {
    return static_cast<CompositionResult>(TextEntry::GetCompositionForList(input, composition));
  }

  CompositionResult GetCompositionForString(std::string const& input, std::string& composition)
  {
    std::vector<unsigned long> input_vec;
#if defined(NUX_OS_LINUX)
    input_vec.push_back(XK_Multi_key);

    for (auto c : input)
    {
      input_vec.push_back(c);
    }
#endif
    return GetCompositionForList(input_vec, composition);
  }

  nux::IBusIMEContext* ime() const
  {
#if defined(NUX_OS_LINUX)
    return ime_;
#else
    return nullptr;
#endif
  }

  MOCK_METHOD0(CutClipboard, void());
  MOCK_METHOD0(CopyClipboard, void());
  MOCK_METHOD0(PasteClipboard, void());
#if defined(NUX_OS_LINUX)
  MOCK_METHOD0(PastePrimaryClipboard, void());
#endif
};

class TestEvent : public Event
{
public:
  TestEvent(KeyModifier keymod, unsigned long keysym, EventType type = NUX_KEYDOWN)
    : TestEvent(keysym, type)
  {
    key_modifiers = keymod;
  }

  TestEvent(unsigned long keysym, EventType etype = NUX_KEYDOWN)
  {
    type = etype;
#if defined(NUX_OS_LINUX)
    x11_keysym = keysym;
    dtext = new char[NUX_EVENT_TEXT_BUFFER_SIZE];
    auto len = g_unichar_to_utf8(x11_keysym, dtext);
    dtext[len] = '\0';
#elif defined(NUX_OS_WINDOWS)
    win32_keysym = keysym;
#endif
  }
};

class TestTextEntry : public Test
{
public:
  virtual void SetUp()
  {
    wnd_thread.reset(CreateNuxWindow("Nux Window", 300, 200, WINDOWSTYLE_NORMAL,
                     nullptr, false, NULL, NULL));

    text_entry = new MockTextEntry();
    HLayout* layout = new HLayout();
    layout->AddView(text_entry);
    wnd_thread->SetLayout(layout);

    GetWindowCompositor().SetKeyFocusArea(text_entry);
  }

  void WaitEvent()
  {
    if (text_entry->im_running())
      Utils::WaitForTimeoutMSec(20);
  }

  void SendEvent(Event& event)
  {
    GetWindowCompositor().ProcessEvent(event);
    WaitEvent();
  }

  std::unique_ptr<WindowThread> wnd_thread;
  MockTextEntry* text_entry;
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

  GetWindowCompositor().SetKeyFocusArea(nullptr);

  GetWindowCompositor().SetKeyFocusArea(text_entry);

  EXPECT_EQ(text_entry->IsInTextInputMode(), false);

  text_entry->DeleteText(0, 3);

  EXPECT_EQ(text_entry->GetText() == std::string(""), true);

  EXPECT_EQ(text_entry->IsInTextInputMode(), true);
}

#if defined(NUX_OS_LINUX)
TEST_F(TestTextEntry, AltLinuxKeybindings)
{
  for (unsigned long keysym = 0; keysym < XK_VoidSymbol; ++keysym)
  {
    TestEvent event(KEY_MODIFIER_ALT, keysym);

    if (text_entry->ime()->IsHotkeyEvent(event.type, event.GetKeySym(), event.key_modifiers))
      EXPECT_TRUE(text_entry->InspectKeyEvent(event));
    else
      EXPECT_FALSE(text_entry->InspectKeyEvent(event));
  }
}

TEST_F(TestTextEntry, SuperLinuxKeybindings)
{
  for (unsigned long keysym = 0; keysym < XK_VoidSymbol; ++keysym)
  {
    TestEvent event(KEY_MODIFIER_SUPER, keysym);

    if (text_entry->ime()->IsHotkeyEvent(event.type, event.GetKeySym(), event.key_modifiers))
      EXPECT_TRUE(text_entry->InspectKeyEvent(event));
    else
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
    unsigned int keysym = g_utf8_get_char(c.c_str());
    text_entry->ClearText();
    text_entry->key_down.emit(NUX_KEYDOWN, keysym, 0, c.c_str(), 1);
    WaitEvent();
    EXPECT_EQ(text_entry->GetText(), "");
  }
}

TEST_F(TestTextEntry, CopyCtrlC)
{
  EXPECT_CALL(*text_entry, CopyClipboard());
  TestEvent event(KEY_MODIFIER_CTRL, NUX_VK_c);
  SendEvent(event);
}

TEST_F(TestTextEntry, CopyCtrlIns)
{
  EXPECT_CALL(*text_entry, CopyClipboard());
  TestEvent event(KEY_MODIFIER_CTRL, NUX_VK_INSERT);
  SendEvent(event);
}

TEST_F(TestTextEntry, PasteCtrlV)
{
  EXPECT_CALL(*text_entry, PasteClipboard());
  TestEvent event(KEY_MODIFIER_CTRL, NUX_VK_v);
  SendEvent(event);
}

TEST_F(TestTextEntry, PasteShiftIns)
{
  EXPECT_CALL(*text_entry, PasteClipboard());
  TestEvent event(KEY_MODIFIER_SHIFT, NUX_VK_INSERT);
  SendEvent(event);
}

#if defined(NUX_OS_LINUX)
TEST_F(TestTextEntry, PastePrimaryClipboard)
{
  EXPECT_CALL(*text_entry, PastePrimaryClipboard());
  text_entry->mouse_down.emit(0, 0, NUX_EVENT_BUTTON2_DOWN, 0);

  EXPECT_CALL(*text_entry, PastePrimaryClipboard()).Times(0);
  text_entry->mouse_down.emit(0, 0, NUX_EVENT_BUTTON1_DOWN, 0);

  EXPECT_CALL(*text_entry, PastePrimaryClipboard()).Times(0);
  text_entry->mouse_down.emit(0, 0, NUX_EVENT_BUTTON3_DOWN, 0);
}
#endif

TEST_F(TestTextEntry, CutCtrlX)
{
  EXPECT_CALL(*text_entry, CutClipboard());
  TestEvent event(KEY_MODIFIER_CTRL, NUX_VK_x);
  SendEvent(event);
}

TEST_F(TestTextEntry, CutShiftDel)
{
  EXPECT_CALL(*text_entry, CutClipboard());
  TestEvent event(KEY_MODIFIER_SHIFT, NUX_VK_DELETE);
  SendEvent(event);
}

TEST_F(TestTextEntry, CtrlA)
{
  TestEvent selectall(KEY_MODIFIER_CTRL, NUX_VK_a);
  int start, end;
  const std::string test_str("Nux");
  text_entry->EnterText(test_str.c_str());
  EXPECT_FALSE(text_entry->GetSelectionBounds(&start, &end));
  ASSERT_EQ(start, end);
  ASSERT_EQ(start, test_str.length());

  SendEvent(selectall);
  EXPECT_TRUE(text_entry->GetSelectionBounds(&start, &end));
  EXPECT_EQ(start, 0);
  EXPECT_EQ(end, test_str.length());
}

TEST_F(TestTextEntry, MoveKeys)
{
  const std::string test_str("Nux");
  text_entry->EnterText(test_str.c_str());
  ASSERT_EQ(text_entry->GetCursor(), test_str.length());
  text_entry->SetCursor(0);
  ASSERT_EQ(text_entry->GetCursor(), 0);

  TestEvent right(NUX_VK_RIGHT);
  SendEvent(right);
  EXPECT_EQ(text_entry->GetCursor(), 1);

  TestEvent end(NUX_VK_END);
  SendEvent(end);
  EXPECT_EQ(text_entry->GetCursor(), test_str.length());

  TestEvent left(NUX_VK_LEFT);
  SendEvent(left);
  EXPECT_EQ(text_entry->GetCursor(), 2);

  TestEvent home(NUX_VK_HOME);
  SendEvent(home);
  EXPECT_EQ(text_entry->GetCursor(), 0);
}

TEST_F(TestTextEntry, CtrlMoveKeys)
{
  const std::string test_str("Nux Text Entry");
  text_entry->EnterText(test_str.c_str());
  ASSERT_EQ(text_entry->GetCursor(), test_str.length());
  text_entry->SetCursor(0);
  ASSERT_EQ(text_entry->GetCursor(), 0);

  TestEvent right(KEY_MODIFIER_CTRL, NUX_VK_RIGHT);
  SendEvent(right);
  EXPECT_EQ(text_entry->GetCursor(), 3);

  TestEvent left(KEY_MODIFIER_CTRL, NUX_VK_LEFT);
  SendEvent(left);
  EXPECT_EQ(text_entry->GetCursor(), 0);

  TestEvent end(KEY_MODIFIER_CTRL, NUX_VK_END);
  SendEvent(end);
  EXPECT_EQ(text_entry->GetCursor(), test_str.length());

  TestEvent home(KEY_MODIFIER_CTRL, NUX_VK_HOME);
  SendEvent(home);
  EXPECT_EQ(text_entry->GetCursor(), 0);
}

TEST_F(TestTextEntry, DeleteKeys)
{
  const std::string test_str("Nux");
  text_entry->EnterText(test_str.c_str());
  text_entry->SetCursor(0);

  TestEvent del(NUX_VK_DELETE);
  SendEvent(del);
  EXPECT_EQ(text_entry->GetText(), "ux");

  text_entry->SetCursor(std::string(text_entry->GetText()).length());
  TestEvent backspace(NUX_VK_BACKSPACE);
  SendEvent(backspace);
  EXPECT_EQ(text_entry->GetText(), "u");
}

TEST_F(TestTextEntry, CtrlDeleteKeys)
{
  const std::string test_str("Nux Text Entry");
  text_entry->EnterText(test_str.c_str());
  text_entry->SetCursor(0);

  TestEvent del(KEY_MODIFIER_CTRL, NUX_VK_DELETE);
  SendEvent(del);
  EXPECT_EQ(text_entry->GetText(), " Text Entry");

  text_entry->SetCursor(std::string(text_entry->GetText()).length());
  TestEvent backspace(KEY_MODIFIER_CTRL, NUX_VK_BACKSPACE);
  SendEvent(backspace);
  EXPECT_EQ(text_entry->GetText(), " Text ");
}

#if defined(NUX_OS_LINUX)
TEST_F(TestTextEntry, CompositionStart)
{
  ASSERT_FALSE(text_entry->InCompositionMode());
  TestEvent compose(XK_Multi_key);
  SendEvent(compose);
  EXPECT_TRUE(text_entry->InCompositionMode());
}

TEST_F(TestTextEntry, CompositionWrite)
{
  ASSERT_FALSE(text_entry->InCompositionMode());
  TestEvent compose(XK_Multi_key);
  SendEvent(compose);

  TestEvent tilde(XK_asciitilde);
  SendEvent(tilde);
  EXPECT_TRUE(text_entry->InCompositionMode());

  TestEvent n(TestEvent(XK_n));
  SendEvent(n);
  EXPECT_FALSE(text_entry->InCompositionMode());

  EXPECT_EQ(text_entry->GetText(), "ñ");
  text_entry->ClearText();

  SendEvent(compose);
  TestEvent less(XK_less);
  SendEvent(less);
  EXPECT_TRUE(text_entry->InCompositionMode());

  TestEvent three(TestEvent(XK_3));
  SendEvent(three);
  EXPECT_FALSE(text_entry->InCompositionMode());
  EXPECT_EQ(text_entry->GetText(), "♥");
}

TEST_F(TestTextEntry, CompositionIgnoreModifiers)
{
  ASSERT_FALSE(text_entry->InCompositionMode());
  TestEvent compose(XK_Multi_key);
  SendEvent(compose);

  TestEvent tilde(XK_asciitilde);
  SendEvent(tilde);
  EXPECT_TRUE(text_entry->InCompositionMode());

  for (auto keysym = XK_Shift_L; keysym <= XK_Hyper_R; ++keysym)
  {
    TestEvent modifier(keysym);
    SendEvent(modifier);
    EXPECT_TRUE(text_entry->InCompositionMode());
  }

  TestEvent AltGr(XK_ISO_Level3_Shift);
  SendEvent(AltGr);
  EXPECT_TRUE(text_entry->InCompositionMode());

  TestEvent n(TestEvent(XK_n));
  SendEvent(n);
  EXPECT_FALSE(text_entry->InCompositionMode());

  EXPECT_EQ(text_entry->GetText(), "ñ");
}

TEST_F(TestTextEntry, CompositionDeadKey)
{
  ASSERT_FALSE(text_entry->InCompositionMode());
  TestEvent dead_key_cirucmflex(XK_dead_circumflex);
  SendEvent(dead_key_cirucmflex);
  EXPECT_TRUE(text_entry->InCompositionMode());

  TestEvent a(XK_a);
  SendEvent(a);
  EXPECT_FALSE(text_entry->InCompositionMode());

  EXPECT_EQ(text_entry->GetText(), "â");
  text_entry->ClearText();

  TestEvent dead_key_currency(XK_dead_currency);
  SendEvent(dead_key_currency);
  EXPECT_TRUE(text_entry->InCompositionMode());

  TestEvent e(XK_e);
  SendEvent(e);
  EXPECT_FALSE(text_entry->InCompositionMode());

  EXPECT_EQ(text_entry->GetText(), "€");
}

TEST_F(TestTextEntry, CompositionDeadKeyRepeat)
{
  ASSERT_FALSE(text_entry->InCompositionMode());
  TestEvent dead_key(XK_dead_grave);
  SendEvent(dead_key);
  EXPECT_TRUE(text_entry->InCompositionMode());

  SendEvent(dead_key);
  EXPECT_FALSE(text_entry->InCompositionMode());

  EXPECT_EQ(text_entry->GetText(), "`");
}

TEST_F(TestTextEntry, CompositionDeadKeyComplex)
{
  ASSERT_FALSE(text_entry->InCompositionMode());
  TestEvent dead_key(XK_dead_circumflex);
  SendEvent(dead_key);
  EXPECT_TRUE(text_entry->InCompositionMode());

  SendEvent(dead_key);
  EXPECT_FALSE(text_entry->InCompositionMode());
  EXPECT_EQ(text_entry->GetText(), "^");

  SendEvent(dead_key);
  TestEvent o(XK_o);
  SendEvent(o);
  EXPECT_FALSE(text_entry->InCompositionMode());

  EXPECT_EQ(text_entry->GetText(), "^ô");
}

TEST_F(TestTextEntry, CompositionDeadKeysMix)
{
  // Make sure that the two dead keys we use here aren't used any sequence
  ASSERT_FALSE(text_entry->InCompositionMode());
  TestEvent dead_key1(XK_dead_macron);
  SendEvent(dead_key1);
  EXPECT_TRUE(text_entry->InCompositionMode());

  TestEvent dead_key2(XK_dead_circumflex);
  SendEvent(dead_key2);
  EXPECT_FALSE(text_entry->InCompositionMode());

  EXPECT_EQ(text_entry->GetText(), "");
}

TEST_F(TestTextEntry, CompositionMultipleKeyCancel)
{
  std::string composed;
  auto result = text_entry->GetCompositionForString("iii", composed);

  EXPECT_EQ(result, MockTextEntry::CompositionResult::NO_MATCH);
  EXPECT_EQ(composed, "");

  TestEvent compose(XK_Multi_key);
  TestEvent i(XK_i);

  SendEvent(compose);
  SendEvent(i);
  SendEvent(i);
  SendEvent(i);

  EXPECT_FALSE(text_entry->InCompositionMode());
  EXPECT_EQ(text_entry->GetText(), "i");
}

TEST_F(TestTextEntry, CompositionResultValid)
{
  std::string composed;
  auto result = text_entry->GetCompositionForString("o", composed);

  EXPECT_EQ(result, MockTextEntry::CompositionResult::PARTIAL);
  EXPECT_TRUE(composed.empty());

  result = text_entry->GetCompositionForString("ox", composed);
  EXPECT_EQ(result, MockTextEntry::CompositionResult::MATCH);
  EXPECT_EQ(composed, "¤");

  result = text_entry->GetCompositionForString("ubuntu", composed);
  EXPECT_EQ(result, MockTextEntry::CompositionResult::MATCH);
  EXPECT_EQ(composed, "");
}

TEST_F(TestTextEntry, CompositionResultInvalid)
{
  std::string composed;
  auto result = text_entry->GetCompositionForString("nux", composed);

  EXPECT_EQ(result, MockTextEntry::CompositionResult::NO_MATCH);
  EXPECT_TRUE(composed.empty());
}

TEST_F(TestTextEntry, CompositionSequences)
{
  for (unsigned i = 0; i < COMPOSE_SEQUENCES_SIZE; ++i)
  {
    ComposeSequence const& seq = COMPOSE_SEQUENCES[i];
    std::vector<unsigned long> input_vec;
    std::string composed;

    for (unsigned j = 0; j < ComposeSequence::MAX_SYMBOLS && seq.symbols[j] != XK_VoidSymbol; ++j)
    {
      input_vec.push_back(seq.symbols[j]);
    }

    std::string expected_result = seq.result;
    auto result = text_entry->GetCompositionForList(input_vec, composed);

    EXPECT_EQ(result, MockTextEntry::CompositionResult::MATCH);
    EXPECT_EQ(composed, expected_result);
  }
}

TEST_F(TestTextEntry, CompositionSequencesInput)
{
  for (unsigned i = 0; i < COMPOSE_SEQUENCES_SIZE; ++i)
  {
    ComposeSequence const& seq = COMPOSE_SEQUENCES[i];

    EXPECT_FALSE(text_entry->InCompositionMode());

    for (unsigned j = 0; j < ComposeSequence::MAX_SYMBOLS && seq.symbols[j] != XK_VoidSymbol; ++j)
    {
      // We use a different strategy if ibus is active, to speedup the test
      if (text_entry->im_running())
      {
        text_entry->HandleComposition(seq.symbols[j]);
      }
      else
      {
        TestEvent event(seq.symbols[j]);
        SendEvent(event);
      }

      if (seq.symbols[j+1] != XK_VoidSymbol)
        EXPECT_TRUE(text_entry->InCompositionMode());
    }

    std::string expected_result = seq.result;

    EXPECT_FALSE(text_entry->InCompositionMode());
    EXPECT_EQ(text_entry->GetText(), expected_result);

    text_entry->ClearText();
  }
}

TEST_F(TestTextEntry, PasswordMode)
{
  text_entry->SetPasswordMode(true);
  EXPECT_TRUE(text_entry->PasswordMode());
  
  text_entry->SetPasswordChar("*");
  EXPECT_EQ(text_entry->GetPasswordChar(), std::string("*"));

  text_entry->SetText("Nux");
  EXPECT_EQ(text_entry->GetText(), std::string("Nux"));

  text_entry->SetPasswordMode(false);
  EXPECT_FALSE(text_entry->PasswordMode());
}

#endif
}
