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
  NUX_IMPLEMENT_OBJECT_TYPE (LayeredLayout);

  LayeredLayout::LayeredLayout (NUX_FILE_LINE_DECL)
  : Layout (NUX_FILE_LINE_PARAM),
    m_active_index (0),
    m_active_area (NULL),
    m_paint_all (false)

  {
    m_ContentStacking = eStackLeft;
  }

  LayeredLayout::~LayeredLayout()
  {

  }

  void LayeredLayout::GetCompositeList (std::list<Area *> *ViewList)
  {
    if (m_active_area)
      ViewList->push_back (m_active_area);
    return;
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
    nux::Geometry base = GetGeometry();
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin (); it != _layout_element_list.end (); ++it)
    {
      (*it)->SetMinimumSize (base.width, base.height);
      (*it)->SetGeometry (base);
      (*it)->ComputeLayout2 ();
    }
    return eCompliantHeight | eCompliantWidth;
  }

  void LayeredLayout::PaintOne (Area *_area, GraphicsEngine &GfxContext, bool force_draw)
  {
    if (_area->IsArea ())
    {
      CoreArea *area = NUX_STATIC_CAST (CoreArea *, _area);
      area->OnDraw (GfxContext, force_draw);
    }
    else if (_area->IsView ())
    {
      View *ic = NUX_STATIC_CAST (View *, _area);
      ic->ProcessDraw (GfxContext, force_draw);
    }
    else if (_area->IsLayout ())
    {
      Layout *layout = NUX_STATIC_CAST (Layout *, _area);
      layout->ProcessDraw (GfxContext, force_draw);
    }
  }

  void LayeredLayout::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry ();
    GfxContext.PushClippingRectangle (base);
    
    if (m_paint_all)
    {
      std::list<Area *>::iterator it, eit = _layout_element_list.end ();

      for (it = _layout_element_list.begin (); it != eit; ++it)
        PaintOne (static_cast<Area *> (*it), GfxContext, force_draw);
    }
    else if (m_active_area)
    {
      PaintOne (m_active_area, GfxContext, force_draw);
    }
        
    GfxContext.PopClippingRectangle ();
    _queued_draw = false;
  }

  long LayeredLayout::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    
    if (m_active_area)
    {
      if ( m_active_area->IsArea())
      {
        CoreArea *area = NUX_STATIC_CAST (CoreArea *, m_active_area);
        ret = area->OnEvent (ievent, ret, ProcessEventInfo);
      }
      else if (m_active_area->IsView())
      {
        View *ic = NUX_STATIC_CAST (View *, m_active_area);
        ret = ic->ProcessEvent (ievent, ret, ProcessEventInfo);
      }
      else if (m_active_area->IsLayout())
      {
        Layout *layout = NUX_STATIC_CAST (Layout *, m_active_area);
        ret = layout->ProcessEvent (ievent, ret, ProcessEventInfo);
      }
    }

    return ret;
  }

  void LayeredLayout::AddLayout (Layout                *layout,
                                 unsigned int           stretch_factor,
                                 MinorDimensionPosition positioning,
                                 MinorDimensionSize     extend,
                                 float                  percentage)
  {
    if (!m_active_area)
    {
      m_active_area = layout;
      NeedRedraw ();
    }

    Layout::AddLayout (layout, stretch_factor, positioning, extend, percentage);
  }
  
  void LayeredLayout::AddView (Area                  *view,
                               unsigned int           stretch_factor,
                               MinorDimensionPosition positioning,
                               MinorDimensionSize     extend,
                               float                  percentage)
  {
    if (!m_active_area)
    {
      m_active_area = view;
      NeedRedraw ();
    }

    Layout::AddView (view, stretch_factor, positioning, extend, percentage);
  }

  void LayeredLayout::RemoveChildObject (Area *area)
  {
    if (m_active_area == area)
    {
      std::list<Area *>::iterator it, eit = _layout_element_list.end ();
      int i = 0;

      m_active_index = 0;
      m_active_area = NULL;
      for (it = _layout_element_list.begin (); it != eit; ++it)
      {
        if (*it != area)
        {
          m_active_area = static_cast<Area *> (*it);
          m_active_index = i;
          break;
        }
        i++;
      }
    }

    Layout::RemoveChildObject (area);
  }

  void LayeredLayout::Clear ()
  {
    m_active_index = 0;
    m_active_area = NULL;

    Layout::Clear ();
  }

  //
  // LayeredLayout Methods
  //
  void LayeredLayout::SetActiveLayer (t_uint32 index_)
  {
    std::list<Area *>::iterator it, eit = _layout_element_list.end ();
    t_uint32 i = 0;

    g_return_if_fail (index_ < _layout_element_list.size ());

    if (index_ == m_active_index)
      return;

    m_active_index = index_;
    m_active_area = NULL;

    for (it = _layout_element_list.begin (); it != eit; ++it)
    {
      if (i == m_active_index)
      {
        m_active_area = static_cast<Area *> (*it);
        break;
      }
      i++;
    }
    NeedRedraw ();
  }

  void LayeredLayout::SetPaintAll (bool paint_all)
  {
    if (m_paint_all == paint_all)
      return;

    m_paint_all = paint_all;
    NeedRedraw ();
  }
}
