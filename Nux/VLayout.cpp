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
#include "InterfaceControl.h"
#include "VLayout.h"
#include "HLayout.h"

namespace nux { //NUX_NAMESPACE_BEGIN

static const t_s32 VERROR = 0;
NUX_IMPLEMENT_OBJECT_TYPE(VLayout);

VLayout::VLayout(NUX_FILE_LINE_DECL)
:   Layout(NUX_FILE_LINE_PARAM)
{
#if DEBUG_LAYOUT
    m_h_in_margin = 10;
    m_h_out_margin = 10;
    m_v_in_margin = 10;
    m_v_out_margin = 10;
#endif

    // Start packing the elements from the left size. Is the layout has more space than the elements can use,
    // leave that space on the right side of the VLayout.
    m_ContentStacking = eStackTop;

}

VLayout::VLayout(NString name, NUX_FILE_LINE_DECL)
:   Layout(NUX_FILE_LINE_PARAM)
{
    m_name = name;
#if DEBUG_LAYOUT
    m_h_in_margin = 10;
    m_h_out_margin = 10;
    m_v_in_margin = 10;
    m_v_out_margin = 10;
#endif

    m_ContentStacking = eStackTop;
}

VLayout::~VLayout()
{
}

void VLayout::GetCompositeList(std::list<Area*> *InterfaceControlList)
{
    std::list<Area*>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it)->IsInterfaceControl())
        {
            ActiveInterfaceObject* ic = NUX_STATIC_CAST(ActiveInterfaceObject*, (*it));
            InterfaceControlList->push_back(ic);
        }
        else if((*it)->IsLayout())
        {
            Layout* layout = NUX_STATIC_CAST(Layout*, (*it));
            layout->GetCompositeList(InterfaceControlList);
        }
    }
}

//! Compute the how elements are spread inside the layout
/*!
    @param remaining_height Height that remains after subtracting elements height, inner and outer margins from the content height.
    @param offset_space     The space at the top of all elements.
    @param element_margin   The margin between elements.
*/
void VLayout::ComputeStacking(t_s32 remaining_height, t_s32 &offset_space, t_s32 &element_margin)
{
    t_s32 per_element_space = 0;
    t_s32 total_used_space = 0;

    std::list<Area*>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        // gather all the space used by elements
        total_used_space += (*it)->GetBaseHeight();
    }
    if(m_LayoutElementList.size() != 0)
    {
        // Compute the space available for each element
        per_element_space = (remaining_height - total_used_space) / t_s32(m_LayoutElementList.size());
    }
    if(per_element_space < 0)
    {
        per_element_space = 0;
    }
    t_s32 margin;
    if(per_element_space > 0)
    {
        margin = (per_element_space) / 2;
    }
    else
    {
        margin = 0;
    }

    LayoutContentDistribution stacking = GetContentDistribution();
    switch(stacking)
    {
    case eStackTop:
    case eStackLeft:
        {
            offset_space = 0;
            element_margin = 0;
        }
        break;

    case eStackBottom:
    case eStackRight:
        {
            offset_space = (remaining_height - total_used_space);
            if(offset_space < 0)
                offset_space = 0;
            element_margin = 0;
        }
        break;

    case eStackCenter:
        {
            offset_space = (remaining_height - total_used_space)/2;
            if(offset_space < 0)
                offset_space = 0;
            element_margin = 0;
        }
        break;

    case eStackExpand:
    default:
        {
            offset_space = 0;
            element_margin = margin;
        }
        break;
    }
}

