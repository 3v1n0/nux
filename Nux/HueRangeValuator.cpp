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
#include "HueRangeValuator.h"

NAMESPACE_BEGIN_GUI

HueRangeValuator::HueRangeValuator(float value, float vmin, float vmax)
:   RangeValue(value, vmin, vmax)
,   m_HLSSaturation(1.0f)
,   m_HLSLight(1.0f)
,   m_HSVSaturation(1.0f)
,   m_HSVValue(1.0f)
,   m_Model(CM_HLS)
{
    InitializeLayout();
    InitializeWidgets();
}

HueRangeValuator::~HueRangeValuator()
{
    DestroyLayout();
}

void HueRangeValuator::InitializeWidgets()
{

}

void HueRangeValuator::InitializeLayout()
{

}

void HueRangeValuator::DestroyLayout()
{

}

void HueRangeValuator::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    
    GfxContext.PushClippingRectangle(base);

    Geometry P = m_Percentage->GetGeometry();
    if(m_Model == CM_HLS)
    {
        float s = 1.0f - m_HLSSaturation;
        float l = m_HLSLight;
        float fw = P.GetWidth()/6;

        Geometry p = Geometry(P.x, P.y, fw, P.GetHeight());
        gPainter.Paint2DQuadColor(GfxContext, p, Color(1.0f * l, s * l, s * l), Color(1.0f * l, s * l, s * l),
            Color(1.0f * l, 1.0f * l, s * l), Color(1.0f * l, 1.0f * l, s * l));
        p.SetX(P.x + fw);
        gPainter.Paint2DQuadColor(GfxContext, p, Color(1.0f * l, 1.0f * l, s * l), Color(1.0f * l, 1.0f * l, s * l), 
            Color(s*l, 1.0f*l, s*l), Color(s*l, 1.0f*l, s*l));

        p.SetX(P.x + 2*fw);
        gPainter.Paint2DQuadColor(GfxContext, p, Color(s * l, 1.0f * l, s * l), Color(s * l, 1.0f * l, s * l),
            Color(s * l, 1.0f * l, 1.0f * l), Color(s * l, 1.0f * l, 1.0f * l));
        p.SetX(P.x + 3*fw);
        gPainter.Paint2DQuadColor(GfxContext, p, Color(s * l, 1.0f * l, 1.0f * l), Color(s * l, 1.0f * l, 1.0f * l),
            Color(s * l, s * l, 1.0f * l), Color(s * l, s * l, 1.0f * l));

        p.SetX(P.x + 4*fw);
        gPainter.Paint2DQuadColor(GfxContext, p, Color(s * l, s * l, 1.0f * l), Color(s * l, s * l, 1.0f * l),
            Color(1.0f * l, s * l, 1.0f * l), Color(1.0f * l, s * l, 1.0f * l));
        p.SetX(P.x + 5*fw);
        p.SetWidth(P.GetWidth() - 5*fw); // correct rounding errors
        gPainter.Paint2DQuadColor(GfxContext, p, Color(1.0f * l, s * l, 1.0f * l), Color(1.0f * l, s * l, 1.0f * l),
            Color(1.0f * l, s * l, s * l), Color(1.0f * l, s * l, s * l));
    }
    else
    {
        float s = 1.0f - m_HSVSaturation;
        float v = m_HSVValue;
        float fw = P.GetWidth()/6;

        Geometry p = Geometry(P.x, P.y, fw, P.GetHeight());
        gPainter.Paint2DQuadColor(GfxContext, p, Color(1.0f * v, s * v, s * v), Color(1.0f * v, s * v, s * v), 
            Color(1.0f * v, 1.0f * v, s * v), Color(1.0f * v, 1.0f * v, s * v));
        p.SetX(P.x + fw);
        gPainter.Paint2DQuadColor(GfxContext, p, Color(1.0f * v, 1.0f * v, s * v), Color(1.0f * v, 1.0f * v, s * v), 
            Color(s*v, 1.0f*v, s*v), Color(s*v, 1.0f*v, s*v));

        p.SetX(P.x + 2*fw);
        gPainter.Paint2DQuadColor(GfxContext, p, Color(s * v, 1.0f * v, s * v), Color(s * v, 1.0f * v, s * v), 
            Color(s * v, 1.0f * v, 1.0f * v), Color(s * v, 1.0f * v, 1.0f * v));
        p.SetX(P.x + 3*fw);
        gPainter.Paint2DQuadColor(GfxContext, p, Color(s * v, 1.0f * v, 1.0f * v), Color(s * v, 1.0f * v, 1.0f * v), 
            Color(s * v, s * v, 1.0f * v), Color(s * v, s * v, 1.0f * v));

        p.SetX(P.x + 4*fw);
        gPainter.Paint2DQuadColor(GfxContext, p, Color(s * v, s * v, 1.0f * v), Color(s * v, s * v, 1.0f * v), 
            Color(1.0f * v, s * v, 1.0f * v), Color(1.0f * v, s * v, 1.0f * v));
        p.SetX(P.x + 5*fw);
        p.SetWidth(P.GetWidth() - 5*fw); // correct rounding errors
        gPainter.Paint2DQuadColor(GfxContext, p, Color(1.0f * v, s * v, 1.0f * v), Color(1.0f * v, s * v, 1.0f * v), 
            Color(1.0f * v, s * v, s * v), Color(1.0f * v, s * v, s * v));
    }

    m_ValueString->NeedRedraw();
    DrawMarker(GfxContext);

    GfxContext.PopClippingRectangle();
}


NAMESPACE_END_GUI