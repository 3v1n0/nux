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
#ifndef NUXCORE_PROPERTY_OPERATORS_H
#define NUXCORE_PROPERTY_OPERATORS_H

namespace nux {

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE>
inline bool operator ==(TEST_TYPE const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs == rhs();
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE>
inline bool operator ==(PROP_TYPE<VALUE_TYPE> const& lhs,
                        TEST_TYPE const& rhs)
{
  return lhs() == rhs;
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE>
inline bool operator ==(PROP_TYPE<VALUE_TYPE> const& lhs,
                        PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs() == rhs();
}


template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE>
inline bool operator <(TEST_TYPE const& lhs,
                       PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs < rhs();
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE,
          typename TEST_TYPE>
inline bool operator <(PROP_TYPE<VALUE_TYPE> const& lhs,
                       TEST_TYPE const& rhs)
{
  return lhs() < rhs;
}

template <template <typename T> class PROP_TYPE,
          typename VALUE_TYPE>
inline bool operator <(PROP_TYPE<VALUE_TYPE> const& lhs,
                       PROP_TYPE<VALUE_TYPE> const& rhs)
{
  return lhs() < rhs();
}


template <typename LHS, typename RHS>
inline bool operator !=(LHS const& lhs, RHS const& rhs)
{
  return !(lhs == rhs);
}

template <typename LHS, typename RHS>
inline bool operator <=(LHS const& lhs, RHS const& rhs)
{
  return lhs < rhs || lhs == rhs;
}

template <typename LHS, typename RHS>
inline bool operator >(LHS const& lhs, RHS const& rhs)
{
  return !(lhs <= rhs);
}

template <typename LHS, typename RHS>
inline bool operator >=(LHS const& lhs, RHS const& rhs)
{
  return !(lhs < rhs);
}

}

#endif
