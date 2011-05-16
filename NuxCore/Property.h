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

  ConnectableProperty();
  ConnectableProperty(VALUE_TYPE const& initial);

  VALUE_TYPE const& operator=(VALUE_TYPE const& value);
  operator VALUE_TYPE const & () const;

  // function call access
  VALUE_TYPE const& operator()() const;
  void operator()(VALUE_TYPE const& value);

  // get and set access
  VALUE_TYPE const& get() const;
  void set(VALUE_TYPE const& value);

  sigc::signal<void, VALUE_TYPE const&> changed;

  void disable_notifications();
  void enable_notifications();

private:
  // Properties themselves are not copyable.
  ConnectableProperty(ConnectableProperty const&);
  ConnectableProperty& operator=(ConnectableProperty const&);

private:
  VALUE_TYPE value_;
  bool notify_;
};


class PropertyBase
{
public:
  virtual bool set_value(std::string const& serialized_form) = 0;
  virtual std::string get_serialized_value() const = 0;
};


class Introspectable
{
public:
  Introspectable();
  // Needs to have a container of properties

  /// If the property was not able to be set with the value, the method
  /// returns false.
  bool set_property(std::string const& name, const char* value);

  template <typename T>
  bool set_property(std::string const& name, T const& value);

  template <typename T>
  T get_property(std::string const& name, T* foo = 0);

  void add_property(std::string const& name, PropertyBase* property);

private:
  // Introspectable objects are not copyable.
  Introspectable(Introspectable const&);
  Introspectable& operator=(Introspectable const&);

private:
  typedef std::map<std::string, PropertyBase*> PropertyContainer;
  PropertyContainer properties_;
};


template <typename T>
class Property : public ConnectableProperty<T>, public PropertyBase
{
public:
  typedef ConnectableProperty<T> Base;
  typedef typename Base::ValueType ValueType;
  typedef typename Base::TraitType TraitType;

  Property(Introspectable* owner, std::string const& name);
  Property(Introspectable* owner, std::string const& name, T const& initial);

  virtual bool set_value(std::string const& serialized_form);
  virtual std::string get_serialized_value() const;

  // Operator assignment is not inherited nicely, so redeclare it here.
  ValueType const& operator=(ValueType const& value);

private:
  std::string name_;
};



}

#include "Property-inl.h"

#endif
