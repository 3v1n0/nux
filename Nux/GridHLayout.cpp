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
  static const t_s32 HERROR = 0;
  NUX_IMPLEMENT_OBJECT_TYPE (GridHLayout);

  GridHLayout::GridHLayout (NUX_FILE_LINE_DECL)
    :   Layout (NUX_FILE_LINE_PARAM)
  {
#if DEBUG_LAYOUT
    m_h_in_margin = 10;
    m_h_out_margin = 10;
    m_v_in_margin = 10;
    m_v_out_margin = 10;
#endif

    _children_size = Size (64, 64);
    _force_children_size = true;
    _partial_visibility = true;
    _num_row = 1;
    _num_column = 1;
    _dynamic_column = true;
    _height_match_content = true;

    // Start packing the elements from the top. Is the layout has more space than the elements can use,
    // leave that space at the bottom of the GridHLayout.
    m_ContentStacking = eStackLeft;

    SetMinimumSize(32, 32);
  }

  GridHLayout::~GridHLayout()
  {

  }

  int GridHLayout::GetChildPos (Area *child)
  {
    int position = 0;
    std::list<Area *>::const_iterator it;
    for (it = GetChildren ().begin(); it != GetChildren ().end(); it++)
    {
      if ((*it) == child)
        break;
      
      if ((*it)->CanFocus ())
      {
        position++;
      }
    }

    return position;
  }

  Area* GridHLayout::GetChildAtPosition (int pos)
  {
    int position = 0;
    std::list<Area *>::const_iterator it;
    for (it = GetChildren ().begin(); it != GetChildren ().end(); it++)
    {
      if (position == pos)
        return (*it);
      
      if ((*it)->CanFocus ())
      {
        position++;
      }
    }   

    return NULL;
  }

  long GridHLayout::DoFocusLeft (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    // if we are left on an edge, then send up
    Area* focused_child = GetFocusedChild ();
    int position = GetChildPos (focused_child);
    Area *parent = GetParentObject ();

    if (parent == NULL || position % GetNumColumn ())
    {
      return Layout::DoFocusLeft (ievent, TraverseInfo, ProcessEventInfo);
    }
    else
    {
      // left edge
      return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
    }
  }

  long GridHLayout::DoFocusRight (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    // if we are left on an edge, then send up
    Area* focused_child = GetFocusedChild ();
    int position = GetChildPos (focused_child);
    Area *parent = GetParentObject ();

    if (parent == NULL || (position + 1) % GetNumColumn ())
    {
      return Layout::DoFocusRight (ievent, TraverseInfo, ProcessEventInfo);
    }
    else
    {
      // Right Edge
      return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
    }
  }

  //up and down should pass event to parent
  long GridHLayout::DoFocusUp (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    Area* focused_child = GetFocusedChild ();
    int position = GetChildPos (focused_child);
    Area *parent = GetParentObject ();
    
    if (focused_child == NULL || position < GetNumColumn ())
    {
      
      if (parent != NULL)
        return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
      else
        FocusFirstChild ();

    }
    else
    {
      // so hacky, but its cheap!
      // just focus the child position - numcolumns
      //focused_child->SetFocused (false);
      focused_child = GetChildAtPosition (position - GetNumColumn ());
      if (focused_child)
      {
        focused_child->SetFocused (true);
        ChildFocusChanged.emit (/*this,*/ focused_child);
      }
      else
      {
        if (parent != NULL)
          return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
        else
          FocusFirstChild ();
      }
    }

    return TraverseInfo;
  }
  long GridHLayout::DoFocusDown (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    Area* focused_child = GetFocusedChild ();
    int position = GetChildPos (focused_child);
    Area *parent = GetParentObject ();
    
    if (focused_child == NULL || position > GetNumColumn () * (GetNumRow () - 1))
    {
      if (parent != NULL)
        return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
      else
        FocusLastChild ();
    }
    else
    {
      // so hacky, but its cheap!
      // just focus the child position - numcolumns
      //focused_child->SetFocused (false);
      focused_child = GetChildAtPosition (position + GetNumColumn ());
      if (focused_child)
      {
        focused_child->SetFocused (true);
        ChildFocusChanged.emit (/*this,*/ focused_child);
      }
      else
      {
        if (parent != NULL)
          return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
        else
          FocusLastChild ();
      }
    }

    return TraverseInfo;
  }

  void GridHLayout::EnablePartialVisibility (bool partial_visibility)
  {
    _partial_visibility = partial_visibility;
  }

  void GridHLayout::SetChildrenSize (int width, int height)
  {
    _children_size = Size (width, height);
  }

  Size GridHLayout::GetChildrenSize () const
  {
    return _children_size;
  }

  void GridHLayout::ForceChildrenSize (bool force)
  {
    _force_children_size = force;
  }

  int GridHLayout::GetNumColumn () const
  {
    return _num_column;
  }

  int GridHLayout::GetNumRow () const
  {
    return _num_row;
  }

  void GridHLayout::SetHeightMatchContent (bool match_content)
  {
    _height_match_content = match_content;
  }

  bool GridHLayout::GetHeightMatchContent () const
  {
    return _height_match_content;
  }

  void GridHLayout::GetCompositeList (std::list<Area *> *ViewList)
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

  long GridHLayout::ComputeLayout2()
  {
    std::list<Area *> elements;

    if (GetStretchFactor() == 0)
    {
      ApplyMinWidth();
    }

    if (_layout_element_list.size() == 0)
    {
      return eCompliantHeight | eCompliantWidth;
    }

    t_s32 num_elements = 0;

    std::list<Area *>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsVisible ())
      {
        (*it)->SetLayoutDone (false);
        elements.push_back (*it);
        num_elements++;
      }
      (*it)->SetLayoutDone (false);
    }

    t_s32 original_height = GetBaseHeight();

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
      int X = base.x + m_h_out_margin;
      int Y = base.y + m_v_out_margin;

      bool first_element_of_row = true;
      bool first_row = true;

      for(int i = 0; i < num_elements; i++) 
      {
        if (num_row == 1)
          num_column++;

        if(first_element_of_row)
        {
          first_row = false;
          
          first_element_of_row = false;
        }

        if (_force_children_size)
        {
          (*it)->SetMinimumSize (_children_size.width, _children_size.height);
        }

        (*it)->SetGeometry (nux::Geometry (X, Y, _children_size.width, _children_size.height));

        (*it)->ComputeLayout2();

        X += _children_size.width + m_h_in_margin;

        it++;

        if (!_partial_visibility && (X + _children_size.width > base.x + base.width))
        {
          X = base.x + m_h_out_margin;
          Y += _children_size.height + m_v_in_margin;

          first_element_of_row = true;
          if(i < num_elements - 1)
            ++num_row;
        }
        else if (X > base.x + base.width)
        {
          X = base.x + m_h_out_margin;
          Y += _children_size.height + m_v_in_margin;

          first_element_of_row = true;
          if(i < num_elements - 1)
            ++num_row;
        }
      }
    }

    _num_row = num_row;
    _num_column = num_column;

    if ((GetStretchFactor() == 0) || _height_match_content)
    {
      int h = num_row * _children_size.height + 2 * m_v_out_margin + (num_row - 1) * m_v_in_margin;
      SetMinimumHeight(h);
      SetBaseHeight(h);
    }

    long size_compliance = 0L;
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeLayout2: GridHLayout Width compliant = " << m_fittingWidth << std::endl;
#endif
      size_compliance |= eCompliantWidth;
    }

    // The layout has been resized to tightly pack its content
    if (GetBaseHeight() > original_height + HERROR)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout has been resized larger in height to tightly pack its content.
      // Or you can say that the layout refuse to be smaller than total HEIGHT of its elements.
      std::cout << "ComputeLayout2: GridHLayout Height block at " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eLargerHeight; // need scrollbar
    }
    else if (GetBaseHeight() + HERROR < original_height)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout is smaller.
      std::cout << "ComputeLayout2: GridHLayout Height is smaller = " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eSmallerHeight;
    }
    else
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout and its content resized together without trouble.
      std::cout << "ComputeLayout2: GridHLayout Height compliant = " << GetBaseHeight() << std::endl;
