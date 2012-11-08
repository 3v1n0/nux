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

#include "WindowCompositor.h"
#include "WindowThread.h"
#include "ProximityArea.h"

namespace nux
{

ProximityArea::ProximityArea(InputArea* area, int proximity)
  : area_(area)
  , proximity_(proximity)
{
  GetWindowThread()->GetWindowCompositor().AddAreaInProximityList(this);
}

ProximityArea::~ProximityArea()
{
  GetWindowThread()->GetWindowCompositor().RemoveAreaInProximityList(this);
}

void ProximityArea::CheckMousePosition(Point mouse_pos)
{
  Geometry geo = area_->GetGeometry();
  geo.Expand(proximity_, proximity_);

  if (!mouse_near_ && geo.IsInside(mouse_pos))
  {
    mouse_near_ = true;
    mouse_near.emit(mouse_pos);
  }
  else if (mouse_near_ && !geo.IsInside(mouse_pos))
  {
    mouse_near_ = false;
    mouse_beyond.emit(mouse_pos);
  }
}

}

