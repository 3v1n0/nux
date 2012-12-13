/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
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

  static const int HERROR = 0;
  NUX_IMPLEMENT_OBJECT_TYPE(HLayout);

  HLayout::HLayout(NUX_FILE_LINE_DECL)
    : LinearLayout(NUX_FILE_LINE_PARAM)
  {
#if DEBUG_LAYOUT
    space_between_children_ = 10;
    left_padding_ = 10;
    right_padding_ = 10;
    m_v_in_margin = 10;
    top_padding_ = 10;
    bottom_padding_ = 10;
#endif

    // Start packing the elements from the top. Is the layout has more space than the elements can use,
    // leave that space at the bottom of the HLayout.
    m_ContentStacking = eStackLeft;
  }

  HLayout::HLayout(std::string name, NUX_FILE_LINE_DECL)
    : LinearLayout(NUX_FILE_LINE_PARAM)
  {
    m_name = name;
#if DEBUG_LAYOUT
    space_between_children_ = 10;
    left_padding_ = 10;
    right_padding_ = 10;
    m_v_in_margin = 10;
    top_padding_ = 10;
    bottom_padding_ = 10;
#endif

    m_ContentStacking = eStackLeft;
  }

  HLayout::~HLayout()
  {
  }

  void HLayout::GetCompositeList(std::list<Area *> *ViewList)
  {
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsView())
      {
        View *ic = static_cast<View *>(*it);
        ViewList->push_back(ic);
      }
      else if ((*it)->IsLayout())
      {
        Layout *layout = static_cast<Layout *>(*it);
        layout->GetCompositeList(ViewList);
      }
    }
  }

  void HLayout::ComputeStacking(int remaining_width, int &offset_space, int &element_margin)
  {
    int per_element_space = 0;
    int total_used_space = 0;
    int num_elements = 0;

    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsVisible())
      {
        // gather all the space used by elements
        total_used_space += (*it)->GetBaseWidth();
        num_elements++;
      }
    }

    if (num_elements)
    {
      // Compute the space available for each element
      per_element_space = (remaining_width - total_used_space) / int(num_elements);
    }

    if (per_element_space < 0)
    {
      per_element_space = 0;
    }

    int margin;

    if (per_element_space > 0)
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
      case MAJOR_POSITION_START:
      {
        offset_space = 0;
        element_margin = 0;
      }
      break;

      case MAJOR_POSITION_END:
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

  long HLayout::ComputeContentSize()
  {
    if (_layout_element_list.size() == 0)
    {
      return eCompliantHeight | eCompliantWidth;
    }

    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsVisible())
        (*it)->SetLayoutDone(false);
    }

    int original_height = GetBaseHeight();

    if (GetScaleFactor() == 0)
    {
      // The size must exactly fit the children. The parent cannot be larger or smaller
      // than the total width of its children(+ margins).
      // So set the parent size to Geometry(0,0,1,1) and let the children force it to extend.
      if (GetParentObject() && GetParentObject()->Type().IsObjectType(HLayout::StaticObjectType))
      {
        // The parent if a HLayout(same type). Then a Stretch factor of 0 means this layout has its width set to 1.
        Area::SetBaseWidth(1);
      }
      else if (GetParentObject() && GetParentObject()->Type().IsObjectType(VLayout::StaticObjectType))
      {
        // The parent if a VLayout. Then a Stretch factor of 0 means this layout has its height set to 1.
        Area::SetBaseHeight(1);
      }
      else
      {
        // This is for when a layout is set as a composition layout of an View and its stretch factor is explicitly set to 0.
        Area::SetBaseWidth(1);
        Area::SetBaseHeight(1);
      }

      //The children will all assume their minimum size.
    }
    else
    {
      // The total size of the children(+ margins) may be smaller or equal to parent size.
    }

    bool unadjusted_layout = false;

    do
    {
      unsigned int num_element = 0;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if ((*it)->IsVisible())
          num_element++;
      }
     
      // Get layout Width and Height
      int width = GetBaseWidth();
      int height = GetBaseHeight();

      // Remove the margins. This is the real width and height available to the children.
      width -= (int) (num_element - 1) * space_between_children_ + (left_padding_ + right_padding_);
      height -= (top_padding_ + bottom_padding_);

      // Size the children according to their stretch factor.
      HLayoutManagement(width, height);

      // Objects have been resized, now position them.
      int current_x = GetBaseX() + left_padding_;
      int current_y = GetBaseY() + top_padding_;

      int offset_space = 0;
      int space_after_element = 0;
      ComputeStacking(width, offset_space, space_after_element);
      current_x += offset_space;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible())
          continue;

        current_x += space_after_element;

        (*it)->SetBaseX(current_x);
        (*it)->SetBaseY(current_y);

        MinorDimensionSize extend = (*it)->GetExtend();
        MinorDimensionPosition positioning = (*it)->GetPositioning();
        float percentage = (*it)->GetPercentage();

        // Compute the size of an ellement in the minor dimension(vertical)
        switch(extend)
        {
          case MINOR_SIZE_PERCENTAGE:
          {
            // The size of the processed element in the minor dimension is a percentage of layout minor dimension size.
            // Note that children of the processed element may force it to have a bigger size.
            int percentage_height = (height * percentage) / 100.0f;
            (*it)->SetBaseHeight(percentage_height);
            break;
          }

          case MINOR_SIZE_MATCHCONTENT:
          {
            // Force the element height to be the minimum has defined with SetMinimumHeight.
            // The children of this element can force it to get larger.
            (*it)->ApplyMinHeight();
            break;
          }

          case eFix:
          {
            //do nothing
            break;
          }

          case MINOR_SIZE_FULL:
          default:
          {
            (*it)->SetBaseHeight(height);
            break;
          }
        }

        // Compute the position of an element in the minor dimension.
        if ((*it)->GetBaseHeight() < height)
        {
          int widget_height = (*it)->GetBaseHeight();

          switch(positioning)
          {
            case MINOR_POSITION_START:
            {
              // do nothing
              (*it)->SetBaseY(current_y);
              break;
            }
            case MINOR_POSITION_END:
            {
              if (widget_height < height)
                (*it)->SetBaseY(current_y + height - widget_height);
              else
                (*it)->SetBaseY(current_y);

              break;
            }

            case MINOR_POSITION_CENTER:
            default:
            {
              if (widget_height < height)
                (*it)->SetBaseY(current_y + (height - widget_height) / 2);
              else
                (*it)->SetBaseY(current_y);
            }
          }
        }

        current_x += (*it)->GetBaseWidth() + space_after_element + space_between_children_;
      }

      // Manage child layout
      if (num_element == 0)
        m_fittingWidth = (int) (left_padding_ + right_padding_);
      else
        m_fittingWidth = (int) (num_element - 1) * space_between_children_ + (left_padding_ + right_padding_);

      m_contentHeight = GetBaseHeight() - (top_padding_ + bottom_padding_); // Set to the size of the layout.

      int element_height = 0;
      unadjusted_layout = false;

      // This array is meant to store the sizes of some of the elements height. These elements must have MINOR_SIZE_FULL as extent and
      // they have not been not been constrained smaller after ComputeContentSize is called. Because the layout may have a stretch factor of 0
      // and therefore its size has been set to 1x1 at the start of this function, there is a possibility that some of the elements don't have
      // the full height of the layout(these elements uses their minimum height because the layout was set to a size 1x1).
      // We check if that is the case and force a recompute.
      std::vector<int> FullSizeUnadjusted;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible())
          continue;

        int ret = 0;

        if (((*it)->IsLayout()  || (*it)->IsView()) /*&& ((*it)->IsLayoutDone() == false)*/ /*&& ((*it)->GetScaleFactor() != 0)*/)
        {
          Geometry pre_geo = (*it)->GetGeometry();
          ret = (*it)->ComputeContentSize();
          Geometry post_geo = (*it)->GetGeometry();

          bool larger_width    = pre_geo.width < post_geo.width;
          bool smaller_width   = pre_geo.width > post_geo.width;
          bool larger_height   = pre_geo.height < post_geo.height;
          bool smaller_height  = pre_geo.height > post_geo.height;

          if ((larger_width || smaller_width) && ((*it)->IsLayoutDone() == false))
          {
            // Stop computing the size of this layout. Its size was not convenient to its children. So the children size take priority
            // over the layout. In ComputeContentSize, the dimension of the layout has been set so it encompasses its children(and the margins).
            // Now the parent layout cannot be touched again: layout_done_ = true. In VLayoutManagement, it is as if the stretchfactor
            // of this layout is now 0.
            // This is the only place where a layout can have layout_done_ set to "true".

            // If(smaller_width == true) the layout takes less space than anticipated.
            // Set unadjusted_layout = true, so another pass will allow its sibling to claim more space.

            {
              unadjusted_layout = true;
              (*it)->SetLayoutDone(true);
            }
          }

          if ((smaller_height == false) && ((*it)->GetExtend() == MINOR_SIZE_FULL) && ((*it)->GetBaseHeight() < (*it)->GetMaximumHeight()))
          {
            // We catch all object whose size is possibly larger than the layout. We check there size at the end and
            // recompute the layout if necessary.
            // For layout elements, make sure that the stretch factor is not 0. If it is, it means it will not use the
            // size provided by the parent layout. Its size will be adjusted to the minimum size of the layout content.
            if (! ((*it)->IsLayout() && (*it)->GetScaleFactor() == 0))
              FullSizeUnadjusted.push_back((*it)->GetBaseHeight());
          }

          if ((smaller_height || larger_height) && ((*it)->GetExtend() == MINOR_SIZE_MATCHCONTENT))
          {
            (*it)->SetMinimumHeight((*it)->GetBaseHeight());
            unadjusted_layout = true;
          }

          // Should be reactivate so that if the parent Layout does not call
          // ComputeContentPosition, at least it is done here to arrange the internal
          // element of the children.
          //(*it)->ComputeContentPosition(0,0);
        }

        m_fittingWidth += (*it)->GetBaseWidth();

        element_height = (*it)->GetBaseHeight();

        if ((*it)->IsSpaceLayout() == false)
        {
          if ((GetScaleFactor() != 0) && (ret & eSmallerHeight))
          {
            if (m_contentHeight < element_height)
            {
              if (m_contentHeight < GetMaximumHeight())
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

//                    else if ((*it)->GetExtend() == MINOR_SIZE_FULL)
//                    {
//                        unadjusted_layout = true;
//                    }
          }
        }
      }

      // Set the height of the layout to be equal to the largest height it controls.
      // m_contentHeight is the largest height of an element within this layout. The layout size is then
      // m_contentHeight + (top_padding_ + bottom_padding_);
      SetBaseHeight(m_contentHeight + (top_padding_ + bottom_padding_));

      // Get back the Height after it has been bounded by [minHeight, maxHeight] in the preceeding call to SetBaseHeight.
      // Then deduce the height of the content.
      int temp = GetHeight() - (top_padding_ + bottom_padding_);
      std::vector<int>::iterator IntIterator = FullSizeUnadjusted.begin();

      for (IntIterator = FullSizeUnadjusted.begin(); IntIterator != FullSizeUnadjusted.end(); IntIterator++)
      {
        if ((*IntIterator) < temp)
        {
          unadjusted_layout = true;
        }
      }
    }
    while (unadjusted_layout);

    // m_fittingHeight is sum of the largest element height plus the outer margin.
    m_fittingHeight = m_contentHeight + (top_padding_ + bottom_padding_);

    // m_fittingWidth is sum of the element width plus the inner and outer margin.
    // If the stretch factor is 0 then, the layout height has been set to 1 and need to grow with the elements within.
    // If the stretch factor is not 0, then the layout height is set to m_fittingHeight only if the its height is less than m_fittingHeight;
    // This way, if the layout height is greater than m_fittingHeight there is space to compute the content stacking(ComputeStacking).
    if (GetBaseWidth() < m_fittingWidth)
    {
      SetBaseWidth(m_fittingWidth);
    }

    m_contentWidth = m_fittingWidth;
    long size_compliance = 0L;

    ComputeContentPosition(0, 0);

    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeContentSize: HLayout Width compliant = " << m_fittingWidth << std::endl;