#endif
      size_compliance |= eCompliantHeight;
    }

//    if(GetStretchFactor() == 0)
//    {
//        return size_compliance | eForceComply;
//    }

    //SetDirty (false);
    return size_compliance;
  }

  void GridHLayout::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    std::list<Area *> elements;
    std::list<Area *>::iterator it = _layout_element_list.begin ();

    GfxContext.PushModelViewMatrix(Get2DMatrix());

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); ++it)
    {
      if ((*it)->IsVisible())
        elements.push_back(*it);
    }

    it = elements.begin();

    Geometry base = GetGeometry();
    Geometry parent_geometry = GetAbsoluteGeometry();
    Geometry visibility_geometry = parent_geometry;
    if (GetToplevel())
    {
      parent_geometry = GetToplevel()->GetAbsoluteGeometry();
    }

    visibility_geometry = parent_geometry.Intersect(GetAbsoluteGeometry());

    GfxContext.PushClippingRectangle(base);
  
    bool first = false;
    bool last = false;
    for (int j = 0; j < _num_row; j++)
    {
      for (int i = 0; i < _num_column; i++)
      {
        if (it == elements.end())
          break;

        Geometry intersection = visibility_geometry.Intersect((*it)->GetAbsoluteGeometry());

        // Test if the element is inside the Grid before rendering.
        if (!intersection.IsNull())
        {
          first = true;
          int X = base.x + m_h_out_margin + i * (_children_size.width + m_h_in_margin);
          int Y = base.y + m_v_out_margin + j * (_children_size.height + m_v_in_margin);

          GfxContext.PushClippingRectangle (Geometry (X, Y, _children_size.width, _children_size.height));

          if ((*it)->IsView ())
          {
            View *ic = NUX_STATIC_CAST (View *, (*it));
            ic->ProcessDraw (GfxContext, force_draw);
          }
          else if ((*it)->IsLayout ())
          {
            Layout *layout = NUX_STATIC_CAST (Layout *, (*it));
            layout->ProcessDraw (GfxContext, force_draw);
          }

          GfxContext.PopClippingRectangle ();
        }
        else
        {
          if (first)
            last = true;
        }

        if (first && last)
          break;
        it++;
      }
      if (first && last)
        break;
    }

    GfxContext.PopClippingRectangle ();
    GfxContext.PopModelViewMatrix ();

    _queued_draw = false;
  }

  Area* GridHLayout::KeyNavIteration(KeyNavDirection direction)
  {
    if (_layout_element_list.size() == 0)
      return NULL;

    if (next_object_to_key_focus_area_)
    {
      std::list<Area*>::iterator it;
      std::list<Area*>::iterator it_next;
      it = std::find (_layout_element_list.begin(), _layout_element_list.end(), next_object_to_key_focus_area_);
      it_next = it;
      ++it_next;

      if (it == _layout_element_list.end())
      {
        // Should never happen
        nuxAssert (0);
        return NULL;
      }

      int position = GetChildPos(*it); // note that (*it) == next_object_to_key_focus_area_
      int nun_column = GetNumColumn();
      int nun_row = GetNumRow();

      if ((direction == KEY_NAV_LEFT) && (it == _layout_element_list.begin()))
      {
        // first item
        return NULL;
      }

      if ((direction == KEY_NAV_RIGHT) && (it_next == _layout_element_list.end()))
      {
        // last item
        return NULL;
      }

      if ((direction == KEY_NAV_LEFT) && ((position % nun_column) == 0))
      {
        // Left edge
        return NULL;
      }
      
      if ((direction == KEY_NAV_RIGHT) && (position == (position / nun_column) * nun_column + (nun_column -1)))
      {
        // right edge
        return NULL;
      }

      if ((direction == KEY_NAV_UP) && ((position / nun_column) == 0))
      {
        // top edge
        return NULL;
      }

      if ((direction == KEY_NAV_DOWN) && ((position / nun_column) == nun_row))
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
          ++it;
          int pos = GetChildPos(*it);

          if ((it == _layout_element_list.end()) || (pos == (pos / nun_column) * nun_column + (nun_column -1)))
            break;

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
        return (*it)->KeyNavIteration(direction);
      }

      if (direction == KEY_NAV_DOWN)
      {
        for (int i = 0; i < nun_column; ++i)
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
