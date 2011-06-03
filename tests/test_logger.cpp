#include "NuxCore/Logger.h"

#include <gtest/gtest.h>


using namespace nux::logging;

namespace {

TEST(TestLogger, TestDefaultConstruction) {

  Logger logger("");

  EXPECT_TRUE(logger.IsErrorEnabled());
  EXPECT_TRUE(logger.IsWarningEnabled());
  EXPECT_FALSE(logger.IsInfoEnabled());
  EXPECT_FALSE(logger.IsDebugEnabled());

}

}
