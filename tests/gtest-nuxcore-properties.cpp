#include "NuxCore/Property.h"

#include <boost/scoped_ptr.hpp>
#include <sigc++/trackable.h>

#include <gmock/gmock.h>
#include <vector>
#include <stdexcept>

#include "Helpers.h"

using namespace testing;

namespace nt = nux::testing;

namespace {

template <typename T>
T to_and_from_string(T const& value)
{
  std::string temp = nux::type::PropertyTrait<T>::to_string(value);
  std::pair<T, bool> result = nux::type::PropertyTrait<T>::from_string(temp);
  if (!result.second) {
    throw std::runtime_error("Failed to convert.");
  }
  return result.first;
}

enum TestEnum
{
  FirstValue,
  MiddleValue,
  LastValue,
};

struct TestClass
{
  int value;
};


TEST(TestTypeTraits, TestSerialization) {
  EXPECT_EQ("hello", nux::type::PropertyTrait<std::string>::to_string("hello"));
  EXPECT_EQ("42", nux::type::PropertyTrait<int>::to_string(42));
  EXPECT_EQ("42", nux::type::PropertyTrait<unsigned>::to_string(42));
  EXPECT_EQ("true", nux::type::PropertyTrait<bool>::to_string(true));
  EXPECT_EQ("false", nux::type::PropertyTrait<bool>::to_string(false));
  EXPECT_EQ("0", nux::type::PropertyTrait<double>::to_string(0));
  EXPECT_EQ("25.5", nux::type::PropertyTrait<double>::to_string(25.5));
  EXPECT_EQ("1", nux::type::PropertyTrait<TestEnum>::to_string(MiddleValue));
}

TEST(TestTypeTraits, TestDeserialization) {
  std::pair<std::string, bool> sr = nux::type::PropertyTrait<std::string>::from_string("hello");
  EXPECT_EQ("hello", sr.first);
  EXPECT_TRUE(sr.second);
  std::pair<int, bool> int_result = nux::type::PropertyTrait<int>::from_string("42");
  EXPECT_EQ(42, int_result.first);
  EXPECT_TRUE(int_result.second);
  int_result = nux::type::PropertyTrait<int>::from_string("OMG!");
  EXPECT_EQ(0, int_result.first);
  EXPECT_FALSE(int_result.second);

  std::pair<unsigned, bool> unsigned_result = nux::type::PropertyTrait<unsigned>::from_string("42");
  EXPECT_EQ(42, unsigned_result.first);
  EXPECT_TRUE(unsigned_result.second);
  unsigned_result = nux::type::PropertyTrait<unsigned>::from_string("OMG!");
  EXPECT_EQ(0, unsigned_result.first);
  EXPECT_FALSE(unsigned_result.second);

  std::pair<bool, bool> bool_result = nux::type::PropertyTrait<bool>::from_string("true");
  EXPECT_TRUE(bool_result.first);
  EXPECT_TRUE(bool_result.second);
  bool_result = nux::type::PropertyTrait<bool>::from_string("false");
  EXPECT_FALSE(bool_result.first);
  EXPECT_TRUE(bool_result.second);
  bool_result = nux::type::PropertyTrait<bool>::from_string("what?");
  EXPECT_FALSE(bool_result.first);
  EXPECT_FALSE(bool_result.second);
  std::pair<double, bool> double_result = nux::type::PropertyTrait<double>::from_string("25.5");
  EXPECT_EQ(25.5, double_result.first);
  EXPECT_TRUE(double_result.second);
  double_result = nux::type::PropertyTrait<double>::from_string("2e6");
  EXPECT_EQ(2000000.0, double_result.first);
  EXPECT_TRUE(double_result.second);
  double_result = nux::type::PropertyTrait<double>::from_string("what?");
  EXPECT_EQ(0, double_result.first);
  EXPECT_FALSE(double_result.second);

  std::pair<TestEnum, bool> enum_result = nux::type::PropertyTrait<TestEnum>::from_string("0");
  EXPECT_EQ(FirstValue, enum_result.first);
  EXPECT_TRUE(enum_result.second);
  // This is tested to show behaviour even though it is undesirable (as there
  // isn't an enum value for 42).
  enum_result = nux::type::PropertyTrait<TestEnum>::from_string("42");
  EXPECT_EQ(42, enum_result.first);
  EXPECT_TRUE(enum_result.second);
}

TEST(TestTypeTraits, TestConversionHolds) {
  std::string string_value("Hello World!");
  EXPECT_EQ(string_value, to_and_from_string(string_value));
  double double_value = 34.345;
  EXPECT_EQ(double_value, to_and_from_string(double_value));
}


TEST(TestProperty, TestDefaultConstructor) {
  nux::Property<std::string> string_prop;
  // Need either an assignment or static cast to check the operator VALUE_TYPE
  // due to google-mock's template matching.
  std::string value = string_prop;
  EXPECT_THAT(value, Eq(""));
  EXPECT_THAT(string_prop.Get(), Eq(""));
  EXPECT_THAT(string_prop(), Eq(""));
}

TEST(TestProperty, TestValueExplicitConstructor) {
  nux::Property<std::string> string_prop("Hello world!");
  // Need either an assignment or static cast to check the operator VALUE_TYPE
  // due to google-mock's template matching.
  std::string value = string_prop;
  EXPECT_THAT(value, Eq("Hello world!"));
  EXPECT_THAT(string_prop.Get(), Eq("Hello world!"));
  EXPECT_THAT(string_prop(), Eq("Hello world!"));
}

TEST(TestProperty, TestAssignment) {
  nux::Property<std::string> string_prop;
  // Need either an assignment or static cast to check the operator VALUE_TYPE
  // due to google-mock's template matching.
  string_prop = "Assignment operator";
  std::string value = string_prop;
  EXPECT_THAT(value, Eq("Assignment operator"));
  EXPECT_THAT(string_prop.Get(), Eq("Assignment operator"));
  EXPECT_THAT(string_prop(), Eq("Assignment operator"));

  string_prop.Set("Set method");
  value = string_prop;
  EXPECT_THAT(value, Eq("Set method"));
  EXPECT_THAT(string_prop.Get(), Eq("Set method"));
  EXPECT_THAT(string_prop(), Eq("Set method"));

  string_prop("Function call assignment");
  value = string_prop;
  EXPECT_THAT(value, Eq("Function call assignment"));
  EXPECT_THAT(string_prop.Get(), Eq("Function call assignment"));
  EXPECT_THAT(string_prop(), Eq("Function call assignment"));
}

TEST(TestProperty, TestChanged) {
  nux::Property<std::string> string_prop;
  nt::ChangeRecorder<std::string> recorder;
  string_prop.changed.connect(recorder.listener());

  string_prop = "Hello world" ;
  EXPECT_THAT(1, Eq(recorder.size()));
  EXPECT_THAT("Hello world", Eq(recorder.last()));
  // No notification if not changed.
  string_prop = std::string("Hello world");
  EXPECT_THAT(1, Eq(recorder.size()));
}

TEST(TestProperty, TestEnableAndDisableNotifications) {
  nux::Property<std::string> string_prop;
  nt::ChangeRecorder<std::string> recorder;
  string_prop.changed.connect(recorder.listener());

  string_prop.DisableNotifications();
  string_prop = "Hello world" ;
  EXPECT_THAT(0, Eq(recorder.size()));

  string_prop.EnableNotifications();
  // No notification if not changed.
  string_prop = "Hello world" ;
  EXPECT_THAT(0, Eq(recorder.size()));

  string_prop = "New value" ;
  EXPECT_THAT(1, Eq(recorder.size()));
  EXPECT_THAT("New value", Eq(recorder.last()));
}

bool string_prefix(std::string& target, std::string const& value)
{
  bool changed = false;
  std::string prefixed("prefix-" + value);
  if (target != prefixed)
  {
    target = prefixed;
    changed = true;
  }
  return changed;
}

TEST(TestProperty, TestSetterConstructor) {
  nux::Property<std::string> string_prop("", sigc::ptr_fun(&string_prefix));

  string_prop = "foo";
  // Need either an assignment or static cast to check the operator VALUE_TYPE
  // due to google-mock's template matching.
  std::string value = string_prop;
  EXPECT_THAT(value, Eq("prefix-foo"));
  EXPECT_THAT(string_prop.Get(), Eq("prefix-foo"));
  EXPECT_THAT(string_prop(), Eq("prefix-foo"));
}

class FloatClamp
{
public:
  FloatClamp(float min, float max)
    : min_(min), max_(max)
    {
    }
  bool Set(float& target, float const& value)
    {
      bool changed = false;
      float new_val = std::min(max_, std::max(min_, value));
      if (target != new_val) {
        target = new_val;
        changed = true;
      }
      return changed;
    }
private:
  float min_;
  float max_;
};

TEST(TestProperty, TestCustomSetterFunction) {
  nux::Property<float> float_prop;
  FloatClamp clamp(0, 1);
  float_prop.SetSetterFunction(sigc::mem_fun(&clamp, &FloatClamp::Set));
  nt::ChangeRecorder<float> recorder;
  float_prop.changed.connect(recorder.listener());

  // Since the default value for a float is zero, and we clamp at zero,
  // setting to a negative value will result in setting to zero, which will
  // not signal a changed event.
  float_prop = -2;
  EXPECT_THAT(float_prop(), Eq(0));
  EXPECT_THAT(0, Eq(recorder.size()));

  float_prop = 0.5;
  EXPECT_THAT(float_prop(), Eq(0.5));
  EXPECT_THAT(1, Eq(recorder.size()));
  EXPECT_THAT(0.5, Eq(recorder.last()));

  float_prop = 4;
  EXPECT_THAT(float_prop(), Eq(1));
  EXPECT_THAT(2, Eq(recorder.size()));
  EXPECT_THAT(1, Eq(recorder.last()));
}

TEST(TestProperty, TestCustomSetterFunctionLambda) {
  nux::Property<float> float_prop;
  FloatClamp clamp(0, 1);
  float_prop.SetSetterFunction([&clamp] (float &target, float const& value) {
    return clamp.Set(target, value);
  });
  nt::ChangeRecorder<float> recorder;
  float_prop.changed.connect(recorder.listener());

  // Since the default value for a float is zero, and we clamp at zero,
  // setting to a negative value will result in setting to zero, which will
  // not signal a changed event.
  float_prop = -2;
  EXPECT_THAT(float_prop(), Eq(0));
  EXPECT_THAT(0, Eq(recorder.size()));

  float_prop = 0.5;
  EXPECT_THAT(float_prop(), Eq(0.5));
  EXPECT_THAT(1, Eq(recorder.size()));
  EXPECT_THAT(0.5, Eq(recorder.last()));

  float_prop = 4;
  EXPECT_THAT(float_prop(), Eq(1));
  EXPECT_THAT(2, Eq(recorder.size()));
  EXPECT_THAT(1, Eq(recorder.last()));
}

TEST(TestProperty, TestIntOperators) {
  nux::Property<int> int_prop(42);

  EXPECT_TRUE(int_prop == 42);
  EXPECT_TRUE(42 == int_prop);
  EXPECT_FALSE(int_prop != 42);
  EXPECT_FALSE(42 != int_prop);

  EXPECT_FALSE(int_prop == 5);
  EXPECT_FALSE(5 == int_prop);
  EXPECT_TRUE(int_prop != 5);
  EXPECT_TRUE(5 != int_prop);

  EXPECT_FALSE(int_prop < 5);
  EXPECT_FALSE(int_prop <= 5);
  EXPECT_TRUE(int_prop > 5);
  EXPECT_TRUE(int_prop >= 5);

  EXPECT_TRUE(5 < int_prop);
  EXPECT_TRUE(5 <= int_prop);
  EXPECT_FALSE(5 > int_prop);
  EXPECT_FALSE(5 >= int_prop);

  nux::Property<int> int_prop2(42);
  EXPECT_TRUE(int_prop2 == int_prop);
  EXPECT_FALSE(int_prop2 != int_prop);

  int_prop2 = 5;

  EXPECT_FALSE(int_prop2 == int_prop);
  EXPECT_TRUE(int_prop2 != int_prop);

  EXPECT_FALSE(int_prop < int_prop2);
  EXPECT_FALSE(int_prop <= int_prop2);
  EXPECT_TRUE(int_prop > int_prop2);
  EXPECT_TRUE(int_prop >= int_prop2);
}

// Only testing strings and ints, to show that the template classes work with
// both primitive types and classes.
TEST(TestProperty, TestStringOperators) {
  std::string value("Hello");
  nux::Property<std::string> str_prop(value);

  EXPECT_TRUE(str_prop == "Hello");
  EXPECT_TRUE("Hello" == str_prop);
  EXPECT_FALSE(str_prop != "Hello");
  EXPECT_FALSE("Hello" != str_prop);
  EXPECT_TRUE(str_prop == value);
  EXPECT_TRUE(value == str_prop);
  EXPECT_FALSE(str_prop != value);
  EXPECT_FALSE(value != str_prop);

  EXPECT_FALSE(str_prop == "World");
  EXPECT_FALSE("World" == str_prop);
  EXPECT_TRUE(str_prop != "World");
  EXPECT_TRUE("World" != str_prop);

  EXPECT_FALSE(str_prop < "Aardvark");
  EXPECT_FALSE(str_prop <= "Aardvark");
  EXPECT_TRUE(str_prop > "Aardvark");
  EXPECT_TRUE(str_prop >= "Aardvark");

  EXPECT_TRUE("Aardvark" < str_prop);
  EXPECT_TRUE("Aardvark" <= str_prop);
  EXPECT_FALSE("Aardvark" > str_prop);
  EXPECT_FALSE("Aardvark" >= str_prop);

  nux::Property<std::string> str_prop2(value);
  EXPECT_TRUE(str_prop2 == str_prop);
  EXPECT_FALSE(str_prop2 != str_prop);

  str_prop2 = "Aardvark";

  EXPECT_FALSE(str_prop2 == str_prop);
  EXPECT_TRUE(str_prop2 != str_prop);

  EXPECT_FALSE(str_prop < str_prop2);
  EXPECT_FALSE(str_prop <= str_prop2);
  EXPECT_TRUE(str_prop > str_prop2);
  EXPECT_TRUE(str_prop >= str_prop2);
}

TEST(TestROProperty, TestDefaultConstructor) {
  nux::ROProperty<int> int_prop;
  int value = int_prop;
  EXPECT_THAT(value, Eq(0));
  EXPECT_THAT(int_prop(), Eq(0));
  EXPECT_THAT(int_prop.Get(), Eq(0));

  nux::ROProperty<std::string> string_prop;
  std::string svalue = string_prop;
  EXPECT_THAT(svalue, Eq(""));
  EXPECT_THAT(string_prop(), Eq(""));
  EXPECT_THAT(string_prop.Get(), Eq(""));
}

int simple_int_result()
{
  return 42;
}

TEST(TestROProperty, TestGetterConstructor) {
  nux::ROProperty<int> int_prop(sigc::ptr_fun(&simple_int_result));
  int value = int_prop;
  EXPECT_THAT(value, Eq(42));
  EXPECT_THAT(int_prop(), Eq(42));
  EXPECT_THAT(int_prop.Get(), Eq(42));
}

class Incrementer
{
public:
  Incrementer() : value_(0) {}
  int value() { return ++value_; }
private:
  int value_;
};

TEST(TestROProperty, TestSetGetter) {
  nux::ROProperty<int> int_prop;
  Incrementer incrementer;
  int_prop.SetGetterFunction(sigc::mem_fun(&incrementer, &Incrementer::value));

  int value = int_prop;
  EXPECT_THAT(value, Eq(1));
  EXPECT_THAT(int_prop(), Eq(2));
  EXPECT_THAT(int_prop.Get(), Eq(3));
}

#if __cplusplus >= 201100L || defined (__GXX_EXPERIMENTAL_CXX0X__)
TEST(TestROProperty, TestSetGetterLambda) {
  nux::ROProperty<int> int_prop;
  int_prop.SetGetterFunction([] { static int value = 0; return ++value; });

  int value = int_prop;
  EXPECT_THAT(value, Eq(1));
  EXPECT_THAT(int_prop(), Eq(2));
  EXPECT_THAT(int_prop.Get(), Eq(3));
}
#endif

TEST(TestROProperty, TestChangedEvent) {
  // RO Properties have a changed event, but it is up to the continer of the
  // property to emit the events as nothing is done automatically.
  nux::ROProperty<int> int_prop;

  nt::ChangeRecorder<int> recorder;
  int_prop.changed.connect(recorder.listener());

  int_prop.EmitChanged(42);
  EXPECT_THAT(1, Eq(recorder.size()));
  EXPECT_THAT(42, Eq(recorder.last()));
}

// A simple class that just has a reader functon.
template <typename VALUE_TYPE>
class ROPropHolder
{
public:
  ROPropHolder(VALUE_TYPE const& initial)
    : prop(sigc::mem_fun(this, &ROPropHolder<VALUE_TYPE>::get_prop))
    , prop_(initial)
    {}
  nux::ROProperty<VALUE_TYPE> prop;

