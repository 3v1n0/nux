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
#include "Layout.h"
#include "View.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE (Layout);

  Layout::Layout (NUX_FILE_LINE_DECL)
    :   Area (NUX_FILE_LINE_PARAM)
  {
    m_h_in_margin       = 0;
    m_h_out_margin      = 0;
    m_v_in_margin       = 0;
    m_v_out_margin      = 0;
    m_contentWidth      = 0;
    m_contentHeight     = 0;
    m_ContentStacking   = eStackExpand;
    _queued_draw        = false;

    SetMinimumSize(1, 1);
  }

  Layout::~Layout()
  {
    // It is possible that this layout object is in the refresh list. Remove it here before it is deleted.
    GetWindowThread()->RemoveObjectFromLayoutQueue(this);

    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      (*it)->UnParentObject();
    }

    _layout_element_list.clear();
  }


  void Layout::RemoveChildObject (Area *bo)
  {
    std::list<Area *>::iterator it;
    it = std::find (_layout_element_list.begin(), _layout_element_list.end(), bo);

    if (it != _layout_element_list.end())
    {
      bo->UnParentObject();
      _layout_element_list.erase (it);
    }
  }

  bool Layout::FindWidget (Area *WidgetObject) const
  {
    std::list<Area *>::const_iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ( (*it) == WidgetObject)
      {
        return true;
      }
    }

    return false;
  }

  bool Layout::IsEmpty() const
  {
    return (_layout_element_list.size() == 0);
  }

// If(stretchfactor == 0): the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
// and the children will take their natural size by expending WidgetLayout.
// If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
  void Layout::AddLayout (Layout *layout, unsigned int stretchFactor, MinorDimensionPosition minor_position, MinorDimensionSize minor_size, float percentage)
  {
    nuxAssertMsg (layout != 0, TEXT ("[Layout::AddView] Invalid parameter.") );
    NUX_RETURN_IF_TRUE (layout == 0);
    //  Should never happen
    nuxAssertMsg (layout != this, TEXT ("[Layout::AddLayout] Error: Trying to add a layout to itself.") );
    NUX_RETURN_IF_FALSE (layout != 0);

    Area *parent = layout->GetParentObject();
    nuxAssertMsg (parent == 0, TEXT ("[Layout::AddLayout] Trying to add an object that already has a parent.") );
    NUX_RETURN_IF_TRUE (parent != 0);

    layout->SetStretchFactor (stretchFactor);
    layout->SetPositioning (minor_position);
    layout->SetExtend (minor_size);

    if (percentage < 1.0f)
    {
      layout->SetPercentage (1.0f);
    }
    else if (percentage > 100.0f)
    {
      layout->SetPercentage (100.0f);
    }
    else
    {
      layout->SetPercentage (percentage);
    }

    layout->SetParentObject (this);

    layout->OnChildQueueDraw.connect (sigc::mem_fun (this, &Layout::ChildLayoutChildQueuedDraw));
    layout->OnQueueDraw.connect (sigc::mem_fun (this, &Layout::ChildLayoutQueuedDraw));

    _layout_element_list.push_back (layout);

    //--->> Removed because it cause problem with The splitter widget: ComputeLayout2();
  }