#endif
      size_compliance |= eCompliantWidth;
    }

    // The layout has been resized to tightly pack its content
    if (GetBaseHeight() > original_height + HERROR)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout has been resized larger in height to tightly pack its content.
      // Or you can say that the layout refuse to be smaller than total HEIGHT of its elements.
      std::cout << "ComputeContentSize: HLayout Height block at " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eLargerHeight; // need scrollbar
    }
    else if (GetBaseHeight() + HERROR < original_height)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout is smaller.
      std::cout << "ComputeContentSize: HLayout Height is smaller = " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eSmallerHeight;
    }
    else
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeContentSize: HLayout Height compliant = " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eCompliantHeight;
    }

    return size_compliance;
  }

  void HLayout::HLayoutManagement(int width, int /* height */)
  {
    bool need_recompute = false;

    do
    {
      need_recompute = false;
      int available_width = width;
      unsigned int max_stretchfactor = GetMaxStretchFactor();
      std::list<Area *>::iterator it;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible())
          continue;

        if (((*it)->GetScaleFactor() == 0) && ((*it)->IsLayoutDone() != true))
        {
          (*it)->ApplyMinWidth();
        }
      }

      if (max_stretchfactor == 0)
      {
        // It means all object are fixed sized or all re-sizable object have been sized to there max or there min.

        if (GetScaleFactor() == 0)
        {
          // It is unlikely that we get here!
          int w = 0;
          for (it = _layout_element_list.begin(); it != _layout_element_list.end() && !need_recompute; it++)
          {
            if (!(*it)->IsVisible())
              continue;

            w += (*it)->GetBaseWidth();
          }
          SetBaseWidth(w);
        }
        return;
      }

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible())
          continue;

        if ((*it)->GetScaleFactor() == 0 || (*it)->IsLayoutDone() == true)
        {
          available_width -= (*it)->GetBaseWidth();
        }
      }

      if (available_width <= 2)
      {
        for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
        {
          if (!(*it)->IsVisible())
            continue;

          if (((*it)->GetScaleFactor() != 0) && (*it)->IsArea())
          {
            // If it is not an object of type eInputArea, do not set layout_done_ to true,
            // so, the layout management function will later be called on the object.
            (*it)->ApplyMinWidth();
            (*it)->SetLayoutDone(true);
          }
          else if (((*it)->GetScaleFactor() != 0) && ((*it)->IsLayout()) && ((*it)->IsLayoutDone() == false)) // layout and not fixed by child
          {
            // The out of bound must be reset to false.
            (*it)->ApplyMinWidth();
            (*it)->SetLayoutDone(false);
          }
          else if (((*it)->GetScaleFactor() != 0) && ((*it)->IsLayoutDone() == false)) // layout and not fixed
          {
            (*it)->ApplyMinWidth();
            // A layout must never have layout_done_ set to true "here" because it must continue
            // doing the layout of its children and finally resize itself to fit them.
            // The same way, A layout size factor should never be set to 0.
          }
        }

        return;
      }

      float cumul = 0;
      Area *LastElementThatCanBeResized = 0;
      int total_distributed_size = 0;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible())
          continue;

        if (((*it)->GetScaleFactor() != 0) && ((*it)->IsLayoutDone() == false))
        {
          float sf = (float) (*it)->GetScaleFactor();
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

      unsigned int ref_width = available_width / cumul;

      need_recompute = false;;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end() && !need_recompute; it++)
      {
        if (!(*it)->IsVisible())
          continue;
        
        if (((*it)->GetScaleFactor() != 0) && ((*it)->IsLayoutDone() == false))
        {
          unsigned int sf = (*it)->GetScaleFactor();
          int new_width;

          if (sf == max_stretchfactor)
          {
            new_width = ref_width;
          }
          else
          {
            new_width = ref_width * sf / max_stretchfactor;
          }

          total_distributed_size += new_width;

          if (LastElementThatCanBeResized == (*it))
          {
            // Redistribute the remaining size to the last element(at the right).
            // This is necessary because of imprecision. For instance if available_height = 451 and we have 2 elements
            // with the same stretch factor than each one will have a size of 225. With is correction, the first element will have a size of
            // 225 while the second one will have a size of 226.
            if (available_width - total_distributed_size > 0)
            {
              new_width += available_width - total_distributed_size;
              total_distributed_size = available_width;
            }
          }

          int elemt_max_width = (*it)->GetMaximumSize().width;
          int elemt_min_width = (*it)->GetMinimumSize().width;

          // A layout must never have layout_done_ set to true "here" because it must continue
          // doing the layout of its children and finally resize itself to fit them.
          // The same way, A layout size factor should never be set to 0.
          // Q: How about SpaceLayout? Should we treat them the same as layout or widget in this particular case?
          // A: SapceLayout are treated like widgets in this case

          if (new_width < elemt_min_width)
          {
            // assume the minimum width
            (*it)->SetBaseWidth(elemt_min_width);

            if ((*it)->IsLayout() == false || (*it)->IsSpaceLayout())
            {
              (*it)->SetLayoutDone(true);
              need_recompute = true;
            }
          }
          else if (new_width > elemt_max_width)
          {
            // assume the maximum width
            (*it)->SetBaseWidth(elemt_max_width);

            if ((*it)->IsLayout() == false || (*it)->IsSpaceLayout())
            {
              (*it)->SetLayoutDone(true);
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
            (*it)->SetBaseWidth(new_width);
          }
        }
        else
        {
          // For fixed element, reset their size to the same so it is checked against
          // the min and max. This is necessary in case you have set the size of the element first then latter,
          // you define its MinimumSize and/or MaximumSize size.
          unsigned int w = (*it)->GetBaseWidth();
          unsigned int h = (*it)->GetBaseHeight();
          (*it)->SetBaseWidth(w);
          (*it)->SetBaseHeight(h);
        }
      }
    }
    while (need_recompute);
  }

  unsigned int HLayout::GetMaxStretchFactor()
  {
    unsigned int value = 0;
    unsigned int sf;
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if (!(*it)->IsVisible())
        continue;

      // In the recursive process, make sure we get always the highest stretch factor
      // of an element that has not been bounded.
      if ((*it)->IsLayoutDone() == false)
      {
        sf = (*it)->GetScaleFactor();

        if (sf >= value)
        {
          value = sf;
        }
      }
    }

    return value;
  }

  void HLayout::ComputeContentPosition(float offsetX, float offsetY)
  {
    std::list<Area *>::iterator it;
    {
      unsigned int num_element = 0;
      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if ((*it)->IsVisible())
          num_element++;
      }
     
      // Get layout Width and Height
      int width = GetBaseWidth();
      int height = GetBaseHeight();
      // remove the margins
      width -= (int) (num_element - 1) * space_between_children_ + (left_padding_ + right_padding_);
      height -= (top_padding_ + bottom_padding_);

      // Objects have been resized, now position them.
      int current_x = GetBaseX() + left_padding_ + offsetX; // add base offset in X(used for scrolling)
      int current_y = GetBaseY() + top_padding_ + offsetY; // add base offset in Y(used for scrolling)

      int offset_space = 0;
      int element_margin = 0;
      ComputeStacking(width, offset_space, element_margin);
      current_x += offset_space;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible())
          continue;

        current_x += element_margin;

        (*it)->SetBaseX(current_x);
        (*it)->SetBaseY(current_y);

        MinorDimensionPosition positioning = (*it)->GetPositioning();

        if ((*it)->GetBaseHeight() < height)
        {
          int widget_height = (*it)->GetBaseHeight();

          switch(positioning)
          {
            case MINOR_POSITION_START:
            {
              // do nothing
              (*it)->SetBaseY(current_y);
              break;
            }
            case MINOR_POSITION_END:
            {
              if (widget_height < height)
                (*it)->SetBaseY(current_y + height - widget_height);
              else
                (*it)->SetBaseY(current_y);

              break;
            }

            case MINOR_POSITION_CENTER:
            default:
            {
              if (widget_height < height)
                (*it)->SetBaseY(current_y + (height - widget_height) / 2);
              else
                (*it)->SetBaseY(current_y);
            }
          }
        }

        current_x += (*it)->GetBaseWidth() + element_margin + space_between_children_;
      }
    }

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if (!(*it)->IsVisible())
        continue;

      if ((*it)->Type().IsDerivedFromType(Layout::StaticObjectType))
      {
        (*it)->ComputeContentPosition(offsetX, offsetY);
      }
      else if ((*it)->Type().IsDerivedFromType(View::StaticObjectType))
      {
        (*it)->ComputeContentPosition(offsetX, offsetY);
      }
    }
  }

  Area* HLayout::KeyNavIteration(KeyNavDirection direction)
  {
    if (_layout_element_list.size() == 0)
      return NULL;

    if (IsVisible() == false)
      return NULL;

    if (next_object_to_key_focus_area_)
    {
      if ((direction == KEY_NAV_UP) || (direction == KEY_NAV_DOWN))
      {
        // Don't know what to do with this
        return NULL;
      }
      std::list<Area*>::iterator it;
      std::list<Area*>::iterator it_next;
      it = std::find(_layout_element_list.begin(), _layout_element_list.end(), next_object_to_key_focus_area_);
      
      if (it == _layout_element_list.end())
      {
        // Should never happen
        nuxAssert(0);
        return NULL;
      }

      it_next = it;
      ++it_next;

      if ((direction == KEY_NAV_LEFT) && (it == _layout_element_list.begin()))
      {
        // can't go further
        return NULL;
      }

      if ((direction == KEY_NAV_RIGHT) && (it_next == _layout_element_list.end()))
      {
        // can't go further
        return NULL;
      }

      if (direction == KEY_NAV_LEFT)
      {
        --it;
        Area* key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          if (it == _layout_element_list.begin())
            break;

          --it;
          key_nav_focus = (*it)->KeyNavIteration(direction);
        }

        return key_nav_focus;
      }

      if (direction == KEY_NAV_RIGHT)
      {
        ++it;
        Area* key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          ++it;
          if (it == _layout_element_list.end())
            break;

          key_nav_focus = (*it)->KeyNavIteration(direction);
        }

        return key_nav_focus;
      }
    }
    else
    {
      Area* key_nav_focus = NULL;

      if (direction == KEY_NAV_LEFT)
      {
        std::list<Area*>::reverse_iterator it = _layout_element_list.rbegin();
        key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          ++it;
          if (it == _layout_element_list.rend())
            break;

          key_nav_focus = (*it)->KeyNavIteration(direction);
        }
      }
      else
      {
        std::list<Area*>::iterator it = _layout_element_list.begin();
        key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          ++it;
          if (it == _layout_element_list.end())
            break;

          key_nav_focus = (*it)->KeyNavIteration(direction);
        }
      }

      return key_nav_focus;
    }

    return NULL;
  }
}
