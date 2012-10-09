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
#include "ScrollView.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(ScrollView);

  ScrollView::ScrollView(NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , m_MouseWheelScrollSize(32)
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
    SetAcceptMouseWheelEvent(true);

    SetHScrollBar(new HScrollBar(NUX_TRACKER_LOCATION));
    SetVScrollBar(new VScrollBar(NUX_TRACKER_LOCATION));

    FormatContent();

    mouse_wheel.connect(sigc::mem_fun(this, &ScrollView::RecvMouseWheel));
  }

  ScrollView::~ScrollView()
  {
    scroll_up_connection_.disconnect();
    scroll_down_connection_.disconnect();
    vmouse_whell_connection_.disconnect();
    scroll_left_connection_.disconnect();
    scroll_right_connection_.disconnect();
    hmouse_whell_connection_.disconnect();
  }

  void ScrollView::SetVScrollBar(VScrollBar* vscrollbar)
  {
    if (!vscrollbar)
      return;

    if (_vscrollbar)
    {
      scroll_up_connection_.disconnect();
      scroll_down_connection_.disconnect();
      vmouse_whell_connection_.disconnect();
      
    }

    _vscrollbar = vscrollbar;

    _vscrollbar->SetParentObject(this);
    _vscrollbar->SetReconfigureParentLayoutOnGeometryChange(false);

    scroll_up_connection_ = _vscrollbar->OnScrollUp.connect(sigc::mem_fun(this, &ScrollView::ScrollUp));
    scroll_down_connection_ = _vscrollbar->OnScrollDown.connect(sigc::mem_fun(this, &ScrollView::ScrollDown));
    vmouse_whell_connection_ = _vscrollbar->mouse_wheel.connect(sigc::mem_fun(this, &ScrollView::RecvMouseWheel));
  }

  void ScrollView::SetHScrollBar(HScrollBar* hscrollbar)
  {
    if (!hscrollbar)
      return;

    if (_hscrollbar)
    {
      scroll_left_connection_.disconnect();
      scroll_right_connection_.disconnect();
      hmouse_whell_connection_.disconnect();
    }

    _hscrollbar = hscrollbar;

    _hscrollbar->SetParentObject(this);
    _hscrollbar->SetReconfigureParentLayoutOnGeometryChange(false);

    scroll_left_connection_ = _hscrollbar->OnScrollLeft.connect(sigc::mem_fun(this, &ScrollView::ScrollLeft));
    scroll_right_connection_ = _hscrollbar->OnScrollRight.connect(sigc::mem_fun(this, &ScrollView::ScrollRight));
    hmouse_whell_connection_ = _hscrollbar->mouse_wheel.connect(sigc::mem_fun(this, &ScrollView::RecvMouseWheel));
  }

  Area* ScrollView::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (!mouse_inside)
      return nullptr;

    if (m_vertical_scrollbar_enable)
    {
      Area* found_area = _vscrollbar->FindAreaUnderMouse(mouse_position, event_type);
      NUX_RETURN_VALUE_IF_NOTNULL(found_area, found_area);
    }

    if (m_horizontal_scrollbar_enable)
    {
      Area* found_area = _hscrollbar->FindAreaUnderMouse(mouse_position, event_type);
      NUX_RETURN_VALUE_IF_NOTNULL(found_area, found_area);
    }

    return View::FindAreaUnderMouse(mouse_position, event_type);
  }

  void ScrollView::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry const& base = GetGeometry();

    graphics_engine.PushClippingRectangle(base);

    if (view_layout_)
      view_layout_->QueueDraw();

    GetPainter().PaintBackground(graphics_engine, base);

    if (m_vertical_scrollbar_enable)
      _vscrollbar->QueueDraw();

    if (m_horizontal_scrollbar_enable)
      _hscrollbar->QueueDraw();

    graphics_engine.PopClippingRectangle();
  }

  void ScrollView::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    if (IsFullRedraw())
      GetPainter().PushBackgroundStack();
      
    graphics_engine.PushClippingRectangle(GetGeometry());

    graphics_engine.PushClippingRectangle(view_geo_);

    if (view_layout_)
      view_layout_->ProcessDraw(graphics_engine, force_draw);

    graphics_engine.PopClippingRectangle();

    if (m_vertical_scrollbar_enable)
      _vscrollbar->ProcessDraw(graphics_engine, force_draw);

    if (m_horizontal_scrollbar_enable)
      _hscrollbar->ProcessDraw(graphics_engine, force_draw);

    graphics_engine.PopClippingRectangle();

    if (IsFullRedraw())
      GetPainter().PopBackgroundStack();
  }

  void ScrollView::EnableVerticalScrollBar(bool b)
  {
    m_vertical_scrollbar_enable = b;
    ComputeContentSize();
  }

  void ScrollView::EnableHorizontalScrollBar(bool b)
  {
    m_horizontal_scrollbar_enable = b;
    ComputeContentSize();
  }

  void ScrollView::FormatContent()
  {
    ComputeContentSize();
  }

  void ScrollView::PreLayoutManagement()
  {
    // Give the managed layout the same size and position as the Control.

    Geometry const& geo = GetGeometry();

    view_geo_.x = GetBaseX() + m_border + m_ViewContentLeftMargin;
    view_geo_.y = GetBaseY() + m_top_border + m_ViewContentTopMargin;

    view_geo_.width = GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    nuxAssertMsg(view_geo_.width > 0, "[ScrollView::PreLayoutManagement] Invalid view width: %d", view_geo_.width);

    view_geo_.height = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;
    nuxAssertMsg(view_geo_.height > 0, "[ScrollView::PreLayoutManagement] Invalid view height: %d", view_geo_.height);

    if (_delta_x +  content_geo_.width < view_geo_.width)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute _delta_x so the end of the content match exactly the view right border position
      _delta_x = - (content_geo_.width > view_geo_.width ? content_geo_.width - view_geo_.width : 0);
      nuxAssert(_delta_x <= 0);
    }

    if (_delta_y + content_geo_.height < view_geo_.height)
    {
      // The position of the end of the content is smaller than the view right border position
      // Compute _delta_y so the end of the content match exactly the view right border position
      _delta_y = - (content_geo_.height > view_geo_.height ? content_geo_.height - view_geo_.height : 0);
      nuxAssert(_delta_y <= 0);
    }

    if (view_layout_)
    {
      // Set the composition layout to the size of the view area and offset it by(_delta_x, _delta_y)
      if (view_layout_->GetScaleFactor() != 0)
        view_layout_->SetGeometry(view_geo_);

      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);
    }

    if (m_horizontal_scrollbar_enable)
    {
      _hscrollbar->SetBaseWidth(GetBaseWidth() - 2 * m_border);
      _hscrollbar->SetBaseX(geo.x + m_border);
      _hscrollbar->SetBaseY(geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeContentSize();
    }

    if (m_vertical_scrollbar_enable)
    {
      _vscrollbar->SetBaseHeight(GetBaseHeight() - m_top_border - m_border);
      _vscrollbar->SetBaseX(geo.x + geo.GetWidth() - _vscrollbar->GetBaseWidth() - m_border);
      _vscrollbar->SetBaseY(geo.y + m_top_border);
      _vscrollbar->ComputeContentSize();
    }
  }

  long ScrollView::PostLayoutManagement(long LayoutResult)
  {
    if (IsSizeMatchContent())
      return PostLayoutManagement2(LayoutResult);

    if (view_layout_)
      content_geo_ = view_layout_->GetGeometry();

    if (m_horizontal_scrollbar_enable)
    {
      _hscrollbar->SetContainerSize(GetBaseWidth() -  2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
                                  
      if (view_layout_)
        _hscrollbar->SetContentSize( view_layout_->GetBaseWidth(), view_layout_->GetBaseHeight());
      else
        _hscrollbar->SetContentSize(0, 0);

      _hscrollbar->SetContentOffset(_delta_x, _delta_y);
    }
    else
    {
      _hscrollbar->SetContentSize(0, 0);
      _hscrollbar->SetContentOffset(0, 0);
    }

    if (m_vertical_scrollbar_enable)
    {
      _vscrollbar->SetContainerSize(GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
  
      if (view_layout_)
        _vscrollbar->SetContentSize(view_layout_->GetBaseWidth(), view_layout_->GetBaseHeight());
      else
        _vscrollbar->SetContentSize(0, 0);

      _vscrollbar->SetContentOffset(_delta_x, _delta_y);
    }
    else
    {
      _vscrollbar->SetContentSize(0, 0);
      _vscrollbar->SetContentOffset(0, 0);
    }

    // The ScrollView always returns complient width and height to its parent layout.
    return (eCompliantHeight | eCompliantWidth);
  }

  long ScrollView::PostLayoutManagement2(long /* LayoutResult */)
  {
    // In case IsSizeMatchContent returns True, The scroll view is resized to match its content.

    // We want the controller to match the size of the content as defined in content_geo_
    // So we make the composition layout the same size as the content
    // Note that classes that inherits from ScrollView are responsible for setting the dimension of the ViewContent

    if (view_layout_)
      view_layout_->SetGeometry(content_geo_);

    Geometry base;
    // Given the(content_geo_.width, content_geo_.height) compute the size of the ScrollView.
    // It is possible that the ScrollView size be limited by its min/Max dimension. If this happens, then the scrollbar will reflect that.
    base.SetX(content_geo_.x - m_border - m_ViewContentLeftMargin);
    base.SetY(content_geo_.y - m_top_border - m_ViewContentTopMargin);

    base.SetHeight(m_top_border + m_ViewContentTopMargin + content_geo_.height + m_ViewContentBottomMargin + m_border);
    base.SetWidth(m_border + m_ViewContentLeftMargin + content_geo_.width + m_ViewContentRightMargin + m_border);

    // Set the size so that is is equal to the visible content.
    Area::SetBaseWidth(base.GetWidth());
    Area::SetBaseHeight(base.GetHeight());
    Geometry const& geo = GetGeometry();

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      _hscrollbar->SetBaseWidth(GetBaseWidth() - 2 * m_border);      
      _hscrollbar->SetBaseX(geo.x + m_border);
      _hscrollbar->SetBaseY(geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeContentSize();

      _hscrollbar->SetContainerSize(GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

      if (view_layout_)
        _hscrollbar->SetContentSize(view_layout_->GetBaseWidth(), view_layout_->GetBaseHeight());
      else
        _hscrollbar->SetContentSize(0, 0);

      _hscrollbar->SetContentOffset(_delta_x, _delta_y);
    }

    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      _vscrollbar->SetBaseHeight(GetBaseHeight() - m_top_border - m_border);
      _vscrollbar->SetBaseX(geo.x + geo.GetWidth() - _vscrollbar->GetBaseWidth() - m_border);
      _vscrollbar->SetBaseY(geo.y + m_top_border);
      _vscrollbar->ComputeContentSize();

      _vscrollbar->SetContainerSize(GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
                                    GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);

      if (view_layout_)
        _vscrollbar->SetContentSize(view_layout_->GetBaseWidth(), view_layout_->GetBaseHeight());
      else
        _vscrollbar->SetContentSize(0, 0);

      _vscrollbar->SetContentOffset(_delta_x, _delta_y);
    }

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
    Geometry const& geo = GetGeometry();

    view_geo_.x = GetBaseX() + m_border + m_ViewContentLeftMargin;
    view_geo_.y = GetBaseY() + m_top_border + m_ViewContentTopMargin;
    view_geo_.width = GetBaseWidth() - 2 * m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    view_geo_.height = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;

    if (_delta_x +  content_geo_.width < view_geo_.width)
      _delta_x = - (content_geo_.width > view_geo_.width ? content_geo_.width - view_geo_.width : 0);

    if (_delta_y + content_geo_.height < view_geo_.height)
      _delta_y = - (content_geo_.height > view_geo_.height ? content_geo_.height - view_geo_.height : 0);

    if (view_layout_)
    {
      view_layout_->SetBaseX(view_geo_.x);
      view_layout_->SetBaseY(view_geo_.y);
    }

    // Horizontal scrollbar Geometry
    if (m_horizontal_scrollbar_enable)
    {
      _hscrollbar->SetBaseWidth(GetBaseWidth() - 2 * m_border);
      _hscrollbar->SetBaseX(geo.x + m_border);
      _hscrollbar->SetBaseY(geo.y + geo.GetHeight() - _hscrollbar->GetBaseHeight() - m_border);
      _hscrollbar->ComputeContentSize();
    }

    // Vertical scrollbar Geometry
    if (m_vertical_scrollbar_enable)
    {
      _vscrollbar->SetBaseHeight(GetBaseHeight() - m_top_border - m_border);
      _vscrollbar->SetBaseX(geo.x + geo.GetWidth() - _vscrollbar->GetBaseWidth() - m_border);
      _vscrollbar->SetBaseY(geo.y + m_top_border);
      _vscrollbar->ComputeContentSize();
    }

    if (view_layout_)
    {
      content_geo_.x = view_layout_->GetBaseX();
      content_geo_.y = view_layout_->GetBaseY();
    }
    else
    {
      content_geo_.x = view_geo_.x;
      content_geo_.y = view_geo_.y;
    }

    _vscrollbar->SetContentOffset(_delta_x, _delta_y);
    _hscrollbar->SetContentOffset(_delta_x, _delta_y);

    if (view_layout_)
      view_layout_->ComputeContentPosition(0, 0);
  }


  void ScrollView::ScrollLeft(float stepx, int mousedx)
  {
    if (view_layout_)
    {
      _delta_x += stepx * mousedx;;

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
      _delta_x -= stepx * mousedx;

      if (_delta_x +  content_geo_.width < view_geo_.width)
      {
        _delta_x = - (content_geo_.width > view_geo_.width ? content_geo_.width - view_geo_.width : 0);
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
    if (content_geo_.height <= view_geo_.height)
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
    if (!view_layout_ || content_geo_.height <= view_geo_.height)
      return;

    int last_delta_y = _delta_y;
    _delta_y -= stepy * mousedy;

    if (_delta_y + content_geo_.height < view_geo_.height)
      _delta_y = - (content_geo_.height > view_geo_.height ? content_geo_.height - view_geo_.height : 0);

    if (last_delta_y != _delta_y)
    {
      view_layout_->Set2DTranslation(_delta_x, _delta_y, 0);
     _vscrollbar->SetContentOffset(_delta_x, _delta_y);

      QueueDraw();
      _vscrollbar->QueueDraw();

      scrolling.emit(_delta_x, _delta_y);
    }
  }

  void ScrollView::SetSizeMatchContent(bool b)
  {
    m_bSizeMatchContent = b;

    if (view_layout_)
      view_layout_->ComputeContentSize();
  }

  bool ScrollView::IsSizeMatchContent() const
  {
    return m_bSizeMatchContent;
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
      _delta_x = - (content_geo_.width > view_geo_.width ? content_geo_.width - view_geo_.width : 0);
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
      _delta_y = - (content_geo_.height > view_geo_.height ? content_geo_.height - view_geo_.height : 0);
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
    if (wheel_delta < 0)
      ScrollDown(abs(wheel_delta / NUX_MOUSEWHEEL_DELTA), m_MouseWheelScrollSize);
    else
      ScrollUp(abs(wheel_delta / NUX_MOUSEWHEEL_DELTA), m_MouseWheelScrollSize);
  }

  bool ScrollView::AcceptKeyNavFocus()
  {
    return false;
  }
}
