#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>
#include <streambuf>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include "NuxCore/RollingFileAppender.h"
#include "Helpers.h"

namespace bf = boost::filesystem;

using namespace nux::logging;
using namespace nux::testing;
using namespace testing;

namespace {

/**
 * Due to the asynchronous manner in which the rolling file appender writes
 * the data to disk, it is incredibly hard to test in a simple unit test as
 * the underlying file on the file system isn't created synchronously, nor is
 * the output written synchronously.
 *
 * A files_rolled event exists on the RollingFileAppender so we can at least
 * wait for that signal and check the underlying files on the file system at
 * that stage.  This does mean that we aren't testing the smaller chunks of
 * how it works, but more only the complete system, which I guess has to be
 * good enough for this case.
 */

const std::string TEST_ROOT("/tmp/nux-test-cases");

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

  bool WaitForRoll(RollingFileAppender& appender, unsigned timeout = 5) {
    TestCallback rolled;
    TestCallback timed_out;
    g_timeout_add_seconds(timeout, &TestCallback::glib_callback, &timed_out);
    appender.files_rolled.connect(rolled.sigc_callback());

    while (!rolled.happened && !timed_out.happened) {
      PumpGObjectMainLoop();
    }
    return rolled.happened;
  }

};

TEST_F(TestRollingFileAppender, NoTestRoot) {
  // The test root should not exist.
  EXPECT_FALSE(bf::exists(TEST_ROOT));
}

TEST_F(TestRollingFileAppender, TestLogFileRollsAtFlush) {

  std::string logfile = TEST_ROOT + "/nux.log";
  unsigned max_log_size = 20; // roll every 20 characters
  RollingFileAppender output(logfile, 5, max_log_size);

  output << "Testing the rolling of the logfile" << std::endl;
  WaitForRoll(output);
  output << "Long line greater than max_log_size" << std::endl;
  WaitForRoll(output);

  // Since the log files are rolled on flush, if the last thing written out
  // takes the filesize greater than the max_log_size, the log files are
  // rolled and the current file being appended to is now empty.
  EXPECT_THAT(ReadFile(logfile + ".1"),
              Eq("Long line greater than max_log_size\n"));
  EXPECT_THAT(ReadFile(logfile + ".2"),
              Eq("Testing the rolling of the logfile\n"));
}

TEST_F(TestRollingFileAppender, TestExistingLogFileMoved) {

  std::string logfile = TEST_ROOT + "/nux.log";
  {
    bf::create_directories(bf::path(logfile).parent_path());
    std::ofstream output(logfile);
    output << "Existing file.";
  }
  EXPECT_TRUE(bf::exists(logfile));

  RollingFileAppender output(logfile);

  EXPECT_THAT(ReadFile(logfile + ".1"),
              Eq("Existing file."));
}

TEST_F(TestRollingFileAppender, TestDeletingOld) {

  std::string logfile = TEST_ROOT + "/nux.log";
  // Two backups, size 20 bytes.
  RollingFileAppender output(logfile, 2, 20);

  // Due to the asynchronous manner in which the output is sent to the
  // underlying file, we explicitly wait for the roll here.  Otherwise we may
  // just send all the logging lines to one file then it would roll.
  output << "Oldest line should be deleted." << std::endl;
  WaitForRoll(output);
  output << "This line will be in the last backup." << std::endl;
  WaitForRoll(output);
  output << "This is backup number 1." << std::endl;
  WaitForRoll(output);

  EXPECT_THAT(ReadFile(logfile + ".1"),
              Eq("This is backup number 1.\n"));
  EXPECT_THAT(ReadFile(logfile + ".2"),
              Eq("This line will be in the last backup.\n"));
  EXPECT_FALSE(bf::exists(logfile + ".3"));
}

TEST_F(TestRollingFileAppender, TestFullPathNeeded) {
  EXPECT_THROW(RollingFileAppender("nux.log"), std::runtime_error);
  EXPECT_THROW(RollingFileAppender("relative/nux.log"), std::runtime_error);
}

TEST_F(TestRollingFileAppender, TestFileNeeded) {
  // For some obscure reason, EXPECT_THROW won't accept:
  //  RollingFileAppender(logfile)
  // as its first arg.
  std::string directory_path = TEST_ROOT + "/somedir";
  bf::create_directories(directory_path);
  EXPECT_THROW(RollingFileAppender appender(directory_path), std::runtime_error);

  std::string symlink_path = TEST_ROOT + "/somelink";
  bf::create_symlink(directory_path, symlink_path);
  EXPECT_THROW(RollingFileAppender appender(symlink_path), std::runtime_error);
}


} // anon namespace
