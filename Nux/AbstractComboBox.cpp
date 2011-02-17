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
#include "AbstractComboBox.h"
#include "HLayout.h"

namespace nux
{

  Color AbstractComboBox::m_sCOMBO_COLOR = Color (0xFF9F9F9F);
  Color AbstractComboBox::m_sCOMBO_BUTTON_COLOR = Color (0xFF4D4D4D);
  Color AbstractComboBox::m_sCOMBO_MOUSEOVER_COLOR = Color (0xFF222222);

  AbstractComboBox::AbstractComboBox (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_MenuIsActive (false)
  {
    InitializeLayout();
    InitializeWidgets();
  }

  AbstractComboBox::~AbstractComboBox()
  {
    DestroyLayout();
  }

  void AbstractComboBox::InitializeWidgets()
  {
    m_hlayout->AddView (m_ComboArea, 1);
    m_hlayout->AddView (m_Button, 0);
    m_hlayout->SetHorizontalExternalMargin (0);
    m_hlayout->SetVerticalExternalMargin (0);
    SetCompositionLayout (m_hlayout);

    m_ComboArea->OnMouseEnter.connect (sigc::mem_fun (this, &AbstractComboBox::RecvMouseEnter) );
    m_ComboArea->OnMouseLeave.connect (sigc::mem_fun (this, &AbstractComboBox::RecvMouseLeave) );

    m_Button->OnMouseEnter.connect (sigc::mem_fun (this, &AbstractComboBox::RecvMouseEnter) );
    m_Button->OnMouseLeave.connect (sigc::mem_fun (this, &AbstractComboBox::RecvMouseLeave) );

    SetTextColor (Color::Black);
  }

  void AbstractComboBox::InitializeLayout()
  {
    m_hlayout   = new HLayout(NUX_TRACKER_LOCATION);
    m_ComboArea = new InputArea(NUX_TRACKER_LOCATION);
    m_Button    = new InputArea(NUX_TRACKER_LOCATION);
  }

  void AbstractComboBox::DestroyLayout()
  {
  }

  void AbstractComboBox::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    GetPainter().PaintBackground (GfxContext, base);
    GetPainter().PaintShape (GfxContext, base, m_sCOMBO_COLOR,  eSHAPE_CORNER_ROUND4);
    GetPainter().PaintTextLineStatic (GfxContext, GetFont (), m_ComboArea->GetGeometry(), m_ComboArea->GetBaseString().GetTCharPtr(), GetTextColor(), eAlignTextLeft);
    Geometry button_geo = m_Button->GetGeometry();
    button_geo.OffsetSize (-5, -2);
    button_geo.OffsetPosition (+4, +1);

    if (m_ComboArea->IsMouseInside() || m_Button->IsMouseInside() )
      GetPainter().PaintShape (GfxContext, button_geo, m_sCOMBO_MOUSEOVER_COLOR,  eSHAPE_CORNER_ROUND4);
    else
      GetPainter().PaintShape (GfxContext, button_geo, m_sCOMBO_BUTTON_COLOR,  eSHAPE_CORNER_ROUND4);

    GeometryPositioning gp (eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning (button_geo, GetTheme().GetImageGeometry (eCOMBOBOX_OPEN_BUTTON), gp);
    GetPainter().PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eCOMBOBOX_OPEN_BUTTON);

    Geometry popup_geometry;
    popup_geometry.SetX (m_ComboArea->GetBaseX() );
    popup_geometry.SetY (m_ComboArea->GetBaseY() + m_ComboArea->GetBaseHeight() );
    popup_geometry.SetWidth (m_ComboArea->GetBaseWidth() );
    popup_geometry.SetHeight (m_ComboArea->GetBaseHeight() );
  }

  void AbstractComboBox::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    GfxContext.PushClippingRectangle (GetGeometry() );

    GfxContext.PopClippingRectangle();
  }

  void AbstractComboBox::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void AbstractComboBox::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void AbstractComboBox::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }



}
