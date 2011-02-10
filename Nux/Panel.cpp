/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
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

  Panel::Panel (NUX_FILE_LINE_DECL)
    :   ScrollView (NUX_FILE_LINE_PARAM)
    ,   m_layout (0)
  {
    //vlayout = new VLayout();
    //vlayout->SetVerticalInternalMargin(4);

    setTopBorder (0);
    setBorder (0);

    //SetMinimumSize(200, 200);

    //SetCompositionLayout(vlayout);
    //FormatContent();
  }

  Panel::~Panel()
  {
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
    //delete vlayout;
    m_layout = NULL;
  }

  long Panel::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    long ProcEvInfo = 0;

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      if (!m_Geometry.IsPointInside (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root) )
      {
        ProcEvInfo = eDoNotProcess;
        //return TraverseInfo;
      }
    }

    if (m_vertical_scrollbar_enable)
      ret = vscrollbar->ProcessEvent (ievent, ret, ProcEvInfo);

    if (m_horizontal_scrollbar_enable)
      ret = hscrollbar->ProcessEvent (ievent, ret, ProcEvInfo);

    // The child layout get the Mouse down button only if the MouseDown happened inside the client view Area
    Geometry viewGeometry = Geometry (m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight);
    bool traverse = true;

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      if (!viewGeometry.IsPointInside (ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root) )
      {
        ProcEvInfo = eDoNotProcess;
        traverse = false;
      }
    }

    if (m_layout)
      ret = m_layout->ProcessEvent (ievent, ret, ProcEvInfo);

    ret = PostProcessEvent2 (ievent, ret, 0);
    return ret;
  }

  void Panel::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry() );

    Geometry base = GetGeometry();

    if (m_layout)
      m_layout->NeedRedraw();

    GetPainter().PaintBackground (GfxContext, base);

    if (m_vertical_scrollbar_enable)
    {
      vscrollbar->NeedRedraw();
    }

    if (m_horizontal_scrollbar_enable)
    {
      hscrollbar->NeedRedraw();
    }

    GfxContext.PopClippingRectangle();
  }

  void Panel::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry() );

    GfxContext.PushClippingRectangle (Rect (m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight) );

    if (m_layout)
    {
      GfxContext.PushClippingRectangle (m_layout->GetGeometry() );
      m_layout->ProcessDraw (GfxContext, force_draw);
      GfxContext.PopClippingRectangle();
    }

    GfxContext.PopClippingRectangle();

    if (m_vertical_scrollbar_enable)
    {
      vscrollbar->ProcessDraw (GfxContext, force_draw);
    }

    if (m_horizontal_scrollbar_enable)
    {
      hscrollbar->ProcessDraw (GfxContext, force_draw);
    }

    GfxContext.PopClippingRectangle();
  }

  void Panel::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void Panel::AddWidget (View *ic, int stretchfactor)
  {
    if (ic && m_layout)
    {
      m_layout->AddView (ic, stretchfactor);
      // if(stretchfactor ==0): the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
      // and the children will take their natural size by expending WidgetLayout.
      // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.

      ComputeChildLayout();
    }
  }

  void Panel::AddWidget (std::list<View *> *ViewList)
  {
    std::list<View *>::iterator it;

    for (it = ViewList->begin(); it != ViewList->end(); it++)
    {
      AddWidget ( (*it) );
    }
  }

  bool Panel::SetLayout (Layout *layout)
  {
    if(View::SetLayout(layout) == false)
    {
      return false;
    }

    m_layout = layout;
    
    SetCompositionLayout (m_layout);

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
  long Panel::PostLayoutManagement (long LayoutResult)
  {
    long result = ScrollView::PostLayoutManagement (LayoutResult);
    return result;
  }

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
  void Panel::PositionChildLayout (float offsetX, float offsetY)
  {
    ScrollView::PositionChildLayout (offsetX, offsetY);
  }

  void Panel::ScrollLeft (float stepx, int mousedx)
  {
    ScrollView::ScrollLeft (stepx, mousedx);
    ComputeChildLayout();
    NeedRedraw();
  }

  void Panel::ScrollRight (float stepx, int mousedx)
  {
    ScrollView::ScrollRight (stepx, mousedx);
    ComputeChildLayout();
    NeedRedraw();
  }

  void Panel::ScrollUp (float stepy, int mousedy)
  {
    ScrollView::ScrollUp (stepy, mousedy);
    ComputeChildLayout();
    NeedRedraw();
  }

  void Panel::ScrollDown (float stepy, int mousedy)
  {
    ScrollView::ScrollDown (stepy, mousedy);
    ComputeChildLayout();
    NeedRedraw();
  }

}
