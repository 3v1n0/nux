#include <string>
#include <fstream>

#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>

#include "Nux/Nux.h"
#include "Nux/HLayout.h"
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

TEST(TestView, TestQueueDraw)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("View Test", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::TestView* main_view = new nux::TestView("");

  nux::HLayout* hlayout = new nux::HLayout("");
  main_view->SetLayout(hlayout);

  nux::TestView* test_view1 = new nux::TestView("");
  hlayout->AddView(test_view1);
  nux::TestView* test_view2 = new nux::TestView("");
  hlayout->AddView(test_view2);
  nux::TestView* test_view3 = new nux::TestView("");
  hlayout->AddView(test_view3);

  hlayout->ComputeContentSize();
  EXPECT_EQ(main_view->calls_to_queue_draw_, 0);
  EXPECT_EQ(test_view1->calls_to_queue_draw_, 0);
  EXPECT_EQ(test_view2->calls_to_queue_draw_, 0);
  EXPECT_EQ(test_view3->calls_to_queue_draw_, 0);

  main_view->QueueDraw();
  EXPECT_EQ(main_view->calls_to_queue_draw_, 1);
  EXPECT_EQ(test_view1->calls_to_queue_draw_, 1);
  EXPECT_EQ(test_view2->calls_to_queue_draw_, 1);
  EXPECT_EQ(test_view3->calls_to_queue_draw_, 1);

  int old_size = wnd_thread->GetDrawList().size();
  main_view->QueueDraw();
  test_view2->QueueDraw();
  int new_size = wnd_thread->GetDrawList().size();
  EXPECT_EQ(old_size, new_size);  // LP: #1014610

  EXPECT_EQ(main_view->calls_to_queue_draw_, 2);
  EXPECT_EQ(test_view1->calls_to_queue_draw_, 1);
  EXPECT_EQ(test_view2->calls_to_queue_draw_, 2);
  EXPECT_EQ(test_view3->calls_to_queue_draw_, 1);

  main_view->UnReference();
  delete wnd_thread;
}

}
