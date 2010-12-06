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


#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "NuxGraphics/Events.h"
#include "Utils.h"

namespace nux
{
  typedef enum
  {
    AREA_MOUSE_STATUS_NONE   = 0L,
    AREA_MOUSE_STATUS_DOWN   = (1L << 0),
    AREA_MOUSE_STATUS_UP     = (1L << 1),
    AREA_MOUSE_STATUS_MOVE   = (1L << 2),
    AREA_MOUSE_STATUS_ENTER  = (1L << 3),
    AREA_MOUSE_STATUS_LEAVE  = (1L << 4),
  } AreaMouseStatus;

  class BaseMouseHandler
  {
  public:
    BaseMouseHandler();
    ~BaseMouseHandler();

    /*!
        Return the physical status of the mouse with regard to the area.

        @param ievent The event to process.
        @param geo The geometry of the area.
        @param process_mouse_focus This parameter should be true if it is allowed to change the mouse focus status.
          process_mouse_focus should be true only when the event has not been solved yet.
        @return A value of type AreaMouseStatus, reflecting the position of the mouse relatively to the area.
    */
    unsigned int Process (IEvent &ievent, const Geometry &g, bool process_mouse_focus);
    void SetMouseFocus (bool b);

    // HasMouseFous == true mean the mouse (left button) was pressed on an area at a previous moment,
    // and now the mouse maybe inside or outside of that area while the button(left mouse button)
    // who initiated the "button down" is still pressed.
    bool HasMouseFocus();
    bool MouseIn();
    bool ReleaseMouseFocus();

    void ForceMouseFocus (int x, int y, const Geometry &g);
    void StopMouseFocus (int x, int y, const Geometry &g);

  private:
    bool _has_mouse_focus;
    bool _previous_mouse_in;
    bool _current_mouse_in;
    unsigned int _geometric_mouse_status;

    //! Last know mouse X coordinate inside the area.
    int m_mouse_positionx;
    //! Last know mouse Y coordinate inside the area.
    int m_mouse_positiony;
    //! Mouse DeltaX coordinate inside the area.
    int m_mouse_deltax;
    //! Mouse DeltaY coordinate inside the area.
    int m_mouse_deltay;

    bool m_first_time;

    friend class InputArea;
  };
}

#endif // EVENTHANDLER_H
