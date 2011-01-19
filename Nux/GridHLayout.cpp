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
    _partial_visibility = false;
    _num_row = 1;
    _num_column = 1;
    _dynamic_column = true;

    // Start packing the elements from the top. Is the layout has more space than the elements can use,
    // leave that space at the bottom of the GridHLayout.
    m_ContentStacking = eStackLeft;

  }

  GridHLayout::~GridHLayout()
  {

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
    if (_layout_element_list.size() == 0)
    {
      return eCompliantHeight | eCompliantWidth;
    }

    std::list<Area *>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      (*it)->setOutofBound (false);
    }

    t_s32 original_height = GetBaseHeight();

    // The grid layout goes through the child elements and assign them a size and position. Children are filled in the grid like this:
    //  0   1   2   3   4   5
    //  6   7   8   9   10  11
    //  12  13  ..  ..  ..  ..
    // This is a left to right fill going down. An option can be added the fill the grid from top to bottom and going toward the right.

    nux::Geometry base = GetGeometry();
    it = _layout_element_list.begin();
    int num_elements = (int) _layout_element_list.size();
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

    if (GetStretchFactor() == 0)
      SetBaseHeight (num_row * _children_size.height + 2 * m_v_out_margin + (num_row - 1) * m_v_in_margin);

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
    std::list<Area *>::iterator it = _layout_element_list.begin ();

    Geometry base = GetGeometry ();
    GfxContext.PushClippingRectangle (base);

    for (int j = 0; j < _num_row; j++)
    {
      for (int i = 0; i < _num_column; i++)
      {
        if (it == _layout_element_list.end ())
          break;

        int X = base.x + m_h_out_margin + i * (_children_size.width + m_h_in_margin);
        int Y = base.y + m_v_out_margin + j * (_children_size.height + m_v_in_margin);

        GfxContext.PushClippingRectangle (Geometry (X, Y, _children_size.width, _children_size.height));

        if ((*it)->IsArea ())
        {
          CoreArea *area = NUX_STATIC_CAST (CoreArea *, (*it));
          area->OnDraw (GfxContext, force_draw);
        }
        else if ((*it)->IsView ())
        {
          View *ic = NUX_STATIC_CAST (View *, (*it) );
          ic->ProcessDraw (GfxContext, force_draw);
        }
        else if ((*it)->IsLayout ())
        {
          Layout *layout = NUX_STATIC_CAST (Layout *, (*it));
          layout->ProcessDraw (GfxContext, force_draw);
        }
        GfxContext.PopClippingRectangle ();

        it++;
      }
    }
    GfxContext.PopClippingRectangle ();
    _queued_draw = false;
  }
}
