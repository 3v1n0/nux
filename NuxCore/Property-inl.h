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
ConnectableProperty<VALUE_TYPE>::ConnectableProperty()
  : value_(VALUE_TYPE())
{}

template <typename VALUE_TYPE>
ConnectableProperty<VALUE_TYPE>::ConnectableProperty(VALUE_TYPE const& initial)
  : value_(initial)
{}

template <typename VALUE_TYPE>
VALUE_TYPE const& ConnectableProperty<VALUE_TYPE>::operator=(VALUE_TYPE const& value)
{
  set(value);
  return value_;
}

template <typename VALUE_TYPE>
ConnectableProperty<VALUE_TYPE>::operator VALUE_TYPE const&() const
{
  return value_;
}

template <typename VALUE_TYPE>
VALUE_TYPE const& ConnectableProperty<VALUE_TYPE>::operator()() const
{
  return value_;
}

template <typename VALUE_TYPE>
void ConnectableProperty<VALUE_TYPE>::operator()(VALUE_TYPE const& value)
{
  set(value);
}

  // get and set access
template <typename VALUE_TYPE>
VALUE_TYPE const& ConnectableProperty<VALUE_TYPE>::get() const
{
  return value_;
}

template <typename VALUE_TYPE>
void ConnectableProperty<VALUE_TYPE>::set(VALUE_TYPE const& value)
{
  if (value != value_) {
    value_ = value;
    if (SignalBase::notify_) {
      SignalBase::changed.emit(value_);
    }
  }
}

// We need to provide a default constructor since we hide the copy ctor.
inline Introspectable::Introspectable()
{}

inline void Introspectable::add_property(std::string const& name,
                                         PropertyBase* property)
{
    // check to see if it exists and if it does barf horribly as we can't
    // have two properties with the same name;
    properties_[name] = property;
}

inline bool Introspectable::set_property(std::string const& name,
                                         const char* value)
{
    PropertyContainer::iterator i = properties_.find(name);
    if (i == properties_.end())
        return false;
    else
        return i->second->set_value(value);
}

template <typename T>
bool Introspectable::set_property(std::string const& name, T const& value)
{
    PropertyContainer::iterator i = properties_.find(name);
    if (i == properties_.end())
        return false;
    else
    {
        return i->second->set_value(type::PropertyTrait<T>::to_string(value));
    }
}

template <typename T>
T Introspectable::get_property(std::string const& name, T* foo)
{
    PropertyContainer::iterator i = properties_.find(name);
    if (i == properties_.end())
        return T();

    std::string s = i->second->get_serialized_value();
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
    owner->add_property(name, this);
}

template <typename T>
SerializableProperty<T>::SerializableProperty(Introspectable* owner,
                      std::string const& name,
                      T const& initial)
: Base(initial)
, name_(name)
{
    owner->add_property(name, this);
}

template <typename T>
bool SerializableProperty<T>::set_value(std::string const& serialized_form)
{
    std::pair<ValueType, bool> result = TraitType::from_string(serialized_form);
    if (result.second) {
      set(result.first);
    }
    return result.second;
}

template <typename T>
std::string SerializableProperty<T>::get_serialized_value() const
{
    return TraitType::to_string(Base::get());
}

template <typename T>
typename SerializableProperty<T>::ValueType const& SerializableProperty<T>::operator=(typename SerializableProperty<T>::ValueType const& value)
{
    set(value);
    // There are no arguments to ‘get’ that depend on a template parameter,
    // so we explicitly specify Base.
    return Base::get();
}



}

#endif
