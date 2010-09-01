#ifndef POINT2D_H
#define POINT2D_H

NAMESPACE_BEGIN

template<typename T>
class Point2D
	{
	public:
		Point2D();
		~Point2D();
		Point2D(const Point2D& Pt);
		Point2D(T x, T y);

        bool operator == (const Point2D<T>& Pt) const;
        bool operator != (const Point2D<T>& Pt) const;

		void Set(T x, T y);

		T x, y;
	};

template<typename T>
Point2D<T>::Point2D()
{
    x = 0;
    y = 0;
}

template<typename T>
Point2D<T>::~Point2D()
{
}

template<typename T>
Point2D<T>::Point2D(const Point2D& Pt)
{
    x = Pt.x;
    y = Pt.y;
}

template<typename T>
Point2D<T>::Point2D(T X, T Y)
{
    x = X;
    y = Y;
}

template<typename T>
void Point2D<T>::Set(T X, T Y)
{
    x = X;
    y = Y;
}

template<typename T>
bool Point2D<T>::operator == (const Point2D& Pt) const
{
    if((x == Pt.x) && (y == Pt.y))
    {
        return true;
    }
    else
    {
        return false;
    }
}

template<typename T>
bool Point2D<T>::operator != (const Point2D& Pt) const
{
    return !((*this) == Pt);
}

NAMESPACE_END

#endif // POINT2D_H
