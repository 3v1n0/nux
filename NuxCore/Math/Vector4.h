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


#ifndef VECTOR4_H
#define VECTOR4_H

namespace nux
{

  template <typename T>
  class Vec4
  {
  public:
    inline Vec4<T>();
    inline ~Vec4<T>();
    inline Vec4<T> (const T &, const T &, const T &, const T &);

    inline Vec4 (const Vec4<T>&);

    inline Vec4<T>& operator = (const Vec4<T>&);
    inline bool operator == (const Vec4<T>&) const;
    inline bool operator != (const Vec4<T>&) const;
    inline Vec4<T> operator + (const Vec4<T>&) const;
    inline Vec4<T> operator * (const Vec4<T>&) const;
    inline Vec4<T> operator - (const Vec4<T>&) const;
    inline Vec4<T> operator - () const;

    inline Vec4<T>& operator *= (const Vec4<T>&);
    inline Vec4<T>& operator += (const Vec4<T>&);
    inline Vec4<T>& operator -= (const Vec4<T>&);

    inline Vec4<T> operator / (const T &) const;
    inline Vec4<T> operator * (const T &) const;
    inline Vec4<T>& operator /= (const T &);
    inline Vec4<T>& operator *= (const T &);

    inline T &operator [] (int i);
    inline const T &operator [] (int i) const;

    void divide_xyz_by_w();

    template <typename U> friend Vec4<U> operator* (const U &, const Vec4<U>&);

    T x, y, z, w;
  };


  template <typename T>
  inline Vec4<T>::Vec4()
  {
    x = 0;
    y = 0;
    z = 0;
    w = 1;
  }

  template <typename T>
  inline Vec4<T>::~Vec4()
  {

  }

  template <typename T>
  inline Vec4<T>::Vec4 (const T &fx, const T &fy, const T &fz, const T &fw)
  {
    x = fx;
    y = fy;
    z = fz;
    w = fw;
  }

//Vec4::Vec4(double fx, double fy, double fz, double fw)
//{
//    x = T(fx);
//    y = T(fy);
//    z = T(fz);
//    w = T(fw);
//}
//
//Vec4::Vec4(int fx, int fy, int fz, int fw)
//{
//    x = T(fx);
//    y = T(fy);
//    z = T(fz);
//    w = T(fw);
//}

  template <typename T>
  inline Vec4<T>::Vec4 (const Vec4<T>& v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
  }

  template <typename T>
  inline Vec4<T>& Vec4<T>::operator= (const Vec4<T>& v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    w = v.w;
    return (*this);
  }

  template <typename T>
  inline bool Vec4<T>::operator == (const Vec4<T>& v) const
  {
    if ( (x == v.x) &&
         (y == v.y) &&
         (z == v.z) &&
         (w == v.w) )
    {
      return true;
    }

    return false;
  }

  template <typename T>
  inline bool Vec4<T>::operator != (const Vec4<T>& v) const
  {
    return ! (*this == v);
  }

  template <typename T>
  inline Vec4<T> Vec4<T>::operator+ (const Vec4<T>& v) const
  {
    return Vec4 (x + v.x, y + v.y, z + v.z, w + v.w);
  }

  template <typename T>
  inline Vec4<T> Vec4<T>::operator* (const Vec4<T>& v) const
  {
    return Vec4<T> (x * v.x, y * v.y, z * v.z, w * v.w);
  }

  template <typename T>
  inline Vec4<T> Vec4<T>::operator- (const Vec4<T>& v) const
  {
    return Vec4<T> (x - v.x, y - v.y, z - v.z, w - v.w);
  }

  template <typename T>
  inline Vec4<T> Vec4<T>::operator- () const
  {
    //Do that for Matices too
    return Vec4<T> (-x, -y, -z, -w);
  }

  template <typename T>
  inline Vec4<T>& Vec4<T>::operator*= (const Vec4<T>& v)
  {
    x *= v.x;
    y *= v.y;
    z *= v.z;
    w *= v.w;
    return *this;
  }

  template <typename T>
  inline Vec4<T>& Vec4<T>::operator+= (const Vec4<T>& v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    w += v.w;
    return *this;
  }

  template <typename T>
  inline Vec4<T>& Vec4<T>::operator-= (const Vec4<T>& v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    w -= v.w;
    return *this;
  }

  template <typename T>
  inline Vec4<T> Vec4<T>::operator / (const T &f) const
  {
    if (f == 0)
    {
      throw DivisionByZeroException();
    }

    return Vec4 (x / f, y / f, z / f, w / f);
  }

  template <typename T>
  inline Vec4<T> Vec4<T>::operator * (const T &f) const
  {
    return Vec4<T> (x * f, y * f, z * f, w * f);
  }

  template <typename T>
  inline Vec4<T>& Vec4<T>::operator /= (const T &f)
  {
    if (f == 0)
    {
      throw DivisionByZeroException();
    }

    x = x / f;
    y = y / f;
    z = z / f;
    w = w / f;
    return *this;
  }

  template <typename T>
  inline Vec4<T>& Vec4<T>::operator *= (const T &f)
  {
    x = x * f;
    y = y * f;
    z = z * f;
    w = w * f;
    return *this;
  }

  template <typename T>
  inline Vec4<T> operator* (T f , const Vec4<T>& v)
  {
    return Vec4<T> (f * v.x, f * v.y, f * v.z, f * v.w);
  }
/// element access
  template <typename T>
  inline T &Vec4<T>::operator [] (int i)
  {
    assert (i >= 0);
    assert (i <= 3);
    return * (&x + i);
  }

/// element access (const)
  template <typename T>
  inline const T &Vec4<T>::operator [] (int i) const
  {
    assert (i >= 0);
    assert (i <= 3);
    return * (&x + i);
  }

  template <typename T>
  void Vec4<T>::divide_xyz_by_w()
  {
    if (w == 0)
    {
      throw DivisionByZeroException();
    }

    x = x / w;
    y = y / w;
    z = z / w;
  }

  template <typename U>
  Vec4<U> operator* (const U &f, const Vec4<U>& v)
  {
    return v * f;
  }

  typedef Vec4<float> Vector4;

}

#endif // VECTOR4_H

