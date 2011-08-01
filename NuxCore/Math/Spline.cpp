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
#include "Spline.h"
#include "MathFunctions.h"

namespace nux
{

  double *CubicSpline::SolveTridiag ( int n, double a[], double b[] )
  {
    int i;
    double *x;
    double xmult;

    //
    //  Check.
    //
    for ( i = 0; i < n; i++ )
    {
      if ( a[1+i*3] == 0.0 )
      {
        return NULL;
      }
    }

    x = new double[n];

    for ( i = 0; i < n; i++ )
    {
      x[i] = b[i];
    }

    for ( i = 1; i < n; i++ )
    {
      xmult = a[2+ (i-1) *3] / a[1+ (i-1) *3];
      a[1+i*3] = a[1+i*3] - xmult * a[0+i*3];
      x[i] = x[i] - xmult * x[i-1];
    }

    x[n-1] = x[n-1] / a[1+ (n-1) *3];

    for ( i = n - 2; 0 <= i; i-- )
    {
      x[i] = ( x[i] - a[0+ (i+1) *3] * x[i+1] ) / a[1+i*3];
    }

    return x;
  }


  CubicSpline::CubicSpline (int numpoint, std::vector<double> x_array, std::vector<double> y_array, int ibcbeg, double ybcbeg, int ibcend, double ybcend )
  {
    if ( ( (int) x_array.size() != numpoint) || ( (int) y_array.size() != numpoint) )
    {
      NUX_BREAK_ASM_INT3;
    }

    np = numpoint;
    t = new double[np];
    y = new double[np];
    ddy = 0; //new double[np];

    for (int i = 0; i < np; i++)
    {
      t[i] = x_array[i];
      y[i] = y_array[i];
      //ddy[i] = 0;
    }

    ibcbeg_ = ibcbeg;
    ybcbeg_ = ybcbeg;
    ibcend_ = ibcend;
    ybcend_ = ybcend;
    Compute (ibcbeg_, ybcbeg_, ibcend_, ybcend_ );
  }

  CubicSpline::CubicSpline()
  {
    np = 2;
    t = new double[np];
    y = new double[np];
    ddy = 0; //new double[np];

    t[0] = 0.0;
    t[1] = 1.0;
    y[0] = 0.0;
    y[1] = 1.0;

    ibcbeg_ = 0;
    ybcbeg_ = 0;
    ibcend_ = 0;
    ybcend_ = 0;
    Compute (ibcbeg_, ybcbeg_, ibcend_, ybcend_ );

  }

  CubicSpline::CubicSpline (const CubicSpline &Other)
  {
    if (Other.np == 0)
      NUX_BREAK_ASM_INT3;

    np = Other.np;
    t = new double[np];
    y = new double[np];

    for (int i = 0; i < np; i++)
    {
      t[i] = Other.t[i];
      y[i] = Other.y[i];
    }

    ibcbeg_ = Other.ibcbeg_;
    ybcbeg_ = Other.ybcbeg_;
    ibcend_ = Other.ibcend_;
    ybcend_ = Other.ybcend_;
    Compute (ibcbeg_, ybcbeg_, ibcend_, ybcend_ );
  }

  CubicSpline &CubicSpline::operator = (const CubicSpline &Other)
  {
    if (Other.np == 0)
      NUX_BREAK_ASM_INT3;

    np = Other.np;
    t = new double[np];
    y = new double[np];

    for (int i = 0; i < np; i++)
    {
      t[i] = Other.t[i];
      y[i] = Other.y[i];
    }

    ibcbeg_ = Other.ibcbeg_;
    ybcbeg_ = Other.ybcbeg_;
    ibcend_ = Other.ibcend_;
    ybcend_ = Other.ybcend_;
    Compute (ibcbeg_, ybcbeg_, ibcend_, ybcend_);
    return *this;
  }

  void CubicSpline::Set (int numpoint, std::vector<double> x_array, std::vector<double> y_array, int ibcbeg, double ybcbeg, int ibcend, double ybcend)
  {
    if (numpoint <= 1)
    {
      np = 0;
      return;
    }

    if ( ( (int) x_array.size() != numpoint) || ( (int) y_array.size() != numpoint) )
    {
      NUX_BREAK_ASM_INT3;
    }

    np = numpoint;

    if (t) delete [] t;

    if (y) delete [] y;

    if (ddy) delete [] ddy;

    t = new double[np];
    y = new double[np];
    ddy = 0;

    for (int i = 0; i < np; i++)
    {
      t[i] = x_array[i];
      y[i] = y_array[i];
    }

    Compute (ibcbeg, ybcbeg, ibcend, ybcend );
  }

  CubicSpline::~CubicSpline()
  {
    if (t) delete [] t;

    if (y) delete [] y;

    if (ddy) delete [] ddy;
  }

