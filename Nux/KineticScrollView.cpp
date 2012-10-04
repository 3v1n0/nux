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
#include <Nux/Layout.h>
#include "KineticScrollView.h"

using namespace nux;

NUX_IMPLEMENT_OBJECT_TYPE(KineticScrollView);

KineticScrollView::KineticScrollView(NUX_FILE_LINE_DECL)
  : View(NUX_FILE_LINE_PARAM),
  mouse_pressed_on_child_(false),
  last_child_mouse_position_x_(0),
  last_child_mouse_position_y_(0)
{
  mouse_down.connect(sigc::mem_fun(this, &KineticScrollView::OnMouseDown));
  mouse_up.connect(sigc::mem_fun(this, &KineticScrollView::OnMouseUp));
  mouse_drag.connect(sigc::mem_fun(this, &KineticScrollView::OnMouseDrag));

  scroller_.content_position_changed.connect(
      sigc::mem_fun(this, &KineticScrollView::SetLayoutTranslation));

  SetTrackChildMouseEvents(true);

  SetBoundsBehavior(BoundsBehavior::DragAndOvershootBounds);
}

KineticScrollView::~KineticScrollView()
{
}

void KineticScrollView::SetBoundsBehavior(BoundsBehavior bounds_behavior)
{
  scroller_.SetBoundsBehavior(bounds_behavior);
}

void KineticScrollView::SetScrollableDirections(ScrollableDirections scrollable_directions)
{
  scroller_.SetScrollableDirections(scrollable_directions);
}

bool KineticScrollView::ChildMouseEvent(const Event& event)
{
  bool want_mouse_ownership = false;

  if (event.type == NUX_MOUSE_PRESSED || event.type == NUX_MOUSE_DOUBLECLICK)
  {
    mouse_pressed_on_child_ = true;
    last_child_mouse_position_x_ = event.x;
    last_child_mouse_position_y_ = event.y;

    scroller_.ProcessFingerDown();
  }
  else if (event.type == NUX_MOUSE_MOVE)
  {
    int dx = event.x - last_child_mouse_position_x_;
    int dy = event.y - last_child_mouse_position_y_;
    scroller_.ProcessFingerDrag(dx, dy);

    if (scroller_.GetHorizontalAxisState() ==
        KineticScrollerAxisStateFollowingFinger
        ||
        scroller_.GetVerticalAxisState() ==
        KineticScrollerAxisStateFollowingFinger)
    {
      want_mouse_ownership = true;
    }
  }
  else if (event.type == NUX_MOUSE_RELEASED)
  {
    mouse_pressed_on_child_ = false;
    scroller_.ProcessFingerUp();
  }

  return want_mouse_ownership;
}

void KineticScrollView::Draw(GraphicsEngine & /* graphics_engine */,
                             bool /* force_draw */)
{
  if (view_layout_)
  {
    view_layout_->QueueDraw();
  }
}

void KineticScrollView::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
{
  if (view_layout_)
    view_layout_->ProcessDraw(graphics_engine, force_draw);
}

long KineticScrollView::PostLayoutManagement(long /* LayoutResult */)
{
  if (view_layout_)
  {
    scroller_.SetContentSize(view_layout_->GetBaseWidth(),
                             view_layout_->GetBaseHeight());
  }
  scroller_.SetViewportSize(GetBaseWidth(), GetBaseHeight());

  /* the return value is meaningless and not used for anything. */
  return 0; 
}

void KineticScrollView::PostResizeGeometry()
{
  scroller_.SetViewportSize(GetBaseWidth(), GetBaseHeight());
}

void KineticScrollView::OnMouseDown(int /* x */, int /* y */,
                            unsigned long /* button_flags */,
                            unsigned long /* key_flags */)
{
  scroller_.ProcessFingerDown();
}

void KineticScrollView::OnMouseUp(int /* x */, int /* y */,
                          unsigned long /* button_flags */,
                          unsigned long /* key_flags */)
{
  scroller_.ProcessFingerUp();
}

void KineticScrollView::OnMouseDrag(int /* x */, int /* y */, int dx, int dy,
                            unsigned long /* button_flags */,
                            unsigned long /* key_flags */)
{
  scroller_.ProcessFingerDrag(dx, dy);
}

void KineticScrollView::SetLayoutTranslation(int x, int y)
{
  if (view_layout_)
  {
    view_layout_->Set2DTranslation(x, y, 0);
    QueueDraw();
  }
}
