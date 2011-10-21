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


#ifndef MATRIX2_H
#define MATRIX2_H


#include "Vector2.h"

namespace nux
{

  template <typename T>
  class Matrix2x2
  {
  public:
    Matrix2x2<T>();
    ~Matrix2x2<T>();

    Matrix2x2<T> (const Matrix2x2<T>&);
    Matrix2x2<T>&   operator = (const Matrix2x2<T>&);
    bool          operator == (const Matrix2x2<T>&);
    Matrix2x2<T>    operator * (const Matrix2x2<T>&) const;
    Matrix2x2<T>    operator + (const Matrix2x2<T>&) const;
    Matrix2x2<T>    operator - (const Matrix2x2<T>&) const;
    Matrix2x2<T>&   operator *= (const Matrix2x2<T>&) const;
    Matrix2x2<T>&   operator += (const Matrix2x2<T>&) const;
    Matrix2x2<T>&   operator -= (const Matrix2x2<T>&) const;

    Matrix2x2<T>    operator * (const T &) const;
    Matrix2x2<T>    operator / (const T &) const;
    Matrix2x2<T>&   operator *= (const T &) const;
    Matrix2x2<T>&   operator /= (const T &) const;

    Vec2<T>         operator * (const Vec2<T>&) const;
    Matrix2x2<T>    operator - ();

    // Get the (i, j) element of the current matrix.
    T &operator() (unsigned int i, unsigned int j);
    T operator () (unsigned int i, unsigned int j) const;

    void Zero();
    void Identity();
    T Determinant() const ;
    void Inverse();
    Matrix2x2<T> GetInverse() const;

    static Matrix2x2<T> IDENTITY();
    static Matrix2x2<T> ZERO();

    T m[2][2];
  };


