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
struct Assignable
{
    /**
     * Expected content:
     *
     * static bool assign(int& var, int new_value)
     */
};

template <typename T>
struct Serializable
{
    /**
     * Expected content:
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
 * The Serializable_impl template has different signatures than the standard
 * Serializable traits class to avoid the generalised template being used as a
 * traits type when we really don't want it being used in that way, but we do
 * want a generalized implementation for the types we do care about.
 */
template <typename T>
struct Serializable_impl
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
struct Serializable<int>
{
  static std::string to_string(int value)
  {
      return Serializable_impl<int>::to_string_impl(value);
  }

  static std::pair<int, bool> from_string(std::string const& serialized_form)
  {
      return Serializable_impl<int>::from_string_impl(serialized_form);
  }
};

template <>
struct Assignable<int> : Serializable<int>
{
  static bool assign(int& var, int new_value)
  {
    var = new_value;
    return true;
  }
};



template <>
struct Serializable<float>
{
  static std::string to_string(float value)
  {
      return Serializable_impl<float>::to_string_impl(value);
  }

  static std::pair<float, bool> from_string(std::string const& serialized_form)
  {
      return Serializable_impl<float>::from_string_impl(serialized_form);
  }
};

template <>
struct Assignable<float> : Serializable<float>
{
  static bool assign(float& var, float new_value)
  {
    var = new_value;
    return true;
  }
};


template <>
struct Serializable<double>
{
  static std::string to_string(double value)
  {
      return Serializable_impl<double>::to_string_impl(value);
  }

  static std::pair<double, bool> from_string(std::string const& serialized_form)
  {
      return Serializable_impl<double>::from_string_impl(serialized_form);
  }
};

template <>
struct Assignable<double> : Serializable<double>
{
  static bool assign(double& var, double new_value)
  {
    var = new_value;
    return true;
  }
};


template <>
struct Serializable<bool>
{
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
struct Assignable<bool> : Serializable<bool>
{
  static bool assign(bool& var, bool new_value)
  {
    var = new_value;
    return true;
  }
};


template <>
struct Serializable<std::string>
{
  static std::string to_string(std::string const& value)
    {
      return value;
    }

  static std::pair<std::string, bool> from_string(std::string const& serialized_form)
    {
      return std::make_pair(serialized_form, true);
    }
};

template <>
struct Assignable<std::string> : Serializable<std::string>
{
  static bool assign(std::string& var, std::string const& new_value)
  {
    var = new_value;
    return true;
  }
};

}
}

#endif
