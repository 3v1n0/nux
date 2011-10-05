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
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */


#include "Nux.h"
#include "View.h"
#include "LayeredLayout.h"

#include <math.h>

namespace nux
{
  class LayeredChildProperties : public Area::LayoutProperties
  {
    public:

    LayeredChildProperties (bool expand, int x, int y, int width, int height)
    : m_expand (expand),
      m_x (x),
      m_y (y),
      m_width (width),
      m_height (height)
    {

    }

    ~LayeredChildProperties ()
    {
    }

    void Update (bool expand, int x, int y, int width, int height)
    {
      m_expand = expand;
      m_x = x;
      m_y = y;
      m_width = width;
      m_height = height;
    }

    bool m_expand;
    int  m_x;
    int  m_y;
    int  m_width;
    int  m_height;

    sigc::signal<void, Area *, bool>::iterator m_vis_it;
  };

  NUX_IMPLEMENT_OBJECT_TYPE (LayeredLayout);

  LayeredLayout::LayeredLayout (NUX_FILE_LINE_DECL)
  : Layout (NUX_FILE_LINE_PARAM),
    m_active_index (0),
    m_active_area (NULL),
    m_paint_all (false),
    m_input_mode (INPUT_MODE_ACTIVE),
    m_child_draw_queued (false)
  {
    m_ContentStacking = eStackLeft;
    OnChildQueueDraw.connect (sigc::mem_fun (this, &LayeredLayout::ChildQueueDraw));
  }

