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

#include "HLayout.h"
#include "WindowThread.h"
#include "WindowCompositor.h"
#include "FloatingWindow.h"

NAMESPACE_BEGIN_GUI

const int SizeGripWidth = 20;
const int SizeGripHeight = 20;
const int TitleBarHeight = 20;

/*
    Elements inside the Window have coordinates based on the top-left corner of the window.
    This is true whether we are drawing or computing the layout. When computing the layout, use x_root and y_root to
    pass the top-left corner position of the window. When drawing, make a similar adjustment.
*/

FloatingWindow::FloatingWindow(const TCHAR* WindowName)
:   m_WindowTitle(0)
,   m_bIsVisible(false)
,   m_bSizeMatchLayout(false)
,   m_bIsModal(false)
,   m_bIsVisibleSizeGrip(true)
,   m_SizeGripDragPositionX(0)
,   m_SizeGripDragPositionY(0)
,   m_hasTitleBar(true)
{
    // Should be at the end of the constructor
    //GetThreadWindowCompositor().RegisterWindow(smptr(FloatingWindow)(this, false));

    m_MinimizeButton = smptr(CoreArea)(new CoreArea());
    m_CloseButton = smptr(CoreArea)(new CoreArea());
    m_SizeGrip = smptr(CoreArea)(new CoreArea());
    m_TitleBar = smptr(CoreArea)(new CoreArea());
    m_WindowTitleBar = smptr(StaticTextBox)(new StaticTextBox());
    m_TitleBarLayout = smptr(HLayout)(new HLayout());

    m_MinimizeButton->SetMinMaxSize(20, 20);
    m_MinimizeButton->setGeometry(0, 0, 20, 20);
    m_CloseButton->SetMinimumSize(20, 20);
    m_CloseButton->setGeometry(0, 0, 20, 20);
    m_SizeGrip->SetMinimumSize(SizeGripWidth, SizeGripHeight);
    m_SizeGrip->setGeometry(Geometry(0, 0, SizeGripWidth, SizeGripHeight));

    m_TitleBar->OnMouseDown.connect( sigc::mem_fun(this, &FloatingWindow::RecvTitleBarMouseDown));
    m_TitleBar->OnMouseDrag.connect( sigc::mem_fun(this, &FloatingWindow::RecvTitleBarMouseDrag));
    m_CloseButton->OnMouseClick.connect( sigc::mem_fun(this, &FloatingWindow::RecvCloseButtonClick));

    m_SizeGrip->OnMouseDrag.connect( sigc::mem_fun(this, &FloatingWindow::OnSizeGrigMouseDrag));
    m_SizeGrip->OnMouseDown.connect( sigc::mem_fun(this, &FloatingWindow::OnSizeGrigMouseDown));

    m_WindowTitle = Strdup(TEXT(""));
    m_WindowTitleBar->SetBaseString(TEXT("Window Title"));
    m_TitleBarLayout->AddActiveInterfaceObject((m_WindowTitleBar), 1, eCenter, eFix);
    //m_TitleBarLayout->AddActiveInterfaceObject((&m_MinimizeButton), 0, eCenter, eFix);
    m_TitleBarLayout->AddActiveInterfaceObject((m_CloseButton), 0, eCenter, eFix);

    if(HasTitleBar())
        SetTopBorder(24);
    else
        SetTopBorder(6);
    SetBorder(2);

    SetMinimumSize(32, 32);
    setGeometry(Geometry(100, 100, 320, 200));

    NString Path = INL_FINDRESOURCELOCATION(TEXT("Media/UITextures/AddButton.png"));
    MinimizeIcon.Update(Path.GetTCharPtr());
    Path = INL_FINDRESOURCELOCATION(TEXT("Media/UITextures/CancelButton.png"));
    CloseIcon.Update(Path.GetTCharPtr());

    SetWindowTitle(WindowName);
}

FloatingWindow::~FloatingWindow()
{
    GetThreadWindowCompositor().UnRegisterWindow(smptr(FloatingWindow)(this, false));
    m_InterfaceObject.clear();
    INL_SAFE_DELETE_ARRAY(m_WindowTitle);
}

long FloatingWindow::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
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

    ret = m_CloseButton->OnEvent(window_event, ret, ProcEvInfo);

    if(HasTitleBar())
        ret = m_TitleBar->OnEvent(window_event, ret, ProcEvInfo);

