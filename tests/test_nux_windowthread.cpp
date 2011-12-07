#include <string>
#include <fstream>

#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>

#include "Nux/Nux.h"


using namespace testing;

namespace {

TEST(TestWindowThread, TestCreate)
{
  nux::WindowThread *wnd_thread = nux::CreateSingleNuxWindow("Nux Window", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  ASSERT_TRUE(wnd_thread != NULL);
  EXPECT_EQ(wnd_thread->GetWindowTitle(), std::string("Nux Window"));
  EXPECT_EQ(wnd_thread->IsModalWindow(), false);

  EXPECT_EQ(wnd_thread->IsComputingLayout(), false);
  EXPECT_EQ(wnd_thread->IsInsideLayoutCycle(), false);

  delete wnd_thread;
}


} // anon namespace
