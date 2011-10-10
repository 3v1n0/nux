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

  NUX_IMPLEMENT_OBJECT_TYPE(View);

  View::View(NUX_FILE_LINE_DECL)
    :   InputArea(NUX_FILE_LINE_PARAM)
  {
    _font = GetSysFont();
    view_layout_ = NULL;
    draw_cmd_queued_        = false;
    m_TextColor         = Color(1.0f, 1.0f, 1.0f, 1.0f);

    // Set widget default size;
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
  }

  View::~View()
  {
    // It is possible that the window thread has been deleted before the view
    // itself, so check prior to calling.
    WindowThread* wt = GetWindowThread();

    if (wt)
    {
      // It is possible that the object is in the refresh list. Remove it here
      // before it is deleted.
      wt->RemoveObjectFromLayoutQueue(this);
    }

    RemoveLayout();
  }

  long View::ComputeContentSize()
  {
    if (view_layout_)
    {
      //view_layout_->SetDirty(true);
//        if (view_layout_->GetStretchFactor() != 0)
//        {
//            PreLayoutManagement();
//            long ret = view_layout_->ComputeContentSize();
//            return PostLayoutManagement(ret);
//        }
//        else
      {
        PreLayoutManagement();

        int PreWidth = /*view_layout_->*/GetBaseWidth();
        int PreHeight = /*view_layout_->*/GetBaseHeight();

        long ret = view_layout_->ComputeContentSize();

        PostLayoutManagement(ret);
        //return eCompliantWidth | eCompliantHeight;

        int PostWidth = /*view_layout_->*/GetBaseWidth();
        int PostHeight = /*view_layout_->*/GetBaseHeight();

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

        //Area::SetGeometry(view_layout_->GetGeometry());
        return size_compliance;
      }
    }
    else
    {
      PreLayoutManagement();
      int ret = PostLayoutManagement(eCompliantHeight | eCompliantWidth);
      return ret;
    }

    return 0;
  }

  void View::ComputeContentPosition(float offsetX, float offsetY)
  {
    if (view_layout_)
    {
      view_layout_->SetBaseX(GetBaseX());
      view_layout_->SetBaseY(GetBaseY());
      view_layout_->ComputeContentPosition(offsetX, offsetY);
    }
  }

  void View::PreLayoutManagement()
  {
    // Give the managed layout the same size and position as the Control.
    if (view_layout_)
      view_layout_->SetGeometry(GetGeometry());
  }

  long View::PostLayoutManagement(long LayoutResult)
  {
    // Set the geometry of the control to be the same as the managed layout.
    // Only the size is changed. The position of the composition layout hasn't
    // been changed by ComputeContentSize.
    if (view_layout_)
    {
      // If The layout is empty, do not change the size of the parent element.
      if (!view_layout_->IsEmpty())
        Area::SetGeometry(view_layout_->GetGeometry());
    }

    return LayoutResult;
  }

  void View::PreResizeGeometry()
  {

  }

  void View::PostResizeGeometry()
  {
  }

  void View::ProcessDraw(GraphicsEngine &graphics_engine, bool force_draw)
  {
    full_view_draw_cmd_ = false;

    graphics_engine.PushModelViewMatrix(Get2DMatrix());

    if (force_draw)
    {
      draw_cmd_queued_ = true;
      full_view_draw_cmd_ = true;
      Draw(graphics_engine, force_draw);
      DrawContent(graphics_engine, force_draw);
      PostDraw(graphics_engine, force_draw);
    }
    else
    {
      if (draw_cmd_queued_)
      {
        full_view_draw_cmd_ = true;
        Draw(graphics_engine, false);
        DrawContent(graphics_engine, false);
        PostDraw(graphics_engine, false);
      }
      else
      {
        DrawContent(graphics_engine, false);
        PostDraw(graphics_engine, false);
      }
    }

    graphics_engine.PopModelViewMatrix();

    draw_cmd_queued_ = false;
    full_view_draw_cmd_ = false;
  }

  void View::Draw(GraphicsEngine &graphics_engine, bool force_draw)
  {

  }

  void View::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {

  }

  void View::PostDraw(GraphicsEngine &graphics_engine, bool force_draw)
  {

  }

  void View::QueueDraw()
  {
    //GetWindowCompositor()..AddToDrawList(this);
    WindowThread* application = GetWindowThread();
    if (application)
    {
      application->AddToDrawList(this);
      application->RequestRedraw();
      //GetWindowCompositor().AddToDrawList(this);
    }
    if (view_layout_)
      view_layout_->QueueDraw();

    draw_cmd_queued_ = true;
    OnQueueDraw.emit(this);
  }

  void View::NeedSoftRedraw()
  {
    //GetWindowCompositor()..AddToDrawList(this);
    WindowThread* application = GetWindowThread();
    if (application)
    {
        application->AddToDrawList(this);
        application->RequestRedraw();
    }
    //draw_cmd_queued_ = false;
  }

  bool View::IsRedrawNeeded()
  {
    return draw_cmd_queued_;
  }

  bool View::IsFullRedraw() const
  {
    return full_view_draw_cmd_;
  }

  void View::DoneRedraw()
  {
    draw_cmd_queued_ = false;

    if (view_layout_)
    {
      view_layout_->DoneRedraw();
    }
  }

  Layout* View::GetLayout()
  {
    return view_layout_;
  }

  Layout *View::GetCompositionLayout()
  {
    return GetLayout();
  }

  bool View::SetLayout(Layout *layout)
  {
    NUX_RETURN_VALUE_IF_NULL(layout, false);
    nuxAssert(layout->IsLayout());
    NUX_RETURN_VALUE_IF_TRUE(view_layout_ == layout, true);

    Area *parent = layout->GetParentObject();

    if (parent == this)
    {
      nuxAssert(view_layout_ == layout);
      return false;
    }
    else if (parent != 0)
    {
      nuxDebugMsg(0, "[View::SetCompositionLayout] Object already has a parent. You must UnParent the object before you can parenting again.");
      return false;
    }

    if (view_layout_)
    {
      /* we need to emit the signal before the unparent, just in case
         one of the callbacks wanted to use this object */

      LayoutRemoved.emit(this, view_layout_);
      view_layout_->UnParentObject();
    }
    layout->SetParentObject(this);
    view_layout_ = layout;

    GetWindowThread()->QueueObjectLayout(this);

    LayoutAdded.emit(this, view_layout_);

    return true;
  }

  void View::OnChildFocusChanged(/*Area *parent,*/ Area *child)
  {
    ChildFocusChanged.emit(/*parent,*/ child);
  }

  bool View::SetCompositionLayout(Layout *layout)
  {
    return SetLayout(layout);
  }

  void View::RemoveLayout()
  {
    NUX_RETURN_IF_NULL(view_layout_);

    if (view_layout_)
      view_layout_->UnParentObject();

    view_layout_ = 0;
  }

  void View::RemoveCompositionLayout()
  {
    RemoveLayout();
  }

  bool View::SearchInAllSubNodes(Area *bo)
  {
    if (view_layout_)
      return view_layout_->SearchInAllSubNodes(bo);

    return false;
  }

  bool View::SearchInFirstSubNodes(Area *bo)
  {
    if (view_layout_)
      return view_layout_->SearchInFirstSubNodes(bo);

    return false;
  }

  void View::SetGeometry(const Geometry &geo)
  {
    Area::SetGeometry(geo);
    ComputeContentSize();
    PostResizeGeometry();
  }

  void View::SetFont(ObjectPtr<FontTexture> font)
  {
    _font = font;
  }

  ObjectPtr<FontTexture> View::GetFont()
  {
    return _font;
  }

  void View::SetTextColor(const Color &color)
  {
    m_TextColor = color;
  }

  Color View::GetTextColor()
  {
    return m_TextColor;
  }

  void View::EnableView()
  {
    view_enabled_ = false;
  }

  void View::DisableView()
  {
    view_enabled_ = true;
  }

  void View::SetEnableView(bool enable)
  {
    if (enable)
    {
      EnableView();
    }
    else
    {
      DisableView();
    }
  }

  bool View::IsViewEnabled() const
  {
    return view_enabled_;
  }

  void View::GeometryChangePending()
  {
    QueueDraw();
  }

  void View::GeometryChanged()
  {
    QueueDraw();
  }

  Area* View::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    if (view_layout_)
    {
      Area* view = view_layout_->FindAreaUnderMouse(mouse_position, event_type);

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
    else if (view_layout_)
    {
      return view_layout_->KeyNavIteration(direction);
    }

    return NULL;
  }

  bool View::AcceptKeyNavFocus()
  {
    return true;
  }
}
