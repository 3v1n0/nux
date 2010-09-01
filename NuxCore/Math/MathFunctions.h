#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include "Constants.h"

NAMESPACE_BEGIN

t_int Factorial(t_int n);
t_double BinomialCoefficient(t_int n, t_int k);


//! Template power function. The exponent is an integer.
/*!
    Template power function. The exponent is an integer.
*/

template<class T> inline
T PowerInt(T _X, int _Y)    // taken from Microsoft <math.h>
{
    unsigned int _N;
    if (_Y >= 0)
        _N = _Y;
    else
        _N = -_Y;
    for (T _Z = T(1); ; _X *= _X)
    {
        if ((_N & 1) != 0)
            _Z *= _X;
        if ((_N >>= 1) == 0)
            return (_Y < 0 ? T(1) / _Z : _Z);
    }
}

t_double Power(t_double x, t_double y);

t_double Log2(t_double d);
t_double Floor(t_double d);


NAMESPACE_END

#endif // MATHFUNCTIONS_H

