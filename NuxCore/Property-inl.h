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
#ifndef NUXCORE_PROPERTY_INL_H
#define NUXCORE_PROPERTY_INL_H

namespace nux {

template <typename VALUE_TYPE>
PropertyChangedSignal<VALUE_TYPE>::PropertyChangedSignal()
  : notify_(true)
{}

template <typename VALUE_TYPE>
void PropertyChangedSignal<VALUE_TYPE>::DisableNotifications()
{
  notify_ = false;
}

template <typename VALUE_TYPE>
void PropertyChangedSignal<VALUE_TYPE>::EnableNotifications()
{
  notify_ = true;
}

template <typename VALUE_TYPE>
void PropertyChangedSignal<VALUE_TYPE>::EmitChanged(VALUE_TYPE const& new_value)
{
  if (notify_)
    changed.emit(new_value);
}


template <typename VALUE_TYPE>
Property<VALUE_TYPE>::Property()
  : value_(VALUE_TYPE())
  , setter_function_(std::bind(&Property<VALUE_TYPE>::DefaultSetter, this,
                               std::placeholders::_1, std::placeholders::_2))
{}

template <typename VALUE_TYPE>
Property<VALUE_TYPE>::Property(VALUE_TYPE const& initial)
  : value_(initial)
  , setter_function_(std::bind(&Property<VALUE_TYPE>::DefaultSetter, this,
                               std::placeholders::_1, std::placeholders::_2))
{}

template <typename VALUE_TYPE>
Property<VALUE_TYPE>::Property(VALUE_TYPE const& initial,
                               SetterFunction setter_function)
  : value_(initial)
  , setter_function_(setter_function)
{}

template <typename VALUE_TYPE>
VALUE_TYPE Property<VALUE_TYPE>::operator=(VALUE_TYPE const& value)
{
  return Set(value);
}

template <typename VALUE_TYPE>
Property<VALUE_TYPE>::operator VALUE_TYPE() const
{
  return value_;
}

template <typename VALUE_TYPE>
VALUE_TYPE Property<VALUE_TYPE>::operator()() const
{
  return value_;
}

template <typename VALUE_TYPE>
VALUE_TYPE Property<VALUE_TYPE>::operator()(VALUE_TYPE const& value)
{
  return Set(value);
}

template <typename VALUE_TYPE>
VALUE_TYPE Property<VALUE_TYPE>::Get() const
{
  return value_;
}

template <typename VALUE_TYPE>
VALUE_TYPE Property<VALUE_TYPE>::Set(VALUE_TYPE const& value)
{
  if (setter_function_(value_, value))
    SignalBase::EmitChanged(value_);
  return value_;
}

template <typename VALUE_TYPE>
bool Property<VALUE_TYPE>::DefaultSetter(VALUE_TYPE& target,
                                         VALUE_TYPE const& value)
{
  bool changed = false;
  if (target != value) {
    target = value;
    changed = true;
  }
  return changed;
}

template <typename VALUE_TYPE>
void Property<VALUE_TYPE>::SetSetterFunction(SetterFunction setter_function)
{
  setter_function_ = setter_function;
}


template <typename VALUE_TYPE>
ROProperty<VALUE_TYPE>::ROProperty()
  : getter_function_(std::bind(&ROProperty<VALUE_TYPE>::DefaultGetter, this))
{}

template <typename VALUE_TYPE>
ROProperty<VALUE_TYPE>::ROProperty(GetterFunction getter_function)
  : getter_function_(getter_function)
{}

template <typename VALUE_TYPE>
ROProperty<VALUE_TYPE>::operator VALUE_TYPE() const
{
  return getter_function_();
}

template <typename VALUE_TYPE>
VALUE_TYPE ROProperty<VALUE_TYPE>::operator()() const
{
  return getter_function_();
}

template <typename VALUE_TYPE>
VALUE_TYPE ROProperty<VALUE_TYPE>::Get() const
{
  return getter_function_();
}

template <typename VALUE_TYPE>
VALUE_TYPE ROProperty<VALUE_TYPE>::DefaultGetter() const
{
  return VALUE_TYPE();
}

template <typename VALUE_TYPE>
void ROProperty<VALUE_TYPE>::SetGetterFunction(GetterFunction getter_function)
{
  getter_function_ = getter_function;
}


template <typename VALUE_TYPE>
RWProperty<VALUE_TYPE>::RWProperty()
  : getter_function_(std::bind(&RWProperty<VALUE_TYPE>::DefaultGetter, this))
  , setter_function_(std::bind(&RWProperty<VALUE_TYPE>::DefaultSetter, this,
                               std::placeholders::_1))
{}

template <typename VALUE_TYPE>
RWProperty<VALUE_TYPE>::RWProperty(GetterFunction getter_function,
                                   SetterFunction setter_function)
  : getter_function_(getter_function)
  , setter_function_(setter_function)
{}

template <typename VALUE_TYPE>
VALUE_TYPE RWProperty<VALUE_TYPE>::operator=(VALUE_TYPE const& value)
{
  return Set(value);
}

template <typename VALUE_TYPE>
RWProperty<VALUE_TYPE>::operator VALUE_TYPE() const
{
  return getter_function_();
}

template <typename VALUE_TYPE>
VALUE_TYPE RWProperty<VALUE_TYPE>::operator()() const
{
  return getter_function_();
}

template <typename VALUE_TYPE>
VALUE_TYPE RWProperty<VALUE_TYPE>::operator()(VALUE_TYPE const& value)
{
  return Set(value);
}

template <typename VALUE_TYPE>
VALUE_TYPE RWProperty<VALUE_TYPE>::Get() const
{
  return getter_function_();
}

template <typename VALUE_TYPE>
VALUE_TYPE RWProperty<VALUE_TYPE>::Set(VALUE_TYPE const& value)
{
  if (setter_function_(value))
  {
    VALUE_TYPE new_value = getter_function_();
    SignalBase::EmitChanged(new_value);
    return new_value;
  }
  return getter_function_();
}

template <typename VALUE_TYPE>
VALUE_TYPE RWProperty<VALUE_TYPE>::DefaultGetter() const
{
  return VALUE_TYPE();
}

template <typename VALUE_TYPE>
bool RWProperty<VALUE_TYPE>::DefaultSetter(VALUE_TYPE const& /* value */)
{
  return false;
}

template <typename VALUE_TYPE>
void RWProperty<VALUE_TYPE>::SetSetterFunction(SetterFunction setter_function)
{
  setter_function_ = setter_function;
}

template <typename VALUE_TYPE>
void RWProperty<VALUE_TYPE>::SetGetterFunction(GetterFunction getter_function)
{
  getter_function_ = getter_function;
}


// We need to provide a default constructor since we hide the copy ctor.
inline Introspectable::Introspectable()
{}

inline void Introspectable::AddProperty(std::string const& name,
                                        PropertyBase* property)
{
  // check to see if it exists and if it does barf horribly as we can't
  // have two properties with the same name;
  properties_[name] = property;
}

inline bool Introspectable::SetProperty(std::string const& name,
                                        const char* value)
{
  PropertyContainer::iterator i = properties_.find(name);
  if (i == properties_.end())
    return false;
  else
    return i->second->SetValue(value);
}

template <typename T>
bool Introspectable::SetProperty(std::string const& name, T const& value)
{
  PropertyContainer::iterator i = properties_.find(name);
  if (i == properties_.end())
    return false;
  else
  {
    return i->second->SetValue(type::PropertyTrait<T>::to_string(value));
  }
}

template <typename T>
T Introspectable::GetProperty(std::string const& name, T* /* foo */)
{
  PropertyContainer::iterator i = properties_.find(name);
  if (i == properties_.end())
    return T();

  std::string s = i->second->GetSerializedValue();
  std::pair<T, bool> result = type::PropertyTrait<T>::from_string(s);
  // If this is called with a template type that the property does not
  // support nice conversion to, you'll get no error, but will get
  // a default constructed T.  We could use an exception here.
  return result.first;
}


template <typename T>
SerializableProperty<T>::SerializableProperty(Introspectable* owner,
                                              std::string const& name)
  : Base()
  , name_(name)
{
  owner->AddProperty(name, this);
}

template <typename T>
SerializableProperty<T>::SerializableProperty(Introspectable* owner,
                                              std::string const& name,
                                              T const& initial)
  : Base(initial)
  , name_(name)
{
  owner->AddProperty(name, this);
}

template <typename T>
bool SerializableProperty<T>::SetValue(std::string const& serialized_form)
{
  std::pair<ValueType, bool> result = TraitType::from_string(serialized_form);
  if (result.second) {
    Base::Set(result.first);
  }
  return result.second;
}

template <typename T>
std::string SerializableProperty<T>::GetSerializedValue() const
{
  return TraitType::to_string(Base::Get());
}

template <typename T>
T SerializableProperty<T>::operator=(T const& value)
{
  Base::Set(value);
  // There are no arguments to ‘get’ that depend on a template parameter,
  // so we explicitly specify Base.
  return Base::Get();
}



}

#endif
