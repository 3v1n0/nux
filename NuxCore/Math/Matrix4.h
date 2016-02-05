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


#ifndef MATRIX4_H
#define MATRIX4_H


#include "Constants.h"
#include "Vector3.h"
#include "Vector4.h"

namespace nux
{

//  Our matrices are Row major just like C/C++:
//      m[2][3] represent the element at row 2 and column 3.
//  When multiplying a vector by a matrix, the vector is a column at the right of the matrix
//
//    |a00,  a01,  a02,  a03|       |v0|
//    |a10,  a11,  a12,  a13|   *   |v1|
//    |a20,  a21,  a22,  a23|       |v2|
//    |a30,  a31,  a32,  a33|       |v3|
//
// Note: OpenGL is column major. Before passing it a Matrix4x4 through glLoadMatrix, transpose it.

  template<typename T>
  class Matrix4x4
  {
  public:
    Matrix4x4<T>();
    ~Matrix4x4<T>();
    Matrix4x4<T> (const Matrix4x4 &);
    Matrix4x4<T> (
      T a00, T a01, T a02, T a03,
      T a10, T a11, T a12, T a13,
      T a20, T a21, T a22, T a23,
      T a30, T a31, T a32, T a33);

    Matrix4x4<T>& operator = (const Matrix4x4<T>&);
    bool operator == (const Matrix4x4<T>&);
    Matrix4x4<T> operator * (const Matrix4x4<T>&) const;
    Matrix4x4<T> operator + (const Matrix4x4<T>&) const;
    Matrix4x4<T> operator - (const Matrix4x4<T>&) const;
    Matrix4x4<T>& operator *= (const Matrix4x4<T>&) const;
    Matrix4x4<T>& operator += (const Matrix4x4<T>&) const;
    Matrix4x4<T>& operator -= (const Matrix4x4<T>&) const;

    Matrix4x4<T> operator * (const T &) const;
    Matrix4x4<T> operator / (const T &) const;
    Matrix4x4<T> operator *= (const T &) const;
    Matrix4x4<T> operator /= (const T &) const;

    Vector4      operator * (const Vector4 &) const;
    Matrix4x4<T> operator - ();

    // Get the (i, j) element of the current matrix.
    T &operator() (unsigned int i, unsigned int j);
    T operator () (unsigned int i, unsigned int j) const;

    // Get a pointer to the current matrix.
    operator T *();
    operator const T *() const;

    // Utility for 3D
    void Translate (T x, T y, T z);
    void Translate (const Vector3 &);

    void Rotate_x (T angle);
    void Rotate_y (T angle);
    void Rotate_z (T angle);
    void Scale (T sx, T sy, T sz);

    // Matrix Math
    T Trace() const;
    T Determinant() const ;
    void Inverse();
    Matrix4x4<T> GetInverse() const;
    void Transpose();

    //Matrix3x3<T> GetUpper3x3() const;
    //Matrix2x2<T> GetUpper2x2() const;

    ///////////////////////////////////////////////
    void Scale (T s);
    void Diagonal (T x, T y, T z, T w = T (1) );
    void Rotate (T angle, Vector3 axis);

    // OpenGL
    void LookAt (const Vector3 &eye, const Vector3 &at, const Vector3 &up);
    void Orthographic (T l, T r, T b, T t, T n, T f);
    void Perspective (T l, T r, T t, T b, T n, T f);
    void PerspectiveInverse (T l, T r, T t, T b, T n, T f);
    void Perspective (T FoV, T AspectRatio, T NearPlane, T FarPlane);

    void Zero();
    void Identity();

    static Matrix4x4<T> IDENTITY();
    static Matrix4x4<T> ZERO();
    static Matrix4x4<T> ROTATEX(T angle);
    static Matrix4x4<T> ROTATEY(T angle);
    static Matrix4x4<T> ROTATEZ(T angle);
    static Matrix4x4<T> TRANSLATE(T x, T y, T z);
    static Matrix4x4<T> SCALE(T x, T y, T z);
    T m[4][4];
  };


