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
#include <functional>

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
  typedef VALUE_TYPE ValueType;
  typedef PropertyChangedSignal<VALUE_TYPE> SignalBase;
  typedef std::function<bool(VALUE_TYPE&, VALUE_TYPE const&)> SetterFunction;

  Property();
  explicit Property(VALUE_TYPE const& initial);
  Property(VALUE_TYPE const& initial, SetterFunction setter_function);

  VALUE_TYPE operator=(VALUE_TYPE const& value);
  operator VALUE_TYPE() const;

  // function call access
  VALUE_TYPE operator()() const;
  VALUE_TYPE operator()(VALUE_TYPE const& value);

  // get and set access
  VALUE_TYPE Get() const;
  VALUE_TYPE Set(VALUE_TYPE const& value);

  void SetSetterFunction(SetterFunction setter_function);

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
 * The default constructor creates a read only property that always returns
 * the default constructed VALUE_TYPE.
 */
template <typename VALUE_TYPE>
class ROProperty : public PropertyChangedSignal<VALUE_TYPE>
{
public:
  typedef VALUE_TYPE ValueType;
  typedef std::function<VALUE_TYPE()> GetterFunction;

  ROProperty();
  explicit ROProperty(GetterFunction getter_function);

  operator VALUE_TYPE() const;
  VALUE_TYPE operator()() const;
  VALUE_TYPE Get() const;

  void SetGetterFunction(GetterFunction getter_function);

private:
  // ROProperties themselves are not copyable.
  ROProperty(ROProperty const&);
  ROProperty& operator=(ROProperty const&);

  VALUE_TYPE DefaultGetter() const;

private:
  GetterFunction getter_function_;
};

/**
 * A read/write property that uses a functions to get and set the value.
 *
 * The value type is not stored in the propery, but maintained by the setter
 * and getter functions.
 *
 * A changed signal is emitted if the setter function specifies that the value
 * has changed.
 *
 * The default setter does nothing and emits no signal, and the default getter
 * returns a default constructed VALUE_TYPE.  The default getter and setter
 * should be overridden through either the constructor args or through the
 * SetGetterFunction / SetSetterFunction.
 */
template <typename VALUE_TYPE>
class RWProperty : public PropertyChangedSignal<VALUE_TYPE>
{
public:
  typedef VALUE_TYPE ValueType;
  typedef PropertyChangedSignal<VALUE_TYPE> SignalBase;
  typedef std::function<bool(VALUE_TYPE const&)> SetterFunction;
  typedef std::function<VALUE_TYPE()> GetterFunction;

  RWProperty();
  RWProperty(GetterFunction getter_function, SetterFunction setter_function);

  VALUE_TYPE operator=(VALUE_TYPE const& value);
  operator VALUE_TYPE() const;

  // function call access
  VALUE_TYPE operator()() const;
  VALUE_TYPE operator()(VALUE_TYPE const& value);

  // get and set access
  VALUE_TYPE Get() const;
  VALUE_TYPE Set(VALUE_TYPE const& value);

  void SetGetterFunction(GetterFunction getter_function);
  void SetSetterFunction(SetterFunction setter_function);

private:
  // RWProperties themselves are not copyable.
  RWProperty(RWProperty const&);
  RWProperty& operator=(RWProperty const&);

  VALUE_TYPE DefaultGetter() const;
  bool DefaultSetter(VALUE_TYPE const& value);

private:
  GetterFunction getter_function_;
  SetterFunction setter_function_;
};


class PropertyBase
{
public:
  virtual ~PropertyBase() {}
  virtual bool SetValue(std::string const& serialized_form) = 0;
  virtual std::string GetSerializedValue() const = 0;
};


class Introspectable
{
public:
  Introspectable();
  // Needs to have a container of properties

  /// If the property was not able to be set with the value, the method
  /// returns false.
  bool SetProperty(std::string const& name, const char* value);

  template <typename T>
  bool SetProperty(std::string const& name, T const& value);

  template <typename T>
  T GetProperty(std::string const& name, T* foo = 0);

  void AddProperty(std::string const& name, PropertyBase* property);

private:
  // Introspectable objects are not copyable.
  Introspectable(Introspectable const&);
  Introspectable& operator=(Introspectable const&);

private:
  typedef std::map<std::string, PropertyBase*> PropertyContainer;
  PropertyContainer properties_;
};


template <typename VALUE_TYPE>
class SerializableProperty : public Property<VALUE_TYPE>, public PropertyBase
{
public:
  typedef Property<VALUE_TYPE> Base;
  typedef typename type::PropertyTrait<VALUE_TYPE> TraitType;
  typedef typename TraitType::ValueType ValueType;

  SerializableProperty(Introspectable* owner,
                       std::string const& name);
  SerializableProperty(Introspectable* owner,
                       std::string const& name,
                       VALUE_TYPE const& initial);

  virtual bool SetValue(std::string const& serialized_form);
  virtual std::string GetSerializedValue() const;

  // Operator assignment is not inherited nicely, so redeclare it here.
  VALUE_TYPE operator=(VALUE_TYPE const& value);

private:
  std::string name_;
};



}

#include "Property-inl.h"
#include "PropertyOperators.h"

#endif
