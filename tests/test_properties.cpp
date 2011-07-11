#include "NuxCore/Property.h"

#include <sigc++/trackable.h>

#include <gmock/gmock.h>
#include <vector>
#include <stdexcept>

using namespace testing;

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


TEST(TestConnectableProperty, TestConstruction) {
  nux::ConnectableProperty<std::string> string_prop;
  EXPECT_EQ("", string_prop());
  EXPECT_EQ("", string_prop.get());
  EXPECT_EQ("", static_cast<std::string>(string_prop));
  nux::ConnectableProperty<std::string> string_prop_val("hello");
  EXPECT_EQ("hello", string_prop_val());
  EXPECT_EQ("hello", string_prop_val.get());
  EXPECT_EQ("hello", static_cast<std::string>(string_prop_val));
}

template <typename T>
struct ChangeRecorder : sigc::trackable
{
  void value_changed(T const& value)
    {
      changed_values.push_back(value);
    }
  typedef std::vector<T> ChangedValues;
  ChangedValues changed_values;

  int size() const { return changed_values.size(); }
  T last() const { return *changed_values.rbegin(); }
};

TEST(TestConnectableProperty, TestAssignmentNotification) {
  nux::ConnectableProperty<std::string> string_prop;
  ChangeRecorder<std::string> recorder;
  string_prop.changed.connect(
    sigc::mem_fun(recorder, &ChangeRecorder<std::string>::value_changed));
  string_prop = "Hello world" ;
  EXPECT_EQ(1, recorder.changed_values.size());
  EXPECT_EQ("Hello world", recorder.changed_values[0]);
  // No notification if not changed.
  string_prop = std::string("Hello world");
  EXPECT_EQ(1, recorder.changed_values.size());
}

TEST(TestConnectableProperty, TestEnableAndDisableNotification) {
  nux::ConnectableProperty<std::string> string_prop;
  ChangeRecorder<std::string> recorder;
  string_prop.changed.connect(
    sigc::mem_fun(recorder, &ChangeRecorder<std::string>::value_changed));
  string_prop.DisableNotifications();
  string_prop = "Hello world" ;
  EXPECT_EQ(0, recorder.changed_values.size());
  string_prop.EnableNotifications();
  // No notification if not changed.
  string_prop = "Hello world" ;
  EXPECT_EQ(0, recorder.changed_values.size());

  string_prop = "New value" ;
  EXPECT_EQ(1, recorder.changed_values.size());
  EXPECT_EQ("New value", recorder.changed_values[0]);

  nux::ConnectableProperty<TestEnum> enum_prop;
  // This fails to compile.
  // nux::ConnectableProperty<TestClass> class_prop;
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
  ChangeRecorder<std::string> recorder;
  string_prop.changed.connect(
    sigc::mem_fun(recorder, &ChangeRecorder<std::string>::value_changed));

  string_prop = "Hello world" ;
  EXPECT_THAT(1, Eq(recorder.size()));
  EXPECT_THAT("Hello world", Eq(recorder.last()));
  // No notification if not changed.
  string_prop = std::string("Hello world");
  EXPECT_THAT(1, Eq(recorder.size()));
}

TEST(TestProperty, TestEnableAndDisableNotifications) {
  nux::Property<std::string> string_prop;
  ChangeRecorder<std::string> recorder;
  string_prop.changed.connect(
    sigc::mem_fun(recorder, &ChangeRecorder<std::string>::value_changed));

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
  float_prop.CustomSetterFunction(sigc::mem_fun(&clamp, &FloatClamp::Set));
  ChangeRecorder<float> recorder;
  float_prop.changed.connect(
    sigc::mem_fun(recorder, &ChangeRecorder<float>::value_changed));

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
  int_prop.CustomGetterFunction(sigc::mem_fun(&incrementer, &Incrementer::value));

  int value = int_prop;
  EXPECT_THAT(value, Eq(1));
  EXPECT_THAT(int_prop(), Eq(2));
  EXPECT_THAT(int_prop.Get(), Eq(3));
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
  ChangeRecorder<std::string> recorder;
  props.name.changed.connect(
    sigc::mem_fun(recorder, &ChangeRecorder<std::string>::value_changed));
  EXPECT_EQ("", props.name());
  EXPECT_EQ(0, props.index());
  props.name = "Testing";
  props.index = 5;
  EXPECT_EQ("Testing", props.name());
  props.name("New Value");
  EXPECT_EQ("New Value", props.name());
  props.name.set("Another");
  EXPECT_EQ("Another", props.name());

  EXPECT_EQ(3, recorder.changed_values.size());
  EXPECT_EQ("Testing", recorder.changed_values[0]);
  EXPECT_EQ("New Value", recorder.changed_values[1]);
  EXPECT_EQ("Another", recorder.changed_values[2]);
}

TEST(TestIntrospectableProperty, TestPropertyAccessByName) {
  TestProperties props;
  ChangeRecorder<std::string> name_recorder;
  ChangeRecorder<int> index_recorder;
  props.name.changed.connect(
    sigc::mem_fun(name_recorder, &ChangeRecorder<std::string>::value_changed));
  props.index.changed.connect(
    sigc::mem_fun(index_recorder, &ChangeRecorder<int>::value_changed));

  props.name = "Testing";
  props.index = 5;
  EXPECT_EQ("Testing", props.get_property<std::string>("name"));
  EXPECT_EQ("5", props.get_property<std::string>("index"));
  EXPECT_EQ(5, props.get_property<int>("index"));

  bool assigned = props.set_property("name", "New value");
  EXPECT_TRUE(assigned);
  EXPECT_EQ("New value", props.name());
  EXPECT_EQ("New value", props.get_property<std::string>("name"));
  // A little dangreous, but legal.
  EXPECT_EQ(0, props.get_property<int>("name"));

  assigned = props.set_property("name", 42);
  EXPECT_TRUE(assigned);
  EXPECT_EQ("42", props.name());
  EXPECT_EQ("42", props.get_property<std::string>("name"));
  // A little dangreous, but legal.
  EXPECT_EQ(42, props.get_property<int>("name"));

  assigned = props.set_property("index", 42);
  EXPECT_TRUE(assigned);
  EXPECT_EQ(42, props.index());
  EXPECT_EQ("42", props.get_property<std::string>("index"));
  EXPECT_EQ(42, props.get_property<int>("index"));

  assigned = props.set_property("index", "hello");
  EXPECT_FALSE(assigned);
  EXPECT_EQ(42, props.index());
  EXPECT_EQ("42", props.get_property<std::string>("index"));
  EXPECT_EQ(42, props.get_property<int>("index"));

  // Gettin a non-existant property returns a default constructed instance.
  std::string surname = props.get_property<std::string>("surname");
  EXPECT_EQ("", surname);
  int foo = props.get_property<int>("foo");
  EXPECT_EQ(0, foo);

  assigned = props.set_property("non-existant", "hello");
  EXPECT_FALSE(assigned);

}


}
