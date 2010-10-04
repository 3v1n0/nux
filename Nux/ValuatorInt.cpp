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
#include "TimerProc.h"
#include "WindowCompositor.h"
#include "ValuatorInt.h"

namespace nux { //NUX_NAMESPACE_BEGIN

const Color SPINBOX_BUTTON_COLOR = Color(0xFF4D4D4D);
const Color SPINBOX_BUTTON_MOUSEOVER_COLOR = Color(0xFF222222);

ValuatorInt::ValuatorInt(int Value, int Step, int MinValue, int MaxValue)
:   m_IntValidator(MinValue, MaxValue)
,   m_Step(Step)
{
    m_EditLine->SetValidator(&m_IntValidator);
    m_EditLine->SetSuffix(TEXT(""));
    m_EditLine->SetPrefix(TEXT(""));
    m_EditLine->SetText(inlPrintf(TEXT("%d"), m_IntValidator.GetMinimum()));

    m_EditLine->SetMinimumSize(1.5*DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    m_EditLine->SetGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    m_MouseControlledButton->SetMinimumSize(16+9, 10);
    m_MouseControlledButton->SetGeometry(Geometry(0, 0, 15, 10));

    // Set the minimum size of this widget.
    // This is use by TextLineEditPropertyItem::GetItemBestHeight
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    m_hlayout = new HLayout(TEXT(""), NUX_TRACKER_LOCATION);
    m_hlayout->AddActiveInterfaceObject(m_MouseControlledButton, 0);
    m_hlayout->AddActiveInterfaceObject(m_EditLine, 1);
    m_hlayout->SetHorizontalInternalMargin(4);
    SetCompositionLayout(m_hlayout);

    SetValue(Value);
}

ValuatorInt::~ValuatorInt()
{
}

long ValuatorInt::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;
    ret = m_MouseControlledButton->OnEvent(ievent, ret, ProcessEventInfo);
    ret = m_EditLine->ProcessEvent(ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
}

void ValuatorInt::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    gPainter.PaintBackground(GfxContext, base);
    if(m_EditLine->IsMouseInside() ||
        m_MouseControlledButton->HasMouseFocus() || m_MouseControlledButton->IsMouseInside())
    {

        gPainter.PaintShapeCorner(GfxContext, base, SPINBOX_BUTTON_MOUSEOVER_COLOR, eSHAPE_CORNER_ROUND4, 
            eCornerTopLeft|eCornerBottomLeft, false); 
    }
    else
    {
        gPainter.PaintShapeCorner(GfxContext, base, SPINBOX_BUTTON_COLOR, eSHAPE_CORNER_ROUND4, 
            eCornerTopLeft|eCornerBottomLeft, false); 
    }

    GeometryPositioning gp(eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning(m_MouseControlledButton->GetGeometry(), gTheme.GetImageGeometry(eVALUATORMOVE), gp);
    if(m_Mouse == 1)
    {
        if(m_MouseControlledButton->IsMouseInside())
            gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eVALUATORHORIZONTALMOVE);
        else
            gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eVALUATORHORIZONTALMOVE);
    }
    else if(m_Mouse == 3)
    {
        if(m_MouseControlledButton->IsMouseInside())
            gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eVALUATORVERTICALMOVE);
        else
            gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eVALUATORVERTICALMOVE);
    }
    else
    {
        if(m_MouseControlledButton->IsMouseInside())
            gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eVALUATORMOVE);
        else
            gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eVALUATORMOVE);
    }



    Geometry geo = m_EditLine->GetGeometry();
    geo.OffsetPosition(-4, 0);
    gPainter.PaintShapeCorner(GfxContext, geo, m_EditLine->GetTextBackgroundColor(), eSHAPE_CORNER_ROUND4, 
        eCornerTopLeft|eCornerBottomLeft, false); 

    m_EditLine->NeedRedraw();
}

void ValuatorInt::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    m_EditLine->ProcessDraw(GfxContext, force_draw);
}

void ValuatorInt::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void ValuatorInt::SetValue(int value)
{
    m_iValue = m_IntValidator.Validate(value);
    m_EditLine->SetText(inlPrintf("%d", m_iValue));
    sigValueChanged.emit(this);
    sigValue.emit(m_iValue);
    NeedRedraw();
}

int ValuatorInt::GetValue() const
{
    return m_iValue;
}

void ValuatorInt::SetStep(int i)
{
    m_Step = i;
    if(m_Step <= 0)
        m_Step = 1;
    NeedRedraw();
}

int ValuatorInt::GetStep() const
{
    return m_Step;
}

int ValuatorInt::GetMinValue() const
{
    return m_IntValidator.GetMinimum();
}

int ValuatorInt::GetMaxValue() const
{
    return m_IntValidator.GetMaximum();
}

void ValuatorInt::SetRange(int MinValue, int Maxvalue)
{
    m_IntValidator.SetMinimum(MinValue);
    m_IntValidator.SetMaximum(Maxvalue);
    m_iValue = m_IntValidator.Validate(m_iValue);
    sigValueChanged.emit(this);
    sigValue.emit(m_iValue);
    NeedRedraw();
}

void ValuatorInt::ImplementIncrementBtn()
{
    SetValue(m_iValue + m_Step);
    if(m_iValue < m_IntValidator.GetMaximum())
    {
        NeedRedraw();
    }
    sigValueChanged.emit(this);
    sigIncrement.emit(this);
    sigValue.emit(m_iValue);
}

void ValuatorInt::ImplementDecrementBtn()
{
    SetValue(m_iValue - m_Step);
    if(m_iValue > m_IntValidator.GetMinimum())
    {
        NeedRedraw();
    }
    sigValueChanged.emit(this);
    sigDecrement.emit(this);
    sigValue.emit(m_iValue);
}

void ValuatorInt::ImplementValidateEntry()
{
    double ret = 0;
    ret = CharToDouble(m_EditLine->GetCleanText().GetTCharPtr());
    {
        m_iValue = m_IntValidator.Validate(ret);
        m_EditLine->SetText(inlPrintf("%d", m_iValue));
        sigValueChanged.emit(this);
        sigValue.emit(m_iValue);
//
//        if(m_iValue < m_IntValidator.GetMinimum())
//        {
//            m_iValue = m_IntValidator.GetMinimum();
//            m_EditLine->SetText(inlPrintf("%d", m_iValue));
//        }
//        if(m_iValue > m_IntValidator.GetMaximum())
//        {
//            m_iValue = m_IntValidator.GetMaximum();
//            m_EditLine->SetText(inlPrintf("%d", m_iValue));
//        }
    }
//     else
//     {
//         m_EditLine->SetText(inlPrintf("%d", m_iValue));
//         sigValueChanged.emit(this);
//         sigValue.emit(m_iValue);
//     }
}



} //NUX_NAMESPACE_END
