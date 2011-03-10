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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "View.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux
{

  static const t_s32 HERROR = 0;
  NUX_IMPLEMENT_OBJECT_TYPE (HLayout);

  HLayout::HLayout (NUX_FILE_LINE_DECL)
    :   Layout (NUX_FILE_LINE_PARAM)
  {
#if DEBUG_LAYOUT
    m_h_in_margin = 10;
    m_h_out_margin = 10;
    m_v_in_margin = 10;
    m_v_out_margin = 10;
#endif

    // Start packing the elements from the top. Is the layout has more space than the elements can use,
    // leave that space at the bottom of the HLayout.
    m_ContentStacking = eStackLeft;
  }

  HLayout::HLayout (NString name, NUX_FILE_LINE_DECL)
    :   Layout (NUX_FILE_LINE_PARAM)
  {
    m_name = name;
#if DEBUG_LAYOUT
    m_h_in_margin = 10;
    m_h_out_margin = 10;
    m_v_in_margin = 10;
    m_v_out_margin = 10;
#endif

    m_ContentStacking = eStackLeft;
  }

  HLayout::~HLayout()
  {
  }

  //up and down should pass event to parent
  long HLayout::DoFocusUp (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    Area *parent = GetParentObject ();
    if (parent != NULL)
      return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
    else
      FocusFirstChild ();

    return TraverseInfo;
  }
  long HLayout::DoFocusDown (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    g_debug ("got do focus down in hlayout, going up!");
    
    Area *parent = GetParentObject ();
    if (parent != NULL)
      return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
    else
      FocusFirstChild ();
    return TraverseInfo;
  }

  void HLayout::GetCompositeList (std::list<Area *> *ViewList)
  {
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ( (*it)->IsView() )
      {
        View *ic = NUX_STATIC_CAST (View *, (*it) );
        ViewList->push_back (ic);
      }
      else if ( (*it)->IsLayout() )
      {
        Layout *layout = NUX_STATIC_CAST (Layout *, (*it) );
        layout->GetCompositeList (ViewList);
      }
    }
  }

  void HLayout::ComputeStacking (t_s32 remaining_width, t_s32 &offset_space, t_s32 &element_margin)
  {
    t_s32 per_element_space = 0;
    t_s32 total_used_space = 0;
    t_s32 num_elements = 0;

    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsVisible ())
      {
        // gather all the space used by elements
        total_used_space += (*it)->GetBaseWidth();
        num_elements++;
      }
    }

    if (num_elements)
    {
      // Compute the space available for each element
      per_element_space = (remaining_width - total_used_space) / t_s32 (num_elements);
    }

    if (per_element_space < 0)
    {
      per_element_space = 0;
    }

    t_s32 margin;

    if (per_element_space > 0)
    {
      margin = (per_element_space) / 2;
    }
    else
    {
      margin = 0;
    }

    LayoutContentDistribution stacking = GetContentDistribution();

    switch (stacking)
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
        offset_space = (remaining_width - total_used_space);

        if (offset_space < 0)
          offset_space = 0;

        element_margin = 0;
      }
      break;

      case eStackCenter:
      {
        offset_space = (remaining_width - total_used_space) / 2;

        if (offset_space < 0)
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

  long HLayout::ComputeLayout2()
  {
    if (_layout_element_list.size() == 0)
    {
      return eCompliantHeight | eCompliantWidth;
    }

    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsVisible ())
        (*it)->SetLayoutDone (false);
    }

    t_s32 original_height = GetBaseHeight();

    if (GetStretchFactor() == 0)
    {
      // The size must exactly fit the children. The parent cannot be larger or smaller
      // than the total width of its children (+ margins).
      // So set the parent size to Geometry(0,0,1,1) and let the children force it to extend.
      if (GetParentObject() && GetParentObject()->Type().IsObjectType (HLayout::StaticObjectType) )
      {
        // The parent if a HLayout(same type). Then a Stretch factor of 0 means this layout has its width set to 1.
        Area::SetBaseWidth (1);
      }
      else if (GetParentObject() && GetParentObject()->Type().IsObjectType (VLayout::StaticObjectType) )
      {
        // The parent if a VLayout. Then a Stretch factor of 0 means this layout has its height set to 1.
        Area::SetBaseHeight (1);
      }
      else
      {
        // This is for when a layout is set as a composition layout of an View and its stretch factor is explicitly set to 0.
        Area::SetBaseWidth (1);
        Area::SetBaseHeight (1);
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
      t_u32 num_element = 0;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if ((*it)->IsVisible ())
          num_element++;
      }
     
      // Get layout Width and Height
      t_s32 width = GetBaseWidth();
      t_s32 height = GetBaseHeight();

      // Remove the margins. This is the real width and height available to the children.
      width -= (t_s32) (num_element - 1) * m_h_in_margin + 2 * m_h_out_margin;
      height -= 2 * m_v_out_margin;

      // Size the children according to their stretch factor.
      HLayoutManagement (width, height);

      // Objects have been resized, now position them.
      t_s32 current_x = GetBaseX() + m_h_out_margin;
      t_s32 current_y = GetBaseY() + m_v_out_margin;

      t_s32 offset_space = 0;
      t_s32 space_after_element = 0;
      ComputeStacking (width, offset_space, space_after_element);
      current_x += offset_space;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible ())
          continue;

        current_x += space_after_element;

        (*it)->SetBaseX (current_x);
        (*it)->SetBaseY (current_y);

        MinorDimensionSize extend = (*it)->GetExtend();
        MinorDimensionPosition positioning = (*it)->GetPositioning();
        float percentage = (*it)->GetPercentage();

        // Compute the size of an ellement in the minor dimension (vertical)
        switch (extend)
        {
          case ePercentage:
          {
            // The size of the processed element in the minor dimension is a percentage of layout minor dimension size.
            // Note that children of the processed element may force it to have a bigger size.
            t_s32 percentage_height = (height * percentage) / 100.0f;
            (*it)->SetBaseHeight (percentage_height);
            break;
          }

          case eMatchContent:
          {
            // Force the element height to be the minimum has defined with SetMinimumHeight.
            // The children of this element can force it to get larger.
            (*it)->SetBaseHeight (0);
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
            (*it)->SetBaseHeight (height);
            break;
          }
        }

        // Compute the position of an element in the minor dimension.
        if ( (extend != eFull) || ( (*it)->GetBaseHeight() < height) )
        {
          t_s32 widget_height = (*it)->GetBaseHeight();

          switch (positioning)
          {
            case eAbove:
            {
              // do nothing
              (*it)->SetBaseY (current_y);
              break;
            }
            case eBelow:
            {
              if (widget_height < height)
                (*it)->SetBaseY (current_y + height - widget_height);
              else
                (*it)->SetBaseY (current_y);

              break;
            }

            case eCenter:
            default:
            {
              if (widget_height < height)
                (*it)->SetBaseY (current_y + (height - widget_height) / 2);
              else
                (*it)->SetBaseY (current_y);
            }
          }
        }

        current_x += (*it)->GetBaseWidth() + space_after_element + m_h_in_margin;
      }

      // Manage child layout
      if (num_element == 0)
        m_fittingWidth = (t_s32) 2 * m_h_out_margin;
      else
        m_fittingWidth = (t_s32) (num_element - 1) * m_h_in_margin + 2 * m_h_out_margin;

      m_contentHeight = GetBaseHeight() - 2 * m_v_out_margin; // Set to the size of the layout.

      t_s32 element_height = 0;
      unadjusted_layout = false;

      // This array is meant to store the sizes of some of the elements height. These elements must have eFull as extent and
      // they have not been not been constrained smaller after ComputeLayout2 is called. Because the layout may have a stretch factor of 0
      // and therefore its size has been set to 1x1 at the start of this function, there is a possibility that some of the elements don't have
      // the full height of the layout(these elements uses their minimum height because the layout was set to a size 1x1).
      // We check if that is the case and force a recompute.
      std::vector<t_s32> FullSizeUnadjusted;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible ())
          continue;
        bool largerHeight = false;
        bool smallerHeight = false;
        bool largerWidth = false;
        bool smallerWidth = false;
        t_s32 ret = 0;

        if ( ( (*it)->IsLayout()  || (*it)->IsView() ) /*&& ((*it)->IsLayoutDone() == false)*/ /*&& ((*it)->GetStretchFactor() != 0)*/)
        {
          ret = (*it)->ComputeLayout2();

          largerWidth = (ret & eLargerWidth) ? true : false;
          smallerWidth = (ret & eSmallerWidth) ? true : false;
          largerHeight = (ret & eLargerHeight) ? true : false;
          smallerHeight = (ret & eSmallerHeight) ? true : false;

          if ( (largerWidth || smallerWidth) && ( (*it)->IsLayoutDone() == false) )
          {
            // Stop computing the size of this layout. Its size was not convenient to its children. So the children size take priority
            // over the layout. In ComputeLayout2, the dimension of the layout has been set so it encompasses its children (and the margins).
            // Now the parent layout cannot be touched again: _layout_done = true. In VLayoutManagement, it is as if the stretchfactor
            // of this layout is now 0.
            // This is the only place where a layout can have _layout_done set to "true".

            // If (smallerWidth == true) the layout takes less space than anticipated.
            // Set unadjusted_layout = true, so another pass will allow its sibling to claim more space.

            {
              unadjusted_layout = true;
              (*it)->SetLayoutDone (true);
            }
          }

          if ( (smallerHeight == false) && ( (*it)->GetExtend() == eFull) && ( (*it)->GetBaseHeight() < (*it)->GetMaximumHeight() ) )
          {
            // We catch all object whose size is possibly larger than the layout. We check there size at the end and
            // recompute the layout if necessary.
            // For layout elements, make sure that the stretch factor is not 0. If it is, it means it will not use the
            // size provided by the parent layout. Its size will be adjusted to the minimum size of the layout content.
            if (! ( (*it)->IsLayout() && (*it)->GetStretchFactor() == 0) )
              FullSizeUnadjusted.push_back ( (*it)->GetBaseHeight() );
          }

          // Should be reactivate so that if the parent Layout does not call
          // ComputePosition2, at least it is done here to arrange the internal
          // element of the children.
          //(*it)->ComputePosition2(0,0);
        }

        m_fittingWidth += (*it)->GetBaseWidth();

        element_height = (*it)->GetBaseHeight();

        if ( (*it)->IsSpaceLayout() == false)
        {
          if ( (GetStretchFactor() != 0) /* && (ret & eSmallerHeight)*/)
          {
            if (m_contentHeight < element_height)
            {
              if (m_contentHeight < GetMaximumHeight() )
              {
                // An element is larger than the layout height and the layout has not reach its maximum height yet.
                m_contentHeight = element_height;
                unadjusted_layout = true;
              }
            }
          }
          else
          {
            if (m_contentHeight <= element_height)
            {
              m_contentHeight = element_height;
            }

//                    else if((*it)->GetExtend() == eFull)
//                    {
//                        unadjusted_layout = true;
//                    }
          }
        }
      }

      // Set the height of the layout to be equal to the largest height it controls.
      // m_contentHeight is the largest height of an element within this layout. The layout size is then
      // m_contentHeight + 2 * m_v_out_margin;
      SetBaseHeight (m_contentHeight + 2 * m_v_out_margin);

      t_s32 temp = m_contentHeight;
      std::vector<t_s32>::iterator IntIterator = FullSizeUnadjusted.begin();

      for (IntIterator = FullSizeUnadjusted.begin(); IntIterator != FullSizeUnadjusted.end(); IntIterator++)
      {
        if ( (*IntIterator) < temp)
        {
          unadjusted_layout = true;
        }
      }
    }
    while (unadjusted_layout);

    // m_fittingHeight is sum of the largest element height plus the outer margin.
    m_fittingHeight = m_contentHeight + 2 * m_v_out_margin;

    // m_fittingWidth is sum of the element width plus the inner and outer margin.
    // If the stretch factor is 0 then, the layout height has been set to 1 and need to grow with the elements within.
    // If the stretch factor is not 0, then the layout height is set to m_fittingHeight only if the its height is less than m_fittingHeight;
    // This way, if the layout height is greater than m_fittingHeight there is space to compute the content stacking (ComputeStacking).
    if (GetBaseWidth() < m_fittingWidth)
    {
      SetBaseWidth (m_fittingWidth);
    }

    m_contentWidth = m_fittingWidth;
    long size_compliance = 0L;

    ComputePosition2 (0, 0);

    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeLayout2: HLayout Width compliant = " << m_fittingWidth << std::endl;
