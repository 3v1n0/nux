#ifndef POINT2D_INL_H
#define POINT2D_INL_H

namespace nux
{

  template<typename T>
  Point2D<T>::Point2D()
      : x(0), y(0)
  {
  }

  template<typename T>
  Point2D<T>::Point2D(T const& X, T const& Y)
      : x(X), y(Y)
  {
  }

  template<typename T>
  bool operator ==(const Point2D<T>& lhs, const Point2D<T>& rhs)
  {
      return (lhs.x == rhs.x) && (lhs.y == rhs.y);
  }

  template<typename T>
  bool operator !=(const Point2D<T>& lhs, const Point2D<T>& rhs)
  {
    return !(lhs == rhs);
  }

  template<typename T>
  Point2D<T> operator -(const Point2D<T>& lhs, const Point2D<T>& rhs)
  {
    return Point2D<T>(lhs.x - rhs.x, lhs.y - rhs.y);
  }

  template<typename T>
  Point2D<T> operator +(const Point2D<T>& lhs, const Point2D<T>& rhs)
  {
    return Point2D<T>(lhs.x + rhs.x, lhs.y + rhs.y);
  }

  template<typename T, typename S>
  Point2D<T> operator *(const Point2D<T>& lhs, S rhs)
  {
    return Point2D<T>(lhs.x * rhs, lhs.y * rhs);
  }

  template<typename T, typename S>
  Point2D<T> operator *(S lhs, const Point2D<T>& rhs)
  {
    return Point2D<T>(rhs.x * lhs, rhs.y * lhs);
  }

}


#endif