  VALUE_TYPE get_prop() const { return prop_; }
  VALUE_TYPE prop_;

};

TEST(TestROProperty, TestIntOperators) {
  ROPropHolder<int> int_prop(42);

  EXPECT_TRUE(int_prop.prop == 42);
  EXPECT_TRUE(42 == int_prop.prop);
  EXPECT_FALSE(int_prop.prop != 42);
  EXPECT_FALSE(42 != int_prop.prop);

  EXPECT_FALSE(int_prop.prop == 5);
  EXPECT_FALSE(5 == int_prop.prop);
  EXPECT_TRUE(int_prop.prop != 5);
  EXPECT_TRUE(5 != int_prop.prop);

  EXPECT_FALSE(int_prop.prop < 5);
  EXPECT_FALSE(int_prop.prop <= 5);
  EXPECT_TRUE(int_prop.prop > 5);
  EXPECT_TRUE(int_prop.prop >= 5);

  EXPECT_TRUE(5 < int_prop.prop);
  EXPECT_TRUE(5 <= int_prop.prop);
  EXPECT_FALSE(5 > int_prop.prop);
  EXPECT_FALSE(5 >= int_prop.prop);

  ROPropHolder<int> int_prop2(42);
  EXPECT_TRUE(int_prop2.prop == int_prop.prop);
  EXPECT_FALSE(int_prop2.prop != int_prop.prop);

  int_prop2.prop_ = 5;

  EXPECT_FALSE(int_prop2.prop == int_prop.prop);
  EXPECT_TRUE(int_prop2.prop != int_prop.prop);

  EXPECT_FALSE(int_prop.prop < int_prop2.prop);
  EXPECT_FALSE(int_prop.prop <= int_prop2.prop);
  EXPECT_TRUE(int_prop.prop > int_prop2.prop);
  EXPECT_TRUE(int_prop.prop >= int_prop2.prop);
}

// Only testing strings and ints, to show that the template classes work with
// both primitive types and classes.
TEST(TestROProperty, TestStringOperators) {
  std::string value("Hello");
  ROPropHolder<std::string> str_prop(value);

  EXPECT_TRUE(str_prop.prop == "Hello");
  EXPECT_TRUE("Hello" == str_prop.prop);
  EXPECT_FALSE(str_prop.prop != "Hello");
  EXPECT_FALSE("Hello" != str_prop.prop);
  EXPECT_TRUE(str_prop.prop == value);
  EXPECT_TRUE(value == str_prop.prop);
  EXPECT_FALSE(str_prop.prop != value);
  EXPECT_FALSE(value != str_prop.prop);

  EXPECT_FALSE(str_prop.prop == "World");
  EXPECT_FALSE("World" == str_prop.prop);
  EXPECT_TRUE(str_prop.prop != "World");
  EXPECT_TRUE("World" != str_prop.prop);

  EXPECT_FALSE(str_prop.prop < "Aardvark");
  EXPECT_FALSE(str_prop.prop <= "Aardvark");
  EXPECT_TRUE(str_prop.prop > "Aardvark");
  EXPECT_TRUE(str_prop.prop >= "Aardvark");

  EXPECT_TRUE("Aardvark" < str_prop.prop);
  EXPECT_TRUE("Aardvark" <= str_prop.prop);
  EXPECT_FALSE("Aardvark" > str_prop.prop);
  EXPECT_FALSE("Aardvark" >= str_prop.prop);

  ROPropHolder<std::string> str_prop2(value);
  EXPECT_TRUE(str_prop2.prop == str_prop.prop);
  EXPECT_FALSE(str_prop2.prop != str_prop.prop);

  str_prop2.prop_ = "Aardvark";

  EXPECT_FALSE(str_prop2.prop == str_prop.prop);
  EXPECT_TRUE(str_prop2.prop != str_prop.prop);

  EXPECT_FALSE(str_prop.prop < str_prop2.prop);
  EXPECT_FALSE(str_prop.prop <= str_prop2.prop);
  EXPECT_TRUE(str_prop.prop > str_prop2.prop);
  EXPECT_TRUE(str_prop.prop >= str_prop2.prop);
}


TEST(TestRWProperty, TestDefaultConstructor) {
  nux::RWProperty<int> int_prop;
  nt::ChangeRecorder<int> recorder;
  int_prop.changed.connect(recorder.listener());

  int_prop = 42;
  int value = int_prop;
  EXPECT_THAT(value, Eq(0));
  EXPECT_THAT(int_prop(), Eq(0));
  EXPECT_THAT(int_prop.Get(), Eq(0));
  EXPECT_THAT(recorder.size(), Eq(0));
}

bool is_even(int const& value)
{
  return value % 2 == 0;
}


TEST(TestRWProperty, TestFunctionConstructor) {
  // This is a somewhat convoluted example.  The setter emits if the value is
  // even, but the value being emitted is controlled by the incrementer.
  Incrementer incrementer;
  nux::RWProperty<int> int_prop(sigc::mem_fun(&incrementer, &Incrementer::value),
                                sigc::ptr_fun(&is_even));
  nt::ChangeRecorder<int> recorder;
  int_prop.changed.connect(recorder.listener());

  int_prop = 42;
  EXPECT_THAT(recorder.size(), Eq(1));
  EXPECT_THAT(recorder.last(), Eq(1));

  // Catch the return value of the assignment.  The getter is called.
  int assign_result = int_prop = 13;
  EXPECT_THAT(recorder.size(), Eq(1));
  EXPECT_THAT(assign_result, Eq(2));

  // each access increments the value.
  int value = int_prop;
  EXPECT_THAT(value, Eq(3));
  EXPECT_THAT(int_prop(), Eq(4));
  EXPECT_THAT(int_prop.Get(), Eq(5));
}

// This bit would normally be in the header file.
class HiddenImpl
{
public:
  HiddenImpl();

