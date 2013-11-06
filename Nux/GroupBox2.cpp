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
#include "GroupBox2.h"

namespace nux
{

  int GroupBox2::CAPTION_X_MARGIN = 6;
  int GroupBox2::X_MARGIN = 4;
  int GroupBox2::Y_MARGIN = 4;
  Color GroupBox2::GROUPBOX2_HEADER_BASE_COLOR = Color(0xFF191919);
  Color GroupBox2::GROUPBOX2_HEADER_TEXT_COLOR = Color(0xFFFFFFFF);
  int GroupBox2::TOP_HEADER_HEIGHT = 24;

  GroupBox2::GroupBox2(const char *Caption, NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
    ,   m_layout(0)
  {
    m_CaptionArea = new BasicView(NUX_TRACKER_LOCATION);
    SetMinimumSize(DEFAULT_WIDGET_WIDTH + 5, PRACTICAL_WIDGET_HEIGHT + 5);
    SetBaseSize(DEFAULT_WIDGET_WIDTH + 5, PRACTICAL_WIDGET_HEIGHT + 5);
    SetCaption(Caption);
  }

  GroupBox2::~GroupBox2()
  {
    m_CaptionArea->Dispose();
  }

  void GroupBox2::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    graphics_engine.PushClippingRectangle(GetGeometry());

    Geometry header = GetGeometry();
    header.SetHeight(TOP_HEADER_HEIGHT);
    Geometry layoutgeomerty = GetGeometry();
    layoutgeomerty.OffsetPosition(0, TOP_HEADER_HEIGHT);
    layoutgeomerty.OffsetSize(0, -TOP_HEADER_HEIGHT);
    GetPainter().PaintShapeCorner(graphics_engine, header, Color(0xFF000000), eSHAPE_CORNER_ROUND4, eCornerTopLeft | eCornerTopRight, false);

    GetPainter().PushDrawShapeLayer(graphics_engine, layoutgeomerty, eSHAPE_CORNER_ROUND4, GROUPBOX2_HEADER_BASE_COLOR, eCornerBottomLeft | eCornerBottomRight);

    GetPainter().PaintTextLineStatic(graphics_engine, GetSysBoldFont(), m_CaptionArea->GetGeometry(), m_CaptionArea->GetBaseString(), GROUPBOX2_HEADER_TEXT_COLOR);

    if (m_layout != 0)
    {
      m_layout->QueueDraw();
    }

    GetPainter().PopBackground();
    graphics_engine.PopClippingRectangle();
  }

  void GroupBox2::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    if (m_layout == 0)
      return;

    graphics_engine.PushClippingRectangle(m_layout->GetGeometry());
    Geometry layoutgeomerty = GetGeometry();
    layoutgeomerty.OffsetPosition(0, TOP_HEADER_HEIGHT);
    layoutgeomerty.OffsetSize(0, -TOP_HEADER_HEIGHT);

    if (force_draw)
      GetPainter().PushDrawShapeLayer(graphics_engine, layoutgeomerty, eSHAPE_CORNER_ROUND4, GROUPBOX2_HEADER_BASE_COLOR, eAllCorners);
    else
      GetPainter().PushShapeLayer(graphics_engine, layoutgeomerty, eSHAPE_CORNER_ROUND4, GROUPBOX2_HEADER_BASE_COLOR, eAllCorners);

    if (m_layout)
    {
      graphics_engine.PushClippingRectangle(m_layout->GetGeometry());
      m_layout->ProcessDraw(graphics_engine, force_draw);
      graphics_engine.PopClippingRectangle();
    }

