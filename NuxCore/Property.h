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
#ifndef NUXCORE_PROPERTY_H
#define NUXCORE_PROPERTY_H

#include "PropertyTraits.h"

#include <string>
#include <map>
#include <sigc++/signal.h>

/**
 * Much of this property work is based on the work by Lois Goldthwaite,
 * SC22/WG21/N1615=04-0055 - C++ Properties -- a Library Solution
 *
 * The basic ideas were extended to add update notifications, and
 * serialisation and introspection.
 */
namespace nux {


template <typename VALUE_TYPE>
class PropertyChangedSignal
{
public:
  PropertyChangedSignal();

  sigc::signal<void, VALUE_TYPE const&> changed;

  void DisableNotifications();
  void EnableNotifications();

  void EmitChanged(VALUE_TYPE const& new_value);

private:
  bool notify_;
};

/**
 * A read/write property that stores the value type.
 *
 * The default setter emits the changed event if and only if the value
 * changes.  A custom setter can be provided by passing in a setter function
 * using sigc::mem_fun or sigc::ptr_fun.
 */
template <typename VALUE_TYPE>
class Property : public PropertyChangedSignal<VALUE_TYPE>
{
public:
  typedef PropertyChangedSignal<VALUE_TYPE> SignalBase;
  typedef sigc::slot<bool, VALUE_TYPE&, VALUE_TYPE const&> SetterFunction;

  Property();
  explicit Property(VALUE_TYPE const& initial);
  Property(VALUE_TYPE const& initial, SetterFunction setter_function);

  VALUE_TYPE operator=(VALUE_TYPE const& value);
  operator VALUE_TYPE() const;

  // function call access
  VALUE_TYPE operator()() const;
  void operator()(VALUE_TYPE const& value);

  // get and set access
  VALUE_TYPE Get() const;
  void Set(VALUE_TYPE const& value);

  void CustomSetterFunction(SetterFunction setter_function);

private:
  // Properties themselves are not copyable.
  Property(Property const&);
  Property& operator=(Property const&);

  bool DefaultSetter(VALUE_TYPE& target, VALUE_TYPE const& value);

private:
  VALUE_TYPE value_;
  SetterFunction setter_function_;
};

// We could easily add a Write Only Property if and when we need one.

/**
 * A read only property that uses a function to get the value.
 *
 * The read only property does not have a changed signal.
 *
 * The default constructor creates a read only property that always returns
 * the default constructed VALUE_TYPE.
 */
template <typename VALUE_TYPE>
class ROProperty
{
public:
  typedef sigc::slot<VALUE_TYPE> GetterFunction;

  ROProperty();
  explicit ROProperty(GetterFunction getter_function);

  operator VALUE_TYPE() const;
  VALUE_TYPE operator()() const;
  VALUE_TYPE Get() const;

  void CustomGetterFunction(GetterFunction getter_function);

private:
  // ROProperties themselves are not copyable.
  ROProperty(ROProperty const&);
  ROProperty& operator=(ROProperty const&);

  VALUE_TYPE DefaultGetter() const;

private:
  GetterFunction getter_function_;
};




template <typename VALUE_TYPE>
class ConnectableProperty : public PropertyChangedSignal<VALUE_TYPE>
{
public:
  typedef PropertyChangedSignal<VALUE_TYPE> SignalBase;
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

private:
  // Properties themselves are not copyable.
  ConnectableProperty(ConnectableProperty const&);
  ConnectableProperty& operator=(ConnectableProperty const&);

private:
  VALUE_TYPE value_;
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
class SerializableProperty : public ConnectableProperty<T>, public PropertyBase
{
public:
  typedef ConnectableProperty<T> Base;
  typedef typename Base::ValueType ValueType;
  typedef typename Base::TraitType TraitType;

  SerializableProperty(Introspectable* owner, std::string const& name);
  SerializableProperty(Introspectable* owner, std::string const& name, T const& initial);

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
