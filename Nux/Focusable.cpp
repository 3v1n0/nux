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
    return false;
  }
  void Focusable::SetFocused (bool focused)
  {
  }
  bool Focusable::CanFocus ()
  {
    return false;
  }
  void Focusable::ActivateFocus ()
  {
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
          type = FOCUS_EVENT_ACTIVATE;
          break;
        case NUX_KP_UP:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_UP;
          break;
        case NUX_KP_DOWN:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_DOWN;
          break;
        case NUX_KP_LEFT:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_LEFT;
          break;
        case NUX_KP_RIGHT:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_RIGHT;
          break;
        case NUX_VK_TAB:
          type = FOCUS_EVENT_DIRECTION;
          *direction = FOCUS_DIRECTION_NEXT;
          if (keysym & NUX_STATE_SHIFT)
          {
            *direction = FOCUS_DIRECTION_PREV;
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
};
