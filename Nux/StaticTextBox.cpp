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
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Validator.h"
#include "StaticTextBox.h"

NAMESPACE_BEGIN_GUI
StaticTextBox::StaticTextBox(const TCHAR* Caption, const Color& textcolor, const NFontPtr& Font)
:   m_TextColor(textcolor)
,   m_BackgroundColor(0xFF343434)
,   m_TextAlignment(eAlignTextLeft)
,   m_bMinimumWidthMatchText(true)
,   m_bDrawBackground(false)
,   m_WriteAlpha(true)
{
    // First, set the default minimum size.
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    // Set the font as it is used to compute the text width.
    SetFont(Font);
    // The width size of the text is computed in SetText and set as the minimum for the widget.
    // If the text is null or empty, then the default minimum widtth set above remains.
    SetText(Caption);

    setGeometry(Geometry(0, 0, 3*DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    // This widget does not use a layout!
    m_Background = 0;
}

StaticTextBox::~StaticTextBox()
{
    NUX_SAFE_DELETE(m_Background);
}

long StaticTextBox::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;

    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
}

void StaticTextBox::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    {
        GfxContext.PushClippingRectangle(base);
        if(m_bDrawBackground)
        {
            gPainter.PushDrawLayer(GfxContext, base, m_Background);
            gPainter.PaintTextLineStatic(GfxContext, GetFont(), GetGeometry(), m_Text.GetTCharPtr(), m_TextColor, m_WriteAlpha, m_TextAlignment);
            gPainter.PopBackground();
        }
        else
        {
            //gPainter.PaintBackground(GfxContext, base);
            gPainter.PaintTextLineStatic(GfxContext, GetFont(), GetGeometry(), m_Text.GetTCharPtr(), m_TextColor, m_WriteAlpha, m_TextAlignment);
        }
        GfxContext.PopClippingRectangle();
    }
}

void StaticTextBox::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{

}

void StaticTextBox::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void StaticTextBox::SetText(const TCHAR& Caption)
{
    NString s(Caption);
    SetText(s);
}

void StaticTextBox::SetText(const TCHAR* Caption)
{
    NString s(Caption);
    SetText(s);
}

void StaticTextBox::SetText(const tstring& Caption)
{
    NString s(Caption);
    SetText(s);
}

void StaticTextBox::SetText(const NString& Caption)
{
    m_Text = Caption;
    if(GetMinWidthMatchText())
        AdjustMinWidthToMatchText();
    NeedRedraw();
}

void StaticTextBox::SetMinWidthMatchText(bool b)
{
    m_bMinimumWidthMatchText = b;
    if(m_bMinimumWidthMatchText)
        AdjustMinWidthToMatchText();
}

bool StaticTextBox::GetMinWidthMatchText() const
{
    return m_bMinimumWidthMatchText;
}

void StaticTextBox::AdjustMinWidthToMatchText()
{
    if(m_Text.Size() == 0)
        return;
    SetMinimumWidth(/*4 + */GetFont()->GetStringWidth(m_Text.GetTStringRef()));
}

void StaticTextBox::SetFont(const NFontPtr& Font)
{
    ActiveInterfaceObject::SetFont(Font);
    if(GetMinWidthMatchText())
        AdjustMinWidthToMatchText();
}

void StaticTextBox::SetBackground(AbstractPaintLayer* layer)
{
    NUX_SAFE_DELETE(m_Background);
    m_Background = layer->Clone();
}

NAMESPACE_END_GUI
