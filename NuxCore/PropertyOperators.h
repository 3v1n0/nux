// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2011 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Tim Penhey <tim.penhey@canonical.com>
 *
 */

#if defined(NUX_OS_WINDOWS)
  #pragma warning(disable : 4519) // error C4519: default template arguments are only allowed on a class template
#endif

#ifndef NUXCORE_PROPERTY_OPERATORS_H
#define NUXCORE_PROPERTY_OPERATORS_H

namespace nux {

/**
 * This is a very boring file.
 *
 * It adds boolean operators for properties.  The challenge has been making
 * operators that work for properties and only properties.  Since we don't
 * want to add explicit methods for each type of property we have, we have the
 * default template argument VT which gets the ValueType from the Property
 * definition.  Since things like std::_list_iterator<foo> doesn't have a
 * ValueType typedef, these functions are discarded from the possible choices,
 * but since the Property templates do, they are valid, and get chosen.
 *
 * However as much as I'd like to have more generic operators for !=, <=, >
 * and >=, we can't really without making these functions choosable for types
 * that we don't want, so we have three functions defined for each operator.
 */

// operator ==

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator ==(TEST_TYPE const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs == rhs();
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator ==(PROP_TYPE<VALUE_TYPE> const& lhs,
                        TEST_TYPE const& rhs)
{
  return lhs() == rhs;
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator ==(PROP_TYPE<VALUE_TYPE> const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs() == rhs();
}

// operator !=

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator !=(TEST_TYPE const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs != rhs();
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator !=(PROP_TYPE<VALUE_TYPE> const& lhs,
                        TEST_TYPE const& rhs)
{
  return lhs() != rhs;
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator !=(PROP_TYPE<VALUE_TYPE> const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs() != rhs();
}

// operator <

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator <(TEST_TYPE const& lhs,
                       PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs < rhs();
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator <(PROP_TYPE<VALUE_TYPE> const& lhs,
                       TEST_TYPE const& rhs)
{
  return lhs() < rhs;
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator <(PROP_TYPE<VALUE_TYPE> const& lhs,
                       PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs() < rhs();
}

// operator <=

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator <=(TEST_TYPE const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs <= rhs();
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator <=(PROP_TYPE<VALUE_TYPE> const& lhs,
                        TEST_TYPE const& rhs)
{
  return lhs() <= rhs;
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator <=(PROP_TYPE<VALUE_TYPE> const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs() <= rhs();
}

// operator >

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator >(TEST_TYPE const& lhs,
                       PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs > rhs();
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator >(PROP_TYPE<VALUE_TYPE> const& lhs,
                       TEST_TYPE const& rhs)
{
  return lhs() > rhs;
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator >(PROP_TYPE<VALUE_TYPE> const& lhs,
                       PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs() > rhs();
}

// operator >=

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator >=(TEST_TYPE const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs >= rhs();
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator >=(PROP_TYPE<VALUE_TYPE> const& lhs,
                        TEST_TYPE const& rhs)
{
  return lhs() >= rhs;
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename VT = typename PROP_TYPE<VALUE_TYPE>::ValueType>
inline bool operator >=(PROP_TYPE<VALUE_TYPE> const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs() >= rhs();
}

}

#endif
