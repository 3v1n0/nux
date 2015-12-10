// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2010-2011 Inalogic® Inc.
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
#include "HScrollBar.h"
#include "VScrollBar.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "ScrollView.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(ScrollView);

  ScrollView::ScrollView(NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , m_MouseWheelScrollSize(32)
      // TODO: these should really be Rects.
    , m_ViewContentX(0)
    , m_ViewContentY(0)
    , m_ViewContentWidth(0)
    , m_ViewContentHeight(0)
    , m_ViewX(0)
    , m_ViewY(0)
    , m_ViewWidth(0)
    , m_ViewHeight(0)
    , m_TextureIndex(0)
    , m_ReformatTexture(true)
    , m_horizontal_scrollbar_enable(true)
    , m_vertical_scrollbar_enable(true)
    , m_top_border(0)
    , m_border(0)
    , _delta_x(0)
    , _delta_y(0)
    , m_bSizeMatchContent(false)
    , m_ViewContentLeftMargin(0)
    , m_ViewContentRightMargin(0)
    , m_ViewContentTopMargin(0)
    , m_ViewContentBottomMargin(0)
  {

    _hscrollbar = new HScrollBar(NUX_TRACKER_LOCATION);
    _vscrollbar = new VScrollBar(NUX_TRACKER_LOCATION);
    // _hscrollbar and _vscrollbar have to be parented so they are correctly
    // rendered and so that GetRootGeometry/GetAbsoluteGeometry returns the
    // correct Geometry. This is necessary in embedded mode.
    _hscrollbar->SetParentObject(this);
    _vscrollbar->SetParentObject(this);

    _hscrollbar->SetReconfigureParentLayoutOnGeometryChange(false);
    _vscrollbar->SetReconfigureParentLayoutOnGeometryChange(false);

    SetMinimumSize(30, 30);
    SetGeometry(Geometry(0, 0, 400, 200));

    _hscrollbar->OnScrollLeft.connect(sigc::mem_fun(this, &ScrollView::ScrollLeft));
    _hscrollbar->OnScrollRight.connect(sigc::mem_fun(this, &ScrollView::ScrollRight));
    _vscrollbar->OnScrollUp.connect(sigc::mem_fun(this, &ScrollView::ScrollUp));
    _vscrollbar->OnScrollDown.connect(sigc::mem_fun(this, &ScrollView::ScrollDown));

    mouse_wheel.connect(sigc::mem_fun(this, &ScrollView::RecvMouseWheel));
    _vscrollbar->mouse_wheel.connect(sigc::mem_fun(this, &ScrollView::RecvMouseWheel));

    FormatContent();

    SetAcceptMouseWheelEvent(true);
  }

  // customization for Unity
  void ScrollView::SetVScrollBar(VScrollBar* newVScrollBar)
  {
    if (_vscrollbar)
    {
      // disconnect old _vscrollbar
      _vscrollbar->OnScrollUp.connect(sigc::mem_fun(this,
                                                     &ScrollView::ScrollUp));
      _vscrollbar->OnScrollDown.connect(sigc::mem_fun(this,
                                                       &ScrollView::ScrollDown));
      _vscrollbar->mouse_wheel.connect(sigc::mem_fun(this,
                                                     &ScrollView::RecvMouseWheel));
      
      _vscrollbar->UnReference();
    }

    _vscrollbar = newVScrollBar;

    _vscrollbar->SetParentObject(this);
    _vscrollbar->SetReconfigureParentLayoutOnGeometryChange(false);

    // connect new _vscrollbar
    _vscrollbar->OnScrollUp.connect(sigc::mem_fun(this,
                                                   &ScrollView::ScrollUp));
    _vscrollbar->OnScrollDown.connect(sigc::mem_fun(this,
                                                     &ScrollView::ScrollDown));
    _vscrollbar->mouse_wheel.connect(sigc::mem_fun(this,
                                                   &ScrollView::RecvMouseWheel));
  }

  ScrollView::~ScrollView()
  {
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
    _hscrollbar->UnReference();
    _vscrollbar->UnReference();
  }

  Area* ScrollView::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    // Test if the mouse is inside the ScrollView.
    // The last parameter of TestMousePointerInclusion is a boolean used to test if the case
    // of mouse wheel events. If that boolean value is true, then TestMousePointerInclusion
    // returns true only if the mouse pointer is over this area and the the area accepts
    // mouse wheel events(see Area::SetAcceptMouseWheelEvent)
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
    {
      // The mouse pointer is not over this Area. return NULL.
      return NULL;
    }

    Area* found_area;

    // Recursively go over the ui element that are managed by this ScrollView and look
    // for the area that is below the mouse.

    // Test the vertical scrollbar
    if (m_vertical_scrollbar_enable)
    {
      found_area = _vscrollbar->FindAreaUnderMouse(mouse_position, event_type);
      NUX_RETURN_VALUE_IF_NOTNULL(found_area, found_area);
    }

    // Test the horizontal scrollbar
    if (m_horizontal_scrollbar_enable)
    {
      found_area = _hscrollbar->FindAreaUnderMouse(mouse_position, event_type);
      NUX_RETURN_VALUE_IF_NOTNULL(found_area, found_area);
    }

    // If the code gets here, it means that no area has been found yet.
    // Test the layout of the ScrollView
    return View::FindAreaUnderMouse(mouse_position, event_type);
  }

  bool ScrollView::SetLayout(Layout *layout)
  {
    if (View::SetLayout(layout) == false)
    {
      return false;
    }

    FormatContent();

    return true;
  }

  void ScrollView::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    graphics_engine.PushClippingRectangle(GetGeometry());

    Geometry base = GetGeometry();

    GetPainter().PaintBackground(graphics_engine, base);

    if (m_vertical_scrollbar_enable)
    {
      _vscrollbar->QueueDraw();
    }

    if (m_horizontal_scrollbar_enable)
    {
      _hscrollbar->QueueDraw();
    }

    graphics_engine.PopClippingRectangle();
  }

  void ScrollView::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    if (IsFullRedraw())
      GetPainter().PushBackgroundStack();
      
    graphics_engine.PushClippingRectangle(GetGeometry());

    graphics_engine.PushClippingRectangle(Rect(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight));

    if (view_layout_)
      view_layout_->ProcessDraw(graphics_engine, force_draw);

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

    if (IsFullRedraw())
      GetPainter().PopBackgroundStack();
  }

