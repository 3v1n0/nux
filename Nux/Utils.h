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


#ifndef UTILS_H
#define UTILS_H

// check if a value lies within a closed interval
#ifndef IN_BOUNDS
#define IN_BOUNDS( x, lo, hi ) ((x) >= (lo) && (x) <= (hi))
#endif

//check if a 2D point lies within a 2D box
#ifndef PT_IN_BOX
#define PT_IN_BOX( x, y, lo_x, hi_x, lo_y, hi_y ) ( IN_BOUNDS(x,lo_x,hi_x) && IN_BOUNDS(y,lo_y,hi_y))
#endif

namespace nux
{
  typedef enum
  {
    eSolveNone                      = (0L),
    eMouseEventSolved               = (1L),
    eKeyEventSolved                 = (1L) << 1,
    eDoNotProcess                   = (1L) << 2,
    EVENT_CYCLE_EXCLUSIVE           = (1L) << 3,  //!< Value of ProcessEventInfo when processing the event cycle of the input area that has the exclusivity.
    EVENT_CYCLE_EXCLUSIVE_CONTINUE  = (1L) << 4,  //!< Return value of the cycle of the exclusive input area. If it is returned, allow other areas to process the event.
    EVENT_CYCLE_SOLVED              = (1L) << 5,
    EVENT_CYCLE_DO_NOT_PROCESS      = (1L) << 6,
    EVENT_CYCLE_RESET_AREA_STATE    = (1L) << 7,  //!< Reset the mouse event processor of an InputArea.
  } EventCycleStatus;

  enum PositionPolicy
  {
    ePositionLeft,
    ePositionRight,
    ePositionCenter,
    ePositionFull,
    ePositionProportional,
  };

  enum SizePolicy
  {
    eSizeFixed,
    eSizeResizeable,
  };

  typedef enum /*RectangleShapeCorners*/
  {
    eCornerNone         = 0,
    eCornerTopLeft      = (1L),
    eCornerTopRight     = (1L << 1),
    eCornerBottomLeft   = (1L << 2),
    eCornerBottomRight  = (1L << 3),
    eAllCorners         = eCornerTopLeft | eCornerTopRight | eCornerBottomLeft | eCornerBottomRight,
  } ShapeCornerType;

  enum TextureAlignment
  {
    eTACenter,
    eTALeft,
    eTARight,
    eTATop,
    eTABottom
  };

  enum VerticalAlignment
  {
    eVACenter,
    eVATop,
    eVABottom
  };

  enum HorizontalAlignment
  {
    eHACenter,
    eHALeft,
    eHARight,
  };

  typedef struct
  {
    int x;
    int y;
    int width;
    int height;

    int x_clipregion;
    int y_clipregion;
    int width_clipregion;
    int height_clipregion;

  } DrawAreaContext;


  bool IsMouseOwned();

}

#endif // UTILS_H
