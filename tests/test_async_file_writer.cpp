#include <string>
#include <fstream>

#include <gmock/gmock.h>

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
};

TEST_F(TestAsyncfileWriter, TestConstructor) {
  std::string filename(TEST_ROOT + "/empty-file");
  {
    nux::AsyncFileWriter writer(filename);
  }
  EXPECT_TRUE(bf::exists(filename));
  EXPECT_THAT(ReadFile(filename), Eq(""));
}



} // anon namespace
