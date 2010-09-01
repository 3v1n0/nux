#include "../NKernel.h"
#include "MathFunctions.h"


NAMESPACE_BEGIN

const float Const::pi       = 3.1415926535f;
const float Const::Pi       = 3.1415926535f;
const float Const::e        = 2.7182817f;
const float Const::sqrt2    = 1.414214f;
const float Const::sqrt3    = 1.732051f;
const float Const::golden   = 1.618034f;

const double Const::EpsilonMilli   =   0.001;
const double Const::EpsilonMicro   =   0.000001;
const double Const::EpsilonNano    =   0.000000001;
const double Const::EpsilonPico    =   0.000000000001;
const double Const::EpsilonFemto   =   0.000000000000001;

t_int Factorial(t_int n)
{
    t_int i = 1;
    while(0 < n)
    {
        i *= n;
        n--;
    }
    return i;
}

t_double BinomialCoefficient(t_int n, t_int k)
{
    if(n < 0)
        INL_BREAK_ASM_INT3;

    if(k < 0 || k > n)
        return 0.0;

    t_double d = (t_double) Factorial(n) / (t_double) (Factorial(n-k) * Factorial(k));
    return d;
}

t_double Power(t_double x, t_double y)
{
    return std::pow(x, y);
}

t_double Log2(t_double d)
{
    return std::log(d) / std::log(2.0);
}

t_double Floor(t_double d)
{
    return std::floor(d);
}

NAMESPACE_END