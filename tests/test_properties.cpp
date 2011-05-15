#include "NuxCore/Property.h"

#include <sigc++/trackable.h>

#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

namespace {

template <typename T>
T to_and_from_string(T const& value)
{
  std::string temp = nux::type::serializable<T>::to_string(value);
  std::pair<T, bool> result = nux::type::serializable<T>::from_string(temp);
  if (!result.second) {
    throw std::runtime_error("Failed to convert.");
  }
  return result.first;
}


TEST(TestTypeTraits, TestSerialization) {
  EXPECT_EQ("hello", nux::type::serializable<std::string>::to_string("hello"));
  EXPECT_EQ("42", nux::type::serializable<int>::to_string(42));
  EXPECT_EQ("true", nux::type::serializable<bool>::to_string(true));
  EXPECT_EQ("false", nux::type::serializable<bool>::to_string(false));
  EXPECT_EQ("0", nux::type::serializable<double>::to_string(0));
  EXPECT_EQ("25.5", nux::type::serializable<double>::to_string(25.5));
}

TEST(TestTypeTraits, TestDeserialization) {
  std::pair<std::string, bool> sr = nux::type::serializable<std::string>::from_string("hello");
  EXPECT_EQ("hello", sr.first);
  EXPECT_TRUE(sr.second);
  std::pair<int, bool> int_result = nux::type::serializable<int>::from_string("42");
  EXPECT_EQ(42, int_result.first);
  EXPECT_TRUE(int_result.second);
  int_result = nux::type::serializable<int>::from_string("OMG!");
  EXPECT_EQ(0, int_result.first);
  EXPECT_FALSE(int_result.second);
  std::pair<bool, bool> bool_result = nux::type::serializable<bool>::from_string("true");
  EXPECT_TRUE(bool_result.first);
  EXPECT_TRUE(bool_result.second);
  bool_result = nux::type::serializable<bool>::from_string("false");
  EXPECT_FALSE(bool_result.first);
  EXPECT_TRUE(bool_result.second);
  bool_result = nux::type::serializable<bool>::from_string("what?");
  EXPECT_FALSE(bool_result.first);
  EXPECT_FALSE(bool_result.second);
  std::pair<double, bool> double_result = nux::type::serializable<double>::from_string("25.5");
  EXPECT_EQ(25.5, double_result.first);
  EXPECT_TRUE(double_result.second);
  double_result = nux::type::serializable<double>::from_string("2e6");
  EXPECT_EQ(2000000.0, double_result.first);
  EXPECT_TRUE(double_result.second);
  double_result = nux::type::serializable<double>::from_string("what?");
  EXPECT_EQ(0, double_result.first);
  EXPECT_FALSE(double_result.second);
}

TEST(TestTypeTraits, TestConversionHolds) {
  std::string string_value("Hello World!");
  EXPECT_EQ(string_value, to_and_from_string(string_value));
  double double_value = 34.345;
  EXPECT_EQ(double_value, to_and_from_string(double_value));
}

TEST(TestTypeTraits, TestAssign) {
  std::string value;
  // std::string's can be assigned to with strings
  nux::type::assignable<std::string>::assign(value, std::string("foo"));
  EXPECT_EQ("foo", value);
  // or char const*
  nux::type::assignable <std::string>::assign(value, "bar");
  EXPECT_EQ("bar", value);
}


TEST(TestConnectableProperty, TestConstruction) {
  nux::ConnectableProperty<std::string> string_prop;
  EXPECT_EQ("", string_prop.value());
  EXPECT_EQ("", static_cast<std::string>(string_prop));
  nux::ConnectableProperty<std::string> string_prop_val("hello");
  EXPECT_EQ("hello", string_prop_val.value());
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
  string_prop = std::string("Hello world");
  EXPECT_EQ(1, recorder.changed_values.size());
  EXPECT_EQ("Hello world", recorder.changed_values[0]);
  // No notification if not changed.
  string_prop = std::string("Hello world");
  EXPECT_EQ(1, recorder.changed_values.size());
}

TEST(TestConnectableProperty, TestFreezeAndThaw) {
  nux::ConnectableProperty<std::string> string_prop;
  ChangeRecorder<std::string> recorder;
  string_prop.changed.connect(
    sigc::mem_fun(recorder, &ChangeRecorder<std::string>::value_changed));
  string_prop.freeze();
  string_prop = std::string("Hello world");
  EXPECT_EQ(0, recorder.changed_values.size());
  string_prop.thaw();
  // No notification if not changed.
  string_prop = std::string("Hello world");
  EXPECT_EQ(0, recorder.changed_values.size());

  string_prop = std::string("New value");
  EXPECT_EQ(1, recorder.changed_values.size());
  EXPECT_EQ("New value", recorder.changed_values[0]);
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
