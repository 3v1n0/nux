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

#include "NuxGraphics/GLTextureResourceManager.h"

#include "Layout.h"
#include "HLayout.h"
#include "WindowCompositor.h"
#include "BaseWindow.h"

NAMESPACE_BEGIN_GUI

const int SizeGripWidth = 20;
const int SizeGripHeight = 20;
const int TitleBarHeight = 20;

/*
    Elements inside the Window have coordinates based on the top-left corner of the window.
    This is true whether we are drawing or computing the layout. When computing the layout, use x_root and y_root to
    pass the top-left corner position of the window. When drawing, make a similar adjustment.
*/

BaseWindow::BaseWindow(const TCHAR* WindowName)
:   m_bIsVisible(false)
,   m_bSizeMatchLayout(false)
,   m_bIsModal(false)
,   m_Border(0)
,   m_TopBorder(0)
,   m_ConfigureNotifyCallback(0)
,   m_BackgroundColor(Color(0xFF707070))
,   m_BluredBackground(false)
{
    // Should be at the end of the constructor
    GetThreadWindowCompositor().RegisterWindow(smptr(BaseWindow)(this, false));

    SetMinimumSize(1, 1);
    setGeometry(Geometry(100, 100, 320, 200));

    m_PaintLayer = new ColorLayer(Color(0xFFFF7070));
}

BaseWindow::~BaseWindow()
{
    GetThreadWindowCompositor().UnRegisterWindow(smptr(BaseWindow)(this, false));
    m_InterfaceObject.clear();
    INL_SAFE_DELETE(m_PaintLayer);
}

long BaseWindow::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;
    long ProcEvInfo = 0;

    IEvent window_event = ievent;
    Geometry base = GetGeometry();
    window_event.e_x_root = base.x;
    window_event.e_y_root = base.y;

    if(ievent.e_event == INL_MOUSE_PRESSED)
    {
        if(!m_Geometry.IsPointInside(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root))
        {
            ProcEvInfo = eDoNotProcess;
            //return TraverseInfo;
        }
    }

//    if(m_vertical_scrollbar_enable)
//        ret = vscrollbar->ProcessEvent(ievent, ret, ProcEvInfo);
//    if(m_horizontal_scrollbar_enable)
//        ret = hscrollbar->ProcessEvent(ievent, ret, ProcEvInfo);

    if(IsSizeMatchContent() == false)
    {

    }

    // The child layout get the Mouse down button only if the MouseDown happened inside the client view Area
    Geometry viewGeometry = GetGeometry(); //Geometry(m_ViewX, m_ViewY, m_ViewWidth, m_ViewHeight);
    bool traverse = true;
    if(ievent.e_event == INL_MOUSE_PRESSED)
    {
        if(!viewGeometry.IsPointInside(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root))
        {
            ProcEvInfo = eDoNotProcess;
            traverse = false;
        }
    }

//    {   std::list<smptr(ActiveInterfaceObject)>::iterator it;
//        for(it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
//        {
//            ret = (*it)->ProcessEvent(ievent, ret, ProcEvInfo);
//        }
//    }
    if(m_layout.IsValid())
        ret = m_layout->ProcessEvent(window_event, ret, ProcEvInfo);

    // PostProcessEvent2 must always have its last parameter set to 0
    // because the m_BackgroundArea is the real physical limit of the window.
    // So the previous test about IsPointInside do not prevail over m_BackgroundArea 
    // testing the event by itself.
    ret = PostProcessEvent2(ievent, ret, 0);
    return ret;
}

void BaseWindow::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    int x = base.x;
    int y = base.y;
    // The elements position inside the window are referenced to top-left window corner. So bring base to (0, 0).
    base.SetX(0);
    base.SetY(0);
    GfxContext.PushClippingRectangle(base);
    
    int window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    int window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();

    if(UseBlurredBackground())
    {
        TexCoordXForm texxform;
        texxform.uoffset = (float)x/(float)GetThreadWindowCompositor().GetScreenBlurTexture()->GetWidth();
        texxform.voffset = (float)y/(float)GetThreadWindowCompositor().GetScreenBlurTexture()->GetHeight();
        texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);

        gPainter.PushDrawTextureLayer(GfxContext, base, GetThreadWindowCompositor().GetScreenBlurTexture(), texxform, Color::White, true);
    }
    else
    {
        gPainter.PushDrawShapeLayer(GfxContext, base, eSHAPE_CORNER_ROUND10, m_BackgroundColor, eAllCorners, true);
    }
    gPainter.PopBackground();
    GfxContext.PopClippingRectangle();
}

