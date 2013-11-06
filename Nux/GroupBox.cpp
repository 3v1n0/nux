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
#include "GroupBox.h"

namespace nux
{

  GroupBox::GroupBox(const char * /* Caption */, NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
    ,   m_layout(0)
  {
    m_CaptionArea.SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    m_CaptionArea.SetBaseSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    SetMinimumSize(DEFAULT_WIDGET_WIDTH + 5, PRACTICAL_WIDGET_HEIGHT + 5);
    SetBaseSize(DEFAULT_WIDGET_WIDTH + 5, 2 * PRACTICAL_WIDGET_HEIGHT);
    SetCaption("");
  }

  GroupBox::~GroupBox()
  {

  }

  void GroupBox::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    graphics_engine.PushClippingRectangle(GetGeometry());

    Geometry wireborder_geo = GetGeometry();

    wireborder_geo.OffsetPosition(0, 10);
    wireborder_geo.OffsetSize(0, -10);
    GetPainter().PaintTextLineStatic(graphics_engine, GetSysBoldFont(), m_CaptionArea.GetGeometry(), m_CaptionArea.GetBaseString(), GetTextColor(),
                                    true, eAlignTextCenter);

    if (m_layout != 0)
    {
      m_layout->QueueDraw();
    }

    graphics_engine.PopClippingRectangle();
  }

  void GroupBox::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    graphics_engine.PushClippingRectangle(GetGeometry());

    if (m_layout)
    {
      graphics_engine.PushClippingRectangle(m_layout->GetGeometry());
      m_layout->ProcessDraw(graphics_engine, force_draw);
      graphics_engine.PopClippingRectangle();
    }

    graphics_engine.PopClippingRectangle();
  }

  bool GroupBox::SetLayout(Layout *layout)
  {
    if (View::SetLayout(layout) == false)
    {
      return false;
    }

    m_layout = layout;

    return true;

//    Geometry geo = GetGeometry();
//    Geometry layout_geo = Geometry(geo.x + m_border, geo.y + m_top_border,
//        geo.GetWidth() - 2*m_border, geo.GetHeight() - m_border - m_top_border);
//    m_layout->SetGeometry(layout_geo);
  }

  void GroupBox::PreLayoutManagement()
  {
    // Give the managed layout appropriate size and position..
    if (view_layout_)
    {
      Geometry layout_geo = GetGeometry();
      layout_geo.OffsetPosition(2, 20);
      layout_geo.OffsetSize(-4, -22);
      view_layout_->SetGeometry(layout_geo);
    }
  }

  long GroupBox::PostLayoutManagement(long /* LayoutResult */)
  {
    // A Group box must tightly group its children.
    // So it must embrace the size that was compute for the composition layout.
    // Only the size is change is important here of the GroupBox is important here.

    long ret = 0;
    Geometry old_geo = Area::GetGeometry();

    if (view_layout_)
    {
      Geometry base = view_layout_->GetGeometry();
      base.OffsetPosition(-2, -20);
      base.OffsetSize(4, 22);
      Area::SetGeometry(base);
    }

    Geometry base = GetGeometry();
    m_CaptionArea.SetBaseXY(base.x + 6, base.y);

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

  void GroupBox::ComputeContentPosition(float offsetX, float offsetY)
  {
    if (view_layout_)
    {
      view_layout_->SetBaseX(GetBaseX() + 2);
      view_layout_->SetBaseY(GetBaseY() + 20);
      view_layout_->ComputeContentPosition(offsetX, offsetY);
    }

    Geometry base = GetGeometry();
    m_CaptionArea.SetBaseXY(base.x + 6, base.y);
  }

  void GroupBox::SetCaption(const char *name)
  {
    if ((name == 0) || strlen(name) == 0)
    {
      m_CaptionArea.SetBaseString("");
      m_CaptionArea.SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
      m_CaptionArea.SetBaseSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    }
    else
    {
      m_CaptionArea.SetBaseString(name);
      m_CaptionArea.SetMinimumSize(4 + GetSysBoldFont()->GetStringWidth(name), PRACTICAL_WIDGET_HEIGHT);
      m_CaptionArea.SetBaseSize(4 + GetSysBoldFont()->GetStringWidth(name), PRACTICAL_WIDGET_HEIGHT);
    }
  }

  bool GroupBox::AcceptKeyNavFocus()
  {
    return false;
  }
}
