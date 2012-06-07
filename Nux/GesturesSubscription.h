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
  /*!
    Typical usage:
      GesturesSubscription *subscription = new GesturesSubscription;
      subscription->SetGestureClass(TAP_GESTURE);
      subscription->SetNumTouches(2);
      subscription->Activate();
   */
  /*
    Wraps all the complexity involved in directly manipulating a GeisSubscription
    and exposes a simpler, C++, API.
   */
  class GesturesSubscription
  {
    public:
      //! Creates a new, inactive, gestures subscription
      GesturesSubscription();

      //! Destroys this gestures subscription.
      virtual ~GesturesSubscription();

      //! Activates the subscription
      void Activate();

      //! Deactivates the subscription
      void Deactivate();

      //! Returns whether the subscription is active
      /*!
        Nux only get gesture events that matches active subscriptions.

        Subscriptions are inactive by default.
       */
      bool IsActive() const {return is_active_;}

      //! Sets the class of gestures this subscription is interested on
      void SetGestureClass(GestureClass gesture_class);

      //! Returns the gesture class that this subscription is interested on
      /*!
        By default it's set to TOUCH_GESTURE
       */
      GestureClass GetGestureClass() const {return gesture_class_;}

      //! The subscription will be interested in gestures with the given number of touches
      void SetNumTouches(unsigned int num_touches);

      //! Returns the number of touches that this subscription is interested on
      /*!
        By default it is set to two touches.
       */
      unsigned int GetNumTouches() const {return num_touches_;}

      //! Specifies the native window from which touch events will be gathered
      /*!
        See also GetWindowId()
       */
      void SetWindowId(int window_id);

      //! Returns the native window id from which touch events are gathered.
      /*!
        By default this is the same native window that Nux uses for rendering.
        But in cases they differ you should speficy the native window that will
        provide the touch events with SetWindowId().
       */
      int GetWindowId() const {return window_id_;}

      //! Returns whether the gesture from the given event matches this subscription
      bool MatchesGesture(const GestureEvent &event) const;

    private:
      static GeisString MapToGeisGestureClass(GestureClass nux_gesture_class);
      void CreateGeisSubscription();
      void CreateGeisSubscriptionWhenPossible();
      void UpdateGeisSubscription();

      template<typename T>
      void SetProperty(T& prop, T new_value)
      {
        if (prop == new_value)
          return;

        prop = new_value;

        if (sub_)
          UpdateGeisSubscription();
      }

      GestureClass gesture_class_;
      unsigned int num_touches_;
      int window_id_;
      GeisSubscription sub_;
      bool is_active_;
  };

  typedef std::shared_ptr<GesturesSubscription> ShGesturesSubscription;

} // namespace nux

#endif // NUX_GESTURES_SUPPORT
#endif // NUX_GESTURES_SUBSCRIPTION_H
