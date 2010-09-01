#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <limits>

/*
PI with 500 decimals (from http://www.ex.ac.uk/cimt/general/pi10000.htm)

      3.1415926535   8979323846   2643383279   5028841971   6939937510
        5820974944   5923078164   0628620899   8628034825   3421170679
        8214808651   3282306647   0938446095   5058223172   5359408128
        4811174502   8410270193   8521105559   6446229489   5493038196
        4428810975   6659334461   2847564823   3786783165   2712019091
        4564856692   3460348610   4543266482   1339360726   0249141273
        7245870066   0631558817   4881520920   9628292540   9171536436
        7892590360   0113305305   4882046652   1384146951   9415116094
        3305727036   5759591953   0921861173   8193261179   3105118548
        0744623799   6274956735   1885752724   8912279381   8301194912
*/
// 2.7182818284590452353602874713526624977572470936999595749669676277



NAMESPACE_BEGIN

class Const
{
public:
    static const float pi;
    static const float Pi;
    static const float e;
    static const float sqrt2;
    static const float sqrt3;
    static const float golden;

    static const double EpsilonMilli;
    static const double EpsilonMicro;
    static const double EpsilonNano;
    static const double EpsilonPico;
    static const double EpsilonFemto;

private:
    Const();
    ~Const();
    Const (const Const&);
    Const& operator = (const Const&);
};

NAMESPACE_END

#endif // CONSTANTS_H

