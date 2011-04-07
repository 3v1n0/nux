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


#ifndef SPLINE_H
#define SPLINE_H

#include <vector>

namespace nux
{

  double *d3_np_fs ( int n, double a[], double b[] );
  double *spline_cubic_set ( int n, double t[], double y[], int ibcbeg, double ybcbeg, int ibcend, double ybcend );
  double spline_cubic_val ( int n, double t[], double tval, double y[], double ypp[], double *ypval, double *yppval );


  class CubicSpline
  {
  public:
    CubicSpline();
    CubicSpline (const CubicSpline &Other);
    CubicSpline &operator = (const CubicSpline &Other);

    CubicSpline (int numpoint, std::vector<double> x_array, std::vector<double> y_array, int ibcbeg = 0, double ybcbeg = 0, int ibcend = 0, double ybcend = 0);
    void Set (int numpoint, std::vector<double> x_array, std::vector<double> y_array, int ibcbeg = 0, double ybcbeg = 0, int ibcend = 0, double ybcend = 0);

    ~CubicSpline();

    //**********************************************************************
    //  double* CubicSpline::Compute ( int n, double t[], double y[], int ibcbeg, double ybcbeg, int ibcend, double ybcend )
    //  Purpose:
    //
    //    SPLINE_CUBIC_SET computes the second derivatives of a piecewise cubic spline.
    //
    //  Discussion:
    //
    //    For data interpolation, the user must call SPLINE_SET to determine
    //    the second derivative data, passing in the data to be interpolated,
    //    and the desired boundary conditions.
    //
    //    The data to be interpolated, plus the SPLINE_SET output, defines
    //    the spline.  The user may then call SPLINE_VAL to evaluate the
    //    spline at any point.
    //
    //    The cubic spline is a piecewise cubic polynomial.  The intervals
    //    are determined by the "knots" or abscissas of the data to be
    //    interpolated.  The cubic spline has continous first and second
    //    derivatives over the entire interval of interpolation.
    //
    //    For any point T in the interval T(IVAL), T(IVAL+1), the form of
    //    the spline is
    //
    //      SPL(T) = A(IVAL)
    //             + B(IVAL) * ( T - T(IVAL) )
    //             + C(IVAL) * ( T - T(IVAL) )**2
    //             + D(IVAL) * ( T - T(IVAL) )**3
    //
    //    If we assume that we know the values Y(*) and DDY(*), which represent
    //    the values and second derivatives of the spline at each knot, then
    //    the coefficients can be computed as:
    //
    //      A(IVAL) = Y(IVAL)
    //      B(IVAL) = ( Y(IVAL+1) - Y(IVAL) ) / ( T(IVAL+1) - T(IVAL) )
    //        - ( DDY(IVAL+1) + 2 * DDY(IVAL) ) * ( T(IVAL+1) - T(IVAL) ) / 6
    //      C(IVAL) = DDY(IVAL) / 2
    //      D(IVAL) = ( DDY(IVAL+1) - DDY(IVAL) ) / ( 6 * ( T(IVAL+1) - T(IVAL) ) )
    //
    //    Since the first derivative of the spline is
    //
    //      SPL'(T) =     B(IVAL)
    //              + 2 * C(IVAL) * ( T - T(IVAL) )
    //              + 3 * D(IVAL) * ( T - T(IVAL) )**2,
    //
    //    the requirement that the first derivative be continuous at interior
    //    knot I results in a total of N-2 equations, of the form:
    //
    //      B(IVAL-1) + 2 C(IVAL-1) * (T(IVAL)-T(IVAL-1)) + 3 * D(IVAL-1) * (T(IVAL) - T(IVAL-1))^2 = B(IVAL)
    //
    //    or, setting H(IVAL) = T(IVAL+1) - T(IVAL)
    //
    //      ( Y(IVAL) - Y(IVAL-1) ) / H(IVAL-1) - ( DDY(IVAL) + 2 * DDY(IVAL-1) ) * H(IVAL-1) / 6 + DDY(IVAL-1) * H(IVAL-1)
    //      + ( DDY(IVAL) - DDY(IVAL-1) ) * H(IVAL-1) / 2
    //      = ( Y(IVAL+1) - Y(IVAL) ) / H(IVAL) - ( DDY(IVAL+1) + 2 * DDY(IVAL) ) * H(IVAL) / 6
    //
    //    or
    //
    //      DDY(IVAL-1) * H(IVAL-1) + 2 * DDY(IVAL) * ( H(IVAL-1) + H(IVAL) ) + DDY(IVAL) * H(IVAL)
    //      = 6 * ( Y(IVAL+1) - Y(IVAL) ) / H(IVAL) - 6 * ( Y(IVAL) - Y(IVAL-1) ) / H(IVAL-1)
    //
    //    Boundary conditions must be applied at the first and last knots.
    //    The resulting tridiagonal system can be solved for the DDY values.
    //
    //
    //  Parameters:
    //
    //    Input, int N, the number of data points.  N must be at least 2.
    //    In the special case where N = 2 and IBCBEG = IBCEND = 0, the
    //    spline will actually be linear.
    //
    //    Input, double T[N], the knot values, that is, the points were data is
    //    specified.  The knot values should be distinct, and increasing.
    //
    //    Input, double Y[N], the data values to be interpolated.
    //
    //    Input, int IBCBEG, left boundary condition flag:
    //      0: the cubic spline should be a quadratic over the first interval;
    //      1: the first derivative at the left endpoint should be YBCBEG;
    //      2: the second derivative at the left endpoint should be YBCBEG.
    //
    //    Input, double YBCBEG, the values to be used in the boundary
    //    conditions if IBCBEG is equal to 1 or 2.
    //
    //    Input, int IBCEND, right boundary condition flag:
    //      0: the cubic spline should be a quadratic over the last interval;
    //      1: the first derivative at the right endpoint should be YBCEND;
    //      2: the second derivative at the right endpoint should be YBCEND.
    //
    //    Input, double YBCEND, the values to be used in the boundary
    //    conditions if IBCEND is equal to 1 or 2.
    //
    //    Output, double SPLINE_CUBIC_SET[N], the second derivatives of the cubic spline.
    //
    double *Compute (int ibcbeg, double ybcbeg, int ibcend, double ybcend);

