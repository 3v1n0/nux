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


#ifndef MATRIX3_H
#define MATRIX3_H


#include "Vector3.h"

namespace nux
{

  template<typename T>
  class Matrix3x3
  {
  public:
    Matrix3x3<T>();
    ~Matrix3x3();

    Matrix3x3 (const Matrix3x3<T>&);
    Matrix3x3<T> (
      T a00, T a01, T a02,
      T a10, T a11, T a12,
      T a20, T a21, T a22);

    Matrix3x3<T>&   operator = (const Matrix3x3<T>&);
    bool          operator == (const Matrix3x3<T>&);
    Matrix3x3<T>    operator * (const Matrix3x3<T>&) const;
    Matrix3x3<T>    operator + (const Matrix3x3<T>&) const;
    Matrix3x3<T>    operator - (const Matrix3x3<T>&) const;
    Matrix3x3<T>&   operator *= (const Matrix3x3<T>&) const;
    Matrix3x3<T>&   operator += (const Matrix3x3<T>&) const;
    Matrix3x3<T>&   operator -= (const Matrix3x3<T>&) const;

    Matrix3x3<T>    operator * (const T &) const;
    Matrix3x3<T>    operator / (const T &) const;
    Matrix3x3<T>&   operator *= (const T &) const;
    Matrix3x3<T>&   operator /= (const T &) const;

    Vec3<T>         operator * (const Vec3<T>&) const;
    Matrix3x3<T>    operator - ();

    // Get the (i, j) element of the current matrix.
    T &operator() (unsigned int i, unsigned int j);
    T operator () (unsigned int i, unsigned int j) const;

    T Determinant() const ;
    void Inverse();
    Matrix3x3<T> GetInverse() const;

    //Matrix2x2<T> GetUpper2x2() const;

    void Zero();
    void Identity();

    static Matrix3x3<T> IDENTITY();
    static Matrix3x3<T> ZERO();
    T m[3][3];
  };


