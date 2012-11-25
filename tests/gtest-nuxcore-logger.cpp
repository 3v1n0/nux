#include "NuxCore/Logger.h"
#include "NuxCore/LoggerPrivate.h"
#include "NuxCore/LoggingWriter.h"

#include "Helpers.h"

#include <cstdlib>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace nt = nux::testing;

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
  EXPECT_EQ(logger.GetLogLevel(), Level::NotSpecified);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), Level::Warning);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(Level::Debug);
  EXPECT_EQ(logger.GetLogLevel(), Level::Debug);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), Level::Debug);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_TRUE(logger.IsInfoEnabled());
  EXPECT_TRUE(logger.IsDebugEnabled());
  logger.SetLogLevel(Level::Info);
  EXPECT_EQ(logger.GetLogLevel(), Level::Info);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), Level::Info);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_TRUE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(Level::Warning);
  EXPECT_EQ(logger.GetLogLevel(), Level::Warning);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), Level::Warning);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(Level::Error);
  EXPECT_EQ(logger.GetLogLevel(), Level::Error);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), Level::Error);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_FALSE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(Level::NotSpecified);
  EXPECT_EQ(logger.GetLogLevel(), Level::NotSpecified);
  EXPECT_EQ(logger.GetEffectiveLogLevel(), Level::Warning);
}

TEST(TestLogger, TestLevelsSharedForSameModule) {
  Logger logger1("testing.module");
  Logger logger2("testing.module");

  logger1.SetLogLevel(Level::Info);
  EXPECT_TRUE(logger1.IsInfoEnabled());
  EXPECT_TRUE(logger2.IsInfoEnabled());
}

TEST(TestLogger, TestModuleLowered) {
  Logger logger1("TESTING.MODULE");
  Logger logger2("Testing");

  EXPECT_THAT(logger1.module(), Eq("testing.module"));
  EXPECT_THAT(logger2.module(), Eq("testing"));
}

TEST(TestLogger, TestLevelsInherited) {
  Logger root("");
  Logger first("first");
  Logger second("first.second");

  root.SetLogLevel(Level::Error);
  EXPECT_EQ(root.GetLogLevel(), Level::Error);
  EXPECT_EQ(root.GetEffectiveLogLevel(), Level::Error);
  EXPECT_EQ(first.GetLogLevel(), Level::NotSpecified);
  EXPECT_EQ(first.GetEffectiveLogLevel(), Level::Error);
  EXPECT_EQ(second.GetLogLevel(), Level::NotSpecified);
  EXPECT_EQ(second.GetEffectiveLogLevel(), Level::Error);

  first.SetLogLevel(Level::Debug);
  EXPECT_EQ(root.GetLogLevel(), Level::Error);
  EXPECT_EQ(root.GetEffectiveLogLevel(), Level::Error);
  EXPECT_EQ(first.GetLogLevel(), Level::Debug);
  EXPECT_EQ(first.GetEffectiveLogLevel(), Level::Debug);
  EXPECT_EQ(second.GetLogLevel(), Level::NotSpecified);
  EXPECT_EQ(second.GetEffectiveLogLevel(), Level::Debug);

  second.SetLogLevel(Level::Info);
  EXPECT_EQ(root.GetLogLevel(), Level::Error);
  EXPECT_EQ(root.GetEffectiveLogLevel(), Level::Error);
  EXPECT_EQ(first.GetLogLevel(), Level::Debug);
  EXPECT_EQ(first.GetEffectiveLogLevel(), Level::Debug);
  EXPECT_EQ(second.GetLogLevel(), Level::Info);
  EXPECT_EQ(second.GetEffectiveLogLevel(), Level::Info);

  first.SetLogLevel(Level::NotSpecified);
  EXPECT_EQ(root.GetLogLevel(), Level::Error);
  EXPECT_EQ(root.GetEffectiveLogLevel(), Level::Error);
  EXPECT_EQ(first.GetLogLevel(), Level::NotSpecified);
  EXPECT_EQ(first.GetEffectiveLogLevel(), Level::Error);
  EXPECT_EQ(second.GetLogLevel(), Level::Info);
  EXPECT_EQ(second.GetEffectiveLogLevel(), Level::Info);
}

class UseTimezone
{
public:
  UseTimezone(std::string const& tz)
    {
      char* old_tz = ::getenv("TZ");
      if (old_tz) {
        // std::strings don't like beeing assigned NULL
        old_tz_ = old_tz;
      }
      ::setenv("TZ", tz.c_str(), true);
      ::tzset();
    }

