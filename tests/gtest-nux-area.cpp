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

TEST(TestArea, TestAreaSize)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Area Test", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::TestView* test_view = new nux::TestView("");

  EXPECT_EQ(test_view->GetMinimumWidth(), nux::AREA_MIN_WIDTH);
  EXPECT_EQ(test_view->GetMaximumWidth(), nux::AREA_MAX_WIDTH);
  EXPECT_EQ(test_view->GetMinimumHeight(), nux::AREA_MIN_HEIGHT);
  EXPECT_EQ(test_view->GetMaximumHeight(), nux::AREA_MAX_HEIGHT);

  test_view->SetMinimumWidth(100);
  EXPECT_EQ(test_view->GetMinimumWidth(), 100);

  test_view->SetMaximumWidth(1111);
  EXPECT_EQ(test_view->GetMaximumWidth(), 1111);

  test_view->SetMinimumHeight(123);
  EXPECT_EQ(test_view->GetMinimumHeight(), 123);

  test_view->SetMaximumHeight(1234);
  EXPECT_EQ(test_view->GetMaximumHeight(), 1234);

  test_view->SetMaximumSize(4321, 1234);
  test_view->SetMinimumSize(123, 432);

  EXPECT_EQ(test_view->GetMinimumWidth(), 123);
  EXPECT_EQ(test_view->GetMaximumWidth(), 4321);
  EXPECT_EQ(test_view->GetMinimumHeight(), 432);
  EXPECT_EQ(test_view->GetMaximumHeight(), 1234);

  test_view->UnReference();
  delete wnd_thread;
}

TEST(TestArea, TestAreaGeometry)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Area Test", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::TestView* test_view = new nux::TestView("");

  static_cast<nux::Area*>(test_view)->SetGeometry(0, 0, 100, 100);

  EXPECT_EQ(test_view->GetBaseX(), 0);
  EXPECT_EQ(test_view->GetBaseX(), 0);
  EXPECT_EQ(test_view->GetBaseWidth(), 100);
  EXPECT_EQ(test_view->GetBaseHeight(), 100);


  test_view->UnReference();
  delete wnd_thread;
}
}
