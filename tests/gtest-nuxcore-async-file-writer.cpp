#include <string>
#include <fstream>

#include <iostream>

#include <gmock/gmock.h>

#include <boost/filesystem.hpp>

#include <glib.h>

#include "NuxCore/AsyncFileWriter.h"

#include "Helpers.h"

namespace bf = boost::filesystem;
using namespace testing;
using namespace nux::testing;

namespace {

const std::string TEST_ROOT("/tmp/nux-test-cases");


class TestAsyncfileWriter : public ::testing::Test
{
protected:
  virtual void SetUp() {
    // Make sure that the tests start with and empty TEST_ROOT.
    bf::remove_all(TEST_ROOT);
    bf::create_directories(TEST_ROOT);
  }

  virtual void TearDown() {
    // Delete the unity test directory
    bf::remove_all(TEST_ROOT);
  }

  bool WaitForOpen(nux::AsyncFileWriter& writer, unsigned timeout = 5) {
    TestCallback opened;
    TestCallback timed_out;
    g_timeout_add_seconds(timeout, &TestCallback::glib_callback, &timed_out);
    writer.opened.connect(opened.sigc_callback());

    while (!opened.happened && !timed_out.happened) {
      PumpGObjectMainLoop();
    }
    return opened.happened;
  }

  bool WaitForClose(nux::AsyncFileWriter& writer, unsigned timeout = 5) {
    TestCallback closed;
    TestCallback timed_out;
    g_timeout_add_seconds(timeout, &TestCallback::glib_callback, &timed_out);
    writer.closed.connect(closed.sigc_callback());

    while (!closed.happened && !timed_out.happened) {
      PumpGObjectMainLoop();
    }
    return closed.happened;
  }

};

TEST_F(TestAsyncfileWriter, TestConstructor) {
  std::string filename(TEST_ROOT + "/empty-file");
  {
    nux::AsyncFileWriter writer(filename);
    bool opened = WaitForOpen(writer);
    EXPECT_TRUE(opened);
  }
  EXPECT_TRUE(bf::exists(filename));
  EXPECT_THAT(ReadFile(filename), Eq(""));
}

TEST_F(TestAsyncfileWriter, TestWrites) {
  std::string filename(TEST_ROOT + "/write-file");
  std::string data(200, 'x');
  {
    nux::AsyncFileWriter writer(filename);
    writer.Write(data);
    writer.Close();
    bool closed = WaitForClose(writer);
    EXPECT_TRUE(closed);
  }
  EXPECT_THAT(ReadFile(filename), Eq(data));
}

TEST_F(TestAsyncfileWriter, TestWriteLots) {
  std::string filename(TEST_ROOT + "/lots-file");
  std::string data(200, 'x');
  const int loop_count = 1000;
  {
    nux::AsyncFileWriter writer(filename);
    for (int i = 0; i < loop_count; ++i) {
      writer.Write(data);
    }
    writer.Close();
    bool closed = WaitForClose(writer);
    EXPECT_TRUE(closed);
  }
  std::string file_content = ReadFile(filename);
  EXPECT_THAT(file_content.size(), Eq(data.size() * loop_count));
  // They are all x's.
  EXPECT_THAT(file_content, MatchesRegex("^x+$"));
}



} // anon namespace
