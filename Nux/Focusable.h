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



#ifndef FOCUSABLE_H
#define FOCUSABLE_H

namespace nux
{
  typedef enum
  {
    FOCUS_DIRECTION_NONE,
    FOCUS_DIRECTION_NEXT,
    FOCUS_DIRECTION_PREV,
    FOCUS_DIRECTION_UP,
    FOCUS_DIRECTION_DOWN,
    FOCUS_DIRECTION_LEFT,
    FOCUS_DIRECTION_RIGHT,
  } FocusDirection;

  typedef enum
  {
    FOCUS_EVENT_NONE,
    FOCUS_EVENT_ACTIVATE,
    FOCUS_EVENT_DIRECTION
  } FocusEventType;

  class Focusable
  {
  public:
    virtual bool GetFocused ();
    virtual void SetFocused (bool focused);
    virtual bool CanFocus ();
    virtual void ActivateFocus ();

    static FocusEventType GetFocusableEventType (unsigned long    eventType,
                                                 unsigned long    keysym,
                                                 const char* character,
                                                 FocusDirection *direction
                                                );
  };


};
#endif // FOCUSABLE_H
