#include <memory>
#include <gmock/gmock.h>
#include <glib.h>

#include "Nux/Nux.h"
#include "NuxGraphics/GraphicsDisplay.h"

using namespace testing;
using namespace nux;

namespace {

struct TestGraphicsDisplay : Test
{
  TestGraphicsDisplay()
    : graphic_display_(nullptr)
  {}

  virtual void SetUp()
  {
    nux::NuxInitialize(0);
    wnd_thread.reset(nux::CreateNuxWindow("nux::TestGraphicsDisplay", 300, 200, nux::WINDOWSTYLE_NORMAL, NULL, false, NULL, NULL));
    graphic_display_ = nux::GetGraphicsDisplay();
    ASSERT_NE(graphic_display_, nullptr);
  }

  std::unique_ptr<nux::WindowThread> wnd_thread;
  GraphicsDisplay* graphic_display_;
};

#if defined(USE_X11)

TEST_F(TestGraphicsDisplay, X11TimestampOnButtonPress)
{
  XEvent xevent;
  xevent.type = ButtonPress;
  xevent.xany.display = graphic_display_->GetX11Display();
  xevent.xbutton.time = g_random_int();
  graphic_display_->ProcessXEvent(xevent, true);

  auto event_time = nux::GetGraphicsDisplay()->GetCurrentEvent().x11_timestamp;
  EXPECT_EQ(xevent.xbutton.time, event_time);
}

#endif

}