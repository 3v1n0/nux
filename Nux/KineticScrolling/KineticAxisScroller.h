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

#ifndef NUX_KINETIC_AXIS_SCROLLER_H
#define NUX_KINETIC_AXIS_SCROLLER_H

#include "KineticScrollingEnums.h"

namespace nux
{

namespace kinetic_scrolling
{
  class TickSourceInterface;
}

/*
 Drives the scrolling of an entity based on user input, along an axis

 Given information on user input (e.g. touches and mouse events) it tells
 how a view/entity/object should react by scrolling in a kinetic fashion.

 The scroller moves the content and considers the viewport to be fixed.
 */
class KineticAxisScroller
{
 public:
  /*
    Minimum flick speed, in axis units per millisecond.
    If the flick speed is lower than that there will be no kinetic scrolling.
   */
  static const float MINIMUM_FLICK_SPEED;

  /*
    Multiplier for the flick speed, to make flicking more effective.
   */
  static const float FLICK_BOOST;

  /*!
    Enumeration that describes the possible states of the kinetic scroller
   */
  enum States
  {
   Idle, /*!< It's not pressed and not moving. This is the initial state */
   Pressed, /*!< It's pressed but not yet moving. */
   FollowingFinger, /*!< It's pressed and moving to follow the user's finger */
   MovingByInertia /*!< It's not pressed anymore but it's still moving due to inertia.
                        Reacheable from FollowingFinger.
                        After it loses all its momentum it will stop and therefore
                        move to the Idle state. */
  };

  /*!
    Minimum movement of the finger along an axis necessary for the scroller to
    start following it. I.e. to move from Pressed to FollowingFinger state.
   */
  static const int MOVEMENT_THRESHOLD = 5;

  KineticAxisScroller();

  virtual ~KineticAxisScroller();

  /***** Initialization ******/

  /*!
    Specifies the length of the viewport.

    Any existing momentum or movement is halted. You will usually call this
    method only during initialization.
   */
  void SetViewportLength(int length);

  /*!
    Returns the viewport length set via SetViewportLength()
   */
  int GetViewportLength() const;

  /*!
    Specifies the length of the content.

    Scrolling is only useful when the content is larger than the viewport.

    Any existing momentum or movement is halted. You will usually call this
    method only during initialization.
   */
  void SetContentLength(int length);

  /*!
    Returns the content length set via SetViewportLength()
   */
  int GetContentLength() const;

  /*!
    Sets the content position, relative to viewport's top/left corner.

    Any existing momentum or movement is halted. You will usually call this
    method only during initialization.
   */
  void SetContentPosition(int pos);

  /*!
    Defines what happens when the viewport is about to be beyond content boundaries

    The default value is DragAndOvershootBounds.
   */
  void SetBoundsBehavior(BoundsBehavior bounds_behavior);

  /***** input ******/

  /*!
    Tells the scroller that a finger is touching the scrollable surface

    The scroller is interested only in single finger interactions.
    If subsequent fingers hit the surface they shouldn't be informed to the
    scroller.
    Thus calling this method twice, without a ProcessFingerUp() in between, is
    not supported.
   */
  void ProcessFingerDown();

  /*!
    Tells the scroller that a finger has left the scrollable surface

    \sa ProcessFingerDown()
   */
  void ProcessFingerUp();

  /*!
    Tells the scroller that a finger has moved along the scrollable surface

    \param movement How far it has moved along the axis.
   */
  void ProcessFingerDrag(int movement);

  /*!
    Moves animations forward by delta_time milliseconds.

    After calling this method you will normally want to know the new
    position (GetPosition()) and whether the animation has finished (IsActive()),
    meaning that movement along that axis came to a halt.

    \param delta_time Time elapsed since the last call to Update()
   */ 
  void UpdateTime(int delta_time);

  /***** Scrolling output ******/

  int GetContentPosition() const;

  /*!
    Whether, in its current state, the scroller needs UpdateTime() to be called

    That happens in states where content position changes happen due to internal
    animations and not in direct response to user input like in ProcessFingerDrag()
   */
  bool NeedTimeUpdates() const;

  /*!
    Returns the current state of the kinetic scroller.
   */
  KineticScrollerAxisState GetState() const;

 private:
  class Private;
  Private *p;
};

} // namespace nux

#endif // NUX_KINETIC_AXIS_SCROLLER_H

