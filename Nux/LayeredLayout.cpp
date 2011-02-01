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
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */


#include "Nux.h"
#include "View.h"
#include "LayeredLayout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux
{

  static const t_s32 VERROR = 0;
  NUX_IMPLEMENT_OBJECT_TYPE (LayeredLayout);

  LayeredLayout::LayeredLayout (NUX_FILE_LINE_DECL)
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
    // leave that space at the bottom of the LayeredLayout.
    m_ContentStacking = eStackLeft;

  }

  LayeredLayout::~LayeredLayout()
  {

  }

  void LayeredLayout::EnablePartialVisibility (bool partial_visibility)
  {
    _partial_visibility = partial_visibility;
  }

  void LayeredLayout::SetChildrenSize (int width, int height)
  {
    _children_size = Size (width, height);
  }

  Size LayeredLayout::GetChildrenSize () const
  {
    return _children_size;
  }

  void LayeredLayout::ForceChildrenSize (bool force)
  {
    _force_children_size = force;
  }

  int LayeredLayout::GetNumColumn () const
  {
    return _num_column;
  }

  int LayeredLayout::GetNumRow () const
  {
    return _num_row;
  }

  void LayeredLayout::GetCompositeList (std::list<Area *> *ViewList)
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

  long LayeredLayout::ComputeLayout2()
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

    t_s32 original_width = GetBaseWidth();

    nux::Geometry base = GetGeometry();
    it = _layout_element_list.begin();
    int num_elements = (int) _layout_element_list.size();
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

    if (GetStretchFactor() == 0)
      SetBaseWidth (num_column * _children_size.width + 2 * m_h_out_margin + (num_column - 1) * m_h_in_margin);

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

  void LayeredLayout::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    std::list<Area *>::iterator it = _layout_element_list.begin ();

    Geometry base = GetGeometry ();
    GfxContext.PushClippingRectangle (base);

    for (int i = 0; i < _num_column; i++)
    {
      for (int j = 0; j < _num_row; j++)
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
