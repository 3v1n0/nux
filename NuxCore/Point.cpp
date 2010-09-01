#include "NKernel.h"
#include "Point.h"

NAMESPACE_BEGIN

Point::Point()
: x(0), y(0)
{
}

Point::Point(t_int32 x_, t_int32 y_)
: x(x_), y(y_)
{
}

//----------------------------------------------------------------------------
Point::~Point()
{
}

//----------------------------------------------------------------------------
Point::Point(const Point& p)
: x(p.x), y(p.y)
{
} 
  
//----------------------------------------------------------------------------
Point& Point::operator = (const Point& p)
{
    if(&p == this)
        return *this;
    x = p.x;
    y = p.y;
    return *this;
}

//----------------------------------------------------------------------------
t_bool Point::operator == (const Point& p) const
{
    if((x == p.x) && (y == p.y))
    {
        return true;
    }
    return false;
}

//----------------------------------------------------------------------------
t_bool Point::operator != (const Point& p) const
{
    if((x == p.x) && (y == p.y))
    {
        return false;
    }
    return true;
}

//----------------------------------------------------------------------------
Point Point::operator + (const Point& p) const
{
    Point point;

    point.x = x + p.x;
    point.y = y + p.y;
    return point;
}

//----------------------------------------------------------------------------
Point Point::operator - (const Point& p) const
{
    Point point;

    point.x = x - p.x;
    point.y = y - p.y;
    return point;
}

//----------------------------------------------------------------------------
Point& Point::operator += (const Point& p)
{
    x += p.x;
    y += p.y;
    return *this;
}

//----------------------------------------------------------------------------
Point& Point::operator -= (const Point& p)
{
    x -= p.x;
    y -= p.y;
    return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


NAMESPACE_END
