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


#ifndef LINE3D_H
#define LINE3D_H

#include "Vector3.h"

namespace nux
{

  template<typename T>
  class Line3D
  {
  public:
    Line3D();
    ~Line3D();
    Line3D (const Line3D &line);
    Line3D (T lx_start, T ly_start, T lz_start,
            T lx_end, T ly_end, T lz_end);
    Line3D (const Vector3 &pt, Vector3 v);

    const Line3D<T>& operator = (const Line3D<T>&);
    bool operator == (const Line3D<T>& line) const;

    float Length() const;
    const Vec3<T> GetDirectionVector() const;
    const Vec3<T> GetStartPoint() const;
    const Vec3<T> GetEndPoint() const;

  private:
    T x_start, y_start, z_start;
    T x_end, y_end, z_end;

  };

  template<typename T>
  Line3D<T>::Line3D()
  {
    x_start = y_start = z_start = x_end = y_end = z_end = 0;
  }

  template<typename T>
  Line3D<T>::~Line3D()
  {

  }

  template<typename T>
  Line3D<T>::Line3D (const Line3D &line)
  {
    x_start = line.x_start;
    x_end   = line.x_end;
    y_start = line.y_start;
    y_end   = line.y_end;
    z_start = line.z_start;
    z_end   = line.z_end;
  }

  template<typename T>
  Line3D<T>::Line3D (T lx_start, T ly_start, T lz_start,
                     T lx_end, T ly_end, T lz_end)
  {
    x_start = lx_start;
    x_end   = lx_end;
    y_start = ly_start;
    y_end   = ly_end;
    z_start = lz_start;
    z_end   = lz_end;
  }

  template<typename T>
  Line3D<T>::Line3D (const Vector3 &pt, Vector3 v)
  {
    x_start = pt.x;
    y_start = pt.y;
    z_start = pt.z;

    x_end   = x_start + v.x;
    y_end   = y_start + v.y;
    z_end   = y_start + v.z;
  }

  template<typename T>
  const Line3D<T>& Line3D<T>::operator = (const Line3D<T>& Line)
  {
    x_start = Line.x_start;
    y_start = Line.y_start;
    z_start = Line.z_start;
    x_end = Line.x_end;
    y_end = Line.y_end;
    z_end = Line.z_end;

  }

  template<typename T>
  bool Line3D<T>::operator == (const Line3D &line) const
  {
    if ( (x_start  == line.x_start) &&
         (y_start == line.y_start) &&
         (z_start == line.z_start) &&
         (x_end   == line.x_end)   &&
         (y_end   == line.y_end)   &&
         (z_end   == line.z_end) )
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  template<typename T>
  float Line3D<T>::Length() const
  {
    float l;
    l = (float) std::sqrt ( (x_end - x_start) * (x_end - x_start) +
                            (y_end - y_start) * (y_end - y_start) +
                            (z_end - z_start) * (z_end - z_start) );

    return l;
  }

  template<typename T>
  const Vec3<T> Line3D<T>::GetDirectionVector() const
  {
    return Vec3<T> (x_start - x_end, y_start - y_end, z_start - z_end);
  }

  template<typename T>
  const Vec3<T> Line3D<T>::GetStartPoint() const
  {
    Vec3<T> p (x_start, y_start, z_start);
    return p;
  }

  template<typename T>
  const Vec3<T> Line3D<T>::GetEndPoint() const
  {
    Vec3<T> p (x_end, y_end, z_end);
    return p;
  }

}

#endif // LINE3D_H
