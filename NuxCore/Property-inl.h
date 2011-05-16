#ifndef NUXCORE_PROPERTY_INL_H
#define NUXCORE_PROPERTY_INL_H

namespace nux {

template <typename VALUE_TYPE>
ConnectableProperty<VALUE_TYPE>::ConnectableProperty()
  : value_(VALUE_TYPE())
  , notify_(true)
{}

template <typename VALUE_TYPE>
ConnectableProperty<VALUE_TYPE>::ConnectableProperty(VALUE_TYPE const& initial)
  : value_(initial)
  , notify_(true)
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

template <typename VALUE_TYPE>
void ConnectableProperty<VALUE_TYPE>::disable_notifications()
{
  notify_ = false;
}

template <typename VALUE_TYPE>
void ConnectableProperty<VALUE_TYPE>::enable_notifications()
{
  notify_ = true;
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
    if (notify_) {
      changed.emit(value_);
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
    std::string s = properties_[name]->get_serialized_value();
    std::pair<T, bool> result = type::PropertyTrait<T>::from_string(s);
    // If this is called with a template type that the property does not
    // support nice conversion to, you'll get no error, but will get
    // a default constructed T.  We could use an exception here.
    return result.first;
}


template <typename T>
Property<T>::Property(Introspectable* owner,
                      std::string const& name)
: Base()
, name_(name)
{
    owner->add_property(name, this);
}

template <typename T>
Property<T>::Property(Introspectable* owner,
                      std::string const& name,
                      T const& initial)
: Base(initial)
, name_(name)
{
    owner->add_property(name, this);
}

template <typename T>
bool Property<T>::set_value(std::string const& serialized_form)
{
    std::pair<ValueType, bool> result = TraitType::from_string(serialized_form);
    if (result.second) {
      set(result.first);
    }
    return result.second;
}

template <typename T>
std::string Property<T>::get_serialized_value() const
{
    return TraitType::to_string(Base::get());
}

template <typename T>
typename Property<T>::ValueType const& Property<T>::operator=(typename Property<T>::ValueType const& value)
{
    set(value);
    // There are no arguments to ‘get’ that depend on a template parameter,
    // so we explicitly specify Base.
    return Base::get();
}



}

#endif
