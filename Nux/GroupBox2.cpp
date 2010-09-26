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
#include "GroupBox2.h"

NAMESPACE_BEGIN_GUI

int GroupBox2::CAPTION_X_MARGIN = 6;
int GroupBox2::X_MARGIN = 4;
int GroupBox2::Y_MARGIN = 4;
Color GroupBox2::GROUPBOX2_HEADER_BASE_COLOR = Color(0xFF191919);
Color GroupBox2::GROUPBOX2_HEADER_TEXT_COLOR = Color(0xFFFFFFFF);
int GroupBox2::TOP_HEADER_HEIGHT = 24;

GroupBox2::GroupBox2(const TCHAR* Caption, NUX_FILE_LINE_DECL)
:   ActiveInterfaceObject(NUX_FILE_LINE_PARAM)
,   bCaptionAvailable(false)
,   m_layout(0)
{
    m_CaptionArea = smptr(CoreArea)(new CoreArea);
    SetMinimumSize(DEFAULT_WIDGET_WIDTH+5, PRACTICAL_WIDGET_HEIGHT+5);
    SetBaseSize(DEFAULT_WIDGET_WIDTH+5, PRACTICAL_WIDGET_HEIGHT+5);
    setCaption(Caption);
}

GroupBox2::~GroupBox2()
{

}

long GroupBox2::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;
    if(m_layout != 0)
    {
        ret = m_layout->ProcessEvent(ievent, ret, ProcessEventInfo);
    }
    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
}


void GroupBox2::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    GfxContext.PushClippingRectangle(GetGeometry());

    Geometry header = GetGeometry();
    header.SetHeight(TOP_HEADER_HEIGHT);
    Geometry layoutgeomerty = GetGeometry();
    layoutgeomerty.OffsetPosition(0, TOP_HEADER_HEIGHT);
    layoutgeomerty.OffsetSize(0, -TOP_HEADER_HEIGHT);
    gPainter.PaintShapeCorner(GfxContext, header, Color(0xFF000000), eSHAPE_CORNER_ROUND4, eCornerTopLeft|eCornerTopRight, false);

    gPainter.PushDrawShapeLayer(GfxContext, layoutgeomerty, eSHAPE_CORNER_ROUND4, GROUPBOX2_HEADER_BASE_COLOR, eCornerBottomLeft|eCornerBottomRight);

    //if(bCaptionAvailable)
    {
        gPainter.PaintTextLineStatic(GfxContext, GFontBold, m_CaptionArea->GetGeometry(), m_CaptionArea->GetBaseString(), GROUPBOX2_HEADER_TEXT_COLOR);
    }

    if(m_layout != 0)
    {
        m_layout->NeedRedraw();
    }

    gPainter.PopBackground();
    GfxContext.PopClippingRectangle();
}

void GroupBox2::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    if(m_layout == 0)
        return;
    GfxContext.PushClippingRectangle(m_layout->GetGeometry());
    Geometry layoutgeomerty = GetGeometry();
    layoutgeomerty.OffsetPosition(0, TOP_HEADER_HEIGHT);
    layoutgeomerty.OffsetSize(0, -TOP_HEADER_HEIGHT);
    
    if(force_draw)
        gPainter.PushDrawShapeLayer(GfxContext, layoutgeomerty, eSHAPE_CORNER_ROUND4, GROUPBOX2_HEADER_BASE_COLOR, eAllCorners);
    else
        gPainter.PushShapeLayer(GfxContext, layoutgeomerty, eSHAPE_CORNER_ROUND4, GROUPBOX2_HEADER_BASE_COLOR, eAllCorners);

    if(m_layout.IsValid())
    {
        GfxContext.PushClippingRectangle(m_layout->GetGeometry());
        m_layout->ProcessDraw(GfxContext, force_draw);
        GfxContext.PopClippingRectangle();
    }

    gPainter.PopBackground();
    GfxContext.PopClippingRectangle();
}

void GroupBox2::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void GroupBox2::setLayout(smptr(Layout) layout)
{
    if(layout == 0)
        return;

    m_layout = layout;
    SetCompositionLayout(m_layout);
}

