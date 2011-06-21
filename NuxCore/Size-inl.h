#ifndef NUX_CORE_SIZE_INL_H
#define NUX_CORE_SIZE_INL_H

namespace nux
{

inline Size::Size()
  : width(0), height(0)
{}

inline Size::Size(int w, int h)
  : width(w), height(h)
{}

inline bool operator == (const Size& lhs, const Size& rhs)
{
  return (lhs.width == rhs.width) && (lhs.height == rhs.height);
}

inline bool operator != (const Size& lhs, const Size& rhs)
{
  return !(lhs == rhs);
}

}


#endif
