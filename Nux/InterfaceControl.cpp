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
#include "InterfaceControl.h"

namespace nux { //NUX_NAMESPACE_BEGIN

NUX_IMPLEMENT_OBJECT_TYPE(ActiveInterfaceObject);

ActiveInterfaceObject::ActiveInterfaceObject(NUX_FILE_LINE_DECL)
:   BaseArea(NUX_FILE_LINE_PARAM)
{
    m_CompositionLayout = smptr(Layout)(0);
    m_NeedRedraw        = false;
    m_UseStyleDrawing   = true;
    m_TextColor         = 0;
    m_IsEnabled         = true;

    // Set widget default size;
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
}

ActiveInterfaceObject::~ActiveInterfaceObject()
{
    // It is possible that the object is in the refresh list. Remove it here before it is deleted.
    GetGraphicsThread()->RemoveObjectFromRefreshList(smptr(BaseObject)(this, true));

    NUX_SAFE_DELETE(m_TextColor);
}

long ActiveInterfaceObject::ComputeChildLayout()
{
    if(m_CompositionLayout.IsValid())
    {
        m_CompositionLayout->SetDirty(true);
//        if(m_CompositionLayout->GetStretchFactor() != 0)
//        {
//            PreLayoutManagement();
//            long ret = m_CompositionLayout->ComputeLayout2();
//            return PostLayoutManagement(ret);
//        }
//        else
        {
            PreLayoutManagement();
            
            int PreWidth = /*m_CompositionLayout->*/GetBaseWidth();
            int PreHeight = /*m_CompositionLayout->*/GetBaseHeight();

            long ret = m_CompositionLayout->ComputeLayout2();

            PostLayoutManagement(ret);
            //return eCompliantWidth | eCompliantHeight;

            int PostWidth = /*m_CompositionLayout->*/GetBaseWidth();
            int PostHeight = /*m_CompositionLayout->*/GetBaseHeight();

            long size_compliance = 0;
            // The layout has been resized to tightly pack its content
            if(PostWidth > PreWidth)
            {
                size_compliance |= eLargerWidth; // need scrollbar
            }
            else if(PostWidth < PreWidth)
            {
                size_compliance |= eSmallerWidth;
            }
            else
            {
                size_compliance |= eCompliantWidth;
            }

            // The layout has been resized to tightly pack its content
            if(PostHeight > PreHeight)
            {
                size_compliance |= eLargerHeight; // need scrollbar
            }
            else if(PostHeight < PreHeight)
            {
                size_compliance |= eSmallerHeight;
            }
            else
            {
                size_compliance |= eCompliantHeight;
            }

            //BaseObject::setGeometry(m_CompositionLayout->GetGeometry());
            return size_compliance;
        }
    }
    else
    {
        PreLayoutManagement();
        int ret = PostLayoutManagement(eCompliantHeight|eCompliantWidth);
        return ret;
    }
    return 0;
}

void ActiveInterfaceObject::PositionChildLayout(float offsetX, float offsetY)
{
    if(m_CompositionLayout.IsValid())
    {
        // This section from //1 to //2 is not needed. here we should not do any size management. Only position..
        //1
        if(m_CompositionLayout->GetStretchFactor() != 0)
        {
            m_CompositionLayout->setGeometry(GetGeometry());
        }
        else //2
        {
            m_CompositionLayout->SetBaseX(GetBaseX());
            m_CompositionLayout->SetBaseY(GetBaseY());
        }
        m_CompositionLayout->ComputePosition2(offsetX, offsetY);

    }
}

void ActiveInterfaceObject::PreLayoutManagement()
{
    // Give the managed layout the same size and position as the Control.
    if(m_CompositionLayout.IsValid())
        m_CompositionLayout->setGeometry(GetGeometry());
}

long ActiveInterfaceObject::PostLayoutManagement(long LayoutResult)
{
    // Set the geometry of the control to be the same as the managed layout.
    // Only the size is changed. The position of the composition layout hasn't
    // been changed by ComputeLayout2.
    if(m_CompositionLayout.IsValid())
    {
        // If The layout is empty, do not change the size of the parent element.
        if(!m_CompositionLayout->IsEmpty())
            BaseObject::setGeometry(m_CompositionLayout->GetGeometry());
    }
    return LayoutResult;
}

void ActiveInterfaceObject::PreResizeGeometry()
{

}

void ActiveInterfaceObject::PostResizeGeometry()
{
}

long ActiveInterfaceObject::PostProcessEvent2(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    return OnEvent(ievent, TraverseInfo, ProcessEventInfo);
}

void ActiveInterfaceObject::ProcessDraw(GraphicsContext& GfxContext, bool force_draw)
{
    m_IsFullRedraw = false;
    if(force_draw)
    {
        m_NeedRedraw = true;
        m_IsFullRedraw = true;
        Draw(GfxContext, force_draw);
        DrawContent(GfxContext, force_draw);
        PostDraw(GfxContext, force_draw);
    }
    else
    {
        if(m_NeedRedraw)
        {
            m_IsFullRedraw = true;
            Draw(GfxContext, false);
            DrawContent(GfxContext, false);
            PostDraw(GfxContext, false);
        }
        else
        {
            DrawContent(GfxContext, false);
            PostDraw(GfxContext, false);
        }
    }
    m_NeedRedraw = false;
    m_IsFullRedraw = false;
}

void ActiveInterfaceObject::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{

}

void ActiveInterfaceObject::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void ActiveInterfaceObject::NeedRedraw()
{
    //GetThreadWindowCompositor()..AddToDrawList(this);
    WindowThread* application = GetGraphicsThread();
    if(application)
        application->RequestRedraw();
    m_NeedRedraw = true;
}

void ActiveInterfaceObject::NeedSoftRedraw()
{
    //GetThreadWindowCompositor()..AddToDrawList(this);
    WindowThread* application = GetGraphicsThread();
    if(application)
        application->RequestRedraw();
    //m_NeedRedraw = false;
}

bool ActiveInterfaceObject::IsRedrawNeeded()
{
    return m_NeedRedraw;
}

void ActiveInterfaceObject::DoneRedraw()
{
    m_NeedRedraw = false;
    if(m_CompositionLayout.IsValid())
    {
        m_CompositionLayout->DoneRedraw();
    }
}

void ActiveInterfaceObject::DrawLayout()
{
    if(m_CompositionLayout.IsValid())
    {
        m_CompositionLayout->Draw();
    }
    else
    {
        int x, y, width, height;
        x = GetBaseX();
        y = GetBaseY();
        width = GetBaseWidth();
        height = GetBaseHeight();

        glColor3f(0.0f, 1.0f, 0.0f);
        glDisable(GL_TEXTURE_2D);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBegin(GL_QUADS);
        glVertex3i(x,  y, 1);
        glVertex3i(x+width,  y, 1);
        glVertex3i(x+width,  y+height, 1);
        glVertex3i(x,  y+height, 1);
        glEnd();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_TEXTURE_2D);
    }
}