long VLayout::ComputeLayout2()
{
    if(m_LayoutElementList.size() == 0)
    {
        return eCompliantHeight|eCompliantWidth;
    }

    std::list<Area*>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        (*it)->setOutofBound(false);
    }

    t_s32 original_width = GetBaseWidth();

    if(GetStretchFactor() == 0)
    {
        // The size must exactly fit the children. The parent cannot be larger or smaller 
        // than the total height of its children (+ margins).
        // So set the parent size to Geometry(0,0,1,1) and let the children force it to extend.
        if(GetParentObject() && GetParentObject()->Type().IsObjectType(HLayout::StaticObjectType))
        {
            // The parent if a HLayout. Then a Stretch factor of 0 means this layout has its width set to 1.
            Area::SetBaseWidth(1);
        }
        else if(GetParentObject() && GetParentObject()->Type().IsObjectType(VLayout::StaticObjectType))
        {
            // The parent if a VLayout(same type). Then a Stretch factor of 0 means this layout has its height set to 1.
            Area::SetBaseHeight(1);
        }
        else
        {
            // This is for when a layout is set as a composition layout of an ActiveInterfaceObject and its stretch factor is explicitly set to 0.
            Area::SetBaseWidth(1);
            Area::SetBaseHeight(1);
        }
        //The children will all assume their minimum size.
    }
    else
    {
        // The total size of the children (+ margins) may be smaller or equal to parent size.
    }

    bool unadjusted_layout = false;

    do
    {
        t_size num_element = m_LayoutElementList.size();
        // Get layout Width and Height
        t_s32 width = GetBaseWidth();
        t_s32 height = GetBaseHeight();

        // Remove the margins. This is the real width and height available to the children.
        width -= 2 * m_h_out_margin;
        height -= (t_s32)(num_element - 1) * m_v_in_margin + 2 * m_v_out_margin;

        // Size the children according to their stretch factor.
        VLayoutManagement(width, height);

        // Objects have been resized, now position them.
        t_s32 current_x = GetBaseX() + m_h_out_margin;
        t_s32 current_y = GetBaseY() + m_v_out_margin;

        //t_s32 per_element_space = 0;
        //t_s32 total_used_space = 0;
        t_s32 offset_space = 0;
        t_s32 space_after_element = 0;
        ComputeStacking(height, offset_space, space_after_element);
        current_y += offset_space;

        for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
        {
            current_y += space_after_element;

            (*it)->SetBaseX(current_x);
            (*it)->SetBaseY(current_y);

            eMinorSize extend = (*it)->GetExtend();
            eMinorPosition positioning = (*it)->getPositioning();
            float percentage = (*it)->GetPercentage();

            // Compute the size of an element in the minor dimension (horizontal)
            switch(extend)
            {
            case ePercentage:
                {
                    // The size of the element in the minor dimension is a percentage of layout minor dimension size.
                    // Note that children of the processed element may force it to have a bigger size.
                    t_s32 percentage_width = (width * percentage) / 100.0f;
                    (*it)->SetBaseWidth(percentage_width);
                    break;
                }

            case eMatchContent:
                {
                    // Force the element width to be the minimum has defined with SetMinimumWidth.
                    // The children of this element can force it to get larger.
                    (*it)->SetBaseWidth(0);
                    break;
                }

            case eFix:
                {
                    //do nothing
                    break;
                }

            case eFull:
            default:
                {
                    (*it)->SetBaseWidth(width);
                    break;
                }
            }

            // Compute the position of an element in the minor dimension.
            if((extend != eFull) || ((*it)->GetBaseWidth() < width))
            {
                t_s32 widget_width = (*it)->GetBaseWidth();

                switch(positioning)
                {
                case eLeft:
                    {
                        // do nothing
                        (*it)->SetBaseX(current_x);
                        break;
                    }
                case eRight:
                    {
                        if(widget_width < width)
                            (*it)->SetBaseX(current_x + width - widget_width);
                        else
                            (*it)->SetBaseX(current_x);
                        break;
                    }

                case eCenter:
                default:
                    {
                        if(widget_width < width)
                            (*it)->SetBaseX(current_x + (width - widget_width) / 2);
                        else
                            (*it)->SetBaseX(current_x);
                    }
                }
            }
            current_y += (*it)->GetBaseHeight() + space_after_element + m_v_in_margin;
        }

        // Manage child layout
        m_contentWidth = GetBaseWidth() - 2 * m_h_out_margin; // Set to the size of the layout.
        if(num_element == 0)
            m_fittingHeight = (t_s32)2 * m_v_out_margin;
        else
            m_fittingHeight = (t_s32)(num_element - 1) * m_v_in_margin + 2 * m_v_out_margin;

        t_s32 element_width;
        unadjusted_layout = false;

        // This array is meant to store the sizes of some of the elements width. These elements must have eFull as extent and
        // they have not been not been constrained smaller after ComputeLayout2 is called. Because the layout may have a stretch factor of 0
        // and therefore its size has been set to 1x1 at the start of this function, there is a possibility that some of the elements don't have 
        // the full width of the layout(these elements uses their minimum width because the layout was set to a size 1x1).
        // We check if that is the case and force a recompute.
        std::vector<t_s32> FullSizeUnadjusted;

        for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
        {
            bool largerHeight = false;
            bool smallerHeight = false;
            bool largerWidth = false;
            bool smallerWidth = false;
            t_s32 ret = 0;
            if(((*it)->IsLayout() || (*it)->IsInterfaceControl()) /*&& ((*it)->isOutofBound() == false)*/ /*&& ((*it)->GetStretchFactor() != 0)*/)
            {
                if((*it)->IsLayout())
                {
                    Layout* layout = NUX_STATIC_CAST(Layout*, (*it));
                    layout->SetDirty(true);
                }

                ret = (*it)->ComputeLayout2();

                largerHeight = (ret & eLargerHeight)? true : false;
                smallerHeight = (ret & eSmallerHeight)? true : false;
                largerWidth = (ret & eLargerWidth)? true : false;
                smallerWidth = (ret & eSmallerWidth)? true : false;
                if((largerHeight || smallerHeight) && ((*it)->isOutofBound() == false) )
                {
                    // Stop computing the size of this layout. Its size was not convenient to its children. So the children size take priority
                    // over the layout. In ComputeLayout2, the dimension of the layout has been set so it encompasses its children (and the margins).
                    // Now the parent layout cannot be touched again: outofbound = true. In VLayoutManagement, it is as if the stretchfactor
                    // of this layout is now 0.
                    // This is the only place where a layout can have outofbound set to "true".

                    // If (smallerHeight == true) the layout takes less space than anticipated.
                    // Set unadjusted_layout = true, so another pass will allow its sibling to claim more space.
                    {
                        unadjusted_layout = true;
                        (*it)->setOutofBound(true);
                    }
                }
                if((smallerWidth == false) && ((*it)->GetExtend() == eFull) && ((*it)->GetBaseWidth() < (*it)->GetMaximumWidth()) )
                {
                    // We catch all object whose size is possibly larger than the layout. We check there size at the end and
                    // recompute the layout if necessary.
                    // For layout elements, make sure that the stretch factor is not 0. If it is, it means it will not use the 
                    // size provided by the parent layout. Its size will be adjusted to the minimum size of the layout content.
                    if(!((*it)->IsLayout() && (*it)->m_stretchfactor == 0))
                        FullSizeUnadjusted.push_back((*it)->GetBaseWidth());
                }
                // Should be reactivate so that if the parent Layout does not call
                // ComputePosition2, at least it is done here to arrange the internal 
                // element of the children.
                //(*it)->ComputePosition2(0,0);
            }

            m_fittingHeight += (*it)->GetBaseHeight();

            element_width = (*it)->GetBaseWidth();            
            if((*it)->IsSpaceLayout() == false)
            {
                if((GetStretchFactor() != 0) && (ret & eSmallerWidth))
                {
                    if(m_contentWidth < element_width)
                    {
                        if(m_contentWidth < GetMaximumHeight())
                        {
                            // An element is larger than the layout width and the layout has not reach its maximum width yet.
                            m_contentWidth = element_width;
                            unadjusted_layout = true;
                        }
                    }
                }
                else
                {
                    if(m_contentWidth <= element_width)
                    {
                        m_contentWidth = element_width;
                    }
//                    else if((*it)->GetExtend() == eFull)
//                    {
//                        unadjusted_layout = true;
//                    }
                }
            }
        }

        // Set the width of the layout to be equal to the largest width it controls.
        // m_contentWidth is the largest width of an element within this layout. The layout size is then
        // m_contentWidth + 2 * m_h_out_margin;
        SetBaseWidth(m_contentWidth + 2 * m_h_out_margin);

        t_s32 temp = m_contentWidth;
        std::vector<t_s32>::iterator IntIterator = FullSizeUnadjusted.begin();
        for(IntIterator = FullSizeUnadjusted.begin(); IntIterator != FullSizeUnadjusted.end(); IntIterator++)
        {
            if((*IntIterator) < temp)
            {
                unadjusted_layout = true;
            }
        }
    } while(unadjusted_layout);

    // m_fittingWidth is sum of the largest element width plus the outer margin.
    m_fittingWidth = m_contentWidth + 2 * m_h_out_margin;
    // m_fittingHeight is sum of the element height plus the inner and outer margin.
    // If the stretch factor is 0 then, the layout height has been set to 1 and need to grow with the elements within.
    // If the stretch factor is not 0, then the layout height is set to m_fittingHeight only if the its height is less than m_fittingHeight;
    // This way, if the layout height is greater than m_fittingHeight there is space to compute the content stacking (ComputeStacking).
    if(GetBaseHeight() < m_fittingHeight)
    {
        SetBaseHeight(m_fittingHeight);
    }
    m_contentHeight = m_fittingHeight;
    long size_compliance= 0L;

    ComputePosition2(0,0);

    if(GetBaseWidth() > original_width + VERROR)
    {
#if DEBUG_LAYOUT_COMPUTATION
        // The layout has been resized larger in WIDTH to tightly pack its content.
        // Or you can say that the layout refuse to be smaller than total WIDTH of its elements.
        std::cout << "ComputeLayout2: VLayout Width block at " << GetWidth() << std::endl;
#endif
        size_compliance |= eLargerWidth; // need scrollbar
    }
    else if(GetBaseWidth() + VERROR < original_width)
    {
#if DEBUG_LAYOUT_COMPUTATION
        // The layout is smaller.
        std::cout << "ComputeLayout2: VLayout Width smaller = " << GetWidth() << std::endl;
#endif
        size_compliance |= eSmallerWidth;
    }
    else
    {
#if DEBUG_LAYOUT_COMPUTATION
        // The layout and its content resized together without trouble.
        std::cout << "ComputeLayout2: VLayout Width compliant = " << GetWidth() << std::endl;
#endif
        size_compliance |= eCompliantWidth;
    }

    {
#if DEBUG_LAYOUT_COMPUTATION
        // The layout and its content resized together without trouble.
        std::cout << "ComputeLayout2: VLayout Height compliant = " << m_fittingHeight << std::endl;
#endif
        size_compliance |= eCompliantHeight;
    }

    SetDirty(false);
    return size_compliance;
}