  /***************************************************************************************\
  Function:       Matrix4x4<T>::Matrix4x4<T>

  Description:    Constructor. Initialize the matrix to identity.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T>::Matrix4x4()
  {
    Identity();
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::~Matrix4x4

  Description:    Destructor.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T>::~Matrix4x4()
  {

  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::Matrix4x4

  Description:    None.

  Parameters:     - M

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T>::Matrix4x4 (const Matrix4x4 &M)
  {
    m[0][0] = M.m[0][0];
    m[0][1] = M.m[0][1];
    m[0][2] = M.m[0][2];
    m[0][3] = M.m[0][3];
    m[1][0] = M.m[1][0];
    m[1][1] = M.m[1][1];
    m[1][2] = M.m[1][2];
    m[1][3] = M.m[1][3];
    m[2][0] = M.m[2][0];
    m[2][1] = M.m[2][1];
    m[2][2] = M.m[2][2];
    m[2][3] = M.m[2][3];
    m[3][0] = M.m[3][0];
    m[3][1] = M.m[3][1];
    m[3][2] = M.m[3][2];
    m[3][3] = M.m[3][3];
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::Matrix4x4

  Description:    None.

  Parameters:     T a00, T a01, T a02, T a03,
                  T a10, T a11, T a12, T a13,
                  T a20, T a21, T a22, T a23,
                  T a30, T a31, T a32, T a33

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T>::Matrix4x4 (
    T a00, T a01, T a02, T a03,
    T a10, T a11, T a12, T a13,
    T a20, T a21, T a22, T a23,
    T a30, T a31, T a32, T a33)
  {
    m[0][0] = a00;
    m[0][1] = a01;
    m[0][2] = a02;
    m[0][3] = a03;
    m[1][0] = a10;
    m[1][1] = a11;
    m[1][2] = a12;
    m[1][3] = a13;
    m[2][0] = a20;
    m[2][1] = a21;
    m[2][2] = a22;
    m[2][3] = a23;
    m[3][0] = a30;
    m[3][1] = a31;
    m[3][2] = a32;
    m[3][3] = a33;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::Matrix4x4

  Description:    Assignment operator.

  Parameters:     - M

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T>& Matrix4x4<T>::operator = (const Matrix4x4<T>& M)
  {
    m[0][0] = M.m[0][0];
    m[0][1] = M.m[0][1];
    m[0][2] = M.m[0][2];
    m[0][3] = M.m[0][3];
    m[1][0] = M.m[1][0];
    m[1][1] = M.m[1][1];
    m[1][2] = M.m[1][2];
    m[1][3] = M.m[1][3];
    m[2][0] = M.m[2][0];
    m[2][1] = M.m[2][1];
    m[2][2] = M.m[2][2];
    m[2][3] = M.m[2][3];
    m[3][0] = M.m[3][0];
    m[3][1] = M.m[3][1];
    m[3][2] = M.m[3][2];
    m[3][3] = M.m[3][3];

    return (*this);
  }

  template <typename T>
  bool Matrix4x4<T>::operator == (const Matrix4x4<T>& M)
  {
    for (int i = 0; i < 4; i++)
      for (int j = 0; j < 4; j++)
      {
        if (m[i][j] != M.m[i][j])
          return false;
      }

    return true;
  }


  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator *

  Description:    Multiply by matrix iM.

  Parameters:     - iM

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T> Matrix4x4<T>::operator * (const Matrix4x4<T>& iM) const
  {
    Matrix4x4<T> oM;

    // Output matrix first row
    oM.m[0][0] = m[0][0] * iM.m[0][0] + m[0][1] * iM.m[1][0] + m[0][2] * iM.m[2][0] + m[0][3] * iM.m[3][0];
    oM.m[0][1] = m[0][0] * iM.m[0][1] + m[0][1] * iM.m[1][1] + m[0][2] * iM.m[2][1] + m[0][3] * iM.m[3][1];
    oM.m[0][2] = m[0][0] * iM.m[0][2] + m[0][1] * iM.m[1][2] + m[0][2] * iM.m[2][2] + m[0][3] * iM.m[3][2];
    oM.m[0][3] = m[0][0] * iM.m[0][3] + m[0][1] * iM.m[1][3] + m[0][2] * iM.m[2][3] + m[0][3] * iM.m[3][3];

    // Output matrix second row
    oM.m[1][0] = m[1][0] * iM.m[0][0] + m[1][1] * iM.m[1][0] + m[1][2] * iM.m[2][0] + m[1][3] * iM.m[3][0];
    oM.m[1][1] = m[1][0] * iM.m[0][1] + m[1][1] * iM.m[1][1] + m[1][2] * iM.m[2][1] + m[1][3] * iM.m[3][1];
    oM.m[1][2] = m[1][0] * iM.m[0][2] + m[1][1] * iM.m[1][2] + m[1][2] * iM.m[2][2] + m[1][3] * iM.m[3][2];
    oM.m[1][3] = m[1][0] * iM.m[0][3] + m[1][1] * iM.m[1][3] + m[1][2] * iM.m[2][3] + m[1][3] * iM.m[3][3];

    // Output matrix third row
    oM.m[2][0] = m[2][0] * iM.m[0][0] + m[2][1] * iM.m[1][0] + m[2][2] * iM.m[2][0] + m[2][3] * iM.m[3][0];
    oM.m[2][1] = m[2][0] * iM.m[0][1] + m[2][1] * iM.m[1][1] + m[2][2] * iM.m[2][1] + m[2][3] * iM.m[3][1];
    oM.m[2][2] = m[2][0] * iM.m[0][2] + m[2][1] * iM.m[1][2] + m[2][2] * iM.m[2][2] + m[2][3] * iM.m[3][2];
    oM.m[2][3] = m[2][0] * iM.m[0][3] + m[2][1] * iM.m[1][3] + m[2][2] * iM.m[2][3] + m[2][3] * iM.m[3][3];

    // Output matrix fourth row
    oM.m[3][0] = m[3][0] * iM.m[0][0] + m[3][1] * iM.m[1][0] + m[3][2] * iM.m[2][0] + m[3][3] * iM.m[3][0];
    oM.m[3][1] = m[3][0] * iM.m[0][1] + m[3][1] * iM.m[1][1] + m[3][2] * iM.m[2][1] + m[3][3] * iM.m[3][1];
    oM.m[3][2] = m[3][0] * iM.m[0][2] + m[3][1] * iM.m[1][2] + m[3][2] * iM.m[2][2] + m[3][3] * iM.m[3][2];
    oM.m[3][3] = m[3][0] * iM.m[0][3] + m[3][1] * iM.m[1][3] + m[3][2] * iM.m[2][3] + m[3][3] * iM.m[3][3];

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator +

  Description:    Add matrix iM.

  Parameters:     - iM

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T> Matrix4x4<T>::operator + (const Matrix4x4<T>& iM) const
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = m[0][0] + iM.m[0][0];
    oM.m[0][1] = m[0][1] + iM.m[0][1];
    oM.m[0][2] = m[0][2] + iM.m[0][2];
    oM.m[0][3] = m[0][3] + iM.m[0][3];
    oM.m[1][0] = m[1][0] + iM.m[1][0];
    oM.m[1][1] = m[1][1] + iM.m[1][1];
    oM.m[1][2] = m[1][2] + iM.m[1][2];
    oM.m[1][3] = m[1][3] + iM.m[1][3];
    oM.m[2][0] = m[2][0] + iM.m[2][0];
    oM.m[2][1] = m[2][1] + iM.m[2][1];
    oM.m[2][2] = m[2][2] + iM.m[2][2];
    oM.m[2][3] = m[2][3] + iM.m[2][3];
    oM.m[3][0] = m[3][0] + iM.m[3][0];
    oM.m[3][1] = m[3][1] + iM.m[3][1];
    oM.m[3][2] = m[3][2] + iM.m[3][2];
    oM.m[3][3] = m[3][3] + iM.m[3][3];

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator -

  Description:    Substract matrix iM.

  Parameters:     - iM

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T> Matrix4x4<T>::operator - (const Matrix4x4<T>& iM) const
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = m[0][0] - iM.m[0][0];
    oM.m[0][1] = m[0][1] - iM.m[0][1];
    oM.m[0][2] = m[0][2] - iM.m[0][2];
    oM.m[0][3] = m[0][3] - iM.m[0][3];
    oM.m[1][0] = m[1][0] - iM.m[1][0];
    oM.m[1][1] = m[1][1] - iM.m[1][1];
    oM.m[1][2] = m[1][2] - iM.m[1][2];
    oM.m[1][3] = m[1][3] - iM.m[1][3];
    oM.m[2][0] = m[2][0] - iM.m[2][0];
    oM.m[2][1] = m[2][1] - iM.m[2][1];
    oM.m[2][2] = m[2][2] - iM.m[2][2];
    oM.m[2][3] = m[2][3] - iM.m[2][3];
    oM.m[3][0] = m[3][0] - iM.m[3][0];
    oM.m[3][1] = m[3][1] - iM.m[3][1];
    oM.m[3][2] = m[3][2] - iM.m[3][2];
    oM.m[3][3] = m[3][3] - iM.m[3][3];

    return oM;
  }





  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator *=

  Description:    Multiply by matrix iM.

  Parameters:     - iM

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T>& Matrix4x4<T>::operator *= (const Matrix4x4<T>& iM) const
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = m[0][0] * iM.m[0][0] + m[0][1] * iM.m[1][0] + m[0][2] * iM.m[2][0] + m[0][3] * iM.m[3][0];
    oM.m[1][0] = m[1][0] * iM.m[0][0] + m[1][1] * iM.m[1][0] + m[1][2] * iM.m[2][0] + m[1][3] * iM.m[3][0];
    oM.m[2][0] = m[2][0] * iM.m[0][0] + m[2][1] * iM.m[1][0] + m[2][2] * iM.m[2][0] + m[2][3] * iM.m[3][0];
    oM.m[3][0] = m[3][0] * iM.m[0][0] + m[3][1] * iM.m[1][0] + m[3][2] * iM.m[2][0] + m[3][3] * iM.m[3][0];

    oM.m[0][1] = m[0][0] * iM.m[0][1] + m[0][1] * iM.m[1][1] + m[0][2] * iM.m[2][1] + m[0][3] * iM.m[3][1];
    oM.m[1][1] = m[1][0] * iM.m[0][1] + m[1][1] * iM.m[1][1] + m[1][2] * iM.m[2][1] + m[1][3] * iM.m[3][1];
    oM.m[2][1] = m[2][0] * iM.m[0][1] + m[2][1] * iM.m[1][1] + m[2][2] * iM.m[2][1] + m[2][3] * iM.m[3][1];
    oM.m[3][1] = m[3][0] * iM.m[0][1] + m[3][1] * iM.m[1][1] + m[3][2] * iM.m[2][1] + m[3][3] * iM.m[3][1];

    oM.m[0][2] = m[0][0] * iM.m[0][2] + m[0][1] * iM.m[1][2] + m[0][2] * iM.m[2][2] + m[0][3] * iM.m[3][2];
    oM.m[1][2] = m[1][0] * iM.m[0][2] + m[1][1] * iM.m[1][2] + m[1][2] * iM.m[2][2] + m[1][3] * iM.m[3][2];
    oM.m[2][2] = m[2][0] * iM.m[0][2] + m[2][1] * iM.m[1][2] + m[2][2] * iM.m[2][2] + m[2][3] * iM.m[3][2];
    oM.m[3][2] = m[3][0] * iM.m[0][2] + m[3][1] * iM.m[1][2] + m[3][2] * iM.m[2][2] + m[3][3] * iM.m[3][2];

    oM.m[0][3] = m[0][0] * iM.m[0][3] + m[0][1] * iM.m[1][3] + m[0][2] * iM.m[2][3] + m[0][3] * iM.m[3][3];
    oM.m[1][3] = m[1][0] * iM.m[0][3] + m[1][1] * iM.m[1][3] + m[1][2] * iM.m[2][3] + m[1][3] * iM.m[3][3];
    oM.m[2][3] = m[2][0] * iM.m[0][3] + m[2][1] * iM.m[1][3] + m[2][2] * iM.m[2][3] + m[2][3] * iM.m[3][3];
    oM.m[3][3] = m[3][0] * iM.m[0][3] + m[3][1] * iM.m[1][3] + m[3][2] * iM.m[2][3] + m[3][3] * iM.m[3][3];

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator +=

  Description:    Add matrix iM.

  Parameters:     - iM

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T>& Matrix4x4<T>::operator += (const Matrix4x4<T>& iM) const
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = m[0][0] + iM.m[0][0];
    oM.m[0][1] = m[0][1] + iM.m[0][1];
    oM.m[0][2] = m[0][2] + iM.m[0][2];
    oM.m[0][3] = m[0][3] + iM.m[0][3];
    oM.m[1][0] = m[1][0] + iM.m[1][0];
    oM.m[1][1] = m[1][1] + iM.m[1][1];
    oM.m[1][2] = m[1][2] + iM.m[1][2];
    oM.m[1][3] = m[1][3] + iM.m[1][3];
    oM.m[2][0] = m[2][0] + iM.m[2][0];
    oM.m[2][1] = m[2][1] + iM.m[2][1];
    oM.m[2][2] = m[2][2] + iM.m[2][2];
    oM.m[2][3] = m[2][3] + iM.m[2][3];
    oM.m[3][0] = m[3][0] + iM.m[3][0];
    oM.m[3][1] = m[3][1] + iM.m[3][1];
    oM.m[3][2] = m[3][2] + iM.m[3][2];
    oM.m[3][3] = m[3][3] + iM.m[3][3];

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator -=

  Description:    Substract matrix iM.

  Parameters:     - iM

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T>& Matrix4x4<T>::operator -= (const Matrix4x4<T>& iM) const
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = m[0][0] - iM.m[0][0];
    oM.m[0][1] = m[0][1] - iM.m[0][1];
    oM.m[0][2] = m[0][2] - iM.m[0][2];
    oM.m[0][3] = m[0][3] - iM.m[0][3];
    oM.m[1][0] = m[1][0] - iM.m[1][0];
    oM.m[1][1] = m[1][1] - iM.m[1][1];
    oM.m[1][2] = m[1][2] - iM.m[1][2];
    oM.m[1][3] = m[1][3] - iM.m[1][3];
    oM.m[2][0] = m[2][0] - iM.m[2][0];
    oM.m[2][1] = m[2][1] - iM.m[2][1];
    oM.m[2][2] = m[2][2] - iM.m[2][2];
    oM.m[2][3] = m[2][3] - iM.m[2][3];
    oM.m[3][0] = m[3][0] - iM.m[3][0];
    oM.m[3][1] = m[3][1] - iM.m[3][1];
    oM.m[3][2] = m[3][2] - iM.m[3][2];
    oM.m[3][3] = m[3][3] - iM.m[3][3];

    *this = oM;
    return *this;
  }


  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator *

  Description:    Multiply all elements by f.

  Parameters:     - f

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T> Matrix4x4<T>::operator * (const T &f) const
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = m[0][0] * f;
    oM.m[0][1] = m[0][1] * f;
    oM.m[0][2] = m[0][2] * f;
    oM.m[0][3] = m[0][3] * f;
    oM.m[1][0] = m[1][0] * f;
    oM.m[1][1] = m[1][1] * f;
    oM.m[1][2] = m[1][2] * f;
    oM.m[1][3] = m[1][3] * f;
    oM.m[2][0] = m[2][0] * f;
    oM.m[2][1] = m[2][1] * f;
    oM.m[2][2] = m[2][2] * f;
    oM.m[2][3] = m[2][3] * f;
    oM.m[3][0] = m[3][0] * f;
    oM.m[3][1] = m[3][1] * f;
    oM.m[3][2] = m[3][2] * f;
    oM.m[3][3] = m[3][3] * f;

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator /

  Description:    Divide all elements by f.

  Parameters:     - f

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T> Matrix4x4<T>::operator / (const T &f) const
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = m[0][0] / f;
    oM.m[0][1] = m[0][1] / f;
    oM.m[0][2] = m[0][2] / f;
    oM.m[0][3] = m[0][3] / f;
    oM.m[1][0] = m[1][0] / f;
    oM.m[1][1] = m[1][1] / f;
    oM.m[1][2] = m[1][2] / f;
    oM.m[1][3] = m[1][3] / f;
    oM.m[2][0] = m[2][0] / f;
    oM.m[2][1] = m[2][1] / f;
    oM.m[2][2] = m[2][2] / f;
    oM.m[2][3] = m[2][3] / f;
    oM.m[3][0] = m[3][0] / f;
    oM.m[3][1] = m[3][1] / f;
    oM.m[3][2] = m[3][2] / f;
    oM.m[3][3] = m[3][3] / f;

    return oM;
  }




  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator *=

  Description:    Multiply all elements by f.

  Parameters:     - f

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T> Matrix4x4<T>::operator *= (const T &f) const
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = m[0][0] * f;
    oM.m[0][1] = m[0][1] * f;
    oM.m[0][2] = m[0][2] * f;
    oM.m[0][3] = m[0][3] * f;
    oM.m[1][0] = m[1][0] * f;
    oM.m[1][1] = m[1][1] * f;
    oM.m[1][2] = m[1][2] * f;
    oM.m[1][3] = m[1][3] * f;
    oM.m[2][0] = m[2][0] * f;
    oM.m[2][1] = m[2][1] * f;
    oM.m[2][2] = m[2][2] * f;
    oM.m[2][3] = m[2][3] * f;
    oM.m[3][0] = m[3][0] * f;
    oM.m[3][1] = m[3][1] * f;
    oM.m[3][2] = m[3][2] * f;
    oM.m[3][3] = m[3][3] * f;

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator /=

  Description:    Divide all elements by f.

  Parameters:     - f

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T> Matrix4x4<T>::operator /= (const T &f) const
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = m[0][0] / f;
    oM.m[0][1] = m[0][1] / f;
    oM.m[0][2] = m[0][2] / f;
    oM.m[0][3] = m[0][3] / f;
    oM.m[1][0] = m[1][0] / f;
    oM.m[1][1] = m[1][1] / f;
    oM.m[1][2] = m[1][2] / f;
    oM.m[1][3] = m[1][3] / f;
    oM.m[2][0] = m[2][0] / f;
    oM.m[2][1] = m[2][1] / f;
    oM.m[2][2] = m[2][2] / f;
    oM.m[2][3] = m[2][3] / f;
    oM.m[3][0] = m[3][0] / f;
    oM.m[3][1] = m[3][1] / f;
    oM.m[3][2] = m[3][2] / f;
    oM.m[3][3] = m[3][3] / f;

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator *

  Description:    Multiply a matrix by a vector.

  Parameters:     - V

  Return Value:   Vector4.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Vector4 Matrix4x4<T>::operator * (const Vector4 &V) const
  {
    Vector4 oV;

    oV.x = V.x * m[0][0] + V.y * m[0][1] + V.z * m[0][2] + V.w * m[0][3];
    oV.y = V.x * m[1][0] + V.y * m[1][1] + V.z * m[1][2] + V.w * m[1][3];
    oV.z = V.x * m[2][0] + V.y * m[2][1] + V.z * m[2][2] + V.w * m[2][3];
    oV.w = V.x * m[3][0] + V.y * m[3][1] + V.z * m[3][2] + V.w * m[3][3];

    return oV;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::operator - ()

  Description:    Negate all elements of the matrix.

  Parameters:     None.

  Return Value:   Matrix4x4<T>.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T> Matrix4x4<T>::operator - ()
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = -m[0][0];
    oM.m[0][1] = -m[0][1];
    oM.m[0][2] = -m[0][2];
    oM.m[0][3] = -m[0][3];

    oM.m[1][0] = -m[1][0];
    oM.m[1][1] = -m[1][1];
    oM.m[1][2] = -m[1][2];
    oM.m[1][3] = -m[1][3];

    oM.m[2][0] = -m[2][0];
    oM.m[2][1] = -m[2][1];
    oM.m[2][2] = -m[2][2];
    oM.m[2][3] = -m[2][3];

    oM.m[3][0] = -m[3][0];
    oM.m[3][1] = -m[3][1];
    oM.m[3][2] = -m[3][2];
    oM.m[3][3] = -m[3][3];

    return oM;
  }

  template <typename T>
  T &Matrix4x4<T>::operator () (unsigned int i, unsigned int j)
  {
    return m[i][j];
  }

  template <typename T>
  T Matrix4x4<T>::operator () (unsigned int i, unsigned int j) const
  {
    return m[i][j];
  }

  template <typename T>
  Matrix4x4<T>::operator T *()
  {
    return reinterpret_cast<T *> (&m);
  }

  template <typename T>
  Matrix4x4<T>::operator const T *() const
  {
    return reinterpret_cast<const T *> (&m);
  }


  /***************************************************************************************\
  Function:       Matrix4x4<T>::zero

  Description:    Set the matrix to zero.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix4x4<T>::Zero()
  {
    m[0][0] = 0.0;
    m[0][1] = 0.0;
    m[0][2] = 0.0;
    m[0][3] = 0.0;
    m[1][0] = 0.0;
    m[1][1] = 0.0;
    m[1][2] = 0.0;
    m[1][3] = 0.0;
    m[2][0] = 0.0;
    m[2][1] = 0.0;
    m[2][2] = 0.0;
    m[2][3] = 0.0;
    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 0.0;

    //memset(m, 0, sizeof(m));
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::identity

  Description:    Set the matrix to identity.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix4x4<T>::Identity()
  {
    m[0][0] = 1.0;
    m[0][1] = 0.0;
    m[0][2] = 0.0;
    m[0][3] = 0.0;
    m[1][0] = 0.0;
    m[1][1] = 1.0;
    m[1][2] = 0.0;
    m[1][3] = 0.0;
    m[2][0] = 0.0;
    m[2][1] = 0.0;
    m[2][2] = 1.0;
    m[2][3] = 0.0;
    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1.0;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::translate

  Description:    Add a translation to the current matrix.

  Parameters:     - x
  - y
  - z

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix4x4<T>::Translate (T x, T y, T z)
  {
    Identity();
    m[0][3] = x;
    m[1][3] = y;
    m[2][3] = z;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::Transpose

  Description:    Transpose the current matrix.

  Parameters:     None

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix4x4<T>::Transpose()
  {
    for (int i = 0; i < 4; i++)
    {
      for (int j = 0; j < i; j++)
      {
        T t = m[i][j];
        m[i][j] = m[j][i];
        m[j][i] = t;
      }
    }
  }


  /***************************************************************************************\
  Function:       Matrix4x4<T>::Rotate_x

  Description:    Add rotation matrix around axe X.

  Parameters:     - angle

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix4x4<T>::Rotate_x (T angle)
  {
    Identity();

    m[0][0] = 1.0f;
    m[1][0] = 0.0f;
    m[2][0] = 0.0f;
    m[3][0] = 0.0f;

    m[0][1] = 0.0f;
    m[1][1] = (T) cos (angle);
    m[2][1] = (T) sin (angle);
    m[3][1] = 0.0f;

    m[0][2] = 0.0f;
    m[1][2] = (T) - sin (angle);
    m[2][2] = (T) cos (angle);
    m[3][2] = 0.0f;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::Rotate_y

  Description:    Add rotation matrix around axe Y.

  Parameters:     - angle

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix4x4<T>::Rotate_y (T angle)
  {
    Identity();

    m[0][0] = (T) cos (angle);
    m[1][0] = 0.0f;
    m[2][0] = (T) - sin (angle);
    m[3][0] = 0.0f;

    m[0][1] = 0.0f;
    m[1][1] = 1.0f;
    m[2][1] = 0.0f;
    m[3][1] = 0.0f;

    m[0][2] = (T) sin (angle);
    m[1][2] = 0.0f;
    m[2][2] = (T) cos (angle);
    m[3][2] = 0.0f;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::Rotate_z

  Description:    Add rotation matrix around axe Z.

  Parameters:     - angle

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix4x4<T>::Rotate_z (T angle)
  {
    Identity();

    m[0][0] = (T) cos (angle);
    m[1][0] = (T) sin (angle);
    m[2][0] = 0.0f;
    m[3][0] = 0.0f;

    m[0][1] = (T) - sin (angle);
    m[1][1] = (T) cos (angle);
    m[2][1] = 0.0f;
    m[3][1] = 0.0f;

    m[0][2] = 0.0f;
    m[1][2] = 0.0f;
    m[2][2] = 1.0f;
    m[3][2] = 0.0f;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
  }

  /***************************************************************************************\
  Function:       Matrix4x4<T>::Scale

  Description:    Add a scale matrix.

  Parameters:     - sx, sy, sz

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix4x4<T>::Scale (T sx, T sy, T sz)
  {
    Identity();

    m[0][0] = sx;
    m[1][0] = 0.0f;
    m[2][0] = 0.0f;
    m[3][0] = 0.0f;

    m[0][1] = 0.0f;
    m[1][1] = sy;
    m[2][1] = 0.0f;
    m[3][1] = 0.0f;

    m[0][2] = 0.0f;
    m[1][2] = 0.0f;
    m[2][2] = sz;
    m[3][2] = 0.0f;

    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
  }

  template <typename T>
  T Matrix4x4<T>::Trace() const
  {
    return m[0][0] + m[1][1] + m[2][2] + m[3][3];
  }

  template <typename T>
  T Matrix4x4<T>::Determinant() const
  {
    const T &m00 = m[0][0];
    const T &m01 = m[0][1];
    const T &m02 = m[0][2];
    const T &m03 = m[0][3];
    const T &m10 = m[1][0];
    const T &m11 = m[1][1];
    const T &m12 = m[1][2];
    const T &m13 = m[1][3];
    const T &m20 = m[2][0];
    const T &m21 = m[2][1];
    const T &m22 = m[2][2];
    const T &m23 = m[2][3];
    const T &m30 = m[3][0];
    const T &m31 = m[3][1];
    const T &m32 = m[3][2];
    const T &m33 = m[3][3];

    T det =
      m03 * m12 * m21 * m30 - m02 * m13 * m21 * m30 - m03 * m11 * m22 * m30 + m01 * m13 * m22 * m30 +
      m02 * m11 * m23 * m30 - m01 * m12 * m23 * m30 - m03 * m12 * m20 * m31 + m02 * m13 * m20 * m31 +
      m03 * m10 * m22 * m31 - m00 * m13 * m22 * m31 - m02 * m10 * m23 * m31 + m00 * m12 * m23 * m31 +
      m03 * m11 * m20 * m32 - m01 * m13 * m20 * m32 - m03 * m10 * m21 * m32 + m00 * m13 * m21 * m32 +
      m01 * m10 * m23 * m32 - m00 * m11 * m23 * m32 - m02 * m11 * m20 * m33 + m01 * m12 * m20 * m33 +
      m02 * m10 * m21 * m33 - m00 * m12 * m21 * m33 - m01 * m10 * m22 * m33 + m00 * m11 * m22 * m33;

    return det;
  }

  template <typename T>
  void Matrix4x4<T>::Inverse()
  {
    T det = Determinant();

    if (det == T (0) )
    {
      // Determinant is null. Matrix cannot be inverted.
#ifdef NUX_DEBUG
      NUX_HARDWARE_BREAK;
#endif
      return;
    }

    const T &m00 = m[0][0];

    const T &m01 = m[0][1];

    const T &m02 = m[0][2];

    const T &m03 = m[0][3];

    const T &m10 = m[1][0];

    const T &m11 = m[1][1];

    const T &m12 = m[1][2];

    const T &m13 = m[1][3];

    const T &m20 = m[2][0];

    const T &m21 = m[2][1];

    const T &m22 = m[2][2];

    const T &m23 = m[2][3];

    const T &m30 = m[3][0];

    const T &m31 = m[3][1];

    const T &m32 = m[3][2];

    const T &m33 = m[3][3];

    Matrix4x4<T> Temp;

    Temp.m[0][0] = m12 * m23 * m31 - m13 * m22 * m31 + m13 * m21 * m32 - m11 * m23 * m32 - m12 * m21 * m33 + m11 * m22 * m33;

    Temp.m[0][1] = m03 * m22 * m31 - m02 * m23 * m31 - m03 * m21 * m32 + m01 * m23 * m32 + m02 * m21 * m33 - m01 * m22 * m33;

    Temp.m[0][2] = m02 * m13 * m31 - m03 * m12 * m31 + m03 * m11 * m32 - m01 * m13 * m32 - m02 * m11 * m33 + m01 * m12 * m33;

    Temp.m[0][3] = m03 * m12 * m21 - m02 * m13 * m21 - m03 * m11 * m22 + m01 * m13 * m22 + m02 * m11 * m23 - m01 * m12 * m23;

    Temp.m[1][0] = m13 * m22 * m30 - m12 * m23 * m30 - m13 * m20 * m32 + m10 * m23 * m32 + m12 * m20 * m33 - m10 * m22 * m33;

    Temp.m[1][1] = m02 * m23 * m30 - m03 * m22 * m30 + m03 * m20 * m32 - m00 * m23 * m32 - m02 * m20 * m33 + m00 * m22 * m33;

    Temp.m[1][2] = m03 * m12 * m30 - m02 * m13 * m30 - m03 * m10 * m32 + m00 * m13 * m32 + m02 * m10 * m33 - m00 * m12 * m33;

    Temp.m[1][3] = m02 * m13 * m20 - m03 * m12 * m20 + m03 * m10 * m22 - m00 * m13 * m22 - m02 * m10 * m23 + m00 * m12 * m23;

    Temp.m[2][0] = m11 * m23 * m30 - m13 * m21 * m30 + m13 * m20 * m31 - m10 * m23 * m31 - m11 * m20 * m33 + m10 * m21 * m33;

    Temp.m[2][1] = m03 * m21 * m30 - m01 * m23 * m30 - m03 * m20 * m31 + m00 * m23 * m31 + m01 * m20 * m33 - m00 * m21 * m33;

    Temp.m[2][2] = m01 * m13 * m30 - m03 * m11 * m30 + m03 * m10 * m31 - m00 * m13 * m31 - m01 * m10 * m33 + m00 * m11 * m33;

    Temp.m[2][3] = m03 * m11 * m20 - m01 * m13 * m20 - m03 * m10 * m21 + m00 * m13 * m21 + m01 * m10 * m23 - m00 * m11 * m23;

    Temp.m[3][0] = m12 * m21 * m30 - m11 * m22 * m30 - m12 * m20 * m31 + m10 * m22 * m31 + m11 * m20 * m32 - m10 * m21 * m32;

    Temp.m[3][1] = m01 * m22 * m30 - m02 * m21 * m30 + m02 * m20 * m31 - m00 * m22 * m31 - m01 * m20 * m32 + m00 * m21 * m32;

    Temp.m[3][2] = m02 * m11 * m30 - m01 * m12 * m30 - m02 * m10 * m31 + m00 * m12 * m31 + m01 * m10 * m32 - m00 * m11 * m32;

    Temp.m[3][3] = m01 * m12 * m20 - m02 * m11 * m20 + m02 * m10 * m21 - m00 * m12 * m21 - m01 * m10 * m22 + m00 * m11 * m22;

    *this = (T (1) / det) * Temp;
  }

  template <typename T>
  Matrix4x4<T> Matrix4x4<T>::GetInverse() const
  {
    Matrix4x4<T> Temp = *this;
    Temp.Inverse();
    return Temp;
  }

// template <typename T>
// Matrix3x3<T> Matrix4x4<T>::GetUpper3x3() const
// {
//     Matrix3x3<T> Temp;
//     Temp.m[0][0] = m[0][0];
//     Temp.m[0][1] = m[0][1];
//     Temp.m[0][2] = m[0][2];
//
//     Temp.m[1][0] = m[1][0];
//     Temp.m[1][1] = m[1][1];
//     Temp.m[1][2] = m[1][2];
//
//     Temp.m[2][0] = m[2][0];
//     Temp.m[2][1] = m[2][1];
//     Temp.m[2][2] = m[2][2];
//
//     return Temp;
// }
//
// template <typename T>
// Matrix2x2<T> Matrix4x4<T>::GetUpper2x2() const
// {
//     Matrix2x2<T> Temp;
//     Temp.m[0][0] = m[0][0];
//     Temp.m[0][1] = m[0][1];
//
//     Temp.m[1][0] = m[1][0];
//     Temp.m[1][1] = m[1][1];
//
//     return Temp;
// }

// s 0 0 0
// 0 s 0 0
// 0 0 s 0
// 0 0 0 1

  template <typename T>
  void Matrix4x4<T>::Scale (T s)
  {
    m[0][0] = s;
    m[0][1] = 0.0;
    m[0][2] = 0.0;
    m[0][3] = 0.0;
    m[1][0] = 0.0;
    m[1][1] = s;
    m[1][2] = 0.0;
    m[1][3] = 0.0;
    m[2][0] = 0.0;
    m[2][1] = 0.0;
    m[2][2] = s;
    m[2][3] = 0.0;
    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = 1;
  }

// x 0 0 0
// 0 y 0 0
// 0 0 z 0
// 0 0 0 w

  template <typename T>
  void Matrix4x4<T>::Diagonal (T x, T y, T z, T w)
  {
    m[0][0] = x;
    m[0][1] = 0.0;
    m[0][2] = 0.0;
    m[0][3] = 0.0;
    m[1][0] = 0.0;
    m[1][1] = y;
    m[1][2] = 0.0;
    m[1][3] = 0.0;
    m[2][0] = 0.0;
    m[2][1] = 0.0;
    m[2][2] = z;
    m[2][3] = 0.0;
    m[3][0] = 0.0;
    m[3][1] = 0.0;
    m[3][2] = 0.0;
    m[3][3] = w;
  }


  template <typename T>
  void Matrix4x4<T>::Rotate (T angle, Vector3 axis)
  {
    //See Quaternion::from_angle_axis() and Quaternion::get_matrix()
    // note: adapted from david eberly's code without permission
    //TODO: make sure it is correct
    
    if (axis.LengthSquared() < constants::epsilon_micro)
    {
      Identity();
    }
    else
    {
      axis.Normalize();

      T fCos = (T) cos(angle);
      T fSin = (T) sin(angle);
      T fOneMinusCos = 1.0f-fCos;
      T fX2 = axis.x*axis.x;
      T fY2 = axis.y*axis.y;
      T fZ2 = axis.z*axis.z;
      T fXYM = axis.x*axis.y*fOneMinusCos;
      T fXZM = axis.x*axis.z*fOneMinusCos;
      T fYZM = axis.y*axis.z*fOneMinusCos;
      T fXSin = axis.x*fSin;
      T fYSin = axis.y*fSin;
      T fZSin = axis.z*fSin;

      m[0][0] = fX2*fOneMinusCos+fCos;
      m[0][1] = fXYM-fZSin;
      m[0][2] = fXZM+fYSin;
      m[0][3] = 0;

      m[1][0] = fXYM+fZSin;
      m[1][1] = fY2*fOneMinusCos+fCos;
      m[1][2] = fYZM-fXSin;
      m[1][3] = 0;

      m[2][0] = fXZM-fYSin;
      m[2][1] = fYZM+fXSin;
      m[2][2] = fZ2*fOneMinusCos+fCos;
      m[2][3] = 0;

      m[3][0] = 0;
      m[3][1] = 0;
      m[3][2] = 0;
      m[3][3] = 1;
    }
  }

  template <typename T>
  void Matrix4x4<T>::LookAt (const Vector3 &eye, const Vector3 &at, const Vector3 &up)
  {
    // left handed

    Vector3 z_axis = at - eye;
    Vector3 x_axis = z_axis.CrossProduct (up);
    Vector3 y_axis = x_axis.CrossProduct (z_axis);

    x_axis.Normalize();
    y_axis.Normalize();
    z_axis.Normalize();

    Matrix4x4<T> Rot;
    Rot.m[0][0]	= x_axis.x;
    Rot.m[0][1] = x_axis.y;
    Rot.m[0][2] = x_axis.z;
    Rot.m[0][3] = 0;

    Rot.m[1][0]	= y_axis.x;
    Rot.m[1][1] = y_axis.y;
    Rot.m[1][2] = y_axis.z;
    Rot.m[1][3] = 0;

    Rot.m[2][0]	= -z_axis.x;
    Rot.m[2][1] = -z_axis.y;
    Rot.m[2][2] = -z_axis.z;
    Rot.m[2][3] = 0;

    Rot.m[3][0]	= 0.0f;
    Rot.m[3][1] = 0.0f;
    Rot.m[3][2] = 0.0f;
    Rot.m[3][3] = 1.0f;

    Matrix4x4<T> Trans;
    Trans.Translate (-eye.x, -eye.y, -eye.z);

    *this = Rot * Trans;
  }

/// set to an orthographic projection matrix.

// 2/(r-l)  0           0           -(r+l)/(r-l)
// 0        2/(t-b)     0           -(t+b)/(t-b)
// 0        0           -2/(f-n)    -(f+n)/(f-n)
// 0        0           0           1
  template <typename T>
  void Matrix4x4<T>::Orthographic (T l, T r, T b, T t, T n, T f)
  {
    T sx = 1 / (r - l);
    T sy = 1 / (t - b);
    T sz = 1 / (f - n);

    m[0][0] = 2.0f * sx;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = - (r + l) * sx;

    m[1][0] = 0.0f;
    m[1][1] = 2.0f * sy;
    m[1][2] = 0.0f;
    m[1][3] = - (t + b) * sy;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -2.0f * sz;
    m[2][3] = - (f + n) * sz;

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
  }

/// set to a perspective projection matrix.
// 2*n/(r-l)    0           (r+l)/(r-l)     0
// 0            2*n/(t-b)   (t+b)/(t-b)     0
// 0            0           -(f+n)/(f-n)    -2*f*n/(f-n)
// 0            0           -1              0
  template <typename T>
  void Matrix4x4<T>::Perspective (T l, T r, T t, T b, T n, T f)
  {
    m[0][0]	= 2.0f * n / (r - l);
    m[0][1] = 0.0f;
    m[0][2] = (r + l) / (r - l);
    m[0][3] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = 2.0f * n / (t - b);
    m[1][2] = (t + b) / (t - b);
    m[1][3] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = - (f + n) / (f - n);
    m[2][3] = -2.0f * f * n / (f - n);

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = -1.0f;
    m[3][3] = 0.0f;
  }

// (r-l)/2*n    0           0               (r+l)/(2*n)
// 0            (t-b)/(2*n) 0               (t+b)/(2*n)
// 0            0           0               -1
// 0            0           -(f-n)/(2*f*n)  (f+n)/(2*f*n)
  template <typename T>
  void Matrix4x4<T>::PerspectiveInverse (T l, T r, T t, T b, T n, T f)
  {
    m[0][0]	= (r - l) / (2.0f * n);
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = (r + l) / (2.0f * n);

    m[1][0] = 0;
    m[1][1] = (t - b) / (2.0f * n);
    m[1][2] = (t + b) / (2.0f * n);
    m[1][3] = 0;

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 0;
    m[2][3] = -1;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = - (f - n) / (2.0f * f * n);
    m[3][3] = (f + n) / (2.0f * f * n);
  }

/// set to a perspective projection matrix specified in terms of field of view and aspect ratio.
  template <typename T>
  void Matrix4x4<T>::Perspective (T FoV, T AspectRatio, T NearPlane, T FarPlane)
  {
    const T t = tan (FoV * 0.5f) * NearPlane;
    const T b = -t;

    const T l = AspectRatio * b;
    const T r = AspectRatio * t;

    Perspective (l, r, t, b, NearPlane, FarPlane);
  }

  template <typename T>
  Matrix4x4<T> Matrix4x4<T>::IDENTITY()
  {
    Matrix4x4<T> matrix;
    matrix.Identity();
    return matrix;
  }

  template <typename T>
  Matrix4x4<T> Matrix4x4<T>::ZERO()
  {
    Matrix4x4<T> matrix;
    matrix.Zero();
    return matrix;
  }

  template <typename T>
  Matrix4x4<T> Matrix4x4<T>::ROTATEX(T angle)
  {
    Matrix4x4<T> matrix;
    matrix.Rotate_x(angle);
    return matrix;
  }

  template <typename T>
  Matrix4x4<T> Matrix4x4<T>::ROTATEY(T angle)
  {
    Matrix4x4<T> matrix;
    matrix.Rotate_y(angle);
    return matrix;
  }

  template <typename T>
  Matrix4x4<T> Matrix4x4<T>::ROTATEZ(T angle)
  {
    Matrix4x4<T> matrix;
    matrix.Rotate_z(angle);
    return matrix;
  }

  template <typename T>
  Matrix4x4<T> Matrix4x4<T>::TRANSLATE(T x, T y, T z)
  {
    Matrix4x4<T> matrix;
    matrix.Translate(x, y, z);
    return matrix;
  }

  template <typename T>
  Matrix4x4<T> Matrix4x4<T>::SCALE(T x, T y, T z)
  {
    Matrix4x4<T> matrix;
    matrix.Scale(x, y, z);
    return matrix;
  }

  /***************************************************************************************\
  Function:       operator *

  Description:    Multiply matrix rhs by constant lhs.
                  Allow "f * matrix" operation.

  Parameters:     None.

  Return Value:   Matrix4.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix4x4<T> operator * (const T &lhs, const Matrix4x4<T>& rhs)
  {
    Matrix4x4<T> oM;

    oM.m[0][0] = rhs.m[0][0] * lhs;
    oM.m[0][1] = rhs.m[0][1] * lhs;
    oM.m[0][2] = rhs.m[0][2] * lhs;
    oM.m[0][3] = rhs.m[0][3] * lhs;
    oM.m[1][0] = rhs.m[1][0] * lhs;
    oM.m[1][1] = rhs.m[1][1] * lhs;
    oM.m[1][2] = rhs.m[1][2] * lhs;
    oM.m[1][3] = rhs.m[1][3] * lhs;
    oM.m[2][0] = rhs.m[2][0] * lhs;
    oM.m[2][1] = rhs.m[2][1] * lhs;
    oM.m[2][2] = rhs.m[2][2] * lhs;
    oM.m[2][3] = rhs.m[2][3] * lhs;
    oM.m[3][0] = rhs.m[3][0] * lhs;
    oM.m[3][1] = rhs.m[3][1] * lhs;
    oM.m[3][2] = rhs.m[3][2] * lhs;
    oM.m[3][3] = rhs.m[3][3] * lhs;

    return oM;
  }

  typedef Matrix4x4<float> Matrix4;

}


#endif // MATRIX4_H