  /***************************************************************************************\
  Function:       Matrix2::Matrix2

  Description:    Constructor. Initialize the matrix to identity.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T>::Matrix2x2()
  {
    Identity();
  }

  template <typename T>
  T Matrix2x2<T>::Determinant() const
  {
    T det;
    det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
    return det;
  }

  template <typename T>
  void Matrix2x2<T>::Inverse()
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

    Matrix2x2<T> Temp;
    Temp.m[0][0] = m[1][1];
    Temp.m[0][1] = -m[0][1];
    Temp.m[1][0] = -m[1][0];
    Temp.m[1][1] = m[0][0];

    *this = (T (1) / det) * Temp;
  }

  template <typename T>
  Matrix2x2<T> Matrix2x2<T>::GetInverse() const
  {
    Matrix2x2<T> Temp = *this;
    Temp.Inverse();
    return Temp;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::~Matrix2

  Description:    Destructor.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T>::~Matrix2x2()
  {

  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::Matrix2

  Description:    Copy constructor.

  Parameters:     - M

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T>::Matrix2x2 (const Matrix2x2<T>& M)
  {
    m[0][0] = M.m[0][0];
    m[0][1] = M.m[0][1];
    m[1][0] = M.m[1][0];
    m[1][1] = M.m[1][1];
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator =

  Description:    Assignment operator.

  Parameters:     - M

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T>& Matrix2x2<T>::operator = (const Matrix2x2<T>& M)
  {
    m[0][0] = M.m[0][0];
    m[0][1] = M.m[0][1];
    m[1][0] = M.m[1][0];
    m[1][1] = M.m[1][1];

    return (*this);
  }

  template <typename T>
  bool Matrix2x2<T>::operator == (const Matrix2x2<T>& M)
  {
    for (int i = 0; i < 2; i++)
      for (int j = 0; j < 2; j++)
      {
        if (m[i][j] != M.m[i][j])
          return false;
      }

    return true;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator *

  Description:    Multiply by matrix iM.

  Parameters:     - iM.

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T> Matrix2x2<T>::operator * (const Matrix2x2<T>& iM) const
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = m[0][0] * iM.m[0][0] + m[0][1] * iM.m[1][0];
    oM.m[1][0] = m[1][0] * iM.m[0][0] + m[1][1] * iM.m[1][0];

    oM.m[0][1] = m[0][0] * iM.m[0][1] + m[0][1] * iM.m[1][1];
    oM.m[1][1] = m[1][0] * iM.m[0][1] + m[1][1] * iM.m[1][1];

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator +

  Description:    Add matrix iM.

  Parameters:     - iM

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T> Matrix2x2<T>::operator + (const Matrix2x2<T>& iM) const
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = m[0][0] + iM.m[0][0];
    oM.m[0][1] = m[0][1] + iM.m[0][1];
    oM.m[1][0] = m[1][0] + iM.m[1][0];
    oM.m[1][1] = m[1][1] + iM.m[1][1];

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator -

  Description:    Substract matrix iM.

  Parameters:      -iM

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T> Matrix2x2<T>::operator - (const Matrix2x2<T>& iM) const
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = m[0][0] - iM.m[0][0];
    oM.m[0][1] = m[0][1] - iM.m[0][1];
    oM.m[1][0] = m[1][0] - iM.m[1][0];
    oM.m[1][1] = m[1][1] - iM.m[1][1];

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator *=

  Description:    Multiply by matrix iM.

  Parameters:     - iM.

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T>& Matrix2x2<T>::operator *= (const Matrix2x2<T>& iM) const
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = m[0][0] * iM.m[0][0] + m[0][1] * iM.m[1][0];
    oM.m[1][0] = m[1][0] * iM.m[0][0] + m[1][1] * iM.m[1][0];

    oM.m[0][1] = m[0][0] * iM.m[0][1] + m[0][1] * iM.m[1][1];
    oM.m[1][1] = m[1][0] * iM.m[0][1] + m[1][1] * iM.m[1][1];

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator +=

  Description:    Add matrix iM.

  Parameters:     - iM

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T>& Matrix2x2<T>::operator += (const Matrix2x2<T>& iM) const
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = m[0][0] + iM.m[0][0];
    oM.m[0][1] = m[0][1] + iM.m[0][1];
    oM.m[1][0] = m[1][0] + iM.m[1][0];
    oM.m[1][1] = m[1][1] + iM.m[1][1];

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator -=

  Description:    Substract matrix iM.

  Parameters:      -iM

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T>& Matrix2x2<T>::operator -= (const Matrix2x2<T>& iM) const
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = m[0][0] - iM.m[0][0];
    oM.m[0][1] = m[0][1] - iM.m[0][1];
    oM.m[1][0] = m[1][0] - iM.m[1][0];
    oM.m[1][1] = m[1][1] - iM.m[1][1];

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator *=

  Description:    Multiply all elements by f.

  Parameters:     - f.

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T>& Matrix2x2<T>::operator *= (const T &f) const
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = m[0][0] * f;
    oM.m[0][1] = m[0][1] * f;
    oM.m[1][0] = m[1][0] * f;
    oM.m[1][1] = m[1][1] * f;

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator /

  Description:    Divide all elements by f.

  Parameters:     - f

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T> Matrix2x2<T>::operator / (const T &f) const
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = m[0][0] / f;
    oM.m[0][1] = m[0][1] / f;
    oM.m[1][0] = m[1][0] / f;
    oM.m[1][1] = m[1][1] / f;

    return oM;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator /=

  Description:    Divide all elements by f.

  Parameters:     - f

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T>& Matrix2x2<T>::operator /= (const T &f) const
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = m[0][0] / f;
    oM.m[0][1] = m[0][1] / f;
    oM.m[1][0] = m[1][0] / f;
    oM.m[1][1] = m[1][1] / f;

    *this = oM;
    return *this;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator *

  Description:    Multiply a matrix by a vector.

  Parameters:     - V

  Return Value:   Vector2.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Vec2<T> Matrix2x2<T>::operator * (const Vec2<T>& V) const
  {
    Vec2<T> oV;

    oV.x = V.x * m[0][0] + V.y * m[0][1];
    oV.y = V.x * m[1][0] + V.y * m[1][1];

    return oV;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator -

  Description:    Negate all elements of the matrix.

  Parameters:     None.

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T> Matrix2x2<T>::operator - ()
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = -m[0][0];
    oM.m[0][1] = -m[0][1];
    oM.m[1][0] = -m[1][0];
    oM.m[1][1] = -m[1][1];

    return oM;
  }

  template <typename T>
  T &Matrix2x2<T>::operator () (unsigned int i, unsigned int j)
  {
    return m[i][j];
  }

  template <typename T>
  T Matrix2x2<T>::operator () (unsigned int i, unsigned int j) const
  {
    return m[i][j];
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::zero

  Description:    Set the matrix to zero.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix2x2<T>::Zero()
  {
    m[0][0] = 0.0;
    m[0][1] = 0.0;
    m[1][0] = 0.0;
    m[1][1] = 0.0;

    //memset(m, 0, sizeof(m));
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::identity

  Description:    Set the matrix to identity.

  Parameters:     None.

  Return Value:   None.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  void Matrix2x2<T>::Identity()
  {
    m[0][0] = 1.0;
    m[0][1] = 0.0;
    m[1][0] = 0.0;
    m[1][1] = 1.0;
  }

  template <typename T>
  Matrix2x2<T> Matrix2x2<T>::IDENTITY()
  {
    Matrix2x2<T> matrix;
    matrix.Identity();
    return matrix;
  }

  template <typename T>
  Matrix2x2<T> Matrix2x2<T>::ZERO()
  {
    Matrix2x2<T> matrix;
    matrix.Zero();
    return matrix;
  }

  /***************************************************************************************\
  Function:       Matrix2x2<T>::operator *

  Description:    Multiply matrix rhs by constant lhs.
  Allow "f * matrix" operation.

  Parameters:     None.

  Return Value:   Matrix2x2<T>.

  Comments:       None.
  \***************************************************************************************/
  template <typename T>
  Matrix2x2<T> operator * (const T &lhs, const Matrix2x2<T>& rhs)
  {
    Matrix2x2<T> oM;

    oM.m[0][0] = rhs.m[0][0] / lhs;
    oM.m[0][1] = rhs.m[0][1] / lhs;
    oM.m[1][0] = rhs.m[1][0] / lhs;
    oM.m[1][1] = rhs.m[1][1] / lhs;

    return oM;
  }

}


#endif // MATRIX2_H