void VLayout::VLayoutManagement(t_s32 width, t_s32 height)
{
    bool need_recompute = false;
    do
    {
        t_s32 available_height = height;
        t_u32 max_stretchfactor = getMaxStretchFactor();
        std::list<Area*>::iterator it;

        for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
        {
            if(((*it)->GetStretchFactor() == 0) && ((*it)->isOutofBound() != true))
            {
                (*it)->ApplyMinHeight();
            }
        }

        if(max_stretchfactor == 0) 
        {
            // It means all object are fixed sized or all resizable object have been sized to there max or there min.
            return;
        }

        for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
        {
            if((*it)->GetStretchFactor() == 0 || (*it)->isOutofBound() == true)
            {
                available_height -= (*it)->GetBaseHeight();
            }
        }

        if(available_height <= 2)
        {
            for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
            {
                if(((*it)->GetStretchFactor() != 0) && (*it)->IsArea())
                {
                    // If it is not an object of type eInputArea, do not set outofbound to true,
                    // so, the layout management function will later be called on the object.
                    (*it)->ApplyMinHeight();
                    (*it)->setOutofBound(true);
                }
                else if(((*it)->GetStretchFactor() != 0) && ((*it)->IsLayout()) && ((*it)->isOutofBound() == false))
                {
                    // The out of bound must be reset to false.
                    (*it)->ApplyMinHeight();
                    (*it)->setOutofBound(false);
                }
                else if(((*it)->GetStretchFactor() != 0)  && ((*it)->isOutofBound() == false))
                {
                    (*it)->ApplyMinHeight();
                    // A layout must never have outofbound set to true "here" because it must continue
                    // doing the layout of its children and finally resize itself to fit them.
                    // The same way, A layout size factor should never be set to 0.
                }
            }
            return;
        }

        float cumul = 0;
        Area* LastElementThatCanBeResized = 0;
        t_s32 total_distributed_size = 0;
        for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
        {
            if(((*it)->GetStretchFactor() != 0) && ((*it)->isOutofBound() == false))
            {
                float sf = (float) (*it)->GetStretchFactor();
                cumul += sf / max_stretchfactor;
                LastElementThatCanBeResized = (*it);
            }
            else
            {
                total_distributed_size += (*it)->GetBaseWidth();
            }
        }

        //                  --
        //                  \      factor
        //    Ref_Size *     \  ------------     = available_height
        //                   /   max_factor
        //                  /
        //                  --
        //                  all element with stretchfactor != 0

        t_u32 ref_height = available_height / cumul;

    // 
    need_recompute = false;;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end() && !need_recompute; it++)
    {
        if(((*it)->GetStretchFactor() != 0) && ((*it)->isOutofBound() == false))
        {
            t_u32 sf = (*it)->GetStretchFactor();
            t_u32 new_height;
            if(sf == max_stretchfactor)
            {
                new_height = ref_height;
            }
            else
            {
                new_height = ref_height * sf / max_stretchfactor;
            }

            total_distributed_size += new_height;
            if(LastElementThatCanBeResized == (*it))
            {
                // Redistribute the remaining size to the last element (at the bottom).
                // This is necessary because of imprecision. For instance if available_height = 451 and we have 2 elements
                // with the same stretch factor than each one will have a size of 225. With is correction, the first element will have a size of 
                // 225 while the second one will have a size of 226.
                if(available_height - total_distributed_size > 0)
                {
                    new_height += available_height - total_distributed_size;
                    total_distributed_size = available_height;
                }
            }

            t_u32 elemt_max_height = (*it)->GetMaximumSize().GetHeight();
            t_u32 elemt_min_height = (*it)->GetMinimumSize().GetHeight();

            // A layout must never have outofbound set to true "here" because it must continue
            // doing the layout of its children and finally resize itself to fit them.
            // The same way, A layout size factor should never be set to 0.
            // Q: How about SpaceLayout? Should we treat them the same as layout or widget in this particular case?
            // A: SapceLayout are treated like widgets in this case
            if(new_height < elemt_min_height)
            {
                // assume the minimum width
                (*it)->SetBaseHeight(elemt_min_height);
                if(((*it)->IsLayout() == false) || (*it)->IsSpaceLayout())
                {
                    (*it)->setOutofBound(true);
                    need_recompute = true;
                }
            }
            else if(new_height > elemt_max_height)
            {
                // assume the maximum width
                (*it)->SetBaseHeight(elemt_max_height);
                if(((*it)->IsLayout() == false) || (*it)->IsSpaceLayout())
                {
                    (*it)->setOutofBound(true);
                    need_recompute = true;
                }
//                 else
//                 {
//                     // set the extra space back in the pool
//                     total_distributed_size -= (new_height - elemt_max_height);
//                 }
            }
            else
            {
                (*it)->SetBaseHeight(new_height);
            }
        }
        else
        {
            // For fixed element, reset their size to the same so it is checked against 
            // the min and max. This is necessary in case you have set the size of the element first then latter, 
            // you define its MinimumSize and/or MaximumSize size.
            t_u32 w = (*it)->GetBaseWidth();
            t_u32 h = (*it)->GetBaseHeight();
            (*it)->SetBaseWidth(w);
            (*it)->SetBaseHeight(h);
        }
    }
    }while(need_recompute);
}

