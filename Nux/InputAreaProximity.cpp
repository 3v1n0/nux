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
#include "InputAreaProximity.h"

namespace nux
{

InputAreaProximity::InputAreaProximity(InputArea* area, int proximity)
  : area_(area)
  , proximity_(proximity)
  , is_mouse_near_(false)
{
  GetWindowThread()->GetWindowCompositor().AddAreaInProximityList(this);
}

InputAreaProximity::~InputAreaProximity()
{
  GetWindowThread()->GetWindowCompositor().RemoveAreaInProximityList(this);
}

void InputAreaProximity::CheckMousePosition(const Point& mouse)
{
  if (!area_.IsValid())
    return;

  const Geometry& geo = area_->GetGeometry();
  const Geometry& expanded = geo.GetExpand(proximity_, proximity_);

  if (!is_mouse_near_ && expanded.IsInside(mouse))
  {
    is_mouse_near_ = true;
    mouse_near.emit(mouse);
  }
  else if (is_mouse_near_ && !expanded.IsInside(mouse))
  {
    is_mouse_near_ = false;
    mouse_beyond.emit(mouse);
  }

  if (is_mouse_near_ && !geo.IsInside(mouse))
  {
    const nux::Point& distance = geo.GetDistanceFromMouse(mouse.x, mouse.y);
    mouse_approaching.emit(mouse, distance);
  }
}

}