  nux::RWProperty<std::string> name;
private:
  class Impl;
  boost::scoped_ptr<Impl> pimpl;
};

// This bit is in the implementation file.
class HiddenImpl::Impl
{
public:
  bool set_name(std::string const& name) {
    bool changed = false;
    std::string new_name("Impl::" + name);
    if (name_ != new_name) {
      name_ = new_name;
      changed = true;
    }
    return changed;
  }
  std::string get_name() const {
    return name_;
  }

private:
  std::string name_;
};

HiddenImpl::HiddenImpl()
  : pimpl(new Impl())
{
  name.SetSetterFunction(sigc::mem_fun(pimpl.get(), &HiddenImpl::Impl::set_name));
  name.SetGetterFunction(sigc::mem_fun(pimpl.get(), &HiddenImpl::Impl::get_name));
}


TEST(TestRWProperty, TestPimplClassExample) {
  HiddenImpl hidden;
  nt::ChangeRecorder<std::string> recorder;
  hidden.name.changed.connect(recorder.listener());

  hidden.name = "NewName";
  EXPECT_THAT(recorder.size(), Eq(1));
  EXPECT_THAT(recorder.last(), Eq("Impl::NewName"));

  // Since the name is updated before comparison, no event emitted.
  hidden.name = "NewName";
  EXPECT_THAT(recorder.size(), Eq(1));

  std::string value = hidden.name;
  EXPECT_THAT(value, Eq("Impl::NewName"));
  EXPECT_THAT(hidden.name(), Eq("Impl::NewName"));
  EXPECT_THAT(hidden.name.Get(), Eq("Impl::NewName"));
}

// A simple class that just has a reader and writer functon.
template <typename VALUE_TYPE>
class RWPropHolder
{
public:
  RWPropHolder(VALUE_TYPE const& initial)
    : prop(sigc::mem_fun(this, &RWPropHolder<VALUE_TYPE>::get_prop),
           sigc::mem_fun(this, &RWPropHolder<VALUE_TYPE>::set_prop))
    , prop_(initial)
    {}
  nux::RWProperty<VALUE_TYPE> prop;

private:
  VALUE_TYPE get_prop() const { return prop_; }
  bool set_prop(VALUE_TYPE const& prop) { prop_ = prop; return true; }

