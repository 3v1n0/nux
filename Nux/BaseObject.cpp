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
#include "NuxCore/SmartPtr/NRefCount.h"
#include "BaseObject.h"
#include "NuxGraphics/OpenGLEngine.h"
#include "Layout.h"
#include "VSplitter.h"
#include "HSplitter.h"

NAMESPACE_BEGIN_GUI

IMPLEMENT_ROOT_OBJECT_TYPE(BaseObject);
BaseObject::BaseObject()
:   m_SizePolicy(eSizeResizeable)
,   m_PositionPolicy(ePositionLeft)
,   m_Geometry(0,0,DEFAULT_WIDGET_WIDTH,DEFAULT_WIDGET_HEIGHT)
,   m_minSize(BASEOBJECT_MINWIDTH, BASEOBJECT_MINHEIGHT)
,   m_maxSize(BASEOBJECT_MAXWIDTH, BASEOBJECT_MAXHEIGHT)
,   m_ParentObject(0)
,   m_stretchfactor(1)
,   m_Application(0)
,   m_IsSizeDirty(true)
{
}


BaseObject::~BaseObject()
{

}

const NString& BaseObject::GetBaseString() const
{
    return m_BaseString;
}

void BaseObject::SetBaseString(const TCHAR* Caption)
{
    m_BaseString = Caption;
}

void BaseObject::CheckMinSize()
{
    if(m_minSize.GetWidth() > m_maxSize.GetWidth())
    {
        //temp = m_maxSize.GetWidth();
        m_maxSize.SetWidth(m_minSize.GetWidth());
        //m_minSize.SetWidth(temp);
    }

    if(m_minSize.GetHeight() > m_maxSize.GetHeight())
    {
        //temp = m_maxSize.GetBaseHeight();
        m_maxSize.SetHeight(m_minSize.GetHeight());
        //m_minSize.SetHeight(temp);
    }

    if(m_Geometry.GetWidth() < m_minSize.GetWidth())
    {
        m_Geometry.SetWidth(m_minSize.GetWidth());
    }
    if(m_Geometry.GetHeight() < m_minSize.GetHeight())
    {
        m_Geometry.SetHeight(m_minSize.GetHeight());
    }
}

void BaseObject::CheckMaxSize()
{
    if(m_minSize.GetWidth() > m_maxSize.GetWidth())
    {
        //temp = m_maxSize.GetWidth();
        m_minSize.SetWidth(m_maxSize.GetWidth());
        //m_minSize.SetWidth(temp);
    }

    if(m_minSize.GetHeight() > m_maxSize.GetHeight())
    {
        //temp = m_maxSize.GetBaseHeight();
        m_minSize.SetHeight(m_maxSize.GetHeight());
        //m_minSize.SetHeight(temp);
    }

    if(m_Geometry.GetWidth() > m_maxSize.GetWidth())
    {
        m_Geometry.SetWidth(m_maxSize.GetWidth());
    }
    if(m_Geometry.GetHeight() > m_maxSize.GetHeight())
    {
        m_Geometry.SetHeight(m_maxSize.GetHeight());
    }
}

void BaseObject::SetMinimumSize(int w, int h)
{
    nuxAssert(w >= 0);
    nuxAssert(h >= 0);
    m_minSize.SetWidth(w);
    m_minSize.SetHeight(h);

    CheckMinSize();

    InitiateResizeLayout();
}

void BaseObject::SetMaximumSize(int w, int h)
{
    nuxAssert(w >= 0);
    nuxAssert(h >= 0);
    m_maxSize.SetWidth(w);
    m_maxSize.SetHeight(h);

    CheckMaxSize();

    InitiateResizeLayout();
}

void BaseObject::SetMinMaxSize(int w, int h)
{
    nuxAssert(w >= 0);
    nuxAssert(h >= 0);
    SetMinimumSize(w, h);
    SetMaximumSize(w, h);

    //InitiateResizeLayout();
}

void BaseObject::ApplyMinWidth()
{
    m_Geometry.SetWidth(m_minSize.GetWidth());

    InitiateResizeLayout();
}

