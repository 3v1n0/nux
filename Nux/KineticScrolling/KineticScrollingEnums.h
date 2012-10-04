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

#ifndef NUX_KINETIC_SCROLLING_ENUMS_H
#define NUX_KINETIC_SCROLLING_ENUMS_H
namespace nux
{

enum BoundsBehavior
{
  StopAtBounds, /*!< content can not be dragged beyond viewport boundaries, and
                     flicks will not overshoot */
  DragOverBounds, /*!< content can be dragged beyond viewport boundaries, but
                       flicks will not overshoot. */
  DragAndOvershootBounds /*!< content can be dragged beyond viewport boundaries
                              and can overshoot it when flicked. */
};

enum ScrollableDirections
{
  ScrollableDirectionsAuto,
  ScrollableDirectionsHorizontal,
  ScrollableDirectionsVertical,
  ScrollableDirectionsHorizontalAndVertical
};

/*!
  Enumeration that describes the possible states of an axis of the kinetic scroller

  Kinetic scrolling works for each axis independently. Therefore each axis has its
  own separate state.
 */
enum KineticScrollerAxisState
{

  /*!< It's not pressed and not moving. This is the initial state.
       This is also the state an axis will be if scrolling is not
       possible or enabled for it. */
  KineticScrollerAxisStateIdle,

  /*!< It's pressed but not yet moving. */
  KineticScrollerAxisStatePressed,

  /*!< It's pressed and moving to follow the user's finger */
  KineticScrollerAxisStateFollowingFinger,

  /*!< It's not pressed anymore but it's still moving due to inertia.
       Reacheable from FollowingFinger. After it loses all its momentum
       it will stop and therefore move to the Idle state. */
  KineticScrollerAxisStateMovingByInertia };

} // namespace nux
#endif // NUX_KINETIC_SCROLLING_ENUMS_H
