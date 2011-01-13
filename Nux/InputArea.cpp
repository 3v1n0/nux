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

#define nuxEventDebugTrace(enable, str, ...)           { if(enable) nux::LogOutputDebugMessage(str, ##__VA_ARGS__);}

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (InputArea);

  InputArea::InputArea (NUX_FILE_LINE_DECL)
    :   Area (NUX_FILE_LINE_PARAM)
    ,   m_AreaColor (Color::Green)
  {
    //m_Handle = ++s_Handle;

    m_Geometry.SetX (0);
    m_Geometry.SetY (0);
    m_Geometry.SetWidth (1);
    m_Geometry.SetHeight (1);
    m_hasKeyboardFocus = false;

    m_CaptureMouseDownAnyWhereElse = false;
    m_EnableDoubleClick = false;
    m_EnableUserKeyboardProcessing = false;

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

  void InputArea::ForceStartFocus (int x, int y)
  {
    if (GetWindowCompositor().InExclusiveInputMode ())
    {
      return;
    }
    
    // emit OnEndFocus on the area that "had" the focus
    // previous_focus_area->OnEndFocus ();

    OnStartFocus.emit();
    m_EventHandler.ForceMouseFocus (x, y, m_Geometry);
  }

  void InputArea::ForceStopFocus (int x, int y)
  {
    if (GetWindowCompositor().InExclusiveInputMode ())
    {
      return;
    }

    OnEndFocus.emit();
    m_EventHandler.StopMouseFocus (x, y, m_Geometry);
    
//     if (GetWindowCompositor().m_PreviousMouseOverArea == this)
//     {
//       GetWindowCompositor().SetPreviousMouseOverArea (0);
//     }
    
//     if (GetWindowCompositor().GetMouseOverArea () == this)
//     {
//       GetWindowCompositor().SetMouseOverArea (0);
//     }
    
    if (GetWindowCompositor().GetMouseFocusArea () == this)
    {
      GetWindowCompositor().SetMouseFocusArea (0);
    }
  }

  long InputArea::OnEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    if ((GetWindowCompositor ().GetExclusiveInputArea () == this) && (!(ProcessEventInfo & EVENT_CYCLE_EXCLUSIVE)))
    {
      // Skip the area that has the exclusivity on events
      return 0;
    }

    if (GetWindowCompositor ().InExclusiveInputMode ())
    {
      // Bypass the regular processing and use a simplified processing of events.
      return ProcessEventInExclusiveMode (ievent, TraverseInfo, ProcessEventInfo);
    }

    InputArea *PreviousMouseOverArea = (GetWindowCompositor().m_PreviousMouseOverArea);
    InputArea *CurrentMouseOverArea = (GetWindowCompositor().m_MouseOverArea);

    if (ProcessEventInfo & eDoNotProcess)
    {
      nuxEventDebugTrace (_print_event_debug_trace, TEXT("The event flag info is eDoNotProcess."));

      if (ievent.e_event == NUX_MOUSE_PRESSED)
      {
        if (HasKeyboardFocus() )
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Area has keyboard focus. Disable keyboard focus on area. Emit OnEndFocus."));
          SetKeyboardFocus (false);
          OnEndFocus.emit();
        }

        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Event = Mouse down event."));
        if (m_CaptureMouseDownAnyWhereElse)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("m_CaptureMouseDownAnyWhereElse is true. Emit signal OnMouseDownOutsideArea."));
          OnMouseDownOutsideArea.emit (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.GetMouseState(), ievent.GetKeyState() );
        }
      }

      nuxEventDebugTrace (_print_event_debug_trace, TEXT("Processing mouse Enter/Leave event."));
      unsigned int mouse_signals;
      mouse_signals = m_EventHandler.Process (ievent, m_Geometry, false);

      if (mouse_signals & AREA_MOUSE_STATUS_LEAVE)
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse is leaving the area. Emit OnMouseLeave."));
        OnMouseLeave.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
      }

      if (mouse_signals & AREA_MOUSE_STATUS_ENTER)
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse is entering the area. Emit OnMouseEnter."));
        OnMouseEnter.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
      }

      nuxEventDebugTrace (_print_event_debug_trace, TEXT("Exit InputArea::OnEvent."));
      return TraverseInfo;
    }

    long ret = TraverseInfo;

    if (TraverseInfo & eMouseEventSolved)
    {
      nuxEventDebugTrace (_print_event_debug_trace, TEXT("The event flag info is eMouseEventSolved."));
      m_MouseEventCurrent.MouseIn = false;
      m_EventHandler.SetMouseFocus (false);

      ret |=  eMouseEventSolved;

      if ((CurrentMouseOverArea == 0) && (GetGeometry().IsPointInside (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root)))
      {
        CurrentMouseOverArea = this;
      }

      if (ievent.e_event == NUX_MOUSE_PRESSED)
      {
        if (HasKeyboardFocus() )
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Area has keyboard focus. Disable keyboard focus on area. Emit OnEndFocus."));
          SetKeyboardFocus (false);
          OnEndFocus.emit();
        }

        if (m_CaptureMouseDownAnyWhereElse)
        {
          OnMouseDownOutsideArea.emit (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.GetMouseState(), ievent.GetKeyState() );
        }
      }

      // Even if it is eMouseEventSolved, we still want to respond to mouse Enter/Leave events.
      {
        unsigned int mouse_signals;
        mouse_signals = m_EventHandler.Process (ievent, m_Geometry, false);

        if (mouse_signals & AREA_MOUSE_STATUS_LEAVE)
        {
          OnMouseLeave.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }

        if (mouse_signals & AREA_MOUSE_STATUS_ENTER)
        {
          OnMouseEnter.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }
      }
    }
    else
    {
      unsigned int mouse_signals;
      mouse_signals = m_EventHandler.Process (ievent, m_Geometry, true);

      if (HasMouseFocus () && (GetWindowCompositor().GetMouseFocusArea() == 0)) // never evince and object that has the mouse focus.
      {
        GetWindowCompositor ().SetMouseFocusArea (this);
        GetWindowCompositor ().SetAreaEventRoot (ievent.e_x_root, ievent.e_y_root);
      }

      if (ievent.e_event == NUX_WINDOW_EXIT_FOCUS)
      {
        if (HasKeyboardFocus ())
        {
          SetKeyboardFocus (false);
          OnEndFocus.emit ();
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
            (CurrentMouseOverArea)->OnMouseLeave.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
          }
          CurrentMouseOverArea = NULL;
          PreviousMouseOverArea = NULL;
        }
      }

      if (PreviousMouseOverArea != CurrentMouseOverArea)
      {
        if (PreviousMouseOverArea)
        {
          (PreviousMouseOverArea)->OnMouseLeave.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
          // When calling OnMouseLeave.emit on a widget, we have to set the following states to false
          (PreviousMouseOverArea)->m_EventHandler._current_mouse_in = false;
          (PreviousMouseOverArea)->m_EventHandler._previous_mouse_in = false;
          //nuxDebugMsg(TEXT("InputArea: Previous MouseOver Leave"));
        }

        if (CurrentMouseOverArea)
        {
          (CurrentMouseOverArea)->OnMouseEnter.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
          //nuxDebugMsg(TEXT("InputArea: Current MouseOver Enter"));
        }

        PreviousMouseOverArea = CurrentMouseOverArea;
      }

      if ((ievent.e_event == NUX_MOUSEWHEEL) && m_EventHandler.MouseIn ())
      {
        OnMouseWheel.emit (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root,
                           ievent.e_wheeldelta,
                           ievent.GetMouseState(), ievent.GetKeyState() );
      }

      if (ievent.e_event == NUX_MOUSE_PRESSED)
      {
        if (m_CaptureMouseDownAnyWhereElse)
        {
          if (m_EventHandler.MouseIn() == false)
          {
            // A mouse Down happened outside of this view.
            OnMouseDownOutsideArea.emit (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.GetMouseState(), ievent.GetKeyState() );
          }
        }
      }

      if ((mouse_signals == AREA_MOUSE_STATUS_NONE) && (ievent.e_event == NUX_MOUSE_PRESSED) )
      {
        if (HasKeyboardFocus() )
        {
          SetKeyboardFocus (false);
          OnEndFocus.emit();
        }
      }
      else if (mouse_signals & AREA_MOUSE_STATUS_DOWN) // The mouse is down inside the area
      {
        OnStartMouseFocus.emit();

        if (ievent.e_mouse_state & NUX_EVENT_BUTTON1_DBLCLICK)
        {
          // Double click
          OnMouseDoubleClick.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }
        else
        {
          OnMouseDown.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x,
                            m_EventHandler.m_mouse_positiony - m_Geometry.y,
                            ievent.GetMouseState(), ievent.GetKeyState() );
        }

        if (HasKeyboardFocus() == false)
        {
          // First mouse down
          SetKeyboardFocus (true);
          OnStartFocus.emit();
        }

        ret |=  eMouseEventSolved;
      }
      else if (mouse_signals & AREA_MOUSE_STATUS_UP) // The mouse is up. This goes to the area that had the mouse focus.
      {
        OnEndMouseFocus.emit ();

        // This is a mouse up on an area that has the mouse focus.
        // Just check that the mouse in. If the mouse is in, then it is a "Click".
        if (m_EventHandler.MouseIn())
        {
          OnMouseClick.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }

        OnMouseUp.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x,
                        m_EventHandler.m_mouse_positiony - m_Geometry.y,
                        ievent.GetMouseState(), ievent.GetKeyState() );

        ret |=  eMouseEventSolved;
      }
      else if ((mouse_signals & AREA_MOUSE_STATUS_MOVE) && m_EventHandler.HasMouseFocus())
      {
        OnMouseDrag.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y,
                          m_EventHandler.m_mouse_deltax, m_EventHandler.m_mouse_deltay,
                          ievent.GetMouseState(), ievent.GetKeyState() );
      }
      else if (mouse_signals & AREA_MOUSE_STATUS_MOVE && (!m_EventHandler.HasMouseFocus()))
      {
        OnMouseMove.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y,
                          m_EventHandler.m_mouse_deltax, m_EventHandler.m_mouse_deltay,
                          ievent.GetMouseState(), ievent.GetKeyState() );
      }
    }

    if (HasKeyboardFocus() && (ievent.e_event == NUX_KEYDOWN || ievent.e_event == NUX_KEYUP) )
    {
      OnKeyEvent.emit (
        GetWindowThread ()->GetGraphicsEngine(),
        ievent.e_event,
        ievent.GetKeySym(),
        ievent.GetKeyState(),
        ievent.GetChar(),
        ievent.GetKeyRepeatCount() );
    }

    GetWindowCompositor().m_PreviousMouseOverArea = PreviousMouseOverArea;
    GetWindowCompositor().m_MouseOverArea = CurrentMouseOverArea;

    return ret;
  }

  long InputArea::ProcessEventInExclusiveMode (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    unsigned int mouse_status;
    mouse_status = m_EventHandler.Process (ievent, m_Geometry, false);

    if (m_EventHandler.MouseIn () && (ievent.e_event == NUX_MOUSE_PRESSED))
    {
      OnMouseDown.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x,
        m_EventHandler.m_mouse_positiony - m_Geometry.y,
        ievent.GetMouseState(), ievent.GetKeyState() );
    }

    if (m_EventHandler.MouseIn () && (ievent.e_event == NUX_MOUSE_RELEASED))
    {
      OnMouseUp.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x,
        m_EventHandler.m_mouse_positiony - m_Geometry.y,
        ievent.GetMouseState(), ievent.GetKeyState() );
    }

    if (m_EventHandler.MouseIn () && (ievent.e_event == NUX_MOUSE_MOVE))
    {
      OnMouseMove.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y,
        m_EventHandler.m_mouse_deltax, m_EventHandler.m_mouse_deltay,
        ievent.GetMouseState(), ievent.GetKeyState() );
    }

    if (mouse_status & AREA_MOUSE_STATUS_LEAVE)
    {
      OnMouseLeave.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
    }

    if (mouse_status & AREA_MOUSE_STATUS_ENTER)
    {
      OnMouseEnter.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
    }

    return 0;
  }

  void InputArea::OnDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.QRP_Color (m_Geometry.x, m_Geometry.y, m_Geometry.GetWidth(), m_Geometry.GetHeight(), m_AreaColor);
  }

  void InputArea::SetBaseString (const TCHAR *Caption)
  {
    Area::SetBaseString (Caption);
  }

  bool InputArea::HasKeyboardFocus()
  {
    return m_hasKeyboardFocus;
  }

  void InputArea::SetKeyboardFocus (bool b)
  {
    m_hasKeyboardFocus = b;
  }

  int InputArea::GetMouseX()
  {
    return m_EventHandler.m_mouse_positionx - m_Geometry.x;
  }

  int InputArea::GetMouseY()
  {
    return m_EventHandler.m_mouse_positiony - m_Geometry.y;
  }

  bool InputArea::IsMouseInside()
  {
    return m_EventHandler.MouseIn();
  }

  bool InputArea::HasMouseFocus()
  {
    return m_EventHandler.HasMouseFocus();
  }

  bool InputArea::MouseFocusOnOtherArea()
  {
    return (GetWindowCompositor().GetMouseFocusArea() == 0);
  }

  void InputArea::CaptureMouseDownAnyWhereElse (bool b)
  {
    m_CaptureMouseDownAnyWhereElse = b;
  }

  bool InputArea::IsCaptureMouseDownAnyWhereElse() const
  {
    return m_CaptureMouseDownAnyWhereElse;
  }

  void InputArea::EnableDoubleClick (bool b)
  {
    m_EnableDoubleClick = b;
  }

  bool InputArea::IsDoubleClickEnabled()
  {
    return m_EnableDoubleClick;
  }

  void InputArea::EnableUserKeyboardProcessing (bool b)
  {
    m_EnableUserKeyboardProcessing = b;
  }

  bool InputArea::IsUserKeyboardProcessingEnabled()
  {
    return m_EnableUserKeyboardProcessing;
  }

  void InputArea::SetAreaMousePosition (int x, int y)
  {
    m_EventHandler.m_mouse_positionx = x;
    m_EventHandler.m_mouse_positiony = y;
  }

}
