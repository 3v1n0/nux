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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
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
    OnStartFocus.emit();
    m_EventHandler.ForceMouseFocus (x, y, m_Geometry);
  }

  void InputArea::ForceStopFocus (int x, int y)
  {
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
    InputArea *PreviousMouseOverArea = (GetWindowCompositor().m_PreviousMouseOverArea);
    InputArea *CurrentMouseOverArea = (GetWindowCompositor().m_MouseOverArea);

    if (ProcessEventInfo & eDoNotProcess)
    {
      if (ievent.e_event == NUX_MOUSE_PRESSED)
      {
        if (m_CaptureMouseDownAnyWhereElse)
        {
          OnMouseDownOutsideArea.emit (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.GetMouseState(), ievent.GetKeyState() );
        }

        if (HasKeyboardFocus() )
        {
          SetKeyboardFocus (false);
          OnEndFocus.emit();
        }

        return TraverseInfo;
      }

      // Even if it is eDoNotProcess, we still want to respond to mouse Enter/Leave events.
      {

        unsigned int mouse_signals;
        mouse_signals = m_EventHandler.ProcessMouseInOut (ievent, m_Geometry);

        if (mouse_signals & eSigMouseLeave)
        {
          OnMouseLeave.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }

        if (mouse_signals & eSigMouseEnter)
        {
          OnMouseEnter.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }
      }
      return TraverseInfo;
    }

    long ret = TraverseInfo;

    if (TraverseInfo & eMouseEventSolved)
    {
      m_MouseEventCurrent.MouseIn = false;
      m_EventHandler.SetMouseFocus (false);
      ret |=  eMouseEventSolved;


      if ( (CurrentMouseOverArea == 0) && (GetGeometry().IsPointInside (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root) ) )
      {
        CurrentMouseOverArea = this;
      }

      if (ievent.e_event == NUX_MOUSE_PRESSED)
      {
        if (HasKeyboardFocus() )
        {
          SetKeyboardFocus (false);
          OnEndFocus.emit();
        }
      }

      if (ievent.e_event == NUX_MOUSE_PRESSED)
      {
        if (m_CaptureMouseDownAnyWhereElse)
        {
          OnMouseDownOutsideArea.emit (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.GetMouseState(), ievent.GetKeyState() );
        }
      }

      // Even if it is eMouseEventSolved, we still want to respond to mouse Enter/Leave events.
      {
        unsigned int mouse_signals;
        mouse_signals = m_EventHandler.ProcessMouseInOut (ievent, m_Geometry);

        if (mouse_signals & eSigMouseLeave)
        {
          OnMouseLeave.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }

        if (mouse_signals & eSigMouseEnter)
        {
          OnMouseEnter.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
        }
      }
    }
    else
    {
      unsigned int mouse_signals;
      mouse_signals = m_EventHandler.Process (ievent, m_Geometry);

      if (HasMouseFocus() && (GetWindowCompositor().GetMouseFocusArea() == 0) ) // never evince and object that has the mouse focus.
      {
        GetWindowCompositor().SetMouseFocusArea (this);
        GetWindowCompositor().SetAreaEventRoot (ievent.e_x_root, ievent.e_y_root);
      }

      if (IsMouseInside() )
      {
        if (CurrentMouseOverArea != this)
        {
          PreviousMouseOverArea = CurrentMouseOverArea;
//                 if(*CurrentMouseOverArea)
//                     nuxDebugMsg(TEXT("InputArea: Mouse is inside; Setting PreviousMouseOverArea."));
//                 else
//                     nuxDebugMsg(TEXT("InputArea: Mouse is inside; Setting PreviousMouseOverArea to 0."));
        }

        CurrentMouseOverArea = this;
      }
      else
      {
        // The mouse is outside this widget. If the PreviousMouseOverArea == this, then call OnMouseLeave.
        if (PreviousMouseOverArea == this)
        {
          // We should also have CurrentMouseOverArea == this.
          if (CurrentMouseOverArea)
          {
            (CurrentMouseOverArea)->OnMouseLeave.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
            //nuxDebugMsg(TEXT("InputArea: Current MouseOver Leave"));
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
          (PreviousMouseOverArea)->m_EventHandler.m_CurrentMouseIn = false;
          (PreviousMouseOverArea)->m_EventHandler.m_PreviousMouseIn = false;
          //nuxDebugMsg(TEXT("InputArea: Previous MouseOver Leave"));
        }

        if (CurrentMouseOverArea)
        {
          (CurrentMouseOverArea)->OnMouseEnter.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
          //nuxDebugMsg(TEXT("InputArea: Current MouseOver Enter"));
        }

        PreviousMouseOverArea = CurrentMouseOverArea;
      }

      if (ievent.e_event == NUX_WINDOW_EXIT_FOCUS)
      {
        if (HasKeyboardFocus() )
        {
          SetKeyboardFocus (false);
          OnEndFocus.emit();
        }
      }

      if (ievent.e_event == NUX_MOUSEWHEEL)
      {
        // Mouse wheel should not be conditioned to keyboard focus.
        //if (HasKeyboardFocus() )
        {
          OnMouseWheel.emit (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root,
                             ievent.e_wheeldelta,
                             ievent.GetMouseState(), ievent.GetKeyState() );
        }
      }

      // Imagine a Toolbar. When a MouseDown happens on the toolbar, it gets the mouse focus. While the mouse
      // is being pressed, we can move it above a widget that is inside the toolbar. That widget should not claim that it has
      // resolved the mouse event because the mouse is still being owned by the Toolbar. At most, the widget
      // should be set in CurrentMouseOverArea.
      if (m_EventHandler.MouseIn() && (GetWindowCompositor().GetMouseFocusArea() == 0) )
      {
        ret |=  eMouseEventSolved;
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

      if ( (mouse_signals == eSigMouseNone) && (ievent.e_event == NUX_MOUSE_PRESSED) )
      {
        if (HasKeyboardFocus() )
        {
          SetKeyboardFocus (false);
          OnEndFocus.emit();
        }
      }
      else if ( (mouse_signals & eSigMouseDown) /*&& (ievent.e_event == NUX_MOUSE_PRESSED)*/)
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

      if (mouse_signals & eSigMouseDown)
      {
//            ret |=  eMouseEventSolved;
      }

      if ( (mouse_signals & eSigMouseUp) )
      {
        OnEndMouseFocus.emit();

        // This is a mouse up on an area that has the mouse focus.
        // Just check that the mouse in. If the mouse is in, then it is a "Click".
        if (m_EventHandler.MouseIn() )
        {
          OnMouseClick.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.GetMouseState(), ievent.GetKeyState() );
          ret |=  eMouseEventSolved;
        }

        OnMouseUp.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x,
                        m_EventHandler.m_mouse_positiony - m_Geometry.y,
                        ievent.GetMouseState(), ievent.GetKeyState() );
      }

      if ( (mouse_signals & eSigMouseMove) && m_EventHandler.HasMouseFocus() )
      {
        OnMouseDrag.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y,
                          m_EventHandler.m_mouse_deltax, m_EventHandler.m_mouse_deltay,
                          ievent.GetMouseState(), ievent.GetKeyState() );
      }

      if (mouse_signals & eSigMouseMove && (!m_EventHandler.HasMouseFocus() ) )
      {
        OnMouseMove.emit (m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y,
                          m_EventHandler.m_mouse_deltax, m_EventHandler.m_mouse_deltay,
                          ievent.GetMouseState(), ievent.GetKeyState() );
      }
    }

    if (HasKeyboardFocus() && (ievent.e_event == NUX_KEYDOWN || ievent.e_event == NUX_KEYUP) )
    {
      OnKeyEvent.emit (
        GetGraphicsThread()->GetGraphicsEngine(),
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

  void InputArea::OnDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.QRP_GLSL_Color (m_Geometry.x, m_Geometry.y, m_Geometry.GetWidth(), m_Geometry.GetHeight(), m_AreaColor);
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
