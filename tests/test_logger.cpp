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

}