  double *CubicSpline::Compute (int ibcbeg, double ybcbeg, int ibcend, double ybcend )
  {
    double *a;
    double *b;
    int i;

    //
    //  Check.
    //
    if ( np <= 1 )
    {
      //"Fatal error: The number of data points N must be at least 2.\n";
      NUX_BREAK_ASM_INT3;
      return 0;
    }

    for ( i = 0; i < np - 1; i++ )
    {
      if ( t[i+1] <= t[i] )
      {
        //"Fatal error: The knots must be strictly increasing, but\n";
        NUX_BREAK_ASM_INT3;
        return NULL;
      }
    }

    a = new double[3*np];
    b = new double[np];

    //
    //  Set up the first equation.
    //
    if ( ibcbeg == 0 )
    {
      b[0] = 0.0;
      a[1+0*3] = 1.0;
      a[0+1*3] = -1.0;
    }
    else if ( ibcbeg == 1 )
    {
      b[0] = ( y[1] - y[0] ) / ( t[1] - t[0] ) - ybcbeg;
      a[1+0*3] = ( t[1] - t[0] ) / 3.0;
      a[0+1*3] = ( t[1] - t[0] ) / 6.0;
    }
    else if ( ibcbeg == 2 )
    {
      b[0] = ybcbeg;
      a[1+0*3] = 1.0;
      a[0+1*3] = 0.0;
    }
    else
    {
      //"Fatal error: IBCBEG must be 0, 1 or 2.\n";
      NUX_BREAK_ASM_INT3;
      delete [] a;
      delete [] b;
      return NULL;
    }

    //
    //  Set up the intermediate equations.
    //
    for ( i = 1; i < np - 1; i++ )
    {
      b[i]            = ( y[i+1] - y[i] ) / ( t[i+1] - t[i] ) - ( y[i] - y[i-1] ) / ( t[i] - t[i-1] );

      a[2+ (i-1) *3]    = ( t[i] - t[i-1] ) / 6.0;

      a[1+ i   *3]    = ( t[i+1] - t[i-1] ) / 3.0;

      a[0+ (i+1) *3]    = ( t[i+1] - t[i] ) / 6.0;
    }

    //
    //  Set up the last equation.
    //
    if ( ibcend == 0 )
    {
      b[np-1]          = 0.0;
      a[2+ (np-2) *3]    = -1.0;
      a[1+ (np-1) *3]    = 1.0;
    }
    else if ( ibcend == 1 )
    {
      b[np-1]          = ybcend - ( y[np-1] - y[np-2] ) / ( t[np-1] - t[np-2] );
      a[2+ (np-2) *3]    = ( t[np-1] - t[np-2] ) / 6.0;
      a[1+ (np-1) *3]    = ( t[np-1] - t[np-2] ) / 3.0;
    }
    else if ( ibcend == 2 )
    {
      b[np-1]          = ybcend;
      a[2+ (np-2) *3]    = 0.0;
      a[1+ (np-1) *3]    = 1.0;
    }
    else
    {
      //"Fatal error: IBCEND must be 0, 1 or 2.\n";
      NUX_BREAK_ASM_INT3;
      delete [] a;
      delete [] b;
      return NULL;
    }

    //
    //  Solve the linear system.
    //
    if ( np == 2 && ibcbeg == 0 && ibcend == 0 )
    {
      ddy = new double[2];

      ddy[0] = 0.0;
      ddy[1] = 0.0;
    }
    else
    {
      if (ddy)
      {
        delete [] ddy;
      }

      ddy = SolveTridiag ( np, a, b );

      if ( !ddy )
      {
        //"Fatal error: The linear system could not be solved.\n";
        NUX_BREAK_ASM_INT3;
        delete [] a;
        delete [] b;
        return NULL;
      }
    }

    delete [] a;
    delete [] b;
    return ddy;
  }

  double CubicSpline::Eval (double tval)
  {
    double dt;
    double h;
    int i;
    int ival;
    double yval;

    //
    //  Determine the interval [ T(I), T(I+1) ] that contains TVAL.
    //  Values below T[0] or above T[N-1] use extrapolation.
    //
    if (np <= 1)
      return 0.0;

    ival = np - 2;

    if (tval > t[np-1])
    {
      tval = t[np-1];
    }

    if (tval < t[0])
    {
      tval = t[0];
    }

    for ( i = 0; i < np - 1; i++ )
    {
      if ( tval < t[i+1] )
      {
        ival = i;
        break;
      }
    }

    //
    //  In the interval I, the polynomial is in terms of a normalized
    //  coordinate between 0 and 1.
    //
    dt = tval - t[ival];
    h = t[ival+1] - t[ival];

    yval = y[ival] + dt * ( ( y[ival+1] - y[ival] ) / h  - ( ddy[ival+1] / 6.0 + ddy[ival] / 3.0 ) * h + dt * ( 0.5 * ddy[ival]
                            + dt * ( ( ddy[ival+1] - ddy[ival] ) / ( 6.0 * h ) ) ) );

    return yval;
  }

}
