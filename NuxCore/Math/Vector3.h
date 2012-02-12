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


#ifndef VECTOR3_H
#define VECTOR3_H

#include "../Exception.h"

#include "Vector4.h"

namespace nux
{

  template <typename T>
  class Vec3
  {
  public:
    inline Vec3();
    inline Vec3 (const T &, const T &, const T &);
    inline ~Vec3();
    inline Vec3 (const Vec3 &);

    inline Vec3<T>& operator = (const Vec3<T>&);

    inline bool operator == (const Vec3<T>&) const;
    inline bool operator != (const Vec3<T>&) const;
    inline Vec3<T> operator + (const Vec3<T>&) const;
    inline Vec3<T> operator * (const Vec3<T>&) const;
    inline Vec3<T> operator - (const Vec3<T>&) const;
    inline Vec3<T> operator - () const;

    inline Vec3<T>& operator *= (const Vec3<T>&);
    inline Vec3<T>& operator += (const Vec3<T>&);
    inline Vec3<T>& operator -= (const Vec3<T>&);

    inline Vec3<T> operator / (const T &) const;
    inline Vec3<T> operator * (const T &) const;
    inline Vec3<T>& operator /= (const T &);
    inline Vec3<T>& operator *= (const T &);

    inline T &operator [] (int i);
    inline const T &operator [] (int i) const;

    inline T Length() const;
    inline T LengthSquared() const;
    inline T DotProduct (const Vec3<T>&) const;
    inline Vec3 CrossProduct (const Vec3<T>&) const;
    inline void Normalize();

    template <typename U> friend Vec3<U> operator* (const U &, const Vec3<U>&);

    //friend  Vec3<T> operator * (T, Vec3&);

    T x, y, z;
  };

  template <typename T>
  inline Vec3<T>::Vec3()
  {
    x = 0;
    y = 0;
    z = 0;
  }

  template <typename T>
  inline Vec3<T>::~Vec3()
  {

  }

  template <typename T>
  inline Vec3<T>::Vec3 (const T &fx, const T &fy, const T &fz)
  {
    x = fx;
    y = fy;
    z = fz;
  }

//Vec3::Vec3(double fx, double fy, double fz)
//{
//    x = T(fx);
//    y = T(fy);
//    z = T(fz);
//}
//
//Vec3::Vec3(int fx, int fy, int fz)
//{
//    x = T(fx);
//    y = T(fy);
//    z = T(fz);
//}

  template <typename T>
  Vec3<T>::Vec3 (const Vec3<T>& v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
  }

  template <typename T>
  Vec3<T>& Vec3<T>::operator = (const Vec3<T>& v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
  }

  template <typename T>
  bool Vec3<T>::operator == (const Vec3<T>& v) const
  {
    if ( (x == v.x) &&
         (y == v.y) &&
         (z == v.z) )
    {
      return true;
    }

    return false;
  }

  template <typename T>
  bool Vec3<T>::operator != (const Vec3<T>& v) const
  {
    return ! (*this == v);
  }

  template <typename T>
  Vec3<T> Vec3<T>::operator + (const Vec3<T>& v) const
  {
    return Vec3<T> (x + v.x, y + v.y, z + v.z);
  }

  template <typename T>
  Vec3<T> Vec3<T>::operator * (const Vec3 &v) const
  {
    return Vec3<T> (x * v.x, y * v.y, z * v.z);
  }

  template <typename T>
  Vec3<T> Vec3<T>::operator - (const Vec3 &v) const
  {
    return Vec3<T> (x - v.x, y - v.y, z - v.z);
  }

  template <typename T>
  Vec3<T> Vec3<T>::operator - () const
  {
    return Vec3 (-x, -y, -z);
  }

  template <typename T>
  Vec3<T>& Vec3<T>::operator *= (const Vec3 &v)
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    return *this;
  }

  template <typename T>
  Vec3<T>& Vec3<T>::operator += (const Vec3 &v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }

  template <typename T>
  Vec3<T>& Vec3<T>::operator -= (const Vec3 &v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }

  template <typename T>
  Vec3<T> Vec3<T>::operator / (const T &f) const
  {
    if (f == 0)
    {
      throw DivisionByZeroException();
    }

    return Vec3<T> (x / f, y / f, z / f);
  }

  template <typename T>
  Vec3<T> Vec3<T>::operator * (const T &f) const
  {
    return Vec3<T> (x * f, y * f, z * f);
  }

  template <typename T>
  Vec3<T>& Vec3<T>::operator /= (const T &f)
  {
    if (f == 0)
    {
      throw DivisionByZeroException();
    }

    x = x / f;
    y = y / f;
    z = z / f;
    return *this;
  }

  template <typename T>
  Vec3<T>& Vec3<T>::operator *= (const T &f)
  {
    x = x * f;
    y = y * f;
    z = z * f;
    return *this;
  }

/// element access
  template <typename T>
  T &Vec3<T>::operator [] (int i)
  {
    assert (i >= 0);
    assert (i <= 2);
    return * (&x + i);
  }

/// element access (const)
  template <typename T>
  const T &Vec3<T>::operator [] (int i) const
  {
    assert (i >= 0);
    assert (i <= 2);
    return * (&x + i);
  }

  template <typename T>
  T Vec3<T>::Length() const
  {
    return sqrt (x * x + y * y + z * z);
  }

  template <typename T>
  T Vec3<T>::LengthSquared() const
  {
    return (x * x + y * y + z * z);
  }

  template <typename T>
  T Vec3<T>::DotProduct (const Vec3<T>& v) const
  {
    return x * v.x + y * v.y + z * v.z;
  }

  template <typename T>
  Vec3<T> Vec3<T>::CrossProduct (const Vec3<T>& v) const
  {
    return Vec3<T> (y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }

  template <typename T>
  void Vec3<T>::Normalize()
  {
    T l;
    l = Length();

    if (l == 0)
    {
      throw DivisionByZeroException();
    }

    x = x / l;
    y = y / l;
    z = z / l;
  }

  template <typename T>
  T DotProduct (const Vec3<T>& lhs, const Vec3<T>& rhs)
  {
    T out;
    out = lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    return out;
  }

  template <typename T>
  const Vec3<T> CrossProduct (const Vec3<T>& lhs, const Vec3<T>& rhs)
  {
    Vec3<T> out;
    out.x = lhs.y * rhs.z - lhs.z * rhs.y;
    out.y = lhs.z * rhs.x - lhs.x * rhs.z;
    out.z = lhs.x * rhs.y - lhs.y * rhs.x;

    return out;
  }

  template <typename U>
  inline Vec3<U> operator * (const U &f, const Vec3<U>& v)
  {
    return v * f;
  }

  typedef Vec3<float> Vector3;
  typedef Vec3<float> Vertex3;

}

#endif // VECTOR3_H