//    if(m_vertical_scrollbar_enable)
//        ret = vscrollbar->ProcessEvent(ievent, ret, ProcEvInfo);
//    if(m_horizontal_scrollbar_enable)
//        ret = hscrollbar->ProcessEvent(ievent, ret, ProcEvInfo);

    if(IsSizeMatchContent() == false)
    {
        // Do not let the m_SizeGrip test the event because the window is not displaying it;
        int XGrip = window_event.e_x - window_event.e_x_root - m_SizeGrip->GetBaseX();
        int YGrip = window_event.e_y - window_event.e_y_root - m_SizeGrip->GetBaseY();

        if(ievent.e_event == INL_MOUSE_PRESSED)
        {
            // We want to false on one half of the size grip square to register a mouse down. This is meant to leave more room
            // for the scrollbar buttons (if any) at the bottom right of the window.
            if((XGrip > 0) && (YGrip > 0) && (XGrip > SizeGripHeight - YGrip + 4))
               ret = m_SizeGrip->OnEvent(window_event, ret, ProcEvInfo);
        }
        else
            ret = m_SizeGrip->OnEvent(window_event, ret, ProcEvInfo);
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

void FloatingWindow::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to (0, 0).
    base.SetX(0);
    base.SetY(0);
    GfxContext.PushClippingRectangle(base);
    
    gPainter.PushDrawShapeLayer(GfxContext, base, eSHAPE_CORNER_ROUND10, Color(m_BackgroundColor), eCornerTopLeft|eCornerTopRight, true);
    if(HasTitleBar())
    {
        gPainter.PaintShapeCorner(GfxContext, Geometry(m_TitleBar->GetBaseX(), m_TitleBar->GetBaseY(),
            m_TitleBar->GetBaseWidth(), m_TitleBar->GetBaseHeight()), Color(0xFF2f2f2f),
            eSHAPE_CORNER_ROUND10, eCornerTopLeft|eCornerTopRight);

        gPainter.PaintTextLineStatic(GfxContext, GFontBold, m_WindowTitleBar->GetGeometry(), m_WindowTitle, Color(0xFFFFFFFF), true, eAlignTextCenter);
        gPainter.Draw2DTextureAligned(GfxContext, &CloseIcon, m_CloseButton->GetGeometry(), TextureAlignmentStyle(eTACenter, eTACenter));
    }

    gPainter.PopBackground();
    GfxContext.PopClippingRectangle();
}

void FloatingWindow::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    // The elements position inside the window are referenced to top-left window corner. So bring base to (0, 0).
    base.SetX(0);
    base.SetY(0);
    
    gPainter.PushShapeLayer(GfxContext, base, eSHAPE_CORNER_ROUND10, Color(m_BackgroundColor), eCornerTopLeft|eCornerTopRight, true);
    if(m_layout.IsValid())
    {
        GfxContext.PushClippingRectangle(base);
        
        m_layout->ProcessDraw(GfxContext, force_draw);
        
        GfxContext.PopClippingRectangle();
    }
    gPainter.PopBackground();
}

void FloatingWindow::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{
    if(force_draw == false)
    {
        return;
    }
    if((IsVisibleSizeGrip() == true) && (IsSizeMatchContent() == false))
    {
        // Do not draw the size grip if the window is constrained by the size of the container layout.
        gPainter.PaintShape(GfxContext, m_SizeGrip->GetGeometry(), Color(0xFF999999), eWINDOW_SIZEGRIP);
    }
}

void FloatingWindow::EnableTitleBar(bool b)
{
    m_hasTitleBar = b;
    ComputeChildLayout();
}

bool FloatingWindow::HasTitleBar() const
{
    return m_hasTitleBar;
}

void FloatingWindow::OnSizeGrigMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    m_SizeGripDragPositionX = x;
    m_SizeGripDragPositionY = y;
    //GetThreadWindowCompositor().SetMouseFocusArea(this);
}

void FloatingWindow::OnSizeGrigMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    if(IsSizeMatchContent())
    {
        return;
    }
    Geometry geo;
    geo = GetGeometry();

    int ddx = 0;
    int ddy = 0;

    if((dx > 0) && (x > m_SizeGripDragPositionX))
    {
        ddx = dx;
    }
    if((dx < 0) && (x < m_SizeGripDragPositionX))
    {
        ddx = dx;
    }
    if((dy > 0) && (y > m_SizeGripDragPositionY))
    {
        ddy = dy;
    }
    if((dy < 0) && (y < m_SizeGripDragPositionY))
    {
        ddy = dy;
    }

    geo.OffsetSize(ddx, ddy);

    setGeometry(geo);

    NeedRedraw();
}

void FloatingWindow::RecvTitleBarMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    m_TitleBarMouseDown.Set(x, y);
}

void FloatingWindow::RecvTitleBarMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    Geometry geo;
    geo = GetGeometry();
    geo.OffsetPosition(dx, dy);

    // Set the Window Size and Position
    BaseObject::setGeometry(geo);
    // No need to compute the window layout elements [LayoutWindowElements()]. They haven't changed.
    // No need to compute the layout [ComputeChildLayout()]. It hasn't changed.

    m_TitleBar->setGeometry(0, 0, geo.GetWidth(), TitleBarHeight);

    NeedRedraw();
}

