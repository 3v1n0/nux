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
  BaseMouseHandler *gFocusMouseHandler = 0; // put this is the GfxServer class

// static void SetMouseFocusOwner(BaseMouseHandler* ptr)
// {
//     gFocusMouseHandler = ptr;
//     if(ptr)
//         gMouseOwned = true;
//     else
//         gMouseOwned = false;
// }
//
// static BaseMouseHandler* GetMouseFocusOwner()
// {
//     return gFocusMouseHandler;
// }

  BaseMouseHandler::BaseMouseHandler()
  {
    m_first_time        = true;
    _has_mouse_focus    = false;
    _current_mouse_in   = false;
    _previous_mouse_in  = false;
    _geometric_mouse_status = 0;
  }

  BaseMouseHandler::~BaseMouseHandler()
  {

  }

  unsigned int BaseMouseHandler::Process (IEvent &ievent, const Geometry &geo, bool process_mouse_focus)
  {
    _geometric_mouse_status = AREA_MOUSE_STATUS_NONE;

    if (ievent.e_event == NUX_NO_EVENT)
      return _geometric_mouse_status;

    _previous_mouse_in = _current_mouse_in;

    int x, y, lo_x, hi_x, lo_y, hi_y;
    // Usually (e_x_root, e_y_root) is equal to (0, 0). In that case, (x, y) is the mouse coordinate
    // that refers to the top-left corner of the window.
    // If (e_x_root, e_y_root) is equal to the top left corner of this area in the window,
    // then (x, y) represent the coordinate of the mouse based on the top-left corner of this area.
    x = ievent.e_x - ievent.e_x_root;
    y = ievent.e_y - ievent.e_y_root;

    lo_x = geo.x;
    hi_x = geo.x + geo.GetWidth() - 1;
    lo_y = geo.y;
    hi_y = geo.y + geo.GetHeight() - 1;

    if ((ievent.e_x == -1) && (ievent.e_y == -1))
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
        _geometric_mouse_status |= AREA_MOUSE_STATUS_LEAVE;
      }

      if ( (_previous_mouse_in == false) && (_current_mouse_in == true) )
      {
        _geometric_mouse_status |= AREA_MOUSE_STATUS_ENTER;
      }
    }

    m_mouse_deltax = x - m_mouse_positionx;
    m_mouse_deltay = y - m_mouse_positiony;
    m_mouse_positionx = x;
    m_mouse_positiony = y;

    if ((_current_mouse_in == false) && (HasMouseFocus () == false))
    {
      return _geometric_mouse_status;
    }

    if (process_mouse_focus == false)
    {
      return _geometric_mouse_status;
    }

    switch (ievent.e_event)
    {
      case NUX_MOUSE_PRESSED:
      {
        if (_current_mouse_in)
        {
          _geometric_mouse_status |= AREA_MOUSE_STATUS_DOWN;
          SetMouseFocus (true);
        }
      }
      break;

      case NUX_MOUSE_RELEASED:
      {
        if (HasMouseFocus ())
        {
          _geometric_mouse_status |= AREA_MOUSE_STATUS_UP;
          SetMouseFocus (false);
        }
      }
      break;

      case NUX_MOUSE_MOVE:
      {
        _geometric_mouse_status |= AREA_MOUSE_STATUS_MOVE;
      }
      break;
    }

    return _geometric_mouse_status;
  }

  void BaseMouseHandler::SetMouseFocus (bool focus)
  {
    if (focus)
    {
      _has_mouse_focus = true;
      //_current_mouse_in = true;
    }
    else
    {
      _has_mouse_focus = false;
//       // If the mouse is released do not change the MouseIn status.
//       if (!(_geometric_mouse_status & AREA_MOUSE_STATUS_UP))
//         _current_mouse_in = false;
//       else
//       {
//         int i = 4;
//       }
    }
  }

  bool BaseMouseHandler::ReleaseMouseFocus()
  {
    SetMouseFocus (false);
    return true;
  }

  bool BaseMouseHandler::HasMouseFocus()
  {
    return _has_mouse_focus;
  }

  bool BaseMouseHandler::MouseIn()
  {
    return _current_mouse_in;
  }

  void BaseMouseHandler::ForceMouseFocus (int x, int y, const Geometry &g)
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

    _has_mouse_focus = true;
  }

  void BaseMouseHandler::StopMouseFocus (int x, int y, const Geometry &g)
  {
    _has_mouse_focus = false;
  }
}
