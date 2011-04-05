/*
 * Copyright 2011 Canonical Ltd.
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
 * Authored by: Gordon Allott <gord.allott@canonical.com>
 *
 */
#include "Nux.h"
#include "NuxGraphics/Events.h"
#include "Focusable.h"

namespace nux
{
  bool Focusable::GetFocused ()
  {
    return DoGetFocused ();
  }
  void Focusable::SetFocused (bool focused)
  {
    DoSetFocused (focused);
  }
  bool Focusable::CanFocus ()
  {
    return DoCanFocus ();
  }
  void Focusable::ActivateFocus ()
  {
    DoActivateFocus ();
  }

  FocusEventType Focusable::GetFocusableEventType (unsigned long    eventType,
                                                  unsigned long    keysym,
                                                  const char* character,
                                                  FocusDirection *direction)
  {
    FocusEventType type = FOCUS_EVENT_NONE;
    *direction = FOCUS_DIRECTION_NONE;
    if (eventType == NUX_KEYDOWN)
    {
      switch (keysym)
      {
        case NUX_VK_ENTER:
        case NUX_KP_ENTER:
          type = FOCUS_EVENT_ACTIVATE;
          //g_debug ("focus key activated");
          break;
        case NUX_VK_UP:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_UP;
          //g_debug ("direction up");
          break;
        case NUX_VK_DOWN:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_DOWN;
          //g_debug ("direction down");
          break;
        case NUX_VK_LEFT:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_LEFT;
          //g_debug ("direction left");
          break;
        case NUX_VK_RIGHT:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_RIGHT;
          //g_debug ("direction right");
          break;
        case NUX_VK_TAB:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_NEXT;
          if (keysym & NUX_STATE_SHIFT)
          {
            *direction = FOCUS_DIRECTION_PREV;
            //g_debug ("direction_prev");
          }
          else
          {
            //g_debug ("direction next");
          }
          break;
        default:
          type = FOCUS_EVENT_NONE;
          *direction = FOCUS_DIRECTION_NONE;
          break;

      }
    }
    return type;
  }

  bool Focusable::DoGetFocused ()
  {
    return false;
  }

  void Focusable::DoSetFocused (bool focused)
  {
  }

  bool Focusable::DoCanFocus ()
  {
    g_debug ("Focusable DoCanFocus");
    return false;
  }

  void Focusable::DoActivateFocus ()
  {
  }

};