t_u32 VLayout::getMaxStretchFactor()
{
    t_u32 value = 0;
    t_u32 sf;
    std::list<Area*>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        // In the recursive process, make sure we get always the highest stretch factor
        // of an element that has not been bounded.
        if((*it)->isOutofBound() == false)
        {
            sf = (*it)->GetStretchFactor(); 
            if(sf >= value)
            {
                value = sf;
            }
        }
    }
    return value;
}

void VLayout::Draw()
{
    // Draw Child Layout
    std::list<Area*>::iterator it;
    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        // Test Space Layout first because it is also a Layout and it will answer true to IsLayout().
        if((*it)->IsSpaceLayout())
        {
            SpaceLayout* spacelyt = NUX_STATIC_CAST(SpaceLayout*, (*it));
            spacelyt->Draw();
        }
        else if((*it)->IsLayout())
        {
            Layout* lyt = NUX_STATIC_CAST(Layout*, (*it));
            lyt->Draw();
        }
        else if((*it)->IsInterfaceControl())
        {
            ActiveInterfaceObject* ic = NUX_STATIC_CAST(ActiveInterfaceObject*, (*it));
            ic->DrawLayout();
        }
        else
        {
            // This is either an Area or and ActiveInterfaceObject
            t_s32 x, y, width, height;
            x = (*it)->GetBaseX();
            y = (*it)->GetBaseY();
            width = (*it)->GetBaseWidth();
            height = (*it)->GetBaseHeight();

            glColor3f(1.0f, 1.0f, 1.0f);
            glDisable(GL_TEXTURE_2D);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glBegin(GL_QUADS);
            {
                glVertex3i(x,  y, 1);
                glVertex3i(x+width,  y, 1);
                glVertex3i(x+width,  y+height, 1);
                glVertex3i(x,  y+height, 1);
            }
            glEnd();
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glEnable(GL_TEXTURE_2D);
        }
    }

    // Draw this Layout border
    t_s32 x, y, width, height;
    x = GetBaseX();
    y = GetBaseY();
    width = GetBaseWidth();
    height = GetBaseHeight();

    glColor3f(0.0f, 0.0f, 1.0f);
    glDisable(GL_TEXTURE_2D);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glBegin(GL_QUADS);
    {
        glVertex3i(x,  y, 1);
        glVertex3i(x+width,  y, 1);
        glVertex3i(x+width,  y+height, 1);
        glVertex3i(x,  y+height, 1);
    }
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_TEXTURE_2D);
}


