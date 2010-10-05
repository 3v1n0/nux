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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef POINT3D_H
#define POINT3D_H

namespace nux   //NUX_NAMESPACE_BEGIN
{

  template<typename T>
  class Point3D
  {
  public:
    Point3D();
    ~Point3D();
    Point3D (const Point3D &Pt);
    Point3D (T x, T y, T z);

    void Set (T X, T Y, T Z);
    bool operator == (const Point3D<T>& Pt) const;
    bool operator != (const Point3D<T>& Pt) const;

    T x, y, z;
  };

  template<typename T>
  Point3D<T>::Point3D()
  {
    x = 0;
    y = 0;
    z = 0;
  }

  template<typename T>
  Point3D<T>::~Point3D()
  {

  }

  template<typename T>
  Point3D<T>::Point3D (const Point3D &Pt)
  {
    x = Pt.x;
    y = Pt.y;
    z = Pt.z;
  }

  template<typename T>
  Point3D<T>::Point3D (T X, T Y, T Z)
  {
    x = X;
    y = Y;
    z = Z;
  }

  template<typename T>
  void Point3D<T>::Set (T X, T Y, T Z)
  {
    x = X;
    y = Y;
    z = Z;
  }

  template<typename T>
  bool Point3D<T>::operator == (const Point3D<T>& Pt) const
  {
    if ( (x == Pt.x) &&
         (y == Pt.y) &&
         (z == Pt.z) )
    {
      return true;
    }

    return false;
  }

  template<typename T>
  bool Point3D<T>::operator != (const Point3D<T>& Pt) const
  {
    return ! ( (*this) == Pt);
  }

} //NUX_NAMESPACE_END

#endif // POINT3D_H
