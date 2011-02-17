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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include "Constants.h"

namespace nux
{

  t_int Factorial (t_int n);
  t_double BinomialCoefficient (t_int n, t_int k);


//! Template power function. The exponent is an integer.
  /*!
      Template power function. The exponent is an integer.
  */

  template<class T> inline
  T PowerInt (T _X, int _Y)   // taken from Microsoft <math.h>
  {
    unsigned int _N;

    if (_Y >= 0)
      _N = _Y;
    else
      _N = -_Y;

    for (T _Z = T (1); ; _X *= _X)
    {
      if ( (_N & 1) != 0)
        _Z *= _X;

      if ( (_N >>= 1) == 0)
        return (_Y < 0 ? T (1) / _Z : _Z);
    }
  }

  t_double Power (t_double x, t_double y);

  t_double Log2 (t_double d);
  t_double Floor (t_double d);


}

#endif // MATHFUNCTIONS_H