#endif
      size_compliance |= eCompliantWidth;
    }

    // The layout has been resized to tightly pack its content
    if (GetBaseHeight() > original_height + HERROR)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout has been resized larger in height to tightly pack its content.
      // Or you can say that the layout refuse to be smaller than total HEIGHT of its elements.
      std::cout << "ComputeLayout2: HLayout Height block at " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eLargerHeight; // need scrollbar
    }
    else if (GetBaseHeight() + HERROR < original_height)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout is smaller.
      std::cout << "ComputeLayout2: HLayout Height is smaller = " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eSmallerHeight;
    }
    else
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeLayout2: HLayout Height compliant = " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eCompliantHeight;
    }

    return size_compliance;
  }

  void HLayout::HLayoutManagement (t_s32 width, t_s32 height)
  {
    bool need_recompute = false;

    do
    {
      need_recompute = false;
      t_s32 available_width = width;
      t_u32 max_stretchfactor = getMaxStretchFactor();
      std::list<Area *>::iterator it;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible ())
          continue;

        if ( ( (*it)->GetStretchFactor() == 0) && ( (*it)->IsLayoutDone() != true) )
        {
          (*it)->ApplyMinWidth();
        }
      }

      if (max_stretchfactor == 0)
      {
        // It means all object are fixed sized or all resizable object have been sized to there max or there min.
        return;
      }

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible ())
          continue;

        if ( (*it)->GetStretchFactor() == 0 || (*it)->IsLayoutDone() == true)
        {
          available_width -= (*it)->GetBaseWidth();
        }
      }

      if (available_width <= 2)
      {
        for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
        {
          if (!(*it)->IsVisible ())
            continue;

          if ( ( (*it)->GetStretchFactor() != 0) && (*it)->IsArea() )
          {
            // If it is not an object of type eInputArea, do not set _layout_done to true,
            // so, the layout management function will later be called on the object.
            (*it)->ApplyMinWidth();
            (*it)->SetLayoutDone (true);
          }
          else if ( ( (*it)->GetStretchFactor() != 0) && ( (*it)->IsLayout() ) && ( (*it)->IsLayoutDone() == false) ) // layout and not fixed by child
          {
            // The out of bound must be reset to false.
            (*it)->ApplyMinWidth();
            (*it)->SetLayoutDone (false);
          }
          else if ( ( (*it)->GetStretchFactor() != 0) && ( (*it)->IsLayoutDone() == false) ) // layout and not fixed
          {
            (*it)->ApplyMinWidth();
            // A layout must never have _layout_done set to true "here" because it must continue
            // doing the layout of its children and finally resize itself to fit them.
            // The same way, A layout size factor should never be set to 0.
          }
        }

        return;
      }

      float cumul = 0;
      Area *LastElementThatCanBeResized = 0;
      t_s32 total_distributed_size = 0;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible ())
          continue;

        if ( ( (*it)->GetStretchFactor() != 0) && ( (*it)->IsLayoutDone() == false) )
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

      //                  -----
      //                  \      factor
      //    Ref_Size *     \  ------------     = available_width
      //                   /   max_factor
      //                  /
      //                  -----
      //                  all element with stretchfactor != 0

      t_u32 ref_width = available_width / cumul;

      need_recompute = false;;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end() && !need_recompute; it++)
      {
        if (!(*it)->IsVisible ())
          continue;
        
        if ( ( (*it)->GetStretchFactor() != 0) && ( (*it)->IsLayoutDone() == false) )
        {
          t_u32 sf = (*it)->GetStretchFactor();
          t_u32 new_width;

          if (sf == max_stretchfactor)
          {
            new_width = ref_width;
          }
          else
          {
            new_width = ref_width * sf / max_stretchfactor;
          }

          total_distributed_size += new_width;

          if (LastElementThatCanBeResized == (*it) )
          {
            // Redistribute the remaining size to the last element (at the right).
            // This is necessary because of imprecision. For instance if available_height = 451 and we have 2 elements
            // with the same stretch factor than each one will have a size of 225. With is correction, the first element will have a size of
            // 225 while the second one will have a size of 226.
            if (available_width - total_distributed_size > 0)
            {
              new_width += available_width - total_distributed_size;
              total_distributed_size = available_width;
            }
          }

          t_u32 elemt_max_width = (*it)->GetMaximumSize().GetWidth();
          t_u32 elemt_min_width = (*it)->GetMinimumSize().GetWidth();

          // A layout must never have _layout_done set to true "here" because it must continue
          // doing the layout of its children and finally resize itself to fit them.
          // The same way, A layout size factor should never be set to 0.
          // Q: How about SpaceLayout? Should we treat them the same as layout or widget in this particular case?
          // A: SapceLayout are treated like widgets in this case

          if (new_width < elemt_min_width)
          {
            // assume the minimum width
            (*it)->SetBaseWidth (elemt_min_width);

            if ( (*it)->IsLayout() == false || (*it)->IsSpaceLayout() )
            {
              (*it)->SetLayoutDone (true);
              need_recompute = true;
            }
          }
          else if (new_width > elemt_max_width)
          {
            // assume the maximum width
            (*it)->SetBaseWidth (elemt_max_width);

            if ( (*it)->IsLayout() == false || (*it)->IsSpaceLayout() )
            {
              (*it)->SetLayoutDone (true);
              need_recompute = true;
            }

//                     else
//                     {
//                         // set the extra space back in the pool
//                         total_distributed_size -= (new_width - elemt_max_width);
//                     }
          }
          else
          {
            (*it)->SetBaseWidth (new_width);
          }
        }
        else
        {
          // For fixed element, reset their size to the same so it is checked against
          // the min and max. This is necessary in case you have set the size of the element first then latter,
          // you define its MinimumSize and/or MaximumSize size.
          t_u32 w = (*it)->GetBaseWidth();
          t_u32 h = (*it)->GetBaseHeight();
          (*it)->SetBaseWidth (w);
          (*it)->SetBaseHeight (h);
        }
      }
    }
    while (need_recompute);
  }

  t_u32 HLayout::getMaxStretchFactor()
  {
    t_u32 value = 0;
    t_u32 sf;
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if (!(*it)->IsVisible ())
        continue;

      // In the recursive process, make sure we get always the highest stretch factor
      // of an element that has not been bounded.
      if ( (*it)->IsLayoutDone() == false)
      {
        sf = (*it)->GetStretchFactor();

        if (sf >= value)
        {
          value = sf;
        }
      }
    }

    return value;
  }

  void HLayout::ComputePosition2 (float offsetX, float offsetY)
  {
    std::list<Area *>::iterator it;
    {
      t_u32 num_element = 0;
      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if ((*it)->IsVisible ())
          num_element++;
      }
     
      // Get layout Width and Height
      t_s32 width = GetBaseWidth();
      t_s32 height = GetBaseHeight();
      // remove the margins
      width -= (t_s32) (num_element - 1) * m_h_in_margin + 2 * m_h_out_margin;
      height -= 2 * m_v_out_margin;

      // Objects have been resized, now position them.
      t_s32 current_x = GetBaseX() + m_h_out_margin + offsetX; // add base offset in X (used for scrolling)
      t_s32 current_y = GetBaseY() + m_v_out_margin + offsetY; // add base offset in Y (used for scrolling)

      t_s32 offset_space = 0;
      t_s32 element_margin = 0;
      ComputeStacking (width, offset_space, element_margin);
      current_x += offset_space;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible ())
          continue;

        current_x += element_margin;

        (*it)->SetBaseX (current_x);
        (*it)->SetBaseY (current_y);

        MinorDimensionSize extend = (*it)->GetExtend();
        MinorDimensionPosition positioning = (*it)->GetPositioning();

        if ( (extend != eFull) || ( (*it)->GetBaseHeight() < height) )
        {
          t_s32 widget_height = (*it)->GetBaseHeight();

          switch (positioning)
          {
            case eAbove:
            {
              // do nothing
              (*it)->SetBaseY (current_y);
              break;
            }
            case eBelow:
            {
              if (widget_height < height)
                (*it)->SetBaseY (current_y + height - widget_height);
              else
                (*it)->SetBaseY (current_y);

              break;
            }

            case eCenter:
            default:
            {
              if (widget_height < height)
                (*it)->SetBaseY (current_y + (height - widget_height) / 2);
              else
                (*it)->SetBaseY (current_y);
            }
          }
        }

        current_x += (*it)->GetBaseWidth() + element_margin + m_h_in_margin;
      }
    }

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if (!(*it)->IsVisible ())
        continue;

      if ( (*it)->Type().IsDerivedFromType (Layout::StaticObjectType) )
      {
        (*it)->ComputePosition2 (offsetX, offsetY);
      }
      else if ( (*it)->Type().IsDerivedFromType (View::StaticObjectType) )
      {
        (*it)->PositionChildLayout (offsetX, offsetY);
      }
    }
  }


}
