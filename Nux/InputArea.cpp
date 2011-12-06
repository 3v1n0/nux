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


#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>

#include "NuxCore/Logger.h"

#include "Nux.h"
#include "InputArea.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "WindowCompositor.h"

namespace nux
{
namespace {
logging::Logger logger("nux.inputarea");
}

  NUX_IMPLEMENT_OBJECT_TYPE(InputArea);

  InputArea::InputArea(NUX_FILE_LINE_DECL)
    :   Area(NUX_FILE_LINE_PARAM)
    ,   m_AreaColor(color::Green)
  {
    SetGeometry(0, 0, 1, 1);
    _has_keyboard_focus = false;

    _capture_mouse_down_any_where_else = false;
    _double_click = false;

    _dnd_enabled_as_source = false;
    _dnd_enabled_as_target = false;
    _dnd_safety_x = 0;
    _dnd_safety_y = 0;

    _keyboard_receiver_ignore_mouse_down_outside = false;
  }

  InputArea::~InputArea()
  {
  }

  
  void InputArea::OnDraw(GraphicsEngine &graphics_engine, bool force_draw)
  {
    graphics_engine.QRP_Color(GetBaseX(), GetBaseY(), GetBaseWidth(), GetBaseHeight(), m_AreaColor);
  }

  void InputArea::SetBaseString(const char *Caption)
  {
    Area::SetBaseString(Caption);
  }

  bool InputArea::HasKeyboardFocus()
  {
    return GetWindowThread()->GetWindowCompositor().GetKeyFocusArea() == this;
  }

  void InputArea::SetKeyboardFocus(bool b)
  {
    _has_keyboard_focus = b;
  }

  int InputArea::GetMouseX()
  {
    return _event_processor._mouse_positionx - GetRootX();
  }

  int InputArea::GetMouseY()
  {
    return _event_processor._mouse_positiony - GetRootY();
  }

  bool InputArea::IsMouseInside()
  {
    return _event_processor.MouseIn();
  }

  bool InputArea::HasMouseFocus()
  {
    return (_event_processor._state & AREA_MOUSE_STATUS_FOCUS ? true : false);
  }

  // TODO: DEPRECATED
  bool InputArea::MouseFocusOnOtherArea()
  {
    return false;
  }

  void InputArea::CaptureMouseDownAnyWhereElse(bool b)
  {
    _capture_mouse_down_any_where_else = b;
  }

  bool InputArea::IsCaptureMouseDownAnyWhereElse() const
  {
    return _capture_mouse_down_any_where_else;
  }

  void InputArea::EnableDoubleClick(bool double_click)
  {
    _double_click = double_click;
  }

  bool InputArea::DoubleClickEnabled() const
  {
    return _double_click;
  }

  void InputArea::SetKeyboardReceiverIgnoreMouseDownOutside(bool ignore_mouse_down_outside)
  {
    _keyboard_receiver_ignore_mouse_down_outside = ignore_mouse_down_outside;
  }

  bool InputArea::KeyboardReceiverIgnoreMouseDownOutside()
  {
    return _keyboard_receiver_ignore_mouse_down_outside;
  }

  void InputArea::SetAreaMousePosition(int x, int y)
  {
    _event_processor._mouse_positionx = x;
    _event_processor._mouse_positiony = y;
  }

  void InputArea::HandleDndMove(Event &event)
  {
#if defined(NUX_OS_LINUX)
    std::list<char *> mimes;

    mimes = GetWindow().GetDndMimeTypes();
    std::list<char *>::iterator it;
    ProcessDndMove(event.e_x, event.e_y, mimes);

    for (it = mimes.begin(); it != mimes.end(); it++)
      g_free(*it);
#endif
  }

  void InputArea::HandleDndDrop(Event &event)
  {
#if defined(NUX_OS_LINUX)
    ProcessDndDrop(event.e_x, event.e_y);
#endif
  }

#if defined(NUX_OS_LINUX)
  void InputArea::SendDndStatus(bool accept, DndAction action, Geometry region)
  {
    GetWindow().SendDndStatus(accept, action, Rect(region.x, region.y, region.width, region.height));
  }

  void InputArea::SendDndFinished(bool accepted, DndAction action)
  {
    GetWindow().SendDndFinished(accepted, action);
  }

  void InputArea::ProcessDndMove(int x, int y, std::list<char *>mimes)
  {
    // must learn to deal with x/y offsets
    Area *parent = GetToplevel();

    if (parent)
    {
      x += parent->GetGeometry().x;
      y += parent->GetGeometry().y;
    }

    SendDndStatus(false, DNDACTION_NONE, Geometry(x, y, GetGeometry().width, GetGeometry().height));
  }

  void InputArea::ProcessDndDrop(int x, int y)
  {
    SendDndFinished(false, DNDACTION_NONE);
  }

  void InputArea::ProcessDndEnter()
  {
  }

  void InputArea::ProcessDndLeave()
  {
  }
  
  void InputArea::SetDndEnabled(bool as_source, bool as_target)
  {
    _dnd_enabled_as_source = as_source;
    _dnd_enabled_as_target = as_target;
  }
  
  bool InputArea::DndSourceDragBegin()
  {
    return false;
  }
  
