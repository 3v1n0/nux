#include "NuxCore/Logger.h"
#include "NuxCore/LoggingWriter.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>


using namespace nux::logging;
using namespace testing;

namespace {

TEST(TestLogger, TestSimpleConstruction) {
  Logger logger("");
  EXPECT_EQ(logger.module(), "");
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
}

TEST(TestLogger, TestModuleName) {
  Logger logger("nux.logging");
  EXPECT_EQ(logger.module(), "nux.logging");
}

TEST(TestLogger, TestSetLevel) {
  Logger logger("testing");
  EXPECT_EQ(logger.GetLogLevel(), NOT_SPECIFIED);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), WARNING);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(DEBUG);
  EXPECT_EQ(logger.GetLogLevel(), DEBUG);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), DEBUG);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_TRUE(logger.IsInfoEnabled());
  EXPECT_TRUE(logger.IsDebugEnabled());
  logger.SetLogLevel(INFO);
  EXPECT_EQ(logger.GetLogLevel(), INFO);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), INFO);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_TRUE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(WARNING);
  EXPECT_EQ(logger.GetLogLevel(), WARNING);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), WARNING);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(ERROR);
  EXPECT_EQ(logger.GetLogLevel(), ERROR);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), ERROR);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_FALSE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(NOT_SPECIFIED);
  EXPECT_EQ(logger.GetLogLevel(), NOT_SPECIFIED);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), WARNING);
}

TEST(TestLogger, TestLevelsSharedForSameModule) {
  Logger logger1("testing.module");
  Logger logger2("testing.module");

  logger1.SetLogLevel(INFO);
  EXPECT_TRUE(logger1.IsInfoEnabled());
  EXPECT_TRUE(logger2.IsInfoEnabled());
}

TEST(TestLogger, TestLevelsInherited) {
  Logger root("");
  Logger first("first");
  Logger second("first.second");

  root.SetLogLevel(ERROR);
  EXPECT_EQ(root.GetLogLevel(), ERROR);
  EXPECT_EQ(root.GetEffectiveLogLevel(), ERROR);
  EXPECT_EQ(first.GetLogLevel(), NOT_SPECIFIED);
  EXPECT_EQ(first.GetEffectiveLogLevel(), ERROR);
  EXPECT_EQ(second.GetLogLevel(), NOT_SPECIFIED);
  EXPECT_EQ(second.GetEffectiveLogLevel(), ERROR);

  first.SetLogLevel(DEBUG);
  EXPECT_EQ(root.GetLogLevel(), ERROR);
  EXPECT_EQ(root.GetEffectiveLogLevel(), ERROR);
  EXPECT_EQ(first.GetLogLevel(), DEBUG);
  EXPECT_EQ(first.GetEffectiveLogLevel(), DEBUG);
  EXPECT_EQ(second.GetLogLevel(), NOT_SPECIFIED);
  EXPECT_EQ(second.GetEffectiveLogLevel(), DEBUG);

  second.SetLogLevel(INFO);
  EXPECT_EQ(root.GetLogLevel(), ERROR);
  EXPECT_EQ(root.GetEffectiveLogLevel(), ERROR);
  EXPECT_EQ(first.GetLogLevel(), DEBUG);
  EXPECT_EQ(first.GetEffectiveLogLevel(), DEBUG);
  EXPECT_EQ(second.GetLogLevel(), INFO);
  EXPECT_EQ(second.GetEffectiveLogLevel(), INFO);

  first.SetLogLevel(NOT_SPECIFIED);
  EXPECT_EQ(root.GetLogLevel(), ERROR);
  EXPECT_EQ(root.GetEffectiveLogLevel(), ERROR);
  EXPECT_EQ(first.GetLogLevel(), NOT_SPECIFIED);
  EXPECT_EQ(first.GetEffectiveLogLevel(), ERROR);
  EXPECT_EQ(second.GetLogLevel(), INFO);
  EXPECT_EQ(second.GetEffectiveLogLevel(), INFO);
}


TEST(TestLoggingWriter, TestWriteMessage) {
  std::stringstream out;
  Writer& writer = Writer::Instance();
  writer.SetOutputStream(out);
  std::time_t now = std::time(0);
  writer.WriteMessage(ERROR, "test.module", "testfile.cpp", 1234, now, "my message");
  std::string result = out.str();

  EXPECT_THAT(result, StartsWith("ERROR test.module"));
  EXPECT_THAT(result, HasSubstr("testfile.cpp:1234"));
  EXPECT_THAT(result, EndsWith("my message\n"));
}

TEST(TestLogStream, TestSimpleConstruction) {
  // First test is to make sure a LogStream can be constructed and destructed.
  LogStream test(DEBUG, "module", "filename", 42);
}

TEST(TestLogStream, TestOutput) {
  // First test is to make sure a LogStream can be constructed and destructed.
  std::stringstream out;
  Writer::Instance().SetOutputStream(out);

  LogStream test(DEBUG, "module", "filename", 42);
  test << "testing message" << std::flush;

  std::string result = out.str();

  EXPECT_THAT(result, StartsWith("DEBUG module"));
  EXPECT_THAT(result, HasSubstr("filename:42"));
  EXPECT_THAT(result, EndsWith("testing message\n"));
}


} // anon namespace
