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


#ifndef BEZIER_H
#define BEZIER_H

#include "Constants.h"

namespace nux
{

//! Evaluate the Bernstein polynomial of degree n, at parameter t.
  /*!
      Evaluate the Bernstein polynomial of degree n, at parameter t.

      @param n The degree of the Bernstein basis polynomials.
      @param t The evaluation point.

      @return the values of the n+1 Bernstein basis of degree n.
  */
  NUX_DECLSPEC_DLL double *Bernstein (int n, double t);

//! Evaluate 2D Bezier curve of degree n.
  /*!
      Evaluate 2D Bezier curve of degree n.

      @param n The degree of the Bezier curve.
      @param t Parameter.
      @param xcon Array of n+1 x coordinates of control points.
      @param ycon Array of n+1 y coordinates of control points.
      @param xval Return the x coordinates of the Bezier curve at parameter t.
      @param yval Return the y coordinates of the Bezier curve at parameter t.
  */
  NUX_DECLSPEC_DLL void Bezier_XY (int n, double t, double xcon[], double ycon[], double *xval, double *yval);

//! Evaluate 2D Bezier curve of degree n.
  /*!
      Evaluate 2D Bezier curve of degree n.

      @param n The degree of the Bezier curve.
      @param t Parameter.
      @param xcon Array of n+1 x coordinates of control points.
      @param ycon Array of n+1 y coordinates of control points.
      @param ycon Array of n+1 z coordinates of control points.
      @param xval Return the x coordinates of the Bezier curve at parameter t.
      @param yval Return the y coordinates of the Bezier curve at parameter t.
      @param yval Return the z coordinates of the Bezier curve at parameter t.
  */
  NUX_DECLSPEC_DLL void Bezier_XYZ (int n, double t, double xcon[], double ycon[], double zcon[], double *xval, double *yval, double *zval);



}

#endif // BEZIER_H


