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

  NUX_IMPLEMENT_OBJECT_TYPE (InputArea);

  InputArea::InputArea (NUX_FILE_LINE_DECL)
    :   Area (NUX_FILE_LINE_PARAM)
    ,   m_AreaColor (color::Green)
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

  unsigned short InputArea::getKeyState (int nVirtKey)
  {
#ifdef WIN32

    if (HasKeyboardFocus() )
      return GetKeyState (nVirtKey);
    else
      return 0;

#else
    return 0;
#endif
  }

  bool InputArea::ForceStartFocus (int x, int y)
  {
    if (GetWindowCompositor ().InExclusiveInputMode ())
    {
      return false ;
    }

    InputArea *keyboard_focus_area = GetWindowCompositor ().GetKeyboardEventReceiver ();
    InputArea *mouse_focus_area = GetWindowCompositor ().GetMouseFocusArea ();

    if ((mouse_focus_area == this) && (keyboard_focus_area == this))
    {
      // already has the focus
      return false;
    }

    Geometry geo = GetGeometry ();
    geo.SetPosition (0, 0);
    if (!geo.IsPointInside (x, y))
    {
      return false;
    }

    if (mouse_focus_area != this)
    {
      GetWindowCompositor ().GetMouseFocusArea ()->OnStopKeyboardReceiver ();
    }

    if (keyboard_focus_area != this)
    {
      GetWindowCompositor ().GetMouseFocusArea ()->OnStopKeyboardReceiver ();
    }

    OnStartKeyboardReceiver.emit ();
    _event_processor.ForceMouseFocus (x, y, GetGeometry ());

    return true;
  }

  void InputArea::ForceStopFocus (int x, int y)
  {
    if (GetWindowCompositor ().InExclusiveInputMode ())
    {
      return;
    }

    if (GetWindowCompositor ().GetMouseFocusArea () == this)
    {
      GetWindowCompositor ().SetMouseFocusArea (0);
      OnStopKeyboardReceiver.emit ();
      _event_processor.StopMouseFocus (x, y, GetGeometry ());
    }

    /*OnStopKeyboardReceiver.emit();
    _event_processor.StopMouseFocus (x, y, GetGeometry());

    if (GetWindowCompositor().GetMouseFocusArea () == this)
    {
      GetWindowCompositor().SetMouseFocusArea (0);
    }*/
  }

  long InputArea::OnEvent (Event &event, long TraverseInfo, long ProcessEventInfo)
  {
    // Mouse event processing.
    if ((event.e_event >= NUX_DND_MOVE) && (event.e_event <= NUX_DND_LEAVE_WINDOW))
    {
      if (TraverseInfo & eMouseEventSolved) // It is not mouse event but let use this enum for now.
      {
        return eMouseEventSolved;
      }

      // We are in the range of DND events

      if (event.e_event == NUX_DND_MOVE)
      {
        InputArea *current_dnd_area = GetWindowCompositor().GetDnDArea();
        if (GetGeometry().IsPointInside (event.e_x - event.e_x_root, event.e_y - event.e_y_root))
        {
          if (current_dnd_area != this)
          {
            // We just entered this area.
            GetWindowCompositor().SetDnDArea(this);
            HandleDndMove(event);

            return eMouseEventSolved;
          }
          else
          {
            HandleDndMove(event);
            return eMouseEventSolved;
          }
        }
        else
        {
          if (GetWindowCompositor().GetDnDArea() == this)
          {
            // we are going out of this area
            GetWindowCompositor().SetDnDArea(NULL);
          }
        }
      }

      if (event.e_event == NUX_DND_DROP)
      {
        InputArea *current_dnd_area = GetWindowCompositor().GetDnDArea();
        if ((current_dnd_area == this) && GetGeometry().IsPointInside (event.e_x - event.e_x_root, event.e_y - event.e_y_root))
        {
          HandleDndDrop(event);
          return eMouseEventSolved;
        }
      }
      return TraverseInfo;
    }

    InputArea *PreviousMouseOverArea = GetWindowCompositor().GetPreviousMouseOverArea();
    InputArea *CurrentMouseOverArea = GetWindowCompositor().GetMouseOverArea();
    InputArea *keyboard_focus_area = GetWindowCompositor().GetKeyboardEventReceiver();
    InputArea *mouse_focus_area = GetWindowCompositor().GetMouseFocusArea();

    if (ProcessEventInfo & EVENT_CYCLE_RESET_AREA_STATE)
    {
      _event_processor.ResetState ();
    }

    if (ProcessEventInfo & eDoNotProcess)
    {
      LOG_TRACE(logger) << "The event flag info is eDoNotProcess.";
      unsigned int event_processor_state;
      event_processor_state = _event_processor.EventProcessor (event, GetRootGeometry (), false);

      if (event.e_event == NUX_MOUSE_PRESSED)
      {
        LOG_TRACE(logger) << "Mouse pressed event.";
        if ((_event_processor.MouseIn () == false) && _capture_mouse_down_any_where_else)
        {
          LOG_TRACE(logger) << "Mouse down outside of area. "
                            << "Emit OnMouseDownOutsideArea.";
          OnMouseDownOutsideArea.emit (event.e_x - event.e_x_root, event.e_y - event.e_y_root, event.GetMouseState(), event.GetKeyState() );
        }

        if (keyboard_focus_area == this)
        {
          LOG_TRACE(logger) << "Area has keyboard focus. Disable keyboard focus on area. Emit OnEndFocus.";
        }
      }

      if (PreviousMouseOverArea == this)
      {
        GetWindowCompositor().SetPreviousMouseOverArea (NULL);
      }

      if (CurrentMouseOverArea == this)
      {
        GetWindowCompositor().SetMouseOverArea (NULL);
      }

      LOG_TRACE(logger) << "Processing mouse Enter/Leave event.";
      // Even though the mouse event has been solved, the area can still respond respond to mouse Enter/Leave events.
      if (event_processor_state & AREA_MOUSE_STATUS_LEAVE)
      {
        LOG_TRACE(logger) << "Mouse leave. Emit OnMouseLeave.";
        OnMouseLeave.emit (_event_processor._mouse_positionx - GetRootX(), _event_processor._mouse_positiony - GetRootY(), event.GetMouseState(), event.GetKeyState() );
      }

      if (event_processor_state & AREA_MOUSE_STATUS_ENTER)
      {
        LOG_TRACE(logger) << "Mouse enter. Emit OnMouseEnter.";
        OnMouseEnter.emit (_event_processor._mouse_positionx - GetRootX(), _event_processor._mouse_positiony - GetRootY(), event.GetMouseState(), event.GetKeyState() );
      }

      LOG_TRACE(logger) << "Exit InputArea::OnEvent.";
      return TraverseInfo;
    }

    long ret = TraverseInfo;

    if (TraverseInfo & eMouseEventSolved)
    {
      unsigned int event_processor_state;
      event_processor_state = _event_processor.EventProcessor (event, GetRootGeometry (), false);

      LOG_TRACE(logger) << "The event flag info is eMouseEventSolved.";
      _event_processor._state &= ~AREA_MOUSE_STATUS_FOCUS;

      if ((CurrentMouseOverArea == 0) && (GetRootGeometry ().IsPointInside (event.e_x - event.e_x_root, event.e_y - event.e_y_root)))
      {
        CurrentMouseOverArea = this;
      }

      if (event.e_event == NUX_MOUSE_PRESSED)
      {
        LOG_TRACE(logger) << "Mouse pressed event.";
        if ((_event_processor.MouseIn () == false) && _capture_mouse_down_any_where_else)
        {
          LOG_TRACE(logger) << "Mouse down outside of area. Emit OnMouseDownOutsideArea.";
          OnMouseDownOutsideArea.emit (event.e_x - event.e_x_root, event.e_y - event.e_y_root, event.GetMouseState(), event.GetKeyState() );
        }

        if (keyboard_focus_area == this)
        {
          LOG_TRACE(logger) << "Area has keyboard focus. Disable keyboard focus on area. Emit OnEndFocus.";
        }
      }

      // Even though the mouse event has been solved, the area can still respond respond to mouse Enter/Leave events.
      if (event_processor_state & AREA_MOUSE_STATUS_LEAVE)
      {
        LOG_TRACE(logger) << "Mouse leave. Emit OnMouseLeave.";
        OnMouseLeave.emit (_event_processor._mouse_positionx - GetRootX(), _event_processor._mouse_positiony - GetRootY(), event.GetMouseState(), event.GetKeyState() );
      }

      if(event_processor_state & AREA_MOUSE_STATUS_ENTER)
      {
        LOG_TRACE(logger) << "Mouse enter. Emit OnMouseEnter.";
        OnMouseEnter.emit (_event_processor._mouse_positionx - GetRootX(), _event_processor._mouse_positiony - GetRootY(), event.GetMouseState(), event.GetKeyState() );
      }

      ret |= eMouseEventSolved;
    }
    else
    {
      unsigned long event_type = event.e_event;
      // If double click is not enable for this area, transform it to a mouse press event.
      if ((event_type == NUX_MOUSE_DOUBLECLICK) && (_double_click == false))
      {
        LOG_TRACE(logger) << "Double click received but signal not activated. "
                          << "Change double click to Mouse Press event.";
        event_type = NUX_MOUSE_PRESSED;
      }

      LOG_TRACE(logger) << "Event has not been solved.";
      unsigned int event_processor_state;
      event_processor_state = _event_processor.EventProcessor (event, GetRootGeometry (), true);

      if ((event_processor_state & AREA_MOUSE_STATUS_DOWN) && (mouse_focus_area == 0)) // never override an area that has the mouse focus.
      {
        LOG_TRACE(logger) << "getting the mouse focus.";
        GetWindowCompositor ().SetMouseFocusArea (this);
        GetWindowCompositor ().SetAreaEventRoot (event.e_x_root, event.e_y_root);

        //InputArea *keyboard_focus_area = GetWindowCompositor().GetKeyboardEventReceiver();
        if ((keyboard_focus_area != 0) && (keyboard_focus_area != this))
        {
          keyboard_focus_area->OnStopKeyboardReceiver.emit ();
        }
        if(keyboard_focus_area != this)
        {
          GetWindowCompositor ().SetKeyboardEventReceiver (this);
          keyboard_focus_area = this;
        }
      }

      if (event_type == NUX_WINDOW_EXIT_FOCUS)
      {
        if (keyboard_focus_area == this)
        {
          GetWindowCompositor ().SetKeyboardEventReceiver (NULL);
          keyboard_focus_area = NULL;
        }
      }

      /*if (event_type == NUX_WINDOW_MOUSELEAVE && (!(event_processor_state & AREA_MOUSE_STATUS_FOCUS)))
      {
        if (mouse_focus_area == this)
        {
          GetWindowCompositor ().SetMouseFocusArea (NULL);
          mouse_focus_area = NULL;
          OnMouseLeave.emit(_event_processor._mouse_positionx - GetRootX(),
          _event_processor._mouse_positiony - GetRootY(),
          event.GetMouseState(),
          event.GetKeyState() );

          _event_processor._current_mouse_in = false;
          _event_processor._previous_mouse_in = false;
        }
      }*/

      // Imagine a Toolbar. When a MouseDown happens on the toolbar, it gets the mouse focus. While the mouse
      // is being pressed, we can move it above a widget that is inside the toolbar. That widget should not claim that it has
      // resolved the mouse event because the mouse is still being owned by the Toolbar. At most, the widget
      // should be set in CurrentMouseOverArea.
      if (IsMouseInside () && (GetWindowCompositor ().GetMouseFocusArea () == 0))
      {
        ret |= eMouseEventSolved;
      }

      if (IsMouseInside ())
      {
        if (CurrentMouseOverArea != this)
        {
          PreviousMouseOverArea = CurrentMouseOverArea;
        }
        CurrentMouseOverArea = this;
      }
      else
      {
        // The mouse is outside this widget. If the PreviousMouseOverArea == this, then call OnMouseLeave.
        LOG_TRACE(logger) << "The mouse is outside the area.";
        if (PreviousMouseOverArea == this)
        {
          LOG_TRACE(logger) << "The mouse was previously above this area.";
          // The mouse was previously over this area. We should also have CurrentMouseOverArea == this.
          if (CurrentMouseOverArea)
          {
            if (CurrentMouseOverArea == this)
            {
              LOG_TRACE(logger) << "Send OnMouseLeave to this area.";
            }
            else
            {
              LOG_TRACE(logger) << "Send OnMouseLeave to the area that was directly below the mouse.";
            }

            (CurrentMouseOverArea)->OnMouseLeave.emit(_event_processor._mouse_positionx - GetRootX(),
              _event_processor._mouse_positiony - GetRootY(),
              event.GetMouseState(),
              event.GetKeyState());
          }
          CurrentMouseOverArea = NULL;
          PreviousMouseOverArea = NULL;
        }
      }

      if (PreviousMouseOverArea != CurrentMouseOverArea)
      {
        LOG_TRACE(logger) << "The mouse has entered a new area and/or exited and area.";
        if (PreviousMouseOverArea)
        {
          LOG_TRACE(logger) << "The mouse has exited and area. "
                            << "Send OnMouseLeave to the area that was directly below the mouse.";
          (PreviousMouseOverArea)->OnMouseLeave.emit (_event_processor._mouse_positionx - GetRootX(), _event_processor._mouse_positiony - GetRootY(), event.GetMouseState(), event.GetKeyState() );
          // When calling OnMouseLeave.emit on a widget, we have to set the following states to false
          (PreviousMouseOverArea)->_event_processor._current_mouse_in = false;
          (PreviousMouseOverArea)->_event_processor._previous_mouse_in = false;
          //nuxDebugMsg(TEXT("InputArea: Previous MouseOver Leave"));
        }

        if (CurrentMouseOverArea)
        {
          LOG_TRACE(logger) << "The mouse has entered and area."
                            << "Send OnMouseEnter to the area that the mouse has entered.";
          (CurrentMouseOverArea)->OnMouseEnter.emit (_event_processor._mouse_positionx - GetRootX(), _event_processor._mouse_positiony - GetRootY(), event.GetMouseState(), event.GetKeyState() );
          //nuxDebugMsg(TEXT("InputArea: Current MouseOver Enter"));
        }

        PreviousMouseOverArea = CurrentMouseOverArea;
      }

      if ((event_type == NUX_MOUSE_WHEEL) && _event_processor.MouseIn ())
      {
        OnMouseWheel.emit (event.e_x - event.e_x_root, event.e_y - event.e_y_root,
                           event.e_wheeldelta,
                           event.GetMouseState (), event.GetKeyState ());
      }

      if ((event_type == NUX_MOUSE_PRESSED) && (event_processor_state == AREA_MOUSE_STATUS_NONE))
      {
        if ((_event_processor.MouseIn () == false) && _capture_mouse_down_any_where_else)
        {
          LOG_TRACE(logger) << "Mouse pressed outside of area. Emit OnMouseDownOutsideArea.";
          // A mouse Down happened outside of this view.
          OnMouseDownOutsideArea.emit (event.e_x - event.e_x_root, event.e_y - event.e_y_root, event.GetMouseState(), event.GetKeyState() );
        }

        if (keyboard_focus_area == this)
        {
          LOG_TRACE(logger) << "Has the focus but mouse pressed and outside the area. Losing keyboard focus: Emit OnEndFocus.";
          keyboard_focus_area = NULL;
        }
      }
      else if((event_type == NUX_MOUSE_DOUBLECLICK) && (event_processor_state & AREA_MOUSE_STATUS_DOWN))
      {
        if ((event.e_mouse_state & NUX_EVENT_BUTTON1_DBLCLICK))
        {
          LOG_TRACE(logger) << "Mouse pressed inside the area. Emit OnMouseDoubleClick";
          // Double click
          OnMouseDoubleClick.emit(_event_processor._mouse_positionx - GetRootX(), _event_processor._mouse_positiony - GetRootY(), event.GetMouseState(), event.GetKeyState());
        }

        ret |= eMouseEventSolved;
      }
      else if (event_processor_state & AREA_MOUSE_STATUS_DOWN) // The mouse is down inside the area
      {
        LOG_TRACE(logger) << "Mouse pressed inside the area. Emit OnStartMouseFocus";
        OnStartMouseFocus.emit();

        LOG_TRACE(logger) << "Mouse pressed inside the area. Emit OnMouseDown";
        OnMouseDown.emit (_event_processor._mouse_positionx - GetRootX(),
                          _event_processor._mouse_positiony - GetRootY(),
                          event.GetMouseState(), event.GetKeyState() );

        ret |= eMouseEventSolved;
      }
      else if(event_processor_state & AREA_MOUSE_STATUS_UP) // The mouse is up. This goes to the area that had the mouse focus.
      {
        LOG_TRACE(logger) << "Mouse up. Emit OnEndMouseFocus";
        OnEndMouseFocus.emit ();

        // This is a mouse up on an area that has the mouse focus.
        // Just check that the mouse in. If the mouse is in, then it is a "Click".
        if (_event_processor.MouseIn())
        {
          LOG_TRACE(logger) << "Mouse up and inside area. Emit OnMouseClick";

          OnMouseClick.emit(_event_processor._mouse_positionx - GetRootX(),
                            _event_processor._mouse_positiony - GetRootY(),
                            event.GetMouseState(), event.GetKeyState() );
        }

        LOG_TRACE(logger) << "Mouse up. Emit OnMouseUp";
        OnMouseUp.emit(_event_processor._mouse_positionx - GetRootX(),
                       _event_processor._mouse_positiony - GetRootY(),
                       event.GetMouseState(), event.GetKeyState() );

        ret |=  eMouseEventSolved;
      }
      else if ((event_processor_state & AREA_MOUSE_STATUS_MOVE) &&
               (event_processor_state & AREA_MOUSE_STATUS_FOCUS))
      {
        int delta_x = _event_processor._mouse_deltax;
        int delta_y = _event_processor._mouse_deltay;
        int x = _event_processor._mouse_positionx - GetRootX ();
        int y = _event_processor._mouse_positiony - GetRootY ();

        if (_dnd_enabled_as_source)
        {
          _dnd_safety_x += delta_x;
          _dnd_safety_y += delta_y;
          
          if (abs (_dnd_safety_x) > 30 || abs (_dnd_safety_y) > 30)
          {
            ForceStopFocus (x, y);
#if defined (NUX_OS_LINUX)
            StartDragAsSource ();
#endif
            _dnd_safety_x = 0;
            _dnd_safety_y = 0;
          }
        }
        else
        {
          LOG_TRACE(logger) << "Mouse move and has mouse focus. Emit OnMouseDrag";
          OnMouseDrag.emit (_event_processor._mouse_positionx - GetRootX(), _event_processor._mouse_positiony - GetRootY(),
                            _event_processor._mouse_deltax, _event_processor._mouse_deltay,
                            event.GetMouseState(), event.GetKeyState() );
        }
        ret |=  eMouseEventSolved;
      }
      else if ((event_processor_state & AREA_MOUSE_STATUS_MOVE) && (!(event_processor_state & AREA_MOUSE_STATUS_FOCUS)))
      {
        LOG_TRACE(logger) << "Mouse move and no mouse focus. Emit OnMouseMove";
        OnMouseMove.emit (_event_processor._mouse_positionx - GetRootX(), _event_processor._mouse_positiony - GetRootY(),
                          _event_processor._mouse_deltax, _event_processor._mouse_deltay,
                          event.GetMouseState(), event.GetKeyState() );

        ret |=  eMouseEventSolved;
      }

      if(keyboard_focus_area == this)
      {
        if(event_type == NUX_KEYDOWN)
        {
          LOG_TRACE(logger) << "Key down and has keyboard focus: Emit OnKeyPressed";
          OnKeyPressed.emit (event.GetKeySym(), event.e_x11_keycode,
            event.GetKeyState());

          LOG_TRACE(logger) << "Key down and has keyboard focus: Emit OnKeyEvent. "
                            << "String: " << event.e_text;

          OnKeyEvent.emit(GetWindowThread()->GetGraphicsEngine(),
                          event_type, event.GetKeySym(),
                          event.GetKeyState(), event.GetText(),
                          event.GetKeyRepeatCount());

          ret |=  eMouseEventSolved;
        }
        else if(event_type == NUX_KEYUP)
        {
          LOG_TRACE(logger) << "Key up and has keyboard focus: Emit OnKeyReleased";
          OnKeyReleased.emit (event.GetKeySym(), event.e_x11_keycode,
            event.GetKeyState());

          LOG_TRACE(logger) << "Key up and has keyboard focus: Emit OnKeyEvent. "
                            << "String: " << event.e_text;
          OnKeyEvent.emit (GetWindowThread ()->GetGraphicsEngine(),
            event_type, event.GetKeySym(),
            event.GetKeyState(), event.GetText(),
            event.GetKeyRepeatCount());

          ret |=  eMouseEventSolved;
        }
      }
      else if ((event_type == NUX_KEYDOWN) || (event_type == NUX_KEYUP))
      {
        LOG_TRACE(logger) << "Key event but does not have the keyboard focus.";
      }
    }

    GetWindowCompositor().SetPreviousMouseOverArea (PreviousMouseOverArea);
    GetWindowCompositor().SetMouseOverArea (CurrentMouseOverArea);

    return ret;
  }

  void InputArea::OnDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.QRP_Color (GetBaseX(), GetBaseY(), GetBaseWidth(), GetBaseHeight(), m_AreaColor);
  }

  void InputArea::SetBaseString (const TCHAR *Caption)
  {
    Area::SetBaseString (Caption);
  }

  bool InputArea::HasKeyboardFocus()
  {
    return GetWindowCompositor ().GetKeyboardEventReceiver () == this;
  }

  void InputArea::SetKeyboardFocus (bool b)
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

  bool InputArea::MouseFocusOnOtherArea()
  {
    return (GetWindowCompositor().GetMouseFocusArea() == 0);
  }

  void InputArea::CaptureMouseDownAnyWhereElse (bool b)
  {
    _capture_mouse_down_any_where_else = b;
  }

  bool InputArea::IsCaptureMouseDownAnyWhereElse() const
  {
    return _capture_mouse_down_any_where_else;
  }

  void InputArea::SetEnableDoubleClickEnable (bool double_click)
  {
    _double_click = double_click;
  }

  bool InputArea::DoubleClickEnable() const
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

  void InputArea::SetAreaMousePosition (int x, int y)
  {
    _event_processor._mouse_positionx = x;
    _event_processor._mouse_positiony = y;
  }

  void InputArea::HandleDndMove (Event &event)
  {
#if defined (NUX_OS_LINUX)
    std::list<char *> mimes;

    mimes = GetWindow ().GetDndMimeTypes ();
    std::list<char *>::iterator it;
    ProcessDndMove (event.e_x, event.e_y, mimes);

    for (it = mimes.begin (); it != mimes.end (); it++)
      g_free (*it);
#endif
  }

  void InputArea::HandleDndDrop (Event &event)
  {
#if defined (NUX_OS_LINUX)
    ProcessDndDrop (event.e_x, event.e_y);
#endif
  }

