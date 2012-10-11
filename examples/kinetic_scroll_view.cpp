/*
 * Copyright (C) 2012 - Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 */

#include <Nux/Nux.h>
#include <Nux/KineticScrollView.h>
#include <Nux/HLayout.h>
#include <Nux/VLayout.h>

#include "TestButton.h"

using namespace nux;

View *CreateKineticScrollView()
{
  VLayout *layout = new VLayout (NUX_TRACKER_LOCATION);

  char buffer[500];
  for (int i = 0; i < 100; i++)
  {
    sprintf(buffer, "TestButton %d", i+1);
    Button *button = new TestButton(buffer, NUX_TRACKER_LOCATION);
    button->SetMinimumHeight(50);

    layout->AddView(button, 1, eLeft, eFull);
  }

  KineticScrollView *kinetic_scroll_view = new KineticScrollView(NUX_TRACKER_LOCATION);
  kinetic_scroll_view->SetLayout(layout);
  kinetic_scroll_view->SetScrollableDirections(ScrollableDirectionsVertical);

  return kinetic_scroll_view;
}

void UserInterfaceInitialization(NThread* /* thread */, void* /* InitData */)
{

  HLayout* mainLayout = new HLayout(NUX_TRACKER_LOCATION);
  mainLayout->AddView(CreateKineticScrollView(), 1, eCenter, eFull);

  GetWindowThread()->SetLayout(mainLayout);
}

int main()
{
  NuxInitialize(0);

  WindowThread* window_thread = CreateGUIThread(
      "KineticScrollView Example", 640, 300, 0, &UserInterfaceInitialization, 0);

  // Start the main loop.
  window_thread->Run(NULL);

  delete window_thread;
  return 0;
}
