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

#ifndef NUX_GESTURES_SUBSCRIPTION_H
#define NUX_GESTURES_SUBSCRIPTION_H
#include "Features.h"
#ifdef NUX_GESTURES_SUPPORT

#include <geis/geis.h>
#include <memory>

// For the GestureClass enumeration.
#include "NuxGraphics/GestureEvent.h"

namespace nux
{
  //! Represents a subscription to multitouch gestures
  class GesturesSubscription
  {
    public:
      //! Creates a new multitouch gestures subscription
      /*!
        Creates a subscription for multitouch gestures of the given class and with
        the given number of touches.
       */
      GesturesSubscription(GestureClass gesture_class, unsigned int num_touches);

      //! Destroys this gestures subscription.
      virtual ~GesturesSubscription();

      //! Activates the subscription
      void Activate();

      //! Deactivates the subscription
      void Deactivate();

      //! Returns whether the subscription is active
      /*!
        Subscriptions are active by default. So you would only call this method
        if Deactivate() has been previously called on this instance.
       */
      bool IsActive() const {return is_active_;}

      //! Returns the gesture class that this subscription is interested on
      GestureClass GetGestureClass() const {return gesture_class_;}

      //! Returns the number of touches that this subscription is interested on
      unsigned int GetNumTouches() const {return num_touches_;}

      //! Returns whether the gesture from the given event matches this subscription
      bool MatchesGesture(const GestureEvent &event) const;

    private:
      static GeisString MapToGeisGestureClass(GestureClass nux_gesture_class);
      void CreateGeisSubscription();

      GestureClass gesture_class_;
      unsigned int num_touches_;
      GeisSubscription sub_;
      bool is_active_;
  };

  typedef std::shared_ptr<GesturesSubscription> ShGesturesSubscription;

} // namespace nux

#endif // NUX_GESTURES_SUPPORT
#endif // NUX_GESTURES_SUBSCRIPTION_H