void BaseWindow::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    int x = base.x;
    int y = base.y;
    // The elements position inside the window are referenced to top-left window corner. So bring base to (0, 0).
    base.SetX(0);
    base.SetY(0);
    
    int window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    int window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();

    if(UseBlurredBackground())
    {
        TexCoordXForm texxform;
        texxform.uoffset = (float)x/(float)GetThreadWindowCompositor().GetScreenBlurTexture()->GetWidth();
        texxform.voffset = (float)y/(float)GetThreadWindowCompositor().GetScreenBlurTexture()->GetHeight();
        texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);

        gPainter.PushTextureLayer(GfxContext, base, GetThreadWindowCompositor().GetScreenBlurTexture(), texxform, Color::White, true);
    }
    else
    {
        gPainter.PushShapeLayer(GfxContext, base, eSHAPE_CORNER_ROUND10, m_BackgroundColor, eAllCorners, true);
    }

    if(m_layout.IsValid())
    {
        GfxContext.PushClippingRectangle(base);
        m_layout->ProcessDraw(GfxContext, force_draw);
        GfxContext.PopClippingRectangle();
    }
    gPainter.PopBackground();
}

void BaseWindow::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void BaseWindow::AddWidget(int id)
{
//    smptr(ActiveInterfaceObject) ic = InterfaceControlFactory::Instance().CreateObject(id);
//    if(ic.IsValid() && m_layout.IsValid())
//    {
//        m_layout->AddActiveInterfaceObject(ic, 0);
//        // 0: the WidgetLayout geometry will be set to setGeometry(0,0,1,1);
//        // and the children will take their natural size by expending WidgetLayout.
//        // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
//
//        m_InterfaceObject.push_back(ic);
//
//        //FormatContent();
//        ComputeChildLayout();
//    }
}

void BaseWindow::AddWidget(smptr(ActiveInterfaceObject) ic)
{
    if(ic.IsValid() && m_layout.IsValid())
    {
        m_layout->AddActiveInterfaceObject(ic, 0);
        // 0: the WidgetLayout geometry will be set to setGeometry(0,0,1,1);
        // and the children will take their natural size by expending WidgetLayout.
        // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.

        m_InterfaceObject.push_back(ic);

        //FormatContent();
        ComputeChildLayout();
    }
}

void BaseWindow::AddWidget(smptr(ActiveInterfaceObject) ic, int stretchfactor)
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

void BaseWindow::AddWidget(std::list<smptr(ActiveInterfaceObject)> *InterfaceControlList)
{
    m_CompositionLayout->Clear();
    m_InterfaceObject.clear();

    std::list<smptr(ActiveInterfaceObject)>::iterator it;
    
    for(it = InterfaceControlList->begin(); it != InterfaceControlList->end(); it++)
    {
        AddWidget((*it));
    }
}