/////////
// API //
/////////
  void ScrollView::EnableVerticalScrollBar(bool b)
  {
    if (b == m_vertical_scrollbar_enable)
      return;

    m_vertical_scrollbar_enable = b;
    ComputeContentSize();
  }

  void ScrollView::EnableHorizontalScrollBar(bool b)
  {
    if (m_horizontal_scrollbar_enable == b)
      return;

    m_horizontal_scrollbar_enable = b;
    ComputeContentSize();
  }

///////////////////////
// Internal function //
///////////////////////
  void ScrollView::FormatContent()
  {
    Geometry geo;
    geo = GetGeometry();

    ComputeContentSize();
  }

  void ScrollView::PreLayoutManagement()
  {
    // Give the managed layout the same size and position as the Control.

    Geometry geo = GetGeometry();
    int ScrollBarWidth = _vscrollbar->GetBaseWidth();
    int ScrollBarHeight = _hscrollbar->GetBaseHeight();

    nuxAssertMsg(ScrollBarWidth > 0, "[ScrollView::PreLayoutManagement] Invalid scrollbar width: %d", ScrollBarWidth);
    nuxAssertMsg(ScrollBarHeight > 0, "[ScrollView::PreLayoutManagement] Invalid scrollbar height: %d", ScrollBarHeight);

    m_ViewX = GetBaseX() + m_border + m_ViewContentLeftMargin;
    m_ViewY = GetBaseY() + m_top_border + m_ViewContentTopMargin;

    m_ViewWidth = GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    nuxAssertMsg(m_ViewWidth > 0, "[ScrollView::PreLayoutManagement] Invalid view width: %d", m_ViewWidth);

    m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;
    nuxAssertMsg(m_ViewHeight > 0, "[ScrollView::PreLayoutManagement] Invalid view height: %d", m_ViewHeight);

    if (m_ViewX + _delta_x +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute _delta_x so the end of the content match exactly the view right border position
      _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      nuxAssert(_delta_x <= 0);
    }

    if (m_ViewY + _delta_y + m_ViewContentHeight < m_ViewY + m_ViewHeight)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute _delta_y so the end of the content match exactly the view right border position
      _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      nuxAssert(_delta_y <= 0);
    }

    if (view_layout_)
    {
      // Set the composition layout to the size of the view area and offset it by(_delta_x, _delta_y)

      if (view_layout_->GetScaleFactor() != 0)
      {
        view_layout_->SetGeometry(
                m_ViewX,
                m_ViewY,
                m_ViewWidth,
                m_ViewHeight);
      }

      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);
    }

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      if (m_vertical_scrollbar_enable == false)
      {
        // If there is no vertical scrollbar, take all the width available.
        _hscrollbar->SetBaseWidth(GetBaseWidth() - 2 * m_border);
      }
      else
        _hscrollbar->SetBaseWidth(GetBaseWidth() - ScrollBarWidth - 2 * m_border);

      _hscrollbar->SetBaseX(geo.x + m_border);
      _hscrollbar->SetBaseY(geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeContentSize();
    }
    else
    {
      // The horizontal scrollbar won't be visible but give it a proper size anyway.
      _hscrollbar->SetBaseWidth(GetBaseWidth() - ScrollBarWidth - 2 * m_border);
      _hscrollbar->SetBaseX(geo.x + m_border);
      _hscrollbar->SetBaseY(geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeContentSize();
    }


    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      if (m_horizontal_scrollbar_enable == false)
      {
        // If there is no horizontal scrollbar, take all the width available.
        _vscrollbar->SetBaseHeight(GetBaseHeight() - m_top_border - m_border);
      }
      else
        _vscrollbar->SetBaseHeight(GetBaseHeight() - ScrollBarHeight - m_top_border - m_border);

      _vscrollbar->SetBaseX(geo.x + geo.GetWidth() - ScrollBarWidth - m_border);
      _vscrollbar->SetBaseY(geo.y + m_top_border);
      _vscrollbar->ComputeContentSize();
    }
    else
    {
      // The vertical scrollbar won't be visible but give it a proper size anyway.
      _vscrollbar->SetBaseHeight(GetBaseHeight() - ScrollBarHeight - m_top_border - m_border);
      _vscrollbar->SetBaseX(geo.x + geo.GetWidth() - ScrollBarWidth - m_border);
      _vscrollbar->SetBaseY(geo.y + m_top_border);
      _vscrollbar->ComputeContentSize();
    }
  }

  long ScrollView::PostLayoutManagement(long LayoutResult)
  {
    if (view_layout_)
    {
      m_ViewContentX = view_layout_->GetBaseX();
      m_ViewContentY = view_layout_->GetBaseY();
      m_ViewContentWidth = view_layout_->GetBaseWidth();
      m_ViewContentHeight = view_layout_->GetBaseHeight();
    }

    _hscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                  GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                  GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

    if (m_horizontal_scrollbar_enable)
    {
      if (view_layout_)
      {
        _hscrollbar->SetContentSize(view_layout_->GetBaseX(), view_layout_->GetBaseY(),
                                    view_layout_->GetBaseWidth(), view_layout_->GetBaseHeight());
      }
      else
      {
        _hscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      _hscrollbar->SetContentOffset(_delta_x, _delta_y);
    }
    else
    {
      _hscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      _hscrollbar->SetContentOffset(0, 0);
    }

    _vscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin,
                                  GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                  GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

    if (m_vertical_scrollbar_enable)
    {
      if (view_layout_)
      {
        _vscrollbar->SetContentSize(view_layout_->GetBaseX(), view_layout_->GetBaseY(),
                                    view_layout_->GetBaseWidth(), view_layout_->GetBaseHeight());
      }
      else
      {
        _vscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
                                    GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      }

      _vscrollbar->SetContentOffset(_delta_x, _delta_y);
    }
    else
    {
      _vscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
                                  GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
      _vscrollbar->SetContentOffset(0, 0);
    }

    // I may not be necessary to call this function here since ComputeContentPosition was called on ComputeContentPosition
    // during the layout process.
    if (view_layout_)
      view_layout_->ComputeContentPosition(0, 0);

    // The ScrollView always returns complient width and height to its parent layout.
    return (eCompliantHeight | eCompliantWidth);
  }

// When the ScrollView is in a Layout object, and that layout call View::ComputeContentPosition
// the ScrollView must call its own ComputeContentPosition so it can properly do the positioning of the inner object.
// Otherwise, view_layout_->ComputeContentPosition is called but it doesn't know that it may not contain all the
// object of the ScrollView. Which result in incorrect positioning.
// Here we touch only the position. Do not touch the width or height of object.
// This function is called when the ScrollView is embedded within a Layout.
  void ScrollView::ComputeContentPosition(float /* offsetX */, float /* offsetY */)
  {
    Geometry geo = GetGeometry();
    int w = 0;
    int h = 0;

    w = _vscrollbar->GetBaseWidth();
    h = _hscrollbar->GetBaseHeight();

    m_ViewX = GetBaseX() + m_border + m_ViewContentLeftMargin;
    m_ViewY = GetBaseY() + m_top_border + m_ViewContentTopMargin;

    m_ViewWidth = GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;

    if (m_ViewX + _delta_x +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
    {
      _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
    }

    if (m_ViewY + _delta_y + m_ViewContentHeight < m_ViewY + m_ViewHeight)
    {
      _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
    }

    if (view_layout_)
    {
      view_layout_->SetBaseX(m_ViewX);
      view_layout_->SetBaseY(m_ViewY);
    }

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      if (m_vertical_scrollbar_enable == false)
        _hscrollbar->SetBaseWidth(GetBaseWidth() - 2 * m_border);
      else
        _hscrollbar->SetBaseWidth(GetBaseWidth() - w - 2 * m_border);

      _hscrollbar->SetBaseX(geo.x + m_border);
      _hscrollbar->SetBaseY(geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeContentSize();
    }

    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      if (m_horizontal_scrollbar_enable == false)
        _vscrollbar->SetBaseHeight(GetBaseHeight() - m_top_border - m_border);
      else
        _vscrollbar->SetBaseHeight(GetBaseHeight() - h - m_top_border - m_border);

      _vscrollbar->SetBaseX(geo.x + geo.GetWidth() - w - m_border);
      _vscrollbar->SetBaseY(geo.y + m_top_border);
      _vscrollbar->ComputeContentSize();
    }

    if (view_layout_)
    {
      m_ViewContentX = view_layout_->GetBaseX();
      m_ViewContentY = view_layout_->GetBaseY();
    }
    else
    {
      m_ViewContentX = m_ViewX;
      m_ViewContentY = m_ViewY;
    }

    _vscrollbar->SetContentOffset(_delta_x, _delta_y);
    _hscrollbar->SetContentOffset(_delta_x, _delta_y);

    if (view_layout_)
    {
      view_layout_->ComputeContentPosition(0, 0);
    }
  }


  void ScrollView::ScrollLeft(float stepx, int mousedx)
  {
    if (view_layout_)
    {
      _delta_x += (float) stepx * (float) mousedx;;

      if (_delta_x > 0)
      {
        _delta_x = 0;
      }
      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);
      scrolling.emit(_delta_x, _delta_y);
    }

    if (view_layout_)
    {
      _hscrollbar->SetContentOffset(_delta_x, _delta_y);
      _hscrollbar->QueueDraw();
    }

    QueueDraw();
  }

  void ScrollView::ScrollRight(float stepx, int mousedx)
  {
    if (view_layout_)
    {
      _delta_x -= (float) stepx * (float) mousedx;

      if (m_ViewX + _delta_x +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
      {
        _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      }
      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);
      scrolling.emit(_delta_x, _delta_y);
    }

    if (view_layout_)
    {
      _hscrollbar->SetContentOffset(_delta_x, _delta_y);
      _hscrollbar->QueueDraw();
    }

    QueueDraw();
  }

  void ScrollView::ScrollUp(float stepy, int mousedy)
  {
    if (m_ViewContentHeight <= m_ViewHeight)
      return;

    if (view_layout_)
    {
      int last_delta_y = _delta_y;
      _delta_y += stepy * mousedy;

      if (_delta_y > 0)
      {
        _delta_y = 0;
      }

      if (last_delta_y != _delta_y)
      {
        QueueDraw();
        _vscrollbar->QueueDraw();
      }

      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);
      _vscrollbar->SetContentOffset(_delta_x, _delta_y);

      scrolling.emit(_delta_x, _delta_y);
    }
  }

  void ScrollView::ScrollDown(float stepy, int mousedy)
  {
    if (m_ViewContentHeight <= m_ViewHeight)
      return;

    if (view_layout_)
    {
      int last_delta_y = _delta_y;
      _delta_y -= stepy * mousedy;

      if (m_ViewY + _delta_y + m_ViewContentHeight < m_ViewY + m_ViewHeight)
      {
        _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      }

      if (last_delta_y != _delta_y)
      {
        QueueDraw();
        _vscrollbar->QueueDraw();
      }

      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);
     _vscrollbar->SetContentOffset(_delta_x, _delta_y);

      scrolling.emit(_delta_x, _delta_y);
    }
  }

  void ScrollView::ResetScrollToLeft()
  {
    _delta_x = 0;
    if (view_layout_)
      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);

    _hscrollbar->SetContentOffset(_delta_x, _delta_y);
    _hscrollbar->QueueDraw();

    QueueDraw();
  }

  void ScrollView::ResetScrollToRight()
  {
    if (view_layout_)
    {
      _delta_x = - (m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);
    }
    else
    {
      _delta_x = 0;
    }



    _hscrollbar->SetContentOffset(_delta_x, _delta_y);
    _hscrollbar->QueueDraw();

    QueueDraw();
  }

  void ScrollView::ResetScrollToUp()
  {
    _delta_y = 0;
    if (view_layout_)
      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);

    _vscrollbar->SetContentOffset(_delta_x, _delta_y);
    _vscrollbar->QueueDraw();

    QueueDraw();
  }

  void ScrollView::ResetScrollToDown()
  {
    if (view_layout_)
    {
      _delta_y = - (m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);
    }
    else
    {
      _delta_y = 0;
    }

    _vscrollbar->SetContentOffset(_delta_x, _delta_y);
    _vscrollbar->QueueDraw();

    QueueDraw();
  }

  void ScrollView::RecvMouseWheel(int /* x */, int /* y */, int wheel_delta, long /* button_flags */, unsigned long /* key_flags */)
  {
    // nux can't tell the difference between horizontal and vertical mouse wheel events
    // so we are only going to support vertical
    if (abs(wheel_delta) == NUX_MOUSEWHEEL_DELTA)
    {
      // Vertical Scrolling
      if (wheel_delta < 0)
      {
        ScrollDown(1, m_MouseWheelScrollSize);
      }
      else
      {
        ScrollUp(1, m_MouseWheelScrollSize);
      }
    }
    else if (abs(wheel_delta) == (NUX_MOUSEWHEEL_DELTA ^ 2))
    {
      // Horizontal Scrolling
      if (wheel_delta < 0)
      {
        ScrollRight(1, m_MouseWheelScrollSize);
      }
      else
      {
        ScrollLeft(1, m_MouseWheelScrollSize);
      }
    }
    
  }

  bool ScrollView::AcceptKeyNavFocus()
  {
    return false;
  }
}