    //**********************************************************************
    //  double Eval ( int n, double t[], double tval, double y[], double ddy[], double *dyval, double *ddyval )
    //  Purpose:
    //
    //    SPLINE_CUBIC_VAL evaluates a piecewise cubic spline at a point.
    //
    //  Discussion:
    //
    //    SPLINE_CUBIC_SET must have already been called to define the values of YPP.
    //
    //    For any point T in the interval T(IVAL), T(IVAL+1), the form of
    //    the spline is
    //
    //      SPL(T) = A
    //             + B * ( T - T(IVAL) )
    //             + C * ( T - T(IVAL) )**2
    //             + D * ( T - T(IVAL) )**3
    //
    //    Here:
    //      A = Y(IVAL)
    //      B = ( Y(IVAL+1) - Y(IVAL) ) / ( T(IVAL+1) - T(IVAL) )
    //        - ( YPP(IVAL+1) + 2 * YPP(IVAL) ) * ( T(IVAL+1) - T(IVAL) ) / 6
    //      C = YPP(IVAL) / 2
    //      D = ( YPP(IVAL+1) - YPP(IVAL) ) / ( 6 * ( T(IVAL+1) - T(IVAL) ) )
    //
    //  Modified:
    //
    //    04 February 1999
    //
    //  Author:
    //
    //    John Burkardt
    //
    //  Parameters:
    //
    //    Input, int N, the number of knots.
    //
    //    Input, double Y[N], the data values at the knots.
    //
    //    Input, double T[N], the knot values.
    //
    //    Input, double TVAL, a point, typically between T[0] and T[N-1], at
    //    which the spline is to be evalulated.  If TVAL lies outside
    //    this range, extrapolation is used.
    //
    //    Input, double Y[N], the data values at the knots.
    //
    //    Input, double YPP[N], the second derivatives of the spline at
    //    the knots.
    //
    //    Output, double *YPVAL, the derivative of the spline at TVAL.
    //
    //    Output, double *YPPVAL, the second derivative of the spline at TVAL.
    //
    //    Output, double SPLINE_VAL, the value of the spline at TVAL.
    //
    double Eval (double tval);

    //**********************************************************************
    //  double* SolveTridiag ( int n, double a[], double b[] );
    //  Purpose:
    //
    //    D3_NP_FS factors and solves a D3 system.
    //
    //  Discussion:
    //
    //    The D3 storage format is used for a tridiagonal matrix.
    //    The superdiagonal is stored in entries (1,2:N), the diagonal in
    //    entries (2,1:N), and the subdiagonal in (3,1:N-1).  Thus, the
    //    original matrix is "collapsed" vertically into the array.
    //
    //    This algorithm requires that each diagonal entry be nonzero.
    //    It does not use pivoting, and so can fail on systems that
    //    are actually nonsingular.
    //
    //  Example:
    //
    //    Here is how a D3 matrix of order 5 would be stored:
    //
    //       *  A12 A23 A34 A45
    //      A11 A22 A33 A44 A55
    //      A21 A32 A43 A54  *
    //
    //  Parameters:
    //
    //    Input, int N, the order of the linear system.
    //
    //    Input/output, double A[3*N].
    //    On input, the nonzero diagonals of the linear system.
    //    On output, the data in these vectors has been overwritten
    //    by factorization information.
    //
    //    Input, double B[N], the right hand side.
    //
    //    Output, double D3_NP_FS[N], the solution of the linear system.
    //    This is NULL if there was an error because one of the diagonal
    //    entries was zero.
    //
    double *SolveTridiag ( int n, double a[], double b[] );

  public:
    double *t;
    double *y;
    double *ddy;

    int ibcbeg_;
    double ybcbeg_;
    int ibcend_;
    double ybcend_;

    int np;  // number of points
  };

}

#endif // SPLINE_H