void BaseWindow::setLayout(smptr(Layout) layout)
{
    if(layout == 0)
        return;

    m_InterfaceObject.clear();
    std::list<smptr(BaseObject)> *InterfaceControlList = new std::list<smptr(BaseObject)>;

    m_layout = layout;
    SetCompositionLayout(m_layout);

    // Get the list of ActiveInterfaceObject that is in the m_layout and its lower hierarchy of layout.
    // Go down to the WidgetLayout to get every ActiveInterfaceObject Obbject.
    m_layout->GetCompositeList(InterfaceControlList);

    std::list<smptr(BaseObject)>::iterator it;
    for(it = InterfaceControlList->begin(); it != InterfaceControlList->end(); it++)
    {
        m_InterfaceObject.push_back((*it));
    }

    Geometry geo = GetGeometry();
    Geometry layout_geo = Geometry(geo.x + m_Border, geo.y + m_TopBorder,
        geo.GetWidth() - 2*m_Border, geo.GetHeight() - m_Border - m_TopBorder);
    m_layout->setGeometry(layout_geo);
    ComputeChildLayout();
    delete InterfaceControlList;
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
void BaseWindow::PreLayoutManagement()
{
    Geometry geo = GetGeometry();
    if(m_ConfigureNotifyCallback)
    {
        (*m_ConfigureNotifyCallback)(GetThreadGLWindow()->GetWindowWidth(), GetThreadGLWindow()->GetWindowHeight(), geo);
        if(geo.IsNull())
        {
            nuxDebugMsg(TEXT("[BaseWindow::PreLayoutManagement] Received an invalid Geometry."));
            geo = GetGeometry();
        }
        else
        {
            BaseObject::setGeometry(geo);
            // Get the geometry adjusted with respect to min and max dimension of this area.
            geo = GetGeometry();
        }
    }

    if(m_layout.IsValid())
    {
        Geometry layout_geo = Geometry(m_Border, m_TopBorder,
            geo.GetWidth() - 2*m_Border, geo.GetHeight() - m_Border - m_TopBorder);
        m_layout->setGeometry(layout_geo);
    }
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
long BaseWindow::PostLayoutManagement(long LayoutResult)
{
    if(IsSizeMatchContent() && m_layout.IsValid())
    {
        Geometry layout_geometry = m_layout->GetGeometry();

        Geometry WindowGeometry = Geometry(GetGeometry().x,
            GetGeometry().y,
            layout_geometry.GetWidth() + 2*m_Border,
            layout_geometry.GetHeight() + m_Border + m_TopBorder);

        BaseObject::setGeometry(WindowGeometry);
    }

    // A BaseWindow must kill the result of the management and pass it to the parent Layout.
    return (eCompliantHeight | eCompliantWidth);
    //return result;
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
void BaseWindow::PositionChildLayout(float offsetX, float offsetY)
{

}

void BaseWindow::setGeometry(const Geometry& geo)
{
    BaseObject::setGeometry(geo);

    //LayoutWindowElements();
    //ComputeChildLayout();
}

void BaseWindow::LayoutWindowElements()
{
    // Define the geometry of some of the component of the window. Otherwise, if the composition layout is not set,
    // then the component won't be correctly placed after a setGeometry. This can be redondant if the composition layout is set.
    Geometry base = GetGeometry();
    //GetGraphicsThread()->ComputeElementLayout(m_TitleBarLayout);
}

void BaseWindow::SetBorder(int border)
{
    if(m_Border != border)
    {
        m_Border = border;
    }
}

void BaseWindow::SetTopBorder(int border)
{
    if(m_TopBorder != border)
    {
        m_TopBorder = border;
    }
}

int BaseWindow::GetBorder() const
{
    return m_Border;
}

int BaseWindow::GetTopBorder() const
{
    return m_TopBorder;
}

void BaseWindow::ShowWindow(bool b, bool StartModal /*  = false */)
{
//    if(m_bIsModal)
//        return;

    if(m_layout.IsValid())
    {
        m_layout->setGeometry(GetGeometry());
    }

    m_bIsVisible = b;
    m_bIsModal = StartModal;

    ComputeChildLayout();

    if(m_bIsVisible)
    {
        NeedRedraw();
    }

    if(m_bIsModal)
        GetThreadWindowCompositor().StartModalWindow(smptr(BaseWindow)(this, false));
}

bool BaseWindow::IsVisible() const
{
    return m_bIsVisible;
}

void BaseWindow::StopModal()
{
    m_bIsVisible = false;
    m_bIsModal = false;
    //ShowWindow(false);
    GetThreadWindowCompositor().StopModalWindow(smptr(BaseWindow)(this, false));
}

bool BaseWindow::IsModal() const
{
    return m_bIsModal;
}

void BaseWindow::NotifyConfigurationChange(int Width, int Height)
{
    Geometry geo = GetGeometry();
    if(m_ConfigureNotifyCallback)
    {
        (*m_ConfigureNotifyCallback)(GetThreadGLWindow()->GetWindowWidth(), GetThreadGLWindow()->GetWindowHeight(), geo);
        if(geo.IsNull())
        {
            nuxDebugMsg(TEXT("[BaseWindow::NotifyConfigurationChange] Received an invalid Geometry."));
            geo = GetGeometry();
        }
        else
        {
            BaseObject::setGeometry(geo);
            // Get the geometry adjusted with respect to min and max dimension of this area.
            geo = GetGeometry();
        }
    }
    else
    {
        return;
    }
}

void BaseWindow::SetBackgroundLayer(AbstractPaintLayer* layer)
{
    INL_SAFE_DELETE(m_PaintLayer);
    m_PaintLayer = layer->Clone();
}

void BaseWindow::SetBackgroundColor(const Color& color)
{
    m_BackgroundColor = color;
}

NAMESPACE_END_GUI