void VLayout::ComputePosition2(float offsetX, float offsetY)
{
    std::list<Area*>::iterator it;
    {
        t_u32 num_element = (t_u32) m_LayoutElementList.size();
        // Get layout Width and Height
        t_s32 width = GetBaseWidth();
        t_s32 height = GetBaseHeight();
        // remove the margins
        width -= 2 * m_h_out_margin;
        height -= (t_s32)(num_element - 1) * m_v_in_margin + 2 * m_v_out_margin;

        // Objects have been resized, now position them.
        t_s32 current_x = GetBaseX() + m_h_out_margin + offsetX; // add base offset in X (used for scrolling)
        t_s32 current_y = GetBaseY() + m_v_out_margin + offsetY; // add base offset in Y (used for scrolling)

        t_s32 offset_space = 0;
        t_s32 element_margin = 0;
        ComputeStacking(height, offset_space, element_margin);
        current_y += offset_space;

        for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
        {
            current_y += element_margin;

            (*it)->SetBaseX(current_x);
            (*it)->SetBaseY(current_y);

            eMinorSize extend = (*it)->GetExtend();
            eMinorPosition positioning = (*it)->getPositioning();

            if((extend != eFull) || ((*it)->GetBaseWidth() < width))
            {
                t_s32 widget_width = (*it)->GetBaseWidth();

                switch(positioning)
                {
                case eLeft:
                    {
                        // do nothing
                        (*it)->SetBaseX(current_x);
                        break;
                    }
                case eRight:
                    {
                        if(widget_width < width)
                            (*it)->SetBaseX(current_x + width - widget_width);
                        else
                            (*it)->SetBaseX(current_x);
                        break;
                    }

                case eCenter:
                default:
                    {
                        if(widget_width < width)
                            (*it)->SetBaseX(current_x + (width - widget_width) / 2);
                        else
                            (*it)->SetBaseX(current_x);
                    }
                }
            }
            current_y += (*it)->GetBaseHeight() + element_margin + m_v_in_margin;
        }
    }

    for(it = m_LayoutElementList.begin(); it != m_LayoutElementList.end(); it++)
    {
        if((*it)->Type().IsDerivedFromType(Layout::StaticObjectType))
        {
            (*it)->ComputePosition2(offsetX, offsetY);
        }
        else if((*it)->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
        {
            (*it)->PositionChildLayout(offsetX, offsetY);
        }

    }
}


} //NUX_NAMESPACE_END
