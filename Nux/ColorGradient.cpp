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
#include "ColorGradient.h"

NAMESPACE_BEGIN_GUI

ColorGradient::ColorGradient(float Value, float MinValue, float MaxValue)
:   m_min(MinValue)
,   m_max(MaxValue)
,   m_BackgroundColor(0xff202020)
,   m_CTRL_KEY(0)
,   m_color_format(Color::COLORFORMAT_FLOAT)
{
    InitializeLayout();
    InitializeWidgets();
    SetColorFormat(m_color_format);
    SetValue(Value);
}

ColorGradient::~ColorGradient()
{
    DestroyLayout();
}

void ColorGradient::InitializeWidgets()
{
    //////////////////
    // Set Signals  //
    //////////////////
    m_Percentage->OnMouseDown.connect( sigc::mem_fun(this, &ColorGradient::OnReceiveMouseDown));
    m_Percentage->OnMouseUp.connect( sigc::mem_fun(this, &ColorGradient::OnReceiveMouseUp));
    m_Percentage->OnMouseDrag.connect( sigc::mem_fun(this, &ColorGradient::OnReceiveMouseDrag));

    m_ValueString->sigValidateKeyboardEntry.connect(sigc::mem_fun(this, &ColorGradient::OnValidateKeyboardEntry));

    //////////////////
    // Set Geometry //
    //////////////////
    m_ValueString->SetMinimumSize(DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    m_ValueString->setGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT));

    m_Percentage->SetMinimumSize(2*DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    m_Percentage->setGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT));

    // Set the minimum size of this widget.
    // This is use by RangeValuePropertyItem::GetItemBestHeight
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    //SetMaximumHeight(20);
    // Set layout
    m_ValueString->SetSizePolicy(eSizeResizeable);
    m_Percentage->SetSizePolicy(eSizeResizeable);

    hlayout->AddActiveInterfaceObject(m_ValueString, 0, eCenter, eFull);
    hlayout->AddActiveInterfaceObject(m_Percentage, 4, eCenter, eFull);
    //hlayout->AddLayout(&vlayout, 4);
    hlayout->SetHorizontalExternalMargin(0);
    hlayout->SetHorizontalInternalMargin(2);
    hlayout->SetVerticalExternalMargin(0);
    SetCompositionLayout(hlayout);
}

void ColorGradient::InitializeLayout()
{
    hlayout = smptr(HLayout)(new HLayout());
    m_Percentage = smptr(CoreArea)(new CoreArea());
    m_ValueString = smptr(EditTextBox)(new EditTextBox());
}

void ColorGradient::DestroyLayout()
{
}


long ColorGradient::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    m_CTRL_KEY = ievent.GetVirtualKeyState(NUX_VK_CONTROL);

    long ret;
    ret = m_Percentage->OnEvent(ievent, TraverseInfo, ProcessEventInfo);
    ret = m_ValueString->ProcessEvent(ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);

    if(m_ValueString->IsRedrawNeeded())
    {
        NeedRedraw();
    }
    return ret;
}

void ColorGradient::DrawMarker(GraphicsContext& GfxContext)
{
    int marker_position_x;
    int marker_position_y;

    GfxContext.PushClippingRectangle(m_Percentage->GetGeometry());

    marker_position_x = m_Percentage->GetBaseX() + (m_Value - m_min) * m_Percentage->GetBaseWidth() * 1/(m_max - m_min);
    marker_position_y = m_Percentage->GetBaseY() + m_Percentage->GetBaseHeight();
    gPainter.Draw2DTriangleColor(GfxContext, marker_position_x - 5, marker_position_y,
        marker_position_x, marker_position_y - 5,
        marker_position_x + 5, marker_position_y, Color(0.0f, 0.0f, 0.0f, 1.0f));

    gPainter.Draw2DTriangleColor(GfxContext, marker_position_x - 4, marker_position_y,
        marker_position_x, marker_position_y - 4,
        marker_position_x + 4, marker_position_y, Color(0.7f, 0.7f, 0.7f, 1.0f));

    GfxContext.PopClippingRectangle();
}


