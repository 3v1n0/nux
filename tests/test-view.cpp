
#include "Nux/Nux.h"
#include "test-view.h"

/*
  TestView:
    This class is a Nux view that processes all mouse events that it receives. It is meant for testing simulated
    mouse events. 
*/

NUX_IMPLEMENT_OBJECT_TYPE(TestView);

TestView::TestView(NUX_FILE_LINE_DECL)
  : nux::View(NUX_FILE_LINE_PARAM)
{
  ResetEvents();

  normal_color_ = nux::color::Green;
  mouse_down_color_ = nux::color::Red;
  mouse_drag_color_ = nux::color::Yellow;
  mouse_in_color_ = nux::color::Blue;
  current_color_ = normal_color_;

  mouse_in_ = false;
  mouse_mouse_drag_ = false;
  mouse_mouse_down_ = false;

  mouse_down.connect(sigc::mem_fun(this, &TestView::OnMouseDown));
  mouse_up.connect(sigc::mem_fun(this, &TestView::OnMouseUp));
  mouse_drag.connect(sigc::mem_fun(this, &TestView::OnMouseDrag));
  mouse_click.connect(sigc::mem_fun(this, &TestView::OnMouseClick));
  mouse_double_click.connect(sigc::mem_fun(this, &TestView::OnMouseDoubleClick));
  mouse_move.connect(sigc::mem_fun(this, &TestView::OnMouseMove));
  mouse_enter.connect(sigc::mem_fun(this, &TestView::OnMouseEnter));
  mouse_leave.connect(sigc::mem_fun(this, &TestView::OnMouseLeave));
}

TestView::~TestView()
{

}

void TestView::ResetEvents()
{
  registered_mouse_down_          = false;
  registered_mouse_up_            = false;
  registered_mouse_drag_          = false;
  registered_mouse_enter_         = false;
  registered_mouse_leave_         = false;
  registered_mouse_click_         = false;
  registered_mouse_double_click_  = false;
  registered_mouse_move_          = false;
  registered_mouse_enter_         = false;
  registered_mouse_leave_         = false;
    
}

nux::Color TestView::GetColor() const
{
  return current_color_;
}

void TestView::Draw(nux::GraphicsEngine &graphics_engine, bool force_draw)
{
  nux::Geometry geo = GetGeometry();
  graphics_engine.QRP_Color(0, 0, geo.width, geo.height, current_color_);
}

void TestView::OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_down_ = true;
  current_color_ = mouse_down_color_;
  mouse_mouse_down_ = true;
  QueueDraw();
}

void TestView::OnMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_up_ = true;
  if (mouse_in_)
    current_color_ = mouse_in_color_;
  else
    current_color_ = normal_color_;

  mouse_mouse_down_ = false;
  mouse_mouse_drag_ = false;
  QueueDraw();
}

void TestView::OnMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_drag_ = true;
  current_color_ = mouse_drag_color_;
  mouse_mouse_drag_ = true;
  QueueDraw();
}

void TestView::OnMouseClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_click_ = true;
  QueueDraw();
}

void TestView::OnMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_double_click_ = true;
  current_color_ = mouse_down_color_;
  mouse_mouse_down_ = true;
  QueueDraw();
}

void TestView::OnMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_move_ = true;
  QueueDraw();
}

void TestView::OnMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_enter_ = true;
  mouse_in_ = true;
  current_color_ = mouse_in_color_;
  QueueDraw();
}

void TestView::OnMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
  registered_mouse_leave_ = true;
  mouse_in_ = false;
  current_color_ = normal_color_;
  QueueDraw();
}