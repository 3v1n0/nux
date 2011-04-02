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


#ifndef RECT_H
#define RECT_H

#include "Point.h"
#include "Size.h"

namespace nux
{

  class Rect
  {
  public:
    Rect();
    Rect (t_int32 x_, t_int32 y_, t_int32 width_, t_int32 height_);
    ~Rect();
    Rect (const Rect &r);

    Rect &operator = (const Rect &r);
    t_bool operator == (const Rect &p) const;
    t_bool operator != (const Rect &p) const;

    t_bool IsNull() const
    {
      return ( (width == 0) || (height == 0) );
    }
    t_bool IsInside (const Point &p) const;
    Rect Intersect (const Rect &) const;

    // expand the width by factor_x and the height by factor_y
    void Expand (t_int32 factor_x, t_int32 factor_y);

    t_int32 GetWidth() const
    {
      return width;
    }
    t_int32 GetHeight() const
    {
      return height;
    }
    //! Returns a Point at the center of the Rectangle.
    Point   GetCenter() const
    {
      return Point (x + width / 2, y + height / 2);
    }
    //! Returns a Point at the center of the Rectangle.
    Point GetPosition()
    {
      return Point (x, y);
    }

    void SetWidth (t_int32 w)
    {
      width = w;
    }
    void SetHeight (t_int32 h)
    {
      height = h;
    }
    void SetX (t_int32 px)
    {
      x = px;
    }
    void SetY (t_int32 py)
    {
      y = py;
    }

    void Set (t_int32 px, t_int32 py, t_int32 w, t_int32 h);
    void SetPosition (t_int32 px, t_int32 py);
    void SetSize (t_int32 px, t_int32 py);

    void OffsetSize (t_int32 dw, t_int32 dh)
    {
      width += dw;
      height += dh;

      if (width < 0)
        width = 0;

      if (height < 0)
        height = 0;
    }
    void OffsetPosition (t_int32 dx, t_int32 dy)
    {
      x += dx;
      y += dy;
    }

    t_bool IsPointInside (t_int32 dx, t_int32 dy) const;
    Rect GetExpand (t_int32 dx, t_int32 dy) const;

  public:
    t_int32 x, y;
    t_int32 width, height;
  };

}

#endif // RECT_H

