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
#include "Bezier.h"

namespace nux
{

  NUX_DECLSPEC_DLL double *Bernstein (int n, double t)
  {
    if (n < 0)
    {
      NUX_BREAK_ASM_INT3;
    }

    double *bernstein;
    int i;

    bernstein = new double[n+1];

    for (i = 0; i <= n; i++ )
    {
      bernstein[i] = BinomialCoefficient (n, i) * PowerInt<double> (t, i) * PowerInt<double> (1.0 - t, n - i);
    }

    return bernstein;
  }

  NUX_DECLSPEC_DLL void Bezier_XY (int n, double t, double xcon[], double ycon[], double *xval, double *yval)
  {
    if (n < 0)
    {
      NUX_BREAK_ASM_INT3;
    }

    double *bval;
    int i;

    bval = Bernstein ( n, t );

    *xval = 0.0;

    for ( i = 0; i <= n; i++ )
      *xval = *xval + xcon[i] * bval[i];

    *yval = 0.0;

    for ( i = 0; i <= n; i++ )
      *yval = *yval + ycon[i] * bval[i];

    delete [] bval;
  }

  NUX_DECLSPEC_DLL void Bezier_XYZ (int n, double t, double xcon[], double ycon[], double zcon[], double *xval, double *yval, double *zval)
  {
    if (n < 0)
      NUX_BREAK_ASM_INT3;

    double *bval;
    int i;

    bval = Bernstein ( n, t );

    *xval = 0.0;

    for ( i = 0; i <= n; i++ )
      *xval = *xval + xcon[i] * bval[i];

    *yval = 0.0;

    for ( i = 0; i <= n; i++ )
      *yval = *yval + ycon[i] * bval[i];

    *zval = 0.0;

    for ( i = 0; i <= n; i++ )
      *zval = *zval + zcon[i] * bval[i];

    delete [] bval;
  }

}