#if defined (NUX_OS_LINUX)
  void InputArea::SendDndStatus (bool accept, DndAction action, Geometry region)
  {
    GetWindow ().SendDndStatus (accept, action, Rect (region.x, region.y, region.width, region.height));
  }

  void InputArea::SendDndFinished (bool accepted, DndAction action)
  {
    GetWindow ().SendDndFinished (accepted, action);
  }

  void InputArea::ProcessDndMove (int x, int y, std::list<char *>mimes)
  {
    // must learn to deal with x/y offsets
    Area *parent = GetToplevel ();

    if (parent)
    {
      x += parent->GetGeometry ().x;
      y += parent->GetGeometry ().y;
    }

    SendDndStatus (false, DNDACTION_NONE, Geometry (x, y, GetGeometry ().width, GetGeometry ().height));
  }

  void InputArea::ProcessDndDrop (int x, int y)
  {
    SendDndFinished (false, DNDACTION_NONE);
  }

  void InputArea::ProcessDndEnter ()
  {
  }

  void InputArea::ProcessDndLeave ()
  {
  }
  
  void InputArea::SetDndEnabled (bool as_source, bool as_target)
  {
    _dnd_enabled_as_source = as_source;
    _dnd_enabled_as_target = as_target;
  }
  
  void InputArea::DndSourceDragBegin ()
  {
  
  }
  
  NBitmapData * InputArea::DndSourceGetDragImage ()
  {
    return 0;
  }
  
  std::list<const char *> InputArea::DndSourceGetDragTypes()
  {
    std::list<const char *> types;
    types.push_back ("text/plain;charset=utf-8");
    types.push_back ("UTF8_STRING");
    return types;
  }
    
  const char * InputArea::DndSourceGetDataForType(const char *type, int *size, int *format)
  {
    *format = 8;

    if (g_str_equal (type, "text/plain;charset=utf-8") || g_str_equal (type, "UTF8_STRING"))
    {
      *size = (int) strlen ("this is just a test");
      return "this is just a test";
    }
    
    *size = 0;
    return 0;
  }
  
  void InputArea::InnerDndSourceDragFinished(DndAction result, void *data) 
  { 
    InputArea *self = static_cast<InputArea *> (data);
    self->DndSourceDragFinished (result);
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
    
    DndSourceDragBegin();
    GetWindow().StartDndDrag(funcs, this);
  }
