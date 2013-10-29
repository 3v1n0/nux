/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include "GLResource.h"
#include "Events.h"


namespace nux
{
  MouseButton GetEventButton(unsigned long button_state)
  {
    if ((button_state & NUX_EVENT_BUTTON1_DOWN) || (button_state & NUX_EVENT_BUTTON1_UP))
      return NUX_MOUSE_BUTTON1;
    else if ((button_state & NUX_EVENT_BUTTON2_DOWN) || (button_state & NUX_EVENT_BUTTON2_UP))
      return NUX_MOUSE_BUTTON2;
    else if ((button_state & NUX_EVENT_BUTTON3_DOWN) || (button_state & NUX_EVENT_BUTTON3_UP))
      return NUX_MOUSE_BUTTON3;
    else if ((button_state & NUX_EVENT_BUTTON4_DOWN) || (button_state & NUX_EVENT_BUTTON4_UP))
      return NUX_MOUSE_BUTTON4;

    return NUX_INVALID_MOUSE_BUTTON;
  }
  
  bool GetButtonState(unsigned long button_state, MouseButton button)
  {
    if (button == NUX_MOUSE_BUTTON1)
      return (button_state & NUX_STATE_BUTTON1_DOWN);
    else if (button == NUX_MOUSE_BUTTON2)
      return (button_state & NUX_STATE_BUTTON2_DOWN);
    else if (button == NUX_MOUSE_BUTTON3)
      return (button_state & NUX_STATE_BUTTON3_DOWN);
    else if (button == NUX_MOUSE_BUTTON4)
      return (button_state & NUX_STATE_BUTTON4_DOWN);

    return false;
  }
  
  bool GetKeyModifierState(unsigned long key_modifiers_states, KeyModifier key_modifier)
  {
    return((key_modifiers_states & key_modifier) != 0);
  }

  Event::Event()
  {
    Memset(text, 0, sizeof(text));
    dtext = nullptr;

    for (int i = 0; i < NUX_MAX_VK; i++)
    {
      VirtualKeycodeState[i] = 0;
    }

    ascii_code = 0;
    virtual_code = 0;
    key_modifiers = 0;
    key_repeat_count = 0;
    mouse_state = 0;
    x = -1;
    y = -1;
    x_root = 0;
    y_root = 0;
    dx = 0;
    dy = 0;
    clicks = 0;
    is_click = 0;
    wheel_delta = 0;

#if defined(NUX_OS_WINDOWS)
    win32_keycode = 0;
    win32_keysym = 0;
#endif

#if defined(USE_X11)
    x11_keycode = 0;
    x11_keysym = 0;
    x11_timestamp = 0;
    x11_window = 0;
    x11_key_state = 0;
#endif

    //Application = 0;
  }

  void Event::Reset()
  {
    type = NUX_NO_EVENT;
    Memset(text, 0, sizeof(text));
#if defined(NUX_OS_WINDOWS)
    win32_keycode = 0;
    win32_keysym = 0;
#endif

#if defined(USE_X11)
    x11_keycode = 0;
    x11_keysym = 0;
    x11_key_state = 0;
#endif

    delete[] dtext;
    dtext = nullptr;

    key_repeat_count = 0;
    key_modifiers = 0;
    wheel_delta = 0;
  }

  int Event::GetX() const
  {
    return x;
  }
  int Event::GetY() const
  {
    return y;
  }
  int Event::GetRootX() const
  {
    return x_root;
  }
  int Event::GetRootY() const
  {
    return y_root;
  }
  int Event::GetDeltaX() const
  {
    return dx;
  }
  int Event::GetDeltaY() const
  {
    return dy;
  }

  unsigned long Event::GetKeyState()	const
  {
    return key_modifiers;
  }

  unsigned long Event::GetMouseState() const
  {
    return mouse_state;
  }

  MouseButton Event::GetEventButton() const
  {
    if ((mouse_state & NUX_EVENT_BUTTON1_DOWN) || (mouse_state & NUX_EVENT_BUTTON1_UP))
      return NUX_MOUSE_BUTTON1;
    else if ((mouse_state & NUX_EVENT_BUTTON2_DOWN) || (mouse_state & NUX_EVENT_BUTTON2_UP))
      return NUX_MOUSE_BUTTON2;
    else if ((mouse_state & NUX_EVENT_BUTTON3_DOWN) || (mouse_state & NUX_EVENT_BUTTON3_UP))
      return NUX_MOUSE_BUTTON3;
    else if ((mouse_state & NUX_EVENT_BUTTON4_DOWN) || (mouse_state & NUX_EVENT_BUTTON4_UP))
      return NUX_MOUSE_BUTTON4;

    return NUX_INVALID_MOUSE_BUTTON;
  }

  bool Event::GetButtonState(MouseButton button) const
  {
    if (button == 1)
      return (mouse_state & NUX_STATE_BUTTON1_DOWN);
    else if (button == 2)
      return (mouse_state & NUX_STATE_BUTTON2_DOWN);
    else if (button == 3)
      return (mouse_state & NUX_STATE_BUTTON3_DOWN);
    else if (button == 4)
      return (mouse_state & NUX_STATE_BUTTON4_DOWN);

    return false;
  }

  bool Event::GetKeyModifierState(KeyModifier key_modifier) const
  {
    return((key_modifiers & key_modifier) != 0);
  }

  //! Return virtual key code of the key that has triggered the last event.
  /*!
      Return virtual key code of the key that has triggered the last event.
      @return the virtual key code.
  */
  unsigned long Event::GetKeySym() const
  {
#if defined(NUX_OS_WINDOWS)
    return win32_keysym;
#elif defined(USE_X11)
    return x11_keysym;
#else
    return 0;
#endif
  }

  unsigned short Event::GetKeyRepeatCount() const
  {
    return key_repeat_count;
  }

  const char* Event::GetText() const
  {
    return dtext;
  }


  //! Return the state of the Virtual key
  /*!
      Return the state of the Virtual key.
      @param VirtualKey virtual key code.
      @return 1 if the key is pressed, 0 if the key is released.
  */
  unsigned long Event::GetVirtualKeyState(unsigned long VirtualKey) const
  {
    if (VirtualKey >= NUX_MAX_VK)
      return 0;

    if (VirtualKey <= 0)
      return 0;

    return VirtualKeycodeState[VirtualKey];
  }

  bool Event::IsShiftDown() const
  {
    return (key_modifiers & NUX_STATE_SHIFT);
  }

  bool Event::IsControlDown() const
  {
    return (key_modifiers & NUX_STATE_CTRL);
  }

  bool Event::IsCapsLockDown() const
  {
    return (key_modifiers & NUX_STATE_CAPS_LOCK);
  }

  bool Event::IsAltDown() const
  {
    return (key_modifiers & NUX_STATE_ALT);
  }

  bool Event::IsNumLockDown() const
  {
    return (key_modifiers & NUX_STATE_NUMLOCK);
  }

  bool Event::IsScrollLockDown() const
  {
    return (key_modifiers & NUX_STATE_SCROLLLOCK);
  }

  bool Event::IsSuperKeyDown() const
  {
    return (key_modifiers & NUX_STATE_SCROLLLOCK);
  }

}

