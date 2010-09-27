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
#include "HLayout.h"
#include "EditTextBox.h"
#include "DoubleValidator.h"
#include "NumericValuator.h"

namespace nux { //NUX_NAMESPACE_BEGIN

const int BTN_WIDTH = 14;
const int BTN_HEIGHT = 14;

NumericValuator::NumericValuator()
:   m_DoubleValidator(0.0, 100.0)
,   m_Step(0.1f)
{
    InitializeLayout();
    InitializeWidgets();
}

NumericValuator::~NumericValuator()
{
    DestroyLayout();
}

void NumericValuator::InitializeWidgets()
{
    m_EditLine->SetValidator(&m_DoubleValidator);
    m_EditLine->SetText(inlPrintf(TEXT("%d"), m_DoubleValidator.GetMinimum()));

    m_EditLine->SetMinimumSize(2*DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    m_EditLine->setGeometry(Geometry(0, 0, 2*DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT));

    m_SpinnerDownBtn->SetMinimumSize(BTN_WIDTH, BTN_HEIGHT);
    m_SpinnerDownBtn->setGeometry(Geometry(0, 0, BTN_WIDTH, BTN_HEIGHT));
    m_SpinnerUpBtn->SetMinimumSize(BTN_WIDTH, BTN_HEIGHT);
    m_SpinnerUpBtn->setGeometry(Geometry(0, 0, BTN_WIDTH, BTN_HEIGHT));
    
    hlayout->AddActiveInterfaceObject(m_SpinnerDownBtn, 0);
    hlayout->AddActiveInterfaceObject(m_EditLine, 1);
    hlayout->AddActiveInterfaceObject(m_SpinnerUpBtn, 0);
    hlayout->SetContentStacking(eStackLeft);

    SetCompositionLayout(hlayout);
}

void NumericValuator::InitializeLayout()
{
    hlayout = smptr(HLayout)(new HLayout());
}

void NumericValuator::DestroyLayout()
{
}

long NumericValuator::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;
    ret = m_SpinnerDownBtn->OnEvent(ievent, ret, ProcessEventInfo);
    ret = m_SpinnerUpBtn->OnEvent(ievent, ret, ProcessEventInfo);
    ret = m_EditLine->ProcessEvent(ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
};

void NumericValuator::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();

    GeometryPositioning gp(eHALeft, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning(m_SpinnerUpBtn->GetGeometry(), gTheme.GetImageGeometry(eTRIANGLE_RIGHT), gp);
    gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eTRIANGLE_RIGHT);

    GeoPo = ComputeGeometryPositioning(m_SpinnerDownBtn->GetGeometry(), gTheme.GetImageGeometry(eTRIANGLE_LEFT), gp);
    gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eTRIANGLE_LEFT);

    m_EditLine->NeedRedraw();
}

void NumericValuator::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    m_EditLine->ProcessDraw(GfxContext, force_draw);
}

void NumericValuator::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void NumericValuator::SetValue(float value)
{
    m_fValue = value;
    if(m_fValue < m_DoubleValidator.GetMinimum())
        m_fValue = m_DoubleValidator.GetMinimum();
    if(m_fValue > m_DoubleValidator.GetMaximum())
        m_fValue = m_DoubleValidator.GetMaximum();

    m_EditLine->SetText(inlPrintf("%f", m_fValue));
}

float NumericValuator::GetValue() const
{
    return m_fValue;
}

void NumericValuator::SetStep(float f)
{
    m_Step = f;
}

float NumericValuator::GetStep()
{
    return m_Step;
}


void NumericValuator::ImplementIncrementBtn()
{
    SetValue(m_fValue + m_Step);
    sigIncrement.emit();
    sigValueChanged.emit(m_fValue);
    if(m_fValue < m_DoubleValidator.GetMaximum())
    {
        m_UpTimerHandler = GetThreadTimer().AddTimerHandler(100, m_UpTimerCallback, 0);
        NeedRedraw();
    }
}

void NumericValuator::ImplementDecrementBtn()
{
    SetValue(m_fValue - m_Step);
    sigDecrement.emit();
    sigValueChanged.emit(m_fValue);
    if(m_fValue > m_DoubleValidator.GetMinimum())
    {
        m_DownTimerHandler = GetThreadTimer().AddTimerHandler(100, m_DownTimerCallback, 0);
        NeedRedraw();
    }
}

void NumericValuator::ImplementValidateEntry()
{
    double ret = 0;
    ret = CharToDouble(m_EditLine->GetCleanText().GetTCharPtr());
    {
        m_fValue = ret;
        if(m_fValue < m_DoubleValidator.GetMinimum())
        {
            m_fValue = m_DoubleValidator.GetMinimum();
            m_EditLine->SetText(inlPrintf("%f", m_fValue));
        }
        if(m_fValue > m_DoubleValidator.GetMaximum())
        {
            m_fValue = m_DoubleValidator.GetMaximum();
            m_EditLine->SetText(inlPrintf("%f", m_fValue));
        }
    }
//     else
//     {
//         m_EditLine->SetText(inlPrintf("%f", m_fValue));
//     }
}

} //NUX_NAMESPACE_END
