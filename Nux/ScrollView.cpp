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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
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

  ScrollView::ScrollView (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    m_ContentOffsetX                = 0;
    m_ContentOffsetY                = 0;
    m_vertical_scrollbar_enable     = true;
    m_horizontal_scrollbar_enable   = true;
    m_bSizeMatchContent             = false;
    m_TextureIndex                  = 0;
    m_ReformatTexture               = true;

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
  }

  long ScrollView::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    ret = vscrollbar->ProcessEvent (ievent, ret, ProcessEventInfo);
    ret = hscrollbar->ProcessEvent (ievent, ret, ProcessEventInfo);

    // PostProcessEvent2 must always have its last parameter set to 0
    // because the m_BackgroundArea is the real physical limit of the window.
    // So the previous test about IsPointInside do not prevail over m_BackgroundArea
    // testing the evnt by itself.
    ret = PostProcessEvent2 (ievent, ret, 0);
    return ret;
  }

  void ScrollView::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry() );

    if (m_vertical_scrollbar_enable)
    {
      vscrollbar->Draw (GfxContext, force_draw);
    }

    if (m_horizontal_scrollbar_enable)
    {
      hscrollbar->Draw (GfxContext, force_draw);
    }

    GfxContext.PopClippingRectangle();
  }

/////////
// API //
/////////
  void ScrollView::EnableVerticalScrollBar (bool b)
  {
    m_vertical_scrollbar_enable = b;
    m_ContentOffsetY = 0;
    ComputeChildLayout();
  }

  void ScrollView::EnableHorizontalScrollBar (bool b)
  {
    m_horizontal_scrollbar_enable = b;
    m_ContentOffsetX = 0;
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
//    if(IsSizeMatchContent())
//    {
//        PreLayoutManagement2();
//        return;
//    }

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

    if (m_ViewX + m_ContentOffsetX +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute m_ContentOffsetX so the end of the content match exactly the view right border position
      m_ContentOffsetX = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      nuxAssert (m_ContentOffsetX <= 0);
    }

    if (m_ViewY + m_ContentOffsetY + m_ViewContentHeight < m_ViewY + m_ViewHeight)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute m_ContentOffsetY so the end of the content match exactly the view right border position
      m_ContentOffsetY = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      nuxAssert (m_ContentOffsetY <= 0);
    }

    if (m_CompositionLayout && m_CompositionLayout->GetStretchFactor() != 0)
    {
      // Set the composition layout to the size of the view area and offset it by (m_ContentOffsetX, m_ContentOffsetX)
      m_CompositionLayout->SetGeometry (
        m_ViewX + m_ContentOffsetX,
        m_ViewY + m_ContentOffsetY,
        m_ViewWidth,
        m_ViewHeight);
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

  void ScrollView::PreLayoutManagement2()
  {

  }

  long ScrollView::PostLayoutManagement (long LayoutResult)
  {
    if (IsSizeMatchContent() )
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

      hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
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

      vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
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

      hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
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

      vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
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


    if (m_ViewX + m_ContentOffsetX +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
    {
      m_ContentOffsetX = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
    }

    if (m_ViewY + m_ContentOffsetY + m_ViewContentHeight < m_ViewY + m_ViewHeight)
    {
      m_ContentOffsetY = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
    }

    if (m_CompositionLayout)
    {
      m_CompositionLayout->SetBaseX (m_ViewX + m_ContentOffsetX);
      m_CompositionLayout->SetBaseY (m_ViewY + m_ContentOffsetY);
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

//    hscrollbar->SetContainerSize(GetX() + m_border, GetY() + m_top_border,
//        GetWidth() - ver_scrollbar_width - 2*m_border, GetBaseHeight() - hor_scrollbar_height - m_top_border - m_border);
//    if(m_CompositionLayout)
//        hscrollbar->SetContentSize(m_CompositionLayout->GetX(), m_CompositionLayout->GetY(),
//        m_CompositionLayout->GetWidth(), m_CompositionLayout->GetBaseHeight());

//    vscrollbar->SetContainerSize(GetX() + m_border, GetY() + m_top_border,
//        GetWidth() - ver_scrollbar_width - 2*m_border, GetBaseHeight() - hor_scrollbar_height - m_top_border - m_border);
//    if(m_CompositionLayout)
//        vscrollbar->SetContentSize(m_CompositionLayout->GetX(), m_CompositionLayout->GetY(),
//        m_CompositionLayout->GetWidth(), m_CompositionLayout->GetBaseHeight());

    vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);

    if (m_CompositionLayout)
    {
      m_CompositionLayout->ComputePosition2 (0, 0);
    }
  }


  void ScrollView::ScrollLeft (float stepx, int mousedx)
  {
    if (m_CompositionLayout)
    {
      m_ContentOffsetX += (float) stepx * (float) mousedx;;

      if (m_ContentOffsetX > 0)
      {
        m_ContentOffsetX = 0;
        m_CompositionLayout->SetBaseX (m_ViewX + m_ContentOffsetX);
      }
      else
      {
        m_CompositionLayout->SetBaseX (m_ViewX + m_ContentOffsetX);
      }

      m_CompositionLayout->ComputePosition2 (0, 0);
      hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }

    hscrollbar->NeedRedraw();
    NeedRedraw();
  }

  void ScrollView::ScrollRight (float stepx, int mousedx)
  {
    if (m_CompositionLayout)
    {
      m_ContentOffsetX -= (float) stepx * (float) mousedx;

      if (m_ViewX + m_ContentOffsetX +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
      {
        m_ContentOffsetX = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
        m_CompositionLayout->SetBaseX (m_ViewX + m_ContentOffsetX);
      }
      else
      {
        m_CompositionLayout->SetBaseX (m_ViewX + m_ContentOffsetX);
      }

      m_CompositionLayout->ComputePosition2 (0, 0);
      hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }

    hscrollbar->NeedRedraw();
    NeedRedraw();
  }

  void ScrollView::ScrollUp (float stepy, int mousedy)
  {
    if (m_CompositionLayout)
    {
      m_ContentOffsetY += (float) stepy * (float) mousedy;

      if (m_ContentOffsetY > 0)
      {
        m_ContentOffsetY = 0;
        m_CompositionLayout->SetBaseY (m_ViewY + m_ContentOffsetY);

      }
      else
      {
        m_CompositionLayout->SetBaseY (m_ViewY + m_ContentOffsetY);
      }

      m_CompositionLayout->ComputePosition2 (0, 0);
      //m_CompositionLayout->Translate(m_ContentOffsetX, m_ContentOffsetY);
      vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }

    vscrollbar->NeedRedraw();
    NeedRedraw();
  }

  void ScrollView::ScrollDown (float stepy, int mousedy)
  {
    if (m_CompositionLayout)
    {
      m_ContentOffsetY -= (float) stepy * (float) mousedy;

      if (m_ViewY + m_ContentOffsetY + m_ViewContentHeight < m_ViewY + m_ViewHeight)
      {
        m_ContentOffsetY = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
        m_CompositionLayout->SetBaseY (m_ViewY + m_ContentOffsetY);
      }
      else
      {
        m_CompositionLayout->SetBaseY (m_ViewY + m_ContentOffsetY);
      }

      m_CompositionLayout->ComputePosition2 (0, 0);
      //m_CompositionLayout->Translate(m_ContentOffsetX, m_ContentOffsetY);
      vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }

    vscrollbar->NeedRedraw();
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
    if (m_CompositionLayout)
    {
      m_ContentOffsetX = 0;
      m_CompositionLayout->SetBaseX (m_ViewX + m_ContentOffsetX);
      m_CompositionLayout->ComputePosition2 (0, 0);
      hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }

    hscrollbar->NeedRedraw();
    NeedRedraw();
  }

  void ScrollView::ResetScrollToRight()
  {
    if (m_CompositionLayout)
    {
      m_ContentOffsetX = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      m_CompositionLayout->SetBaseX (m_ViewX + m_ContentOffsetX);
      m_CompositionLayout->ComputePosition2 (0, 0);
      hscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }

    hscrollbar->NeedRedraw();
    NeedRedraw();
  }

  void ScrollView::ResetScrollToUp()
  {
    if (m_CompositionLayout)
    {
      m_ContentOffsetY = 0;
      m_CompositionLayout->SetBaseY (m_ViewY);
      m_CompositionLayout->ComputePosition2 (0, 0);
      vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }

    vscrollbar->NeedRedraw();
    NeedRedraw();
  }

  void ScrollView::ResetScrollToDown()
  {
    if (m_CompositionLayout)
    {
      m_ContentOffsetY = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      m_CompositionLayout->SetBaseY (m_ViewY + m_ContentOffsetY);
      m_CompositionLayout->ComputePosition2 (0, 0);
      vscrollbar->SetContentOffset (m_ContentOffsetX, m_ContentOffsetY);
    }

    vscrollbar->NeedRedraw();
    NeedRedraw();
  }

}
