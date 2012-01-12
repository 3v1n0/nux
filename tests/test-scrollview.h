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
 
#ifndef TEST_SCROLLVIEW_H
#define TEST_SCROLLVIEW_H

#include "Nux/ScrollView.h"
#include "Nux/VScrollBar.h"

class TestScrollView: public nux::ScrollView
{
  NUX_DECLARE_OBJECT_TYPE(TestView, ScrollView);
public:
  TestScrollView(NUX_FILE_LINE_PROTO);
  ~TestScrollView();

  nux::Color GetColor() const;
  void ResetEvents();
  nux::VScrollBar* GetVScrollbar() {return _vscrollbar;}
  
  bool registered_scroll_;

protected:
  void Draw(nux::GraphicsEngine &graphics_engine, bool force_draw);
  void OnScrolling(int hscroll, int vscroll);
  
private:
  nux::Color normal_color_;
  nux::Color current_color_;

};

#endif // TEST_VIEW_H