  NBitmapData * InputArea::DndSourceGetDragImage()
  {
    return 0;
  }
  
  std::list<const char *> InputArea::DndSourceGetDragTypes()
  {
    std::list<const char *> types;
    types.push_back("text/plain;charset=utf-8");
    types.push_back("UTF8_STRING");
    return types;
  }
    
  const char * InputArea::DndSourceGetDataForType(const char *type, int *size, int *format)
  {
    *format = 8;

    if (g_str_equal(type, "text/plain;charset=utf-8") || g_str_equal(type, "UTF8_STRING"))
    {
      *size = (int) strlen("this is just a test");
      return "this is just a test";
    }
    
    *size = 0;
    return 0;
  }
  
  void InputArea::InnerDndSourceDragFinished(DndAction result, void *data) 
  { 
    InputArea *self = static_cast<InputArea *> (data);
    self->DndSourceDragFinished(result);
  }
  
  void InputArea::DndSourceDragFinished(DndAction result)
  {
  
  }
  
  void InputArea::StartDragAsSource()
  {
    GraphicsDisplay::DndSourceFuncs funcs;
    
    funcs.get_drag_image = &InputArea::InnerDndSourceGetDragImage;
    funcs.get_drag_types = &InputArea::InnerDndSourceGetDragTypes;
    funcs.get_data_for_type = &InputArea::InnerDndSourceGetDataForType;
    funcs.drag_finished = &InputArea::InnerDndSourceDragFinished;
    
    if (DndSourceDragBegin())
      GetWindow().StartDndDrag(funcs, this);
  }
#endif

  void InputArea::GrabPointer()
  {
    GetWindowThread()->GetWindowCompositor().GrabPointerAdd(this);
  }
  
  void InputArea::UnGrabPointer()
  {
    GetWindowThread()->GetWindowCompositor().GrabPointerRemove(this);
  }

  void InputArea::GrabKeyboard()
  {
    GetWindowThread()->GetWindowCompositor().GrabKeyboardAdd(this);
  }
  
  void InputArea::UnGrabKeyboard()
  {
    GetWindowThread()->GetWindowCompositor().GrabKeyboardRemove(this);
  }
  
  bool InputArea::OwnsPointerGrab()
  {
    return GetWindowThread()->GetWindowCompositor().GetPointerGrabArea() == this;
  }
  
  bool InputArea::OwnsKeyboardGrab()
  {
    return GetWindowThread()->GetWindowCompositor().GetKeyboardGrabArea() == this;
  }

  bool InputArea::IsMouseOwner()
  {
    return (GetWindowThread()->GetWindowCompositor().GetMouseOwnerArea() == this);
  }

  // == Signals with 1 to 1 mapping to input device ==
  void InputArea::EmitMouseDownSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    mouse_down.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseUpSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    mouse_up.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseMoveSignal(int x, int y, int dx, int dy, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    mouse_move.emit(x, y, dx, dy, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseWheelSignal(int x, int y, int wheel_delta, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    mouse_wheel.emit(x, y, wheel_delta, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitKeyDownSignal(unsigned int key_symbol, unsigned long x11_key_code, unsigned long special_keys_state)
  {
    //OnKeyPressed.emit(key_symbol, x11_key_code, special_keys_state);
  }

  void InputArea::EmitKeyUpSignal(unsigned int key_symbol, unsigned long x11_key_code, unsigned long special_keys_state)
  {
    key_up.emit(key_symbol, x11_key_code, special_keys_state);
  }

  void InputArea::EmitKeyEventSignal(unsigned long event_type,
    unsigned int key_sym,
    unsigned long special_keys_state,
    const char* text,
    int key_repeat_count)
  {
    key_down.emit(
      event_type,
      key_sym,
      special_keys_state,
      text,
      key_repeat_count);
  }

  void InputArea::EmitMouseDragSignal(int x, int y, int dx, int dy, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    mouse_drag.emit(x, y, dx, dy, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseEnterSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    _event_processor._current_mouse_in = true;
    mouse_enter.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseLeaveSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    _event_processor._current_mouse_in = false;
    mouse_leave.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseClickSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    mouse_click.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseDoubleClickSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    mouse_double_click.emit(x, y, mouse_button_state, special_keys_state);
  }


  void InputArea::EmitStartKeyboardFocus()
  {
    begin_key_focus.emit();
  }

  void InputArea::EmitEndKeyboardFocus()
  {
    end_key_focus.emit();
  }

  void InputArea::EmitMouseDownOutsideArea(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    mouse_down_outside_pointer_grab_area.emit(x, y, mouse_button_state, special_keys_state);
  }

  Area* InputArea::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    if (TestMousePointerInclusion(mouse_position, event_type))
    {
      return this;
    }
    return NULL;
  }

  Area* InputArea::FindKeyFocusArea(unsigned int key_symbol,
                          unsigned long x11_key_code,
                          unsigned long special_keys_state)
  {
    if (has_key_focus_)
    {
      return this;
    }
    else if (next_object_to_key_focus_area_)
    {
      return next_object_to_key_focus_area_->FindKeyFocusArea(key_symbol, x11_key_code, special_keys_state);
    }
    return NULL;
  }

  bool InputArea::AcceptKeyNavFocus()
  {
    return false;
  }
}

