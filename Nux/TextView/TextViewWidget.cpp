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
#include "TextViewWidget.h"

#include "HScrollBar.h"
#include "VScrollBar.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "Button.h"

namespace nux { //NUX_NAMESPACE_BEGIN

TextViewWidget::TextViewWidget(NUX_FILE_LINE_DECL)
:   ActiveInterfaceObject(NUX_FILE_LINE_PARAM)
,   m_ContentOffsetX(0)
,   m_ContentOffsetY(0)
,   m_vertical_scrollbar_enable(true)
,   m_horizontal_scrollbar_enable(true)
,   m_bSizeMatchContent(false)
,   m_TextureIndex(0)
,   m_ReformatTexture(true)
//m_containerWidth(0),
//m_containerHeight(0)
{
    //gPainter.CreateBackgroundTexture(m_BackgroundTexture);
    hscrollbar = new HScrollBar;
    vscrollbar = new VScrollBar;

    SetMinimumSize(30, 30);
    setGeometry(Geometry(0, 0, 400, 200));

//     hscrollbar->OnScrollLeft.connect( sigc::mem_fun(this, &TextViewWidget::ScrollLeft));
//     hscrollbar->OnScrollRight.connect( sigc::mem_fun(this, &TextViewWidget::ScrollRight));
//     vscrollbar->OnScrollUp.connect( sigc::mem_fun(this, &TextViewWidget::ScrollUp));
//     vscrollbar->OnScrollDown.connect( sigc::mem_fun(this, &TextViewWidget::ScrollDown));

    setTopBorder(4);
    setBorder(4);

    m_ViewContentLeftMargin      = 0;
    m_ViewContentRightMargin     = 0;
    m_ViewContentTopMargin       = 0;
    m_ViewContentBottomMargin    = 0;
    FormatContent();

    m_FrameBufferObject = GetThreadGLDeviceFactory()->CreateFrameBufferObject();
    m_TextureBuffer[0] = GetThreadGLDeviceFactory()->CreateTexture(m_ViewWidth, m_ViewHeight, 1, BITFMT_R8G8B8A8);
    m_TextureBuffer[1] = GetThreadGLDeviceFactory()->CreateTexture(m_ViewWidth, m_ViewHeight, 1, BITFMT_D24S8);

    m_FrameBufferObject->FormatFrameBufferObject(m_ViewWidth, m_ViewHeight, BITFMT_R8G8B8A8);
    m_FrameBufferObject->SetRenderTarget( 0, m_TextureBuffer[0]->GetSurfaceLevel(0) );
    m_FrameBufferObject->SetDepthSurface( 0 );

    smptr(VLayout) layout = smptr(VLayout)(new VLayout);
    //layout->AddActiveInterfaceObject(new Button(TEXT("Hello")));
    SetCompositionLayout(layout);

    m_TextFont.reset(new FontTexture(TEXT("Courier_size_10.txt")));
}

TextViewWidget::~TextViewWidget()
{
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
}

long TextViewWidget::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;

    ret = vscrollbar->ProcessEvent(ievent, ret, ProcessEventInfo);
    ret = hscrollbar->ProcessEvent(ievent, ret, ProcessEventInfo);

    ret = GetCompositionLayout()->ProcessEvent(ievent, ret, ProcessEventInfo);

    // PostProcessEvent2 must always have its last parameter set to 0
    // because the m_BackgroundArea is the real physical limit of the window.
    // So the previous test about IsPointInside do not prevail over m_BackgroundArea 
    // testing the evnt by itself.
    ret = PostProcessEvent2(ievent, ret, 0);
    return ret;
}

void TextViewWidget::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    GfxContext.PushClippingRectangle(GetGeometry());

    if(m_vertical_scrollbar_enable)
    {
        vscrollbar->Draw(GfxContext, force_draw);
    }
    if(m_horizontal_scrollbar_enable)
    {
        hscrollbar->Draw(GfxContext, force_draw);
    }

    GfxContext.PopClippingRectangle();
}

