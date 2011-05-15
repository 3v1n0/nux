#ifndef NUXCORE_PROPERTY_H
#define NUXCORE_PROPERTY_H

#include "PropertyTraits.h"

#include <map>
#include <sigc++/signal.h>


namespace nux {

// TODO:
//  range type trait checks
//  object serialisation

template <typename VALUE_TYPE,
          template <typename> class TRAITS = type::assignable>
class ConnectableProperty
{
public:
  typedef TRAITS<VALUE_TYPE> TraitsType;

  ConnectableProperty()
  : value_(VALUE_TYPE())
  , notify_(true)
  {}

  ConnectableProperty(VALUE_TYPE const& initial)
  : value_(initial)
  , notify_(true)
  {}

  template <typename T2>
  VALUE_TYPE& operator=(const T2& value)
  {
      bool is_changed = false;
      if (value != value_) {
        is_changed = TraitsType::assign(value_, value);
      }
      if (is_changed && notify_) {
        changed.emit(value_);
      }
      return value_;
  }

  operator VALUE_TYPE const & () const {
    return value_;
  }

  VALUE_TYPE const& value() const
  {
    return value_;
  }

  sigc::signal<void, VALUE_TYPE const&> changed;

  void freeze()
  {
    notify_ = false;
  }

  void thaw()
  {
    notify_ = true;
  }

private:
  VALUE_TYPE value_;
  bool notify_;
};




class property_base
{
public:
  virtual bool set_value(std::string const& serialized_form) = 0;
  virtual std::string get_serialized_value() = 0;
};


class introspectable
{
public:
  // Needs to have a container of properties

  /// If the property was not able to be set with the value, the method
  /// returns false.
  template <typename T>
  bool set_property(std::string const& name, T const& value)
    {
      // TODO: use boost::serialisation for a binary format?
      // TODO: use an iterator to check the property exists.
      // find the property and set the value...
      // make this nicer
      return properties_[name]->set_value(
          type::serializable<T>::to_string(value));
   }

  template <typename T>
  T get_property(std::string const& name, T* foo = 0)
    {
      std::string s = properties_[name]->get_serialized_value();
      std::pair<T, bool> result = type::serializable<T>::from_string(s);
      // assert(result.second); -- should never fail, but we don't want an assert.
      return result.first;
    }

  void add_property(std::string const& name, property_base* property)
    {
      // check to see if it exists and if it does barf horribly as we can't
      // have two properties with the same name;
      properties_[name] = property;
    }

private:
  typedef std::map<std::string, property_base*> PropertyContainer;
  PropertyContainer properties_;
};




template <typename T, typename TRAITS = type::assignable<T> >
class property : public property_base
{
public:
  property(introspectable* owner, std::string const& name) : name_(name), value_(T())
    {
      owner->add_property(name, this);
    }
  property(introspectable* owner, std::string const& name, T const& initial) : name_(name), value_(initial)
    {
      owner->add_property(name, this);
    }

  T & operator = (const T &i) {
    return value_ = i;
  }

  // Think about a traits type that is used for assignment to allow
  // the specialisation for std::string to get assignment from char*
  // but disallow the general case.

  // This template class member function template serves the purpose to make
  // typing more strict. Assignment to this is only possible with exact identical
  // types.
  template <typename T2> T2 & operator = (const T2 &i) {
    T2 &guard = value_;
    throw guard; // Never reached.
  }

  operator T const & () const {
    return value_;
  }

  virtual bool set_value(std::string const& serialized_form)
    {
        std::pair<T, bool> result = TRAITS::from_string(serialized_form);
        if (result.second) {
          value_ = result.first;
        }
        return result.second;
    }
  virtual std::string get_serialized_value()
    {
      return TRAITS::to_string(value_);
    }

private:
  std::string name_;
  T value_;
};



}

#endif