#endif

  void InputArea::DoSetFocused(bool focused)
  {
    Area::DoSetFocused(focused);
    SetKeyboardFocus(focused);
  }
  
  void InputArea::GrabPointer()
  {
    GetWindowCompositor().GrabPointerAdd (this);
  }
  
  void InputArea::UnGrabPointer()
  {
    GetWindowCompositor ().GrabPointerRemove (this);
  }

  void InputArea::GrabKeyboard()
  {
    GetWindowCompositor().GrabKeyboardAdd (this);
  }
  
  void InputArea::UnGrabKeyboard()
  {
    GetWindowCompositor().GrabKeyboardRemove (this);
  }
  
  bool InputArea::OwnsPointerGrab()
  {
    return GetWindowCompositor().GetPointerGrabArea() == this;
  }
  
  bool InputArea::OwnsKeyboardGrab ()
  {
    return GetWindowCompositor ().GetKeyboardGrabArea() == this;
  }

  bool InputArea::IsMouseOwner()
  {
    return NUX_STATIC_CAST(InputArea*, GetWindowCompositor().GetMouseOwner()) == this;
  }

  // == Signals with 1 to 1 mapping to input device ==
  void InputArea::EmitMouseDownSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    OnMouseDown.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseUpSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    OnMouseUp.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseMoveSignal(int x, int y, int dx, int dy, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    OnMouseMove.emit(x, y, dx, dy, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseWheelSignal(int x, int y, int wheel_delta, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    OnMouseWheel.emit(x, y, wheel_delta, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitKeyDownSignal(unsigned int key_symbol, unsigned long x11_key_code, unsigned long special_keys_state)
  {
    OnKeyPressed.emit(key_symbol, x11_key_code, special_keys_state);
  }

  void InputArea::EmitKeyUpSignal(unsigned int key_symbol, unsigned long x11_key_code, unsigned long special_keys_state)
  {
    OnKeyReleased.emit(key_symbol, x11_key_code, special_keys_state);
  }

  void InputArea::EmitKeyEventSignal(unsigned long event_type,
    unsigned int key_sym,
    unsigned long special_keys_state,
    const char* text,
    int key_repeat_count)
  {
    OnKeyEvent.emit(GetWindowThread()->GetGraphicsEngine(),
      event_type,
      key_sym,
      special_keys_state,
      text,
      key_repeat_count);
  }

  void InputArea::EmitMouseDragSignal(int x, int y, int dx, int dy, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    OnMouseDrag.emit(x, y, dx, dy, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseEnterSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    _event_processor._current_mouse_in = true;
    OnMouseEnter.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseLeaveSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    _event_processor._current_mouse_in = false;
    OnMouseLeave.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseClickSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    OnMouseClick.emit(x, y, mouse_button_state, special_keys_state);
  }

  void InputArea::EmitMouseDoubleClickSignal(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    OnMouseDoubleClick.emit(x, y, mouse_button_state, special_keys_state);
  }


  void InputArea::EmitStartKeyboardFocus()
  {
    OnStartKeyboardReceiver.emit();
  }

  void InputArea::EmitEndKeyboardFocus()
  {
    OnStopKeyboardReceiver.emit();
  }

  void InputArea::EmitMouseDownOutsideArea(int x, int y, unsigned long mouse_button_state, unsigned long special_keys_state)
  {
    OnMouseDownOutsideArea.emit(x, y, mouse_button_state, special_keys_state);
  }

  Area* InputArea::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    if(TestMousePointerInclusion(mouse_position, event_type))
    {
      return NUX_STATIC_CAST(Area*, this);
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

