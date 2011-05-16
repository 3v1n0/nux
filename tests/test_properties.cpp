#include "NuxCore/Property.h"

#include <sigc++/trackable.h>

#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

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


TEST(TestTypeTraits, TestSerialization) {
  EXPECT_EQ("hello", nux::type::PropertyTrait<std::string>::to_string("hello"));
  EXPECT_EQ("42", nux::type::PropertyTrait<int>::to_string(42));
  EXPECT_EQ("true", nux::type::PropertyTrait<bool>::to_string(true));
  EXPECT_EQ("false", nux::type::PropertyTrait<bool>::to_string(false));
  EXPECT_EQ("0", nux::type::PropertyTrait<double>::to_string(0));
  EXPECT_EQ("25.5", nux::type::PropertyTrait<double>::to_string(25.5));
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
  string_prop.disable_notifications();
  string_prop = "Hello world" ;
  EXPECT_EQ(0, recorder.changed_values.size());
  string_prop.enable_notifications();
  // No notification if not changed.
  string_prop = "Hello world" ;
  EXPECT_EQ(0, recorder.changed_values.size());

  string_prop = "New value" ;
  EXPECT_EQ(1, recorder.changed_values.size());
  EXPECT_EQ("New value", recorder.changed_values[0]);
}

struct TestProperties : nux::Introspectable
{
  TestProperties()
    : name(this, "name")
    , index(this, "index")
    {}

  nux::Property<std::string> name;
  nux::Property<int> index;
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

  assigned = props.set_property("non-existant", "hello");
  EXPECT_FALSE(assigned);

}


}
