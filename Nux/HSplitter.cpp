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
#include "HSplitter.h"

#include "NuxGraphics/GLTextureResourceManager.h"

#include "WindowCompositor.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux { //NUX_NAMESPACE_BEGIN

NUX_IMPLEMENT_OBJECT_TYPE(HSplitter);
static const t_s32 HSPLITTERHEIGHT = 5;
static const t_s32 HSTICK_SIZE = 5;

HSplitter::HSplitter(NUX_FILE_LINE_DECL)
:   ActiveInterfaceObject(NUX_FILE_LINE_PARAM)
{ 
    m_layout                    = 0;
    new_addition                = false;
    m_initial_config            = true;
    m_focus_splitter_index      = -1;
    m_ResizeOnSplitterRelease   = true;

    mvt_dx = 0;
    mvt_dy = 0;
    m_current_x = 0;
    m_current_y = 0;
    m_current_width = 200;
    m_current_height = 200;

    //SetMinimumSize(200,200);
    setGeometry(Geometry(m_current_x, m_current_y, m_current_width, m_current_height));
}

HSplitter::~HSplitter()
{
    // Delete all the interface object: This is a problem... The widget should be destroy by there associated parameters
    //delete vlayout;
    m_layout = 0;
}

long HSplitter::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;
    long ProcEvInfo = ProcessEventInfo;
    if(ievent.e_event == NUX_MOUSE_PRESSED)
    {
        if(!m_Geometry.IsPointInside(ievent.e_x, ievent.e_y))
        {
            ProcEvInfo |= eDoNotProcess;
            //return TraverseInfo;
        }
    }

    //if(traverse)
    {   
        std::vector<MySplitter*>::iterator it_splitter;
        for(it_splitter = m_SplitterObject.begin(); it_splitter != m_SplitterObject.end(); it_splitter++)
        {
            ret = (*it_splitter)->OnEvent(ievent, ret, ProcEvInfo);
        }

        std::vector<smptr(BaseObject)>::iterator it;
        for(it = m_InterfaceObject.begin(),
            it_splitter = m_SplitterObject.begin();
            it != m_InterfaceObject.end();
            it++, it_splitter++)
        {
            smptr(BaseObject) object = (*it);
            MySplitter *splitter = (*it_splitter);

            Geometry clip_geo;
            clip_geo.SetX(((*it)->GetGeometry().x));
            clip_geo.SetY(((*it)->GetGeometry().y));
            clip_geo.SetWidth((*it)->GetGeometry().GetWidth());
            clip_geo.SetHeight((*it_splitter)->GetGeometry().y - (*it)->GetGeometry().y);

            long DoNotProcess = ProcEvInfo;
            if(clip_geo.IsPointInside(ievent.e_x, ievent.e_y) == false)
            {
                DoNotProcess |= eDoNotProcess;
            }
            if((*it)->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
            {
                smptr(ActiveInterfaceObject) ic(*it);
                ret = ic->ProcessEvent(ievent, ret, ProcEvInfo | DoNotProcess);
            }
            else if((*it)->Type().IsObjectType(BaseArea::StaticObjectType))
            {
                smptr(BaseArea) base_area(*it);
                ret = base_area->OnEvent(ievent, ret, ProcEvInfo | DoNotProcess);
            }
            else if((*it)->Type().IsDerivedFromType(Layout::StaticObjectType))
            {
                smptr(Layout) layout(*it);
                ret = layout->ProcessEvent(ievent, ret, ProcEvInfo | DoNotProcess);
            }
        }
    }
    ret = PostProcessEvent2(ievent, ret, ProcEvInfo);
    return ret;
}

void HSplitter::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    GfxContext.PushClippingRectangle(GetGeometry());
    Geometry base = GetGeometry();
