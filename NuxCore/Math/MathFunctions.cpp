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


#include "../NuxCore.h"
#include "MathFunctions.h"


namespace nux
{

  t_int Factorial (t_int n)
  {
    t_int i = 1;

    while (0 < n)
    {
      i *= n;
      n--;
    }

    return i;
  }

  t_double BinomialCoefficient (t_int n, t_int k)
  {
    if (n < 0)
      NUX_BREAK_ASM_INT3;

    if (k < 0 || k > n)
      return 0.0;

    t_double d = (t_double) Factorial (n) / (t_double) (Factorial (n - k) * Factorial (k) );
    return d;
  }

  t_double Power (t_double x, t_double y)
  {
    return std::pow (x, y);
  }

  t_double Log2 (t_double d)
  {
    return std::log (d) / std::log (2.0);
  }

  t_double Floor (t_double d)
  {
    return std::floor (d);
  }

}