smptr(Layout) ActiveInterfaceObject::GetCompositionLayout() const
{
    return m_CompositionLayout;
}

void ActiveInterfaceObject::SetCompositionLayout(smptr(Layout) layout)
{
    if(layout == 0)
        return;

    // Unparent layout if it has a Parent.
    {
        smptr(BaseObject) parent = layout->GetParentObject();
        if(parent != 0)
        {
            parent->RemoveChildObject(layout);
        }
        layout->SetParentObject(smptr(BaseObject)(this, true));
    }
    layout->SetParentObject(smptr(BaseObject)(this, true));
    m_CompositionLayout = layout;
}

void ActiveInterfaceObject::RemoveCompositionLayout()
{
    if(m_CompositionLayout.IsValid())
        m_CompositionLayout->SetParentObject(smptr(BaseObject)(0));
    m_CompositionLayout = smptr(Layout)(0);
}

bool ActiveInterfaceObject::SearchInAllSubNodes(smptr(BaseObject) bo)
{
    if(m_CompositionLayout.IsValid())
        return m_CompositionLayout->SearchInAllSubNodes(bo);
    return false;
}

bool ActiveInterfaceObject::SearchInFirstSubNodes(smptr(BaseObject) bo)
{
    if(m_CompositionLayout.IsValid())
        return m_CompositionLayout->SearchInFirstSubNodes(bo);
    return false;
}

void ActiveInterfaceObject::setGeometry(const Geometry& geo)
{
    BaseObject::setGeometry(geo);
    ComputeChildLayout();
    PostResizeGeometry();
}

void ActiveInterfaceObject::SetFont(const NFontPtr& Font)
{
    m_Font = Font;
}

const NFontPtr& ActiveInterfaceObject::GetFont()
{
    if(m_Font)
        return m_Font;
    else
        return GFont;
}

void ActiveInterfaceObject::SetTextColor(const Color& color)
{
    m_TextColor = color.Clone();
}

void ActiveInterfaceObject::SetTextColor(const Color* color)
{
    if(color == 0)
    {
        NUX_SAFE_DELETE(m_TextColor);
    }
    else
    {
        NUX_SAFE_DELETE(m_TextColor);
        m_TextColor = color->Clone();
    }
}

const Color& ActiveInterfaceObject::GetTextColor()
{
    if(m_TextColor)
        return *m_TextColor;
    else
        return GTextColor;
}

void ActiveInterfaceObject::DisableWidget()
{
    m_IsEnabled = false;
}

void ActiveInterfaceObject::EnableWidget()
{
    m_IsEnabled = true;
}

bool ActiveInterfaceObject::IsWidgetEnabled()
{
    return m_IsEnabled;
}


} //NUX_NAMESPACE_END
