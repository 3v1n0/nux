/*
 * Copyright 2010, 2012 Inalogic® Inc.
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
  : InputArea(NUX_FILE_LINE_PARAM)
  , m_TextColor(1.0f, 1.0f, 1.0f, 1.0f)
  , view_layout_(nullptr)
  , draw_cmd_queued_(false)
  , child_draw_cmd_queued_(false)
  {
  }

  View::~View()
  {
    backup_fbo_.Release();
    backup_texture_.Release();
    backup_depth_texture_.Release();
    background_texture_.Release();

    RemoveLayout();
  }

  long View::ComputeContentSize()
  {
    if (view_layout_)
    {
      PreLayoutManagement();

      int PreWidth = GetBaseWidth();
      int PreHeight = GetBaseHeight();

      long ret = view_layout_->ComputeContentSize();

      PostLayoutManagement(ret);

      int PostWidth = GetBaseWidth();
      int PostHeight = GetBaseHeight();

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

      return size_compliance;
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

  void View::ProcessDraw(GraphicsEngine& graphics_engine, bool force_draw)
  {
    full_view_draw_cmd_ = false;

    if (RedirectRenderingToTexture())
    {
      if (update_backup_texture_ || force_draw || draw_cmd_queued_)
      {
        GetPainter().PushPaintLayerStack();        
        BeginBackupTextureRendering(graphics_engine, force_draw);
        {
          graphics_engine.PushModelViewMatrix(Get2DMatrix());

          if (force_draw)
          {
            draw_cmd_queued_ = true;
            full_view_draw_cmd_ = true;
            Draw(graphics_engine, force_draw);
            DrawContent(graphics_engine, force_draw);
          }
          else
          {
            if (draw_cmd_queued_)
            {
              full_view_draw_cmd_ = true;
              Draw(graphics_engine, false);
              DrawContent(graphics_engine, false);
            }
            else if (update_backup_texture_)
            {
              DrawContent(graphics_engine, false);
            }
          }
          graphics_engine.PopModelViewMatrix();
        }
        EndBackupTextureRendering(graphics_engine, force_draw);
        GetPainter().PopPaintLayerStack();
      }

      if (PresentRedirectedView())
      {
        unsigned int current_alpha_blend;
        unsigned int current_src_blend_factor;
        unsigned int current_dest_blend_factor;
        // Be a good citizen, get a copy of the current GPU sates according to Nux
        graphics_engine.GetRenderStates().GetBlend(current_alpha_blend, current_src_blend_factor, current_dest_blend_factor);

        TexCoordXForm texxform;

        if ((force_draw || draw_cmd_queued_) && (background_texture_.IsValid()))
        {
          graphics_engine.GetRenderStates().SetBlend(false);
          texxform.FlipVCoord(true);
          // Draw the background of this view.
          GetGraphicsDisplay()->GetGraphicsEngine()->QRP_1Tex(GetX(), GetY(), background_texture_->GetWidth(), background_texture_->GetHeight(), background_texture_, texxform, color::White);
        }

        texxform.uwrap = TEXWRAP_CLAMP;
        texxform.vwrap = TEXWRAP_CLAMP;
        texxform.FlipVCoord(true);

        graphics_engine.GetRenderStates().SetBlend(true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        GetGraphicsDisplay()->GetGraphicsEngine()->QRP_1Tex(GetX(), GetY(), GetWidth(), GetHeight(), backup_texture_, texxform, Color(color::White));
        // Be a good citizen, restore the Nux blending states.
        graphics_engine.GetRenderStates().SetBlend(current_alpha_blend, current_src_blend_factor, current_dest_blend_factor);
      }
    }
    else
    {
      graphics_engine.PushModelViewMatrix(Get2DMatrix());

      if (force_draw)
      {
        GetPainter().PaintBackground(graphics_engine, GetGeometry());
        GetPainter().PushPaintLayerStack();

        draw_cmd_queued_ = true;
        full_view_draw_cmd_ = true;
        Draw(graphics_engine, force_draw);
        DrawContent(graphics_engine, force_draw);

        GetPainter().PopPaintLayerStack();
      }
      else
      {
        if (draw_cmd_queued_)
        {
          GetPainter().PaintBackground(graphics_engine, GetGeometry());
          GetPainter().PushPaintLayerStack();

          full_view_draw_cmd_ = true;
          Draw(graphics_engine, false);
          DrawContent(graphics_engine, false);

          GetPainter().PopPaintLayerStack();
        }
        else
        {
          DrawContent(graphics_engine, false);
        }
      }

      graphics_engine.PopModelViewMatrix();
    }

    if (view_layout_)
    {
      view_layout_->ResetQueueDraw();
    }

    draw_cmd_queued_ = false;
    child_draw_cmd_queued_ = false;
    full_view_draw_cmd_ = false;
    update_backup_texture_ = false;
  }

  void View::BeginBackupTextureRendering(GraphicsEngine& graphics_engine, bool force_draw)
  {
    ObjectPtr<IOpenGLBaseTexture> active_fbo_texture;
    if (force_draw || draw_cmd_queued_)
    {
      // Get the active fbo color texture
      active_fbo_texture = GetGraphicsDisplay()->GetGpuDevice()->ActiveFboTextureAttachment(0);
    }

    Geometry xform_geo;
    // Compute position in the active fbo texture.
    xform_geo = graphics_engine.ModelViewXFormRect(GetGeometry());

    // Get the active fbo...
    prev_fbo_ = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    // ... and the size of the view port rectangle.
    prev_viewport_ = graphics_engine.GetViewportRect();
    
    const int width = GetWidth();
    const int height = GetHeight();

    // Compute intersection with active fbo.
    Geometry intersection = xform_geo;
    if (active_fbo_texture.IsValid())
    {
      Geometry active_fbo_geo(0, 0, active_fbo_texture->GetWidth(), active_fbo_texture->GetHeight());
      intersection = active_fbo_geo.Intersect(xform_geo);
    }

    if (backup_fbo_.IsNull())
    {
      // Create the fbo before using it for the first time.
      backup_fbo_ = GetGraphicsDisplay()->GetGpuDevice()->CreateFrameBufferObject();
    }

    if (!backup_texture_.IsValid() || (backup_texture_->GetWidth() != width) || (backup_texture_->GetHeight() != height))
    {
      // Create or resize the color and depth textures before using them.
      backup_texture_ = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
      backup_depth_texture_ = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
    }

    if (!background_texture_.IsValid() || (background_texture_->GetWidth() != intersection.width) || (background_texture_->GetHeight() != intersection.height))
    {
      background_texture_ = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(intersection.width, intersection.height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
    }

    backup_fbo_->FormatFrameBufferObject(width, height, BITFMT_R8G8B8A8);
    backup_fbo_->EmptyClippingRegion();

    graphics_engine.SetViewport(0, 0, width, height);
    graphics_engine.SetOrthographicProjectionMatrix(width, height);

    // Draw the background on the previous fbo texture
    if ((force_draw || draw_cmd_queued_) && background_texture_.IsValid())
    {
      backup_fbo_->FormatFrameBufferObject(intersection.width, intersection.height, BITFMT_R8G8B8A8);
      backup_fbo_->EmptyClippingRegion();

      graphics_engine.SetViewport(0, 0, intersection.width, intersection.height);
      graphics_engine.SetOrthographicProjectionMatrix(intersection.width, intersection.height);

      // Set the background texture in the fbo
      backup_fbo_->SetTextureAttachment(0, background_texture_, 0);
      backup_fbo_->SetDepthTextureAttachment(ObjectPtr<IOpenGLBaseTexture>(0), 0);
      backup_fbo_->Activate();
      graphics_engine.SetViewport(0, 0, background_texture_->GetWidth(), background_texture_->GetHeight());
      
      // Clear surface
      // Clear surface
      CHECKGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
      CHECKGL(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT));

      TexCoordXForm texxform;
      texxform.uoffset = xform_geo.x / (float) active_fbo_texture->GetWidth();
      texxform.voffset = xform_geo.y / (float) active_fbo_texture->GetHeight();
      texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);
      texxform.flip_v_coord = true;

      // Temporarily change the model-view matrix to copy the texture background texture.
      // This way we are not affceted by the regular model-view matrix.
      graphics_engine.SetModelViewMatrix(Matrix4::IDENTITY());
      // Copy the texture from the previous fbo attachment into our background texture.
      if (copy_previous_fbo_for_background_)
      {
        graphics_engine.QRP_1Tex(0, 0, intersection.width, intersection.height, active_fbo_texture, texxform, color::White);
      }
      else
      {
        graphics_engine.QRP_Color(0, 0, intersection.width, intersection.height, Color(0.0f, 0.0f, 0.0f, 0.0f));
      }
      // Restore the model-view matrix.
      graphics_engine.ApplyModelViewMatrix();
    }

    backup_fbo_->FormatFrameBufferObject(width, height, BITFMT_R8G8B8A8);
    backup_fbo_->EmptyClippingRegion();
    backup_fbo_->SetTextureAttachment(0, backup_texture_, 0);
    backup_fbo_->SetDepthTextureAttachment(backup_depth_texture_, 0);
    backup_fbo_->Activate();

    if (force_draw || draw_cmd_queued_)
    {
      CHECKGL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
      CHECKGL(glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT));
    }

    graphics_engine.SetViewport(0, 0, width, height);
    graphics_engine.SetOrthographicProjectionMatrix(width, height);
    // Transform the geometry of this area through the current model view matrix. This gives the
    // the position of the view in the active fbo texture.
    Geometry offset_rect = graphics_engine.ModelViewXFormRect(GetGeometry());
    int x_offset = -offset_rect.x;
    int y_offset = -offset_rect.y;
    graphics_engine.PushModelViewMatrix(Matrix4::TRANSLATE(x_offset, y_offset, 0));

  }

  void View::EndBackupTextureRendering(GraphicsEngine& graphics_engine, bool /* force_draw */)
  {
    graphics_engine.PopModelViewMatrix();

    if (prev_fbo_.IsValid())
    {
      // Restore the previous fbo
      prev_fbo_->Activate();

      prev_fbo_->ApplyClippingRegion();
    }

    // Release the reference on the previous fbo
    prev_fbo_.Release();

    // Restore the matrices and the view port.
    graphics_engine.ApplyModelViewMatrix();
    graphics_engine.SetOrthographicProjectionMatrix(prev_viewport_.width, prev_viewport_.height);
    graphics_engine.SetViewport(prev_viewport_.x, prev_viewport_.y, prev_viewport_.width, prev_viewport_.height);
  }

  void View::Draw(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {

  }

  void View::DrawContent(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {

  }

  void View::QueueDraw()
  {
    // Report to a parent view with redirect_rendering_to_texture_ set to true that one of its children
    // needs to be redrawn.
    //
    // We should always do this, as the conditions for parent rendering may have
    // changed between child QueueDraw calls (eg window visibility)
    PrepareParentRedirectedView();

    if (draw_cmd_queued_)
      return;

    WindowThread* application = GetWindowThread();
    if (application)
    {
      application->AddToDrawList(this);
      application->RequestRedraw();
    }

    if (view_layout_)
    {
      // If this view has requested a draw, then all of it children in the view_layout_
      // need to be redrawn as well.
      view_layout_->QueueDraw();
    }

    draw_cmd_queued_ = true;
    queue_draw.emit(this);
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

    view_layout_->queue_draw.connect(sigc::mem_fun(this, &View::ChildViewQueuedDraw));
    view_layout_->child_queue_draw.connect(sigc::mem_fun(this, &View::ChildViewQueuedDraw));
    return true;
  }

  void View::ChildViewQueuedDraw(Area* area)
  {
    if (child_draw_cmd_queued_)
      return;
    child_draw_cmd_queued_ = true;
    child_queue_draw.emit(area);
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

//   void View::SetGeometry(int x, int y, int w, int h)
//   {
//     Area::SetGeometry(x, y, w, h);
//     ComputeContentSize();
//     PostResizeGeometry();
//   }

  void View::SetGeometry(const Geometry& geo)
  {
    Area::SetGeometry(geo.x, geo.y, geo.width, geo.height);
    ComputeContentSize();
    PostResizeGeometry();
  }

  void View::SetFont(ObjectPtr<FontTexture> font)
  {
    _font = font;
  }

  ObjectPtr<FontTexture> View::GetFont()
  {
    if (!_font.IsValid())
      _font = GetSysFont();
    return _font;
  }

  void View::SetTextColor(const Color &color)
  {
    m_TextColor = color;
  }

  Color View::GetTextColor() const
  {
    return m_TextColor;
  }

  void View::EnableView()
  {
    view_enabled_ = true;
  }

  void View::DisableView()
  {
    view_enabled_ = false;
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

  void View::GeometryChangePending(bool /* position_about_to_change */, bool /* size_about_to_change */)
  {
    if (IsLayoutDone())
      QueueDraw();
  }

  void View::GeometryChanged(bool /* position_has_changed */, bool size_has_changed)
  {
    if (RedirectedAncestor())
    {
      if (size_has_changed)
        QueueDraw();
      return;
    }
    if (IsLayoutDone())
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

#ifdef NUX_GESTURES_SUPPORT
  Area* View::GetInputAreaHitByGesture(const nux::GestureEvent &event)
  {
    if (!IsVisible())
      return nullptr;

    if (!IsGestureInsideArea(event))
      return nullptr;

    if (view_layout_)
    {
      Area *area = view_layout_->GetInputAreaHitByGesture(event);
      if (area)
        return area;
    }

    if (HasSubscriptionForGesture(event))
      return this;
    else
      return nullptr;
  }
#endif
}
