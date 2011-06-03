#include "NuxCore/Logger.h"

#include <gtest/gtest.h>


using namespace nux::logging;

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
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(DEBUG);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_TRUE(logger.IsInfoEnabled());
  EXPECT_TRUE(logger.IsDebugEnabled());
  logger.SetLogLevel(INFO);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_TRUE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(WARNING);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());
  logger.SetLogLevel(ERROR);
  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_FALSE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());

}

TEST(TestLogger, TestLevelsSharedForSameModule) {
  Logger logger1("testing.module");
  Logger logger2("testing.module");

  logger1.SetLogLevel(INFO);
  EXPECT_TRUE(logger1.IsInfoEnabled());
  EXPECT_TRUE(logger2.IsInfoEnabled());
}

} // anon namespace
