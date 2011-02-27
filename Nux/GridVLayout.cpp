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
#include "GridVLayout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux
{

  static const t_s32 VERROR = 0;
  NUX_IMPLEMENT_OBJECT_TYPE (GridVLayout);

  GridVLayout::GridVLayout (NUX_FILE_LINE_DECL)
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
    _width_match_content = true;

    // Start packing the elements from the top. Is the layout has more space than the elements can use,
    // leave that space at the bottom of the GridVLayout.
    m_ContentStacking = eStackLeft;

    SetMinimumSize(32, 32);
  }

  GridVLayout::~GridVLayout()
  {

  }

  void GridVLayout::EnablePartialVisibility (bool partial_visibility)
  {
    _partial_visibility = partial_visibility;
  }

  void GridVLayout::SetChildrenSize (int width, int height)
  {
    _children_size = Size (width, height);
  }

  Size GridVLayout::GetChildrenSize () const
  {
    return _children_size;
  }

  void GridVLayout::ForceChildrenSize (bool force)
  {
    _force_children_size = force;
  }

  int GridVLayout::GetNumColumn () const
  {
    return _num_column;
  }

  int GridVLayout::GetNumRow () const
  {
    return _num_row;
  }

  void GridVLayout::SetWidthMatchContent (bool match_content)
  {
    _width_match_content = match_content;
  }

  bool GridVLayout::GetWidthMatchContent () const
  {
    return _width_match_content;
  }

  void GridVLayout::GetCompositeList (std::list<Area *> *ViewList)
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

  long GridVLayout::ComputeLayout2()
  {
    std::list<Area *> elements;

    if (GetStretchFactor() == 0)
    {
      ApplyMinHeight();
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
    }

    t_s32 original_width = GetBaseWidth();

    nux::Geometry base = GetGeometry();
    it = elements.begin();
    int num_row = 0;
    int num_column = 0;

    if(num_elements > 0)
      ++num_column;

    if (_dynamic_column)
    {
      int X = base.x + m_h_out_margin;
      int Y = base.y + m_v_out_margin;

      bool first_element_of_column = true;
      bool first_column = true;

      for(int i = 0; i < num_elements; i++) 
      {
        if (num_column == 1)
          num_row++;

        if(first_element_of_column)
        {
          first_column = false;
          
          first_element_of_column = false;
        }

        if (_force_children_size)
        {
          (*it)->SetMinimumSize (_children_size.width, _children_size.height);
        }

        (*it)->SetGeometry (nux::Geometry (X, Y, _children_size.width, _children_size.height));

        (*it)->ComputeLayout2();

        Y += _children_size.height + m_v_in_margin;

        it++;

        if (!_partial_visibility && (Y + _children_size.height > base.y + base.height - m_v_out_margin))
        {
          X += _children_size.width + m_h_in_margin;
          Y = base.y + m_v_out_margin;

          first_element_of_column = true;
          if(i < num_elements - 1)
            ++num_column;
        }
        else if (Y > base.y + base.height)
        {
          X += _children_size.width + m_h_in_margin;
          Y = base.y + m_v_out_margin;

          first_element_of_column = true;
          if(i < num_elements - 1)
            ++num_column;
        }
      }
    }

    _num_row = num_row;
    _num_column = num_column;

    if ((GetStretchFactor() == 0) || _width_match_content)
    {
      int w = num_column * _children_size.width + 2 * m_h_out_margin + (num_column - 1) * m_h_in_margin;
      SetMinimumWidth (w);
      SetBaseWidth (w);
    }

    long size_compliance = 0L;

    if (GetBaseWidth() > original_width + VERROR)
    {
#if DEBUG_LAYOUT_COMPUTATION
      // The layout has been resized larger in WIDTH to tightly pack its content.
      // Or you can say that the layout refuse to be smaller than total WIDTH of its elements.
      std::cout << "ComputeLayout2: VLayout Width block at " << GetWidth() << std::endl;
#endif
      size_compliance |= eLargerWidth; // need scrollbar
    }
    else if (GetBaseWidth() + VERROR < original_width)
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

    return size_compliance;
  }

  void GridVLayout::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    std::list<Area *> elements;
    std::list<Area *>::iterator it = _layout_element_list.begin ();

    GfxContext.PushModelViewMatrix (Get2DMatrix ());

    for (it = _layout_element_list.begin (); it != _layout_element_list.end (); ++it)
    {
      if ((*it)->IsVisible ())
        elements.push_back (*it);
    }

    it = elements.begin ();

    Geometry base = GetGeometry ();
    GfxContext.PushClippingRectangle (base);

    for (int i = 0; i < _num_column; i++)
    {
      for (int j = 0; j < _num_row; j++)
      {
        if (it == elements.end ())
          break;

        int X = base.x + m_h_out_margin + i * (_children_size.width + m_h_in_margin);
        int Y = base.y + m_v_out_margin + j * (_children_size.height + m_v_in_margin);

        GfxContext.PushClippingRectangle (Geometry (X, Y, _children_size.width, _children_size.height));
        if ((*it)->IsView ())
        {
          View *ic = NUX_STATIC_CAST (View *, (*it) );
          ic->ProcessDraw (GfxContext, force_draw);
        }
        else if ((*it)->IsLayout ())
        {
          Layout *layout = NUX_STATIC_CAST (Layout *, (*it));
          layout->ProcessDraw (GfxContext, force_draw);
        }
        /*// InputArea should be tested last
        else if ((*it)->IsInputArea())
        {
          InputArea *input_area = NUX_STATIC_CAST (InputArea*, (*it));
          input_area->OnDraw (GfxContext, force_draw);
        }*/
        GfxContext.PopClippingRectangle ();

        it++;
      }
    }

    GfxContext.PopClippingRectangle ();
    GfxContext.PopModelViewMatrix ();

    _queued_draw = false;
  }

}
