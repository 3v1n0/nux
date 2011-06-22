#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>
#include <streambuf>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "NuxCore/RollingFileAppender.h"

namespace bf = boost::filesystem;

using namespace nux::logging;
using namespace testing;

namespace {

const std::string TEST_ROOT("/tmp/nux-test-cases");

TEST(TestRootDir, NoTestRoot) {
  // The test root should not exist.
  EXPECT_FALSE(bf::exists(TEST_ROOT));
}

class TestRollingFileAppender : public ::testing::Test
{
protected:
  virtual void SetUp() {
    // Make sure that the tests start with nothing there.
    bf::remove_all(TEST_ROOT);
  }

  virtual void TearDown() {
    // Delete the unity test directory
    bf::remove_all(TEST_ROOT);
  }

  std::string ReadFile(std::string const& filename) {
    std::ifstream input(filename.c_str());
    return std::string((std::istreambuf_iterator<char>(input)),
                       std::istreambuf_iterator<char>());
  }
};

TEST_F(TestRollingFileAppender, TestCreatesFile) {

  std::string logfile = TEST_ROOT + "/nux.log";
  RollingFileAppender output(logfile, 5, 1e6);

  EXPECT_TRUE(bf::exists(logfile));
  EXPECT_TRUE(bf::is_regular_file(logfile));
}

TEST_F(TestRollingFileAppender, TestNestedDirectories) {

  std::string logfile = TEST_ROOT + "/nested/directories/nux.log";
  RollingFileAppender output(logfile, 5, 1e6);

  EXPECT_TRUE(bf::exists(logfile));
  EXPECT_TRUE(bf::is_regular_file(logfile));
}

TEST_F(TestRollingFileAppender, TestWritesToLogFile) {

  std::string logfile = TEST_ROOT + "/nux.log";
  RollingFileAppender output(logfile, 5, 1e6);

  output << "Testing writing" << std::flush;

  EXPECT_THAT(ReadFile(logfile), Eq("Testing writing"));
}

TEST_F(TestRollingFileAppender, TestLogFileRolls) {

  std::string logfile = TEST_ROOT + "/nux.log";
  unsigned max_log_size = 20; // roll every 20 characters
  RollingFileAppender output(logfile, 5, max_log_size);

  output << "Testing the rolling of the logfile" << std::endl
         << "Next line" << std::endl;

  EXPECT_THAT(ReadFile(logfile),
              Eq("Next line\n"));
  EXPECT_THAT(ReadFile(logfile + ".1"),
              Eq("Testing the rolling of the logfile\n"));
}


} // anon namespace
