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
#include "AbstractComboBox.h"
#include "HLayout.h"

NAMESPACE_BEGIN_GUI

Color AbstractComboBox::m_sCOMBO_COLOR = Color(0xFF9F9F9F);
Color AbstractComboBox::m_sCOMBO_BUTTON_COLOR = Color(0xFF4D4D4D);
Color AbstractComboBox::m_sCOMBO_MOUSEOVER_COLOR = Color(0xFF222222);

AbstractComboBox::AbstractComboBox()
:   m_MenuIsActive(false)
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
    m_hlayout->AddActiveInterfaceObject(m_ComboArea, 1);
    m_hlayout->AddActiveInterfaceObject(m_Button, 0);
    m_hlayout->SetHorizontalExternalMargin(0);
    m_hlayout->SetVerticalExternalMargin(0);
    SetCompositionLayout(m_hlayout);

    m_ComboArea->OnMouseEnter.connect(sigc::mem_fun(this, &AbstractComboBox::RecvMouseEnter));
    m_ComboArea->OnMouseLeave.connect(sigc::mem_fun(this, &AbstractComboBox::RecvMouseLeave));

    m_Button->OnMouseEnter.connect(sigc::mem_fun(this, &AbstractComboBox::RecvMouseEnter));
    m_Button->OnMouseLeave.connect(sigc::mem_fun(this, &AbstractComboBox::RecvMouseLeave));

    SetTextColor(Color::Black);
}

void AbstractComboBox::InitializeLayout()
{
    m_hlayout = smptr(HLayout)(new HLayout());
    m_ComboArea = smptr(CoreArea)(new CoreArea());
    m_Button = smptr(CoreArea)(new CoreArea());
}

void AbstractComboBox::DestroyLayout()
{
}

void AbstractComboBox::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    gPainter.PaintBackground(GfxContext, base);
    gPainter.PaintShape(GfxContext, base, m_sCOMBO_COLOR,  eSHAPE_CORNER_ROUND4);
    gPainter.PaintTextLineStatic(GfxContext, GetFont(), m_ComboArea->GetGeometry(), m_ComboArea->GetBaseString().GetTCharPtr(), GetTextColor(), eAlignTextLeft);
    Geometry button_geo = m_Button->GetGeometry();
    button_geo.OffsetSize(-5, -2);
    button_geo.OffsetPosition(+4, +1);
    if(m_ComboArea->IsMouseInside() || m_Button->IsMouseInside())
        gPainter.PaintShape(GfxContext, button_geo, m_sCOMBO_MOUSEOVER_COLOR,  eSHAPE_CORNER_ROUND4);
    else
        gPainter.PaintShape(GfxContext, button_geo, m_sCOMBO_BUTTON_COLOR,  eSHAPE_CORNER_ROUND4);
    
    GeometryPositioning gp(eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning(button_geo, gTheme.GetImageGeometry(eCOMBOBOX_OPEN_BUTTON), gp);
    gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eCOMBOBOX_OPEN_BUTTON);

    Geometry popup_geometry;
    popup_geometry.SetX(m_ComboArea->GetBaseX());
    popup_geometry.SetY(m_ComboArea->GetBaseY() + m_ComboArea->GetBaseHeight());
    popup_geometry.SetWidth(m_ComboArea->GetBaseWidth());
    popup_geometry.SetHeight(m_ComboArea->GetBaseHeight());
}

void AbstractComboBox::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    GfxContext.PushClippingRectangle(GetGeometry());

    GfxContext.PopClippingRectangle();
}

void AbstractComboBox::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void AbstractComboBox::RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void AbstractComboBox::RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}



NAMESPACE_END_GUI