  ~UseTimezone()
    {
      if (old_tz_.empty()) {
        ::unsetenv("TZ");
      }
      else {
        ::setenv("TZ", old_tz_.c_str(), true);
      }
      ::tzset();
    }
private:
  std::string old_tz_;
};

TEST(TestLoggingWriter, TestWriteMessage) {

  nt::CaptureLogOutput log_output;

  // set a known timezone
  UseTimezone timezone(":Antarctica/Vostok");
  // This time is known to be: 2010-09-10 12:34:45 (UTC+12)
  std::time_t when = 1284078885;
  Writer::Instance().WriteMessage(Level::Error, "test.module", "testfile.cpp",
                                  1234, when, "my message");

  std::string result = log_output.GetOutput();
  // Vostok is UTC+6
  EXPECT_THAT(result, Eq("ERROR 2010-09-10 06:34:45 test.module testfile.cpp:1234 my message\n"));
}

TEST(TestLogStream, TestSimpleConstruction) {
  // First test is to make sure a LogStream can be constructed and destructed.
  LogStream test(Level::Debug, "module", "filename", 42);
}

TEST(TestLogStream, TestOutput) {

  nt::CaptureLogOutput log_output;

  LogStream test(Level::Debug, "module", "filename", 42);
  test << "testing message" << std::flush;

  std::string result = log_output.GetOutput();

  EXPECT_THAT(result, StartsWith("DEBUG"));
  EXPECT_THAT(result, HasSubstr("module filename:42"));
  EXPECT_THAT(result, EndsWith("testing message\n"));
}

TEST(TestLogStream, TestShortenedFilename) {
  // Filenames only show the last path segment.
  nt::CaptureLogOutput log_output;

  LogStream test(Level::Debug, "module", "/some/absolute/filename", 42);
  test << "testing message" << std::flush;

  std::string result = log_output.GetOutput();

  EXPECT_THAT(result, HasSubstr("module filename:42"));
}

TEST(TestLogStream, TestTemporary) {
  // First test is to make sure a LogStream can be constructed and destructed.
  nt::CaptureLogOutput log_output;

  LogStream(Level::Debug, "module", "filename", 42).stream() << "testing message";

  std::string result = log_output.GetOutput();

  EXPECT_THAT(result, StartsWith("DEBUG"));
  EXPECT_THAT(result, HasSubstr(" module filename:42"));
  EXPECT_THAT(result, EndsWith("testing message\n"));
}

TEST(TestLogStream, TestDebugMacro) {
  // First test is to make sure a LogStream can be constructed and destructed.
  nt::CaptureLogOutput log_output;
  int counter = 0;

  Logger logger("test.module");

  LOG_DEBUG(logger) << ++counter << "Not output, as not debug.";

  logger.SetLogLevel(Level::Debug);

  LOG_DEBUG(logger) << ++counter << " Is output now.";

  std::string result = log_output.GetOutput();

  EXPECT_THAT(result, StartsWith("DEBUG"));
  EXPECT_THAT(result, HasSubstr(" test.module gtest-nuxcore-logger.cpp"));
  EXPECT_THAT(result, EndsWith("1 Is output now.\n"));
  // Also only one line.
  EXPECT_THAT(counter, Eq(1));
}

TEST(TestLogStream, TestBlockTracer) {
  nt::CaptureLogOutput log_output;

  Logger logger("test");
  logger.SetLogLevel(Level::Debug);
  {
    BlockTracer tracer(logger, Level::Debug, "func_name", "file_name", 42);
  }

  std::string result = log_output.GetOutput();

  EXPECT_THAT(result, MatchesRegex("DEBUG .+ test file_name:42 \\+func_name\n"
                                   "DEBUG .+ test file_name:42 -func_name\n"));
}


TEST(TestLogHelpers, TestGetLoggingLevel) {
  EXPECT_THAT(get_logging_level("trace"), Eq(Level::Trace));
  EXPECT_THAT(get_logging_level("TrAce"), Eq(Level::Trace));
  EXPECT_THAT(get_logging_level("TRACE"), Eq(Level::Trace));
  EXPECT_THAT(get_logging_level("debug"), Eq(Level::Debug));
  EXPECT_THAT(get_logging_level("DEBUG"), Eq(Level::Debug));
  EXPECT_THAT(get_logging_level("info"), Eq(Level::Info));
  EXPECT_THAT(get_logging_level("INFO"), Eq(Level::Info));
  EXPECT_THAT(get_logging_level("warn"), Eq(Level::Warning));
  EXPECT_THAT(get_logging_level("WARN"), Eq(Level::Warning));
  EXPECT_THAT(get_logging_level("warning"), Eq(Level::Warning));
  EXPECT_THAT(get_logging_level("WARNING"), Eq(Level::Warning));
  EXPECT_THAT(get_logging_level("error"), Eq(Level::Error));
  EXPECT_THAT(get_logging_level("ERROR"), Eq(Level::Error));
  // Unknown levels result in WARNING
  EXPECT_THAT(get_logging_level("critical"), Eq(Level::Warning));
  EXPECT_THAT(get_logging_level("not_specified"), Eq(Level::Warning));
  EXPECT_THAT(get_logging_level("other"), Eq(Level::Warning));
}

