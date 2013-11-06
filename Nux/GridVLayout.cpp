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
#include "GridVLayout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux
{

  static const int VERROR = 0;
  NUX_IMPLEMENT_OBJECT_TYPE(GridVLayout);

  GridVLayout::GridVLayout(NUX_FILE_LINE_DECL)
    :   Layout(NUX_FILE_LINE_PARAM)
  {
#if DEBUG_LAYOUT
    m_h_in_margin = 10;
    m_h_out_margin = 10;
    m_v_in_margin = 10;
    m_v_out_margin = 10;
#endif

    _children_size = Size(64, 64);
    _force_children_size = true;
    _partial_visibility = true;
    _num_row = 1;
    _num_column = 1;
    _dynamic_column = true;
    _width_match_content = true;

    // Start packing the elements from the top. Is the layout has more space than the elements can use,
    // leave that space at the bottom of the GridVLayout.
    m_ContentStacking = eStackLeft;

    SetMinimumSize(32, 32);
  }

  GridVLayout::~GridVLayout()
  {

  }

  int GridVLayout::GetChildPos(Area *child)
  {
    int position = 0;
    std::list<Area *>::const_iterator it;
    for (it = GetChildren().begin(); it != GetChildren().end(); it++)
    {
      if ((*it) == child)
        break;
    }

    return position;
  }

  Area* GridVLayout::GetChildAtPosition(int pos)
  {
    int position = 0;
    std::list<Area *>::const_iterator it;
    for (it = GetChildren().begin(); it != GetChildren().end(); it++)
    {
      if (position == pos)
        return (*it);
    }   

    return NULL;
  }

  void GridVLayout::EnablePartialVisibility(bool partial_visibility)
  {
    _partial_visibility = partial_visibility;
  }

  void GridVLayout::SetChildrenSize(int width, int height)
  {
    _children_size = Size(width, height);
  }

  Size GridVLayout::GetChildrenSize() const
  {
    return _children_size;
  }

  void GridVLayout::ForceChildrenSize(bool force)
  {
    _force_children_size = force;
  }

  int GridVLayout::GetNumColumn() const
  {
    return _num_column;
  }

  int GridVLayout::GetNumRow() const
  {
    return _num_row;
  }

  void GridVLayout::SetWidthMatchContent(bool match_content)
  {
    _width_match_content = match_content;
  }

  bool GridVLayout::GetWidthMatchContent() const
  {
    return _width_match_content;
  }

  void GridVLayout::GetCompositeList(std::list<Area *> *ViewList)
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

  long GridVLayout::ComputeContentSize()
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
      int X = base.x + m_h_out_margin;
      int Y = base.y + m_v_out_margin;

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

        if ((!_partial_visibility) && (Y + _children_size.height > base.y + base.height - m_v_out_margin))
        {
          X += _children_size.width + m_h_in_margin;
          Y = base.y + m_v_out_margin;

          first_element_of_column = true;
          if (i < num_elements - 1)
            ++num_column;
        }
        else if (Y >= base.y + base.height)
        {
          X += _children_size.width + m_h_in_margin;
          Y = base.y + m_v_out_margin;

          first_element_of_column = true;
          if (i < num_elements - 1)
            ++num_column;
        }
      }
    }

    _num_row = num_row;
    _num_column = num_column;

    if ((GetScaleFactor() == 0) || _width_match_content)
    {
      int w = num_column * _children_size.width + 2 * m_h_out_margin + (num_column - 1) * m_h_in_margin;
      SetMinimumWidth(w);
      SetBaseWidth(w);
    }

    long size_compliance = 0L;

    if (GetBaseWidth() > original_width + VERROR)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout has been resized larger in WIDTH to tightly pack its content.
      // Or you can say that the layout refuse to be smaller than total WIDTH of its elements.
      std::cout << "ComputeContentSize: VLayout Width block at " << GetWidth() << std::endl;
#endif
      size_compliance |= eLargerWidth; // need scrollbar
    }
    else if (GetBaseWidth() + VERROR < original_width)
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

  void GridVLayout::ProcessDraw(GraphicsEngine &graphics_engine, bool force_draw)
  {
    std::list<Area *> elements;
    std::list<Area *>::iterator it = _layout_element_list.begin();

    graphics_engine.PushModelViewMatrix(Get2DMatrix());

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); ++it)
    {
      if ((*it)->IsVisible())
        elements.push_back(*it);
    }

    it = elements.begin();

    Geometry base = GetGeometry();
    Geometry const& parent_geometry = GetToplevel() ? GetToplevel()->GetAbsoluteGeometry() : GetAbsoluteGeometry();

    bool geometry_is_visible = parent_geometry.IsIntersecting(GetAbsoluteGeometry());

    graphics_engine.PushClippingRectangle(base);

    for (int i = 0; i < _num_column; i++)
    {
      for (int j = 0; j < _num_row; j++)
      {
        if (it == elements.end())
          break;

        // Test if the element is inside the Grid before rendering.
        if (geometry_is_visible)
        {
          int X = base.x + m_h_out_margin + i * (_children_size.width + m_h_in_margin);
          int Y = base.y + m_v_out_margin + j * (_children_size.height + m_v_in_margin);

          graphics_engine.PushClippingRectangle(Geometry(X, Y, _children_size.width, _children_size.height));
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

        it++;
      }
    }

    graphics_engine.PopClippingRectangle();
    graphics_engine.PopModelViewMatrix();

    draw_cmd_queued_ = false;
  }

  Area* GridVLayout::KeyNavIteration(KeyNavDirection direction)
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
}