//    std::vector<smptr(ActiveInterfaceObject)>::iterator it;
//    for(it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
//    {
//        (*it)->ProcessDraw(force_draw);
//    }

    gPainter.PaintBackground(GfxContext, base);
    std::vector<MySplitter*>::iterator it_splitter;
    for(it_splitter = m_SplitterObject.begin(); it_splitter != m_SplitterObject.end(); it_splitter++)
    {
        Geometry geo = (*it_splitter)->GetGeometry();
        Geometry grip_geo;
        t_s32 w = 20;
        if(geo.GetWidth() > w)
        {
            grip_geo.SetX(geo.x + (geo.GetWidth() - w)/2);
            grip_geo.SetY(geo.y);
            grip_geo.SetWidth(w);
            grip_geo.SetHeight(geo.GetHeight());
        }
        else
        {
            grip_geo.SetX(geo.x - (w - geo.GetWidth())/2);
            grip_geo.SetY(geo.y);
            grip_geo.SetWidth(w);
            grip_geo.SetHeight(geo.GetHeight());
        }

        gPainter.Draw2DLine(GfxContext, grip_geo.x, grip_geo.y+1, grip_geo.x + grip_geo.GetWidth(), grip_geo.y+1, Color(0xFF111111));
        gPainter.Draw2DLine(GfxContext, grip_geo.x, grip_geo.y+3, grip_geo.x + grip_geo.GetWidth(), grip_geo.y+3, Color(0xFF111111));
    }
    GfxContext.PopClippingRectangle();
}