  LayeredLayout::~LayeredLayout()
  {
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

  long LayeredLayout::ComputeContentSize()
  {
    nux::Geometry base = GetGeometry();
    std::list<Area *>::iterator it;
    int total_max_width = 0;
    int total_max_height = 0;
    int ret = 0;

    for (it = _layout_element_list.begin (); it != _layout_element_list.end (); ++it)
    {
      Area                   *area = *it;
      LayeredChildProperties *props;
      Geometry                geo = base;

      props = dynamic_cast<LayeredChildProperties *> (area->GetLayoutProperties ());

      if (props->m_expand)
      {
        int max_width, max_height;

        // It wants to expand, however we need to check that it doesn't need at minimum more
        // space than we have
        max_width = MAX (base.width, area->GetMinimumWidth ());
        max_height = MAX (base.height, area->GetMinimumHeight ());

        geo.width = max_width;
        geo.height = max_height;

        total_max_width = MAX (total_max_width, max_width);
        total_max_height = MAX (total_max_height, max_height);
      }
      else
      {
        geo.x = base.x + props->m_x;
        geo.y = base.y + props->m_y;
        geo.width = props->m_width;
        geo.height = props->m_height;
      }

      (*it)->SetGeometry (geo);
      (*it)->ComputeContentSize ();
    }

    SetBaseSize (total_max_width, total_max_height);

    if (base.width < total_max_width)
      ret |= eLargerWidth;
    else
      ret |= eCompliantWidth; // We don't complain about getting more space

    if (base.height < total_max_height)
      ret |= eLargerHeight;
    else
      ret |= eCompliantHeight; // Don't complain about getting more space

    return ret;
  }

  void LayeredLayout::PaintOne (Area *_area, GraphicsEngine &gfx_context, bool force_draw)
  {
    if (_area->IsArea ())
    {
      InputArea *area = NUX_STATIC_CAST (InputArea *, _area);
      area->OnDraw (gfx_context, force_draw);
    }
    else if (_area->IsView ())
    {
      View *ic = NUX_STATIC_CAST (View *, _area);
      ic->ProcessDraw (gfx_context, force_draw);
    }
    else if (_area->IsLayout ())
    {
      Layout *layout = NUX_STATIC_CAST (Layout *, _area);
      layout->ProcessDraw (gfx_context, force_draw);
    }
  }

  void LayeredLayout::ProcessDraw (GraphicsEngine &gfx_context, bool force_draw)
  {
    Geometry base = GetGeometry ();
    gfx_context.PushClippingRectangle (base);

    if (m_paint_all)
    {
      std::list<Area *>::iterator it, eit = _layout_element_list.end ();
      t_u32 alpha = 0, src = 0, dest = 0;

      gfx_context.GetRenderStates ().GetBlend (alpha, src, dest);
      gfx_context.GetRenderStates ().SetBlend (true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

      nux::GetPainter().PaintBackground(gfx_context, base);
      nux::GetPainter().PushBackgroundStack();

      for (it = _layout_element_list.begin (); it != eit; ++it)
      {
        if ((*it)->IsVisible ())
          PaintOne (static_cast<Area *> (*it), gfx_context, true);
      }

      nux::GetPainter().PopBackgroundStack();

      gfx_context.GetRenderStates ().SetBlend (alpha, src, dest);

      m_child_draw_queued = false;
    }
    else if (m_active_area && m_active_area->IsVisible ())
    {
      PaintOne (m_active_area, gfx_context, force_draw);
    }

    gfx_context.PopClippingRectangle ();
    _queued_draw = false;
  }

  Area* LayeredLayout::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    if(m_active_area == NULL)
    return NULL;
    
    bool mouse_inside = m_active_area->TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if(mouse_inside == false)
      return NULL;

    if(m_input_mode == INPUT_MODE_ACTIVE)
    {
      if (m_active_area && m_active_area->IsVisible () && m_active_area->IsSensitive ())
        return m_active_area->FindAreaUnderMouse(mouse_position, event_type);
    }
    else
    {
      std::list<Area *>::reverse_iterator it, eit = _layout_element_list.rend ();

      for (it = _layout_element_list.rbegin (); it != eit; ++it)
      {
        Area *area = (*it);

        if (area->IsVisible () && area->IsSensitive ())
        {
          return m_active_area->FindAreaUnderMouse(mouse_position, event_type);
        }
      }
    }

    return NULL;
  }

  void LayeredLayout::AddLayout (Layout                *layout,
                                 unsigned int           stretch_factor,
                                 MinorDimensionPosition positioning,
                                 MinorDimensionSize     extend,
                                 float                  percentage)
  {
    AddLayer (layout);
  }

  void LayeredLayout::AddView (Area                  *view,
                               unsigned int           stretch_factor,
                               MinorDimensionPosition positioning,
                               MinorDimensionSize     extend,
                               float                  percentage)
  {
    AddLayer (view);
  }

  void LayeredLayout::RemoveChildObject (Area *area)
  {
    RemoveLayer (area);
  }

  void LayeredLayout::Clear ()
  {
    m_active_index = 0;
    m_active_area = NULL;

    Layout::Clear ();
  }

  void LayeredLayout::ChildQueueDraw (Area *area)
  {
    m_child_draw_queued = true;
  }

  void LayeredLayout::ChildVisibilityChanged (Area *area, bool visible)
  {
    QueueDraw ();
  }

  //
  // LayeredLayout Methods
  //
  void LayeredLayout::AddLayer (Area *area, bool expand, int x, int y, int width, int height)
  {
    LayeredChildProperties *props;

    // return if the area is NULL
    NUX_RETURN_IF_NULL(area);
    // Return if the area already has a parent
    NUX_RETURN_IF_NOTNULL(area->GetParentObject ());

    props = new LayeredChildProperties (expand, x, y, width, height);
    area->SetLayoutProperties (props);

    if (!m_active_area)
    {
      m_active_area = area;
    }

    props->m_vis_it = area->OnVisibleChanged.connect (sigc::mem_fun (this, &LayeredLayout::ChildVisibilityChanged));

    if (area->IsLayout ())
      Layout::AddLayout (static_cast<Layout *> (area));
    else
      Layout::AddView (area);

    QueueDraw ();
  }

  void LayeredLayout::UpdateLayer (Area *area, bool expand, int x, int y, int width, int height)
  {
    LayeredChildProperties *props;

    NUX_RETURN_IF_NULL(area);

    props = dynamic_cast<LayeredChildProperties *>(area->GetLayoutProperties ());
    NUX_RETURN_IF_NULL(props);

    props->Update (expand, x, y, width, height);

    QueueDraw ();
  }

  void LayeredLayout::RemoveLayer (Area *area)
  {
    LayeredChildProperties *props;

    NUX_RETURN_IF_NULL(area);

    props = dynamic_cast<LayeredChildProperties *>(area->GetLayoutProperties ());
    NUX_RETURN_IF_NULL(props);

    (*props->m_vis_it).disconnect ();
    area->SetLayoutProperties (NULL);


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

  void LayeredLayout::SetActiveLayerN (int index_)
  {
    std::list<Area *>::iterator it, eit = _layout_element_list.end ();
    int i = 0;

    NUX_RETURN_IF_FALSE((t_uint32)index_ < _layout_element_list.size ());

    if (index_ == m_active_index)
      return;

    m_active_index = index_;
    m_active_area = NULL;

    for (it = _layout_element_list.begin (); it != eit; ++it)
    {
      if (i == m_active_index && !m_active_area)
      {
        m_active_area = static_cast<Area *> (*it);
      }

      if ((*it)->IsView ())
      {
        static_cast<View *> (*it)->QueueDraw ();
      } 
      else if ((*it)->IsLayout ())
      {
        static_cast<Layout *> (*it)->QueueDraw ();
      }

      i++;
    }

    QueueDraw ();
  }

  int LayeredLayout::GetActiveLayerN ()
  {
    return m_active_index;
  }

  void LayeredLayout::SetActiveLayer  (Area *area)
  {
    std::list<Area *>::iterator it, eit = _layout_element_list.end ();
    int i = 0;

    for (it = _layout_element_list.begin (); it != eit; ++it)
    {
      Area *a = static_cast<Area *> (*it);

      if (area == a)
      {
        SetActiveLayerN (i);
        return;
      }
      i++;
    }
    nuxDebugMsg("[LayeredLayout::LowerBottom] Area (%p) is not a child of LayeredLayout (%p)", area, this);
  }

  void LayeredLayout::OnLayerGeometryChanged(Area* area, Geometry geo)
  {
    // Set the LayeredLayout to the same saize as the active layer;
    if(area && (area == m_active_area))
    {
      SetGeometry(geo);
    }
  }

  Area * LayeredLayout::GetActiveLayer ()
  {
    return m_active_area;
  }

  void LayeredLayout::SetPaintAll (bool paint_all)
  {
    if (m_paint_all == paint_all)
      return;

    m_paint_all = paint_all;
    QueueDraw ();
  }

  bool LayeredLayout::GetPaintAll ()
  {
    return m_paint_all;
  }

  void LayeredLayout::SetInputMode (LayeredLayout::InputMode input_mode)
  {
    if (m_input_mode == input_mode)
      return;

    m_input_mode = input_mode;
  }

  LayeredLayout::InputMode LayeredLayout::GetInputMode ()
  {
    return m_input_mode;
  }

  void LayeredLayout::Raise (Area *area, Area *above)
  {
    std::list<Area *>::iterator it, eit = _layout_element_list.end ();
    std::list<Area *>::iterator area_it = eit;
    std::list<Area *>::iterator above_it = eit;

    NUX_RETURN_IF_NULL(area);
    NUX_RETURN_IF_NULL(above);

    for (it = _layout_element_list.begin (); it != eit; ++it)
    {
      if (above == (*it))
        above_it = it;
      else if (area == (*it))
        area_it = it;
    }

    if (area_it == eit)
    {
      nuxDebugMsg("[LayeredLayout::Raise] Area %p is not a valid layer", area);
      return;
    }
    if (above_it == eit)
    {
      nuxDebugMsg("[LayeredLayout::Raise] Area %p is not a valid layer", above);
      return;
    }

    _layout_element_list.erase (area_it);
    _layout_element_list.insert (++above_it, area);
  }

  void LayeredLayout::Lower (Area *area, Area *below)
  {
    std::list<Area *>::iterator it, eit = _layout_element_list.end ();
    std::list<Area *>::iterator area_it = eit;
    std::list<Area *>::iterator below_it = eit;

    NUX_RETURN_IF_NULL(area);
    NUX_RETURN_IF_NULL(below);

    for (it = _layout_element_list.begin (); it != eit; ++it)
    {
      if (below == (*it))
        below_it = it;
      else if (area == (*it))
        area_it = it;
    }

    if (area_it == eit)
    {
      nuxDebugMsg("[LayeredLayout::Lower] Area %p is not a valid layer", area);
      return;
    }
    if (below_it == eit)
    {
      nuxDebugMsg("[LayeredLayout::Lower] Area %p is not a valid layer", below);
      return;
    }

    _layout_element_list.erase (area_it);
    _layout_element_list.insert (below_it, area);
  }

  void LayeredLayout::RaiseTop (Area *area)
  {
    std::list<Area *>::iterator it, eit = _layout_element_list.end ();
    std::list<Area *>::iterator area_it = eit;

    NUX_RETURN_IF_NULL(area);

    for (it = _layout_element_list.begin (); it != eit; ++it)
    {
      if (area == (*it))
        area_it = it;
    }

    if (area_it == eit)
    {
      nuxDebugMsg("[LayeredLayout::RaiseTop] Area %p is not a valid layer", area);
      return;
    }

    _layout_element_list.erase (area_it);
    _layout_element_list.insert (eit, area);
  }

  void LayeredLayout::LowerBottom (Area *area)
  {
    std::list<Area *>::iterator it, eit = _layout_element_list.end ();
    std::list<Area *>::iterator area_it = eit;

    NUX_RETURN_IF_NULL(area);

    for (it = _layout_element_list.begin (); it != eit; ++it)
    {
      if (area == (*it))
        area_it = it;
    }

    if (area_it == eit)
    {
      nuxDebugMsg("[LayeredLayout::LowerBottom] Area %p is not a valid layer", area);
      return;
    }

    _layout_element_list.erase (area_it);
    _layout_element_list.insert (_layout_element_list.begin (), area);
  }

  Area* LayeredLayout::KeyNavIteration(KeyNavDirection direction)
  {
    if (m_active_area == NULL)
      return NULL;

    if (m_active_area->IsVisible() == false)
      return NULL;

    if (next_object_to_key_focus_area_)
    {
      return NULL;
    }
    else
    {
      return m_active_area->KeyNavIteration(direction);
    }

    return NULL;
  }
}
