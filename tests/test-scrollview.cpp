/*
 * Copyright 2011 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Andrea Azzarone <azzaronea@gmail.com>
 *
 */
 
#include "Nux/Nux.h"
#include "test-scrollview.h"

/*
  TestScrollView:
    This class is a Nux scrollview that process the mouse wheel events
    to make sure that everyghing work well.
*/

NUX_IMPLEMENT_OBJECT_TYPE(TestScrollView);

TestScrollView::TestScrollView(NUX_FILE_LINE_DECL)
  : nux::ScrollView(NUX_FILE_LINE_PARAM)
  , registered_scroll_(false)
  , normal_color_(nux::color::Green)
  , current_color_(normal_color_)
{
  EnableVerticalScrollBar(true);
  EnableHorizontalScrollBar(true);
  
  scrolling.connect(sigc::mem_fun(this, &TestScrollView::OnScrolling));
}

TestScrollView::~TestScrollView()
{

}

void TestScrollView::ResetEvents()
{   
  registered_scroll_ = false;
  
  current_color_ = normal_color_; 
}

nux::Color TestScrollView::GetColor() const
{
  return current_color_;
}

void TestScrollView::Draw(nux::GraphicsEngine &graphics_engine, bool /* force_draw */)
{
  nux::Geometry geo = GetGeometry();
  graphics_engine.QRP_Color(geo.x, geo.y, geo.width, geo.height, current_color_);
}

void TestScrollView::OnScrolling(int /* hscroll */, int /* vscroll */)
{
  registered_scroll_ = true;

  QueueDraw();
}