void GroupBox2::PreLayoutManagement()
{
    // Give the managed layout appropriate size and position..
    if(GetCompositionLayout().IsValid())
    {
        Geometry layout_geo = GetGeometry();
        //if(bCaptionAvailable)
        {
            layout_geo.OffsetPosition(X_MARGIN, TOP_HEADER_HEIGHT + Y_MARGIN);
            layout_geo.OffsetSize(-2*X_MARGIN, -TOP_HEADER_HEIGHT-2*Y_MARGIN);
        }
//        else
//        {
//            layout_geo.OffsetPosition(X_MARGIN, 2);
//            layout_geo.OffsetSize(-2*X_MARGIN, -2*Y_MARGIN);
//        }
        GetCompositionLayout()->setGeometry(layout_geo);
    }
}

long GroupBox2::PostLayoutManagement(long LayoutResult)
{
    // A Group box must tightly group its children. 
    // So it must embrace the size that was compute for the composition layout.
    // Only the size is change is important here of the GroupBox2 is important here.

    long ret = 0;
    Geometry old_geo = BaseObject::GetGeometry();
    if(GetCompositionLayout().IsValid())
    {
        Geometry base = GetCompositionLayout()->GetGeometry();
        //if(bCaptionAvailable)
        {
            base.OffsetPosition(-X_MARGIN, -TOP_HEADER_HEIGHT - Y_MARGIN);
            base.OffsetSize(2*X_MARGIN, TOP_HEADER_HEIGHT+2*Y_MARGIN);
        }
//        else
//        {
//            base.OffsetPosition(-X_MARGIN, -2);
//            base.OffsetSize(2*X_MARGIN, 2*Y_MARGIN);
//        }
        BaseObject::setGeometry(base);
    }
    Geometry base = GetGeometry();
    m_CaptionArea->SetBaseXY(base.x + CAPTION_X_MARGIN, base.y + (TOP_HEADER_HEIGHT - m_CaptionArea->GetBaseHeight()) / 2);


    if(old_geo.GetWidth() > base.GetWidth())
        ret |= eLargerWidth;
    else if(old_geo.GetWidth() < base.GetWidth())
        ret |= eSmallerWidth;
    else
        ret |= eCompliantWidth;

    if(old_geo.GetHeight() > base.GetHeight())
        ret |= eLargerHeight;
    else if(old_geo.GetHeight() < base.GetHeight())
        ret |= eSmallerHeight;
    else
        ret |= eCompliantHeight;

    return ret;
}

void GroupBox2::PositionChildLayout(float offsetX, float offsetY)
{
    if(GetCompositionLayout().IsValid())
    {
        //if(bCaptionAvailable)
        {
            GetCompositionLayout()->SetBaseX(GetBaseX() + X_MARGIN);
            GetCompositionLayout()->SetBaseY(GetBaseY() + TOP_HEADER_HEIGHT + Y_MARGIN);
        }
//        else
//        {
//            m_compositionLayout->SetX(GetX() + X_MARGIN);
//            m_compositionLayout->SetY(GetY() + Y_MARGIN);
//        }
        GetCompositionLayout()->ComputePosition2(offsetX, offsetY);
    }
    Geometry base = GetGeometry();
    m_CaptionArea->SetBaseXY(base.x + CAPTION_X_MARGIN, base.y + (TOP_HEADER_HEIGHT - m_CaptionArea->GetBaseHeight()) / 2);
}

void GroupBox2::setCaption(const TCHAR* Caption)
{
    if((Caption == 0) || (StringLength(Caption) == 0))
    {
        //bCaptionAvailable = false;
        m_CaptionArea->SetBaseString(TEXT(""));
        m_CaptionArea->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
        m_CaptionArea->SetBaseSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    }
    else
    {
        //bCaptionAvailable = true;
        m_CaptionArea->SetBaseString(Caption);
        m_CaptionArea->SetMinimumSize(4 + GFontBold->GetStringWidth(Caption), PRACTICAL_WIDGET_HEIGHT);
        m_CaptionArea->SetBaseSize(4 + GFontBold->GetStringWidth(Caption), PRACTICAL_WIDGET_HEIGHT);

        Size s = GetMinimumSize();
        if(s.GetWidth() < 2*CAPTION_X_MARGIN + m_CaptionArea->GetBaseWidth())
        {
            SetMinimumSize(2*CAPTION_X_MARGIN + m_CaptionArea->GetBaseWidth(), s.GetHeight());
            SetBaseSize(2*CAPTION_X_MARGIN + m_CaptionArea->GetBaseWidth(), s.GetHeight());
        }
    }
}

NAMESPACE_END_GUI
