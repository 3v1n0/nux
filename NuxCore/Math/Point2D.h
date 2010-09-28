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


#ifndef POINT2D_H
#define POINT2D_H

namespace nux { //NUX_NAMESPACE_BEGIN

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

} //NUX_NAMESPACE_END

#endif // POINT2D_H
