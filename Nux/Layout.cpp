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

NAMESPACE_BEGIN_GUI

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Layout
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_TYPE(Layout);

Layout::Layout()
{
    m_compositeIC       = smptr(BaseObject)(0);
    m_h_in_margin       = 0;
    m_h_out_margin      = 0;
    m_v_in_margin       = 0;
    m_v_out_margin      = 0;
    m_contentWidth      = 0;
    m_contentHeight     = 0;
    m_ContentStacking   = eStackExpand;
}

Layout::~Layout()
{
    // It is possible that this layout object is in the refresh list. Remove it here before it is deleted.
    GetGraphicsThread()->RemoveObjectFromRefreshList(smptr(BaseObject)(this, false));

    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        (*it)->SetParentObject(smptr(BaseObject)(0));
    }
    m_LayoutElementList.clear();
}


void Layout::RemoveChildObject(smptr(BaseObject) bo)
{
    std::list<smptr(BaseObject)>::iterator it;
    it = std::find(m_LayoutElementList.begin(), m_LayoutElementList.end(), bo);
    if(it != m_LayoutElementList.end())
    {
        bo->SetParentObject(smptr(BaseObject)(0));
        m_LayoutElementList.erase(it);
    }
}

bool Layout::FindWidget(smptr(BaseObject) WidgetObject) const
{
    std::list<smptr(BaseObject)>::const_iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it) == WidgetObject)
        {
            return true;
        }
    }
    return false;
}

bool Layout::IsEmpty() const
{
    return (m_LayoutElementList.size() == 0);
}

// If(stretchfactor == 0): the WidgetLayout geometry will be set to setGeometry(0,0,1,1);
// and the children will take their natural size by expending WidgetLayout.
// If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
void Layout::AddLayout(smptr(Layout) layout, unsigned int stretchFactor, eMinorPosition minor_position, eMinorSize minor_size, float percentage)
{
    nuxAssertMsg(layout != 0, TEXT("[Layout::AddActiveInterfaceObject] Invalid parameter."));
    NUX_RETURN_IF_TRUE(layout == 0);
    //  Should never happen
    nuxAssertMsg(layout != this, TEXT("[Layout::AddLayout] Error: Trying to add a layout to itself."));
    NUX_RETURN_IF_FALSE(layout != 0);

    smptr(BaseObject) parent = layout->GetParentObject();
    nuxAssertMsg(parent == 0, TEXT("[Layout::AddLayout] Trying to add an object that already has a parent."));
    NUX_RETURN_IF_TRUE(parent != 0);

    layout->SetStretchFactor(stretchFactor);
    layout->setPositioning(minor_position);
    layout->SetExtend(minor_size);

    if(percentage < 1.0f)
    {
        layout->SetPercentage(1.0f);
    }
    else if(percentage > 100.0f)
    {
        layout->SetPercentage(100.0f);
    }
    else
    {
        layout->SetPercentage(percentage);
    }

    layout->SetParentObject(smptr(BaseObject)(this, false));
    m_LayoutElementList.push_back(layout);

    //--->> Removed because it cause problem with The splitter widget: ComputeLayout2();
}

//! Add an object to the layout.
/*! Add an object to the layout.
    A baseobject minor dimension with respect to a layout object is the dimension opposite to the layout flow.
    A baseobject major dimension with respect to a layout object is the dimension aligned with the layout flow.
    A layout object minor dimension is the dimension opposite to the layout flow.
    A layout object major dimension is the dimension aligned with the layout flow.

    Add an object to the layout. The added object get its size and position managed by the layout. 
    When a baseobject is added with a stretches factor equal to 0, its major dimension assumes its minimum value.
    For instance, if the layout is a vertical layout and the added object has a stretch factor equal 0, then during the layout,
    the added object height will be set to its minimum value using ApplyMinHeight().

    The minor_position parameter controls how the layout will place the object within itself. A vertical layout object controls the horizontal positioning
    of its children baseobjects, While an horizontal layout object controls the vertical positioning of its children baseobjects.

    The minor_size parameter controls how much size the baseobject minor dimension gets from the layout minor dimension. See eMinorSize.

    /param baseobject The object that is being added.
    /param stretchFactor This value controls how the layout object share space between its children baseobject.
    /param minor_position Controls how the layout position the object.
    /param minor_size Controls the object minor dimension size.
    /param percentage Controls the object minor dimension size in percentage of the layout minor dimension size.
*/

