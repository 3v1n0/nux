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
 * Authored by: Marco Trevisan <marco.trevisan@canonical.com>
 *
 */

#include <gmock/gmock.h>

#include "Nux/Nux.h"
#include "Nux/InputMethodIBus.h"


using namespace nux;

class MockIBusIMEContext : public IBusIMEContext
{
public:
  static std::vector<Event> ParseIBusHotkeys(const gchar** keybindings)
  {
    return IBusIMEContext::ParseIBusHotkeys(keybindings);
  }
};

namespace {

TEST(TestIBusIMEContext, ParseNullHotkeys)
{
  auto hotkeys = MockIBusIMEContext::ParseIBusHotkeys(nullptr);
  EXPECT_TRUE(hotkeys.empty());
}

TEST(TestIBusIMEContext, ParseInvalidHotkeys)
{
  const gchar* invalid_code[] = {"Alt+InvalidCode", nullptr };

  auto hotkeys = MockIBusIMEContext::ParseIBusHotkeys(invalid_code);
  EXPECT_TRUE(hotkeys.empty());


  const gchar* missing_key[] = {"Alt+Control", nullptr };

  hotkeys = MockIBusIMEContext::ParseIBusHotkeys(missing_key);
  EXPECT_TRUE(hotkeys.empty());
}

TEST(TestIBusIMEContext, ParseHotkeys)
{
  const gchar* keybindings[] = {"Control+space", "Zenkaku_Hankaku", "Alt+Kanji",
                                "Alt+grave", "Hangul", "Alt+Release+Alt_R",
                                "Alt+Control+Release+Shift_L", nullptr};

  auto hotkeys = MockIBusIMEContext::ParseIBusHotkeys(keybindings);
  ASSERT_EQ(hotkeys.size(), 7);

  auto event = hotkeys[0];
  EXPECT_EQ(event.type, NUX_KEYDOWN);
  EXPECT_EQ(event.key_modifiers, KEY_MODIFIER_CTRL);
  EXPECT_EQ(event.x11_keysym, XK_space);

  event = hotkeys[1];
  EXPECT_EQ(event.type, NUX_KEYDOWN);
  EXPECT_EQ(event.key_modifiers, 0);
  EXPECT_EQ(event.x11_keysym, XK_Zenkaku_Hankaku);

  event = hotkeys[2];
  EXPECT_EQ(event.type, NUX_KEYDOWN);
  EXPECT_EQ(event.key_modifiers, KEY_MODIFIER_ALT);
  EXPECT_EQ(event.x11_keysym, XK_Kanji);

  event = hotkeys[3];
  EXPECT_EQ(event.type, NUX_KEYDOWN);
  EXPECT_EQ(event.key_modifiers, KEY_MODIFIER_ALT);
  EXPECT_EQ(event.x11_keysym, XK_grave);

  event = hotkeys[4];
  EXPECT_EQ(event.type, NUX_KEYDOWN);
  EXPECT_EQ(event.key_modifiers, 0);
  EXPECT_EQ(event.x11_keysym, XK_Hangul);

  event = hotkeys[5];
  EXPECT_EQ(event.type, NUX_KEYUP);
  EXPECT_EQ(event.key_modifiers, KEY_MODIFIER_ALT);
  EXPECT_EQ(event.x11_keysym, XK_Alt_R);

  event = hotkeys[6];
  EXPECT_EQ(event.type, NUX_KEYUP);
  EXPECT_EQ(event.key_modifiers, (KEY_MODIFIER_ALT | KEY_MODIFIER_CTRL));
  EXPECT_EQ(event.x11_keysym, XK_Shift_L);
}

}
