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

  Layout::Layout(NUX_FILE_LINE_DECL)
    :   Area(NUX_FILE_LINE_PARAM)
  {
    space_between_children_ = 0;

    left_padding_      = 0;
    right_padding_     = 0;    
    top_padding_      = 0;
    bottom_padding_   = 0;
    m_contentWidth      = 0;
    m_contentHeight     = 0;
    m_ContentStacking   = eStackExpand;
    draw_cmd_queued_        = false;
    child_draw_cmd_queued_  = false;

    SetMinimumSize(1, 1);
  }

  Layout::~Layout()
  {
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

  void Layout::SetPadding(int padding)
  {
    top_padding_ = padding < 0 ? 0 : padding;
    bottom_padding_ = top_padding_;
    right_padding_ = top_padding_;
    left_padding_ = top_padding_;
  }

  void Layout::SetPadding(int top_bottom_padding, int left_right_padding)
  {
    top_padding_ = top_bottom_padding < 0 ? 0 : top_bottom_padding;
    bottom_padding_ = top_padding_;

    right_padding_ = left_right_padding < 0 ? 0 : left_right_padding;
    left_padding_ = right_padding_;
  }


  void Layout::SetPadding(int top, int right, int bottom, int left)
  {
    top_padding_ = top < 0 ? 0 : top;
    right_padding_ = right < 0 ? 0 : right;
    bottom_padding_ = bottom < 0 ? 0 : bottom;
    left_padding_ = left < 0 ? 0 : left;
  }

  //! Deprecated. Use SetLeftAndRightPadding.
  void Layout::SetHorizontalExternalMargin(int padding)
  {
    SetLeftAndRightPadding(padding);
  }

  //! Deprecated. Use SetTopAndBottomPadding,
  void Layout::SetVerticalExternalMargin(int padding)
  {
    SetTopAndBottomPadding(padding);
  }

  void Layout::RemoveChildObject(Area *bo)
  {
    std::list<Area *>::iterator it;
    it = std::find(_layout_element_list.begin(), _layout_element_list.end(), bo);

    if (it != _layout_element_list.end())
    {
      /* we need to emit the signal before the un-parent, just in case
         one of the callbacks wanted to use this object */
      ViewRemoved.emit(this, bo);
      bo->UnParentObject();
      _layout_element_list.erase(it);
    }
  }

  bool Layout::FindWidget(Area *WidgetObject) const
  {
    std::list<Area *>::const_iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it) == WidgetObject)
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
  void Layout::AddLayout(Layout *layout, unsigned int stretchFactor, MinorDimensionPosition minor_position, MinorDimensionSize minor_size, float percentage, LayoutPosition index)
  {
    nuxAssertMsg(layout != 0, "[Layout::AddView] Invalid parameter.");
    NUX_RETURN_IF_TRUE(layout == 0);
    //  Should never happen
    nuxAssertMsg(layout != this, "[Layout::AddLayout] Error: Trying to add a layout to itself.");
    NUX_RETURN_IF_FALSE(layout != 0);

    Area *parent = layout->GetParentObject();
    nuxAssertMsg(parent == 0, "[Layout::AddLayout] Trying to add an object that already has a parent.");
    NUX_RETURN_IF_TRUE(parent != 0);

    nuxAssertMsg(index >= 0, "[Layout::AddLayout] Invalid index position. Adding at the beginning of the list..");

    layout->SetScaleFactor(stretchFactor);
    layout->SetPositioning(minor_position);
    layout->SetExtend(minor_size);

    if (percentage < 1.0f)
    {
      layout->SetPercentage(1.0f);
    }
    else if (percentage > 100.0f)
    {
      layout->SetPercentage(100.0f);
    }
    else
    {
      layout->SetPercentage(percentage);
    }

    layout->SetParentObject(this);

    layout->child_queue_draw.connect(sigc::mem_fun(this, &Layout::ChildQueueDraw));
    layout->queue_draw.connect(sigc::mem_fun(this, &Layout::ChildQueueDraw));

    if (index < 0)
      index = NUX_LAYOUT_BEGIN;

    if (index == NUX_LAYOUT_END || index >= _layout_element_list.size())
    {
      _layout_element_list.push_back(layout);
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

    ViewAdded.emit(this, layout);
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

  void Layout::AddView(Area *bo, unsigned int stretchFactor, MinorDimensionPosition minor_position, MinorDimensionSize minor_size, float percentage, LayoutPosition index)
  {
    nuxAssertMsg(bo != 0, "[Layout::AddView] Invalid parameter.");
    NUX_RETURN_IF_TRUE(bo == 0);

    if (!bo->IsView())
      return;

    Area *parent = bo->GetParentObject();
    nuxAssertMsg(parent == 0, "[Layout::AddView] Trying to add an object that already has a parent.");
    NUX_RETURN_IF_TRUE(parent != 0);

    nuxAssertMsg(index >= 0, "[Layout::AddView] Invalid index position. Adding at the beginning of the list..");

    bo->SetScaleFactor(stretchFactor);
    bo->SetPositioning(minor_position);
    bo->SetExtend(minor_size);

    if (percentage < 1.0f)
    {
      bo->SetPercentage(1.0f);
    }
    else if (percentage > 100.0f)
    {
      bo->SetPercentage(100.0f);
    }
    else
    {
      bo->SetPercentage(percentage);
    }

    bo->SetParentObject(this);

    if (bo->IsView())
    {
      static_cast<View*> (bo)->queue_draw.connect(sigc::mem_fun(this, &Layout::ChildQueueDraw));
      static_cast<View*> (bo)->child_queue_draw.connect(sigc::mem_fun(this, &Layout::ChildQueueDraw));
    }

    //if(HasFocusControl() && HasFocusableEntries() == false)
    //{
      //bo->SetFocused(true);
      //ChildFocusChanged(this, bo);
    //}

    if (index < 0)
      index = NUX_LAYOUT_BEGIN;

    if (index == NUX_LAYOUT_END || index >= _layout_element_list.size())
    {
      _layout_element_list.push_back(bo);
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

    ViewAdded.emit(this, bo);
    //--->> Removed because it cause problem with The splitter widget: ComputeContentSize();
  }

  void Layout::AddSpace(unsigned int /* width */, unsigned int stretchFactor, LayoutPosition /* index */)
  {
    AddLayout(new SpaceLayout(), stretchFactor);
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

  bool Layout::SearchInAllSubNodes(Area *bo)
  {
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it) == bo)
      {
        return true;
      }
      else if ((*it)->IsLayout())
      {
        Layout *layout = NUX_STATIC_CAST(Layout *, (*it));

        if (layout->SearchInAllSubNodes(bo))
        {
          return true;
        }
      }
    }

    return false;
  }

  bool Layout::SearchInFirstSubNodes(Area *bo)
  {
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it) == bo)
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
      sf = (*it)->GetScaleFactor();

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
      sf = (*it)->GetScaleFactor();

      if (sf <= value)
      {
        value = sf;
      }
    }

    return value;
  }

  unsigned int Layout::GetNumStretchFactor(unsigned int sf)
  {
    unsigned int count = 0;
    std::list<Area *>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->GetScaleFactor() == sf)
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
      if ((*it)->IsView())
      {
        View *ic = NUX_STATIC_CAST(View *, (*it));
        ic->DoneRedraw();
      }
    }
  }

  Area* Layout::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    std::list<Area *>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsVisible() && (*it)->GetInputEventSensitivity())
      {
        Area* hit_view = NUX_STATIC_CAST(Area*, (*it)->FindAreaUnderMouse(mouse_position, event_type));
        if (hit_view)
          return hit_view;
      }
    }

    return NULL;
  }

  void Layout::ProcessDraw(GraphicsEngine &graphics_engine, bool force_draw)
  {
    std::list<Area *>::iterator it;

    if (RedirectRenderingToTexture())
    {
      if (update_backup_texture_ || force_draw || draw_cmd_queued_)
      {
        GetPainter().PushPaintLayerStack();
        BeginBackupTextureRendering(graphics_engine, force_draw);
        {
          graphics_engine.PushModelViewMatrix(Get2DMatrix());

          for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
          {
            if (!(*it)->IsVisible())
              continue;

            if ((*it)->IsView())
            {
              View* view = static_cast<View*>(*it);
              view->ProcessDraw(graphics_engine, force_draw);
            }
            else if ((*it)->IsLayout())
            {
              Layout* layout = static_cast<Layout*>(*it);
              layout->ProcessDraw(graphics_engine, force_draw);
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
        //Geometry xform_geo = GetGraphicsDisplay()->GetGraphicsEngine()->ModelViewXFormRect(GetGeometry());
        if ((force_draw || draw_cmd_queued_) && background_texture_.IsValid())
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

      // Clip against the padding region.
      Geometry clip_geo = GetGeometry();
      clip_geo.OffsetPosition(left_padding_, top_padding_);
      clip_geo.OffsetSize(-left_padding_ - right_padding_, -top_padding_ - bottom_padding_);

      graphics_engine.PushClippingRectangle(clip_geo);

      for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
      {
        if (!(*it)->IsVisible())
          continue;

        if ((*it)->IsView())
        {
          View* view = static_cast<View*>(*it);
          view->ProcessDraw(graphics_engine, force_draw);
        }
        else if ((*it)->IsLayout())
        {
          Layout* layout = static_cast<Layout*>(*it);
          layout->ProcessDraw(graphics_engine, force_draw);
        }
      }

      graphics_engine.PopClippingRectangle();
      graphics_engine.PopModelViewMatrix();
    }

    ResetQueueDraw();
  }

  void Layout::BeginBackupTextureRendering(GraphicsEngine& graphics_engine, bool force_draw)
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

    // Get the current fbo...
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

  void Layout::EndBackupTextureRendering(GraphicsEngine& graphics_engine, bool /* force_draw */)
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

  void Layout::QueueDraw()
  {
    if (draw_cmd_queued_)
    {
      // A draw has already been scheduled.
      return;
    }

    // Report to a parent view with redirect_rendering_to_texture_ set to true that one of its children
    // needs to be redrawn.
    PrepareParentRedirectedView();

    std::list<Area*>::iterator it;

    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsView())
      {
        View* view = static_cast<View*>(*it);
        view->QueueDraw();
      }
      else if ((*it)->IsLayout())
      {
        Layout* layout = static_cast<Layout*>(*it);
        layout->QueueDraw();
      }
    }

    draw_cmd_queued_ = true;
    queue_draw.emit(this);
  }

  bool Layout::IsQueuedForDraw()
  {
    return draw_cmd_queued_;
  }

  bool Layout::ChildQueuedForDraw()
  {
    return child_draw_cmd_queued_;
  }

  void Layout::SetContentDistribution(LayoutContentDistribution stacking)
  {
    m_ContentStacking = stacking;
  }

  LayoutContentDistribution Layout::GetContentDistribution()
  {
    return m_ContentStacking;
  }

  void Layout::RequestBottomUpLayoutComputation(Area * /* bo_initiator */)
  {

  }

  void Layout::ChildQueueDraw(Area* area)
  {
    if (child_draw_cmd_queued_)
      return;
    child_draw_cmd_queued_ = true;
    child_queue_draw.emit(area);
  }

  bool Layout::AcceptKeyNavFocus()
  {
    return false;
  }

  void Layout::ResetQueueDraw()
  {
    std::list<Area*>::iterator it;
    for (it = _layout_element_list.begin(); it != _layout_element_list.end(); it++)
    {
      if ((*it)->IsLayout())
      {
        Layout* layout = NUX_STATIC_CAST(Layout*, (*it));
        if (layout->ChildQueuedForDraw())
        {
          layout->ResetQueueDraw();
        }
      }
      else if ((*it)->IsView())
      {
        View* view = NUX_STATIC_CAST(View*, (*it));
        if (view->GetLayout())
        {
          view->GetLayout()->ResetQueueDraw();
          view->draw_cmd_queued_ = false;
          view->child_draw_cmd_queued_ = false;
        }
      }
    }

    draw_cmd_queued_ = false;
    child_draw_cmd_queued_ = false;
    update_backup_texture_ = false;
  }

  void Layout::GeometryChangePending(bool /* position_about_to_change */, bool /* size_about_to_change */)
  {
    if (IsLayoutDone())
      QueueDraw();
  }

  void Layout::GeometryChanged(bool /* position_has_changed */, bool size_has_changed)
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

#ifdef NUX_GESTURES_SUPPORT
  Area* Layout::GetInputAreaHitByGesture(const GestureEvent &event)
  {
    if (!IsVisible())
      return nullptr;

    if (!IsGestureInsideArea(event))
      return nullptr;

    for (const auto area : _layout_element_list)
    {
      Area *area_hit = area->GetInputAreaHitByGesture(event);
      if (area_hit)
        return area_hit;
    }

    return nullptr;
  }
#endif
}
