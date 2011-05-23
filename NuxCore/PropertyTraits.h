#ifndef NUXCORE_PROPERTY_TRAITS_H
#define NUXCORE_PROPERTY_TRAITS_H

#include <string>
#include <boost/lexical_cast.hpp>

namespace nux {
namespace type {

/**
 * Here we want to have only explict type support, to avoid someone using the
 * system with something we haven't though of and getting upset when if it
 * doesn't work properly.
 */

template <typename T>
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
  static std::string to_string_impl(T const& value)
  {
    return boost::lexical_cast<std::string>(value);
  }

  static std::pair<T, bool> from_string_impl(std::string const& serialized_form)
  {
    try {
      return std::make_pair<T, bool>(
          boost::lexical_cast<T>(serialized_form), true);
    }
    catch (boost::bad_lexical_cast const& e) {
      return std::make_pair<T, bool>(T(), false);
    }
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


}
}

#endif
