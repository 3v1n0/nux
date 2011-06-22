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
  RollingFileAppender output(logfile);

  EXPECT_TRUE(bf::exists(logfile));
  EXPECT_TRUE(bf::is_regular_file(logfile));
}

TEST_F(TestRollingFileAppender, TestNestedDirectories) {

  std::string logfile = TEST_ROOT + "/nested/directories/nux.log";
  RollingFileAppender output(logfile);

  EXPECT_TRUE(bf::exists(logfile));
  EXPECT_TRUE(bf::is_regular_file(logfile));
}

TEST_F(TestRollingFileAppender, TestWritesToLogFile) {

  std::string logfile = TEST_ROOT + "/nux.log";
  RollingFileAppender output(logfile);

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

TEST_F(TestRollingFileAppender, TestLogFileRollsAtFlush) {

  std::string logfile = TEST_ROOT + "/nux.log";
  unsigned max_log_size = 20; // roll every 20 characters
  RollingFileAppender output(logfile, 5, max_log_size);

  output << "Testing the rolling of the logfile" << std::endl
         << "Long line greater than max_log_size" << std::endl;

  // Since the log files are rolled on flush, if the last thing written out
  // takes the filesize greater than the max_log_size, the log files are
  // rolled and the current file being appended to is now empty.
  EXPECT_THAT(ReadFile(logfile),
              Eq(""));
  EXPECT_THAT(ReadFile(logfile + ".1"),
              Eq("Long line greater than max_log_size\n"));
  EXPECT_THAT(ReadFile(logfile + ".2"),
              Eq("Testing the rolling of the logfile\n"));
}

TEST_F(TestRollingFileAppender, TestExistingLogFileMoved) {

  std::string logfile = TEST_ROOT + "/nux.log";
  {
    // Due to operator<< weirdness, we can't put this on one line without a
    // static_cast<ostream&>, which is slightly unintuitive.
    RollingFileAppender output(logfile);
    output << "Existing file.";
  }
  EXPECT_TRUE(bf::exists(logfile));

  RollingFileAppender output(logfile);

  EXPECT_THAT(ReadFile(logfile),
              Eq(""));
  EXPECT_THAT(ReadFile(logfile + ".1"),
              Eq("Existing file."));
}

TEST_F(TestRollingFileAppender, TestDeletingOld) {

  std::string logfile = TEST_ROOT + "/nux.log";
  // Two backups, size 20 bytes.
  RollingFileAppender output(logfile, 2, 20);

  output << "Oldest line should be deleted." << std::endl
         << "This line will be in the last backup." << std::endl
         << "This is backup number 1." << std::endl
         << "Current." << std::endl;

  EXPECT_THAT(ReadFile(logfile),
              Eq("Current.\n"));
  EXPECT_THAT(ReadFile(logfile + ".1"),
              Eq("This is backup number 1.\n"));
  EXPECT_THAT(ReadFile(logfile + ".2"),
              Eq("This line will be in the last backup.\n"));
  EXPECT_FALSE(bf::exists(logfile + ".3"));
}

TEST_F(TestRollingFileAppender, TestFullPathNeeded) {
  std::string logfile = "nux.log";
  // For some obscure reason, EXPECT_THROW won't accept:
  //  RollingFileAppender(logfile)
  // as its first arg.
  EXPECT_THROW(RollingFileAppender appender(logfile), std::runtime_error);
}


} // anon namespace
