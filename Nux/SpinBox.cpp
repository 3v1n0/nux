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
#include "HLayout.h"
#include "VLayout.h"
#include "IntegerValidator.h"
#include "SpinBox.h"

NAMESPACE_BEGIN_GUI

const Color SPINBOX_BUTTON_COLOR = Color(0xFF4D4D4D);
const Color SPINBOX_BUTTON_MOUSEOVER_COLOR = Color(0xFF222222);

SpinBox::SpinBox(int Value, int Step, int MinValue, int MaxValue)
:   m_IntValidator(MinValue, MaxValue)
,   m_Step(Step)
{
    InitializeLayout();
    InitializeWidgets();
    SetValue(Value);
}

SpinBox::~SpinBox()
{
    DestroyLayout();
}

void SpinBox::InitializeWidgets()
{
    m_EditLine->SetValidator(&m_IntValidator);
    m_EditLine->SetSuffix(TEXT(""));
    m_EditLine->SetPrefix(TEXT(""));
    m_EditLine->SetText(inlPrintf(TEXT("%d"), m_IntValidator.GetMinimum()));

    m_EditLine->SetMinimumSize(1.5*DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    m_EditLine->setGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    m_SpinnerUpBtn->SetMinimumSize(15, 10);
    m_SpinnerUpBtn->setGeometry(Geometry(0, 0, 15, 10));
    m_SpinnerDownBtn->SetMinimumSize(15, 10);
    m_SpinnerDownBtn->setGeometry(Geometry(0, 0, 15, 10));

    m_UpTimerCallback = new TimerFunctor;
    m_UpTimerCallback->OnTimerExpired.connect(sigc::mem_fun(this, &SpinBox_Logic::TimerSpinUpBtn));
    m_DownTimerCallback = new TimerFunctor;
    m_DownTimerCallback->OnTimerExpired.connect(sigc::mem_fun(this, &SpinBox_Logic::TimerSpinDownBtn));

    // Set the minimum size of this widget.
    // This is use by TextLineEditPropertyItem::GetItemBestHeight
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    hlayout->AddActiveInterfaceObject(m_EditLine, 1);

    vlayout->AddActiveInterfaceObject(m_SpinnerUpBtn, 1);
    vlayout->AddActiveInterfaceObject(m_SpinnerDownBtn, 1);
    hlayout->AddLayout(vlayout, 0);

    SetCompositionLayout(hlayout);
}

void SpinBox::InitializeLayout()
{
    hlayout = smptr(HLayout)(new HLayout());
    vlayout = smptr(VLayout)(new VLayout());
}

void SpinBox::DestroyLayout()
{
}

long SpinBox::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;
    ret = m_SpinnerUpBtn->OnEvent(ievent, ret, ProcessEventInfo);
    ret = m_SpinnerDownBtn->OnEvent(ievent, ret, ProcessEventInfo);
    ret = m_EditLine->ProcessEvent(ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
}

void SpinBox::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    gPainter.PaintBackground(GfxContext, base);
    if(m_EditLine->IsMouseInside() || m_SpinnerUpBtn->IsMouseInside() || m_SpinnerDownBtn->IsMouseInside())
    {

        gPainter.PaintShapeCorner(GfxContext, m_SpinnerUpBtn->GetGeometry(), SPINBOX_BUTTON_MOUSEOVER_COLOR, eSHAPE_CORNER_ROUND4, 
            eCornerTopRight, false); 
        gPainter.PaintShapeCorner(GfxContext, m_SpinnerDownBtn->GetGeometry(), SPINBOX_BUTTON_MOUSEOVER_COLOR, eSHAPE_CORNER_ROUND4, 
            eCornerBottomRight, false); 
    }
    else
    {
        gPainter.PaintShapeCorner(GfxContext, m_SpinnerUpBtn->GetGeometry(), SPINBOX_BUTTON_COLOR, eSHAPE_CORNER_ROUND4, 
            eCornerTopRight, false); 
        gPainter.PaintShapeCorner(GfxContext, m_SpinnerDownBtn->GetGeometry(), SPINBOX_BUTTON_COLOR, eSHAPE_CORNER_ROUND4, 
            eCornerBottomRight, false); 
    }

    GeometryPositioning gp(eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning(m_SpinnerUpBtn->GetGeometry(), gTheme.GetImageGeometry(eSPINER_UP), gp);
    if(m_SpinnerUpBtn->IsMouseInside())
        gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eSPINER_UP);
    else
        gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eSPINER_UP);


    gp.SetAlignment(eHACenter, eVACenter);
    GeoPo = ComputeGeometryPositioning(m_SpinnerDownBtn->GetGeometry(), gTheme.GetImageGeometry(eSPINER_DOWN), gp);
    if(m_SpinnerDownBtn->IsMouseInside())
        gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eSPINER_DOWN);
    else
        gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eSPINER_DOWN);

    m_EditLine->NeedRedraw();
}