//! Add an object to the layout.
  /*! Add an object to the layout.
      A baseobject minor dimension with respect to a layout object is the dimension opposite to the layout flow.
      A baseobject major dimension with respect to a layout object is the dimension aligned with the layout flow.
      A layout object minor dimension is the dimension opposite to the layout flow.
      A layout object major dimension is the dimension aligned with the layout flow.

      Add an object to the layout. The added object get its size and position managed by the layout.
      When a baseobject is added with a stretches factor equal to 0, its major dimension assumes its minimum value.
      For instance, if the layout is a vertical layout and the added object has a stretch factor equal 0, then during the layout,
      the added object height will be set to its minimum value using ApplyMinHeight().

      The minor_position parameter controls how the layout will place the object within itself. A vertical layout object controls the horizontal positioning
      of its children baseobjects, While an horizontal layout object controls the vertical positioning of its children baseobjects.

      The minor_size parameter controls how much size the baseobject minor dimension gets from the layout minor dimension. See MinorDimensionSize.

      /param baseobject The object that is being added.
      /param stretchFactor This value controls how the layout object share space between its children baseobject.
      /param minor_position Controls how the layout position the object.
      /param minor_size Controls the object minor dimension size.
      /param percentage Controls the object minor dimension size in percentage of the layout minor dimension size.
  */

  void Layout::AddView (Area *bo, unsigned int stretchFactor, MinorDimensionPosition minor_position, MinorDimensionSize minor_size, float percentage)
  {
    nuxAssertMsg (bo != 0, TEXT ("[Layout::AddView] Invalid parameter.") );
    NUX_RETURN_IF_TRUE (bo == 0);

    Area *parent = bo->GetParentObject();
    nuxAssertMsg (parent == 0, TEXT ("[Layout::AddView] Trying to add an object that already has a parent.") );
    NUX_RETURN_IF_TRUE (parent != 0);

    bo->SetStretchFactor (stretchFactor);
    bo->SetPositioning (minor_position);
    bo->SetExtend (minor_size);

    if (percentage < 1.0f)
    {
      bo->SetPercentage (1.0f);
    }
    else if (percentage > 100.0f)
    {
      bo->SetPercentage (100.0f);
    }
    else
    {
      bo->SetPercentage (percentage);
    }

    bo->SetParentObject (this);

    if (bo->IsView ())
      static_cast<View *> (bo)->OnQueueDraw.connect (sigc::mem_fun (this, &Layout::ChildViewQueuedDraw));

    _layout_element_list.push_back (bo);

    //--->> Removed because it cause problem with The splitter widget: ComputeLayout2();
  }

  void Layout::AddSpace (unsigned int width, unsigned int stretchFactor)
  {
    AddLayout (new SpaceLayout(), stretchFactor);
  }

  void Layout::Clear()
  {
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      (*it)->UnParentObject();
    }

    _layout_element_list.clear();
  }

  bool Layout::SearchInAllSubNodes (Area *bo)
  {
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ( (*it) == bo)
      {
        return true;
      }
      else if ( (*it)->IsLayout() )
      {
        Layout *layout = NUX_STATIC_CAST (Layout *, (*it) );

        if (layout->SearchInAllSubNodes (bo) )
        {
          return true;
        }
      }
    }

    return false;
  }

  bool Layout::SearchInFirstSubNodes (Area *bo)
  {
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ( (*it) == bo)
      {
        return true;
      }
    }

    return false;
  }

  unsigned int Layout::GetMaxStretchFactor()
  {
    unsigned int value = 0;
    unsigned int sf;
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      sf = (*it)->GetStretchFactor();

      if (sf >= value)
      {
        value = sf;
      }
    }

    return value;
  }

  unsigned int Layout::GetMinStretchFactor()
  {
    unsigned int value = 0xFFFFFFFF;
    unsigned int sf;
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      sf = (*it)->GetStretchFactor();

      if (sf <= value)
      {
        value = sf;
      }
    }

    return value;
  }

  unsigned int Layout::GetNumStretchFactor (unsigned int sf)
  {
    unsigned int count = 0;
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ( (*it)->GetStretchFactor() == sf)
      {
        count++;
      }
    }

    return count;
  }

  void Layout::DoneRedraw()
  {
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ( (*it)->IsView() )
      {
        View *ic = NUX_STATIC_CAST (View *, (*it) );
        ic->DoneRedraw();
      }
    }
  }

  long Layout::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if (!(*it)->IsVisible () || !(*it)->IsSensitive ())
        continue;

      if ((*it)->IsView())
      {
        View *ic = NUX_STATIC_CAST (View*, (*it));
        ret = ic->ProcessEvent (ievent, ret, ProcessEventInfo);
      }
      else if ( (*it)->IsLayout() )
      {
        Layout *layout = NUX_STATIC_CAST (Layout*, (*it));
        ret = layout->ProcessEvent (ievent, ret, ProcessEventInfo);
      }
      // InputArea should be tested last
      else if ((*it)->IsInputArea())
      {
        InputArea *input_area = NUX_STATIC_CAST (InputArea*, (*it));
        ret = input_area->OnEvent (ievent, ret, ProcessEventInfo);
      }
    }

    return ret;
  }

  void Layout::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if (!(*it)->IsVisible ())
        continue;

      if ((*it)->IsView())
      {
        View *ic = NUX_STATIC_CAST (View*, (*it));
        ic->ProcessDraw (GfxContext, force_draw);
      }
      else if ((*it)->IsLayout())
      {
        Layout *layout = NUX_STATIC_CAST (Layout*, (*it));
        layout->ProcessDraw (GfxContext, force_draw);
      }
      // InputArea should be tested last
      else if ((*it)->IsInputArea())
      {
        InputArea *input_area = NUX_STATIC_CAST (InputArea*, (*it));
        input_area->OnDraw (GfxContext, force_draw);
      }
    }

    _queued_draw = false;
  }

  void Layout::QueueDraw ()
  {
    if (_queued_draw)
    {
      // A draw has already been scheduled.
      return;
    }

    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsView ())
      {
        View *ic = NUX_STATIC_CAST (View *, (*it));
        ic->NeedRedraw ();
      }
      else if ((*it)->IsLayout ())
      {
        Layout *layout = NUX_STATIC_CAST (Layout *, (*it));
        layout->NeedRedraw ();
      }
    }

    _queued_draw = true;
    OnQueueDraw.emit (this);
  }

  void Layout::NeedRedraw ()
  {
    QueueDraw ();
  }

  bool Layout::IsQueuedForDraw ()
  {
    return _queued_draw;
  }

  void Layout::SetContentDistribution (LayoutContentDistribution stacking)
  {
    m_ContentStacking = stacking;
  }

  LayoutContentDistribution Layout::GetContentDistribution()
  {
    return m_ContentStacking;
  }

  void Layout::RequestBottomUpLayoutComputation (Area *bo_initiator)
  {

  }

  void Layout::ChildViewQueuedDraw (View *view)
  {
    OnChildQueueDraw.emit (view);
  }

  void Layout::ChildLayoutQueuedDraw (Layout *layout)
  {
    OnChildQueueDraw.emit (layout);
  }

  void Layout::ChildLayoutChildQueuedDraw (Area *area)
  {
    OnChildQueueDraw.emit (area);
  }
}