void HSplitter::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle(base);
    bool need_redraw = IsRedrawNeeded();

    std::vector<MySplitter*>::iterator it_splitter;
    std::vector<smptr(BaseObject)>::iterator it;
    //for(it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
    for(it = m_InterfaceObject.begin(), it_splitter = m_SplitterObject.begin();
        it != m_InterfaceObject.end();
        it++, it_splitter++)
    {
        Geometry sgeo = (*it_splitter)->GetGeometry();
        GfxContext.PushClippingRectangle(Rect(
            base.x, base.y, base.GetWidth(), sgeo.y - base.y));

        base.SetY(sgeo.y + sgeo.GetHeight());
        if(force_draw || need_redraw)
        {
            if((*it)->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
            {
                smptr(ActiveInterfaceObject) ic(*it);
                ic->ProcessDraw(GfxContext, true);
            }
            else if((*it)->Type().IsObjectType(BaseArea::StaticObjectType))
            {
                smptr(BaseArea) base_area(*it);
                base_area->OnDraw(GfxContext, true);
            }
            else if((*it)->Type().IsObjectType(HLayout::StaticObjectType))
            {
                smptr(HLayout) layout(*it);
                layout->ProcessDraw(GfxContext, true);
            }
            else if((*it)->Type().IsObjectType(VLayout::StaticObjectType))
            {
                smptr(VLayout) layout(*it);
                layout->ProcessDraw(GfxContext, true);
            }
        }
        else
        {
            if((*it)->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
            {
                smptr(ActiveInterfaceObject) ic(*it);
                ic->ProcessDraw(GfxContext, false);
            }
            else if((*it)->Type().IsObjectType(BaseArea::StaticObjectType))
            {
                smptr(BaseArea) base_area(*it);
                base_area->OnDraw(GfxContext, false);
            }
            else if((*it)->Type().IsObjectType(HLayout::StaticObjectType))
            {
                smptr(HLayout) layout(*it);
                layout->ProcessDraw(GfxContext, false);
            }
            else if((*it)->Type().IsObjectType(VLayout::StaticObjectType))
            {
                smptr(VLayout) layout(*it);
                layout->ProcessDraw(GfxContext, false);
            }
        }
        GfxContext.PopClippingRectangle();
    }
    GfxContext.PopClippingRectangle();
}

void HSplitter::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void HSplitter::OverlayDrawing(GraphicsContext& GfxContext)
{
    t_u32 num_element = (t_u32)m_SplitterObject.size();

    Geometry base = GetGeometry();
    if(m_focus_splitter_index >= 0)
    {
        Geometry geo = m_SplitterObject[m_focus_splitter_index]->GetGeometry();
        geo.OffsetPosition(mvt_dx, mvt_dy);

        if(m_focus_splitter_index == 0 && num_element > 1)
        {   
            if(geo.y < base.y)
            {
                geo.SetY(base.y);
            }
        
            if(geo.y  + HSPLITTERHEIGHT > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().y)
            {
                geo.SetY(m_SplitterObject[m_focus_splitter_index+1]->GetGeometry().y - HSPLITTERHEIGHT);
            }
        }

        if((m_focus_splitter_index > 0) && (m_focus_splitter_index < (t_s32)num_element - 1))
        {   
            if(geo.y < m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().y + HSPLITTERHEIGHT)
            {
                geo.SetY(m_SplitterObject[m_focus_splitter_index - 1]->GetGeometry().y + HSPLITTERHEIGHT);
            }

            if(geo.y + HSPLITTERHEIGHT > m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().y)
            {
                geo.SetY(m_SplitterObject[m_focus_splitter_index + 1]->GetGeometry().y - HSPLITTERHEIGHT);
            }
        }

        GfxContext.GetRenderStates().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        {
            gPainter.Paint2DQuadColor(GfxContext, geo, Color(0xBB868686));
        }
        GfxContext.GetRenderStates().SetBlend(false);
    }
}

void HSplitter::AddWidget(smptr(BaseObject) ic, float stretchfactor)
{
    if(ic.IsValid())
    {
        MySplitter *splitter = new MySplitter;
        splitter->SinkReference();
        t_u32 no = (t_u32)m_InterfaceObject.size();
        splitter->OnMouseDown.connect(sigc::bind( sigc::mem_fun(this, &HSplitter::OnSplitterMouseDown), no));
        splitter->OnMouseDrag.connect(sigc::bind( sigc::mem_fun(this, &HSplitter::OnSplitterMouseDrag), no));
        splitter->OnMouseUp.connect(sigc::bind( sigc::mem_fun(this, &HSplitter::OnSplitterMouseUp), no));

        ic->SetParentObject(smptr(BaseObject)(this, true));
        m_InterfaceObject.push_back(ic);
        m_SplitterObject.push_back(splitter);
        m_SplitConfig.push_back(stretchfactor);

        new_addition = true;
        ComputeChildLayout();
    }
}

void HSplitter::clearContent()
{
    m_layout->Clear();
    m_InterfaceObject.clear();
}

long HSplitter::ComputeChildLayout()
{
    t_u32 num_element = (t_u32)m_InterfaceObject.size();
    t_s32 x = GetBaseX();
    t_s32 y = GetBaseY();
    t_s32 w = GetBaseWidth();
    t_s32 h = GetBaseHeight();

    if((w == 0) || (h == 0))
    {
        return eCompliantHeight | eCompliantWidth;
    }

    if(num_element < 1)
    {
        m_current_height = h;
        m_current_width = w;
        m_current_x = x;
        m_current_y = y;
    
        new_addition = false;
        return eCompliantHeight | eCompliantWidth;
    }

    std::vector<smptr(BaseObject)>::iterator it;
    std::vector<MySplitter*>::iterator it_splitter;

    if(new_addition)
    {
        ResetSplitConfig();
        new_addition = false;
    }
    
    if(m_current_width != w)
    {
        for(t_u32 i = 0; i < num_element; i++)
        {
            Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
            splitter_geo.SetWidth(w);
            splitter_geo.SetX(x);

            m_SplitterObject[i]->setGeometry(splitter_geo);
        }
    }

    if(m_current_height != h)
    {
        t_s32 size_to_distribute = h - num_element * HSPLITTERHEIGHT;
        t_s32 previous_spliter_end = m_current_y;
        t_s32 new_spliter_end = y;
        for(t_u32 i = 0; i < num_element; i++)
        {
            Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();
            // compute percentage of space occupied by the element i;
            // width of element i = m_SplitterObject[i]->GetY() - previous_splliter_end
            t_s32 splitter_start = m_SplitterObject[i]->GetBaseY();
            float percent = float(splitter_start - previous_spliter_end) / float(m_current_height - num_element * HSPLITTERHEIGHT);
            if(percent > 1.0f)
                percent = 1.0f;

            splitter_geo.SetY(new_spliter_end + size_to_distribute * percent);
            previous_spliter_end = splitter_start + HSPLITTERHEIGHT;
            new_spliter_end = new_spliter_end + size_to_distribute * percent + HSPLITTERHEIGHT;
            m_SplitterObject[i]->setGeometry(splitter_geo);
        }
        if(m_SplitterObject[0]->GetBaseY() < y)
        {
            m_SplitterObject[0]->SetBaseY(y);
        }
        m_SplitterObject[num_element-1]->SetBaseY(y + h - HSPLITTERHEIGHT);
    }

    t_s32 accheight = y;

    for(t_u32 i = 0; i < num_element; i++)
    {
        Geometry splitter_geo = m_SplitterObject[i]->GetGeometry();

        //m_InterfaceObject[i]->setGeometry(Geometry(x, accheight, w, splitter_geo.y - accheight));

        if(m_InterfaceObject[i]->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
        {
            smptr(ActiveInterfaceObject) ic = (m_InterfaceObject[i]);
            ic->setGeometry(Geometry(x, accheight, w, splitter_geo.y - accheight));
            // if we are already computing the layout from the main window down, we need to call 
            // ComputeElementLayout to force the computing of this element layout.
            GetGraphicsThread()->ComputeElementLayout(ic);
        }
        else if(m_InterfaceObject[i]->Type().IsObjectType(BaseArea::StaticObjectType))
        {
            smptr(BaseArea) base_area(m_InterfaceObject[i]);
            base_area->setGeometry(Geometry(x, accheight, w, splitter_geo.y - accheight));
        }
        else if(m_InterfaceObject[i]->Type().IsDerivedFromType(Layout::StaticObjectType))
        {
            smptr(Layout) layout =  (m_InterfaceObject[i]);
            layout->setGeometry(Geometry(x, accheight, w, splitter_geo.y - accheight));
            // if we are already computing the layout from the main window down, we need to call 
            // ComputeElementLayout to force the computing of this element layout.
            GetGraphicsThread()->ComputeElementLayout(layout);
        }
        accheight += splitter_geo.y - accheight + HSPLITTERHEIGHT;
    }

    m_current_height = h;
    m_current_width = w;
    m_current_x = x;
    m_current_y = y;

    return eCompliantHeight | eCompliantWidth;
}

void HSplitter::ResetSplitConfig()
{

    t_s32 x = GetBaseX();
    t_s32 y = GetBaseY();
    t_s32 w = GetBaseWidth();
    t_s32 h = GetBaseHeight();
    t_u32 num_element = (t_u32)m_InterfaceObject.size();

    if(num_element < 1)
    {
        return;
    }

    float max_stretch = 0.0f;
    float stretchfactor;
    for(t_u32 i = 0; i < (t_u32)m_SplitConfig.size(); i++)
    {
        stretchfactor = m_SplitConfig[i];
        if(max_stretch < stretchfactor)
        {
            max_stretch = stretchfactor;
        }
    }

    float total = 0;
    for(t_u32 i = 0; i < (t_u32)m_SplitConfig.size(); i++)
    {
        stretchfactor = m_SplitConfig[i];
        total += stretchfactor/ max_stretch;
    }

    t_s32 availableheight = (h - num_element*HSPLITTERHEIGHT);
    float max_size = float(availableheight) / total;
    for(t_u32 i = 0; i < (t_u32)m_SplitConfig.size(); i++)
    {
        stretchfactor = m_SplitConfig[i];
        y += stretchfactor * max_size / max_stretch;
        Geometry geo(x, y, w, HSPLITTERHEIGHT);
        m_SplitterObject[i]->setGeometry(geo);
    }
    m_SplitterObject[num_element-1]->SetBaseX(y + h - HSPLITTERHEIGHT);

    m_initial_config = true;
}

void HSplitter::OnSplitterMouseDown(t_s32 x, t_s32 y, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos)
{
    m_point.Set(x, y);

    m_focus_splitter_index = header_pos;
    GetThreadWindowCompositor().SetWidgetDrawingOverlay(smptr(BaseArea)(this, true), GetThreadWindowCompositor().GetCurrentWindow());


    // Hint for the window to initiate a redraw
    GetGraphicsThread()->RequestRedraw();
}

void HSplitter::OnSplitterMouseUp(t_s32 x, t_s32 y, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos)
{
    if(mvt_dy)
    {
        Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
        geo.OffsetPosition(0, mvt_dy);

        t_u32 num_element = (t_u32)m_SplitterObject.size();
        if(header_pos < (t_s32)num_element - 1)
        {
            // Make the splitter bar stick to the next one if the distance between them is less than HSTICK_SIZE.
            if(m_SplitterObject[header_pos + 1]->GetGeometry().y - geo.y - HSPLITTERHEIGHT < HSTICK_SIZE)
                geo.SetY( m_SplitterObject[header_pos + 1]->GetGeometry().y - HSPLITTERHEIGHT );
        }
        m_SplitterObject[header_pos]->setGeometry(geo);
        ResizeSplitter(header_pos);
    }

    m_focus_splitter_index = -1; 
    mvt_dx = 0;
    mvt_dy = 0;

    // End overlay drawing;
    GetThreadWindowCompositor().SetWidgetDrawingOverlay(smptr(BaseArea)(0), GetThreadWindowCompositor().GetCurrentWindow());
    // Hint for the window to initiate a redraw
    GetGraphicsThread()->RequestRedraw();
}

void HSplitter::OnSplitterMouseDrag(t_s32 x, t_s32 y, t_s32 dx, t_s32 dy, unsigned long button_flags, unsigned long key_flags, t_s32 header_pos)
{
    bool recompute = false;
    Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
    t_s32 num_element = (t_s32)m_SplitterObject.size();

    if(header_pos == num_element - 1)
    {
        // The last splitter cannot be moved
        return;
    }

    recompute = true;
    mvt_dx = 0;
    mvt_dy = (y - m_point.y);

    if(m_ResizeOnSplitterRelease == false)
    {
        // Continuously redraw resize and redraw the 2 parts of the widget.
        // This is slow.
        geo.OffsetPosition(mvt_dx, mvt_dy);
        m_SplitterObject[header_pos]->setGeometry(geo);
        ResizeSplitter(header_pos);

        mvt_dx = 0;
        mvt_dy = 0;
    }

    // Hint for the window to initiate a redraw
    GetGraphicsThread()->RequestRedraw();
}

void HSplitter::ResizeSplitter(t_s32 header_pos)
{
    Geometry geo = m_SplitterObject[header_pos]->GetGeometry();
    t_s32 num_element = (t_s32)m_SplitterObject.size();

    if((header_pos == 0) && (m_SplitterObject[header_pos]->GetBaseY() < GetBaseY()))
    {
        m_SplitterObject[header_pos]->SetBaseY(GetBaseY());
    }

    if((header_pos == num_element -1) && (m_SplitterObject[header_pos]->GetBaseY() > GetBaseY() + GetBaseHeight() - HSPLITTERHEIGHT))
    {
        m_SplitterObject[header_pos]->SetBaseY(GetBaseY() + GetBaseHeight() - HSPLITTERHEIGHT);
    }
    if(header_pos < (t_s32)num_element - 1)
    {
        t_s32 posy0, posy1;
        posy0 = m_SplitterObject[header_pos]->GetBaseY();
        posy1 = m_SplitterObject[header_pos + 1]->GetBaseY();

        if(posy0 > posy1 - HSPLITTERHEIGHT)
        {
            posy0 = posy1 - HSPLITTERHEIGHT;
            m_SplitterObject[header_pos]->SetBaseY(posy0);
        }
    }

    if(0 < header_pos)
    {
        t_s32 posy0, posy1;
        posy0 = m_SplitterObject[header_pos]->GetBaseY();
        posy1 = m_SplitterObject[header_pos - 1]->GetBaseY();

        if(posy0 < posy1 + HSPLITTERHEIGHT)
        {
            posy0 = posy1 + HSPLITTERHEIGHT;
            m_SplitterObject[header_pos]->SetBaseY(posy0);
        }
    }

    ComputeChildLayout();
    NeedRedraw();
}

// HSplitter need to re implement DoneRedraw because it does not 
// have a m_compositionlayout where its child are located;
void HSplitter::DoneRedraw()
{
    std::vector<smptr(BaseObject)>::iterator it;
    for(it = m_InterfaceObject.begin(); it != m_InterfaceObject.end(); it++)
    {
        //(*it)->DoneRedraw();
        if((*it)->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
        {
            smptr(ActiveInterfaceObject) ic = (*it);
            ic->DoneRedraw();
        }
        else if((*it)->Type().IsObjectType(BaseArea::StaticObjectType))
        {
            smptr(BaseArea) base_area(*it);
        }
    }
}

} //NUX_NAMESPACE_END