void SpinBox::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    m_EditLine->ProcessDraw(GfxContext, force_draw);
}

void SpinBox::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void SpinBox::SetValue(int value)
{
    m_iValue = m_IntValidator.Validate(value);
    m_EditLine->SetText(inlPrintf("%d", m_iValue));
    sigValueChanged.emit(smptr(SpinBox)(this, false));
    sigValue.emit(m_iValue);
    NeedRedraw();
}

int SpinBox::GetValue() const
{
    return m_iValue;
}

void SpinBox::SetStep(int i)
{
    m_Step = i;
    if(m_Step <= 0)
        m_Step = 1;
    NeedRedraw();
}

int SpinBox::GetStep() const
{
    return m_Step;
}

int SpinBox::GetMinValue() const
{
    return m_IntValidator.GetMinimum();
}

int SpinBox::GetMaxValue() const
{
    return m_IntValidator.GetMaximum();
}

void SpinBox::SetRange(int MinValue, int Maxvalue)
{
    m_IntValidator.SetMinimum(MinValue);
    m_IntValidator.SetMaximum(Maxvalue);
    m_iValue = m_IntValidator.Validate(m_iValue);
    sigValueChanged.emit(smptr(SpinBox)(this, false));
    sigValue.emit(m_iValue);
    NeedRedraw();
}

void SpinBox::ImplementIncrementBtn()
{
    SetValue(m_iValue + m_Step);
    if(m_iValue < m_IntValidator.GetMaximum())
    {
        if(m_UpTimerHandler)
            m_UpTimerHandler = GetThreadTimer().AddTimerHandler(100, m_UpTimerCallback, 0);
        else
            m_UpTimerHandler = GetThreadTimer().AddTimerHandler(800, m_UpTimerCallback, 0);
        NeedRedraw();
    }
    sigValueChanged.emit(smptr(SpinBox)(this, false));
    sigIncrement.emit(smptr(SpinBox)(this, false));
    sigValue.emit(m_iValue);
}

void SpinBox::ImplementDecrementBtn()
{
    SetValue(m_iValue - m_Step);
    if(m_iValue > m_IntValidator.GetMinimum())
    {
        if(m_DownTimerHandler)
            m_DownTimerHandler = GetThreadTimer().AddTimerHandler(100, m_DownTimerCallback, 0);
        else
            m_DownTimerHandler = GetThreadTimer().AddTimerHandler(800, m_DownTimerCallback, 0);
        NeedRedraw();
    }
    sigValueChanged.emit(smptr(SpinBox)(this, false));
    sigDecrement.emit(smptr(SpinBox)(this, false));
    sigValue.emit(m_iValue);
}

void SpinBox::ImplementValidateEntry()
{
    double ret = 0;
    if(CharToDouble(m_EditLine->GetCleanText().GetTCharPtr(), ret))
    {
        m_iValue = m_IntValidator.Validate(ret);
        m_EditLine->SetText(inlPrintf("%d", m_iValue));
        sigValueChanged.emit(smptr(SpinBox)(this, false));
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
    else
    {
        m_EditLine->SetText(inlPrintf("%d", m_iValue));
        sigValueChanged.emit(smptr(SpinBox)(this, false));
        sigValue.emit(m_iValue);
    }
}

NAMESPACE_END_GUI
