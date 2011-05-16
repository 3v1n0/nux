#ifndef NUXCORE_PROPERTY_H
#define NUXCORE_PROPERTY_H

#include "PropertyTraits.h"

#include <map>
#include <sigc++/signal.h>


namespace nux {

// TODO:
//  object serialisation

template <typename VALUE_TYPE>
class ConnectableProperty
{
public:
  typedef typename type::PropertyTrait<VALUE_TYPE> TraitType;
  typedef typename TraitType::ValueType ValueType;

  ConnectableProperty()
  : value_(ValueType())
  , notify_(true)
  {}

  ConnectableProperty(ValueType const& initial)
  : value_(initial)
  , notify_(true)
  {}

  ValueType& operator=(ValueType const& value)
  {
      set(value);
      return value_;
  }

  operator ValueType const & () const {
    return value_;
  }

  // function call access
  ValueType const& operator()() const
  {
    return value_;
  }

  // function call access
  void operator()(ValueType const& value)
  {
      set(value);
  }

  sigc::signal<void, ValueType const&> changed;

  void disable_notifications()
  {
    notify_ = false;
  }

  void enable_notifications()
  {
    notify_ = true;
  }

  ValueType const& get() const
  {
      return value_;
  }

  void set(ValueType const& value)
  {
      if (value != value_) {
        value_ = value;
        if (notify_) {
          changed.emit(value_);
        }
      }
  }

private:
  ValueType value_;
  bool notify_;
};


class PropertyBase
{
public:
  virtual bool set_value(std::string const& serialized_form) = 0;
  virtual std::string get_serialized_value() = 0;
};


class Introspectable
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
          type::PropertyTrait<T>::to_string(value));
   }

  template <typename T>
  T get_property(std::string const& name, T* foo = 0)
    {
      std::string s = properties_[name]->get_serialized_value();
      std::pair<T, bool> result = type::PropertyTrait<T>::from_string(s);
      // assert(result.second); -- should never fail, but we don't want an assert.
      return result.first;
    }

  void add_property(std::string const& name, PropertyBase* property)
    {
      // check to see if it exists and if it does barf horribly as we can't
      // have two properties with the same name;
      properties_[name] = property;
    }

private:
  typedef std::map<std::string, PropertyBase*> PropertyContainer;
  PropertyContainer properties_;
};


template <typename T>
class Property : public ConnectableProperty<T>, public PropertyBase
{
public:
    typedef ConnectableProperty<T> Base;
    typedef typename Base::TraitType TraitType;

  Property(Introspectable* owner, std::string const& name)
    : Base()
    , name_(name)
    {
      owner->add_property(name, this);
    }

  Property(Introspectable* owner, std::string const& name, T const& initial)
    : Base(initial)
    , name_(name)
    {
        owner->add_property(name, this);
    }

  virtual bool set_value(std::string const& serialized_form)
    {
        std::pair<T, bool> result = TraitType::from_string(serialized_form);
        if (result.second) {
          set(result.first);
        }
        return result.second;
    }
  virtual std::string get_serialized_value()
    {
      return TraitType::to_string(Base::get());
    }

private:
  std::string name_;
};



}

#endif