  VALUE_TYPE prop_;

};

TEST(TestRWProperty, TestIntOperators) {
  RWPropHolder<int> int_prop(42);

  EXPECT_TRUE(int_prop.prop == 42);
  EXPECT_TRUE(42 == int_prop.prop);
  EXPECT_FALSE(int_prop.prop != 42);
  EXPECT_FALSE(42 != int_prop.prop);

  EXPECT_FALSE(int_prop.prop == 5);
  EXPECT_FALSE(5 == int_prop.prop);
  EXPECT_TRUE(int_prop.prop != 5);
  EXPECT_TRUE(5 != int_prop.prop);

  EXPECT_FALSE(int_prop.prop < 5);
  EXPECT_FALSE(int_prop.prop <= 5);
  EXPECT_TRUE(int_prop.prop > 5);
  EXPECT_TRUE(int_prop.prop >= 5);

  EXPECT_TRUE(5 < int_prop.prop);
  EXPECT_TRUE(5 <= int_prop.prop);
  EXPECT_FALSE(5 > int_prop.prop);
  EXPECT_FALSE(5 >= int_prop.prop);

  RWPropHolder<int> int_prop2(42);
  EXPECT_TRUE(int_prop2.prop == int_prop.prop);
  EXPECT_FALSE(int_prop2.prop != int_prop.prop);

  int_prop2.prop = 5;

  EXPECT_FALSE(int_prop2.prop == int_prop.prop);
  EXPECT_TRUE(int_prop2.prop != int_prop.prop);

  EXPECT_FALSE(int_prop.prop < int_prop2.prop);
  EXPECT_FALSE(int_prop.prop <= int_prop2.prop);
  EXPECT_TRUE(int_prop.prop > int_prop2.prop);
  EXPECT_TRUE(int_prop.prop >= int_prop2.prop);
}

// Only testing strings and ints, to show that the template classes work with
// both primitive types and classes.
TEST(TestRWProperty, TestStringOperators) {
  std::string value("Hello");
  RWPropHolder<std::string> str_prop(value);

  EXPECT_TRUE(str_prop.prop == "Hello");
  EXPECT_TRUE("Hello" == str_prop.prop);
  EXPECT_FALSE(str_prop.prop != "Hello");
  EXPECT_FALSE("Hello" != str_prop.prop);
  EXPECT_TRUE(str_prop.prop == value);
  EXPECT_TRUE(value == str_prop.prop);
  EXPECT_FALSE(str_prop.prop != value);
  EXPECT_FALSE(value != str_prop.prop);

  EXPECT_FALSE(str_prop.prop == "World");
  EXPECT_FALSE("World" == str_prop.prop);
  EXPECT_TRUE(str_prop.prop != "World");
  EXPECT_TRUE("World" != str_prop.prop);

  EXPECT_FALSE(str_prop.prop < "Aardvark");
  EXPECT_FALSE(str_prop.prop <= "Aardvark");
  EXPECT_TRUE(str_prop.prop > "Aardvark");
  EXPECT_TRUE(str_prop.prop >= "Aardvark");

  EXPECT_TRUE("Aardvark" < str_prop.prop);
  EXPECT_TRUE("Aardvark" <= str_prop.prop);
  EXPECT_FALSE("Aardvark" > str_prop.prop);
  EXPECT_FALSE("Aardvark" >= str_prop.prop);

  RWPropHolder<std::string> str_prop2(value);
  EXPECT_TRUE(str_prop2.prop == str_prop.prop);
  EXPECT_FALSE(str_prop2.prop != str_prop.prop);

  str_prop2.prop = "Aardvark";

  EXPECT_FALSE(str_prop2.prop == str_prop.prop);
  EXPECT_TRUE(str_prop2.prop != str_prop.prop);

  EXPECT_FALSE(str_prop.prop < str_prop2.prop);
  EXPECT_FALSE(str_prop.prop <= str_prop2.prop);
  EXPECT_TRUE(str_prop.prop > str_prop2.prop);
  EXPECT_TRUE(str_prop.prop >= str_prop2.prop);
}

struct TestProperties : nux::Introspectable
{
  TestProperties()
    : name(this, "name")
    , index(this, "index")
    {}

