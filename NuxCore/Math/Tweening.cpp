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
#include "MathInc.h"

#include "Tweening.h"

NAMESPACE_BEGIN

// Back
double BackEaseIn (double t, double b, double c, double d, double s)
{
    return c*(t/=d)*t*((s+1)*t - s) + b;
}

double BackEaseOut (double t, double b, double c, double d, double s)
{
    return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
}

double BackEaseInOut (double t, double b, double c, double d, double s)
{
    if ((t/=d/2) < 1)
        return c/2*(t*t*(((s*=(1.525))+1)*t - s)) + b;
    return c/2*((t-=2)*t*(((s*=(1.525))+1)*t + s) + 2) + b;
}

// Bounce
double BounceEaseOut (double t, double b, double c, double d)
{
    if ((t/=d) < (1/2.75))
    {
        return c*(7.5625*t*t) + b;
    }
    else if (t < (2/2.75))
    {
        return c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
    }
    else if (t < (2.5/2.75))
    {
        return c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
    }
    else
    {
        return c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
    }
}

double BounceEaseIn (double t, double b, double c, double d)
{
    return c - BounceEaseOut (d-t, 0, c, d) + b;
}

double BounceEaseInOut (double t, double b, double c, double d)
{
    if (t < d/2)
        return BounceEaseIn (t*2, 0, c, d) * .5 + b;
    else
        return BounceEaseOut (t*2-d, 0, c, d) * .5 + c*.5 + b;
}

// Circ
double CircEaseIn (double t, double b, double c, double d)
{
    return -c * (std::sqrt(1 - (t/=d)*t) - 1) + b;
}

double CircEaseOut (double t, double b, double c, double d)
{
    return c * std::sqrt(1 - (t=t/d-1)*t) + b;
}

double CircEaseInOut (double t, double b, double c, double d)
{
    if ((t/=d/2) < 1) return -c/2 * (std::sqrt(1 - t*t) - 1) + b;
    return c/2 * (std::sqrt(1 - (t-=2)*t) + 1) + b;
}

// Cubic
double CubicEaseIn (double t, double b, double c, double d)
{
    return c*(t/=d)*t*t + b;
}

double CubicEaseOut (double t, double b, double c, double d)
{
    return c*((t=t/d-1)*t*t + 1) + b;
}

double CubicEaseInOut (double t, double b, double c, double d)
{
    if ((t/=d/2) < 1) return c/2*t*t*t + b;
    return c/2*((t-=2)*t*t + 2) + b;
}

// Elastic
double ElasticEaseIn (double t, double b, double c, double d, double a, double p)
{
    double s = 0;
    if (t==0)
        return b;
    if ((t/=d)==1)
        return b+c;
    if (!p)
        p=d*.3;
    if (!a || a < std::abs(c))
    {
        a=c;
        s=p/4;
    }
    else
        s = p/(2*nux::Const::pi) * std::asin (c/a);

    return -(a*std::pow(2,10*(t-=1)) * std::sin( (t*d-s)*(2*nux::Const::pi)/p )) + b;
}

double ElasticEaseOut (double t, double b, double c, double d, double a, double p)
{
    double s = 0;
    if (t==0)
        return b;
    if ((t/=d)==1)
        return b+c;
    if (!p)
        p=d*.3;
    if (!a || a < std::abs(c))
    {
        a=c;
        s=p/4;
    }
    else
        s = p/(2*nux::Const::pi) * std::asin (c/a);

    return (a*std::pow(2,-10*t) * std::sin( (t*d-s)*(2*nux::Const::pi)/p ) + c + b);
}

double ElasticEaseInOut (double t, double b, double c, double d, double a, double p)
{
    double s = 0;
    if (t==0)
        return b;
    if ((t/=d/2)==2)
        return b+c;
    if (!p)
        p=d*(.3*1.5);
    if (!a || a < std::abs(c))
    {
        a=c;
        s=p/4;
    }
    else 
        s = p/(2*nux::Const::pi) * std::asin (c/a);
    if (t < 1)
        return -.5*(a*std::pow(2,10*(t-=1)) * std::sin( (t*d-s)*(2*nux::Const::pi)/p )) + b;

    return a*std::pow(2,-10*(t-=1)) * std::sin( (t*d-s)*(2*nux::Const::pi)/p )*.5 + c + b;
}

// Expo
double ExpoEaseIn (double t, double b, double c, double d)
{
    return (t==0) ? b : c * std::pow(2, 10 * (t/d - 1)) + b;
}

double ExpoEaseOut (double t, double b, double c, double d)
{
    return (t==d) ? b+c : c * (-std::pow(2, -10 * t/d) + 1) + b;
}

double ExpoEaseInOut (double t, double b, double c, double d)
{
    if (t==0)
        return b;
    if (t==d)
        return b+c;
    if ((t/=d/2) < 1)
        return c/2 * std::pow(2, 10 * (t - 1)) + b;
    return c/2 * (-std::pow(2, -10 * --t) + 2) + b;
}

// Linear
double LinearEaseNone (double t, double b, double c, double d)
{
    return c*t/d + b;
}

double LinearEaseIn (double t, double b, double c, double d)
{
    return c*t/d + b;
}

double LinearEaseOut (double t, double b, double c, double d)
{
    return c*t/d + b;
}

double LinearEaseInOut (double t, double b, double c, double d)
{
    return c*t/d + b;
}

// Quad
double QuadEaseIn (double t, double b, double c, double d)
{
    return c*(t/=d)*t + b;
}

double QuadEaseOut (double t, double b, double c, double d)
{
    return -c *(t/=d)*(t-2) + b;
}

double QuadEaseInOut (double t, double b, double c, double d)
{
    if ((t/=d/2) < 1) return c/2*t*t + b;
    return -c/2 * ((--t)*(t-2) - 1) + b;
}

// Quart
double QuartEaseIn (double t, double b, double c, double d)
{
    return c*(t/=d)*t*t*t + b;
}

double QuartEaseOut (double t, double b, double c, double d)
{
    return -c * ((t=t/d-1)*t*t*t - 1) + b;
}

double QuartEaseInOut (double t, double b, double c, double d)
{
    if ((t/=d/2) < 1)
        return c/2*t*t*t*t + b;
    return -c/2 * ((t-=2)*t*t*t - 2) + b;
}

// Quint
double QuintEaseIn (double t, double b, double c, double d)
{
    return c*(t/=d)*t*t*t*t + b;
}

double QuintEaseOut (double t, double b, double c, double d)
{
    return c*((t=t/d-1)*t*t*t*t + 1) + b;
}

double QuintEaseInOut (double t, double b, double c, double d)
{
    if ((t/=d/2) < 1)
        return c/2*t*t*t*t*t + b;
    return c/2*((t-=2)*t*t*t*t + 2) + b;
}

// Sine
double SineEaseIn (double t, double b, double c, double d)
{
    return -c * std::cos(t/d * (nux::Const::pi/2)) + c + b;
}

double SineEaseOut (double t, double b, double c, double d)
{
    return c * std::sin(t/d * (nux::Const::pi/2)) + b;
}

double SineEaseInOut (double t, double b, double c, double d)
{
    return -c/2 * (std::cos(nux::Const::pi*t/d) - 1) + b;
}

NAMESPACE_END
