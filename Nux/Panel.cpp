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
#include "WindowCompositor.h"
#include "VScrollBar.h"
#include "HScrollBar.h"
#include "Panel.h"

NAMESPACE_BEGIN_GUI

Panel::Panel()
:   m_layout(0)
{ 
    //vlayout = new VLayout();
    //vlayout->SetVerticalInternalMargin(4);

    setTopBorder(0);
    setBorder(0);

    //SetMinimumSize(200, 200);
    
    //SetCompositionLayout(vlayout);
    //FormatContent();
}

Panel::~Panel()
{
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
    //delete vlayout;
    m_layout = smptr(Layout)(0);
}

long Panel::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;
    long ProcEvInfo = 0;
    if(ievent.e_event == INL_MOUSE_PRESSED)
    {
        if(!m_Geometry.IsPointInside(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root))
        {
            ProcEvInfo = eDoNotProcess;
            //return TraverseInfo;
        }
    }

    if(m_vertical_scrollbar_enable)
        ret = vscrollbar->ProcessEvent(ievent, ret, ProcEvInfo);
    if(m_horizontal_scrollbar_enable)
        ret = hscrollbar->ProcessEvent(ievent, ret, ProcEvInfo);

    // The child layout get the Mouse down button only if the MouseDown happened inside the client view Area
    Geometry viewGeometry = Geometry(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight);
    bool traverse = true;
    if(ievent.e_event == INL_MOUSE_PRESSED)
    {
        if(!viewGeometry.IsPointInside(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root))
        {
            ProcEvInfo = eDoNotProcess;
            traverse = false;
        }
    }
    GetThreadWindowCompositor().PushEventRectangle(viewGeometry);
    if(m_layout.IsValid())
        ret = m_layout->ProcessEvent(ievent, ret, ProcEvInfo);
    GetThreadWindowCompositor().PopEventRectangle();

    ret = PostProcessEvent2(ievent, ret, 0);
    return ret;
}

void Panel::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    GfxContext.PushClippingRectangle(GetGeometry());

    Geometry base = GetGeometry();

    if(m_layout.IsValid())
        m_layout->NeedRedraw();

    gPainter.PaintBackground(GfxContext, base);

    if(m_vertical_scrollbar_enable)
    {
        vscrollbar->NeedRedraw();
    }
    if(m_horizontal_scrollbar_enable)
    {
        hscrollbar->NeedRedraw();
    }

    GfxContext.PopClippingRectangle();
}

void Panel::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    GfxContext.PushClippingRectangle(GetGeometry());

    GfxContext.PushClippingRectangle(Rect(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight));
//    std::list<smptr(ActiveInterfaceObject)>::reverse_iterator it = m_InterfaceObject.rbegin();
//    while (it != m_InterfaceObject.rend())
//    {
//        (*it)->ProcessDraw(force_draw);
//        ++it;
//    }
    if(m_layout.IsValid())
    {
        GfxContext.PushClippingRectangle(m_layout->GetGeometry());
        m_layout->ProcessDraw(GfxContext, force_draw);
        GfxContext.PopClippingRectangle();
    }

    GfxContext.PopClippingRectangle();

    if(m_vertical_scrollbar_enable)
    {
        vscrollbar->ProcessDraw(GfxContext, force_draw);
    }
    if(m_horizontal_scrollbar_enable)
    {
        hscrollbar->ProcessDraw(GfxContext, force_draw);
    }

    GfxContext.PopClippingRectangle();
}

void Panel::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

// 
// void Panel::AddWidget(smptr(ActiveInterfaceObject) ic)
// {
//     if(ic && m_layout)
//     {
//         m_layout->AddActiveInterfaceObject(ic, 1);
//         // 0: the WidgetLayout geometry will be set to setGeometry(0,0,1,1);
//         // and the children will take their natural size by expending WidgetLayout.
//         // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
// 
//         m_InterfaceObject.push_back(ic);
// 
//         //FormatContent();
//         ComputeChildLayout();
//     }
// }

void Panel::AddWidget(smptr(ActiveInterfaceObject) ic, int stretchfactor)
{
   if(ic.IsValid() && m_layout.IsValid())
   {
       m_layout->AddActiveInterfaceObject(ic, stretchfactor);
       // if(stretchfactor ==0): the WidgetLayout geometry will be set to setGeometry(0,0,1,1);
       // and the children will take their natural size by expending WidgetLayout.
       // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.

       m_InterfaceObject.push_back(ic);

       //FormatContent();
       ComputeChildLayout();
   }
}

void Panel::AddWidget(std::list<smptr(ActiveInterfaceObject)> *InterfaceControlList)
{
    //m_compositionLayout->Clear();
    //m_InterfaceObject.clear();

    std::list<smptr(ActiveInterfaceObject)>::iterator it;
    
    for(it = InterfaceControlList->begin(); it != InterfaceControlList->end(); it++)
    {
        AddWidget((*it));
    }
}

void Panel::SetLayout(smptr(Layout) layout)
{
    if(layout == 0)
        return;

    m_InterfaceObject.clear();
    std::list<smptr(BaseObject)> *InterfaceControlList = new std::list<smptr(BaseObject)>;

    m_layout = layout;

    SetCompositionLayout(m_layout);

    // Get the list of ActiveInterfaceObject that is in the m_layout and its lower hierarchy of layout.
    // Go down to the WidgetLayout to get every ActiveInterfaceObject Object.
    m_layout->GetCompositeList(InterfaceControlList);

    std::list<smptr(BaseObject)>::iterator it;
    for(it = InterfaceControlList->begin(); it != InterfaceControlList->end(); it++)
    {
        m_InterfaceObject.push_back((*it));
    }
    FormatContent();

    delete InterfaceControlList;
}

void Panel::clearContent()
{
    m_layout->Clear();
    m_InterfaceObject.clear();
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
void Panel::PreLayoutManagement()
{
    ScrollView::PreLayoutManagement();
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
long Panel::PostLayoutManagement(long LayoutResult)
{
    long result = ScrollView::PostLayoutManagement(LayoutResult);
    return result;
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
void Panel::PositionChildLayout(float offsetX, float offsetY)
{
    ScrollView::PositionChildLayout(offsetX, offsetY);
}

void Panel::ScrollLeft(float stepx, int mousedx)
{
    ScrollView::ScrollLeft(stepx, mousedx);
    ComputeChildLayout();    
    NeedRedraw();
}

void Panel::ScrollRight(float stepx, int mousedx)
{
    ScrollView::ScrollRight(stepx, mousedx);
    ComputeChildLayout();
    NeedRedraw();
}

void Panel::ScrollUp(float stepy, int mousedy)
{
    ScrollView::ScrollUp(stepy, mousedy);
    ComputeChildLayout();
    NeedRedraw();
}

void Panel::ScrollDown(float stepy, int mousedy)
{
    ScrollView::ScrollDown(stepy, mousedy);
    ComputeChildLayout();
    NeedRedraw();
}

NAMESPACE_END_GUI
