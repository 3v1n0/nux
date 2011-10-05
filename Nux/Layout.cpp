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
#include "Layout.h"
#include "View.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(Layout);
  NUX_IMPLEMENT_OBJECT_TYPE(SpaceLayout);
  NUX_IMPLEMENT_OBJECT_TYPE(LinearLayout);

  Layout::Layout (NUX_FILE_LINE_DECL)
    :   Area (NUX_FILE_LINE_PARAM)
  {
    space_between_children_ = 0;

    left_padding_      = 0;
    right_padding_     = 0;    
    top_padding_      = 0;
    bottom_padding_   = 0;
    m_contentWidth      = 0;
    m_contentHeight     = 0;
    m_ContentStacking   = eStackExpand;
    _queued_draw        = false;

    SetMinimumSize(1, 1);
  }

  Layout::~Layout()
  {
    // It is possible that this layout object is in the refresh list. Remove
    // it here before it is deleted.
    WindowThread* wt = GetWindowThread();
    if (wt)
      wt->RemoveObjectFromLayoutQueue(this);

    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      (*it)->UnParentObject();
    }

    _layout_element_list.clear();
  }

  int Layout::GetLeftPadding() const
  {
    return left_padding_;
  }

  int Layout::GetRightPadding() const
  {
    return right_padding_;
  }

  int Layout::GetTopPadding() const
  {
    return top_padding_;
  }

  int Layout::GetBottomPadding() const
  {
    return bottom_padding_;
  }

  void Layout::SetLeftAndRightPadding(int padding)
  {
#if DEBUG_LAYOUT
    return;
#endif
    left_padding_ = padding < 0 ? 0 : padding;
    right_padding_ = padding < 0 ? 0 : padding;
  }

  void Layout::SetLeftAndRightPadding(int left, int right)
  {
#if DEBUG_LAYOUT
    return;
#endif
    left_padding_ = left < 0 ? 0 : left;
    right_padding_ = right < 0 ? 0 : right;
  }

  void Layout::SetTopAndBottomPadding(int padding)
  {
#if DEBUG_LAYOUT
    return;
#endif
    top_padding_ = padding < 0 ? 0 : padding;
    bottom_padding_ = padding < 0 ? 0 : padding;
  }

  void Layout::SetTopAndBottomPadding(int top, int bottom)
  {
#if DEBUG_LAYOUT
    return;
#endif
    top_padding_ = top < 0 ? 0 : top;
    bottom_padding_ = bottom < 0 ? 0 : bottom;
  }

  void Layout::SetPadding(int top, int right, int bottom, int left)
  {
    top_padding_ = top < 0 ? 0 : top;
    right_padding_ = right < 0 ? 0 : right;
    bottom_padding_ = bottom < 0 ? 0 : bottom;
    left_padding_ = left < 0 ? 0 : left;
  }

  //! Deprecated. Use SetLeftRightPadding.
  void Layout::SetHorizontalExternalMargin(int padding)
  {
    SetLeftAndRightPadding(padding);
  }

  //! Deprecated. Use SetTopBottomPadding,
  void Layout::SetVerticalExternalMargin(int padding)
  {
    SetTopAndBottomPadding(padding);
  }

  void Layout::RemoveChildObject (Area *bo)
  {
    std::list<Area *>::iterator it;
    it = std::find (_layout_element_list.begin(), _layout_element_list.end(), bo);

    if (it != _layout_element_list.end())
    {
      /* we need to emit the signal before the un-parent, just in case
         one of the callbacks wanted to use this object */
      ViewRemoved.emit (this, bo);
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
  void Layout::AddLayout (Layout *layout, unsigned int stretchFactor, MinorDimensionPosition minor_position, MinorDimensionSize minor_size, float percentage, LayoutPosition index)
  {
    nuxAssertMsg (layout != 0, TEXT ("[Layout::AddView] Invalid parameter.") );
    NUX_RETURN_IF_TRUE (layout == 0);
    //  Should never happen
    nuxAssertMsg (layout != this, TEXT ("[Layout::AddLayout] Error: Trying to add a layout to itself.") );
    NUX_RETURN_IF_FALSE (layout != 0);

    Area *parent = layout->GetParentObject();
    nuxAssertMsg (parent == 0, TEXT ("[Layout::AddLayout] Trying to add an object that already has a parent.") );
    NUX_RETURN_IF_TRUE (parent != 0);

    nuxAssertMsg (index >= 0, TEXT ("[Layout::AddLayout] Invalid index position. Adding at the beginning of the list..") );

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

    if (index < 0)
      index = NUX_LAYOUT_BEGIN;

    if (index == NUX_LAYOUT_END || index >= _layout_element_list.size())
    {
      _layout_element_list.push_back (layout);
    }
    else
    {
      std::list<Area *>::iterator pos = _layout_element_list.begin();
      int idx = index;
      while (pos != _layout_element_list.end() && idx > 0)
      {
        idx--;
        pos++;
      }
      _layout_element_list.insert(pos, layout);
    }

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
      /param index Controls the object position in the layout children.
  */

  void Layout::AddView (Area *bo, unsigned int stretchFactor, MinorDimensionPosition minor_position, MinorDimensionSize minor_size, float percentage, LayoutPosition index)
  {
    nuxAssertMsg (bo != 0, TEXT ("[Layout::AddView] Invalid parameter.") );
    NUX_RETURN_IF_TRUE (bo == 0);

    Area *parent = bo->GetParentObject();
    nuxAssertMsg (parent == 0, TEXT ("[Layout::AddView] Trying to add an object that already has a parent.") );
    NUX_RETURN_IF_TRUE (parent != 0);

    nuxAssertMsg (index >= 0, TEXT ("[Layout::AddView] Invalid index position. Adding at the beginning of the list..") );

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

    //if (HasFocusControl () && HasFocusableEntries () == false)
    //{
      //bo->SetFocused (true);
      //ChildFocusChanged (this, bo);
    //}

    if (index < 0)
      index = NUX_LAYOUT_BEGIN;

    if (index == NUX_LAYOUT_END || index >= _layout_element_list.size())
    {
      _layout_element_list.push_back (bo);
    }
    else
    {
#if defined(NUX_OS_WINDOWS) && !defined(NUX_VISUAL_STUDIO_2010)
      std::list<Area *>::iterator pos = _layout_element_list.begin();
#else
      auto pos = _layout_element_list.begin();
#endif
      int idx = index;
      while (pos != _layout_element_list.end() && idx > 0)
      {
        idx--;
        pos++;
      }
      _layout_element_list.insert(pos, bo);
    }

    ViewAdded.emit (this, bo);
    //--->> Removed because it cause problem with The splitter widget: ComputeContentSize();
  }

  void Layout::AddSpace (unsigned int width, unsigned int stretchFactor, LayoutPosition index)
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

  Area* Layout::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if(mouse_inside == false)
      return NULL;

    std::list<Area *>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsVisible() && (*it)->IsSensitive())
      {
        Area* hit_view = NUX_STATIC_CAST(Area*, (*it)->FindAreaUnderMouse(mouse_position, event_type));
        if(hit_view)
          return hit_view;
      }
    }

    return NULL;
  }

  void Layout::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    std::list<Area *>::iterator it;
    GfxContext.PushModelViewMatrix(Get2DMatrix());

    // Clip against the padding region.
    Geometry clip_geo = GetGeometry();
    clip_geo.OffsetPosition(left_padding_, top_padding_);
    clip_geo.OffsetSize(-left_padding_ - right_padding_, -top_padding_ - bottom_padding_);

    GfxContext.PushClippingRectangle(clip_geo);

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if (!(*it)->IsVisible ())
        continue;

      if ((*it)->IsView ())
      {
        View *ic = NUX_STATIC_CAST (View*, (*it));
        ic->ProcessDraw (GfxContext, force_draw);
      }
      else if ((*it)->IsLayout ())
      {
        Layout *layout = NUX_STATIC_CAST (Layout*, (*it));
        layout->ProcessDraw (GfxContext, force_draw);
      }
      // InputArea should be tested last
      else if ((*it)->IsInputArea ())
      {
        InputArea *input_area = NUX_STATIC_CAST (InputArea*, (*it));
        input_area->OnDraw (GfxContext, force_draw);
      }
    }

    GfxContext.PopClippingRectangle();
    GfxContext.PopModelViewMatrix();

    //GfxContext.PopClipOffset ();

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
        ic->QueueDraw ();
      }
      else if ((*it)->IsLayout ())
      {
        Layout *layout = NUX_STATIC_CAST (Layout *, (*it));
        layout->QueueDraw ();
      }
    }

    _queued_draw = true;
    OnQueueDraw.emit (this);
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

  bool Layout::AcceptKeyNavFocus()
  {
    return false;
  }

  void LinearLayout::SetHorizontalInternalMargin(int space)
  {
    SetSpaceBetweenChildren(space);
  }

  void LinearLayout::SetVerticalInternalMargin(int space)
  {
    SetSpaceBetweenChildren(space);
  }

  void LinearLayout::SetSpaceBetweenChildren(int space)
  {
#if DEBUG_LAYOUT
    return;
#endif
    space_between_children_ = space >= 0 ? space : 0;
  }
}
