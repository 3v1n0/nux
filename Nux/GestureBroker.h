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

#ifndef NUX_GESTURE_BROKER_H
#define NUX_GESTURE_BROKER_H
#include "Nux/Features.h"
#ifdef NUX_GESTURES_SUPPORT

#include <sigc++/sigc++.h>

#include "Gesture.h"

namespace nux
{
/*!
  Receives GestureEvents and delivers them to the appropriate
  gesture targets.
 */
class GestureBroker : public sigc::trackable
{
public:
  GestureBroker();
  virtual ~GestureBroker();

  void ProcessGestureBegin(nux::GestureEvent &event);
  void ProcessGestureUpdate(nux::GestureEvent &event);
  void ProcessGestureEnd(nux::GestureEvent &event);

private:

  bool BindNewGestureToTarget(nux::GestureEvent &event, ShPtGestureTarget target);

  void ResolveBufferedGestureThatFinishedConstruction(
      std::shared_ptr<Gesture> &gesture);

  /*!
    Finds the targets of the gesture from the given gesture event.
   */
  std::vector<ShPtGestureTarget> virtual FindGestureTargets(const nux::GestureEvent &event) = 0;

  GestureSet gesture_set_;
  std::map <std::shared_ptr <Gesture>, sigc::connection> gesture_lost_all_targets_connections_;
};

} // namespace nux
#endif // NUX_GESTURES_SUPPORT
#endif // NUX_GESTURE_BROKER_H
