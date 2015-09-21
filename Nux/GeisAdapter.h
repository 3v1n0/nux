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

#ifndef GEISADAPTER_H
#define GEISADAPTER_H
#include "Nux/Features.h"
#ifdef NUX_GESTURES_SUPPORT

#include <sigc++/sigc++.h>
#include <geis/geis.h>
#include <glib.h>

#include <map>
#include <memory>

#include "NuxGraphics/GestureEvent.h"

namespace nux
{

/*!
  This class translates events from geis into native nux::GestureEvents
 */
class GeisAdapter : public sigc::trackable
{
public:
  GeisAdapter();
  ~GeisAdapter();

  void CreateGSource(GMainContext *context);

  /*
    Returns whether initialization is complete.
    See also: signal init_complete()
   */
  bool IsInitComplete() const {return is_init_complete_;}

  /*
    Initialization has finished. Subscriptions
    can now be filtered and activated.
   */
  sigc::signal<void> init_complete;

  //! Emitted when a new GestureEvent is ready to be processed
  sigc::signal<void, GestureEvent &> event_ready;

  Geis GetGeisInstance() const {return geis_;}

  void ProcessGeisEvents();

  //! Processes the next pending Geis event, which may yield a GestureEvent
  /*!
    Returns true if GestureEvent was filled and false otherwise.
   */
  bool ProcessNextEvent(GestureEvent *event);

private:

  class Device {
    public:
    int id;
    bool direct_touch;
  };


  void ProcessEventDeviceAvailable(GeisEvent event);
  void ProcessEventDeviceUnavailable(GeisEvent event);
  void ProcessEventClassAvailable(GeisEvent event);
  void FillNuxEvent(GestureEvent &nux_event,
                    GeisEvent event,
                    EventType nux_event_type);

  void FillNuxEventGestureAttributes(GestureEvent &nux_event, GeisFrame frame);
  void FillNuxEventTouches(GestureEvent &nux_event, GeisTouchSet touch_set);

  void SplitUpdateIntoBeginAndEnd(GestureEvent &nux_event);

  Geis geis_;

  GeisGestureClass class_drag_;
  GeisGestureClass class_pinch_;
  GeisGestureClass class_rotate_;
  GeisGestureClass class_tap_;
  GeisGestureClass class_touch_;

  bool is_init_complete_;

  /* maps a device id to its corresponding device */
  std::map<int, Device> devices_;

  /* Sometimes a single GeisEvent can yield two GestureEvents. When that
     happens, that second GestureEvent will be temporarily held here until
     it's consumed. */
  std::unique_ptr<GestureEvent> pending_next_event_;

  friend class GeisAdapterTest;
};

} // namespace nux

#endif // NUX_GESTURES_SUPPORT
#endif // GEISADAPTER_H
