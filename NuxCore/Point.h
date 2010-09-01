#ifndef POINT_H
#define POINT_H

NAMESPACE_BEGIN

class Point
{
public:
    Point();
    Point(t_int32 x, t_int32 y);
    ~Point();
    Point(const Point& p);

    Point& operator = (const Point& p);
    t_bool operator == (const Point& p) const;
    t_bool operator != (const Point& p) const;
	Point operator + (const Point& p) const;
	Point operator - (const Point& p) const;
    Point& operator += (const Point& p);
    Point& operator -= (const Point& p);

    void Set(t_int32 x_, t_int32 y_) {x = x_; y = y_;}
    void Get(t_int32& x_, t_int32& y_) {x_ = x; y_ = y;}

    t_int32 x;
    t_int32 y;
};

NAMESPACE_END
#endif // POINT_H