TEST(TestLogHelpers, TestResetLogging) {
  // First set root and another.
  Logger("").SetLogLevel(Level::Debug);
  Logger("test.module").SetLogLevel(Level::Info);

  reset_logging();

  std::string levels = dump_logging_levels();

  EXPECT_THAT(levels, Eq("<root> WARNING"));
}


TEST(TestLogHelpers, TestHasModule) {
  LoggerModules& modules = LoggerModules::Instance();
  EXPECT_FALSE(modules.HasModule("test.module.non.existent"));
  Logger("test.module.non.existent");
  EXPECT_TRUE(modules.HasModule("test.module.non.existent"));
}


DECLARE_LOGGER(file_level_logger, "test.module.file.level");

TEST(TestStaticFileFunction, TestLoggerConstruction) {
  LoggerModules& modules = LoggerModules::Instance();
  EXPECT_FALSE(modules.HasModule("test.module.file.level"));
  // Call the function
  file_level_logger();
  EXPECT_TRUE(modules.HasModule("test.module.file.level"));
}


TEST(TestLoggerMacros, TestPassingObject) {
  nt::CaptureLogOutput log_output;
  configure_logging("test.module=trace");
  Logger local("test.module");

  LOG_TRACE(local) << "trace log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("TRACE .+ trace log\n"));
  LOG_DEBUG(local) << "debug log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("DEBUG .+ debug log\n"));
  LOG_INFO(local) << "info log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("INFO .+ info log\n"));
  LOG_WARN(local) << "warn log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("WARN .+ warn log\n"));
  LOG_ERROR(local) << "error log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("ERROR .+ error log\n"));
}

DECLARE_LOGGER(test_logger, "test.module");

TEST(TestLoggerMacros, TestPassingFunction) {
  nt::CaptureLogOutput log_output;
  configure_logging("test.module=trace");

  LoggerFunc the_func = test_logger;

  LOG_TRACE(test_logger) << "trace log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("TRACE .+ trace log\n"));
  LOG_DEBUG(test_logger) << "debug log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("DEBUG .+ debug log\n"));
  LOG_INFO(test_logger) << "info log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("INFO .+ info log\n"));
  LOG_WARN(test_logger) << "warn log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("WARN .+ warn log\n"));
  LOG_ERROR(test_logger) << "error log";
  EXPECT_THAT(log_output.GetOutput(),
              MatchesRegex("ERROR .+ error log\n"));
}


TEST(TestLogHelpers, TestConfigureLoggingNull) {

  reset_logging();
  Logger("").SetLogLevel(Level::Debug);
  Logger("test.module").SetLogLevel(Level::Info);
  // Configure passed a null pointer does nothing.
  configure_logging(NULL);
  std::string levels = dump_logging_levels();

  EXPECT_THAT(levels, Eq("<root> DEBUG\n"
                         "test.module INFO"));
}

TEST(TestLogHelpers, TestConfigureLoggingRoot) {
  reset_logging();
  configure_logging("<root>=debug");
  std::string levels = dump_logging_levels();
  EXPECT_THAT(levels, Eq("<root> DEBUG"));
}

TEST(TestLogHelpers, TestConfigureLoggingSingleModule) {
  reset_logging();
  configure_logging("test.module=debug");
  std::string levels = dump_logging_levels();
  EXPECT_THAT(levels, Eq("<root> WARNING\n"
                         "test.module DEBUG"));
}

TEST(TestLogHelpers, TestConfigureLoggingMultipleModules) {
  reset_logging();
  configure_logging("module=info;sub.module=debug;other.module=warning");
  std::string levels = dump_logging_levels();
  EXPECT_THAT(levels, Eq("<root> WARNING\n"
                         "module INFO\n"
                         "other.module WARNING\n"
                         "sub.module DEBUG"));
}

} // anon namespace
