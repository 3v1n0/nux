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


#ifndef LINE2D_H
#define LINE2D_H

#include "Vector2.h"

namespace nux
{

  template<typename T>
  class Line2D
  {
  public:
    Line2D();
    ~Line2D();
    Line2D (const Line2D &line);
    Line2D (T lx_start, T ly_start, T lz_start,
            T lx_end, T ly_end, T lz_end);
    Line2D (const Vec2<T>& pt, Vec2<T> v);

    const Line2D<T>& operator = (const Line2D<T>&);
    bool operator == (const Line2D<T>& line) const;

    float Length() const;
    const Vec2<T> GetDirectionVector() const;
    const Vec2<T> GetStartPoint() const;
    const Vec2<T> GetEndPoint() const;

  private:
    T x_start, y_start;
    T x_end, y_end;

  };

  template<typename T>
  Line2D<T>::Line2D()
  {
    x_start = y_start = x_end = y_end = 0;
  }

  template<typename T>
  Line2D<T>::~Line2D()
  {

  }

  template<typename T>
  Line2D<T>::Line2D (const Line2D &line)
  {
    x_start = line.x_start;
    x_end   = line.x_end;
    y_start = line.y_start;
    y_end   = line.y_end;
  }

  template<typename T>
  Line2D<T>::Line2D (T lx_start, T ly_start, T lz_start,
                     T lx_end, T ly_end, T lz_end)
  {
    x_start = lx_start;
    x_end   = lx_end;
    y_start = ly_start;
    y_end   = ly_end;
  }

  template<typename T>
  Line2D<T>::Line2D (const Vec2<T>& pt, Vec2<T> v)
  {
    x_start = pt.x;
    y_start = pt.y;

    x_end   = x_start + v.x;
    y_end   = y_start + v.y;
  }

  template<typename T>
  const Line2D<T>& Line2D<T>::operator = (const Line2D<T>& Line)
  {
    x_start = Line.x_start;
    x_end = Line.x_end;
    y_start = Line.y_start;
    y_end = Line.y_end;
  }

  template<typename T>
  bool Line2D<T>::operator == (const Line2D<T>& line) const
  {
    if ( (x_start  == line.x_start) &&
         (y_start == line.y_start) &&
         (x_end   == line.x_end)   &&
         (y_end   == line.y_end) )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  template<typename T>
  float Line2D<T>::Length() const
  {
    float l;
    l = (float) std::sqrt ( (x_end - x_start) * (x_end - x_start) + (y_end - y_start) * (y_end - y_start) );

    return l;
  }

  template<typename T>
  const Vec2<T> Line2D<T>::GetDirectionVector() const
  {
    return Vec2<T> (x_start - x_end, y_start - y_end);
  }

  template<typename T>
  const Vec2<T> Line2D<T>::GetStartPoint() const
  {
    Vec2<T> p (x_start, y_start);
    return p;
  }

  template<typename T>
  const Vec2<T> Line2D<T>::GetEndPoint() const
  {
    Vec2<T> p (x_end, y_end);
    return p;
  }


};

#endif // LINE2D_H
