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
#include "NuxCore/Logger.h"

namespace nux
{

DECLARE_LOGGER(logger, "nux.inputarea_proximity");

InputAreaProximity::InputAreaProximity(InputArea* area, unsigned int proximity)
  : area_(area)
  , proximity_(proximity)
  , is_mouse_near_(false)
{
  if(area)
    GetWindowThread()->GetWindowCompositor().AddAreaInProximityList(this);
  else
    LOG_ERROR(logger) << "Error, passing in NULL value for InputArea*";
}

InputAreaProximity::~InputAreaProximity()
{
  GetWindowThread()->GetWindowCompositor().RemoveAreaInProximityList(this);
}

Point GetOffsetFromRect(Rect const& rect, Point const& mouse)
{
  Point offset;

  if (rect.x > mouse.x)
  {
    offset.x = rect.x - mouse.x;
  }
  else if (rect.x + rect.width < mouse.x)
  {
    offset.x = rect.x + rect.width - mouse.x;
  }

  if (rect.y > mouse.y)
  {
    offset.y = rect.y - mouse.y;
  }
  else if (rect.y + rect.height < mouse.y)
  {
    offset.y = rect.y + rect.height - mouse.y;
  }

  return offset;
}

void InputAreaProximity::CheckMousePosition(Point const& mouse)
{
  if (!area_.IsValid())
    return;

  Geometry const& geo = area_->GetAbsoluteGeometry();
  Geometry const& expanded = geo.GetExpand(proximity_, proximity_);

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
    nux::Point const& offset = GetOffsetFromRect(geo, mouse);
    mouse_approaching.emit(mouse, offset);
  }
}

}

