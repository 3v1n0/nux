#include <string>
#include <fstream>

#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>

#include "Nux/Nux.h"
#include "Nux/HLayout.h"
#include "Nux/ProgramFramework/TestView.h"

using namespace testing;

namespace {

TEST(TestWindowCompositor, TestSetKeyFocusArea)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Nux Window", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::TestView* test_view0 = new nux::TestView();
  nux::TestView* test_view1 = new nux::TestView();

  nux::HLayout* layout = new nux::HLayout();
  
  layout->AddView(test_view0, 1);
  layout->AddView(test_view1, 1);

  wnd_thread->SetLayout(layout);

  // Set keyboard focus on test_view0
  {
    wnd_thread->GetWindowCompositor().SetKeyFocusArea(test_view0);

    // Check that test_view0 has registered a keynav focus event
    EXPECT_EQ(test_view0->registered_begin_keynav_focus_, true);
    EXPECT_EQ(test_view0->registered_end_keynav_focus_, false);

    EXPECT_EQ(test_view1->registered_begin_keynav_focus_, false);
    EXPECT_EQ(test_view1->registered_end_keynav_focus_, false);

    test_view0->ResetKeyFocusEvents();
  }

  // Set keyboard focus on test_view1
  {
    wnd_thread->GetWindowCompositor().SetKeyFocusArea(test_view1);

    EXPECT_EQ(test_view0->registered_begin_keynav_focus_, false);
    // Check that test_view0 has loast the keynav focus.
    EXPECT_EQ(test_view0->registered_end_keynav_focus_, true);

    // Check that test_view1 has registered a keynav focus event
    EXPECT_EQ(test_view1->registered_begin_keynav_focus_, true);
    EXPECT_EQ(test_view1->registered_end_keynav_focus_, false);
  }

  delete wnd_thread;
}


}
