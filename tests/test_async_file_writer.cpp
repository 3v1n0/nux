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

class TestAsyncfileWriter : public ::testing::Test
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

  void PumpGObjectMainLoop() {
    GMainContext* context(g_main_context_get_thread_default());
    while (g_main_context_pending(context)) {
      std::cerr << "Pump gobject\n";
      g_main_context_iteration(context, false);
    }
  }
};

TEST_F(TestAsyncfileWriter, TestConstructor) {
  // If the async open isn't allowed to happen before the destructor occurs,
  // the file doesn't exist.
  std::string filename(TEST_ROOT + "/empty-file");
  {
    nux::AsyncFileWriter writer(filename);
  }
  EXPECT_FALSE(bf::exists(filename));

  // If the async open does occur before the destructor occurs, the file does
  // exist.
  {
    nux::AsyncFileWriter writer(filename);
    PumpGObjectMainLoop();
  }
  EXPECT_TRUE(bf::exists(filename));
  EXPECT_THAT(ReadFile(filename), Eq(""));
}



} // anon namespace
