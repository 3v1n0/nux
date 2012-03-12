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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#ifndef TEST_TEXT_ENTRY_H
#define TEST_TEXT_ENTRY_H

#include "Nux/TextEntry.h"

namespace nux {
class TestTextEntry: public nux::TextEntry
{
  NUX_DECLARE_OBJECT_TYPE(TestTextEntry, TextEntry);
public:
  TestTextEntry(NUX_FILE_LINE_PROTO);
  ~TestTextEntry();

  void ResetEvents();
  void ResetKeyFocusEvents();

  bool has_focus_;
  bool registered_mouse_down_;
  bool registered_mouse_up_;
  bool registered_mouse_drag_;
  bool registered_mouse_click_;
  bool registered_mouse_double_click_;
  bool registered_mouse_move_;
  bool registered_mouse_enter_;
  bool registered_mouse_leave_;
  bool registered_object_destroyed_;

  bool registered_begin_keynav_focus_;
  bool registered_end_keynav_focus_;

  bool registered_text_changed_;

  nux::KeyNavDirection key_nav_direction_; //!< The key nav direction received when the view obtained the focus.

protected:
  bool mouse_in_;
  bool mouse_mouse_drag_;
  bool mouse_mouse_down_;


  void OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
  void OnMouseClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
  void OnMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnKeyNavFocusChange(nux::Area* area, bool has_focus, nux::KeyNavDirection direction);
  void OnBeginKeyFocus();
  void OnEndKeyFocus();
  void OnObjectDestroyed(Object* object);

  void OnTextChanged(TextEntry* text_entry);
};

}
#endif // TEST_TEXT_ENTRY_H

