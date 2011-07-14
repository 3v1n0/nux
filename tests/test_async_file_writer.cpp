#include <string>
#include <fstream>

#include <iostream>

#include <gmock/gmock.h>

#include <glib.h>

#include <boost/filesystem.hpp>

#include "NuxCore/AsyncFileWriter.h"

namespace bf = boost::filesystem;
using namespace testing;

namespace {

const std::string TEST_ROOT("/tmp/nux-test-cases");

class TestCallback
{
public:
  TestCallback() : happened(false) {}

  sigc::slot<void> sigc_callback() {
    return sigc::mem_fun(this, &TestCallback::callback);
  }

  static gboolean glib_callback(gpointer data) {
    TestCallback* test = reinterpret_cast<TestCallback*>(data);
    test->callback();
    return FALSE;
  }
  void callback() {
    happened = true;
  }

  bool happened;
};

gboolean gsource_bool_setter(gpointer data)
{
  bool* value = reinterpret_cast<bool*>(data);
  *value = true;
  return FALSE;
}

void sigc_bool_setter(bool& value)
{
  std::cerr << "sigc_bool_setter called\n";
  value = true;
}


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

  std::string ReadFile(std::string const& filename) {
    std::ifstream input(filename.c_str());
    if (input.bad())
      throw std::runtime_error("bad file");
    return std::string((std::istreambuf_iterator<char>(input)),
                       std::istreambuf_iterator<char>());
  }

  void PumpGObjectMainLoop() {
    GMainContext* context(g_main_context_get_thread_default());
    while (g_main_context_pending(context)) {
      g_main_context_iteration(context, false);
    }
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
  const int loop_count = 100;
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
