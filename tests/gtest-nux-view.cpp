#include <string>
#include <fstream>

#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>

#include "Nux/Nux.h"
#include "Nux/StaticText.h"
#include "Nux/ProgramFramework/TestView.h"


using namespace testing;

namespace {

TEST(TestView, TestViewEnable)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("View Test", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::TestView* test_view = new nux::TestView("");
  
  test_view->EnableView();
  EXPECT_TRUE(test_view->IsViewEnabled());

  test_view->DisableView();
  EXPECT_FALSE(test_view->IsViewEnabled());


  test_view->UnReference();
  delete wnd_thread;
}

}