void BaseObject::ApplyMinHeight()
{
    m_Geometry.SetHeight(m_minSize.GetHeight());

    InitiateResizeLayout();
}

void BaseObject::ApplyMaxWidth()
{
    m_Geometry.SetWidth(m_maxSize.GetWidth());

    InitiateResizeLayout();
}

void BaseObject::ApplyMaxHeight()
{
    m_Geometry.SetHeight(m_maxSize.GetHeight());

    InitiateResizeLayout();
}

Size BaseObject::GetMinimumSize() const
{
    return m_minSize;
}

Size BaseObject::GetMaximumSize() const
{
    return m_maxSize;
}

void BaseObject::SetMinimumWidth(int w)
{
    nuxAssert(w >= 0);
    m_minSize.SetWidth(w);
    CheckMinSize();
    InitiateResizeLayout();
}

void BaseObject::SetMaximumWidth(int w)
{
    nuxAssert(w >= 0);
    m_maxSize.SetWidth(w);
    CheckMaxSize();
    InitiateResizeLayout();
}

void BaseObject::SetMinimumHeight(int h)
{
    nuxAssert(h >= 0);
    m_minSize.SetHeight(h);
    CheckMinSize();
    InitiateResizeLayout();
}

void BaseObject::SetMaximumHeight(int h)
{
    nuxAssert(h >= 0);
    m_maxSize.SetHeight(h);
    CheckMaxSize();
    InitiateResizeLayout();
}

int BaseObject::GetMinimumWidth() const
{
    return m_minSize.GetWidth();
}

int BaseObject::GetMaximumWidth() const
{
    return m_maxSize.GetWidth();
}

int BaseObject::GetMinimumHeight() const
{
    return m_minSize.GetHeight();
}

int BaseObject::GetMaximumHeight() const
{
    return m_maxSize.GetHeight();
}

unsigned int BaseObject::GetStretchFactor()
{
    return m_stretchfactor;
}

void BaseObject::SetStretchFactor(unsigned int sf) 
{
    // re implemented by Layout
    m_stretchfactor = sf;
    //if(m_stretchfactor == 0)
    //ComputeLayout2(); // This will make the layout fit exactly its children if any.
}

void BaseObject::UnParentObject(smptr(BaseObject))
{
    if(m_ParentObject.IsValid() && m_ParentObject->IsLayout())
    {
        m_ParentObject->RemoveChildObject(smptr(BaseObject)(this, false));
        m_ParentObject->RequestBottomUpLayoutComputation(smptr(BaseObject)(NULL));
        m_ParentObject = smptr(BaseObject)(0);
    }
}

void BaseObject::RemoveChildObject(smptr(BaseObject))
{

}

void BaseObject::SetParentObject(smptr(BaseObject) bo)
{
    m_ParentObject = bo;
}

smptr(BaseObject) BaseObject::GetParentObject()
{
    return m_ParentObject;
}

void BaseObject::setGeometry(int x, int y, int w, int h) 
{
    m_Geometry.SetX(x);
    m_Geometry.SetY(y);
    _SetBaseWidth(w);
    _SetBaseHeight(h); 

    InitiateResizeLayout();
}

void BaseObject::setGeometry(const Geometry& geo) 
{
    SetBaseX(geo.x);
    SetBaseY(geo.y);
    _SetBaseWidth(geo.GetWidth());
    _SetBaseHeight(geo.GetHeight());

    InitiateResizeLayout();
}

void BaseObject::SetBaseSize(int w, int h)
{
    _SetBaseWidth(w);
    _SetBaseHeight(h);

    InitiateResizeLayout();
}

void BaseObject::SetBaseWidth(int w) 
{
    _SetBaseWidth(w);
    InitiateResizeLayout();
}

void BaseObject::SetBaseHeight(int h) 
{
    _SetBaseHeight(h);
    InitiateResizeLayout();
}

