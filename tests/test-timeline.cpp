/*
 * Copyright (C) 2010 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by Gordon Allott <gord.allott@canonical.com>
 *
 */

#include <glib.h>
#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/Button.h"

static void test_timeline (void);

void
test_timeline_create_suite (void)
{
#define TESTDOMAIN "/Timeline/"

  g_test_add_func (TESTDOMAIN"/Timeline", test_timeline);
}

class TimelineTestClass
{
public:
  nux::Button *button;
  nux::Timeline *timeline_1;

  TimelineTestClass ()
  {
    has_got_new_frame = false;
  }

  void OnNewFrame (unsigned long msecs)
  {
    has_got_new_frame = true;
  }

  void OnCompleted ()
  {
    nux::GetGraphicsThread ()->NuxMainLoopQuit ();
  }

  void Init (nux::Layout *layout)
  {
  }

  bool has_got_new_frame;

};

void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
  TimelineTestClass *self = (TimelineTestClass*) InitData;
  nux::VLayout* MainVLayout = new nux::VLayout(TEXT(""), NUX_TRACKER_LOCATION);
  MainVLayout->SetContentDistribution(nux::eStackCenter);

  self->Init (MainVLayout);

  nux::GetGraphicsThread()->SetLayout(MainVLayout);
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}

static void
test_timeline (void)
{

  TimelineTestClass *test_class = new TimelineTestClass ();

  nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Timeline Test"), 400, 300, 0, ThreadWidgetInit, test_class);

  nux::Timeline *timeline = new nux::Timeline (1000, "My Timeline", NUX_TRACKER_LOCATION);
  timeline->Looping = false;
  timeline->NewFrame.connect (sigc::mem_fun (test_class, &TimelineTestClass::OnNewFrame));
  timeline->Completed.connect (sigc::mem_fun (test_class, &TimelineTestClass::OnCompleted));

  gint64 micro_secs = g_get_real_time();
  unsigned long current_time = micro_secs / 1000;

  wt->AddTimeline (timeline);
  wt->Run(NULL);

  micro_secs = g_get_real_time();
  g_assert(micro_secs / 1000 - current_time > 1000);
  g_assert (test_class->has_got_new_frame);
  delete test_class;
  delete wt;
}