void Layout::AddActiveInterfaceObject(smptr(BaseObject) bo, unsigned int stretchFactor, eMinorPosition minor_position, eMinorSize minor_size, float percentage)
{
    nuxAssertMsg(bo != 0, TEXT("[Layout::AddActiveInterfaceObject] Invalid parameter."));
    NUX_RETURN_IF_TRUE(bo == 0);

    smptr(BaseObject) parent = bo->GetParentObject();
    nuxAssertMsg(parent == 0, TEXT("[Layout::AddActiveInterfaceObject] Trying to add an object that already has a parent."));
    NUX_RETURN_IF_TRUE(parent != 0);

    bo->SetStretchFactor(stretchFactor);
    bo->setPositioning(minor_position);
    bo->SetExtend(minor_size);

    if(percentage < 1.0f)
    {
        bo->SetPercentage(1.0f);
    }
    else if(percentage > 100.0f)
    {
        bo->SetPercentage(100.0f);
    }
    else
    {
        bo->SetPercentage(percentage);
    }

    bo->SetParentObject(smptr(BaseObject)(this, false));

    bo->SetApplication(GetApplication());
    m_LayoutElementList.push_back(bo);

    //--->> Removed because it cause problem with The splitter widget: ComputeLayout2();
}

void Layout::AddSpace(unsigned int width, unsigned int stretchFactor)
{
    AddLayout(smptr(SpaceLayout)(new SpaceLayout()), stretchFactor);
}

void Layout::Clear()
{
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        (*it)->SetParentObject(smptr(BaseObject)(0));
    }
    m_LayoutElementList.clear();
}

bool Layout::SearchInAllSubNodes(smptr(BaseObject) bo)
{
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it) == bo)
        {
            return true;
        }
        else if((*it)->IsLayout())
        {
            smptr(Layout) layout(*it);
            return layout->SearchInAllSubNodes(bo);
        }
    }
    return false;
}

bool Layout::SearchInFirstSubNodes(smptr(BaseObject) bo)
{
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it) == bo)
        {
            return true;
        }
    }
    return false;
}

unsigned int Layout::GetMaxStretchFactor()
{
    unsigned int value = 0;
    unsigned int sf;
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        sf = (*it)->GetStretchFactor(); 
        if(sf >= value)
        {
            value = sf;
        }
    }
    return value;
}

unsigned int Layout::GetMinStretchFactor()
{
    unsigned int value = 0xFFFFFFFF;
    unsigned int sf;
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        sf = (*it)->GetStretchFactor();
        if(sf <= value)
        {
            value = sf;
        }
    }
    return value;
}

unsigned int Layout::GetNumStretchFactor(unsigned int sf)
{
    unsigned int count = 0;
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it)->GetStretchFactor() == sf)
        {
            count++;
        }
    }
    return count;
}

void Layout::DoneRedraw()
{
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it)->IsInterfaceControl())
        {
            smptr(ActiveInterfaceObject) ic = (*it);
            ic->DoneRedraw();
        }
    }
}

long Layout::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it)->IsArea())
        {
            smptr(CoreArea) area(*it);
            ret = area->OnEvent(ievent, ret, ProcessEventInfo);
        }
        else if((*it)->IsInterfaceControl())
        {
            smptr(ActiveInterfaceObject) ic = (*it);
            ret = ic->ProcessEvent(ievent, ret, ProcessEventInfo);
        }
        else if((*it)->IsLayout())
        {
            smptr(Layout) layout =  (*it);
            ret = layout->ProcessEvent(ievent, ret, ProcessEventInfo);
        }
    }
    return ret;
}

void Layout::ProcessDraw(GraphicsContext& GfxContext, bool force_draw)
{
    //GfxContext.Push2DModelViewMatrix(m_Matrix);
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it)->IsArea())
        {
            smptr(CoreArea) area(*it);
            area->OnDraw(GfxContext, force_draw);
        }
        else if((*it)->IsInterfaceControl())
        {
            smptr(ActiveInterfaceObject) ic = (*it);
            ic->ProcessDraw(GfxContext, force_draw);
        }
        else if((*it)->IsLayout())
        {
            smptr(Layout) layout =  (*it);
            layout->ProcessDraw(GfxContext, force_draw);
        }
    }

    SetDrawDirty(false);
    //GfxContext.Pop2DModelViewMatrix();
}

void Layout::NeedRedraw()
{
    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it)->IsArea())
        {
            // Does not have the flag for need redraw.
        }
        else if((*it)->IsInterfaceControl())
        {
            smptr(ActiveInterfaceObject) ic = (*it);
            ic->NeedRedraw();
        }
        else if((*it)->IsLayout())
        {
            smptr(Layout) layout =  (*it);
            layout->NeedRedraw();
        }
    }
}

void Layout::SetContentStacking(eStacking stacking)
{
    m_ContentStacking = stacking;
}

eStacking Layout::GetContentStacking()
{
    return m_ContentStacking;
}

void Layout::RequestBottomUpLayoutComputation(smptr(BaseObject) bo_initiator)
{

}

void Layout::SetApplication(WindowThread* Application)
{
    BaseObject::SetApplication(Application);

    std::list<smptr(BaseObject)>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        (*it)->SetApplication(Application);
    }
}

void Layout::Translate(int x, int y)
{
    m_Matrix.Identity();
    m_Matrix.Translate(x, y, 0);
}

NAMESPACE_END_GUI