  /***************************************************************************************\
  Function:       Matrix3x3<T>::Matrix3x3

  Description:    Constructor. Initialize the matrix to identity.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T>::Matrix3x3()
  {
    Identity();
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::~Matrix3x3

  Description:    Destructor.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T>::~Matrix3x3()
  {

  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::Matrix3x3

  Description:    None.

  Parameters:     - M

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T>::Matrix3x3 (const Matrix3x3<T>& M)
  {
    m[0][0] = M.m[0][0];
    m[0][1] = M.m[0][1];
    m[0][2] = M.m[0][2];
    m[1][0] = M.m[1][0];
    m[1][1] = M.m[1][1];
    m[1][2] = M.m[1][2];
    m[2][0] = M.m[2][0];
    m[2][1] = M.m[2][1];
    m[2][2] = M.m[2][2];
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::Matrix3x3

  Description:    None.

  Parameters:     T a00, T a01, T a02,
                  T a10, T a11, T a12,
                  T a20, T a21, T a22

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template<typename T>
  Matrix3x3<T>::Matrix3x3 (
    T a00, T a01, T a02,
    T a10, T a11, T a12,
    T a20, T a21, T a22)
  {
    m[0][0] = a00;
    m[0][1] = a01;
    m[0][2] = a02;
    m[1][0] = a10;
    m[1][1] = a11;
    m[1][2] = a12;
    m[2][0] = a20;
    m[2][1] = a21;
    m[2][2] = a22;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator =

  Description:    None.

  Parameters:     - M

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T>& Matrix3x3<T>::operator = (const Matrix3x3<T>& M)
  {
    m[0][0] = M.m[0][0];
    m[0][1] = M.m[0][1];
    m[0][2] = M.m[0][2];
    m[1][0] = M.m[1][0];
    m[1][1] = M.m[1][1];
    m[1][2] = M.m[1][2];
    m[2][0] = M.m[2][0];
    m[2][1] = M.m[2][1];
    m[2][2] = M.m[2][2];

    return (*this);
  }

  template <typename T>
  bool Matrix3x3<T>::operator == (const Matrix3x3<T>& M)
  {
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++)
      {
        if (m[i][j] != M.m[i][j])
          return false;
      }

    return true;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator *

  Description:    Multiply by matrix iM.

  Parameters:     - iM

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T> Matrix3x3<T>::operator * (const Matrix3x3<T>& iM) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] * iM.m[0][0] + m[0][1] * iM.m[1][0] + m[0][2] * iM.m[2][0];
    oM.m[1][0] = m[1][0] * iM.m[0][0] + m[1][1] * iM.m[1][0] + m[1][2] * iM.m[2][0];
    oM.m[2][0] = m[2][0] * iM.m[0][0] + m[2][1] * iM.m[1][0] + m[2][2] * iM.m[2][0];

    oM.m[0][1] = m[0][0] * iM.m[0][1] + m[0][1] * iM.m[1][1] + m[0][2] * iM.m[2][1];
    oM.m[1][1] = m[1][0] * iM.m[0][1] + m[1][1] * iM.m[1][1] + m[1][2] * iM.m[2][1];
    oM.m[2][1] = m[2][0] * iM.m[0][1] + m[2][1] * iM.m[1][1] + m[2][2] * iM.m[2][1];

    oM.m[0][2] = m[0][0] * iM.m[0][2] + m[0][1] * iM.m[1][2] + m[0][2] * iM.m[2][2];
    oM.m[1][2] = m[1][0] * iM.m[0][2] + m[1][1] * iM.m[1][2] + m[1][2] * iM.m[2][2];
    oM.m[2][2] = m[2][0] * iM.m[0][2] + m[2][1] * iM.m[1][2] + m[2][2] * iM.m[2][2];

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator +

  Description:    Add matrix iM.

  Parameters:     - iM

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T> Matrix3x3<T>::operator+ (const Matrix3x3<T>& iM) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] + iM.m[0][0];
    oM.m[0][1] = m[0][1] + iM.m[0][1];
    oM.m[0][2] = m[0][2] + iM.m[0][2];
    oM.m[1][0] = m[1][0] + iM.m[1][0];
    oM.m[1][1] = m[1][1] + iM.m[1][1];
    oM.m[1][2] = m[1][2] + iM.m[1][2];
    oM.m[2][0] = m[2][0] + iM.m[2][0];
    oM.m[2][1] = m[2][1] + iM.m[2][1];
    oM.m[2][2] = m[2][2] + iM.m[2][2];

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator -

  Description:    Substract matrix iM.

  Parameters:     - iM

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T> Matrix3x3<T>::operator- (const Matrix3x3<T>& iM) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] - iM.m[0][0];
    oM.m[0][1] = m[0][1] - iM.m[0][1];
    oM.m[0][2] = m[0][2] - iM.m[0][2];
    oM.m[1][0] = m[1][0] - iM.m[1][0];
    oM.m[1][1] = m[1][1] - iM.m[1][1];
    oM.m[1][2] = m[1][2] - iM.m[1][2];
    oM.m[2][0] = m[2][0] - iM.m[2][0];
    oM.m[2][1] = m[2][1] - iM.m[2][1];
    oM.m[2][2] = m[2][2] - iM.m[2][2];

    return oM;
  }







  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator *=

  Description:    Multiply by matrix iM.

  Parameters:     - iM

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T>& Matrix3x3<T>::operator *= (const Matrix3x3<T>& iM) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] * iM.m[0][0] + m[0][1] * iM.m[1][0] + m[0][2] * iM.m[2][0];
    oM.m[1][0] = m[1][0] * iM.m[0][0] + m[1][1] * iM.m[1][0] + m[1][2] * iM.m[2][0];
    oM.m[2][0] = m[2][0] * iM.m[0][0] + m[2][1] * iM.m[1][0] + m[2][2] * iM.m[2][0];

    oM.m[0][1] = m[0][0] * iM.m[0][1] + m[0][1] * iM.m[1][1] + m[0][2] * iM.m[2][1];
    oM.m[1][1] = m[1][0] * iM.m[0][1] + m[1][1] * iM.m[1][1] + m[1][2] * iM.m[2][1];
    oM.m[2][1] = m[2][0] * iM.m[0][1] + m[2][1] * iM.m[1][1] + m[2][2] * iM.m[2][1];

    oM.m[0][2] = m[0][0] * iM.m[0][2] + m[0][1] * iM.m[1][2] + m[0][2] * iM.m[2][2];
    oM.m[1][2] = m[1][0] * iM.m[0][2] + m[1][1] * iM.m[1][2] + m[1][2] * iM.m[2][2];
    oM.m[2][2] = m[2][0] * iM.m[0][2] + m[2][1] * iM.m[1][2] + m[2][2] * iM.m[2][2];

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator +=

  Description:    Add matrix iM.

  Parameters:     - iM

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T>& Matrix3x3<T>::operator += (const Matrix3x3<T>& iM) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] + iM.m[0][0];
    oM.m[0][1] = m[0][1] + iM.m[0][1];
    oM.m[0][2] = m[0][2] + iM.m[0][2];
    oM.m[1][0] = m[1][0] + iM.m[1][0];
    oM.m[1][1] = m[1][1] + iM.m[1][1];
    oM.m[1][2] = m[1][2] + iM.m[1][2];
    oM.m[2][0] = m[2][0] + iM.m[2][0];
    oM.m[2][1] = m[2][1] + iM.m[2][1];
    oM.m[2][2] = m[2][2] + iM.m[2][2];

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator -=

  Description:    Substract matrix iM.

  Parameters:     - iM

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T>& Matrix3x3<T>::operator -= (const Matrix3x3<T>& iM) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] - iM.m[0][0];
    oM.m[0][1] = m[0][1] - iM.m[0][1];
    oM.m[0][2] = m[0][2] - iM.m[0][2];
    oM.m[1][0] = m[1][0] - iM.m[1][0];
    oM.m[1][1] = m[1][1] - iM.m[1][1];
    oM.m[1][2] = m[1][2] - iM.m[1][2];
    oM.m[2][0] = m[2][0] - iM.m[2][0];
    oM.m[2][1] = m[2][1] - iM.m[2][1];
    oM.m[2][2] = m[2][2] - iM.m[2][2];

    *this = oM;
    return *this;
  }


  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator *

  Description:    Multiply all elements by f.

  Parameters:     - f

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T> Matrix3x3<T>::operator * (const T &f) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] * f;
    oM.m[0][1] = m[0][1] * f;
    oM.m[0][2] = m[0][2] * f;
    oM.m[1][0] = m[1][0] * f;
    oM.m[1][1] = m[1][1] * f;
    oM.m[1][2] = m[1][2] * f;
    oM.m[2][0] = m[2][0] * f;
    oM.m[2][1] = m[2][1] * f;
    oM.m[2][2] = m[2][2] * f;

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator /

  Description:    Divide all elements by f.

  Parameters:     - f

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T> Matrix3x3<T>::operator/ (const T &f) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] / f;
    oM.m[0][1] = m[0][1] / f;
    oM.m[0][2] = m[0][2] / f;
    oM.m[1][0] = m[1][0] / f;
    oM.m[1][1] = m[1][1] / f;
    oM.m[1][2] = m[1][2] / f;
    oM.m[2][0] = m[2][0] / f;
    oM.m[2][1] = m[2][1] / f;
    oM.m[2][2] = m[2][2] / f;

    return oM;
  }





  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator *=

  Description:    Multiply all elements by f.

  Parameters:     - f

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T>& Matrix3x3<T>::operator *= (const T &f) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] * f;
    oM.m[0][1] = m[0][1] * f;
    oM.m[0][2] = m[0][2] * f;
    oM.m[1][0] = m[1][0] * f;
    oM.m[1][1] = m[1][1] * f;
    oM.m[1][2] = m[1][2] * f;
    oM.m[2][0] = m[2][0] * f;
    oM.m[2][1] = m[2][1] * f;
    oM.m[2][2] = m[2][2] * f;

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator /=

  Description:    Divide all elements by f.

  Parameters:     - f

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T>& Matrix3x3<T>::operator /= (const T &f) const
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = m[0][0] / f;
    oM.m[0][1] = m[0][1] / f;
    oM.m[0][2] = m[0][2] / f;
    oM.m[1][0] = m[1][0] / f;
    oM.m[1][1] = m[1][1] / f;
    oM.m[1][2] = m[1][2] / f;
    oM.m[2][0] = m[2][0] / f;
    oM.m[2][1] = m[2][1] / f;
    oM.m[2][2] = m[2][2] / f;

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator *

  Description:    Multiply a matrix by a vector.

  Parameters:     - V

  Return Value:   Vector4.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Vec3<T> Matrix3x3<T>::operator * (const Vec3<T>& V) const
  {
    Vec3<T> oV;

    oV.x = V.x * m[0][0] + V.y * m[0][1] + V.z * m[0][2];
    oV.y = V.x * m[1][0] + V.y * m[1][1] + V.z * m[1][2];
    oV.z = V.x * m[2][0] + V.y * m[2][1] + V.z * m[2][2];

    return oV;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator - ()

  Description:    Negate all elements of the matrix.

  Parameters:     None.

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T> Matrix3x3<T>::operator- ()
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = -m[0][0];
    oM.m[0][1] = -m[0][1];
    oM.m[0][2] = -m[0][2];
    oM.m[1][0] = -m[1][0];
    oM.m[1][1] = -m[1][1];
    oM.m[1][2] = -m[1][2];
    oM.m[2][0] = -m[2][0];
    oM.m[2][1] = -m[2][1];
    oM.m[2][2] = -m[2][2];

    return oM;
  }

  template <typename T>
  T &Matrix3x3<T>::operator () (unsigned int i, unsigned int j)
  {
    return m[i][j];
  }

  template <typename T>
  T Matrix3x3<T>::operator () (unsigned int i, unsigned int j) const
  {
    return m[i][j];
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::zero

  Description:    Set the matrix to zero.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix3x3<T>::Zero()
  {
    m[0][0] = 0.0;
    m[0][1] = 0.0;
    m[0][2] = 0.0;
    m[1][0] = 0.0;
    m[1][1] = 0.0;
    m[1][2] = 0.0;
    m[2][0] = 0.0;
    m[2][1] = 0.0;
    m[2][2] = 0.0;

    //memset(m, 0, sizeof(m));
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::identity

  Description:    Set the matrix to identity.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix3x3<T>::Identity()
  {
    m[0][0] = 1.0;
    m[0][1] = 0.0;
    m[0][2] = 0.0;
    m[1][0] = 0.0;
    m[1][1] = 1.0;
    m[1][2] = 0.0;
    m[2][0] = 0.0;
    m[2][1] = 0.0;
    m[2][2] = 1.0;
  }

  template <typename T>
  T Matrix3x3<T>::Determinant() const
  {
    T det;
    det = m[0][0] * m[1][1] * m[2][2] +
          m[0][1] * m[1][2] * m[2][0] +
          m[0][2] * m[2][0] * m[2][1] -
          m[0][0] * m[1][2] * m[2][1] -
          m[0][1] * m[1][0] * m[2][2] -
          m[0][2] * m[1][1] * m[2][0];

    return det;
  }

  template <typename T>
  void Matrix3x3<T>::Inverse()
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

    Matrix3x3<T> Temp;
    Temp.m[0][0] = m[1][1] * m[2][2] - m[1][2] * m[2][1];
    Temp.m[0][1] = m[0][2] * m[2][1] - m[0][1] * m[2][2];
    Temp.m[0][2] = m[0][1] * m[1][2] - m[0][2] * m[1][1];

    Temp.m[1][0] = m[1][2] * m[2][0] - m[1][0] * m[2][2];
    Temp.m[1][1] = m[0][0] * m[2][2] - m[0][2] * m[2][0];
    Temp.m[1][2] = m[0][2] * m[1][0] - m[0][0] * m[1][2];

    Temp.m[2][0] = m[1][0] * m[2][1] - m[1][1] * m[2][0];
    Temp.m[2][1] = m[0][1] * m[2][0] - m[0][0] * m[2][1];
    Temp.m[2][2] = m[0][0] * m[1][1] - m[0][1] * m[1][0];

    *this = (T (1) / det) * Temp;
  }

  template <typename T>
  Matrix3x3<T> Matrix3x3<T>::GetInverse() const
  {
    Matrix3x3<T> Temp = *this;
    Temp.Inverse();
    return Temp;
  }

// template <typename T>
// Matrix2x2<T> Matrix3x3<T>::GetUpper2x2() const
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

  template <typename T>
  Matrix3x3<T> Matrix3x3<T>::IDENTITY()
  {
    Matrix3x3<T> matrix;
    matrix.Identity();
    return matrix;
  }

  template <typename T>
  Matrix3x3<T> Matrix3x3<T>::ZERO()
  {
    Matrix3x3<T> matrix;
    matrix.Zero();
    return matrix;
  }

  /***************************************************************************************\
  Function:       Matrix3x3<T>::operator *

  Description:    Multiply matrix rhs by constant lhs.
  Allow "f * matrix" operation..

  Parameters:     None.

  Return Value:   Matrix3x3<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix3x3<T> operator * (const T &lhs, const Matrix3x3<T>& rhs)
  {
    Matrix3x3<T> oM;

    oM.m[0][0] = rhs.m[0][0] / lhs;
    oM.m[0][1] = rhs.m[0][1] / lhs;
    oM.m[0][2] = rhs.m[0][2] / lhs;
    oM.m[1][0] = rhs.m[1][0] / lhs;
    oM.m[1][1] = rhs.m[1][1] / lhs;
    oM.m[1][2] = rhs.m[1][2] / lhs;
    oM.m[2][0] = rhs.m[2][0] / lhs;
    oM.m[2][1] = rhs.m[2][1] / lhs;
    oM.m[2][2] = rhs.m[2][2] / lhs;

    return oM;
  }

  typedef Matrix3x3<float> Matrix3;

}


#endif // MATRIX3_H

