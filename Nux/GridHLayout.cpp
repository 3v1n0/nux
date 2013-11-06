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
#include "GridHLayout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(GridHLayout);

  GridHLayout::GridHLayout(NUX_FILE_LINE_DECL)
    :   Layout(NUX_FILE_LINE_PARAM)
  {
#if DEBUG_LAYOUT
    m_h_in_margin = 10;
    left_padding_ = 10;
    right_padding_ = 10;
    m_v_in_margin = 10;
    top_padding_ = 10;
    bottom_padding_ = 10;
#endif

    m_h_in_margin = 10;
    m_v_in_margin = 10;

    filling_order_ = FILL_HORIZONTAL;
    _children_size = Size(64, 64);
    _force_children_size = true;
    _partial_visibility = true;
    _num_row = 1;
    _num_column = 1;
    _dynamic_column = true;
    match_content_size_ = true;

    // Start packing the elements from the top. Is the layout has more space than the elements can use,
    // leave that space at the bottom of the GridHLayout.
    m_ContentStacking = MAJOR_POSITION_LEFT;

    SetMinimumSize(32, 32);
  }

  GridHLayout::~GridHLayout()
  {

  }

  void GridHLayout::SetSpaceBetweenChildren(int horizontal_space, int vertical_space)
  {
    m_h_in_margin = horizontal_space;
    m_v_in_margin = vertical_space;
  }

  int GridHLayout::GetChildPos(Area *child)
  {
    int position = 0;
    std::list<Area *>::const_iterator it;
    for (it = GetChildren().begin(); it != GetChildren().end(); it++)
    {
      if ((*it) == child)
        break;
      ++position;
    }

    return position;
  }

  Area* GridHLayout::GetChildAtPosition(int pos)
  {
    int position = 0;
    std::list<Area *>::const_iterator it;
    for (it = GetChildren().begin(); it != GetChildren().end(); it++)
    {
      if (position == pos)
        return (*it);

      ++position;
    }

    return NULL;
  }

  void GridHLayout::EnablePartialVisibility(bool partial_visibility)
  {
    _partial_visibility = partial_visibility;
  }

  void GridHLayout::SetChildrenSize(int width, int height)
  {
    _children_size = Size(width, height);
  }

  Size GridHLayout::GetChildrenSize() const
  {
    return _children_size;
  }

  void GridHLayout::ForceChildrenSize(bool force)
  {
    _force_children_size = force;
  }

  int GridHLayout::GetNumColumn() const
  {
    return _num_column;
  }

  int GridHLayout::GetNumRow() const
  {
    return _num_row;
  }

  void GridHLayout::MatchContentSize(bool match_content)
  {
    match_content_size_ = match_content;
  }

  bool GridHLayout::IsMatchingContentSize() const
  {
    return match_content_size_;
  }

  void GridHLayout::GetCompositeList(std::list<Area *> *ViewList)
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

  long GridHLayout::ComputeLayoutRowOrder()
  {
    std::list<Area *> elements;

    if (GetScaleFactor() == 0)
    {
      ApplyMinWidth();
    }

    if (_layout_element_list.size() == 0)
    {
      return eCompliantHeight | eCompliantWidth;
    }

    int num_elements = 0;

    std::list<Area *>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsVisible())
      {
        (*it)->SetLayoutDone(false);
        elements.push_back(*it);
        num_elements++;
      }
      (*it)->SetLayoutDone(false);
    }

    int original_height = GetBaseHeight();

    // The grid layout goes through the child elements and assign them a size and position. Children are filled in the grid like this:
    //  0   1   2   3   4   5
    //  6   7   8   9   10  11
    //  12  13  ..  ..  ..  ..
    // This is a left to right fill going down. An option can be added the fill the grid from top to bottom and going toward the right.

    nux::Geometry base = GetGeometry();
    it = elements.begin();
    int num_row = 0;
    int num_column = 0;

    if (num_elements > 0)
      ++num_row;

    if (_dynamic_column)
    {
      int X = base.x + left_padding_;
      int Y = base.y + top_padding_;

      bool first_element_of_row = true;

      for (int i = 0; i < num_elements; i++)
      {
        if (num_row == 1)
          num_column++;

        if (first_element_of_row)
        {
          first_element_of_row = false;
        }

        if (_force_children_size)
        {
          (*it)->SetMinimumSize(_children_size.width, _children_size.height);
        }

        (*it)->SetGeometry(nux::Geometry(X, Y, _children_size.width, _children_size.height));

        (*it)->ComputeContentSize();

        X += _children_size.width + m_h_in_margin;

        it++;

        if ((!_partial_visibility) && (X + _children_size.width > base.x + base.width))
        {
          X = base.x + left_padding_;
          Y += _children_size.height + m_v_in_margin;

          first_element_of_row = true;
          if (i < num_elements - 1)
            ++num_row;
        }
        else if (X >= base.x + base.width)
        {
          X = base.x + left_padding_;
          Y += _children_size.height + m_v_in_margin;

          first_element_of_row = true;
          if (i < num_elements - 1)
            ++num_row;
        }
      }
    }

    _num_row = num_row;
    _num_column = num_column;

    if ((GetScaleFactor() == 0) || match_content_size_)
    {
      int h = num_row * _children_size.height + (top_padding_ + bottom_padding_) + (num_row - 1) * m_v_in_margin;
      SetMinimumHeight(h);
      SetBaseHeight(h);
    }

    long size_compliance = 0L;
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeContentSize: GridHLayout Width compliant = " << m_fittingWidth << std::endl;
#endif
      size_compliance |= eCompliantWidth;
    }

    // The layout has been resized to tightly pack its content
    if (GetBaseHeight() > original_height)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout has been resized larger in height to tightly pack its content.
      // Or you can say that the layout refuse to be smaller than total HEIGHT of its elements.
      std::cout << "ComputeContentSize: GridHLayout Height block at " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eLargerHeight; // need scrollbar
    }
    else if (GetBaseHeight() < original_height)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout is smaller.
      std::cout << "ComputeContentSize: GridHLayout Height is smaller = " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eSmallerHeight;
    }
    else
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeContentSize: GridHLayout Height compliant = " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eCompliantHeight;
    }

    //    if (GetScaleFactor() == 0)
    //    {
    //        return size_compliance | eForceComply;
    //    }

    //SetDirty(false);
    return size_compliance;
  }

  long GridHLayout::ComputeLayoutColumnOrder()
  {
    std::list<Area *> elements;

    if (GetScaleFactor() == 0)
    {
      ApplyMinHeight();
    }

    if (_layout_element_list.size() == 0)
    {
      return eCompliantHeight | eCompliantWidth;
    }

    int num_elements = 0;

    std::list<Area *>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsVisible())
      {
        (*it)->SetLayoutDone(false);
        elements.push_back(*it);
        num_elements++;
      }
      (*it)->SetLayoutDone(false);
    }

    int original_width = GetBaseWidth();

    nux::Geometry base = GetGeometry();
    it = elements.begin();
    int num_row = 0;
    int num_column = 0;

    if (num_elements > 0)
      ++num_column;

    if (_dynamic_column)
    {
      int X = base.x + left_padding_;
      int Y = base.y + top_padding_;

      bool first_element_of_column = true;

      for (int i = 0; i < num_elements; i++)
      {
        if (num_column == 1)
          num_row++;

        if (first_element_of_column)
        {
          first_element_of_column = false;
        }

        if (_force_children_size)
        {
          (*it)->SetMinimumSize(_children_size.width, _children_size.height);
        }

        (*it)->SetGeometry(nux::Geometry(X, Y, _children_size.width, _children_size.height));

        (*it)->ComputeContentSize();

        Y += _children_size.height + m_v_in_margin;

        it++;

        if ((!_partial_visibility) && (Y + _children_size.height > base.y + base.height - top_padding_))
        {
          X += _children_size.width + m_h_in_margin;
          Y = base.y + top_padding_;

          first_element_of_column = true;
          if (i < num_elements - 1)
            ++num_column;
        }
        else if (Y >= base.y + base.height)
        {
          X += _children_size.width + m_h_in_margin;
          Y = base.y + top_padding_;

          first_element_of_column = true;
          if (i < num_elements - 1)
            ++num_column;
        }
      }
    }

    _num_row = num_row;
    _num_column = num_column;

    if ((GetScaleFactor() == 0) || match_content_size_)
    {
      int w = num_column * _children_size.width + (left_padding_ + right_padding_) + (num_column - 1) * m_h_in_margin;
      SetMinimumWidth(w);
      SetBaseWidth(w);
    }

    long size_compliance = 0L;

    if (GetBaseWidth() > original_width)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout has been resized larger in WIDTH to tightly pack its content.
      // Or you can say that the layout refuse to be smaller than total WIDTH of its elements.
      std::cout << "ComputeContentSize: VLayout Width block at " << GetWidth() << std::endl;
