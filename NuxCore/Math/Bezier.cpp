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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "../NKernel.h"
#include "MathFunctions.h"
#include "Bezier.h"

NAMESPACE_BEGIN

INL_DECLSPEC_DLL t_double* Bernstein(t_int n, t_double t)
{
    if(n < 0)
    {
        INL_BREAK_ASM_INT3;
    }

    t_double *bernstein;
    t_int i;

    bernstein = new t_double[n+1];

    for (i = 0; i <= n; i++ )
    {
        bernstein[i] = BinomialCoefficient(n, i) * PowerInt<t_double>(t, i) * PowerInt<t_double>(1.0 - t, n-i);
    }
    return bernstein;
}

INL_DECLSPEC_DLL void Bezier_XY(t_int n, t_double t, t_double xcon[], t_double ycon[], t_double *xval, t_double *yval)
{
    if(n < 0)
    {
        INL_BREAK_ASM_INT3;
    }

    double *bval;
    int i;

    bval = Bernstein ( n, t );

    *xval = 0.0;
    for( i = 0; i <= n; i++ )
        *xval = *xval + xcon[i] * bval[i];

    *yval = 0.0;
    for ( i = 0; i <= n; i++ )
        *yval = *yval + ycon[i] * bval[i];

    delete [] bval;
}

INL_DECLSPEC_DLL void Bezier_XYZ(t_int n, t_double t, t_double xcon[], t_double ycon[], t_double zcon[], t_double *xval, t_double *yval, t_double *zval)
{
    if(n < 0)
        INL_BREAK_ASM_INT3;

    double *bval;
    int i;

    bval = Bernstein ( n, t );

    *xval = 0.0;
    for( i = 0; i <= n; i++ )
        *xval = *xval + xcon[i] * bval[i];
    
    *yval = 0.0;
    for ( i = 0; i <= n; i++ )
        *yval = *yval + ycon[i] * bval[i];
    
    *zval = 0.0;
    for ( i = 0; i <= n; i++ )
        *zval = *zval + zcon[i] * bval[i];

    delete [] bval;
}

NAMESPACE_END