  nux::SerializableProperty<std::string> name;
  nux::SerializableProperty<int> index;
};

TEST(TestIntrospectableProperty, TestSimplePropertyAccess) {
  TestProperties props;
  nt::ChangeRecorder<std::string> recorder;
  props.name.changed.connect(recorder.listener());
  EXPECT_EQ("", props.name());
  EXPECT_EQ(0, props.index());
  props.name = "Testing";
  props.index = 5;
  EXPECT_EQ("Testing", props.name());
  props.name("New Value");
  EXPECT_EQ("New Value", props.name());
  props.name.Set("Another");
  EXPECT_EQ("Another", props.name());

  EXPECT_EQ(3, recorder.size());
  EXPECT_EQ("Testing", recorder.changed_values[0]);
  EXPECT_EQ("New Value", recorder.changed_values[1]);
  EXPECT_EQ("Another", recorder.changed_values[2]);
}

TEST(TestIntrospectableProperty, TestPropertyAccessByName) {
  TestProperties props;
  nt::ChangeRecorder<std::string> name_recorder;
  nt::ChangeRecorder<int> index_recorder;
  props.name.changed.connect(name_recorder.listener());
  props.index.changed.connect(index_recorder.listener());

  props.name = "Testing";
  props.index = 5;
  EXPECT_EQ("Testing", props.GetProperty<std::string>("name"));
  EXPECT_EQ("5", props.GetProperty<std::string>("index"));
  EXPECT_EQ(5, props.GetProperty<int>("index"));

  bool assigned = props.SetProperty("name", "New value");
  EXPECT_TRUE(assigned);
  EXPECT_EQ("New value", props.name());
  EXPECT_EQ("New value", props.GetProperty<std::string>("name"));
  // A little dangreous, but legal.
  EXPECT_EQ(0, props.GetProperty<int>("name"));

  assigned = props.SetProperty("name", 42);
  EXPECT_TRUE(assigned);
  EXPECT_EQ("42", props.name());
  EXPECT_EQ("42", props.GetProperty<std::string>("name"));
  // A little dangreous, but legal.
  EXPECT_EQ(42, props.GetProperty<int>("name"));

  assigned = props.SetProperty("index", 42);
  EXPECT_TRUE(assigned);
  EXPECT_EQ(42, props.index());
  EXPECT_EQ("42", props.GetProperty<std::string>("index"));
  EXPECT_EQ(42, props.GetProperty<int>("index"));

  assigned = props.SetProperty("index", "hello");
  EXPECT_FALSE(assigned);
  EXPECT_EQ(42, props.index());
  EXPECT_EQ("42", props.GetProperty<std::string>("index"));
  EXPECT_EQ(42, props.GetProperty<int>("index"));

  // Gettin a non-existant property returns a default constructed instance.
  std::string surname = props.GetProperty<std::string>("surname");
  EXPECT_EQ("", surname);
  int foo = props.GetProperty<int>("foo");
  EXPECT_EQ(0, foo);

  assigned = props.SetProperty("non-existant", "hello");
  EXPECT_FALSE(assigned);
}


}
