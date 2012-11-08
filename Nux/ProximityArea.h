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
 * Authored by: Brandon Schaefer <brandon.schaefer@canonical.com>
 *
 */


#ifndef PROXIMITYAREA_H
#define PROXIMITYAREA_H

#include "InputArea.h"

namespace nux
{

class ProximityArea
{
public:
  ProximityArea(InputArea* area, int proximity);
  ~ProximityArea();

  virtual void CheckMousePosition(int x, int y);

  sigc::signal<void, int, int> mouse_near;
  sigc::signal<void, int, int> mouse_beyond;
protected:
  InputArea* area_;
  int proximity_;

  bool mouse_near_;
};


}

#endif