#endif
      size_compliance |= eLargerWidth; // need scrollbar
    }
    else if (GetBaseWidth() < original_width)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout is smaller.
      std::cout << "ComputeContentSize: VLayout Width smaller = " << GetWidth() << std::endl;
#endif
      size_compliance |= eSmallerWidth;
    }
    else
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeContentSize: VLayout Width compliant = " << GetWidth() << std::endl;
#endif
      size_compliance |= eCompliantWidth;
    }

    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeContentSize: VLayout Height compliant = " << m_fittingHeight << std::endl;
#endif
      size_compliance |= eCompliantHeight;
    }

    return size_compliance;
  }

  long GridHLayout::ComputeContentSize()
  {
    if (filling_order_ == FILL_HORIZONTAL)
    {
      return ComputeLayoutRowOrder();
    }
    else
    {
      return ComputeLayoutColumnOrder();
    }
  }

  void GridHLayout::ProcessDraw(GraphicsEngine &graphics_engine, bool force_draw)
  {
    if (_layout_element_list.size() == 0)
      return;

    std::list<Area *> elements;
    std::list<Area *>::iterator it = _layout_element_list.begin();

    graphics_engine.PushModelViewMatrix(Get2DMatrix());

    Geometry base = GetGeometry();
    Geometry absolute_geometry = GetAbsoluteGeometry();
    Geometry parent_geometry = absolute_geometry;
    Geometry visibility_geometry = absolute_geometry;

    if (GetToplevel())
    {
      parent_geometry = GetToplevel()->GetAbsoluteGeometry();
    }

    visibility_geometry = parent_geometry.Intersect(absolute_geometry);

    graphics_engine.PushClippingRectangle(base);

    it = _layout_element_list.begin();

    bool first = false;
    bool last = false;

    int JJ = 0;
    int II = 0;

    if (filling_order_ == FILL_HORIZONTAL)
    {
      JJ = _num_row;
      II = _num_column;
    }
    else
    {
      JJ = _num_column;
      II = _num_row;
    }

    for (int j = 0; j < JJ; j++)
    {
      for (int i = 0; i < II; i++)
      {
        if (it == _layout_element_list.end())
          break;

        if ((*it)->IsVisible() == false)
        {
          ++it;
          continue;
        }

        // Test if the element is inside the Grid before rendering.
        if (visibility_geometry.IsIntersecting((*it)->GetAbsoluteGeometry()))
        {
          if (first == false)
          {
            first = true; // First invisible child.
          }

          int x = 0;
          int y = 0;
          if (filling_order_ == FILL_HORIZONTAL)
          {
            x = base.x + left_padding_ + i * (_children_size.width + m_h_in_margin);
            y = base.y + top_padding_ + j * (_children_size.height + m_v_in_margin);
          }
          else
          {
            x = base.x + left_padding_ + j * (_children_size.width + m_h_in_margin);
            y = base.y + top_padding_ + i * (_children_size.height + m_v_in_margin);
          }

          graphics_engine.PushClippingRectangle(Geometry(x, y, _children_size.width, _children_size.height));

          if ((*it)->IsView())
          {
            View *ic = static_cast<View *>(*it);
            ic->ProcessDraw(graphics_engine, force_draw);
          }
          else if ((*it)->IsLayout())
          {
            Layout *layout = static_cast<Layout *>(*it);
            layout->ProcessDraw(graphics_engine, force_draw);
          }

          graphics_engine.PopClippingRectangle();
        }
        else
        {
          if (first)
          {
            // First invisible child. Exit!
            last = true;
          }
        }

        if (first && last)
        {
          // Early exit
          break;
        }

        it++;
      }
      if (first && last)
        break;
    }

    graphics_engine.PopClippingRectangle();
    graphics_engine.PopModelViewMatrix();

    draw_cmd_queued_ = false;
  }

  Area* GridHLayout::KeyNavIterationRowOrder(KeyNavDirection direction)
  {
    if (_layout_element_list.size() == 0)
      return NULL;

    if (IsVisible() == false)
      return NULL;

    if (next_object_to_key_focus_area_)
    {
      std::list<Area*>::iterator it;
      std::list<Area*>::iterator it_next;
      it = std::find(_layout_element_list.begin(), _layout_element_list.end(), next_object_to_key_focus_area_);
      it_next = it;
      ++it_next;

      if (it == _layout_element_list.end())
      {
        // Should never happen
        nuxAssert(0);
        return NULL;
      }

      int position = GetChildPos(*it); // note that(*it) == next_object_to_key_focus_area_
      int nun_column = GetNumColumn();
      int nun_row = GetNumRow();

      if ((direction == KEY_NAV_LEFT) && (it == _layout_element_list.begin()))
      {
        // first item
        return NULL;
      }

      if ((direction == KEY_NAV_LEFT) && position % nun_column == 0)
      {
        return NULL;
      }

      if ((direction == KEY_NAV_RIGHT) && (it_next == _layout_element_list.end()))
      {
        // last item
        return NULL;
      }

      if ((direction == KEY_NAV_RIGHT) && position % nun_column == nun_column - 1)
      {
        // last item
        return NULL;
      }

      if ((direction == KEY_NAV_UP) && ((position / nun_column) == 0))
      {
        // top edge
        return NULL;
      }

      if ((direction == KEY_NAV_DOWN) && ((position / nun_column) == (nun_row - 1)))
      {
        // bottom edge
        return NULL;
      }

      //////
      if (direction == KEY_NAV_LEFT)
      {
        --it;
        Area* key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          int pos = GetChildPos(*it);
          if (it == _layout_element_list.begin() || ((pos % nun_column) == 0))
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
          int pos = GetChildPos(*it);

          if ((it == _layout_element_list.end()) || (pos == (pos / nun_column) * nun_column + (nun_column -1)))
            break;

          ++it;
          key_nav_focus = (*it)->KeyNavIteration(direction);
        }

        return key_nav_focus;
      }

      if (direction == KEY_NAV_UP)
      {
        for (int i = 0; i < nun_column; ++i)
        {
          --it;
        }

        if (it != _layout_element_list.end())
          return (*it)->KeyNavIteration(direction);
      }

      if (direction == KEY_NAV_DOWN)
      {
        for (int i = 0; i < nun_column; ++i)
        {
          ++it;
        }

        if (it != _layout_element_list.end())
          return (*it)->KeyNavIteration(direction);
      }
    }
    else
    {
      Area* area = NULL;

      if (direction == KEY_NAV_UP)
        area = GetChildAtPosition(GetNumColumn() * (GetNumRow() - 1));
      else
        area = _layout_element_list.front();

      if (area)
      return area->KeyNavIteration(direction);
    }

    return NULL;
  }

  Area* GridHLayout::KeyNavIterationColumnOrder(KeyNavDirection direction)
  {
    if (_layout_element_list.size() == 0)
      return NULL;

    if (IsVisible() == false)
      return NULL;

    if (next_object_to_key_focus_area_)
    {
      std::list<Area*>::iterator it;
      std::list<Area*>::iterator it_next;
      it = std::find(_layout_element_list.begin(), _layout_element_list.end(), next_object_to_key_focus_area_);
      it_next = it;
      ++it_next;

      if (it == _layout_element_list.end())
      {
        // Should never happen
        nuxAssert(0);
        return NULL;
      }

      int position = GetChildPos(*it); // note that(*it) == next_object_to_key_focus_area_
      int nun_column = GetNumColumn();
      int nun_row = GetNumRow();

      if ((direction == KEY_NAV_UP) && (it == _layout_element_list.begin()))
      {
        // first item
        return NULL;
      }

      if ((direction == KEY_NAV_DOWN) && (it_next == _layout_element_list.end()))
      {
        // last item
        return NULL;
      }

      if ((direction == KEY_NAV_UP) && ((position % nun_row) == 0))
      {
        // Left edge
        return NULL;
      }

      if ((direction == KEY_NAV_DOWN) && (position == (position / nun_row) * nun_row + (nun_row -1)))
      {
        // right edge
        return NULL;
      }

      if ((direction == KEY_NAV_LEFT) && ((position / nun_row) == 0))
      {
        // top edge
        return NULL;
      }

      if ((direction == KEY_NAV_RIGHT) && ((position / nun_row) == nun_column))
      {
        // bottom edge
        return NULL;
      }

      //////
      if (direction == KEY_NAV_UP)
      {
        --it;
        Area* key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          int pos = GetChildPos(*it);
          if (it == _layout_element_list.begin() || ((pos % nun_row) == 0))
            break;

          --it;
          key_nav_focus = (*it)->KeyNavIteration(direction);
        }

        return key_nav_focus;
      }

      if (direction == KEY_NAV_DOWN)
      {
        ++it;
        Area* key_nav_focus = (*it)->KeyNavIteration(direction);

        while (key_nav_focus == NULL)
        {
          ++it;
          int pos = GetChildPos(*it);

          if ((it == _layout_element_list.end()) || (pos == (pos / nun_row) * nun_row + (nun_row -1)))
            break;

          key_nav_focus = (*it)->KeyNavIteration(direction);
        }

        return key_nav_focus;
      }

      if (direction == KEY_NAV_LEFT)
      {
        for (int i = 0; i < nun_row; ++i)
        {
          --it;
        }
        return (*it)->KeyNavIteration(direction);
      }

      if (direction == KEY_NAV_RIGHT)
      {
        for (int i = 0; i < nun_row; ++i)
        {
          ++it;
          if (it == _layout_element_list.end())
            return NULL;
        }
        return (*it)->KeyNavIteration(direction);
      }
    }
    else
    {
      std::list<Area*>::iterator it;
      it = _layout_element_list.begin();
      return (*it)->KeyNavIteration(direction);
    }

    return NULL;

  }

  Area* GridHLayout::KeyNavIteration(KeyNavDirection direction)
  {
    if (filling_order_ == FILL_HORIZONTAL)
    {
      return KeyNavIterationRowOrder(direction);
    }
    else
    {
      return KeyNavIterationColumnOrder(direction);
    }
  }

  void GridHLayout::SetFillingOrder(FillingOrder filling_order)
  {
    filling_order_ = filling_order;
  }

  GridHLayout::FillingOrder GridHLayout::GetFillingOrder() const
  {
    return filling_order_;
  }
}
