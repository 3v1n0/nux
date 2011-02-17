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

#include "Nux.h"
#include "MouseHandler.h"

namespace nux
{
//extern bool gMouseOwned;
  AreaEventProcessor *gFocusMouseHandler = 0; // put this is the GfxServer class

// static void SetMouseFocusOwner(AreaEventProcessor* ptr)
// {
//     gFocusMouseHandler = ptr;
//     if(ptr)
//         gMouseOwned = true;
//     else
//         gMouseOwned = false;
// }
//
// static AreaEventProcessor* GetMouseFocusOwner()
// {
//     return gFocusMouseHandler;
// }

  AreaEventProcessor::AreaEventProcessor()
  {
    m_first_time        = true;
    _has_mouse_focus    = false;
    _current_mouse_in   = false;
    _previous_mouse_in  = false;
    _state = 0;
  }

  AreaEventProcessor::~AreaEventProcessor()
  {

  }

  unsigned int AreaEventProcessor::Process (Event &event, const Geometry &geo, bool process_mouse_focus)
  {
    // preserve mouse focus state.
    bool has_mouse_focus = _state & AREA_MOUSE_STATUS_FOCUS;

    _state = AREA_MOUSE_STATUS_NONE;

    if (has_mouse_focus)
      _state |= AREA_MOUSE_STATUS_FOCUS;

    if (event.e_event == NUX_NO_EVENT)
      return _state;

    _previous_mouse_in = _current_mouse_in;

    int x, y, lo_x, hi_x, lo_y, hi_y;
    // Usually (e_x_root, e_y_root) is equal to (0, 0). In that case, (x, y) is the mouse coordinate
    // that refers to the top-left corner of the window.
    // If (e_x_root, e_y_root) is equal to the top left corner of this area in the window,
    // then (x, y) represent the coordinate of the mouse based on the top-left corner of this area.
    x = event.e_x - event.e_x_root;
    y = event.e_y - event.e_y_root;

    lo_x = geo.x;
    hi_x = geo.x + geo.GetWidth() - 1;
    lo_y = geo.y;
    hi_y = geo.y + geo.GetHeight() - 1;

    if ((event.e_x == -1) && (event.e_y == -1))
    {
      // e_x == -1 and e_y == -1 are associated with some specific window events that have the mouse outside of any widget.
      // See WM_SETFOCUS, WM_KILLFOCUS, NUX_WINDOW_MOUSELEAVE
      _current_mouse_in = false;
    }
    else
    {
      _current_mouse_in = PT_IN_BOX (x, y, lo_x, hi_x, lo_y, hi_y);
    }


    if (m_first_time)
    {
      m_first_time = false;
    }
    else
    {
      if ( (_previous_mouse_in == true) && (_current_mouse_in == false) )
      {
        _state |= AREA_MOUSE_STATUS_LEAVE;
      }

      if ( (_previous_mouse_in == false) && (_current_mouse_in == true) )
      {
        _state |= AREA_MOUSE_STATUS_ENTER;
      }
    }

    m_mouse_deltax = x - m_mouse_positionx;
    m_mouse_deltay = y - m_mouse_positiony;
    m_mouse_positionx = x;
    m_mouse_positiony = y;

    if ((_current_mouse_in == false) && !(_state & AREA_MOUSE_STATUS_FOCUS))
    {
      return _state;
    }

    if (process_mouse_focus == false)
    {
      return _state;
    }

    switch (event.e_event)
    {
      case NUX_MOUSE_PRESSED:
      case NUX_MOUSE_DOUBLECLICK:
      {
        if (_current_mouse_in)
        {
          _state |= AREA_MOUSE_STATUS_DOWN;
          _state |= AREA_MOUSE_STATUS_FOCUS;
          //SetMouseFocus (true);
        }
      }
      break;

      case NUX_MOUSE_RELEASED:
      {
        if (_state & AREA_MOUSE_STATUS_FOCUS)
        {
          _state |= AREA_MOUSE_STATUS_UP;
          _state &= ~AREA_MOUSE_STATUS_FOCUS;
          //SetMouseFocus (false);
        }
      }
      break;

      case NUX_MOUSE_MOVE:
      {
        _state |= AREA_MOUSE_STATUS_MOVE;
      }
      break;
    }

    return _state;
  }

//   void AreaEventProcessor::SetMouseFocus (bool focus)
//   {
//     if (focus)
//     {
//       _has_mouse_focus = true;
//     }
//     else
//     {
//       _has_mouse_focus = false;
//     }
//   }

//   bool AreaEventProcessor::ReleaseMouseFocus()
//   {
//     SetMouseFocus (false);
//     return true;
//   }

//   bool AreaEventProcessor::HasMouseFocus()
//   {
//     return _has_mouse_focus;
//   }

  bool AreaEventProcessor::MouseIn()
  {
    return _current_mouse_in;
  }

  void AreaEventProcessor::ForceMouseFocus (int x, int y, const Geometry &g)
  {
    int lo_x, hi_x, lo_y, hi_y;
    lo_x = g.x;
    hi_x = g.x + g.GetWidth() - 1;
    lo_y = g.y;
    hi_y = g.y + g.GetHeight() - 1;

    bool isIn = PT_IN_BOX ( x, y, lo_x, hi_x, lo_y, hi_y );

    if (isIn)
    {
      _previous_mouse_in = _current_mouse_in = true;
    }
    else
    {
      _previous_mouse_in = _current_mouse_in = false;
    }

    _state |= AREA_MOUSE_STATUS_DOWN;
    _has_mouse_focus = true;
  }

  void AreaEventProcessor::StopMouseFocus (int x, int y, const Geometry &g)
  {
    _has_mouse_focus = false;
  }
}