void BaseObject::_SetBaseWidth(int w) 
{
    if(w < m_minSize.GetWidth())
    {
        m_Geometry.SetWidth(m_minSize.GetWidth());
    }
    else if(w > m_maxSize.GetWidth())
    {
        m_Geometry.SetWidth(m_maxSize.GetWidth());
    }
    else
    {
        m_Geometry.SetWidth(w);
    }
}

void BaseObject::_SetBaseHeight(int h) 
{
    if(h < m_minSize.GetHeight())
    {
        m_Geometry.SetHeight(m_minSize.GetHeight());
    }
    else if(h > m_maxSize.GetHeight())
    {
        m_Geometry.SetHeight(m_maxSize.GetHeight());
    }
    else
    {
        m_Geometry.SetHeight(h);
    }
}

void BaseObject::InitiateResizeLayout(smptr(BaseObject) child)
{
    if(GetGraphicsThread()->IsComputingLayout())
    {
        // there is no need to do the following while we are already computing the layout.
        // If we do, we will end up in an infinite loop.
        return;
    }

    if(this->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
    {
        // The object that is being resized is a ActiveInterfaceObject object
        smptr(ActiveInterfaceObject) ic(this, false);
        if(ic->CanBreakLayout())
        {

            if((child != 0) && 
                (ic->Type().IsObjectType(VSplitter::StaticObjectType) || ic->Type().IsObjectType(HSplitter::StaticObjectType)))
            {
                // If this element is a Splitter, then we submit its child to the refresh list. We don't want to submit the 
                // splitter because this will cause a redraw of all parts of the splitter (costly and unnecessary). 
                GetGraphicsThread()->AddObjectToRefreshList(child);
            }
            else
            {
                GetGraphicsThread()->AddObjectToRefreshList(ic);
            }
        }
        else if(ic->m_ParentObject.IsValid())
            ic->m_ParentObject->InitiateResizeLayout(smptr(BaseObject)(this, false));
        else
        {
            GetGraphicsThread()->AddObjectToRefreshList(ic);
        }
    }
    else if(this->Type().IsDerivedFromType(Layout::StaticObjectType))
    {
        smptr(Layout) layout = smptr(Layout)(this, false);
        if(layout->m_ParentObject.IsValid())
        {
            if(layout->m_ParentObject->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
            {
                smptr(ActiveInterfaceObject) ic = smptr(ActiveInterfaceObject)(layout->m_ParentObject);
                if(ic->CanBreakLayout())
                {
                    if((child != 0) && 
                        (ic->Type().IsObjectType(VSplitter::StaticObjectType) || ic->Type().IsObjectType(HSplitter::StaticObjectType)))
                    {
                        // If the parent of this element is a splitter, then we submit its child to the refresh list. We don't want to submit the 
                        // splitter because this will cause a redraw of all parts of the splitter (costly and unnecessary). 
                        GetGraphicsThread()->AddObjectToRefreshList(smptr(BaseObject)(this, false));
                    }
                    else
                    {
                        GetGraphicsThread()->AddObjectToRefreshList(ic);
                    }
                }
                else
                {
                    // The parent object of an object of type ActiveInterfaceObject is a Layout object type.
                    layout->m_ParentObject->InitiateResizeLayout(smptr(BaseObject)(this, false));
                }
            }
            else
            {
                layout->m_ParentObject->InitiateResizeLayout(smptr(BaseObject)(this, false));
            }
        }
        else
        {
            // This is possibly the Main layout or the layout of a floating object (popup for example) unless the layout is not part of the object tree.
            GetGraphicsThread()->AddObjectToRefreshList(layout);
        }
    }
    else
    {
        // The object that is being resized is a CoreArea object.
        if(this->m_ParentObject.IsValid())
        {
            // The parent object of an object of type CoreArea is a Layout object type.
            this->m_ParentObject->InitiateResizeLayout(smptr(BaseObject)(this, false));
        }
    }
}

void BaseObject::RequestBottomUpLayoutComputation(smptr(BaseObject) bo_initiator)
{
    if(m_ParentObject.IsValid() && m_ParentObject->IsLayout())
    {
        m_ParentObject->RequestBottomUpLayoutComputation(bo_initiator);
    }

}

NAMESPACE_END_GUI
