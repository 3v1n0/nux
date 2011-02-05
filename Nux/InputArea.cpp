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
   
    if (GetWindowCompositor().GetMouseFocusArea () == this)
    {
      // already has the focus
      return;
    }

    // emit OnEndFocus on the area that "had" the focus
    if (GetWindowCompositor().GetMouseFocusArea ())
    {
      GetWindowCompositor().GetMouseFocusArea ()->OnEndFocus ();
    }

    OnStartFocus.emit();
    m_EventHandler.ForceMouseFocus (x, y, m_Geometry);
  }

  void InputArea::ForceStopFocus (int x, int y)
  {
    if (GetWindowCompositor().InExclusiveInputMode ())
    {
      return;
    }

    if (GetWindowCompositor().GetMouseFocusArea () != this)
    {
      GetWindowCompositor().SetMouseFocusArea (0);
      OnEndFocus.emit();
      m_EventHandler.StopMouseFocus (x, y, m_Geometry);
    }

    /*OnEndFocus.emit();
    m_EventHandler.StopMouseFocus (x, y, m_Geometry);

    if (GetWindowCompositor().GetMouseFocusArea () == this)
    {
      GetWindowCompositor().SetMouseFocusArea (0);
    }*/
  }

  long InputArea::OnEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    // Mouse event processing.
    if ((ievent.e_event >= NUX_DND_MOVE) && (ievent.e_event <= NUX_DND_LEAVE_WINDOW))
    {
      if (TraverseInfo & eMouseEventSolved) // It is not mouse event but let use this enum for now.
      {
        return eMouseEventSolved;
      }

      // We are in the range of DND events

      if (ievent.e_event == NUX_DND_MOVE)
      {
        InputArea *current_dnd_area = GetWindowCompositor().GetDnDArea();
        if (GetGeometry().IsPointInside (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root))
        {
          if (current_dnd_area != this)
          {
            // We just entered this area.
            
            // Exit the current dnd area if any.
            if (current_dnd_area != 0)
            {
              // There is a current dnd area.
              current_dnd_area->ProcessDnDLeave();
            }
            
            GetWindowCompositor().SetDnDArea(this);
            ProcessDnDEnter();
            ProcessDnDMove();

            return eMouseEventSolved;
          }
          else
          {
            ProcessDnDMove();
            return eMouseEventSolved;
          }
        }
        else
        {
          if (GetWindowCompositor().GetDnDArea() == this)
          {
            // we are going out of this area
            GetWindowCompositor().SetDnDArea(NULL);
            ProcessDnDLeave();
          }
        }
      }

      if (ievent.e_event == NUX_DND_DROP)
      {
        InputArea *current_dnd_area = GetWindowCompositor().GetDnDArea();
        if ((current_dnd_area == this) && GetGeometry().IsPointInside (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root))
        {
          ProcessDnDDrop();
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
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed event."));
        if (HasKeyboardFocus() )
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Area has keyboard focus. Disable keyboard focus on area. Emit OnEndFocus."));
          SetKeyboardFocus (false);
          OnEndFocus.emit();
        }

        if (m_CaptureMouseDownAnyWhereElse && (!GetGeometry().IsPointInside (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root)))
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse down outside of area. Emit OnMouseDownOutsideArea."));
          OnMouseDownOutsideArea.emit (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.GetMouseState(), ievent.GetKeyState() );
        }
      }

      // Even if it is eMouseEventSolved, we still want to respond to mouse Enter/Leave events.
      {
        unsigned int mouse_signals;
        mouse_signals = m_EventHandler.Process (ievent, m_Geometry, false);

        if (mouse_signals & AREA_MOUSE_STATUS_LEAVE)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse leave. Emit OnMouseLeave."));
          OnMouseLeave.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }

        if (mouse_signals & AREA_MOUSE_STATUS_ENTER)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse enter. Emit OnMouseEnter."));
          OnMouseEnter.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }
      }
    }
    else
    {
      nuxEventDebugTrace (_print_event_debug_trace, TEXT("Event has not been solved."));
      unsigned int mouse_signals;
      mouse_signals = m_EventHandler.Process (ievent, m_Geometry, true);

      if (HasMouseFocus () && (GetWindowCompositor().GetMouseFocusArea() == 0)) // never override an object that has the mouse focus.
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("getting the mouse focus."));
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
      else if (ievent.e_event == NUX_WINDOW_ENTER_FOCUS)
      {
        if (!HasKeyboardFocus ())
        {
          SetKeyboardFocus (true);
          OnStartFocus.emit ();
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
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed."));
        if (m_CaptureMouseDownAnyWhereElse)
        {
          if (m_EventHandler.MouseIn() == false)
          {
            nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed outside of area. Emit OnMouseDownOutsideArea."));
            // A mouse Down happened outside of this view.
            OnMouseDownOutsideArea.emit (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.GetMouseState(), ievent.GetKeyState() );
          }
        }
      }

      if ((mouse_signals == AREA_MOUSE_STATUS_NONE) && (ievent.e_event == NUX_MOUSE_PRESSED) )
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed and outside the area."));
        if (HasKeyboardFocus() )
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Has the foucus but mouse pressed and outside the area. Losing focus: SetKeyboardFocus (false). Emit OnEndFocus."));
          SetKeyboardFocus (false);
          OnEndFocus.emit();
        }
      }
      else if (mouse_signals & AREA_MOUSE_STATUS_DOWN) // The mouse is down inside the area
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed inside the area. Emit OnStartMouseFocus"));
        OnStartMouseFocus.emit();

        if (ievent.e_mouse_state & NUX_EVENT_BUTTON1_DBLCLICK)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed inside the area. Emit OnMouseDoubleClick"));
          // Double click
          OnMouseDoubleClick.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }
        else
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed inside the area. Emit OnMouseDown"));
          OnMouseDown.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x,
                            m_EventHandler.m_mouse_positiony - m_Geometry.y,
                            ievent.GetMouseState(), ievent.GetKeyState() );
        }

        if (HasKeyboardFocus() == false)
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse pressed inside the area. SetKeyboardFocus (true). Emit OnStartFocus"));
          // First mouse down
          SetKeyboardFocus (true);
          OnStartFocus.emit();
        }

        ret |=  eMouseEventSolved;
      }
      else if (mouse_signals & AREA_MOUSE_STATUS_UP) // The mouse is up. This goes to the area that had the mouse focus.
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse up. Emit OnEndMouseFocus"));
        OnEndMouseFocus.emit ();

        // This is a mouse up on an area that has the mouse focus.
        // Just check that the mouse in. If the mouse is in, then it is a "Click".
        if (m_EventHandler.MouseIn())
        {
          nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse up and inside area. Emit OnMouseClick"));
          OnMouseClick.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }

        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse up. Emit OnMouseUp"));
        OnMouseUp.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x,
                        m_EventHandler.m_mouse_positiony - m_Geometry.y,
                        ievent.GetMouseState(), ievent.GetKeyState() );

        ret |=  eMouseEventSolved;
      }
      else if ((mouse_signals & AREA_MOUSE_STATUS_MOVE) && m_EventHandler.HasMouseFocus())
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse move and has mouse focus. Emit OnMouseDrag"));
        OnMouseDrag.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y,
                          m_EventHandler.m_mouse_deltax, m_EventHandler.m_mouse_deltay,
                          ievent.GetMouseState(), ievent.GetKeyState() );
      }
      else if (mouse_signals & AREA_MOUSE_STATUS_MOVE && (!m_EventHandler.HasMouseFocus()))
      {
        nuxEventDebugTrace (_print_event_debug_trace, TEXT("Mouse move and no mouse focus. Emit OnMouseMove"));
        OnMouseMove.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y,
                          m_EventHandler.m_mouse_deltax, m_EventHandler.m_mouse_deltay,
                          ievent.GetMouseState(), ievent.GetKeyState() );
      }
    }

    if (HasKeyboardFocus ())
    {
      if (ievent.e_event == NUX_KEYDOWN)
      {
        nuxEventDebugTrace (_print_event_debug_trace,
                            TEXT("Emit OnKeyPressed"));
        OnKeyPressed.emit (ievent.GetKeySym(), ievent.e_x11_keycode,
                            ievent.GetKeyState());

        nuxEventDebugTrace (_print_event_debug_trace,
                            TEXT("Emit OnKeyEvent. String: %s"), ievent.e_text);
        OnKeyEvent.emit (GetWindowThread ()->GetGraphicsEngine(),
                         ievent.e_event, ievent.GetKeySym(),
                         ievent.GetKeyState(), ievent.GetText(),
                         ievent.GetKeyRepeatCount());
      }
      else if (ievent.e_event == NUX_KEYUP)
      {
        nuxEventDebugTrace (_print_event_debug_trace,
                            TEXT("Emit OnKeyReleased"));
        OnKeyReleased.emit (ievent.GetKeySym(), ievent.e_x11_keycode,
                            ievent.GetKeyState());

        nuxEventDebugTrace (_print_event_debug_trace,
                            TEXT("Emit OnKeyEvent. String: %s"), ievent.e_text);
        OnKeyEvent.emit (GetWindowThread ()->GetGraphicsEngine(),
                         ievent.e_event, ievent.GetKeySym(),
                         ievent.GetKeyState(), ievent.GetText(),
                         ievent.GetKeyRepeatCount());
      }
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
  
  void InputArea::EnableEventDebugTrace (bool enable)
  {
    _print_event_debug_trace = enable;
  }
  
  bool InputArea::GetEventDebugTrace () const
  {
    return _print_event_debug_trace;
  }

  void InputArea::ProcessDnDMove ()
  {
    // must learn to deal with x/y offsets
    GetWindow ().SendDndStatus (false, DNDACTION_NONE, Rect (GetGeometry ().x, GetGeometry ().y, 1, 1));
  }
  
  void InputArea::ProcessDnDDrop ()
  {
    GetWindow ().SendDndFinished (false, DNDACTION_NONE);
  }

  void InputArea::ProcessDnDEnter ()
  {
  }

  void InputArea::ProcessDnDLeave ()
  {
  }
}

