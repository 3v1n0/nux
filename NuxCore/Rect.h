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
    Rect(int x_, int y_, int width_, int height_);
    Rect(const Rect &);
    Rect(Rect&&);

    Rect &operator = (Rect r);
    bool operator == (const Rect &p) const;
    bool operator != (const Rect &p) const;

    //! Return true is the area of this rectangle is Null.
    /*!
        Return true is the area of this rectangle is Null.
    */
    bool IsNull() const;

    bool IsInside(const Point &p) const;
    bool IsInside(const Point2D<float> &p) const;
    bool IsIntersecting(const Rect&) const;
    Rect Intersect(const Rect &) const;

    // expand the width by factor_x and the height by factor_y
    void Expand (int factor_x, int factor_y);

    int GetWidth() const
    {
      return width;
    }
    int GetHeight() const
    {
      return height;
    }
    //! Returns a Point at the center of the Rectangle.
    Point   GetCenter() const
    {
      return Point (x + width / 2, y + height / 2);
    }
    //! Returns a Point at the center of the Rectangle.
    Point GetPosition() const
    {
      return Point (x, y);
    }

    void SetWidth (int w)
    {
      width = w;
    }
    void SetHeight (int h)
    {
      height = h;
    }
    void SetX (int px)
    {
      x = px;
    }
    void SetY (int py)
    {
      y = py;
    }

    void Set (int px, int py, int w, int h);
    void SetPosition (int px, int py);
    void SetSize (int px, int py);

    void OffsetSize (int dw, int dh)
    {
      width += dw;
      height += dh;

      if (width < 0)
        width = 0;

      if (height < 0)
        height = 0;
    }

    void OffsetPosition (int dx, int dy)
    {
      x += dx;
      y += dy;
    }

    bool IsPointInside (int dx, int dy) const;
    Rect GetExpand (int dx, int dy) const;

    friend void swap(Rect& lhs, Rect& rhs)
    {
      using std::swap;
      swap(lhs.x, rhs.x);
      swap(lhs.y, rhs.y);
      swap(lhs.width, rhs.width);
      swap(lhs.height, rhs.height);
    }

    int x, y;
    int width, height;
  };

  Rect operator+(Rect const& lhs, Rect const& rhs);
  Rect operator-(Rect const& lhs, Rect const& rhs);
  Rect operator*(Rect const& lhs, float scalar);

  std::ostream& operator<<(std::ostream &os, Rect const&);

  typedef Rect Geometry;
}

#endif // RECT_H