void FloatingWindow::RecvCloseButtonClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    StopModal();
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
void FloatingWindow::PreLayoutManagement()
{
    Geometry geo = GetGeometry();
    if(m_ConfigureNotifyCallback)
    {
        (*m_ConfigureNotifyCallback)(GetThreadGLWindow()->GetWindowWidth(), GetThreadGLWindow()->GetWindowHeight(), geo);
        if(geo.IsNull())
        {
            nuxDebugMsg(TEXT("[FloatingWindow::PreLayoutManagement] Received an invalid Geometry."));
            geo = GetGeometry();
        }
        else
        {
            BaseObject::setGeometry(geo);
            // Get the geometry adjusted with respect to min and max dimension of this area.
            geo = GetGeometry();
        }
    }

    // Drag Bar Geometry
    if(HasTitleBar())
    {
        m_TitleBar->setGeometry(0, 0, geo.GetWidth(), TitleBarHeight);
    }
    // Size grip Geometry
    Geometry SizeGripGeometry(geo.GetWidth() - SizeGripWidth, geo.GetHeight() - SizeGripHeight,
        SizeGripWidth, SizeGripHeight);
    m_SizeGrip->setGeometry(SizeGripGeometry);

    if(m_layout.IsValid())
    {
        Geometry layout_geo = Geometry(m_Border, m_TopBorder,
            geo.GetWidth() - 2*m_Border, geo.GetHeight() - m_Border - m_TopBorder);
        m_layout->setGeometry(layout_geo);
    }
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
long FloatingWindow::PostLayoutManagement(long LayoutResult)
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
    Geometry geo = GetGeometry();
    // Drag Bar Geometry
    if(HasTitleBar())
    {
        m_TitleBar->setGeometry(0, 0, geo.GetWidth(), TitleBarHeight);
    }
    // Size grip Geometry
    Geometry temp(geo.GetWidth() - SizeGripWidth, geo.GetHeight() - SizeGripHeight,
        SizeGripWidth, SizeGripHeight);
    m_SizeGrip->setGeometry(temp);
    
    // Title Bar
    m_TitleBarLayout->setGeometry(m_TitleBar->GetGeometry());
    GetGraphicsThread()->ComputeElementLayout(m_TitleBarLayout);

    // A FloatingWindow must kill the result of the management and pass it to the parent Layout.
    return (eCompliantHeight | eCompliantWidth);
    //return result;
}

// Get a change to do any work on an element.
// Here we need to position the header by hand because it is not under the control of vlayout.
void FloatingWindow::PositionChildLayout(float offsetX, float offsetY)
{
    //ScrollView::PositionChildLayout(offsetX, offsetY);

    Geometry geo = GetGeometry();
    // Drag Bar Geometry
    if(HasTitleBar())
    {
        m_TitleBar->setGeometry(0, 0, geo.GetWidth(), TitleBarHeight);
    }
    // Size grip Geometry
    Geometry temp(geo.GetWidth() - SizeGripWidth, geo.GetHeight() - SizeGripHeight,
        SizeGripWidth, SizeGripHeight);
    m_SizeGrip->setGeometry(temp);
    
    // Title Bar
    m_TitleBarLayout->setGeometry(m_TitleBar->GetGeometry());
    GetGraphicsThread()->ComputeElementLayout(m_TitleBarLayout);

}

void FloatingWindow::LayoutWindowElements()
{
    // Define the geometry of some of the component of the window. Otherwise, if the composition layout is not set,
    // then the component won't be correctly placed after a setGeometry. This can be redundant if the composition layout is set.
    Geometry base = GetGeometry();
    m_TitleBar->setGeometry(0, 0, base.GetWidth(), TitleBarHeight);

    m_TitleBarLayout->setGeometry(m_TitleBar->GetGeometry());
    GetGraphicsThread()->ComputeElementLayout(m_TitleBarLayout);
    
    // Size grip Geometry
    Geometry temp(base.GetWidth() - SizeGripWidth, base.GetHeight() - SizeGripHeight,
        SizeGripWidth, SizeGripHeight);
    m_SizeGrip->setGeometry(temp);
}

void FloatingWindow::SetWindowTitle(const TCHAR *title)
{
    if(m_WindowTitle != 0)
        INL_SAFE_DELETE_ARRAY(m_WindowTitle);

    if(title == 0)
    {
        m_WindowTitle = 0;
        return;
    }
    m_WindowTitle = Strdup(title);
}

const TCHAR* FloatingWindow::GetWindowTitle()
{
    return m_WindowTitle;
}

NAMESPACE_END_GUI