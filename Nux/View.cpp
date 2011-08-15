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
#include "WindowCompositor.h"
#include "Layout.h"
#include "View.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE (View);

  View::View (NUX_FILE_LINE_DECL)
    :   InputArea (NUX_FILE_LINE_PARAM)
  {
    _font = GetSysFont ();
    _is_view_active     = true; // The view is active by default
    m_CompositionLayout = 0;
    _need_redraw        = false;
    m_UseStyleDrawing   = true;
    m_TextColor         = Color (1.0f, 1.0f, 1.0f, 1.0f);
    _can_pass_focus_to_composite_layout = true;
    _can_focus          = true;

    // Set widget default size;
    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    mouse_down_outside_pointer_grab_area.connect (sigc::mem_fun (this, &View::DoMouseDownOutsideArea));
  }

  View::~View()
  {
    if (GetFocused () && HasPassiveFocus() == false)
    {
      nux::GetWindowThread ()->SetFocusedArea (NULL);
    }

    // It is possible that the object is in the refresh list. Remove it here before it is deleted.
    GetWindowThread()->RemoveObjectFromLayoutQueue(this);

    RemoveLayout();
  }

  void View::DoMouseDownOutsideArea (int x, int y,unsigned long mousestate, unsigned long keystate)
  {
    if (GetFocused ())
    {
      SetFocused (false);
    }
  }

  long View::ProcessFocusEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    // we assume we were chained up to by our layout
    if (GetLayout () == NULL)
      return TraverseInfo;

    Area *parent = GetParentObject ();
    if (parent == NULL)
    {
      GetLayout ()->SetFocused (false);
      GetLayout ()->SetFocused (true); // just reset the layout focus becase we are top level
    }

    if (parent != NULL && parent->IsLayout ())
    {
      Layout *parent_layout = (Layout *)parent;
      return parent_layout->ProcessFocusEvent (ievent, TraverseInfo, ProcessEventInfo);
    }

    return TraverseInfo;
  }

  long View::BaseProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    if ((GetWindowCompositor ().GetExclusiveInputArea () == this) && (!(ProcessEventInfo & EVENT_CYCLE_EXCLUSIVE)))
    {
      // Skip the area that has the exclusivity on events
      return TraverseInfo;
    }

    return ProcessEvent (ievent, TraverseInfo, ProcessEventInfo);
  }

  // NUXTODO: Find better name
  long View::ComputeLayout2()
  {
    return ComputeChildLayout();
  }

  // NUXTODO: Find better name
  void View::ComputePosition2 (float offsetX, float offsetY)
  {
    PositionChildLayout (offsetX, offsetY);
  }

  long View::ComputeChildLayout()
  {
    if (m_CompositionLayout)
    {
      //m_CompositionLayout->SetDirty (true);
//        if(m_CompositionLayout->GetStretchFactor() != 0)
//        {
//            PreLayoutManagement();
//            long ret = m_CompositionLayout->ComputeLayout2();
//            return PostLayoutManagement(ret);
//        }
//        else
      {
        PreLayoutManagement();

        int PreWidth = /*m_CompositionLayout->*/GetBaseWidth();
        int PreHeight = /*m_CompositionLayout->*/GetBaseHeight();

        long ret = m_CompositionLayout->ComputeLayout2();

        PostLayoutManagement (ret);
        //return eCompliantWidth | eCompliantHeight;

        int PostWidth = /*m_CompositionLayout->*/GetBaseWidth();
        int PostHeight = /*m_CompositionLayout->*/GetBaseHeight();

        long size_compliance = 0;

        // The layout has been resized to tightly pack its content
        if (PostWidth > PreWidth)
        {
          size_compliance |= eLargerWidth; // need scrollbar
        }
        else if (PostWidth < PreWidth)
        {
          size_compliance |= eSmallerWidth;
        }
        else
        {
          size_compliance |= eCompliantWidth;
        }

        // The layout has been resized to tightly pack its content
        if (PostHeight > PreHeight)
        {
          size_compliance |= eLargerHeight; // need scrollbar
        }
        else if (PostHeight < PreHeight)
        {
          size_compliance |= eSmallerHeight;
        }
        else
        {
          size_compliance |= eCompliantHeight;
        }

        //Area::SetGeometry(m_CompositionLayout->GetGeometry());
        return size_compliance;
      }
    }
    else
    {
      PreLayoutManagement();
      int ret = PostLayoutManagement (eCompliantHeight | eCompliantWidth);
      return ret;
    }

    return 0;
  }

  void View::PositionChildLayout (float offsetX, float offsetY)
  {
    if (m_CompositionLayout)
    {
      // This section from //1 to //2 is not needed. here we should not do any size management. Only position..
      //1
      if (m_CompositionLayout->GetStretchFactor() != 0)
      {
        m_CompositionLayout->SetGeometry (GetGeometry() );
      }
      else //2
      {
        m_CompositionLayout->SetBaseX (GetBaseX() );
        m_CompositionLayout->SetBaseY (GetBaseY() );
      }

      m_CompositionLayout->ComputePosition2 (offsetX, offsetY);

    }
  }

  void View::PreLayoutManagement()
  {
    // Give the managed layout the same size and position as the Control.
    if (m_CompositionLayout)
      m_CompositionLayout->SetGeometry (GetGeometry() );
  }

  long View::PostLayoutManagement (long LayoutResult)
  {
    // Set the geometry of the control to be the same as the managed layout.
    // Only the size is changed. The position of the composition layout hasn't
    // been changed by ComputeLayout2.
    if (m_CompositionLayout)
    {
      // If The layout is empty, do not change the size of the parent element.
      if (!m_CompositionLayout->IsEmpty() )
        Area::SetGeometry (m_CompositionLayout->GetGeometry() );
    }

    return LayoutResult;
  }

  void View::PreResizeGeometry()
  {

  }

  void View::PostResizeGeometry()
  {
  }

  long View::PostProcessEvent2 (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return OnEvent (ievent, TraverseInfo, ProcessEventInfo);
  }

  void View::ProcessDraw (GraphicsEngine &GfxContext, bool force_draw)
  {
    _full_redraw = false;

    GfxContext.PushModelViewMatrix (Get2DMatrix ());

    if (force_draw)
    {
      _need_redraw = true;
      _full_redraw = true;
      Draw (GfxContext, force_draw);
      DrawContent (GfxContext, force_draw);
      PostDraw (GfxContext, force_draw);
    }
    else
    {
      if (_need_redraw)
      {
        _full_redraw = true;
        Draw (GfxContext, false);
        DrawContent (GfxContext, false);
        PostDraw (GfxContext, false);
      }
      else
      {
        DrawContent (GfxContext, false);
        PostDraw (GfxContext, false);
      }
    }

// just leave this here, its helpful for debugging focus issues :)
//     if (GetFocused () && _can_pass_focus_to_composite_layout == false)
//     {
//       GetPainter ().Paint2DQuadColor (GfxContext, GetGeometry (), nux::Color (0.2, 1.0, 0.2, 1.0));
//     }

//     if (GetFocused () && _can_pass_focus_to_composite_layout == true)
//     {
//       GetPainter ().Paint2DQuadColor (GfxContext, GetGeometry (), nux::Color (1.0, 0.2, 0.2, 1.0));
//     }


    GfxContext.PopModelViewMatrix ();

    _need_redraw = false;
    _full_redraw = false;
  }

  void View::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void View::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void View::QueueDraw ()
  {
    //GetWindowCompositor()..AddToDrawList(this);
    WindowThread* application = GetWindowThread ();
    if(application)
    {
      application->AddToDrawList(this);
      application->RequestRedraw();
      //GetWindowCompositor().AddToDrawList(this);
    }
    if (m_CompositionLayout)
      m_CompositionLayout->QueueDraw ();

    _need_redraw = true;
    OnQueueDraw.emit (this);
  }

  void View::NeedSoftRedraw()
  {
    //GetWindowCompositor()..AddToDrawList(this);
    WindowThread* application = GetWindowThread ();
    if(application)
    {
        application->AddToDrawList(this);
        application->RequestRedraw();
    }
    //_need_redraw = false;
  }

  bool View::IsRedrawNeeded()
  {
    return _need_redraw;
  }

  bool View::IsFullRedraw() const
  {
    return _full_redraw;
  }

  void View::DoneRedraw()
  {
    _need_redraw = false;

    if (m_CompositionLayout)
    {
      m_CompositionLayout->DoneRedraw();
    }
  }

  Layout* View::GetLayout()
  {
    return m_CompositionLayout;
  }

  Layout *View::GetCompositionLayout()
  {
    return GetLayout ();
  }

  bool View::SetLayout (Layout *layout)
  {
    nuxAssert(layout->IsLayout());
    NUX_RETURN_VALUE_IF_NULL (layout, false);
    NUX_RETURN_VALUE_IF_TRUE (m_CompositionLayout == layout, true);

    Area *parent = layout->GetParentObject();

    if (parent == this)
    {
      nuxAssert (m_CompositionLayout == layout);
      return false;
    }
    else if (parent != 0)
    {
      nuxDebugMsg (0, TEXT ("[View::SetCompositionLayout] Object already has a parent. You must UnParent the object before you can parenting again.") );
      return false;
    }

    if (m_CompositionLayout)
    {
      if (GetFocused ())
        m_CompositionLayout->SetFocused (false);

      _on_focus_changed_handler.disconnect ();

      /* we need to emit the signal before the unparent, just in case
         one of the callbacks wanted to use this object */

      LayoutRemoved.emit (this, m_CompositionLayout);
      m_CompositionLayout->UnParentObject();
    }
    layout->SetParentObject (this);
    m_CompositionLayout = layout;

    GetWindowThread()->QueueObjectLayout (this);
    if (GetFocused ())
      layout->SetFocused (true);

    _on_focus_changed_handler = layout->ChildFocusChanged.connect (sigc::mem_fun (this, &View::OnChildFocusChanged));

    LayoutAdded.emit (this, m_CompositionLayout);

    return true;
  }

  void View::OnChildFocusChanged (/*Area *parent,*/ Area *child)
  {
    ChildFocusChanged.emit (/*parent,*/ child);
  }

  bool View::SetCompositionLayout (Layout *layout)
  {
    return SetLayout (layout);
  }

  void View::RemoveLayout()
  {
    NUX_RETURN_IF_NULL(m_CompositionLayout);

    if (m_CompositionLayout)
      m_CompositionLayout->UnParentObject();

    m_CompositionLayout = 0;
  }

  void View::RemoveCompositionLayout()
  {
    RemoveLayout();
  }

  bool View::SearchInAllSubNodes (Area *bo)
  {
    if (m_CompositionLayout)
      return m_CompositionLayout->SearchInAllSubNodes (bo);

    return false;
  }

  bool View::SearchInFirstSubNodes (Area *bo)
  {
    if (m_CompositionLayout)
      return m_CompositionLayout->SearchInFirstSubNodes (bo);

    return false;
  }

  void View::SetGeometry (const Geometry &geo)
  {
    Area::SetGeometry (geo);
    ComputeChildLayout();
    PostResizeGeometry();
  }

  void View::SetFont (ObjectPtr<FontTexture> font)
  {
    _font = font;
  }

  ObjectPtr<FontTexture> View::GetFont ()
  {
    return _font;
  }

  void View::SetTextColor (const Color &color)
  {
    m_TextColor = color;
  }

  Color View::GetTextColor()
  {
    return m_TextColor;
  }

  void View::ActivateView ()
  {
    _is_view_active = false;
  }

  void View::DeactivateView ()
  {
    _is_view_active = true;
  }

  bool View::IsViewActive () const
  {
    return _is_view_active;
  }

  void View::GeometryChangePending ()
  {
    QueueDraw ();
  }

  void View::GeometryChanged ()
  {
    QueueDraw ();
  }

  bool View::DoGetFocused ()
  {
    if (HasPassiveFocus ())
      return GetLayout ()->GetFocused ();

    return _is_focused;
  }

  void View::DoSetFocused (bool focused)
  {
    if (GetFocused () == focused)
    {
      return;
    }

    InputArea::DoSetFocused (focused);

    if (HasPassiveFocus ())
    {
      Layout *layout = GetLayout ();

      InputArea::DoSetFocused (focused);

      if (layout != NULL)
      {
        layout->SetFocused (focused);
      }
    }
    else if (focused == true)
    {
      // we only set the focused area if we are not passing focus
      nux::GetWindowThread ()->SetFocusedArea (this);
    }

    if (focused == false)
    {
      bool has_focused_entry = false;
      Area *_parent = GetParentObject ();
      if (_parent == NULL)
        return;

      if (_parent->IsLayout ())
        has_focused_entry = _parent->GetFocused ();

      if (has_focused_entry == false)
        SetFocusControl (false);

    }
  }

  bool View::DoCanFocus ()
  {
    if (IsVisible () == false)
      return false;

    if (_can_focus == false)
      return false;

    if (_can_pass_focus_to_composite_layout)
    {
      if (GetLayout () != NULL)
      {
        return GetLayout ()->CanFocus ();
      }
    }

    return _can_focus;
  }

  void View::SetCanFocus (bool can_focus)
  {
    _can_focus = can_focus;
    if ((_can_focus == false) && GetFocused ())
    {
      SetFocused (false);
    }
  }

  // if we have a layout, returns true if we pass focus to it
  // else returns false
  bool View::HasPassiveFocus ()
  {
    if (_can_pass_focus_to_composite_layout && GetLayout () != NULL)
      return true;

    return false;
  }

  void View::SetFocusControl (bool focus_control)
  {
    Area *_parent = GetParentObject ();
    if (_parent == NULL)
      return;

    if (_parent->IsView ())
    {
      View *parent = (View*)_parent;
      parent->SetFocusControl (focus_control);
    }
    else if (_parent->IsLayout ())
    {
      Layout *parent = (Layout *)_parent;
      parent->SetFocusControl (focus_control);
    }
  }

  bool View::HasFocusControl()
  {
    Area *_parent = GetParentObject();
    if (_parent == NULL)
      return false;

    if (_parent->IsView())
    {
      View *parent = (View*)_parent;
      return parent->HasFocusControl();
    }
    else if (_parent->IsLayout())
    {
      Layout *parent = (Layout *)_parent;
      return parent->HasFocusControl();
    }
    return false;
  }

  Area* View::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    if (m_CompositionLayout)
    {
      Area* view = m_CompositionLayout->FindAreaUnderMouse(mouse_position, event_type);

      if (view)
        return view;
    }

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  Area* View::FindKeyFocusArea(unsigned int key_symbol,
                      unsigned long x11_key_code,
                      unsigned long special_keys_state)
  {
    if (has_key_focus_)
    {
      return this;
    }
    else if (next_object_to_key_focus_area_)
    {
      return next_object_to_key_focus_area_->FindKeyFocusArea(key_symbol, x11_key_code, special_keys_state);
    }
    return NULL;
  }

  Area* View::KeyNavIteration(KeyNavDirection direction)
  {
    if (next_object_to_key_focus_area_)
    {
      return NULL;
    }

    if (IsVisible() == false)
      return NULL;

    if (AcceptKeyNavFocus())
    {
      QueueDraw();
      return this;
    }
    else if (m_CompositionLayout)
    {
      return m_CompositionLayout->KeyNavIteration(direction);
    }

    return NULL;
  }

  bool View::AcceptKeyNavFocus()
  {
    return true;
  }
}
