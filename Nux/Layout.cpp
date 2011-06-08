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
    _has_focus_control  = false;
    _queued_draw        = false;
    _ignore_focus       = false;

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
    bool was_focused = GetFocused ();
    std::list<Area *>::iterator it;
    it = std::find (_layout_element_list.begin(), _layout_element_list.end(), bo);

    if ((*it)->GetFocused ())
    {
      // the child was focused, oh dear. lets handle that gracefully
      if (_layout_element_list.size() > 1)
      {
        if (it == _layout_element_list.end ())
        {
          FocusPreviousChild ((*it));
        }
        else
        {
          FocusNextChild ((*it));
        }
      }

      (*it)->SetFocused (false);
    }

    sigc::connection onchildfocuscon = _connection_map[bo];
    onchildfocuscon.disconnect ();

    if (it != _layout_element_list.end())
    {
      /* we need to emit the signal before the unparent, just in case
         one of the callbacks wanted to use this object */
      ViewRemoved.emit (this, bo);
      bo->UnParentObject();
      _layout_element_list.erase (it);
    }

    if (IsEmpty () && was_focused)
    {
      // we are now empty, so we need to handle our focus state
      Area *area = GetParentObject ();
      if (area == NULL)
        return;


      if (area->IsLayout ())
      {
        Layout *parent = (Layout *)area;
        parent->SetFocused (true);
      }
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

  bool Layout::HasFocusableEntries ()
  {
    std::list<Area *>::const_iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      bool can_focus = false;
      if ( (*it)->IsArea() )
      {
        CoreArea *area = NUX_STATIC_CAST (CoreArea *, (*it) );
        can_focus = area->CanFocus ();
      }
      else if ( (*it)->IsView() )
      {
        View *ic = NUX_STATIC_CAST (View *, (*it) );
        can_focus = ic->CanFocus ();
      }
      else if ( (*it)->IsLayout() )
      {
        Layout *layout = NUX_STATIC_CAST (Layout *, (*it) );
        can_focus = layout->CanFocus ();
      }

      if (can_focus == true)
        return true;
    }

    return false;
  }

  void Layout::OnChildFocusChanged (Area *parent, Area *child)
  {
    ChildFocusChanged.emit (parent, child);
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

    if (HasFocusControl () && HasFocusableEntries () == false)
    {
      //layout->SetFocused (true);
      //ChildFocusChanged (this, layout);
    }

    _layout_element_list.push_back (layout);
    _connection_map[layout] = layout->ChildFocusChanged.connect (sigc::mem_fun (this, &Layout::OnChildFocusChanged));

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

    //if (HasFocusControl () && HasFocusableEntries () == false)
    //{
      //bo->SetFocused (true);
      //ChildFocusChanged (this, bo);
    //}


    _layout_element_list.push_back (bo);
    _connection_map[bo] = bo->ChildFocusChanged.connect (sigc::mem_fun (this, &Layout::OnChildFocusChanged));

    ViewAdded.emit (this, bo);
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

  void Layout::DoActivateFocus ()
  {
    std::list<Area *>::const_iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->GetFocused ())
      {
        (*it)->ActivateFocus ();
        break;
      }
    }
  }

  bool Layout::FocusFirstChild ()
  {
    if (_layout_element_list.empty ())
      return false;
    
    std::list<Area *>::const_iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->CanFocus ())
      {
        (*it)->SetFocused (true);
        ChildFocusChanged (this, (*it));
        return true;
      }
    }

    return false;
  }

  bool Layout::FocusLastChild ()
  {
    if (_layout_element_list.empty ())
      return false;
    
    std::list<Area *>::reverse_iterator it;
    for (it = _layout_element_list.rbegin(); it != _layout_element_list.rend(); it++)
    {
      if ((*it)->CanFocus ())
      {
        (*it)->SetFocused (true);
        ChildFocusChanged (this, (*it));
        return true;
      }
    }

    return false;
  }


  bool Layout::FocusNextChild (Area *child)
  {
    std::list<Area *>::const_iterator it;
    bool found_child = false;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if (found_child)
      {
        if ((*it)->CanFocus ())
        {
          (*it)->SetFocused (true);
          ChildFocusChanged (this, (*it));
          return true;
        }
      }

      if ((*it) == child)
      {
        found_child = true;
      }
    }

    return false;
  }

  bool Layout::FocusPreviousChild (Area *child)
  {
    std::list<Area *>::reverse_iterator it;
    bool found_child = false;

    for (it = _layout_element_list.rbegin(); it != _layout_element_list.rend(); it++)
    {

      if (found_child)
      {
        if ((*it)->CanFocus ())
        {
          (*it)->SetFocused (true);
          ChildFocusChanged (this, (*it));
          return true;
        }
      }

      if ((*it) == child)
      {
        found_child = true;
      }
    }

    return false;
  }


  void Layout::SetFocusControl (bool focus_control)
  {
    _has_focus_control = focus_control;
  }

  bool Layout::HasFocusControl ()
  {
    return _has_focus_control;
  }


  long Layout::SendEventToArea (Area *area, IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    // if parent is null return, thats a valid usecase so no warnings.
    if (area == NULL)
    {
      return 0;
    }

    long ret = 0;
    if ( area->IsView() )
    {
      View *ic = NUX_STATIC_CAST (View *, area );
      ret = ic->ProcessFocusEvent (ievent, ret, ProcessEventInfo);
    }
    else if ( area->IsLayout() )
    {
      Layout *layout = NUX_STATIC_CAST (Layout *, area );
      layout->SetFocusControl (true);
      SetFocusControl (false);
      ret = layout->ProcessFocusEvent (ievent, ret, ProcessEventInfo);
    }

    return ret;
  }

  Area* Layout::GetFocusedChild ()
  {
    std::list<Area *>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->GetFocused ())
      {
        return (*it);
      }
    }

    return NULL;
  }

  long Layout::DoFocusPrev (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    Area *focused_child = GetFocusedChild ();
    if (focused_child == NULL) return ProcessEventInfo;
    bool success = FocusPreviousChild (focused_child);
    //focused_child->SetFocused (false);
    
    if (success == false)
    {
      Area *parent = GetParentObject ();
      if (parent != NULL)
        return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
      else
        FocusFirstChild ();
    }

    return ProcessEventInfo;
  }
  
  long Layout::DoFocusNext (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    Area *focused_child = GetFocusedChild ();
    if (focused_child == NULL) return ProcessEventInfo;
    bool success = FocusNextChild(focused_child);
    //focused_child->SetFocused (false);

    if (success == false)
    {
      Area *parent = GetParentObject ();
      if (parent != NULL)
        return SendEventToArea (parent, ievent, TraverseInfo, ProcessEventInfo);
      else
        FocusFirstChild ();
    }

    return ProcessEventInfo;
  }
  
  long Layout::DoFocusUp (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return DoFocusPrev (ievent, TraverseInfo, ProcessEventInfo);
  }
  long Layout::DoFocusDown (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return DoFocusNext (ievent, TraverseInfo, ProcessEventInfo);
  }
  long Layout::DoFocusLeft (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return DoFocusPrev (ievent, TraverseInfo, ProcessEventInfo);
  }
  long Layout::DoFocusRight (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return DoFocusNext (ievent, TraverseInfo, ProcessEventInfo);
  }


  long Layout::ProcessFocusEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
   

    if (GetFocused () && (ievent.e_event == NUX_KEYDOWN))
    {
      Area *focused_child = NULL;
      Area *parent = GetParentObject ();
      FocusDirection direction;
      FocusEventType type;

      direction = FOCUS_DIRECTION_NONE;

      type = Focusable::GetFocusableEventType (ievent.e_event,
                                               ievent.GetKeySym(),
                                               ievent.GetText(),
                                               &direction);

      if ((type == FOCUS_EVENT_DIRECTION) && (direction != FOCUS_DIRECTION_NONE))
      {
        focused_child = GetFocusedChild ();
  
        /* if nothing is focused, focus the first child else send the event to the parent */

        if (focused_child == NULL)
        {
          bool have_focused_child = FocusFirstChild ();
          if (have_focused_child == false)
          {
            /* propagate up */
            if ((parent != NULL) && parent->IsLayout ())
            {
              ret |= SendEventToArea (parent, ievent, ret, ProcessEventInfo);
            }
            else
            {
              FocusFirstChild ();
            }
          }
        }
        else // we have a focused child
        {
          // if the focused child is the last/first handle next/prev for those
          if (direction == FOCUS_DIRECTION_PREV)
          {
            if (focused_child == _layout_element_list.front ())
            {
              if (parent != NULL)
              {
                ret |= SendEventToArea (parent, ievent, ret, ProcessEventInfo);
              }
              else
              {
                FocusLastChild ();
              }
              return ret;
            }
          }
          else if (direction == FOCUS_DIRECTION_NEXT)
          {
            if (focused_child == _layout_element_list.back ())
            {
              if (parent != NULL)
              {
                ret |= SendEventToArea (parent, ievent, ret, ProcessEventInfo);
              }
              else
              {
                FocusFirstChild ();
              }
              return ret;
            }
          }

          switch (direction)
          {
            case FOCUS_DIRECTION_NEXT:
              ret |= DoFocusNext (ievent, ret, ProcessEventInfo);
              break;
            case FOCUS_DIRECTION_PREV:
              ret |= DoFocusPrev (ievent, ret, ProcessEventInfo);
              break;
            case FOCUS_DIRECTION_UP:
              ret |= DoFocusUp (ievent, ret, ProcessEventInfo);
              break;
            case FOCUS_DIRECTION_DOWN:
              ret |= DoFocusDown (ievent, ret, ProcessEventInfo);
              break;
            case FOCUS_DIRECTION_LEFT:
              ret |= DoFocusLeft (ievent, ret, ProcessEventInfo);
              break;
            case FOCUS_DIRECTION_RIGHT:
              ret |= DoFocusRight (ievent, ret, ProcessEventInfo);
              break;
            default:
              break;
          }

          return ret;
        }
      }

      if (type == FOCUS_EVENT_ACTIVATE)
      {
        ActivateFocus ();
      }
    }

    return ret;
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

    /* must do focus processing after sending events to children */
    if ((ievent.e_event == NUX_KEYDOWN) && HasFocusControl () && (_ignore_focus == false))
    {
      ret |= ProcessFocusEvent (ievent, ret, ProcessEventInfo);
    }

    if (_ignore_focus)
      _ignore_focus = false;

    return ret;
  }

  Area* Layout::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusion(mouse_position, event_type);

    if(mouse_inside == false)
      return NULL;

    std::list<Area *>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      Area* hit_view = NUX_STATIC_CAST(Area*, (*it)->FindAreaUnderMouse(mouse_position, event_type));
      if(hit_view)
        return hit_view;
    }

    return NULL;
  }

  void Layout::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    std::list<Area *>::iterator it;
    GfxContext.PushModelViewMatrix (Get2DMatrix ());
    GfxContext.PushClippingRectangle (GetGeometry ());

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

    GfxContext.PopClippingRectangle ();
    GfxContext.PopModelViewMatrix ();

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

  /* Focusable Code */
  bool Layout::DoGetFocused ()
  {
    bool focused = false;

    std::list<Area *>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->GetFocused ())
        focused = true;
    }

    return focused;
  }

  void Layout::DoSetFocused (bool focused)
  {
    _is_focused = focused;
    if (focused == GetFocused ())
      return;

    if (focused == false)
    {
      std::list<Area *>::iterator it;

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if ((*it)->GetFocused ())
        {
          if ( (*it)->IsArea() )
          {
            CoreArea *area = NUX_STATIC_CAST (CoreArea *, (*it) );
            area->SetFocused (false);
          }
          else if ( (*it)->IsView() )
          {
            View *ic = NUX_STATIC_CAST (View *, (*it) );
            ic->SetFocused (false);
          }
          else if ( (*it)->IsLayout() )
          {
            Layout *layout = NUX_STATIC_CAST (Layout *, (*it) );
            layout->SetFocused (false);
          }
        }

      }
    }
    else
    {
      SetFocusControl (true);
      bool has_focused_child = false;
      std::list<Area *>::iterator it;
      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if ((*it)->GetFocused ())
          has_focused_child = true;
      }

      if (GetFocused () == false)
      {
        FocusFirstChild ();
        _ignore_focus = true;
      }

      // we need to chain up
      Area *_parent = GetParentObject();
      if (_parent == NULL)
      {
        return;
      }

      if (_parent->IsView ())
      {
        View *parent = (View*)_parent;
        if (parent->GetFocused () == false && parent->HasPassiveFocus () == false)
        {
          parent->SetFocused (true);
        }
        parent->SetFocusControl (false);
      }
      else if (_parent->IsLayout ())
      {
        Layout *parent = (Layout *)_parent;
        parent->SetFocusControl (false);
      }

    }
  }

  bool Layout::DoCanFocus ()
  {
    std::list<Area *>::iterator it;
    if (_layout_element_list.empty ())
      return false;
    
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      bool can_focus = false;
      if ( (*it)->IsArea() )
      {
        CoreArea *area = NUX_STATIC_CAST (CoreArea *, (*it) );
        can_focus = area->CanFocus ();
      }
      else if ( (*it)->IsView() )
      {
        View *ic = NUX_STATIC_CAST (View *, (*it) );
        can_focus = ic->CanFocus ();
      }
      else if ( (*it)->IsLayout() )
      {
        Layout *layout = NUX_STATIC_CAST (Layout *, (*it) );
        can_focus = layout->CanFocus ();
      }

      if (can_focus == true)
        return true;
    }

    return false;
  }
}
