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


#include "NuxCore.h"
#include "Point.h"
#include "Size.h"
#include "Rect.h"

namespace nux
{
  Rect::Rect()
  {
    x = 0;
    y = 0;
    width = 0;
    height = 0;

  }

  Rect::Rect (t_int32 x_, t_int32 y_, t_int32 width_, t_int32 height_)
  {
    x = x_;
    y = y_;
    width = Max<int> (0, int (width_) );
    height = Max<int> (0, int (height_) );
  }

  Rect::~Rect()
  {

  }

  Rect::Rect (const Rect &r)
  {
    x = r.x;
    y = r.y;
    width = r.width;
    height = r.height;
  }

  Rect &Rect::operator = (const Rect &r)
  {
    if (&r == this)
      return *this;

    x = r.x;
    y = r.y;
    width = r.width;
    height = r.height;
    return *this;
  }

  t_bool Rect::operator == (const Rect &r) const
  {
    if ( (x == r.x) && (y == r.y) && (width == r.width) && (height == r.height) )
    {
      return true;
    }

    return false;
  }

  t_bool Rect::operator != (const Rect &r) const
  {
    if ( (x == r.x) && (y == r.y) && (width == r.width) && (height == r.height) )
    {
      return false;
    }

    return true;
  }

  void Rect::Set (t_int32 px, t_int32 py, t_int32 w, t_int32 h)
  {
    x = px;
    y = py;
    width = w;
    height = h;
  }

  void Rect::SetPosition (t_int32 px, t_int32 py)
  {
    x = px;
    y = py;
  }

  void Rect::SetSize (t_int32 w, t_int32 h)
  {
    width = w;
    height = h;
  }

  t_bool Rect::IsInside (const Point &p) const
  {
    return ( (x <= p.x) && (x + width > p.x) &&
             (y <= p.y) && (y + height > p.y) );
  }

  t_bool Rect::IsPointInside (int x_, int y_) const
  {
    return ( (x <= x_) && (x + width > x_) &&
             (y <= y_) && (y + height > y_) );
  }

  Rect Rect::Intersect (const Rect &r) const
  {
    // Get the corner points.
    const Point &ul1 = Point (x, y);
    const Point &ul2 = Point (r.x, r.y);
    int x = Max<int> (ul1.x, ul2.x);
    int y = Max<int> (ul1.y, ul2.y);
    int w = Min<int> (ul1.x + width,  ul2.x + r.width) - x;
    int h = Min<int> (ul1.y + height, ul2.y + r.height) - y;

    if (w < 0 || h < 0)
    {
      // No intersection
      return Rect ();
    }

    return Rect (x, y, w, h);
  }

  // expand the width by factor_x and the height by factor_y
  void Rect::Expand (t_int32 dx, t_int32 dy)
  {
    if (!IsNull() )
    {
      x -= dx;
      y -= dy;
      width  += 2 * dx;
      height += 2 * dy;
    }
  }

  // expand the width by factor_x and the height by factor_y
  Rect Rect::GetExpand (t_int32 dx, t_int32 dy) const
  {
    Rect r = Rect (x - dx, y - dy, width + 2 * dx, height + 2 * dy);

    if (r.IsNull() )
    {
      return Rect (0, 0, 0, 0);
    }

    return r;
  }
}

