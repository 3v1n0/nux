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
#include "AbstractComboBox.h"
#include "HLayout.h"
#include "StaticText.h"

namespace nux
{

  Color AbstractComboBox::m_sCOMBO_COLOR = Color(0x009F9F00);
  Color AbstractComboBox::m_sCOMBO_BUTTON_COLOR = Color(0x00FFFFFF);
  Color AbstractComboBox::m_sCOMBO_MOUSEOVER_COLOR = Color(0x55FFFFFF);

  AbstractComboBox::AbstractComboBox(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
    ,   m_MenuIsActive(false)
  {
    m_hlayout   = new HLayout(NUX_TRACKER_LOCATION);
    _combo_box_area = new BasicView(NUX_TRACKER_LOCATION);
    _combo_box_opening_area    = new BasicView(NUX_TRACKER_LOCATION);

    m_hlayout->AddView(_combo_box_area, 1);
    m_hlayout->AddView(_combo_box_opening_area, 0);
    m_hlayout->SetHorizontalExternalMargin(0);
    m_hlayout->SetVerticalExternalMargin(0);
    SetLayout(m_hlayout);

    _combo_box_area->mouse_enter.connect(sigc::mem_fun(this, &AbstractComboBox::RecvMouseEnter));
    _combo_box_area->mouse_leave.connect(sigc::mem_fun(this, &AbstractComboBox::RecvMouseLeave));

    _combo_box_opening_area->mouse_enter.connect(sigc::mem_fun(this, &AbstractComboBox::RecvMouseEnter));
    _combo_box_opening_area->mouse_leave.connect(sigc::mem_fun(this, &AbstractComboBox::RecvMouseLeave));

    SetTextColor(color::Black);

    _pango_static_text = new StaticText("", NUX_TRACKER_LOCATION);
  }

  AbstractComboBox::~AbstractComboBox()
  {
    if (_pango_static_text)
      _pango_static_text->Dispose();
    _pango_static_text = 0;
  }

  void AbstractComboBox::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    
    graphics_engine.PushClippingRectangle(base);
    GetPainter().PaintBackground(graphics_engine, base);
    GetPainter().PaintShape(graphics_engine, base, m_sCOMBO_COLOR,  eSHAPE_CORNER_ROUND4);

    {
      //GetPainter().PaintTextLineStatic(graphics_engine, GetFont(), _combo_box_area->GetGeometry(), _combo_box_area->GetBaseString().c_str(), GetTextColor(), eAlignTextLeft);
      _pango_static_text->SetGeometry(_combo_box_area->GetGeometry());
      _pango_static_text->ProcessDraw(graphics_engine, true);
    }
    
    Geometry button_geo = _combo_box_opening_area->GetGeometry();
    button_geo.OffsetSize(-5, -2);
    button_geo.OffsetPosition(+4, +1);

    if (_combo_box_area->IsMouseInside() || _combo_box_opening_area->IsMouseInside())
      GetPainter().PaintShape(graphics_engine, button_geo, m_sCOMBO_MOUSEOVER_COLOR,  eSHAPE_CORNER_ROUND4);
    else
      GetPainter().PaintShape(graphics_engine, button_geo, m_sCOMBO_BUTTON_COLOR,  eSHAPE_CORNER_ROUND4);

    GeometryPositioning gp(eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning(button_geo, GetTheme().GetImageGeometry(eCOMBOBOX_OPEN_BUTTON), gp);
    GetPainter().PaintShape(graphics_engine, GeoPo, Color(0xFFFFFFFF), eCOMBOBOX_OPEN_BUTTON);

    Geometry popup_geometry;
    popup_geometry.SetX(_combo_box_area->GetBaseX());
    popup_geometry.SetY(_combo_box_area->GetBaseY() + _combo_box_area->GetBaseHeight());
    popup_geometry.SetWidth(_combo_box_area->GetBaseWidth());
    popup_geometry.SetHeight(_combo_box_area->GetBaseHeight());

    graphics_engine.PopClippingRectangle();
  }

  void AbstractComboBox::DrawContent(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {

  }

  void AbstractComboBox::RecvMouseEnter(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void AbstractComboBox::RecvMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }



}
