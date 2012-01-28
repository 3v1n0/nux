
#ifndef TEST_VIEW_H
#define TEST_VIEW_H

#include "Nux/TextureArea.h"

class TestView: public nux::View
{
  NUX_DECLARE_OBJECT_TYPE(TestView, View);
public:
  TestView(NUX_FILE_LINE_PROTO);
  ~TestView();

  nux::Color GetColor() const;
  bool AcceptKeyNavFocus() {return can_focus_;}

  void ResetEvents();

  bool can_focus_;

  bool has_focus_;
  bool registered_mouse_down_;
  bool registered_mouse_up_;
  bool registered_mouse_drag_;
  bool registered_mouse_click_;
  bool registered_mouse_double_click_;
  bool registered_mouse_move_;
  bool registered_mouse_enter_;
  bool registered_mouse_leave_;

  nux::KeyNavDirection key_nav_direction_; //!< The key nav direction received when the view obtained the focus.
protected:
  nux::Color current_color_;
  nux::Color normal_color_;
  nux::Color mouse_down_color_;
  nux::Color mouse_drag_color_;
  nux::Color mouse_in_color_;
  nux::Color with_key_focus_color_;
  nux::Color without_key_focus_color_;
  
  bool mouse_in_;
  bool mouse_mouse_drag_;
  bool mouse_mouse_down_;

  void OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
  void OnMouseClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
  void OnMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
  void OnKeyNavFocusChange(nux::Area* area, bool has_focus, nux::KeyNavDirection direction);

  void Draw(nux::GraphicsEngine &graphics_engine, bool force_draw);
};

#endif // TEST_VIEW_H

