#include <gtest/gtest.h>
#include <glib-object.h>

#include "NuxCore/NuxCore.h"

int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
#if G_ENCODE_VERSION (GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION) <= GLIB_VERSION_2_34
  g_type_init();
#else
  nux::NuxCoreInitialize(0);

  return RUN_ALL_TESTS();
}
