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
#include "KineticScroller.h"
#include "KineticAxisScroller.h"
#include "AxisDecelerationAnimation.h"
#include "VelocityCalculator.h"
#include <memory>
#include "KineticScrollingTickSource.h"

using namespace nux;

/*****************************************************************************
 * Private class
 *****************************************************************************/


class KineticScroller::Private : public sigc::trackable
{
 public:

  Private();
  Private(kinetic_scrolling::TickSourceInterface *tick_source);
  void Init();

  void SetViewportSize(int width, int height);
  void SetContentSize(int width, int height);
  void SetContentPosition(int x, int y);

  void ProcessFingerDown();
  void ProcessFingerUp();
  void ProcessFingerDrag(int dx, int dy);
  void UpdateAnimations(int time);

  void CheckChangesInContentPosition();

  bool CanScrollHorizontally() const;
  bool CanScrollVertically() const;

  ScrollableDirections scrollable_directions_;

  std::unique_ptr<kinetic_scrolling::TickSourceInterface> tick_source_;

  sigc::signal<void, int, int> *content_position_changed;

  KineticAxisScroller scroller_x_;
  KineticAxisScroller scroller_y_;

  int last_content_pos_x_;
  int last_content_pos_y_;
};

KineticScroller::Private::Private()
{
  tick_source_.reset(new kinetic_scrolling::TimerTickSource);
  Init();
}

KineticScroller::Private::Private(kinetic_scrolling::TickSourceInterface *tick_source)
{
  tick_source_.reset(tick_source);
  Init();
}

void KineticScroller::Private::Init()
{
  scrollable_directions_ = ScrollableDirectionsAuto;

  tick_source_->tick.connect(
      sigc::mem_fun(this, &KineticScroller::Private::UpdateAnimations));
}

void KineticScroller::Private::SetViewportSize(int width, int height)
{
  scroller_x_.SetViewportLength(width);
  scroller_y_.SetViewportLength(height);
}

void KineticScroller::Private::SetContentSize(int width, int height)
{
  scroller_x_.SetContentLength(width);
  scroller_y_.SetContentLength(height);
}

void KineticScroller::Private::SetContentPosition(int x, int y)
{
  scroller_x_.SetContentPosition(x);
  scroller_y_.SetContentPosition(y);
  last_content_pos_x_ = x;
  last_content_pos_y_ = y;
}

void KineticScroller::Private::ProcessFingerDown()
{
  if (CanScrollHorizontally())
    scroller_x_.ProcessFingerDown();

  if (CanScrollVertically())
    scroller_y_.ProcessFingerDown();

  tick_source_->Stop();
}

void KineticScroller::Private::ProcessFingerUp()
{
  if (CanScrollHorizontally())
    scroller_x_.ProcessFingerUp();

  if (CanScrollVertically())
    scroller_y_.ProcessFingerUp();

  if (scroller_x_.NeedTimeUpdates() || scroller_y_.NeedTimeUpdates())
    tick_source_->Start();
}

void KineticScroller::Private::ProcessFingerDrag(int dx, int dy)
{
  if (CanScrollHorizontally())
    scroller_x_.ProcessFingerDrag(dx);

  if (CanScrollVertically())
    scroller_y_.ProcessFingerDrag(dy);

  CheckChangesInContentPosition();
}

void KineticScroller::Private::UpdateAnimations(int delta_time)
{
  if (scroller_x_.NeedTimeUpdates())
    scroller_x_.UpdateTime(delta_time);

  if (scroller_y_.NeedTimeUpdates())
    scroller_y_.UpdateTime(delta_time);

  CheckChangesInContentPosition();

  if (!scroller_x_.NeedTimeUpdates() && !scroller_y_.NeedTimeUpdates())
    tick_source_->Stop();
}

void KineticScroller::Private::CheckChangesInContentPosition()
{
  if (last_content_pos_x_ != scroller_x_.GetContentPosition()
      || last_content_pos_y_ != scroller_y_.GetContentPosition())
  {
    content_position_changed->emit(scroller_x_.GetContentPosition(),
                                   scroller_y_.GetContentPosition());
  }
  last_content_pos_x_ = scroller_x_.GetContentPosition();
  last_content_pos_y_ = scroller_y_.GetContentPosition();
}

bool KineticScroller::Private::CanScrollHorizontally() const
{
  return scrollable_directions_ == ScrollableDirectionsHorizontal
    || scrollable_directions_ == ScrollableDirectionsHorizontalAndVertical
    || (scrollable_directions_ == ScrollableDirectionsAuto
        && scroller_x_.GetViewportLength() < scroller_x_.GetContentLength());
}

bool KineticScroller::Private::CanScrollVertically() const
{
  return scrollable_directions_ == ScrollableDirectionsVertical
    || scrollable_directions_ == ScrollableDirectionsHorizontalAndVertical
    || (scrollable_directions_ == ScrollableDirectionsAuto
        && scroller_y_.GetViewportLength() < scroller_y_.GetContentLength());
}

/*****************************************************************************
 * Public class
 *****************************************************************************/

KineticScroller::KineticScroller()
  : p(new Private)
{
  p->content_position_changed = &content_position_changed;
}

KineticScroller::KineticScroller(kinetic_scrolling::TickSourceInterface *tick_source)
  : p(new Private(tick_source))
{
  p->content_position_changed = &content_position_changed;
}

KineticScroller::~KineticScroller()
{
  delete p;
}

void KineticScroller::SetViewportSize(int width, int height)
{
  p->SetViewportSize(width, height);
}

void KineticScroller::SetContentSize(int width, int height)
{
  p->SetContentSize(width, height);
}

void KineticScroller::SetContentPosition(int x, int y)
{
  p->SetContentPosition(x, y);
}

void KineticScroller::SetBoundsBehavior(BoundsBehavior bounds_behavior)
{
  p->scroller_x_.SetBoundsBehavior(bounds_behavior);
  p->scroller_y_.SetBoundsBehavior(bounds_behavior);
}

void KineticScroller::SetScrollableDirections(
    ScrollableDirections scrollable_directions)
{
  p->scrollable_directions_ = scrollable_directions;
}

void KineticScroller::ProcessFingerDown()
{
  p->ProcessFingerDown();
}

void KineticScroller::ProcessFingerUp()
{
  p->ProcessFingerUp();
}

void KineticScroller::ProcessFingerDrag(int dx, int dy)
{
  p->ProcessFingerDrag(dx, dy);
}

KineticScrollerAxisState KineticScroller::GetHorizontalAxisState() const
{
  return p->scroller_x_.GetState();
}

KineticScrollerAxisState KineticScroller::GetVerticalAxisState() const
{
  return p->scroller_y_.GetState();
}
