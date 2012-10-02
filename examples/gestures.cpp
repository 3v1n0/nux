/*
 * Copyright 2012 Canonical Ltd.
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
 *
 */

#include "Nux/FloatingWindow.h"
#include "Nux/GesturesSubscription.h"
#include "Nux/Nux.h"
#include "Nux/WindowThread.h"
#include <iostream>

using namespace nux;

BaseWindow *window = nullptr;

class GesturalWindow : public FloatingWindow
{
  NUX_DECLARE_OBJECT_TYPE(GesturalWindow, FloatingWindow);
  public:
  GesturalWindow(const char *window_name = "", NUX_FILE_LINE_PROTO)
    : FloatingWindow(window_name, NUX_FILE_LINE_PARAM) 
  {
    CreateGesturesSubscription(TOUCH_GESTURE, 3);
    event_count = 0;
  }

  virtual ~GesturalWindow() {}

  virtual GestureDeliveryRequest GestureEvent(const nux::GestureEvent & /* event */)
  {
    std::cout << "GesturalWindow got GestureEvent "
              << ++event_count << "\n";

    return GestureDeliveryRequest::NONE;
  }

  int event_count;
};
NUX_IMPLEMENT_OBJECT_TYPE(GesturalWindow);

int GesturesEventInspector(Area* /* area */, Event* event, void* /* data */)
{
  if (event->type < EVENT_GESTURE_BEGIN || event->type > EVENT_GESTURE_END)
    return false;

  GestureEvent *gesture_event = static_cast<GestureEvent*>(event);

  if (gesture_event->type == EVENT_GESTURE_BEGIN)
  {
    std::cout << "Gesture begun\n";
  }
  else if (gesture_event->type == EVENT_GESTURE_UPDATE)
    std::cout << "Gesture updated\n";
  else
    std::cout << "Gesture ended\n";

  return false;
}

void UserInterfaceInitialization(NThread* /* thread */, void* /* InitData */)
{
  window = new GesturalWindow(TEXT("Gestural Window"), NUX_TRACKER_LOCATION);
  window->SetBaseXY(100, 20);
  window->ShowWindow(true);
}

int main()
{
  // Initialize Nux.
  NuxInitialize(0);

  // Create a window thread.
  WindowThread* window_thread = CreateGUIThread(
      "Gestures", 640, 300, 0, &UserInterfaceInitialization, 0);

  window_thread->InstallEventInspector(GesturesEventInspector, nullptr);

  // Start the main loop.
  window_thread->Run(NULL);

  window->Dispose(); // will cause the window to be deleted (as ref count is zeroed)
  delete window_thread;
  return 0;
}
