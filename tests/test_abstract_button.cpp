#include "Nux/AbstractButton.h"

#include <sigc++/trackable.h>

#include <gtest/gtest.h>
#include <vector>
#include <stdexcept>

namespace {

TEST(TestTypeTraits, TestSerialization) {
  nux::AbstractButton button = nux::AbstractButton ();
  EXPECT_EQ(false, button.togglable());
  EXPECT_EQ(false, button.active());

}

}