    GetPainter().PopBackground();
    graphics_engine.PopClippingRectangle();
  }

  bool GroupBox2::SetLayout(Layout *layout)
  {
    if (View::SetLayout(layout) == false)
    {
      return false;
    }

    m_layout = layout;

    return true;
  }

  void GroupBox2::PreLayoutManagement()
  {
    // Give the managed layout appropriate size and position..
    if (GetCompositionLayout())
    {
      Geometry layout_geo = GetGeometry();
      layout_geo.OffsetPosition(X_MARGIN, TOP_HEADER_HEIGHT + Y_MARGIN);
      layout_geo.OffsetSize(-2 * X_MARGIN, -TOP_HEADER_HEIGHT - 2 * Y_MARGIN);
      GetCompositionLayout()->SetGeometry(layout_geo);
    }
  }

  long GroupBox2::PostLayoutManagement(long /* LayoutResult */)
  {
    // A Group box must tightly group its children.
    // So it must embrace the size that was compute for the composition layout.
    // Only the size is change is important here of the GroupBox2 is important here.

    long ret = 0;
    Geometry old_geo = Area::GetGeometry();

    if (GetCompositionLayout())
    {
      Geometry base = GetCompositionLayout()->GetGeometry();
      base.OffsetPosition(-X_MARGIN, -TOP_HEADER_HEIGHT - Y_MARGIN);
      base.OffsetSize(2 * X_MARGIN, TOP_HEADER_HEIGHT + 2 * Y_MARGIN);
      Area::SetGeometry(base);
    }

    Geometry base = GetGeometry();
    m_CaptionArea->SetBaseXY(base.x + CAPTION_X_MARGIN, base.y + (TOP_HEADER_HEIGHT - m_CaptionArea->GetBaseHeight()) / 2);


    if (old_geo.GetWidth() > base.GetWidth())
      ret |= eLargerWidth;
    else if (old_geo.GetWidth() < base.GetWidth())
      ret |= eSmallerWidth;
    else
      ret |= eCompliantWidth;

    if (old_geo.GetHeight() > base.GetHeight())
      ret |= eLargerHeight;
    else if (old_geo.GetHeight() < base.GetHeight())
      ret |= eSmallerHeight;
    else
      ret |= eCompliantHeight;

    return ret;
  }

  void GroupBox2::ComputeContentPosition(float offsetX, float offsetY)
  {
    if (GetCompositionLayout())
    {
      GetCompositionLayout()->SetBaseX(GetBaseX() + X_MARGIN);
      GetCompositionLayout()->SetBaseY(GetBaseY() + TOP_HEADER_HEIGHT + Y_MARGIN);
      GetCompositionLayout()->ComputeContentPosition(offsetX, offsetY);
    }

    Geometry base = GetGeometry();
    m_CaptionArea->SetBaseXY(base.x + CAPTION_X_MARGIN, base.y + (TOP_HEADER_HEIGHT - m_CaptionArea->GetBaseHeight()) / 2);
  }

  void GroupBox2::SetCaption(const char *Caption)
  {
    if ((Caption == 0) || (StringLength(Caption) == 0))
    {
      m_CaptionArea->SetBaseString("");
      m_CaptionArea->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
      m_CaptionArea->SetBaseSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    }
    else
    {
      m_CaptionArea->SetBaseString(Caption);
      m_CaptionArea->SetMinimumSize(4 + GetSysBoldFont()->GetStringWidth(Caption), PRACTICAL_WIDGET_HEIGHT);
      m_CaptionArea->SetBaseSize(4 + GetSysBoldFont()->GetStringWidth(Caption), PRACTICAL_WIDGET_HEIGHT);

      Size s = GetMinimumSize();

      if (s.width < 2 * CAPTION_X_MARGIN + m_CaptionArea->GetBaseWidth())
      {
        SetMinimumSize(2 * CAPTION_X_MARGIN + m_CaptionArea->GetBaseWidth(), s.height);
        SetBaseSize(2 * CAPTION_X_MARGIN + m_CaptionArea->GetBaseWidth(), s.height);
      }
    }
  }

  bool GroupBox2::AcceptKeyNavFocus()
  {
    return false;
  }

}
