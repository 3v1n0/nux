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

#include "Nux/Nux.h"
#include "TestTextEntry.h"

/*
  TestTextEntry:
    This class is a Nux view that processes all mouse events that it receives. It is meant for testing simulated
    mouse events. 
*/
namespace nux {
NUX_IMPLEMENT_OBJECT_TYPE(TestTextEntry);

TestTextEntry::TestTextEntry(NUX_FILE_LINE_DECL)
: nux::TextEntry("", NUX_FILE_LINE_PARAM)
{
  ResetEvents();
  ResetKeyFocusEvents();

  key_nav_direction_ = nux::KEY_NAV_NONE;

  has_focus_ = false;
  mouse_in_ = false;
  mouse_mouse_drag_ = false;
  mouse_mouse_down_ = false;

  mouse_down.connect(sigc::mem_fun(this, &TestTextEntry::OnMouseDown));
  mouse_up.connect(sigc::mem_fun(this, &TestTextEntry::OnMouseUp));
  mouse_drag.connect(sigc::mem_fun(this, &TestTextEntry::OnMouseDrag));
  mouse_click.connect(sigc::mem_fun(this, &TestTextEntry::OnMouseClick));
  mouse_double_click.connect(sigc::mem_fun(this, &TestTextEntry::OnMouseDoubleClick));
  mouse_move.connect(sigc::mem_fun(this, &TestTextEntry::OnMouseMove));
  mouse_enter.connect(sigc::mem_fun(this, &TestTextEntry::OnMouseEnter));
  mouse_leave.connect(sigc::mem_fun(this, &TestTextEntry::OnMouseLeave));
  key_nav_focus_change.connect(sigc::mem_fun(this, &TestTextEntry::OnKeyNavFocusChange));
  begin_key_focus.connect(sigc::mem_fun(this, &TestTextEntry::OnBeginKeyFocus));
  end_key_focus.connect(sigc::mem_fun(this, &TestTextEntry::OnEndKeyFocus));
  object_destroyed.connect(sigc::mem_fun(this, &TestTextEntry::OnObjectDestroyed));

  text_changed.connect(sigc::mem_fun(this, &TestTextEntry::OnTextChanged));


}

TestTextEntry::~TestTextEntry()
{

}

void TestTextEntry::ResetEvents()
{
  registered_mouse_down_          = false;
  registered_mouse_up_            = false;
  registered_mouse_drag_          = false;
  registered_mouse_enter_         = false;
  registered_mouse_leave_         = false;
  registered_mouse_click_         = false;
  registered_mouse_double_click_  = false;
  registered_mouse_move_          = false;
  registered_mouse_enter_         = false;
  registered_mouse_leave_         = false;
  registered_object_destroyed_    = false;
  registered_text_changed_        = false;
}

void TestTextEntry::ResetKeyFocusEvents()
{
  registered_begin_keynav_focus_ = false;
  registered_end_keynav_focus_ = false;
}

void TestTextEntry::OnMouseDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
{
  registered_mouse_down_ = true;
  mouse_mouse_down_ = true;
  QueueDraw();
}

void TestTextEntry::OnMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
{
  registered_mouse_up_ = true;

  mouse_mouse_down_ = false;
  mouse_mouse_drag_ = false;
  QueueDraw();
}

void TestTextEntry::OnMouseDrag(int /* x */, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
{
  registered_mouse_drag_ = true;
  mouse_mouse_drag_ = true;
  QueueDraw();
}

void TestTextEntry::OnMouseClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
{
  registered_mouse_click_ = true;
  QueueDraw();
}

void TestTextEntry::OnMouseDoubleClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
{
  registered_mouse_double_click_ = true;
  mouse_mouse_down_ = true;
  QueueDraw();
}

void TestTextEntry::OnMouseMove(int /* x */, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
{
  registered_mouse_move_ = true;
  QueueDraw();
}

void TestTextEntry::OnMouseEnter(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
{
  registered_mouse_enter_ = true;
  mouse_in_ = true;
  QueueDraw();
}

void TestTextEntry::OnMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
{
  registered_mouse_leave_ = true;
  mouse_in_ = false;
  QueueDraw();
}

void TestTextEntry::OnKeyNavFocusChange(nux::Area* /* area */, bool has_focus, nux::KeyNavDirection direction)
{
  has_focus_ = HasKeyFocus();
  
  has_focus_ = has_focus;
  key_nav_direction_ = direction;
  QueueDraw();
}

void TestTextEntry::OnBeginKeyFocus()
{
  registered_begin_keynav_focus_ = true;
}

void TestTextEntry::OnEndKeyFocus()
{
  registered_end_keynav_focus_ = true;
}

void TestTextEntry::OnObjectDestroyed(nux::Object* /* object */)
{
  registered_object_destroyed_ = true;
}

void TestTextEntry::OnTextChanged(TextEntry* /* text_entry */)
{
  registered_text_changed_ = true;
}

}
