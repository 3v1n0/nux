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

#include <gmock/gmock.h>

#include "Nux/Nux.h"
#include "Nux/TextEntry.h"
#include "Nux/HLayout.h"


using namespace testing;
using namespace nux;

namespace {

class MockTextEntry : public nux::TextEntry
{
public:
  MockTextEntry() : nux::TextEntry("")
  {}

  bool InspectKeyEvent(nux::Event const& event)
  {
    return nux::TextEntry::InspectKeyEvent(event);
  }

  bool GetSelectionBounds(int* start, int* end) const
  {
    return nux::TextEntry::GetSelectionBounds(start, end);
  }

  int GetCursor() const
  {
    return cursor_;
  }

  void SetCursor(int cursor)
  {
    nux::TextEntry::SetCursor(cursor);
  }

  MOCK_METHOD0(CutClipboard, void());
  MOCK_METHOD0(CopyClipboard, void());
  MOCK_METHOD0(PasteClipboard, void());
#if defined(NUX_OS_LINUX)
  MOCK_METHOD0(PastePrimaryClipboard, void());
#endif
};

class TestEvent : public nux::Event
{
public:
  TestEvent(nux::KeyModifier keymod, unsigned long keysym)
  {
    type = nux::NUX_KEYDOWN;
    key_modifiers = keymod;
#if defined(NUX_OS_LINUX)
    x11_keysym = keysym;
#elif NUX_OS_WINDOWS
    win32_keysym = keysym;
#endif
  }

  TestEvent(unsigned long keysym)
  {
    type = nux::NUX_KEYDOWN;
#if defined(NUX_OS_LINUX)
    x11_keysym = keysym;
#elif NUX_OS_WINDOWS
    win32_keysym = keysym;
#endif
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

    text_entry = new MockTextEntry();
    HLayout* layout = new HLayout();
    layout->AddView(text_entry);
    wnd_thread->SetLayout(layout);

    nux::GetWindowCompositor().SetKeyFocusArea(text_entry);
  }

  std::unique_ptr<nux::WindowThread> wnd_thread;
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

  nux::GetWindowCompositor().SetKeyFocusArea(nullptr);

  nux::GetWindowCompositor().SetKeyFocusArea(text_entry);

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
    unsigned int keysym = g_utf8_get_char(c.c_str());
    text_entry->DeleteText(0, std::numeric_limits<int>::max());
    text_entry->key_down.emit(NUX_KEYDOWN, keysym, 0, c.c_str(), 1);
    EXPECT_EQ(text_entry->GetText(), "");
  }
}

TEST_F(TestTextEntry, CopyCtrlC)
{
  EXPECT_CALL(*text_entry, CopyClipboard());
  TestEvent event(KEY_MODIFIER_CTRL, NUX_VK_c);
  nux::GetWindowCompositor().ProcessEvent(event);
}

TEST_F(TestTextEntry, CopyCtrlIns)
{
  EXPECT_CALL(*text_entry, CopyClipboard());
  TestEvent event(KEY_MODIFIER_CTRL, NUX_VK_INSERT);
  nux::GetWindowCompositor().ProcessEvent(event);
}

TEST_F(TestTextEntry, PasteCtrlV)
{
  EXPECT_CALL(*text_entry, PasteClipboard());
  TestEvent event(KEY_MODIFIER_CTRL, NUX_VK_v);
  nux::GetWindowCompositor().ProcessEvent(event);
}

TEST_F(TestTextEntry, PasteShiftIns)
{
  EXPECT_CALL(*text_entry, PasteClipboard());
  TestEvent event(KEY_MODIFIER_SHIFT, NUX_VK_INSERT);
  nux::GetWindowCompositor().ProcessEvent(event);
}

TEST_F(TestTextEntry, CutCtrlX)
{
  EXPECT_CALL(*text_entry, CutClipboard());
  TestEvent event(KEY_MODIFIER_CTRL, NUX_VK_x);
  nux::GetWindowCompositor().ProcessEvent(event);
}

TEST_F(TestTextEntry, CutShiftDel)
{
  EXPECT_CALL(*text_entry, CutClipboard());
  TestEvent event(KEY_MODIFIER_SHIFT, NUX_VK_DELETE);
  nux::GetWindowCompositor().ProcessEvent(event);
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

  nux::GetWindowCompositor().ProcessEvent(selectall);
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
  nux::GetWindowCompositor().ProcessEvent(right);
  EXPECT_EQ(text_entry->GetCursor(), 1);

  TestEvent end(NUX_VK_END);
  nux::GetWindowCompositor().ProcessEvent(end);
  EXPECT_EQ(text_entry->GetCursor(), test_str.length());

  TestEvent left(NUX_VK_LEFT);
  nux::GetWindowCompositor().ProcessEvent(left);
  EXPECT_EQ(text_entry->GetCursor(), 2);

  TestEvent home(NUX_VK_HOME);
  nux::GetWindowCompositor().ProcessEvent(home);
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
  nux::GetWindowCompositor().ProcessEvent(right);
  EXPECT_EQ(text_entry->GetCursor(), 3);

  TestEvent left(KEY_MODIFIER_CTRL, NUX_VK_LEFT);
  nux::GetWindowCompositor().ProcessEvent(left);
  EXPECT_EQ(text_entry->GetCursor(), 0);

  TestEvent end(KEY_MODIFIER_CTRL, NUX_VK_END);
  nux::GetWindowCompositor().ProcessEvent(end);
  EXPECT_EQ(text_entry->GetCursor(), test_str.length());

  TestEvent home(KEY_MODIFIER_CTRL, NUX_VK_HOME);
  nux::GetWindowCompositor().ProcessEvent(home);
  EXPECT_EQ(text_entry->GetCursor(), 0);
}

TEST_F(TestTextEntry, DeleteKeys)
{
  const std::string test_str("Nux");
  text_entry->EnterText(test_str.c_str());
  text_entry->SetCursor(0);

  TestEvent del(NUX_VK_DELETE);
  nux::GetWindowCompositor().ProcessEvent(del);
  EXPECT_EQ(text_entry->GetText(), "ux");

  text_entry->SetCursor(std::string(text_entry->GetText()).length());
  TestEvent backspace(NUX_VK_BACKSPACE);
  nux::GetWindowCompositor().ProcessEvent(backspace);
  EXPECT_EQ(text_entry->GetText(), "u");
}

TEST_F(TestTextEntry, CtrlDeleteKeys)
{
  const std::string test_str("Nux Text Entry");
  text_entry->EnterText(test_str.c_str());
  text_entry->SetCursor(0);

  TestEvent del(KEY_MODIFIER_CTRL, NUX_VK_DELETE);
  nux::GetWindowCompositor().ProcessEvent(del);
  EXPECT_EQ(text_entry->GetText(), " Text Entry");

  text_entry->SetCursor(std::string(text_entry->GetText()).length());
  TestEvent backspace(KEY_MODIFIER_CTRL, NUX_VK_BACKSPACE);
  nux::GetWindowCompositor().ProcessEvent(backspace);
  EXPECT_EQ(text_entry->GetText(), " Text ");
}
}
