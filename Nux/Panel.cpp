/*
 * Copyright 2010 Inalogic® Inc.
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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "Layout.h"
#include "WindowCompositor.h"
#include "VScrollBar.h"
#include "HScrollBar.h"
#include "Panel.h"

namespace nux
{

  Panel::Panel(NUX_FILE_LINE_DECL)
    :   ScrollView(NUX_FILE_LINE_PARAM)
    ,   m_layout(0)
  {
    m_top_border = 0;
    m_border = 0;
  }

  Panel::~Panel()
  {
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
    //delete vlayout;
    m_layout = NULL;
  }

  void Panel::Draw(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {

  }

  void Panel::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    graphics_engine.PushClippingRectangle(GetGeometry());

    graphics_engine.PushClippingRectangle(Rect(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight));

    if (m_layout)
    {
      graphics_engine.PushClippingRectangle(m_layout->GetGeometry());
      m_layout->ProcessDraw(graphics_engine, force_draw);
      graphics_engine.PopClippingRectangle();
    }

    graphics_engine.PopClippingRectangle();

    if (m_vertical_scrollbar_enable)
    {
      _vscrollbar->ProcessDraw(graphics_engine, force_draw);
    }

    if (m_horizontal_scrollbar_enable)
    {
      _hscrollbar->ProcessDraw(graphics_engine, force_draw);
    }

    graphics_engine.PopClippingRectangle();
  }

  void Panel::AddWidget(View *ic, int stretchfactor)
  {
    if (ic && m_layout)
    {
      m_layout->AddView(ic, stretchfactor);
      // if (stretchfactor ==0): the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
      // and the children will take their natural size by expending WidgetLayout.
      // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.

      ComputeContentSize();
    }
  }

  void Panel::AddWidget(std::list<View *> *ViewList)
  {
    std::list<View *>::iterator it;

    for (it = ViewList->begin(); it != ViewList->end(); it++)
    {
      AddWidget((*it));
    }
  }

  bool Panel::SetLayout(Layout *layout)
  {
    if (View::SetLayout(layout) == false)
    {
      return false;
    }

    m_layout = view_layout_;
    
    FormatContent();

    return true;
  }

  void Panel::clearContent()
  {
    m_layout->Clear();
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  void Panel::PreLayoutManagement()
  {
    ScrollView::PreLayoutManagement();
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  long Panel::PostLayoutManagement(long LayoutResult)
  {
    long result = ScrollView::PostLayoutManagement(LayoutResult);
    return result;
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  void Panel::ComputeContentPosition(float offsetX, float offsetY)
  {
    ScrollView::ComputeContentPosition(offsetX, offsetY);
  }

  void Panel::ScrollLeft(float stepx, int mousedx)
  {
    ScrollView::ScrollLeft(stepx, mousedx);
    ComputeContentSize();
    QueueDraw();
  }

  void Panel::ScrollRight(float stepx, int mousedx)
  {
    ScrollView::ScrollRight(stepx, mousedx);
    ComputeContentSize();
    QueueDraw();
  }

  void Panel::ScrollUp(float stepy, int mousedy)
  {
    ScrollView::ScrollUp(stepy, mousedy);
    ComputeContentSize();
    QueueDraw();
  }

  void Panel::ScrollDown(float stepy, int mousedy)
  {
    ScrollView::ScrollDown(stepy, mousedy);
    ComputeContentSize();
    QueueDraw();
  }

  bool Panel::AcceptKeyNavFocus()
  {
    return false;
  }
}