void TextViewWidget::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    vscrollbar->Draw(GfxContext, force_draw);
    hscrollbar->Draw(GfxContext, force_draw);

    GetCompositionLayout()->ProcessDraw(GfxContext, force_draw);
}
/////////
// API //
/////////
void TextViewWidget::EnableVerticalScrollBar(bool b)
{
    m_vertical_scrollbar_enable = b;
    m_ContentOffsetY = 0;
    ComputeChildLayout();
}

void TextViewWidget::EnableHorizontalScrollBar(bool b)
{
    m_horizontal_scrollbar_enable = b;
    m_ContentOffsetX = 0;
    ComputeChildLayout();
}

///////////////////////
// Internal function //
///////////////////////
void TextViewWidget::setBorder(int border)
{
    m_border = border;
}

void TextViewWidget::setTopBorder(int top_border)
{
    m_top_border = top_border;
}

int TextViewWidget::getBorder() const
{
    return m_border;
}

int TextViewWidget::getTopBorder() const
{
    return m_top_border;
}

void TextViewWidget::setGeometry(const Geometry& geo)
{
    BaseObject::setGeometry(geo);
    //ComputeChildLayout();
}

void TextViewWidget::FormatContent()
{
    Geometry geo;
    geo = GetGeometry();

    ComputeChildLayout();
}

