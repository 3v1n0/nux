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


#ifndef VECTOR2_H
#define VECTOR2_H

#include "../Exception.h"

#include "Vector4.h"
#include "Vector3.h"

namespace nux
{
  class Matrix2;
  template<typename T> class Vec2;

  template <typename T>
  class Vec2
  {
  public:
    inline Vec2();
    inline ~Vec2();
    inline Vec2 (const T &, const T &);
    inline Vec2 (const Vec2 &);

    inline Vec2<T>& operator = (const Vec2<T>&);

    inline bool operator == (const Vec2<T>&) const;
    inline bool operator != (const Vec2<T>&) const;
    inline Vec2<T> operator+ (const Vec2<T>&) const;
    inline Vec2<T> operator* (const Vec2<T>&) const;
    inline Vec2<T> operator- (const Vec2<T>&) const;
    inline Vec2<T> operator- () const;

    inline Vec2<T>& operator*= (const Vec2<T>&);
    inline Vec2<T>& operator+= (const Vec2<T>&);
    inline Vec2<T>& operator-= (const Vec2<T>&);

    inline Vec2<T> operator/ (const T &) const;
    inline Vec2<T> operator* (const T &) const;
    inline T &operator [] (int i);
    inline const T &operator [] (int i) const;

    inline T Length() const;
    inline T DotProduct (const Vec2 &) const;
    inline T CrossProduct (const Vec2 &) const;
    inline void Normalize();

    template <typename U> friend Vec2<U> operator* (const U &, const Vec2<U>&);

    friend class Matrix2;

    T x, y;
  };

  template <typename T>
  Vec2<T>::Vec2()
  {
    x = 0;
    y = 0;
  }

  template <typename T>
  Vec2<T>::~Vec2()
  {

  }

//template <typename T>
//Vec2<T>::Vec2(const T& fx)
//{
//    x = fx;
//    y = fx;
//}

  template <typename T>
  Vec2<T>::Vec2 (const T &fx, const T &fy)
  {
    x = fx;
    y = fy;
  }

//Vec2<T>::Vec2(double fx, double fy)
//{
//    x = float(fx);
//    y = float(fy);
//}
//
//Vec2<T>::Vec2(int fx, int fy)
//{
//    x = float(fx);
//    y = float(fy);
//}

  template <typename T>
  Vec2<T>::Vec2 (const Vec2<T>& v)
  {
    x = v.x;
    y = v.y;
  }

  template <typename T>
  Vec2<T>& Vec2<T>::operator = (const Vec2<T>& v)
  {
    x = v.x;
    y = v.y;
    return *this;
  }

  template <typename T>
  bool Vec2<T>::operator == (const Vec2<T>& v) const
  {
    if ( (x == v.x) &&
         (y == v.y) )
    {
      return true;
    }

    return false;
  }

  template <typename T>
  bool Vec2<T>::operator != (const Vec2<T>& v) const
  {
    return ! (*this == v);
  }

  template <typename T>
  Vec2<T> Vec2<T>::operator+ (const Vec2<T>& v) const
  {
    return Vec2<T> (x + v.x, y + v.y);
  }

  template <typename T>
  Vec2<T> Vec2<T>::operator* (const Vec2<T>& v) const
  {
    return Vec2<T> (x * v.x, y * v.y);
  }

  template <typename T>
  Vec2<T> Vec2<T>::operator- (const Vec2<T>& v) const
  {
    return Vec2<T> (x - v.x, y - v.y);
  }

  template <typename T>
  Vec2<T> Vec2<T>::operator- () const
  {
    return Vec2<T> (-x, -y);
  }

  template <typename T>
  Vec2<T>& Vec2<T>::operator*= (const Vec2<T>& v)
  {
    x *= v.x;
    y *= v.y;
    return *this;
  }

  template <typename T>
  Vec2<T>& Vec2<T>::operator+= (const Vec2<T>& v)
  {
    x += v.x;
    y += v.y;
    return *this;
  }

  template <typename T>
  Vec2<T>& Vec2<T>::operator-= (const Vec2<T>& v)
  {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  template <typename T>
  Vec2<T> Vec2<T>::operator/ (const T &f) const
  {
    if (f == 0)
    {
      throw DivisionByZeroException();
    }

    return Vec2 (x / f, y / f);
  }

  template <typename T>
  Vec2<T> Vec2<T>::operator* (const T &f) const
  {
    return Vec2<T> (x * f, y * f);
  }

/// element access
  template <typename T>
  T &Vec2<T>::operator [] (int i)
  {
    assert (i >= 0);
    assert (i <= 1);
    return * (&x + i);
  }

/// element access (const)
  template <typename T>
  const T &Vec2<T>::operator [] (int i) const
  {
    assert (i >= 0);
    assert (i <= 1);
    return * (&x + i);
  }

  template <typename T>
  T Vec2<T>::Length() const
  {
    return (T) sqrt (x * x + y * y);
  }

  template <typename T>
  T Vec2<T>::DotProduct (const Vec2<T>& v) const
  {
    return x * v.x + y * v.y;
  }

  template <typename T>
  T Vec2<T>::CrossProduct (const Vec2<T>& v) const
  {
    T val;
    val = x * v.y - y * v.x;
    return val;
  }

  template <typename T>
  void Vec2<T>::Normalize()
  {
    T l;

    l = Length();

    if (l == 0)
    {
      throw DivisionByZeroException();
    }

    x = x / l;
    y = y / l;
  }

  template <typename T>
  T DotProduct (const Vec2<T>& lhs, const Vec2<T>& rhs)
  {
    return lhs.x * rhs.x + lhs.y * rhs.y;
  }

  template <typename T>
  T CrossProduct (const Vec2<T>& lhs, const Vec2<T>& rhs)
  {
    return 	lhs.x * rhs.y - lhs.y * rhs.x;
  }

  template <typename U>
  Vec2<U> operator* (const U &f, const Vec2<U>& v)
  {
    return v * f;
  }

  typedef Vec2<float> Vector2;
  typedef Vec2<float> Vertex2;

}

#endif // VECTOR2_H
