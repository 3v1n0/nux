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
#include "HScrollBar.h"
#include "VScrollBar.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "ScrollView.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(ScrollView);

  ScrollView::ScrollView (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    m_vertical_scrollbar_enable     = true;
    m_horizontal_scrollbar_enable   = true;
    m_bSizeMatchContent             = false;
    m_TextureIndex                  = 0;
    m_ReformatTexture               = true;

    _delta_x                        = 0;
    _delta_y                        = 0;

    //GetPainter().CreateBackgroundTexture(m_BackgroundTexture);
    hscrollbar = new HScrollBar (NUX_TRACKER_LOCATION);
    vscrollbar = new VScrollBar (NUX_TRACKER_LOCATION);

    SetMinimumSize (30, 30);
    SetGeometry (Geometry (0, 0, 400, 200) );

    hscrollbar->OnScrollLeft.connect ( sigc::mem_fun (this, &ScrollView::ScrollLeft) );
    hscrollbar->OnScrollRight.connect ( sigc::mem_fun (this, &ScrollView::ScrollRight) );
    vscrollbar->OnScrollUp.connect ( sigc::mem_fun (this, &ScrollView::ScrollUp) );
    vscrollbar->OnScrollDown.connect ( sigc::mem_fun (this, &ScrollView::ScrollDown) );

    setTopBorder (4);
    setBorder (4);

    m_ViewContentLeftMargin      = 0;
    m_ViewContentRightMargin     = 0;
    m_ViewContentTopMargin       = 0;
    m_ViewContentBottomMargin    = 0;
    FormatContent();
  }

  ScrollView::~ScrollView()
  {
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
    hscrollbar->Dispose ();
    vscrollbar->Dispose ();
  }

  long ScrollView::ProcessEvent (Event &event, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    long ProcEvInfo = 0;

    
    if (event.e_event == NUX_MOUSE_PRESSED)
    {
      // Verify that the mouse down event falls inside the of the ScrollView.
      if (!GetGeometry ().IsPointInside (event.e_x - event.e_x_root, event.e_y - event.e_y_root))
      {
        ProcEvInfo = eDoNotProcess;
      }
    }

    if (m_vertical_scrollbar_enable)
      ret = vscrollbar->ProcessEvent (event, ret, ProcEvInfo);

    if (m_horizontal_scrollbar_enable)
      ret = hscrollbar->ProcessEvent (event, ret, ProcEvInfo);

    // The child layout get the Mouse down button only if the MouseDown happened inside the client view Area
    Geometry viewGeometry = Geometry (m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight);
    bool traverse = true;

    if ((event.e_event == NUX_MOUSE_PRESSED) ||
     (event.e_event == NUX_MOUSE_RELEASED) ||
     (event.e_event == NUX_MOUSE_MOVE))
    {
      if (!viewGeometry.IsPointInside (event.e_x - event.e_x_root, event.e_y - event.e_y_root) )
      {
        ProcEvInfo = eDoNotProcess;
        traverse = false;
      }
    }

    if (m_CompositionLayout)
    {
      // The ScrollView layout position is fixed. The ScrollView keeps track of the delta offset in x and y of the layout it manages.
      // Modify the event to account for this offset;
      Event mod_event = event;
      ret = m_CompositionLayout->ProcessEvent (mod_event, ret, ProcEvInfo);
    }

    ret = PostProcessEvent2 (event, ret, 0);
    return ret;
  }

  bool ScrollView::SetLayout (Layout *layout)
  {
    if(View::SetLayout(layout) == false)
    {
      return false;
    }

    FormatContent();

    return true;
  }

  void ScrollView::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry ());

    Geometry base = GetGeometry ();

    if (m_CompositionLayout)
      m_CompositionLayout->NeedRedraw();

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

  void ScrollView::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry ());

    GfxContext.PushClippingRectangle (Rect (m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight));

    if (m_CompositionLayout)
    {
//       GfxContext.PushClipOffset (_delta_x, _delta_y);
//       GfxContext.PushClippingRectangle (m_CompositionLayout->GetGeometry ());
//       GfxContext.Push2DTranslationModelViewMatrix (_delta_x, _delta_y, 0.0f);
      m_CompositionLayout->ProcessDraw (GfxContext, force_draw);
//       GfxContext.PopModelViewMatrix ();
//       GfxContext.PopClippingRectangle ();
//       GfxContext.PopClipOffset ();
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

  void ScrollView::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

/////////
// API //
/////////
  void ScrollView::EnableVerticalScrollBar (bool b)
  {
    m_vertical_scrollbar_enable = b;
    _delta_y = 0;
    ComputeChildLayout();
  }

  void ScrollView::EnableHorizontalScrollBar (bool b)
  {
    m_horizontal_scrollbar_enable = b;
    _delta_x = 0;
    ComputeChildLayout();
  }

///////////////////////
// Internal function //
///////////////////////
  void ScrollView::setBorder (int border)
  {
    m_border = border;
  }

  void ScrollView::setTopBorder (int top_border)
  {
    m_top_border = top_border;
  }

  int ScrollView::getBorder() const
  {
    return m_border;
  }

  int ScrollView::getTopBorder() const
  {
    return m_top_border;
  }

  void ScrollView::SetGeometry (const Geometry &geo)
  {
    Area::SetGeometry (geo);
    //ComputeChildLayout();
  }

  void ScrollView::FormatContent()
  {
    Geometry geo;
    geo = GetGeometry();

    ComputeChildLayout();
  }

  void ScrollView::PreLayoutManagement()
  {
    // Give the managed layout the same size and position as the Control.

    Geometry geo = GetGeometry();
    int ScrollBarWidth = vscrollbar->GetBaseWidth();
    int ScrollBarHeight = hscrollbar->GetBaseHeight();

    nuxAssertMsg (ScrollBarWidth > 0, TEXT ("[ScrollView::PreLayoutManagement] Invalid scrollbar width.") );
    nuxAssertMsg (ScrollBarHeight > 0, TEXT ("[ScrollView::PreLayoutManagement] Invalid scrollbar height.") );

    m_ViewX = GetBaseX() + m_border + m_ViewContentLeftMargin;
    m_ViewY = GetBaseY() + m_top_border + m_ViewContentTopMargin;

    if (m_vertical_scrollbar_enable == false)
      m_ViewWidth = GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    else
      m_ViewWidth = GetBaseWidth() - ScrollBarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;

    nuxAssertMsg (m_ViewWidth > 0, TEXT ("[ScrollView::PreLayoutManagement] Invalid view width.") );

    if (m_horizontal_scrollbar_enable == false)
      m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;
    else
      m_ViewHeight = GetBaseHeight() - ScrollBarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;

    nuxAssertMsg (m_ViewWidth > 0, TEXT ("[ScrollView::PreLayoutManagement] Invalid view height.") );

    if (m_ViewX + _delta_x +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute _delta_x so the end of the content match exactly the view right border position
      _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      nuxAssert (_delta_x <= 0);
    }

    if (m_ViewY + _delta_y + m_ViewContentHeight < m_ViewY + m_ViewHeight)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute _delta_y so the end of the content match exactly the view right border position
      _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      nuxAssert (_delta_y <= 0);
    }

    if (m_CompositionLayout)
    {
      // Set the composition layout to the size of the view area and offset it by (_delta_x, _delta_y)

      if (m_CompositionLayout->GetStretchFactor () != 0)
      {
        m_CompositionLayout->SetGeometry (
                m_ViewX,
                m_ViewY,
                m_ViewWidth,
                m_ViewHeight);
      }

      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      if (m_vertical_scrollbar_enable == false)
      {
        // If there is no vertical scrollbar, take all the width available.
        hscrollbar->SetBaseWidth (GetBaseWidth() - 2 * m_border);
      }
      else
        hscrollbar->SetBaseWidth (GetBaseWidth() - ScrollBarWidth - 2 * m_border);

      hscrollbar->SetBaseX (geo.x + m_border);
      hscrollbar->SetBaseY (geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
      hscrollbar->ComputeChildLayout();
    }
    else
    {
      // The horizontal scrollbar won't be visible but give it a proper size anyway.
      hscrollbar->SetBaseWidth (GetBaseWidth() - ScrollBarWidth - 2 * m_border);
      hscrollbar->SetBaseX (geo.x + m_border);
      hscrollbar->SetBaseY (geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
      hscrollbar->ComputeChildLayout();
    }


    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      if (m_horizontal_scrollbar_enable == false)
      {
        // If there is no horizontal scrollbar, take all the width available.
        vscrollbar->SetBaseHeight (GetBaseHeight() - m_top_border - m_border);
      }
      else
        vscrollbar->SetBaseHeight (GetBaseHeight() - ScrollBarHeight - m_top_border - m_border);

      vscrollbar->SetBaseX (geo.x + geo.GetWidth() - ScrollBarWidth - m_border);
      vscrollbar->SetBaseY (geo.y + m_top_border);
      vscrollbar->ComputeChildLayout();
    }
    else
    {
      // The vertical scrollbar won't be visible but give it a proper size anyway.
      vscrollbar->SetBaseHeight (GetBaseHeight() - ScrollBarHeight - m_top_border - m_border);
      vscrollbar->SetBaseX (geo.x + geo.GetWidth() - ScrollBarWidth - m_border);
      vscrollbar->SetBaseY (geo.y + m_top_border);
      vscrollbar->ComputeChildLayout();
    }
  }

  long ScrollView::PostLayoutManagement (long LayoutResult)
  {
    if (IsSizeMatchContent ())
      return PostLayoutManagement2 (LayoutResult);

    int ScrollBarWidth = 0;
    int ScrollBarHeight = 0;

    if (m_CompositionLayout)
    {
      m_ViewContentX = m_CompositionLayout->GetBaseX();
      m_ViewContentY = m_CompositionLayout->GetBaseY();
      m_ViewContentWidth = m_CompositionLayout->GetBaseWidth();
      m_ViewContentHeight = m_CompositionLayout->GetBaseHeight();
    }

    if (m_horizontal_scrollbar_enable)
      ScrollBarHeight = hscrollbar->GetBaseHeight();

    if (m_vertical_scrollbar_enable)
      ScrollBarWidth = vscrollbar->GetBaseWidth();

    hscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                  GetBaseWidth() - ScrollBarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                  GetBaseHeight() - ScrollBarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

    if (m_horizontal_scrollbar_enable)
    {
      if (m_CompositionLayout)
      {
        hscrollbar->SetContentSize (m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                                    m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight() );
      }
      else
      {
        hscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      hscrollbar->SetContentOffset (_delta_x, _delta_y);
    }
    else
    {
      hscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      hscrollbar->SetContentOffset (0, 0);
    }

    vscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                  GetBaseWidth() - ScrollBarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                  GetBaseHeight() - ScrollBarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

    if (m_vertical_scrollbar_enable)
    {
      if (m_CompositionLayout)
      {
        vscrollbar->SetContentSize (m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                                    m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight() );
      }
      else
      {
        vscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      vscrollbar->SetContentOffset (_delta_x, _delta_y);
    }
    else
    {
      vscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      vscrollbar->SetContentOffset (0, 0);
    }

    // I may not be necessary to call this function here since ComputePosition2 was called on ComputePosition2
    // during the layout process.
    if (m_CompositionLayout)
      m_CompositionLayout->ComputePosition2 (0, 0);

    // The ScrollView always returns complient width and height to its parent layout.
    return (eCompliantHeight | eCompliantWidth);
  }

  long ScrollView::PostLayoutManagement2 (long LayoutResult)
  {
    // In case IsSizeMatchContent returns True, The scroll view is resized to match its content.
    int ScrollbarWidth = 0;
    int ScrollbarHeight = 0;

    if (m_horizontal_scrollbar_enable)
      ScrollbarHeight = hscrollbar->GetBaseHeight();

    if (m_vertical_scrollbar_enable)
      ScrollbarWidth = vscrollbar->GetBaseWidth();

    // We want the controller to match the size of the content as defined in:
    //      m_ViewContentX
    //      m_ViewContentY
    //      m_ViewContentWidth
    //      m_ViewContentHeight
    // So we make the composition layout the same size as the content
    // Note that classes that inherits from ScrollView are responsible for setting the dimension of the ViewContent

    if (m_CompositionLayout)
    {
      m_CompositionLayout->SetBaseX (m_ViewContentX);
      m_CompositionLayout->SetBaseY (m_ViewContentY);
      m_CompositionLayout->SetBaseWidth (m_ViewContentWidth);
      m_CompositionLayout->SetBaseHeight (m_ViewContentHeight);
    }

    Geometry base;
    // Given the (m_ViewContentWidth, m_ViewContentHeight) compute the size of the ScrollView.
    // It is possible that the ScrollView size be limited by its min/Max dimension. If this happens, then the scrollbar will reflect that.
    base.SetX (m_ViewContentX - m_border - m_ViewContentLeftMargin);
    base.SetY (m_ViewContentY - m_top_border - m_ViewContentTopMargin);

    if (m_horizontal_scrollbar_enable)
      base.SetHeight (m_top_border + m_ViewContentTopMargin + m_ViewContentHeight + m_ViewContentBottomMargin + ScrollbarHeight + m_border);
    else
      base.SetHeight (m_top_border + m_ViewContentTopMargin + m_ViewContentHeight + m_ViewContentBottomMargin + m_border);

    if (m_vertical_scrollbar_enable)
      base.SetWidth (m_border + m_ViewContentLeftMargin + m_ViewContentWidth + m_ViewContentRightMargin + ScrollbarWidth + m_border);
    else
      base.SetWidth (m_border + m_ViewContentLeftMargin + m_ViewContentWidth + m_ViewContentRightMargin + m_border);

    // Set the size so that is is equal to the visible content.
    Area::SetBaseWidth (base.GetWidth() );
    Area::SetBaseHeight (base.GetHeight() );
    Geometry geo = GetGeometry();

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      if (m_vertical_scrollbar_enable == false)
        hscrollbar->SetBaseWidth (GetBaseWidth() - 2 * m_border);
      else
        hscrollbar->SetBaseWidth (GetBaseWidth() - ScrollbarWidth - 2 * m_border);

      hscrollbar->SetBaseX (geo.x + m_border);
      hscrollbar->SetBaseY (geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
      hscrollbar->ComputeChildLayout();

      //---
      hscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                    GetBaseWidth() - ScrollbarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

      if (m_CompositionLayout)
      {
        hscrollbar->SetContentSize (m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                                    m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight() );
      }
      else
      {
        hscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      hscrollbar->SetContentOffset (_delta_x, _delta_y);
    }
    else
    {
      hscrollbar->SetBaseWidth (GetBaseWidth() - ScrollbarWidth - 2 * m_border);
      hscrollbar->SetBaseX (geo.x + m_border);
      hscrollbar->SetBaseY (geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
      hscrollbar->ComputeChildLayout();

      //---
      hscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                    GetBaseWidth() - ScrollbarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
      hscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      hscrollbar->SetContentOffset (0, 0);
    }


    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      if (m_horizontal_scrollbar_enable == false)
        vscrollbar->SetBaseHeight (GetBaseHeight() - m_top_border - m_border);
      else
        vscrollbar->SetBaseHeight (GetBaseHeight() - ScrollbarHeight - m_top_border - m_border);

      vscrollbar->SetBaseX (geo.x + geo.GetWidth() - ScrollbarWidth - m_border);
      vscrollbar->SetBaseY (geo.y + m_top_border);
      vscrollbar->ComputeChildLayout();

      //---
      vscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                    GetBaseWidth() - ScrollbarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

      if (m_CompositionLayout)
      {
        vscrollbar->SetContentSize (m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                                    m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight() );
      }
      else
      {
        vscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      vscrollbar->SetContentOffset (_delta_x, _delta_y);
    }
    else
    {
      vscrollbar->SetBaseHeight (GetBaseHeight() - ScrollbarHeight - m_top_border - m_border);
      vscrollbar->SetBaseX (geo.x + geo.GetWidth() - ScrollbarWidth - m_border);
      vscrollbar->SetBaseY (geo.y + m_top_border);
      vscrollbar->ComputeChildLayout();

      //---
      vscrollbar->SetContainerSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                    GetBaseWidth() - ScrollbarWidth - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
      vscrollbar->SetContentSize (GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      vscrollbar->SetContentOffset (0, 0);
    }

    if (m_CompositionLayout)
      m_CompositionLayout->ComputePosition2 (0, 0);

    return (eCompliantHeight | eCompliantWidth);
  }

// When the ScrollView is in a Layout object, and that layout call View::PositionChildLayout
// the ScrollView must call its own PositionChildLayout so it can properly do the positioning of the inner object.
// Otherwise, m_CompositionLayout->ComputePosition2 is called but it doesn't know that it may not contain all the
// object of the ScrollView. Which result in incorrect positioning.
// Here we touch only the position. Do not touch the width or height of object.
// This function is called when the ScrollView is embedded within a Layout.
  void ScrollView::PositionChildLayout (float offsetX, float offsetY)
  {
    Geometry geo = GetGeometry();
    int w = 0;
    int h = 0;

    w = vscrollbar->GetBaseWidth();
    h = hscrollbar->GetBaseHeight();

//    m_ViewX = GetX() + m_border;
//    m_ViewY = GetY() + m_top_border;
//
//    if(m_vertical_scrollbar_enable == false)
//        m_ViewWidth = GetWidth() - 2*m_border - m_ViewContentRightMargin;
//    else
//        m_ViewWidth = GetWidth() - w - 2*m_border - m_ViewContentRightMargin;
//
//    if(m_horizontal_scrollbar_enable == false)
//        m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin;
//    else
//        m_ViewHeight = GetBaseHeight() - h - m_top_border - m_border - m_ViewContentBottomMargin;

    m_ViewX = GetBaseX() + m_border + m_ViewContentLeftMargin;
    m_ViewY = GetBaseY() + m_top_border + m_ViewContentTopMargin;

    if (m_vertical_scrollbar_enable == false)
      m_ViewWidth = GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    else
      m_ViewWidth = GetBaseWidth() - w - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;

    if (m_horizontal_scrollbar_enable == false)
      m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;
    else
      m_ViewHeight = GetBaseHeight() - h - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;


    if (m_ViewX + _delta_x +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
    {
      _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
    }

    if (m_ViewY + _delta_y + m_ViewContentHeight < m_ViewY + m_ViewHeight)
    {
      _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
    }

    if (m_CompositionLayout)
    {
      m_CompositionLayout->SetBaseX (m_ViewX);
      m_CompositionLayout->SetBaseY (m_ViewY);
    }

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      if (m_vertical_scrollbar_enable == false)
        hscrollbar->SetBaseWidth (GetBaseWidth() - 2 * m_border);
      else
        hscrollbar->SetBaseWidth (GetBaseWidth() - w - 2 * m_border);

      hscrollbar->SetBaseX (geo.x + m_border);
      hscrollbar->SetBaseY (geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
      hscrollbar->ComputeChildLayout();
    }

    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      if (m_horizontal_scrollbar_enable == false)
        vscrollbar->SetBaseHeight (GetBaseHeight() - m_top_border - m_border);
      else
        vscrollbar->SetBaseHeight (GetBaseHeight() - h - m_top_border - m_border);

      vscrollbar->SetBaseX (geo.x + geo.GetWidth() - w - m_border);
      vscrollbar->SetBaseY (geo.y + m_top_border);
      vscrollbar->ComputeChildLayout();
    }

    if (m_CompositionLayout)
    {
      m_ViewContentX = m_CompositionLayout->GetBaseX();
      m_ViewContentY = m_CompositionLayout->GetBaseY();
    }
    else
    {
      m_ViewContentX = m_ViewX;
      m_ViewContentY = m_ViewY;
    }

    int hor_scrollbar_height = 0;
    int ver_scrollbar_width = 0;

    if (m_horizontal_scrollbar_enable == true)
      hor_scrollbar_height = hscrollbar->GetBaseHeight();

    if (m_vertical_scrollbar_enable == true)
      ver_scrollbar_width = vscrollbar->GetBaseWidth();

    vscrollbar->SetContentOffset (_delta_x, _delta_y);
    hscrollbar->SetContentOffset (_delta_x, _delta_y);

    if (m_CompositionLayout)
    {
      m_CompositionLayout->ComputePosition2 (0, 0);
    }
  }


  void ScrollView::ScrollLeft (float stepx, int mousedx)
  {
    if (m_CompositionLayout)
    {
      _delta_x += (float) stepx * (float) mousedx;;

      if (_delta_x > 0)
      {
        _delta_x = 0;
      }
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    if (m_CompositionLayout)
    {
      hscrollbar->SetContentOffset (_delta_x, _delta_y);
      hscrollbar->NeedRedraw();
    }

    NeedRedraw();
  }

  void ScrollView::ScrollRight (float stepx, int mousedx)
  {
    if (m_CompositionLayout)
    {
      _delta_x -= (float) stepx * (float) mousedx;

      if (m_ViewX + _delta_x +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
      {
        _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      }
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    if (m_CompositionLayout)
    {
      hscrollbar->SetContentOffset (_delta_x, _delta_y);
      hscrollbar->NeedRedraw();
    }

    NeedRedraw();
  }

  void ScrollView::ScrollUp (float stepy, int mousedy)
  {
    if (m_CompositionLayout)
    {
      _delta_y += (float) stepy * (float) mousedy;

      if (_delta_y > 0)
      {
        _delta_y = 0;
      }
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    if (m_CompositionLayout)
    {
      vscrollbar->SetContentOffset (_delta_x, _delta_y);
      vscrollbar->NeedRedraw();
    }

    NeedRedraw();
  }

  void ScrollView::ScrollDown (float stepy, int mousedy)
  {
    if (m_CompositionLayout)
    {
      _delta_y -= (float) stepy * (float) mousedy;

      if (m_ViewY + _delta_y + m_ViewContentHeight < m_ViewY + m_ViewHeight)
      {
        _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      }
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }

    if (m_CompositionLayout)
    {
      vscrollbar->SetContentOffset (_delta_x, _delta_y);
      vscrollbar->NeedRedraw();
    }

    NeedRedraw();
  }

  void ScrollView::SetSizeMatchContent (bool b)
  {
    m_bSizeMatchContent = b;

    if (m_CompositionLayout)
      m_CompositionLayout->ComputeLayout2();
  }

  bool ScrollView::IsSizeMatchContent() const
  {
    return m_bSizeMatchContent;
  }

  void ScrollView::ResetScrollToLeft()
  {
    _delta_x = 0;
    if (m_CompositionLayout)
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);

    hscrollbar->SetContentOffset (_delta_x, _delta_y);
    hscrollbar->NeedRedraw();

    NeedRedraw();
  }

  void ScrollView::ResetScrollToRight()
  {
    if (m_CompositionLayout)
    {
      _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }
    else
    {
      _delta_x = 0;
    }

    

    hscrollbar->SetContentOffset (_delta_x, _delta_y);
    hscrollbar->NeedRedraw();

    NeedRedraw();
  }

  void ScrollView::ResetScrollToUp()
  {
    _delta_y = 0;
    if (m_CompositionLayout)
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);

    vscrollbar->SetContentOffset (_delta_x, _delta_y);
    vscrollbar->NeedRedraw();

    NeedRedraw();
  }

  void ScrollView::ResetScrollToDown()
  {
    if (m_CompositionLayout)
    {
      _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      m_CompositionLayout->Set2DTranslation (_delta_x, _delta_y, 0);
    }
    else
    {
      _delta_y = 0;
    }

    vscrollbar->SetContentOffset (_delta_x, _delta_y);
    vscrollbar->NeedRedraw();

    NeedRedraw();
  }
}
