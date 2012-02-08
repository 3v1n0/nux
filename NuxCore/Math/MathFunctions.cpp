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


#include "../NuxCore.h"
#include "MathFunctions.h"


namespace nux
{

  int Factorial (int n)
  {
    int i = 1;

    while (0 < n)
    {
      i *= n;
      n--;
    }

    return i;
  }

  double BinomialCoefficient (int n, int k)
  {
    if (n < 0)
      NUX_BREAK_ASM_INT3;

    if (k < 0 || k > n)
      return 0.0;

    double d = (double) Factorial (n) / (double) (Factorial (n - k) * Factorial (k) );
    return d;
  }

  double Power (double x, double y)
  {
    return std::pow (x, y);
  }

  double Log2 (double d)
  {
    return std::log (d) / std::log (2.0);
  }

  double Floor (double d)
  {
    return std::floor (d);
  }

}
