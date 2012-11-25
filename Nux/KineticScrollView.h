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

#ifndef NUX_KINETIC_SCROLL_VIEW_H
#define NUX_KINETIC_SCROLL_VIEW_H

#include <Nux/View.h>
#include <Nux/KineticScrolling/KineticScroller.h>
#include <Nux/KineticScrolling/KineticScrollingEnums.h>

namespace nux
{

/*
  Used to scroll content that is bigger than the available space to display it.

  Unlike ScrollView, where scroll bars are used to move the content, here you directly
  drag the content with a mouse pointer or finger (in case of a touchscreen).

  Items inside a KineticScrollView should properly handle events of type
  EVENT_MOUSE_CANCEL. If an item inside a kinetic scroll view is pressed, it gets
  a EVENT_MOUSE_DOWN as usual. But if the user starts dragging it (instead of
  releasing it to generete a "click"), the KineticScrollView will get ownership of the
  mouse and that item will receive an EVENT_MOUSE_CANCEL signalling that it no
  longer has the mouse and won't get any further events from it. The mouse will
  then be used for scrolling or flicking the KineticScrollView contents until it's
  released.

  Usage example:

  VLayout *layout = new Layout;
  for (...)
  {
    ...
    layout->AddView(item ...);
  }
  KineticScrollView *kinetic_scroll_view = new KineticScrollView;
  kinetic_scroll_view->SetLayout(layout);

 */
class KineticScrollView : public View
{
  NUX_DECLARE_OBJECT_TYPE(KineticScrollView, View);
 public:
  KineticScrollView(NUX_FILE_LINE_PROTO);
  virtual ~KineticScrollView();

  void SetScrollableDirections(ScrollableDirections scrollable_directions);

  /*!
    Defines what happens when the viewport is about to go beyond content boundaries

    The default value is DragAndOvershootBounds.
   */
  void SetBoundsBehavior(BoundsBehavior bounds_behavior);

  /* Reimplemented */
  virtual bool ChildMouseEvent(const Event& event);

 protected:
  virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
  virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);
  virtual long PostLayoutManagement(long LayoutResult);
  virtual void PostResizeGeometry();

 private:
  void OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseDrag(int x, int y, int dx, int dy,
                   unsigned long button_flags, unsigned long key_flags);
  void SetLayoutTranslation(int x, int y);

  KineticScroller scroller_;

  /* Variables used to process events from ChildMouseEven() */
  bool mouse_pressed_on_child_;
  int last_child_mouse_position_x_;
  int last_child_mouse_position_y_;
};

} // namespace nux

#endif // NUX_KINETIC_SCROLL_VIEW_H
