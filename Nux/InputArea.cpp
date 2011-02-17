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


#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>

#include "Nux.h"
#include "InputArea.h"
#include "NuxGraphics/GraphicsEngine.h"
#include "WindowCompositor.h"

#define nuxEventDebugTrace(enable, str, ...) { if(enable) nux::LogOutputDebugMessage("#InputDebug#:" str, ##__VA_ARGS__);}

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (InputArea);

  InputArea::InputArea (NUX_FILE_LINE_DECL)
    :   Area (NUX_FILE_LINE_PARAM)
    ,   m_AreaColor (Color::Green)
  {
    SetGeometry(0, 0, 1, 1);
    _has_keyboard_focus = false;

    _capture_mouse_down_any_where_else = false;
    _double_click = false;

    _print_event_debug_trace = false;
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
   
    InputArea *keyboard_focus_area = GetWindowCompositor ().GetKeyboardFocusArea ();
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
      GetWindowCompositor ().GetMouseFocusArea ()->OnEndFocus ();
    }

    if (keyboard_focus_area != this)
    {
      GetWindowCompositor ().GetMouseFocusArea ()->OnEndFocus ();
    }

    OnStartFocus.emit ();
    _event_processor.ForceMouseFocus (x, y, GetGeometry ());

    return true;
  }

  void InputArea::ForceStopFocus (int x, int y)
  {
    if (GetWindowCompositor ().InExclusiveInputMode ())
    {
      return;
    }

    if (GetWindowCompositor ().GetMouseFocusArea () != this)
    {
      GetWindowCompositor ().SetMouseFocusArea (0);
      OnEndFocus.emit ();
      _event_processor.StopMouseFocus (x, y, GetGeometry ());
    }

    /*OnEndFocus.emit();
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

    // Regular event processing.
    if ((GetWindowCompositor ().GetExclusiveInputArea () == this) && (!(ProcessEventInfo & EVENT_CYCLE_EXCLUSIVE)))
    {
      // Skip the area that has the exclusivity on events
      return 0;
    }

    if (GetWindowCompositor ().InExclusiveInputMode ())
    {
      // Bypass the regular processing and use a simplified processing of events.
      return ProcessEventInExclusiveMode (event, TraverseInfo, ProcessEventInfo);
    }


    InputArea *PreviousMouseOverArea = GetWindowCompositor().GetPreviousMouseOverArea();
    InputArea *CurrentMouseOverArea = GetWindowCompositor().GetMouseOverArea();
    InputArea *keyboard_focus_area = GetWindowCompositor().GetKeyboardFocusArea();
    InputArea *mouse_focus_area = GetWindowCompositor().GetMouseFocusArea();

    if (ProcessEventInfo & eDoNotProcess)
    {
      nuxEventDebugTrace (_print_event_debug_trace, TEXT("The event flag info is eDoNotProcess."));
      unsigned int event_processor_state;
      event_processor_state = _event_processor.Process (event, GetGeometry(), false);

      if (event.e_event == NUX_MOUSE_PRESSED)
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed event."));
        if ((_event_processor.MouseIn () == false) && _capture_mouse_down_any_where_else)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse down outside of area. Emit OnMouseDownOutsideArea."));
          OnMouseDownOutsideArea.emit (event.e_x - event.e_x_root, event.e_y - event.e_y_root, event.GetMouseState(), event.GetKeyState() );
        }

        if (keyboard_focus_area == this)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Area has keyboard focus. Disable keyboard focus on area. Emit OnEndFocus."));
          GetWindowCompositor ().SetKeyboardFocusArea (NULL);
          OnEndFocus.emit ();
        }
      }

      nuxEventDebugTrace (_print_event_debug_trace, TEXT("Processing mouse Enter/Leave event."));
      // Even though the mouse event has been solved, the area can still respond respond to mouse Enter/Leave events.
      if (event_processor_state & AREA_MOUSE_STATUS_LEAVE)
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse leave. Emit OnMouseLeave."));
        OnMouseLeave.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
      }

      if (event_processor_state & AREA_MOUSE_STATUS_ENTER)
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse enter. Emit OnMouseEnter."));
        OnMouseEnter.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
      }

      nuxEventDebugTrace (_print_event_debug_trace, TEXT("Exit InputArea::OnEvent."));
      return TraverseInfo;
    }

    long ret = TraverseInfo;

    if (TraverseInfo & eMouseEventSolved)
    {
      unsigned int event_processor_state;
      event_processor_state = _event_processor.Process (event, GetGeometry(), false);

      nuxEventDebugTrace (_print_event_debug_trace, TEXT("The event flag info is eMouseEventSolved."));
      _event_processor._state &= ~AREA_MOUSE_STATUS_FOCUS;

      if ((CurrentMouseOverArea == 0) && (GetGeometry().IsPointInside (event.e_x - event.e_x_root, event.e_y - event.e_y_root)))
      {
        CurrentMouseOverArea = this;
      }

      if (event.e_event == NUX_MOUSE_PRESSED)
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed event."));
        if ((_event_processor.MouseIn () == false) && _capture_mouse_down_any_where_else)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse down outside of area. Emit OnMouseDownOutsideArea."));
          OnMouseDownOutsideArea.emit (event.e_x - event.e_x_root, event.e_y - event.e_y_root, event.GetMouseState(), event.GetKeyState() );
        }

        if (keyboard_focus_area == this)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Area has keyboard focus. Disable keyboard focus on area. Emit OnEndFocus."));
          GetWindowCompositor ().SetKeyboardFocusArea (NULL);
          OnEndFocus.emit ();
        }
      }

      // Even though the mouse event has been solved, the area can still respond respond to mouse Enter/Leave events.
      if (event_processor_state & AREA_MOUSE_STATUS_LEAVE)
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse leave. Emit OnMouseLeave."));
        OnMouseLeave.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
      }

      if (event_processor_state & AREA_MOUSE_STATUS_ENTER)
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse enter. Emit OnMouseEnter."));
        OnMouseEnter.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
      }

      ret |=  eMouseEventSolved;
    }
    else
    {
      unsigned long event_type = event.e_event;
      // If double click is not enable for this area, transform it to a mouse press event. 
      if ((event_type == NUX_MOUSE_DOUBLECLICK) && (_double_click == false))
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Double click received but signal not activated. Change double click to Mouse Press event."));
        event_type = NUX_MOUSE_PRESSED;
      }

      nuxEventDebugTrace (_print_event_debug_trace, TEXT("Event has not been solved."));
      unsigned int event_processor_state;
      event_processor_state = _event_processor.Process (event, GetGeometry (), true);

      if ((event_processor_state & AREA_MOUSE_STATUS_DOWN) && (mouse_focus_area == 0)) // never override an area that has the mouse focus.
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("getting the mouse focus."));
        GetWindowCompositor ().SetMouseFocusArea (this);
        GetWindowCompositor ().SetAreaEventRoot (event.e_x_root, event.e_y_root);

        //InputArea *keyboard_focus_area = GetWindowCompositor().GetKeyboardFocusArea();
        if ((keyboard_focus_area != 0) && (keyboard_focus_area != this))
        {
          keyboard_focus_area->OnEndFocus ();
        }
        if(keyboard_focus_area != this)
        {
          GetWindowCompositor ().SetKeyboardFocusArea (this);
          keyboard_focus_area = this;
          keyboard_focus_area->OnStartFocus ();
        }
      }

      // Imagine a Toolbar. When a MouseDown happens on the toolbar, it gets the mouse focus. While the mouse
      // is being pressed, we can move it above a widget that is inside the toolbar. That widget should not claim that it has
      // resolved the mouse event because the mouse is still being owned by the Toolbar. At most, the widget
      // should be set in CurrentMouseOverArea.
      if (IsMouseInside () && (GetWindowCompositor ().GetMouseFocusArea () == 0))
      {
        ret |=  eMouseEventSolved;
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
        if (PreviousMouseOverArea == this)
        {
          // The mouse was previously over this area. We should also have CurrentMouseOverArea == this.
          if (CurrentMouseOverArea)
          {
            (CurrentMouseOverArea)->OnMouseLeave.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
          }
          CurrentMouseOverArea = NULL;
          PreviousMouseOverArea = NULL;
        }
      }

      if (PreviousMouseOverArea != CurrentMouseOverArea)
      {
        if (PreviousMouseOverArea)
        {
          (PreviousMouseOverArea)->OnMouseLeave.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
          // When calling OnMouseLeave.emit on a widget, we have to set the following states to false
          (PreviousMouseOverArea)->_event_processor._current_mouse_in = false;
          (PreviousMouseOverArea)->_event_processor._previous_mouse_in = false;
          //nuxDebugMsg(TEXT("InputArea: Previous MouseOver Leave"));
        }

        if (CurrentMouseOverArea)
        {
          (CurrentMouseOverArea)->OnMouseEnter.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
          //nuxDebugMsg(TEXT("InputArea: Current MouseOver Enter"));
        }

        PreviousMouseOverArea = CurrentMouseOverArea;
      }

      if ((event_type == NUX_MOUSEWHEEL) && _event_processor.MouseIn ())
      {
        OnMouseWheel.emit (event.e_x - event.e_x_root, event.e_y - event.e_y_root,
                           event.e_wheeldelta,
                           event.GetMouseState (), event.GetKeyState ());
      }

      if ((event_type == NUX_MOUSE_PRESSED) && (event_processor_state == AREA_MOUSE_STATUS_NONE))
      {
        //nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed."));
        if ((_event_processor.MouseIn () == false) && _capture_mouse_down_any_where_else)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed outside of area. Emit OnMouseDownOutsideArea."));
          // A mouse Down happened outside of this view.
          OnMouseDownOutsideArea.emit (event.e_x - event.e_x_root, event.e_y - event.e_y_root, event.GetMouseState(), event.GetKeyState() );
        }

        if (keyboard_focus_area == this)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Has the focus but mouse pressed and outside the area. Losing keyboard focus: Emit OnEndFocus."));
          GetWindowCompositor ().SetKeyboardFocusArea (NULL);
          keyboard_focus_area = NULL;
          OnEndFocus.emit();
        }
      }
      else if ((event_type == NUX_MOUSE_DOUBLECLICK) && (event_processor_state & AREA_MOUSE_STATUS_DOWN))
      {
        if ((event.e_mouse_state & NUX_EVENT_BUTTON1_DBLCLICK))
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed inside the area. Emit OnMouseDoubleClick"));
          // Double click
          OnMouseDoubleClick.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
        }

        ret |=  eMouseEventSolved;
      }
      else if (event_processor_state & AREA_MOUSE_STATUS_DOWN) // The mouse is down inside the area
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed inside the area. Emit OnStartMouseFocus"));
        OnStartMouseFocus.emit();

        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed inside the area. Emit OnMouseDown"));
        OnMouseDown.emit (_event_processor.m_mouse_positionx - GetBaseX(),
                          _event_processor.m_mouse_positiony - GetBaseY(),
                          event.GetMouseState(), event.GetKeyState() );

        ret |=  eMouseEventSolved;
      }
      else if (event_processor_state & AREA_MOUSE_STATUS_UP) // The mouse is up. This goes to the area that had the mouse focus.
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse up. Emit OnEndMouseFocus"));
        OnEndMouseFocus.emit ();

        // This is a mouse up on an area that has the mouse focus.
        // Just check that the mouse in. If the mouse is in, then it is a "Click".
        if (_event_processor.MouseIn())
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse up and inside area. Emit OnMouseClick"));
          OnMouseClick.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
        }

        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse up. Emit OnMouseUp"));
        OnMouseUp.emit (_event_processor.m_mouse_positionx - GetBaseX(),
                        _event_processor.m_mouse_positiony - GetBaseY(),
                        event.GetMouseState(), event.GetKeyState() );

        ret |=  eMouseEventSolved;
      }
      else if ((event_processor_state & AREA_MOUSE_STATUS_MOVE) && (event_processor_state & AREA_MOUSE_STATUS_FOCUS))
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse move and has mouse focus. Emit OnMouseDrag"));
        OnMouseDrag.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(),
                          _event_processor.m_mouse_deltax, _event_processor.m_mouse_deltay,
                          event.GetMouseState(), event.GetKeyState() );

        ret |=  eMouseEventSolved;
      }
      else if (event_processor_state & AREA_MOUSE_STATUS_MOVE && (!(event_processor_state & AREA_MOUSE_STATUS_FOCUS)))
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse move and no mouse focus. Emit OnMouseMove"));
        OnMouseMove.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(),
                          _event_processor.m_mouse_deltax, _event_processor.m_mouse_deltay,
                          event.GetMouseState(), event.GetKeyState() );

        ret |=  eMouseEventSolved;
      }

      if (keyboard_focus_area == this)
      {
        if (event_type == NUX_KEYDOWN)
        {
          nuxEventDebugTrace (_print_event_debug_trace,
            TEXT("Emit OnKeyPressed"));
          OnKeyPressed.emit (event.GetKeySym(), event.e_x11_keycode,
            event.GetKeyState());

          nuxEventDebugTrace (_print_event_debug_trace,
            TEXT("Emit OnKeyEvent. String: %s"), event.e_text);
          OnKeyEvent.emit (GetWindowThread ()->GetGraphicsEngine(),
            event_type, event.GetKeySym(),
            event.GetKeyState(), event.GetText(),
            event.GetKeyRepeatCount());

          ret |=  eMouseEventSolved;
        }
        else if (event_type == NUX_KEYUP)
        {
          nuxEventDebugTrace (_print_event_debug_trace,
            TEXT("Emit OnKeyReleased"));
          OnKeyReleased.emit (event.GetKeySym(), event.e_x11_keycode,
            event.GetKeyState());

          nuxEventDebugTrace (_print_event_debug_trace,
            TEXT("Emit OnKeyEvent. String: %s"), event.e_text);
          OnKeyEvent.emit (GetWindowThread ()->GetGraphicsEngine(),
            event_type, event.GetKeySym(),
            event.GetKeyState(), event.GetText(),
            event.GetKeyRepeatCount());

          ret |=  eMouseEventSolved;
        }
      }
    }    

//     if (keyboard_focus_area == this)
//     {
//       if (event_type == NUX_KEYDOWN)
//       {
//         nuxEventDebugTrace (_print_event_debug_trace,
//                             TEXT("Emit OnKeyPressed"));
//         OnKeyPressed.emit (event.GetKeySym(), event.e_x11_keycode,
//                             event.GetKeyState());
// 
//         nuxEventDebugTrace (_print_event_debug_trace,
//                             TEXT("Emit OnKeyEvent. String: %s"), event.e_text);
//         OnKeyEvent.emit (GetWindowThread ()->GetGraphicsEngine(),
//                          event_type, event.GetKeySym(),
//                          event.GetKeyState(), event.GetText(),
//                          event.GetKeyRepeatCount());
// 
//         ret |=  eMouseEventSolved;
//       }
//       else if (event_type == NUX_KEYUP)
//       {
//         nuxEventDebugTrace (_print_event_debug_trace,
//                             TEXT("Emit OnKeyReleased"));
//         OnKeyReleased.emit (event.GetKeySym(), event.e_x11_keycode,
//                             event.GetKeyState());
// 
//         nuxEventDebugTrace (_print_event_debug_trace,
//                             TEXT("Emit OnKeyEvent. String: %s"), event.e_text);
//         OnKeyEvent.emit (GetWindowThread ()->GetGraphicsEngine(),
//                          event_type, event.GetKeySym(),
//                          event.GetKeyState(), event.GetText(),
//                          event.GetKeyRepeatCount());
// 
//         ret |=  eMouseEventSolved;
//       }
//     }

    GetWindowCompositor().SetPreviousMouseOverArea (PreviousMouseOverArea);
    GetWindowCompositor().SetMouseOverArea (CurrentMouseOverArea);

    return ret;
  }

  long InputArea::ProcessEventInExclusiveMode (Event &event, long TraverseInfo, long ProcessEventInfo)
  {
    unsigned int mouse_status;
    mouse_status = _event_processor.Process (event, GetGeometry(), false);

    if (_event_processor.MouseIn () && (event.e_event == NUX_MOUSE_PRESSED))
    {
      OnMouseDown.emit (_event_processor.m_mouse_positionx - GetBaseX(),
        _event_processor.m_mouse_positiony - GetBaseY(),
        event.GetMouseState(), event.GetKeyState() );
    }

    if (_event_processor.MouseIn () && (event.e_event == NUX_MOUSE_RELEASED))
    {
      OnMouseUp.emit (_event_processor.m_mouse_positionx - GetBaseX(),
        _event_processor.m_mouse_positiony - GetBaseY(),
        event.GetMouseState(), event.GetKeyState() );
    }

    if (_event_processor.MouseIn () && (event.e_event == NUX_MOUSE_MOVE))
    {
      OnMouseMove.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(),
        _event_processor.m_mouse_deltax, _event_processor.m_mouse_deltay,
        event.GetMouseState(), event.GetKeyState() );
    }

    if (mouse_status & AREA_MOUSE_STATUS_LEAVE)
    {
      OnMouseLeave.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
    }

    if (mouse_status & AREA_MOUSE_STATUS_ENTER)
    {
      OnMouseEnter.emit (_event_processor.m_mouse_positionx - GetBaseX(), _event_processor.m_mouse_positiony - GetBaseY(), event.GetMouseState(), event.GetKeyState() );
    }

    return 0;
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
    return GetWindowCompositor ().GetKeyboardFocusArea () == this;
  }

  void InputArea::SetKeyboardFocus (bool b)
  {
    _has_keyboard_focus = b;
  }

  int InputArea::GetMouseX()
  {
    return _event_processor.m_mouse_positionx - GetBaseX();
  }

  int InputArea::GetMouseY()
  {
    return _event_processor.m_mouse_positiony - GetBaseY();
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

  void InputArea::EnableDoubleClick (bool b)
  {
    _double_click = b;
  }

  bool InputArea::IsDoubleClickEnabled()
  {
    return _double_click;
  }

  void InputArea::SetAreaMousePosition (int x, int y)
  {
    _event_processor.m_mouse_positionx = x;
    _event_processor.m_mouse_positiony = y;
  }
  
  void InputArea::EnableEventDebugTrace (bool enable)
  {
    _print_event_debug_trace = enable;
  }
  
  bool InputArea::GetEventDebugTrace () const
  {
    return _print_event_debug_trace;
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
#endif
}

