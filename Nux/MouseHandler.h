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


#ifndef AREAEVENTPROCESSOR_H
#define AREAEVENTPROCESSOR_H

#include "NuxGraphics/Events.h"
#include "Utils.h"

namespace nux
{
  //! State of the mouse on an area.
  typedef enum
  {
    AREA_MOUSE_STATUS_NONE   = 0L,
    AREA_MOUSE_STATUS_DOWN   = (1L << 0),   //!< A mouse down event is detected on the area. That last only for the duration of this event cycle.
    AREA_MOUSE_STATUS_UP     = (1L << 1),   //!< A mouse up event is detected on the area. That last only for the duration of this event cycle.
    AREA_MOUSE_STATUS_FOCUS  = (1L << 2),   //!< The mouse is pressed on the area. This status last as long as the mouse is douse on the area.
    AREA_MOUSE_STATUS_MOVE   = (1L << 3),   //!< The mouse is moving over the area.
    AREA_MOUSE_STATUS_ENTER  = (1L << 4),   //!< The mouse entered the area.
    AREA_MOUSE_STATUS_LEAVE  = (1L << 5),   //!< The mouse exited the area.
  } AreaMouseStatus;

  class AreaEventProcessor
  {
  public:
    AreaEventProcessor();
    ~AreaEventProcessor();

    /*!
        Return the physical status of the mouse with regard to the area.

        @param event The event to process.
        @param geo The geometry of the area.
        @param process_mouse_focus This parameter should be true if it is allowed to change the mouse focus status.
          process_mouse_focus should be true only when the event has not been solved yet.
        @return A value of type AreaMouseStatus, reflecting the position of the mouse relatively to the area.
    */
    unsigned int EventProcessor (Event &event, const Geometry &g, bool process_mouse_focus);

    bool MouseIn();

    void ForceMouseFocus (int x, int y, const Geometry &g);
    void StopMouseFocus (int x, int y, const Geometry &g);

  private:

    //! Reset the state machine to its initial configuration.
    void ResetState ();

    bool _has_mouse_focus;
    bool _previous_mouse_in;
    bool _current_mouse_in;
    unsigned int _state;

    //! Last know mouse X coordinate inside the area.
    int _mouse_positionx;
    //! Last know mouse Y coordinate inside the area.
    int _mouse_positiony;
    //! Mouse DeltaX coordinate inside the area.
    int _mouse_deltax;
    //! Mouse DeltaY coordinate inside the area.
    int _mouse_deltay;

    bool _initial_state;

    friend class InputArea;
  };
}

#endif // AREAEVENTPROCESSOR_H
