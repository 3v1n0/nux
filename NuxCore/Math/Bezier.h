#ifndef BEZIER_H
#define BEZIER_H

#include "Constants.h"

NAMESPACE_BEGIN

//! Evaluate the Bernstein polynomial of degree n, at parameter t.
/*!
    Evaluate the Bernstein polynomial of degree n, at parameter t.
    
    @param n The degree of the Bernstein basis polynomials.
    @param t The evaluation point.

    @return the values of the n+1 Bernstein basis of degree n.
*/
INL_DECLSPEC_DLL t_double* Bernstein(t_int n, t_double t);

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
INL_DECLSPEC_DLL void Bezier_XY(t_int n, t_double t, t_double xcon[], t_double ycon[], t_double *xval, t_double *yval);

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
INL_DECLSPEC_DLL void Bezier_XYZ(t_int n, t_double t, t_double xcon[], t_double ycon[], t_double zcon[], t_double *xval, t_double *yval, t_double *zval);



NAMESPACE_END

#endif // BEZIER_H


