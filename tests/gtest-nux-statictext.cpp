#include <string>
#include <fstream>

#include <iostream>
#include <gmock/gmock.h>
#include <boost/filesystem.hpp>
#include <glib.h>

#include "Nux/Nux.h"
#include "Nux/StaticText.h"


using namespace testing;

namespace {

TEST(TestStaticText, TestCreate)
{
  nux::NuxInitialize(0);
  nux::WindowThread *wnd_thread = nux::CreateNuxWindow("Nux Window", 300, 200,
    nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL);

  nux::StaticText *statictext = new nux::StaticText("");
  
  EXPECT_EQ(statictext->GetText(), std::string(""));

  // Test the default color. Should be white.
  EXPECT_EQ(statictext->GetTextColor(), nux::color::White);

  // Test random color.
  nux::Color random_color = nux::color::RandomColor();
  statictext->SetTextColor(random_color);
  EXPECT_EQ(statictext->GetTextColor(), random_color);

  // Test default font. Should be "Ubuntu".
  EXPECT_EQ(statictext->GetFontName(), std::string("Ubuntu"));

  // Test random font.
  statictext->SetFontName("Courrier");
  EXPECT_EQ(statictext->GetFontName(), std::string("Courrier"));

  // Test default font point size;
  EXPECT_EQ(statictext->GetTextPointSize(), 10);

  statictext->SetTextPointSize(20);
  EXPECT_EQ(statictext->GetTextPointSize(), 20);

  statictext->UnReference();
  delete wnd_thread;
}

}
