#include <gtest/gtest.h>
#include <glib-object.h>

#include "Nux/Nux.h"

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  g_type_init();
  nux::NuxInitialize(0);

  return RUN_ALL_TESTS();
}
