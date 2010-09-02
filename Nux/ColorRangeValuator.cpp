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
#include "EditTextBox.h"
#include "HLayout.h"
#include "RangeValue.h"
#include "ColorRangeValuator.h"

NAMESPACE_BEGIN_GUI

ColorRangeValuator::ColorRangeValuator(float value, Color StartColor, Color EndColor)
:   RangeValue(value, 0.0f, 1.0f)
,   m_StartColor(StartColor)
,   m_EndColor(EndColor)
{
    InitializeLayout();
    InitializeWidgets();
}

ColorRangeValuator::~ColorRangeValuator()
{
    DestroyLayout();
}

void ColorRangeValuator::InitializeWidgets()
{

}

void ColorRangeValuator::InitializeLayout()
{

}

void ColorRangeValuator::DestroyLayout()
{

}

Color ColorRangeValuator::GetColor() const
{
    return m_Value * m_EndColor + (1.0f - m_Value) * m_StartColor;
}

void ColorRangeValuator::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    
    GfxContext.PushClippingRectangle(base);

    Geometry P = m_Percentage->GetGeometry();

    gPainter.Paint2DQuadColor(GfxContext, P, m_StartColor, m_StartColor, m_EndColor, m_EndColor);

    m_ValueString->NeedRedraw();
    DrawMarker(GfxContext);

    GfxContext.PopClippingRectangle();
}


NAMESPACE_END_GUI