void ColorGradient::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    bool highlighted;
    Geometry base = GetGeometry();

    Geometry P = m_Percentage->GetGeometry();
    
    int NumColorMark = m_ColorMarkGroup.GetNumColorMark();
    int X = P.x;
    
    if(NumColorMark < 2)
    {
        gPainter.Paint2DQuadColor(GfxContext, P, m_BackgroundColor);
    }
    else
    {
        for(int i = 0; i < NumColorMark; i++)
        {
            ColorMarkGroup::ColorMark cmark = m_ColorMarkGroup.GetColorMark(i);
            float coeff = cmark.GetX();
            Color color = cmark.GetColor();

            if((i == 0) && (coeff > 0))
            {
                Geometry geo(X, P.y, P.GetWidth()*coeff, P.GetHeight());
                gPainter.Paint2DQuadColor(GfxContext, geo, color);
                X += P.GetWidth()*coeff;
            }
            else if((i == NumColorMark-1) && (coeff < 1.0f))
            {
                nuxAssert(P.x + P.GetWidth() > X);
                Geometry geo(X, P.y, P.x + P.GetWidth() - X, P.GetHeight());
                gPainter.Paint2DQuadColor(GfxContext, geo, color);
                X = P.GetWidth();
            }
            else if(i < NumColorMark - 1)
            {
                float coeff1 = m_ColorMarkGroup.GetColorMark(i+1).GetX();
                Color color1 = m_ColorMarkGroup.GetColorMark(i+1).GetColor();
                Geometry geo(X, P.y, P.GetWidth()*coeff1 - P.GetWidth()*coeff, P.GetHeight());
                if((i == NumColorMark-2) && (coeff1 == 1.0f))
                {
                    // correct rounding errors
                    geo.SetWidth(P.x + P.GetWidth() - X);
                }
                gPainter.Paint2DQuadColor(GfxContext, geo, color, color, color1, color1);
                X += P.GetWidth()*coeff1 - P.GetWidth()*coeff;
            }
        }
    }

    m_ValueString->ProcessDraw(GfxContext, true);

    if(m_ValueString->IsMouseInside())
        highlighted = true;
    else
        highlighted = false;

    DrawMarker(GfxContext);
}

void ColorGradient::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    m_ValueString->ProcessDraw(GfxContext, force_draw);
}

void ColorGradient::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

/////////////////
//  RECEIVERS  //
/////////////////
void ColorGradient::SetRange(float min_value, float max_value)
{
    if(min_value < max_value)
    {
        m_min = min_value;
        m_max = max_value;
    }
    else
    {
        m_min = max_value;
        m_max = min_value;
    }

    if(m_Value < m_min)
        m_Value = m_min;
    else if(m_Value > m_max)
        m_Value = m_max;

    SetValue(m_Value);
}

void ColorGradient::SetValue(float value)
{
    if(value < m_min)
        m_Value = m_min;
    else if(value > m_max)
        m_Value = m_max;
    else
        m_Value = value;

    if(m_color_format == Color::COLORFORMAT_FLOAT)
    {
        m_ValueString->SetText(inlPrintf("%.3f", m_Value ));
    }

    if(m_color_format == Color::COLORFORMAT_INT)
    {
        m_ValueString->SetText(inlPrintf("%d", (int)(m_Value*255) ));
    }

    if(m_color_format == Color::COLORFORMAT_HEX)
    {
        m_ValueString->SetText(inlPrintf("%x", (int)(m_Value*255) ));
    }

    //m_ValueString->SetText(inlPrintf("%.3f", m_Value));
    NeedRedraw();
}

float ColorGradient::GetValue() const
{
    return m_Value;
}

////////////////
//  EMITTERS  //
////////////////
void ColorGradient::OnReceiveMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags) 
{ 
    if(x < 0)
        m_Value = m_min;
    else if(x > m_Percentage->GetBaseWidth())
        m_Value = m_max;
    else
        m_Value = m_min + (m_max-m_min) * (float)x / (float)m_Percentage->GetBaseWidth();

    SetValue(m_Value);
    //m_ValueString->SetText(inlPrintf("%.3f", m_Value));
    sigValueChanged.emit(smptr(ColorGradient)(this, false));
    sigFloatChanged.emit(m_Value);
    sigMouseDown.emit(m_Value);

    NeedRedraw();
}

