#include <gtest/gtest.h>
#include <glib-object.h>

#include "NuxCore/NuxCore.h"

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  g_type_init();
  nux::NuxCoreInitialize(0);

  return RUN_ALL_TESTS();
}
