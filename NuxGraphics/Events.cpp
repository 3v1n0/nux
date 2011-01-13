/*
 * Copyright 2010 Inalogic Inc.
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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include "GLResource.h"
#include "Events.h"


namespace nux
{
  
  unsigned long GetEventButton(unsigned long button_state)
  {
    if ((button_state & NUX_EVENT_BUTTON1_DOWN) || (button_state & NUX_EVENT_BUTTON1_UP))
      return 1;
    else if ((button_state & NUX_EVENT_BUTTON2_DOWN) || (button_state & NUX_EVENT_BUTTON2_UP))
      return 2;
    else if ((button_state & NUX_EVENT_BUTTON3_DOWN) || (button_state & NUX_EVENT_BUTTON3_UP))
      return 3;
    else if ((button_state & NUX_EVENT_BUTTON4_DOWN) || (button_state & NUX_EVENT_BUTTON4_UP))
      return 4;

    return 0;
  }
  
  bool GetButtonState(unsigned long button_state, int button)
  {
    if (button == 1)
      return (button_state & NUX_STATE_BUTTON1_DOWN) ? true : false;
    else if (button == 2)
      return (button_state & NUX_STATE_BUTTON2_DOWN) ? true : false;
    else if (button == 3)
      return (button_state & NUX_STATE_BUTTON3_DOWN) ? true : false;
    else if (button == 4)
      return (button_state & NUX_STATE_BUTTON4_DOWN) ? true : false;

    return false;    
  }
  
  IEvent::IEvent()
  {
    Memset (e_text, 0, sizeof (e_text));

    for (int i = 0; i < NUX_MAX_VK; i++)
    {
      VirtualKeycodeState[i] = 0;
    }

    ascii_code = 0;
    virtual_code = 0;
    e_key_modifiers = 0;
    e_key_repeat_count = 0;
    e_mouse_state = 0;
    e_x = 0;
    e_y = 0;
    e_x_root = 0;
    e_y_root = 0;
    e_dx = 0;
    e_dy = 0;
    e_clicks = 0;
    e_is_click = 0;
    e_keysym = 0;
    e_wheeldelta = 0;
    e_x11_keycode = 0;

    //Application = 0;
  }

  void IEvent::Reset()
  {
    e_event = NUX_NO_EVENT;
    Memset (e_text, 0, sizeof (e_text));
    e_keysym = 0;
    e_key_repeat_count = 0;
    e_wheeldelta = 0;
  }

  int IEvent::GetX() const
  {
    return e_x;
  }
  int IEvent::GetY() const
  {
    return e_y;
  }
  int IEvent::GetRootX() const
  {
    return e_x_root;
  }
  int IEvent::GetRootY() const
  {
    return e_y_root;
  }
  int IEvent::GetDeltaX() const
  {
    return e_dx;
  }
  int IEvent::GetDeltaY() const
  {
    return e_dy;
  }

  unsigned long IEvent::GetKeyState()	const
  {
    return e_key_modifiers;
  }

  unsigned long IEvent::GetMouseState() const
  {
    return e_mouse_state;
  }

  unsigned long IEvent::GetEventButton() const
  {
    if ((e_mouse_state & NUX_EVENT_BUTTON1_DOWN) || (e_mouse_state & NUX_EVENT_BUTTON1_UP))
      return 1;
    else if ((e_mouse_state & NUX_EVENT_BUTTON2_DOWN) || (e_mouse_state & NUX_EVENT_BUTTON2_UP))
      return 2;
    else if ((e_mouse_state & NUX_EVENT_BUTTON3_DOWN) || (e_mouse_state & NUX_EVENT_BUTTON3_UP))
      return 3;
    else if ((e_mouse_state & NUX_EVENT_BUTTON4_DOWN) || (e_mouse_state & NUX_EVENT_BUTTON4_UP))
      return 4;

    return 0;
  }

  bool IEvent::GetButtonState(int button) const
  {
    if (button == 1)
      return (e_mouse_state & NUX_STATE_BUTTON1_DOWN) ? true : false;
    else if (button == 2)
      return (e_mouse_state & NUX_STATE_BUTTON2_DOWN) ? true : false;
    else if (button == 3)
      return (e_mouse_state & NUX_STATE_BUTTON3_DOWN) ? true : false;
    else if (button == 4)
      return (e_mouse_state & NUX_STATE_BUTTON4_DOWN) ? true : false;

    return false;
  }

  //! Return virtual key code of the key that has triggered the last event.
  /*!
      Return virtual key code of the key that has triggered the last event.
      @return the virtual key code.
  */
  unsigned long IEvent::GetKeySym() const
  {
    return e_keysym;
  }
  unsigned short IEvent::GetKeyRepeatCount() const
  {
    return e_key_repeat_count;
  }

  TCHAR IEvent::GetChar() const
  {
    return e_text[0];
  }


  //! Return the state of the Virtual key
  /*!
      Return the state of the Virtual key.
      @param VirtualKey virtual key code.
      @return 1 if the key is pressed, 0 if the key is released.
  */
  unsigned long IEvent::GetVirtualKeyState (unsigned long VirtualKey) const
  {
    if (VirtualKey >= NUX_MAX_VK)
      return 0;

    if (VirtualKey <= 0)
      return 0;

    return VirtualKeycodeState[VirtualKey];
  }


}

