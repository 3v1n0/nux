/*
 * Copyright 2012 Inalogic® Inc.
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
 * Authored by: Brandon Schaefer <brandon.schaefer@canonical.com>
 *
 */


#ifndef NUX_PROXIMITYAREA_H
#define NUX_PROXIMITYAREA_H

#include "InputArea.h"

namespace nux
{

class InputAreaProximity
{
public:
  InputAreaProximity(InputArea* area, int proximity);
  virtual ~InputAreaProximity();

  virtual void CheckMousePosition(const Point& mouse);

  //! Signal emitted when the Mouse is near the input area.
  /*!
    @param Point mouse is the current Mouse position.
  */
  sigc::signal<void, const Point> mouse_near;

  //! Signal emitted while the mouse is moving, near, and not yet inside the input area.
  /*!
    @param Point mouse is the current Mouse position.
    @param Point difference is the distances between the mouse and the input area.
  */
  sigc::signal<void, const Point, const Point> mouse_approaching;

  //! Signal emitted when the Mouse is moves beyond the input area + proximity.
  /*!
    @param Point mouse is the current Mouse position.
  */
  sigc::signal<void, const Point> mouse_beyond;
protected:
  ObjectWeakPtr<InputArea> area_;
  int proximity_;

  bool is_mouse_near_;
};


}

#endif