void ColorGradient::OnReceiveMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags) 
{ 

    if(x < 0)
        m_Value = m_min;
    else if(x > m_Percentage->GetBaseWidth())
        m_Value = m_max;
    else
        m_Value = m_min + (m_max-m_min) * (float)x / (float)m_Percentage->GetBaseWidth();

    SetValue(m_Value);
    //m_ValueString->SetText(inlPrintf("%.3f", m_Value));
    sigValueChanged.emit(smptr(ColorGradient)(this, false));
    sigFloatChanged.emit(m_Value);
    sigMouseUp.emit(m_Value);

    NeedRedraw();
}

void ColorGradient::OnReceiveMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    if(x < 0)
        m_Value = m_min;
    else if(x > m_Percentage->GetBaseWidth())
        m_Value = m_max;
    else
        m_Value = m_min + (m_max-m_min) * (float)x / (float)m_Percentage->GetBaseWidth();

    SetValue(m_Value);
    //m_ValueString->SetText(inlPrintf("%.3f", m_Value));
    sigValueChanged.emit(smptr(ColorGradient)(this, false));
    sigFloatChanged.emit(m_Value);
    sigMouseDrag.emit(m_Value);

    NeedRedraw();
}

void ColorGradient::OnKeyboardFocus()
{

}

void ColorGradient::OnLostKeyboardFocus()
{

}

void ColorGradient::OnValidateKeyboardEntry(const weaksmptr(EditTextBox) textbox, const NString& text)
{
    float f = 0;
    if((m_color_format == Color::COLORFORMAT_HEX) && (m_HexRegExp.Validate(text.GetTCharPtr()) == Validator::Acceptable))
    {
        f = (float)m_HexRegExp.ToInteger(text.GetTCharPtr()) / 255.0f;
    }
    else if((m_color_format == Color::COLORFORMAT_INT) && (m_IntRegExp.Validate(text.GetTCharPtr()) == Validator::Acceptable))
    {
        f = (float)m_IntRegExp.ToInteger(text.GetTCharPtr()) / 255.0f;
    }
    else 
    {
        f = (float)m_DoubleRegExp.ToDouble(text.GetTCharPtr());
    }

    //inlCharToFloat(text.c_str(), f);
    SetValue(f);
    sigValueChanged.emit(smptr(ColorGradient)(this, false));
    sigFloatChanged.emit(m_Value);
    sigSetTypedValue.emit(f);
    NeedRedraw();
}

void ColorGradient::EmitFloatChangedSignal()
{
    sigFloatChanged.emit(m_Value);
}

void ColorGradient::AddColorMark(DOUBLE x, Color color, bool selected)
{
    m_ColorMarkGroup.AddColorMark(x, color, selected);
    NeedRedraw();
}

void ColorGradient::Reset()
{
    m_ColorMarkGroup.Reset();
}

void ColorGradient::SetColorFormat(Color::Format cf)
{
    if(cf == Color::COLORFORMAT_FLOAT)
    {
        m_color_format = Color::COLORFORMAT_FLOAT;
        m_ValueString->SetKeyEntryType(BaseKeyboardHandler::eAlphaNumeric);
        m_ValueString->SetPrefix(TEXT(""));
    }

    if(cf == Color::COLORFORMAT_INT)
    {
        m_color_format = Color::COLORFORMAT_INT;
        m_ValueString->SetKeyEntryType(BaseKeyboardHandler::eIntegerNumber);
        m_ValueString->SetPrefix(TEXT(""));
    }

    if(cf == Color::COLORFORMAT_HEX)
    {
        m_color_format = Color::COLORFORMAT_HEX;
        m_ValueString->SetKeyEntryType(BaseKeyboardHandler::eHexadecimalNumber);
        m_ValueString->SetPrefix(TEXT("0x"));
    }

    SetValue(m_Value);
    //     if(m_color_model == CM_RGB)
    //     {
    //         SetRGBA(m_Red, m_Green, m_Blue, m_Alpha);
    //     }
    //     else if(m_color_model == CM_HLS)
    //     {
    //         SetHLS(m_HLSHue, m_HLSLight, m_HLSSaturation);
    //         SetAlpha(m_Alpha);
    //     }
    //     else if(m_color_model == CM_HSV)
    //     {
    //         SetHSV(m_HSVHue, m_HSVSaturation, m_HSVValue);
    //         SetAlpha(m_Alpha);
    //     }
}


NAMESPACE_END_GUI