void TextViewWidget::PreLayoutManagement()
{
    //    if(IsSizeMatchContent())
    //    {
    //        PreLayoutManagement2();
    //        return;
    //    }

    // Give the managed layout the same size and position as the Control.

    Geometry geo = GetGeometry();
    int w = 0;
    int h = 0;

    w = vscrollbar->GetBaseWidth();
    h = hscrollbar->GetBaseHeight();

    m_ViewX = GetBaseX() + m_border + m_ViewContentLeftMargin;
    m_ViewY = GetBaseY() + m_top_border + m_ViewContentTopMargin;

    if(m_vertical_scrollbar_enable == false)
        m_ViewWidth = GetBaseWidth() - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    else
        m_ViewWidth = GetBaseWidth() - w - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;

    if(m_horizontal_scrollbar_enable == false)
        m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;
    else
        m_ViewHeight = GetBaseHeight() - h - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;


    if(m_ViewX + m_ContentOffsetX +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
    {
        // The position of the end of the content is smaller than the view right border position
        // Compute m_ContentOffsetX so the end of the content match exactly the view right border position
        //m_ContentOffsetX = -(m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
    }
    if(m_ViewY + m_ContentOffsetY + m_ViewContentHeight < m_ViewY + m_ViewHeight)
    {       
        // The position of the end of the content is smaller than the view right border position
        // Compute m_ContentOffsetY so the end of the content match exactly the view right border position
        //m_ContentOffsetY = -(m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
    }
    if(m_CompositionLayout.IsValid() && (m_CompositionLayout->GetStretchFactor() != 0))
    {
        m_CompositionLayout->setGeometry(
            m_ViewX + m_ContentOffsetX,
            m_ViewY + m_ContentOffsetY,
            m_ViewWidth,
            m_ViewHeight);
    }

    // Horizontal scrollbar Geometry
    if(m_horizontal_scrollbar_enable)
    {
        if(m_vertical_scrollbar_enable == false)
            hscrollbar->SetBaseWidth(GetBaseWidth() - 2*m_border);
        else
            hscrollbar->SetBaseWidth(GetBaseWidth() - w - 2*m_border);

        hscrollbar->SetBaseX(geo.x + m_border);
        hscrollbar->SetBaseY(geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
        hscrollbar->ComputeChildLayout();
    }
    else
    {
        hscrollbar->SetBaseWidth(GetBaseWidth() - w - 2*m_border);
        hscrollbar->SetBaseX(geo.x + m_border);
        hscrollbar->SetBaseY(geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
        hscrollbar->ComputeChildLayout();
    }


    // Vertical scrollbar Geometry
    if(m_vertical_scrollbar_enable)
    {
        if(m_horizontal_scrollbar_enable == false)
            vscrollbar->SetBaseHeight(GetBaseHeight() - m_top_border - m_border);
        else
            vscrollbar->SetBaseHeight(GetBaseHeight() - h - m_top_border - m_border);

        vscrollbar->SetBaseX(geo.x + geo.GetWidth() - w - m_border);
        vscrollbar->SetBaseY(geo.y + m_top_border);
        vscrollbar->ComputeChildLayout();
    }
    else
    {
        vscrollbar->SetBaseHeight(GetBaseHeight() - h - m_top_border - m_border);
        vscrollbar->SetBaseX(geo.x + geo.GetWidth() - w - m_border);
        vscrollbar->SetBaseY(geo.y + m_top_border);
        vscrollbar->ComputeChildLayout();
    }
}

void TextViewWidget::PreLayoutManagement2()
{

}

long TextViewWidget::PostLayoutManagement(long LayoutResult)
{
    if(IsSizeMatchContent())
        return PostLayoutManagement2(LayoutResult);

    if(m_CompositionLayout.IsValid())
    {
        m_ViewContentX = m_CompositionLayout->GetBaseX();
        m_ViewContentY = m_CompositionLayout->GetBaseY();
        m_ViewContentWidth = m_CompositionLayout->GetBaseWidth();
        m_ViewContentHeight = m_CompositionLayout->GetBaseHeight();
    }

    int hor_scrollbar_height = 0;
    int ver_scrollbar_width = 0;
    if(m_horizontal_scrollbar_enable == true)
        hor_scrollbar_height = hscrollbar->GetBaseHeight();
    if(m_vertical_scrollbar_enable == true)
        ver_scrollbar_width = vscrollbar->GetBaseWidth();

//     if(m_horizontal_scrollbar_enable)
//     {
//         hscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
//             GetBaseY() + m_top_border + m_ViewContentTopMargin,
//             GetBaseWidth() - ver_scrollbar_width - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
//             GetBaseHeight() - hor_scrollbar_height - m_top_border - m_border - m_ViewContentBottomMargin -m_ViewContentTopMargin);
//         if(m_CompositionLayout.IsValid())
//         {
//             hscrollbar->SetContentSize(m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
//                 m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight());
//         }
//         else
//         {
//             hscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
//                 GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
//         }
//         hscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
//     }
//     else
//     {
//         hscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
//             GetBaseY() + m_top_border + m_ViewContentTopMargin,
//             GetBaseWidth() - ver_scrollbar_width - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
//             GetBaseHeight() - hor_scrollbar_height - m_top_border - m_border - m_ViewContentBottomMargin -m_ViewContentTopMargin);
//         hscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
//             GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
//         hscrollbar->SetContentOffset(0, 0);
//     }

//     if(m_vertical_scrollbar_enable)
//     {
//         vscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
//             GetBaseY() + m_top_border + m_ViewContentTopMargin,
//             GetBaseWidth() - ver_scrollbar_width - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
//             GetBaseHeight() - hor_scrollbar_height - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
//         if(m_CompositionLayout.IsValid())
//         {
//             vscrollbar->SetContentSize(m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
//                 m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight());
//         }
//         else
//         {
//             vscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
//                 GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
//         }
//         vscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
//     }
//     else
//     {
//         vscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
//             GetBaseY() + m_top_border + m_ViewContentTopMargin,
//             GetBaseWidth() - ver_scrollbar_width - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
//             GetBaseHeight() - hor_scrollbar_height - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
//         vscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
//             GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
//         vscrollbar->SetContentOffset(0, 0);
//     }

    if(m_CompositionLayout.IsValid())
        m_CompositionLayout->ComputePosition2(0, 0);
    //return LayoutResult;
    // A TextViewWidget must kill the result of the management and pass it to the parent Layout.
    return (eCompliantHeight | eCompliantWidth);
}

long TextViewWidget::PostLayoutManagement2(long LayoutResult)
{
    int ScrollbarWidth = 0;
    int ScrollbarHeight = 0;

    if(m_horizontal_scrollbar_enable == true)
        ScrollbarHeight = hscrollbar->GetBaseHeight();
    if(m_vertical_scrollbar_enable == true)
        ScrollbarWidth = vscrollbar->GetBaseWidth();


    // We want the controller to match the size of the content as defined in:
    //      m_ViewContentX
    //      m_ViewContentY
    //      m_ViewContentWidth
    //      m_ViewContentHeight
    // So we make the composition layout the same size as the content

    if(m_CompositionLayout.IsValid())
    {
        m_CompositionLayout->SetBaseX(m_ViewContentX);
        m_CompositionLayout->SetBaseY(m_ViewContentY);
        m_CompositionLayout->SetBaseWidth(m_ViewContentWidth);
        m_CompositionLayout->SetBaseHeight(m_ViewContentHeight);
    }

    Geometry base;
    // Given the (m_ViewContentWidth, m_ViewContentHeight) compute the size of the TextViewWidget.
    // It is possible that the TextViewWidget size be limited by its min/Max dimension. If this happens, then the scrollbar will reflect that.
    base.SetX(m_ViewContentX - m_border - m_ViewContentLeftMargin);
    base.SetY(m_ViewContentY - m_top_border - m_ViewContentTopMargin);
    if(m_horizontal_scrollbar_enable)
        base.SetHeight(m_top_border + m_ViewContentTopMargin + m_ViewContentHeight + m_ViewContentBottomMargin + ScrollbarHeight + m_border);
    else
        base.SetHeight(m_top_border + m_ViewContentTopMargin + m_ViewContentHeight + m_ViewContentBottomMargin + m_border);

    if(m_vertical_scrollbar_enable)
        base.SetWidth(m_border + m_ViewContentLeftMargin + m_ViewContentWidth + m_ViewContentRightMargin + ScrollbarWidth + m_border);
    else
        base.SetWidth(m_border + m_ViewContentLeftMargin + m_ViewContentWidth + m_ViewContentRightMargin + m_border);

    // Set the size so that is is equal to the visible content.
    BaseObject::SetBaseWidth(base.GetWidth());
    BaseObject::SetBaseHeight(base.GetHeight());
    Geometry geo = GetGeometry();

    // Horizontal scrollbar Geometry
    if(m_horizontal_scrollbar_enable)
    {
        if(m_vertical_scrollbar_enable == false)
            hscrollbar->SetBaseWidth(GetBaseWidth() - 2*m_border);
        else
            hscrollbar->SetBaseWidth(GetBaseWidth() - ScrollbarWidth - 2*m_border);

        hscrollbar->SetBaseX(geo.x + m_border);
        hscrollbar->SetBaseY(geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
        hscrollbar->ComputeChildLayout();

        //---
        hscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
            GetBaseY() + m_top_border + m_ViewContentTopMargin,
            GetBaseWidth() - ScrollbarWidth - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
            GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin -m_ViewContentTopMargin);
        if(m_CompositionLayout.IsValid())
        {
            hscrollbar->SetContentSize(m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight());
        }
        else
        {
            hscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
                GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
        }
        hscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
    }
    else
    {
        hscrollbar->SetBaseWidth(GetBaseWidth() - ScrollbarWidth - 2*m_border);
        hscrollbar->SetBaseX(geo.x + m_border);
        hscrollbar->SetBaseY(geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
        hscrollbar->ComputeChildLayout();

        //---
        hscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
            GetBaseY() + m_top_border + m_ViewContentTopMargin,
            GetBaseWidth() - ScrollbarWidth - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
            GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin -m_ViewContentTopMargin);
        hscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
            GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
        hscrollbar->SetContentOffset(0, 0);
    }


    // Vertical scrollbar Geometry
    if(m_vertical_scrollbar_enable)
    {
        if(m_horizontal_scrollbar_enable == false)
            vscrollbar->SetBaseHeight(GetBaseHeight() - m_top_border - m_border);
        else
            vscrollbar->SetBaseHeight(GetBaseHeight() - ScrollbarHeight - m_top_border - m_border);

        vscrollbar->SetBaseX(geo.x + geo.GetWidth() - ScrollbarWidth - m_border);
        vscrollbar->SetBaseY(geo.y + m_top_border);
        vscrollbar->ComputeChildLayout();

        //---
        vscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
            GetBaseY() + m_top_border + m_ViewContentTopMargin,
            GetBaseWidth() - ScrollbarWidth - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
            GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
        if(m_CompositionLayout.IsValid())
        {
            vscrollbar->SetContentSize(m_CompositionLayout->GetBaseX(), m_CompositionLayout->GetBaseY(),
                m_CompositionLayout->GetBaseWidth(), m_CompositionLayout->GetBaseHeight());
        }
        else
        {
            vscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
                GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
        }
        vscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
    }
    else
    {
        vscrollbar->SetBaseHeight(GetBaseHeight() - ScrollbarHeight - m_top_border - m_border);
        vscrollbar->SetBaseX(geo.x + geo.GetWidth() - ScrollbarWidth - m_border);
        vscrollbar->SetBaseY(geo.y + m_top_border);
        vscrollbar->ComputeChildLayout();

        //---
        vscrollbar->SetContainerSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
            GetBaseY() + m_top_border + m_ViewContentTopMargin,
            GetBaseWidth() - ScrollbarWidth - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin,
            GetBaseHeight() - ScrollbarHeight - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin);
        vscrollbar->SetContentSize(GetBaseX() + m_border + m_ViewContentLeftMargin,
            GetBaseY() + m_top_border + m_ViewContentTopMargin, 0, 0);
        vscrollbar->SetContentOffset(0, 0);
    }

    if(m_CompositionLayout.IsValid())
        m_CompositionLayout->ComputePosition2(0, 0);
    return (eCompliantHeight | eCompliantWidth);
}

// When the TextViewWidget is in a Layout object, and that layout call ActiveInterfaceObject::PositionChildLayout
// the TextViewWidget must call its own PositionChildLayout so it can properly do the positioning of the inner object.
// Otherwise, m_CompositionLayout->ComputePosition2 is called but it doesn't know that it may not contain all the 
// object of the TextViewWidget. Which result in incorrect positioning.
// Here we touch only the position. Do not touch the width or height of object.
// This function is called when the TextViewWidget is embedded within a Layout.
void TextViewWidget::PositionChildLayout(float offsetX, float offsetY)
{
    Geometry geo = GetGeometry();
    int w = 0;
    int h = 0;

    w = vscrollbar->GetBaseWidth();
    h = hscrollbar->GetBaseHeight();

    //    m_ViewX = GetX() + m_border;
    //    m_ViewY = GetY() + m_top_border;
    //
    //    if(m_vertical_scrollbar_enable == false)
    //        m_ViewWidth = GetWidth() - 2*m_border - m_ViewContentRightMargin;
    //    else
    //        m_ViewWidth = GetWidth() - w - 2*m_border - m_ViewContentRightMargin;
    //
    //    if(m_horizontal_scrollbar_enable == false)
    //        m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin;
    //    else
    //        m_ViewHeight = GetBaseHeight() - h - m_top_border - m_border - m_ViewContentBottomMargin;

    m_ViewX = GetBaseX() + m_border + m_ViewContentLeftMargin;
    m_ViewY = GetBaseY() + m_top_border + m_ViewContentTopMargin;

    if(m_vertical_scrollbar_enable == false)
        m_ViewWidth = GetBaseWidth() - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;
    else
        m_ViewWidth = GetBaseWidth() - w - 2*m_border - m_ViewContentRightMargin - m_ViewContentLeftMargin;

    if(m_horizontal_scrollbar_enable == false)
        m_ViewHeight = GetBaseHeight() - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;
    else
        m_ViewHeight = GetBaseHeight() - h - m_top_border - m_border - m_ViewContentBottomMargin - m_ViewContentTopMargin;


//     if(m_ViewX + m_ContentOffsetX +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
//     {
//         m_ContentOffsetX = -(m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
//     }
//     if(m_ViewY + m_ContentOffsetY + m_ViewContentHeight < m_ViewY + m_ViewHeight)
//     {
//         m_ContentOffsetY = -(m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
//     }
//     if(m_CompositionLayout.IsValid())
//     {
//         m_CompositionLayout->SetBaseX(m_ViewX + m_ContentOffsetX);
//         m_CompositionLayout->SetBaseY(m_ViewY + m_ContentOffsetY);
//     }

    // Horizontal scrollbar Geometry
    if(m_horizontal_scrollbar_enable)
    {
        if(m_vertical_scrollbar_enable == false)
            hscrollbar->SetBaseWidth(GetBaseWidth() - 2*m_border);
        else
            hscrollbar->SetBaseWidth(GetBaseWidth() - w - 2*m_border);
        hscrollbar->SetBaseX(geo.x + m_border);
        hscrollbar->SetBaseY(geo.y + geo.GetHeight() - hscrollbar->GetBaseHeight() - m_border);
        hscrollbar->ComputeChildLayout();
    }

    // Vertical scrollbar Geometry
    if(m_vertical_scrollbar_enable)
    {
        if(m_horizontal_scrollbar_enable == false)
            vscrollbar->SetBaseHeight(GetBaseHeight() - m_top_border - m_border);
        else
            vscrollbar->SetBaseHeight(GetBaseHeight() - h - m_top_border - m_border);
        vscrollbar->SetBaseX(geo.x + geo.GetWidth() - w - m_border);
        vscrollbar->SetBaseY(geo.y + m_top_border);
        vscrollbar->ComputeChildLayout();
    }

    //////////////////////////////////////////////////////////////////////////
    if(m_CompositionLayout.IsValid())
    {
        m_ViewContentX = m_CompositionLayout->GetBaseX();
        m_ViewContentY = m_CompositionLayout->GetBaseY();
    }
    else
    {
        m_ViewContentX = m_ViewX;
        m_ViewContentY = m_ViewY;
    }

    int hor_scrollbar_height = 0;
    int ver_scrollbar_width = 0;
    if(m_horizontal_scrollbar_enable == true)
        hor_scrollbar_height = hscrollbar->GetBaseHeight();
    if(m_vertical_scrollbar_enable == true)
        ver_scrollbar_width = vscrollbar->GetBaseWidth();

    //    hscrollbar->SetContainerSize(GetX() + m_border, GetY() + m_top_border,
    //        GetWidth() - ver_scrollbar_width - 2*m_border, GetBaseHeight() - hor_scrollbar_height - m_top_border - m_border);
    //    if(m_CompositionLayout.IsValid())
    //        hscrollbar->SetContentSize(m_CompositionLayout->GetX(), m_CompositionLayout->GetY(),
    //        m_CompositionLayout->GetWidth(), m_CompositionLayout->GetBaseHeight());

    //    vscrollbar->SetContainerSize(GetX() + m_border, GetY() + m_top_border,
    //        GetWidth() - ver_scrollbar_width - 2*m_border, GetBaseHeight() - hor_scrollbar_height - m_top_border - m_border);
    //    if(m_CompositionLayout.IsValid())
    //        vscrollbar->SetContentSize(m_CompositionLayout->GetX(), m_CompositionLayout->GetY(),
    //        m_CompositionLayout->GetWidth(), m_CompositionLayout->GetBaseHeight());

    vscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
    hscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);

    if(m_CompositionLayout.IsValid())
    {
        m_CompositionLayout->ComputePosition2(0, 0);
    }
}


// void TextViewWidget::ScrollLeft(float stepx, int mousedx)
// {
//     if(m_CompositionLayout.IsValid())
//     {
//         m_ContentOffsetX += (float)stepx * (float)mousedx;;
//         if(m_ContentOffsetX > 0)
//         {
//             m_ContentOffsetX = 0;
//             m_CompositionLayout->SetBaseX(m_ViewX + m_ContentOffsetX);
//         }
//         else
//         {
//             m_CompositionLayout->SetBaseX(m_ViewX + m_ContentOffsetX);
//         }
//         m_CompositionLayout->ComputePosition2(0, 0);
//         hscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
//     }
//     hscrollbar->NeedRedraw();
//     NeedRedraw();
// }
// 
// void TextViewWidget::ScrollRight(float stepx, int mousedx)
// {
//     if(m_CompositionLayout.IsValid())
//     {
//         m_ContentOffsetX -= (float)stepx * (float)mousedx;
//         if(m_ViewX + m_ContentOffsetX +  m_ViewContentWidth < m_ViewX + m_ViewWidth)
//         {
//             m_ContentOffsetX = -(m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
//             m_CompositionLayout->SetBaseX(m_ViewX + m_ContentOffsetX);
//         }
//         else
//         {
//             m_CompositionLayout->SetBaseX(m_ViewX + m_ContentOffsetX);
//         }
//         m_CompositionLayout->ComputePosition2(0, 0);
//         hscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
//     }
//     hscrollbar->NeedRedraw();
//     NeedRedraw();
// }
// 
// void TextViewWidget::ScrollUp(float stepy, int mousedy)
// {
//     if(m_CompositionLayout.IsValid())
//     {
//         m_ContentOffsetY += (float)stepy * (float)mousedy;
//         if(m_ContentOffsetY > 0)
//         {
//             m_ContentOffsetY = 0;
//             m_CompositionLayout->SetBaseY(m_ViewY + m_ContentOffsetY);
// 
//         }
//         else
//         {
//             m_CompositionLayout->SetBaseY(m_ViewY + m_ContentOffsetY);
//         }
// 
//         m_CompositionLayout->ComputePosition2(0, 0);
//         //m_CompositionLayout->Translate(m_ContentOffsetX, m_ContentOffsetY);
//         vscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
//     }
//     vscrollbar->NeedRedraw();
//     NeedRedraw();
// }
// 
// void TextViewWidget::ScrollDown(float stepy, int mousedy)
// {
//     if(m_CompositionLayout.IsValid())
//     {
//         m_ContentOffsetY -= (float)stepy * (float)mousedy;
//         if(m_ViewY + m_ContentOffsetY + m_ViewContentHeight < m_ViewY + m_ViewHeight)
//         {
//             m_ContentOffsetY = -(m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
//             m_CompositionLayout->SetBaseY(m_ViewY + m_ContentOffsetY);
//         }
//         else
//         {
//             m_CompositionLayout->SetBaseY(m_ViewY + m_ContentOffsetY);
//         }
// 
//         m_CompositionLayout->ComputePosition2(0, 0);
//         //m_CompositionLayout->Translate(m_ContentOffsetX, m_ContentOffsetY);
//         vscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
//     }
//     vscrollbar->NeedRedraw();
//     NeedRedraw();
// }

void TextViewWidget::SetSizeMatchContent(bool b)
{
    m_bSizeMatchContent = b;
    if(m_CompositionLayout.IsValid())
        m_CompositionLayout->ComputeLayout2();
}

bool TextViewWidget::IsSizeMatchContent() const
{
    return m_bSizeMatchContent;
}

void TextViewWidget::ResetScrollToLeft()
{
    if(m_CompositionLayout.IsValid())
    {
        m_ContentOffsetX = 0;
        m_CompositionLayout->SetBaseX(m_ViewX + m_ContentOffsetX);
        m_CompositionLayout->ComputePosition2(0, 0);
        hscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
    }
    hscrollbar->NeedRedraw();
    NeedRedraw();
}

void TextViewWidget::ResetScrollToRight()
{
    if(m_CompositionLayout.IsValid())
    {
        m_ContentOffsetX = -(m_ViewContentWidth > m_ViewWidth ? m_ViewContentWidth - m_ViewWidth : 0);
        m_CompositionLayout->SetBaseX(m_ViewX + m_ContentOffsetX);
        m_CompositionLayout->ComputePosition2(0, 0);
        hscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
    }
    hscrollbar->NeedRedraw();
    NeedRedraw();
}

void TextViewWidget::ResetScrollToUp()
{
    if(m_CompositionLayout.IsValid())
    {
        m_ContentOffsetY = 0;
        m_CompositionLayout->SetBaseY(m_ViewY);
        m_CompositionLayout->ComputePosition2(0, 0);
        vscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
    }
    vscrollbar->NeedRedraw();
    NeedRedraw();
}

void TextViewWidget::ResetScrollToDown()
{
    if(m_CompositionLayout.IsValid())
    {
        m_ContentOffsetY = -(m_ViewContentHeight > m_ViewHeight ? m_ViewContentHeight - m_ViewHeight : 0);
        m_CompositionLayout->SetBaseY(m_ViewY + m_ContentOffsetY);
        m_CompositionLayout->ComputePosition2(0, 0);
        vscrollbar->SetContentOffset(m_ContentOffsetX, m_ContentOffsetY);
    }
    vscrollbar->NeedRedraw();
    NeedRedraw();
}

 } //NUX_NAMESPACE_END
