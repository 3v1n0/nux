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
    m_active_area (NULL)

  {
    m_ContentStacking = eStackLeft;
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

  void LayeredLayout::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    std::list<Area *>::iterator it;

    Geometry base = GetGeometry ();
    GfxContext.PushClippingRectangle (base);

    _layout_element_list.reverse ();

    for (it = _layout_element_list.begin (); it != _layout_element_list.end (); ++it)
    {
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
    }

    _layout_element_list.reverse ();

    GfxContext.PopClippingRectangle ();
    _queued_draw = false;
  }

}
