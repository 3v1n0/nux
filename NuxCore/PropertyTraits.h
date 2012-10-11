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
  #pragma warning(disable : 4355) // warning C4355: 'this' : used in base member initializer list
#endif

#ifndef NUXCORE_PROPERTY_TRAITS_H
#define NUXCORE_PROPERTY_TRAITS_H

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/type_traits/is_enum.hpp>

#include "Color.h"


namespace nux {
namespace type {

/**
 * Here we want to have only explict type support, to avoid someone using the
 * system with something we haven't though of and getting upset when if it
 * doesn't work properly.
 */

template <typename T, bool = boost::is_enum<T>::value>
struct PropertyTrait
{
    /**
     * Expected content:
     *
     * typedef T ValueType;
     *  - used to make sure that there is a definition of the PropertyTrait
     *    for the particular value of T.
     *
     * static std::string to_string(T const& value)
     *  - used to convert the type to a string
     *
     * static std::pair<T, bool> from_string(std::string const& serialized_form)
     *  - If the serialized form is convertable to T, then we get:
     *      (value, true)
     *  - If it is not convertable we get:
     *      (T(), false)
     */
};


/**
 * The serializable_impl template has different signatures than the standard
 * Serializable traits class to avoid the generalised template being used as a
 * traits type when we really don't want it being used in that way, but we do
 * want a generalized implementation for the types we do care about.
 */
template <typename T>
struct serializable_impl
{
  typedef std::pair<T, bool> ResultType;

  static std::string to_string_impl(T const& value)
  {
    return boost::lexical_cast<std::string>(value);
  }

  static ResultType from_string_impl(std::string const& serialized_form)
  {
    try {
      return ResultType(boost::lexical_cast<T>(serialized_form), true);
    }
    catch (const boost::bad_lexical_cast& ) {
      return std::make_pair<T, bool>(T(), false);
    }
  }

};


template <typename ENUM>
struct PropertyTrait<ENUM, true>
{
  typedef ENUM ValueType;
  typedef serializable_impl<unsigned> Serialiser;
  typedef std::pair<ENUM, bool> ResultType;

  static std::string to_string(ENUM value)
  {
      return Serialiser::to_string_impl(value);
  }

  static std::pair<ENUM, bool> from_string(std::string const& serialized_form)
  {
    Serialiser::ResultType result = Serialiser::from_string_impl(serialized_form);
    return ResultType(static_cast<ENUM>(result.first), result.second);
  }
};


template <>
struct PropertyTrait<int>
{
  typedef int ValueType;

  static std::string to_string(int value)
  {
      return serializable_impl<int>::to_string_impl(value);
  }

  static std::pair<int, bool> from_string(std::string const& serialized_form)
  {
      return serializable_impl<int>::from_string_impl(serialized_form);
  }
};

template <>
struct PropertyTrait<unsigned>
{
  typedef unsigned ValueType;

  static std::string to_string(unsigned value)
  {
      return serializable_impl<unsigned>::to_string_impl(value);
  }

  static std::pair<unsigned, bool> from_string(std::string const& serialized_form)
  {
      return serializable_impl<unsigned>::from_string_impl(serialized_form);
  }
};

template <>
struct PropertyTrait<float>
{
  typedef float ValueType;

  static std::string to_string(float value)
  {
      return serializable_impl<float>::to_string_impl(value);
  }

  static std::pair<float, bool> from_string(std::string const& serialized_form)
  {
      return serializable_impl<float>::from_string_impl(serialized_form);
  }
};


template <>
struct PropertyTrait<double>
{
  typedef double ValueType;

  static std::string to_string(double value)
  {
      return serializable_impl<double>::to_string_impl(value);
  }

  static std::pair<double, bool> from_string(std::string const& serialized_form)
  {
      return serializable_impl<double>::from_string_impl(serialized_form);
  }
};


template <>
struct PropertyTrait<bool>
{
  typedef bool ValueType;

  static std::string to_string(bool value)
  {
      return value ? "true" : "false";
  }

  static std::pair<bool, bool> from_string(std::string const& serialized_form)
  {
    if (serialized_form == "true") {
      return std::make_pair(true, true);
    } else if (serialized_form == "false") {
      return std::make_pair(false, true);
    } else {
      return std::make_pair(false, false);
    }
  }
};


template <>
struct PropertyTrait<std::string>
{
  typedef std::string ValueType;

  static std::string to_string(std::string const& value)
  {
      return value;
  }

  static std::pair<std::string, bool> from_string(std::string const& serialized_form)
  {
    return std::make_pair(serialized_form, true);
  }
};


// template <>
// struct PropertyTrait<Color>
// {
//   typedef Color ValueType;

//   static std::string to_string(Color value)
//   {
//     std::string str = std::string("[") +
//       serializable_impl<float>::to_string_impl(value.red) + std::string(" ") +
//       serializable_impl<float>::to_string_impl(value.green) + std::string(" ") +
//       serializable_impl<float>::to_string_impl(value.blue) + std::string(" ") +
//       serializable_impl<float>::to_string_impl(value.alpha) + std::string(" ") +
//       std::string("]");

//     return str;
//   }

//   static std::pair<Color, bool> from_string(std::string const& serialized_form)
//   {
//     // todo
//     return std::make_pair<Color, bool>(color::Red, true);
//   }
// };

}
}

#endif
