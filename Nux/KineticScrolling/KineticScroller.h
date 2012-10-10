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

#ifndef NUX_KINETIC_SCROLLER_H
#define NUX_KINETIC_SCROLLER_H

#include "KineticScrollingEnums.h"

namespace nux
{

namespace kinetic_scrolling
{
  class TickSourceInterface;
}

/*
 Drives the scrolling of an entity based on user input

 Given information on user input (e.g. touches and mouse events) it tells
 how a view/entity/object should react by scrolling in a kinetic fashion.

 Thus this class doesn't do anything by itself but must be attached or composed with
 the object that you want to have kinetic scrolling.

 The scroller moves the content and considers the viewport to be fixed.
 */
class KineticScroller
{
 public:

  /*!
    Minimum movement of the finger along an axis necessary for the scroller to
    start following it. I.e. to move from Pressed to FollowingFinger state.
   */
  static const int MOVEMENT_THRESHOLD = 5;

  KineticScroller();

  /*!
    Constructor useful for testing purposes.
    When testing you would like to manually drive the animations
   */
  KineticScroller(kinetic_scrolling::TickSourceInterface *tick_source);

  virtual ~KineticScroller();

  /***** Initialization ******/

  /*!
    Specifies the size of the viewport.

    Any existing momentum or movement is halted. You will usually call this
    method only during initialization.
   */
  void SetViewportSize(int width, int height);

  /*!
    Specifies the size of the content.

    Scrolling is only useful when the content is larger than the viewport.

    Any existing momentum or movement is halted. You will usually call this
    method only during initialization.
   */
  void SetContentSize(int width, int height);

  /*!
    Sets the content position, relative to viewport's top-left corner.

    Any existing momentum or movement is halted. You will usually call this
    method only during initialization.
   */
  void SetContentPosition(int x, int y);

  /*!
    Defines what happens when the viewport is about to be beyond content boundaries

    The default value is DragAndOvershootBounds.
   */
  void SetBoundsBehavior(BoundsBehavior bounds_behavior);

  /*!
    Defines which directions can be scrolled.

    The default value is Auto.
   */
  void SetScrollableDirections(ScrollableDirections scrollable_directions);

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

    \param dx How far it has moved along the X axis.
    \param dx How far it has moved along the Y axis.
   */
  void ProcessFingerDrag(int dx, int dy);

  /***** Scrolling output ******/

  //! Emitted when the content position changes
  /*!
    It won't be emitted due to a call to SetContentPosition().

    @param int X coordinate of the new content position.
    @param int Y coordinate of the new content position.
   */
  sigc::signal<void, int, int> content_position_changed;

  KineticScrollerAxisState GetHorizontalAxisState() const;

  KineticScrollerAxisState GetVerticalAxisState() const;

 private:
  class Private;
  Private *p;
};

} // namespace nux

#endif // NUX_KINETIC_SCROLLER_H
