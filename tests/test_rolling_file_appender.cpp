#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

// using namespace nux::logging;
using namespace testing;

namespace {

class TestRollingFileAppender : public ::testing::Test
{
public:
  TestRollingFileAppender()
    : test_root("/tmp/nux-test-cases")
    {
    }

 protected:
  boost::filesystem::path test_root;

  virtual void SetUp() {
    // Make the unity test directory in /tmp
    boost::filesystem::create_directories(test_root);
  }

 virtual void TearDown() {
   // Delete the unity test directory
   boost::filesystem::remove_all(test_root);
 }
};


TEST_F(TestRollingFileAppender, TestSetup) {

  boost::filesystem::ofstream fout(test_root / "testing");
  fout << "Testing";
  fout.close();

  EXPECT_TRUE(boost::filesystem::exists("/tmp/nux-test-cases/testing"));
}

TEST_F(TestRollingFileAppender, TestCreatesFile) {

  nux::logging::RollingFileAppender output(test_root, "nux");

  EXPECT_TRUE(boost::filesystem::exists(test_root / "nux"));
  EXPECT_TRUE(boost::filesystem::is_regular_file(test_root / "nux"));
}

} // anon namespace
