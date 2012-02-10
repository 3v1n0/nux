// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2012 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Jason Smith <jason.smith@canonical.com>
 *              Jay Taoko <jay.taoko@canonical.com>
 */

#include "Nux/Nux.h"
#include "Nux/HLayout.h"
#include "Nux/StaticText.h"

#include "NuxGraphics/GraphicsDisplay.h"
#include "NuxGraphics/GLShader.h"
#include "NuxGraphics/GpuDevice.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "NuxGraphics/GLShader.h"
#include "NuxGraphics/GraphicsEngine.h"

#include "Nux/TextLoader.h"

#include <glib.h>

#include "Coverflow.h"


namespace nux
{
namespace
{
  float EaseSin(float x)
  {
    return sin(x * nux::Const::pi / 2.0f);
  }

  float RoundFloor(float x)
  {
    return std::floor(x + 0.5f);
  }

  std::string texture_vertex_code = "attribute vec4 iVertex;                                  \n\
                                    attribute vec4 iTextureCoord0;                            \n\
                                    attribute vec4 iVertexColor;                              \n\
                                    uniform mat4 ViewProjectionMatrix;                        \n\
                                    varying vec4 varyTexCoord0;                               \n\
                                    varying vec4 varyVertexColor;                             \n\
                                    void main()                                               \n\
                                    {                                                         \n\
                                    gl_Position =  ViewProjectionMatrix * (iVertex);          \n\
                                    varyTexCoord0 = iTextureCoord0;                           \n\
                                    varyVertexColor = iVertexColor;                           \n\
                                    }";

  std::string texture_fragment_code = "varying vec4 varyTexCoord0;                            \n\
                                      varying vec4 varyVertexColor;                           \n\
                                      uniform sampler2D TextureObject0;                       \n\
                                      void main()                                             \n\
                                      {                                                       \n\
                                        vec4 v = texture2D(TextureObject0, varyTexCoord0.xy);    \n\
                                        gl_FragColor = v*varyVertexColor;                     \n\
                                      }";

  std::string single_color_fragment_code = "varying vec4 varyTexCoord0;                       \n\
                                      varying vec4 varyVertexColor;                           \n\
                                      uniform sampler2D TextureObject0;                       \n\
                                      void main()                                             \n\
                                      {                                                       \n\
                                        gl_FragColor = varyVertexColor;                       \n\
                                      }";
  struct Vec4
  {
    float x;
    float y;
    float z;
    float rot;
  };

  struct VelocityEvent
  {
    float velocity;
    gint64 time;
  };

  struct Cover
  {
    Vec4 position;
    float opacity;
    bool selected;
    bool mouse_over;
    CoverflowItem::Ptr item;
  };

  typedef std::vector<Cover> CoverList;
}
}

namespace nux
{
  struct Coverflow::Impl : public sigc::trackable
  {
    Impl(Coverflow* parent);
    ~Impl();

    void HandleKeyDown(unsigned long eventType,
                     unsigned long keysym,
                     unsigned long state,
                     const char* character,
                     unsigned short keyCount);
    void HandleKeyUp(unsigned int keysym,
                   unsigned long x11_key_code,
                   unsigned long special_keys_state);

    void HandleMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void HandleMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void HandleMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void HandleMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void HandleMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void HandleMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void HandleMouseClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void HandleMouseWheel(int x, int y, int wheel_delta, unsigned long button_flags, unsigned long key_flags);
    void HandleGeometryChange(Area* area, Geometry geo);

    void DrawCover(nux::GraphicsEngine& graphics_engine, Cover const& cover);
    void DrawCoverHighlight(nux::GraphicsEngine& graphics_engine, Cover const& cover);

    CoverList GetCoverList(float animation_progress, gint64 timestep);

    void OnItemAdded(CoverflowModel* owner, CoverflowItem::Ptr new_item);
    void OnItemRemoved(CoverflowModel* owner, CoverflowItem::Ptr old_item);

    void SetPosition(float position, bool animate);

    float GetCameraDriftFactor();
    void MaybeQueueDraw();

    bool CoverAtPoint(int x, int y, Cover& out_cover);
    void Get3DBoundingBox(float distance_from_camera, nux::Point2& top_left_corner, nux::Point2& bottom_right_corner);
    void GetCoverScreenCoord(Cover const& cover, nux::Vector4& P0, nux::Vector4& P1, nux::Vector4& P2, nux::Vector4& P3);
    void GetFlatCoverScreenSize(float& flat_screen_width, float& flat_screen_height);

    bool TestMouseOverCover(int x, int y, Cover const& cover);
    bool TestCoverVisible(Cover const& cover);

    static gboolean OnAnimationTimeout(gpointer data);
    static gboolean OnTimeout(gpointer data);

    nux::Vector3 camera_position_;
    nux::Vector3 camera_rotation_;

    nux::ObjectPtr<nux::IOpenGLShaderProgram> cover_shader_program_;
    nux::ObjectPtr<nux::IOpenGLShaderProgram> highlight_shader_program_;
    
    nux::Matrix4 perspective_;
    nux::Matrix4 modelview_;

    std::vector<VelocityEvent> velocity_events_;

    Coverflow* parent_;
    guint animation_handle_;
    float camera_drift_factor_;
    gint64 last_draw_time_;
    float last_position_;
    bool mouse_inside_view_;
    float position_;
    gint64 position_set_time_;
    float saved_position_;
    float velocity_;
    guint velocity_handle_;
    nux::Point2 mouse_down_position_;
    nux::Point2 mouse_position_;
    CoverList last_covers_;
    float flat_screen_width_;
    float flat_screen_height_;
    float cover_width_in_3d_space_;
    float near_clip_plan_;
    float far_clip_plan_;
    TextLoader text_loader_;
  };

  Coverflow::Impl::Impl(Coverflow* parent)
   : parent_(parent)
   , animation_handle_(0)
   , camera_drift_factor_(0)
   , last_draw_time_(0)
   , last_position_(0)
   , mouse_inside_view_(false)
   , position_(0)
   , position_set_time_(0)
   , saved_position_(0)
   , velocity_(0)
   , velocity_handle_(0)
   , flat_screen_width_(0.0f)
   , flat_screen_height_(0.0f)
   , cover_width_in_3d_space_(1.0f)
   , near_clip_plan_(1.0f)
   , far_clip_plan_(200.0f)
  {
    mouse_position_ = nux::Point2(0, 0);

    parent_->key_down.connect(sigc::mem_fun(this, &Impl::HandleKeyDown));
    parent_->key_up.connect(sigc::mem_fun(this, &Impl::HandleKeyUp));
    parent_->mouse_move.connect(sigc::mem_fun(this, &Impl::HandleMouseMove));
    parent_->mouse_enter.connect(sigc::mem_fun(this, &Impl::HandleMouseEnter));
    parent_->mouse_leave.connect(sigc::mem_fun(this, &Impl::HandleMouseLeave));
    parent_->mouse_click.connect(sigc::mem_fun(this, &Impl::HandleMouseClick));
    parent_->mouse_drag.connect(sigc::mem_fun(this, &Impl::HandleMouseDrag));
    parent_->mouse_up.connect(sigc::mem_fun(this, &Impl::HandleMouseUp));
    parent_->mouse_down.connect(sigc::mem_fun(this, &Impl::HandleMouseDown));
    parent_->mouse_wheel.connect(sigc::mem_fun(this, &Impl::HandleMouseWheel));
    parent_->OnGeometryChanged.connect(sigc::mem_fun(this, &Impl::HandleGeometryChange));


    camera_position_.x = 0.0f;
    camera_position_.y = 0.0f;
    camera_position_.z = 5.0f;
    camera_rotation_.x = 0.0f;
    camera_rotation_.y = 0.0f;
    camera_rotation_.z = 0.0f;

    // Create a shader to render a textured quad.
    {
      // Create the GLSL shader objects: Vertex, Fragment and the Program.
      // The Program object act as a container for the Vertex and Fragment object.
      nux::ObjectPtr<nux::IOpenGLPixelShader>  fragment_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreatePixelShader();
      nux::ObjectPtr<nux::IOpenGLVertexShader> vertex_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateVertexShader();
      cover_shader_program_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateShaderProgram();

      // Set the actual shader code into the Vertex and Fragment objects.
      vertex_shader_prog_->SetShaderCode(texture_vertex_code.c_str());
      fragment_shader_prog_->SetShaderCode(texture_fragment_code.c_str());

      // Remove all Vertex and Fragment objects from the Program (Not necessary here but do it anyway).
      cover_shader_program_->ClearShaderObjects();
      // Add the Vertex and Fragment objects to the Program object.
      cover_shader_program_->AddShaderObject(vertex_shader_prog_);
      cover_shader_program_->AddShaderObject(fragment_shader_prog_);
      // Link
      cover_shader_program_->Link();

      // The shaders have been loaded into the program. They can be discarded.
      vertex_shader_prog_.Release();
      fragment_shader_prog_.Release();
    }

    // Create a shader to render a the cover highlight.
    {
      // Create the GLSL shader objects: Vertex, Fragment and the Program.
      // The Program object act as a container for the Vertex and Fragment object.
      nux::ObjectPtr<nux::IOpenGLPixelShader>  fragment_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreatePixelShader();
      nux::ObjectPtr<nux::IOpenGLVertexShader> vertex_shader_prog_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateVertexShader();
      highlight_shader_program_ = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateShaderProgram();

      // Set the actual shader code into the Vertex and Fragment objects.
      vertex_shader_prog_->SetShaderCode(texture_vertex_code.c_str());
      fragment_shader_prog_->SetShaderCode(single_color_fragment_code.c_str());

      // Remove all Vertex and Fragment objects from the Program (Not necessary here but do it anyway).
      highlight_shader_program_->ClearShaderObjects();
      // Add the Vertex and Fragment objects to the Program object.
      highlight_shader_program_->AddShaderObject(vertex_shader_prog_);
      highlight_shader_program_->AddShaderObject(fragment_shader_prog_);
      // Link
      highlight_shader_program_->Link();

      // The shaders have been loaded into the program. They can be discarded.
      vertex_shader_prog_.Release();
      fragment_shader_prog_.Release();
    }

    text_loader_.font_size = 24;
  }

  Coverflow::Impl::~Impl()
  {
    if (animation_handle_)
    {
      g_source_remove(animation_handle_);
      animation_handle_ = 0;
    }
  }

  void Coverflow::Impl::Get3DBoundingBox(float distance_from_camera, nux::Point2& top_left_corner, nux::Point2& bottom_right_corner)
  {
    int width = parent_->GetBaseWidth();
    int height = parent_->GetBaseHeight();

    top_left_corner.y = std::tan(DEGTORAD(parent_->fov() * 0.5f)) * distance_from_camera;
    top_left_corner.x = -top_left_corner.y * (width / (float)height);

    bottom_right_corner.x = -top_left_corner.x;
    bottom_right_corner.y = -top_left_corner.y;
  }

  void Coverflow::Impl::GetCoverScreenCoord(Cover const& cover, nux::Vector4& out_p0, nux::Vector4& out_p1, nux::Vector4& out_p2, nux::Vector4& out_p3)
  {
    if (cover.item->GetTexture().IsNull())
      return;

    int width = parent_->GetBaseWidth();
    int height = parent_->GetBaseHeight();

    ObjectPtr<IOpenGLBaseTexture> texture = cover.item->GetTexture()->GetDeviceTexture();
    float ratio = texture->GetWidth()/(float)texture->GetHeight();
    
    float fx = cover_width_in_3d_space_/2.0f;
    float fy = (cover_width_in_3d_space_/2.0f) * (1.0f/ratio);

    modelview_ = nux::Matrix4::TRANSLATE(-camera_position_.x, -camera_position_.y, -camera_position_.z) *
      nux::Matrix4::ROTATEX(DEGTORAD(-camera_rotation_.x)) *
      nux::Matrix4::ROTATEY(DEGTORAD(-camera_rotation_.y)) *
      nux::Matrix4::ROTATEZ(DEGTORAD(-camera_rotation_.z)) *
      nux::Matrix4::TRANSLATE(cover.position.x, cover.position.y, cover.position.z);

    nux::Matrix4 m = nux::Matrix4::ROTATEY(DEGTORAD(cover.position.rot));
    nux::Matrix4 combined_matrix = perspective_ * modelview_ * m;

    nux::Vector4 p0(-fx, fy, 0.0f, 1.0f);
    nux::Vector4 p1(-fx, -fy, 0.0f, 1.0f);
    nux::Vector4 p2(fx, -fy, 0.0f, 1.0f);
    nux::Vector4 p3(fx, fy, 0.0f, 1.0f);

    nux::Vector4 p0_proj = combined_matrix * p0;
    nux::Vector4 p1_proj = combined_matrix * p1;
    nux::Vector4 p2_proj = combined_matrix * p2;
    nux::Vector4 p3_proj = combined_matrix * p3;

    p0_proj.x = p0_proj.x/p0_proj.w; p0_proj.y = p0_proj.y/p0_proj.w; p0_proj.z = p0_proj.z/p0_proj.w;
    p1_proj.x = p1_proj.x/p1_proj.w; p1_proj.y = p1_proj.y/p1_proj.w; p1_proj.z = p1_proj.z/p1_proj.w;
    p2_proj.x = p2_proj.x/p2_proj.w; p2_proj.y = p2_proj.y/p2_proj.w; p2_proj.z = p2_proj.z/p2_proj.w;
    p3_proj.x = p3_proj.x/p3_proj.w; p3_proj.y = p3_proj.y/p3_proj.w; p3_proj.z = p3_proj.z/p3_proj.w;

    p0_proj.x = width * (p0_proj.x + 1.0f)/2.0f;
    p1_proj.x = width * (p1_proj.x + 1.0f)/2.0f;
    p2_proj.x = width * (p2_proj.x + 1.0f)/2.0f;
    p3_proj.x = width * (p3_proj.x + 1.0f)/2.0f;

    p0_proj.y = -height * (p0_proj.y - 1.0f)/2.0f;
    p1_proj.y = -height * (p1_proj.y - 1.0f)/2.0f;
    p2_proj.y = -height * (p2_proj.y - 1.0f)/2.0f;
    p3_proj.y = -height * (p3_proj.y - 1.0f)/2.0f;

    out_p0.x = p0_proj.x; out_p0.y = p0_proj.y;
    out_p1.x = p1_proj.x; out_p1.y = p1_proj.y;
    out_p2.x = p2_proj.x; out_p2.y = p2_proj.y;
    out_p3.x = p3_proj.x; out_p3.y = p3_proj.y;
  }

  void Coverflow::Impl::GetFlatCoverScreenSize(float& flat_screen_width, float& flat_screen_height)
  {
    // BIG ASSUMPTION:
    //  The ratio of the texture.width/texture.height = 1.0f;
    float ratio = 1.0f;

    int width = parent_->GetBaseWidth();
    int height = parent_->GetBaseHeight();

    perspective_.Perspective(DEGTORAD(parent_->fov()), (float)width / (float)height, near_clip_plan_, far_clip_plan_);

    float fx = cover_width_in_3d_space_ / 2.0f;
    float fy = (cover_width_in_3d_space_ / 2.0f) * (1.0f / ratio);

    modelview_ = nux::Matrix4::TRANSLATE(-camera_position_.x, -camera_position_.y, -camera_position_.z) *
      nux::Matrix4::ROTATEX(DEGTORAD(-camera_rotation_.x)) *
      nux::Matrix4::ROTATEY(DEGTORAD(-camera_rotation_.y)) *
      nux::Matrix4::ROTATEZ(DEGTORAD(-camera_rotation_.z));

    nux::Matrix4 m = nux::Matrix4::ROTATEY(DEGTORAD(0.0f));
    nux::Matrix4 combined_matrix = perspective_ * modelview_;

    nux::Vector4 p0(-fx, fy, 0.0f, 1.0f);
    nux::Vector4 p1(-fx, -fy, 0.0f, 1.0f);
    nux::Vector4 p2(fx, -fy, 0.0f, 1.0f);
    nux::Vector4 p3(fx, fy, 0.0f, 1.0f);

    nux::Vector4 p0_proj = combined_matrix * p0;
    nux::Vector4 p1_proj = combined_matrix * p1;
    nux::Vector4 p2_proj = combined_matrix * p2;
    nux::Vector4 p3_proj = combined_matrix * p3;

    p0_proj.x = p0_proj.x/p0_proj.w; p0_proj.y = p0_proj.y/p0_proj.w; p0_proj.z = p0_proj.z/p0_proj.w;
    p1_proj.x = p1_proj.x/p1_proj.w; p1_proj.y = p1_proj.y/p1_proj.w; p1_proj.z = p1_proj.z/p1_proj.w;
    p2_proj.x = p2_proj.x/p2_proj.w; p2_proj.y = p2_proj.y/p2_proj.w; p2_proj.z = p2_proj.z/p2_proj.w;
    p3_proj.x = p3_proj.x/p3_proj.w; p3_proj.y = p3_proj.y/p3_proj.w; p3_proj.z = p3_proj.z/p3_proj.w;

    p0_proj.x = width * (p0_proj.x + 1.0f)/2.0f;
    p1_proj.x = width * (p1_proj.x + 1.0f)/2.0f;
    p2_proj.x = width * (p2_proj.x + 1.0f)/2.0f;
    p3_proj.x = width * (p3_proj.x + 1.0f)/2.0f;

    p0_proj.y = -height * (p0_proj.y - 1.0f)/2.0f;
    p1_proj.y = -height * (p1_proj.y - 1.0f)/2.0f;
    p2_proj.y = -height * (p2_proj.y - 1.0f)/2.0f;
    p3_proj.y = -height * (p3_proj.y - 1.0f)/2.0f;

    flat_screen_width = p3_proj.x - p0_proj.x;
    flat_screen_height = p1_proj.y - p0_proj.y;
  }

  bool Coverflow::Impl::TestMouseOverCover(int x, int y, Cover const& cover)
  {
    nux::Vector4 P0;
    nux::Vector4 P1;
    nux::Vector4 P2;
    nux::Vector4 P3;

    GetCoverScreenCoord(cover, P0, P1, P2, P3);
    // The polygon is convex and P0->P1->P2->P3 follows the right hand rule
    bool test01 = ((y - P0.y) * (P1.x - P0.x) - (x - P0.x) * (P1.y - P0.y)) <= 0.0f;
    bool test12 = ((y - P1.y) * (P2.x - P1.x) - (x - P1.x) * (P2.y - P1.y)) <= 0.0f;
    bool test23 = ((y - P2.y) * (P3.x - P2.x) - (x - P2.x) * (P3.y - P2.y)) <= 0.0f;
    bool test30 = ((y - P3.y) * (P0.x - P3.x) - (x - P3.x) * (P0.y - P3.y)) <= 0.0f;

    if (test01 && test12 && test23 && test30)
    {
      return true;
    }

    return false;
  }

  bool Coverflow::Impl::TestCoverVisible(Cover const& cover)
  {
    int width = parent_->GetBaseWidth();
    int height = parent_->GetBaseHeight();

    nux::Vector4 P0;
    nux::Vector4 P1;
    nux::Vector4 P2;
    nux::Vector4 P3;

    GetCoverScreenCoord(cover, P0, P1, P2, P3);
    bool test0 = ((P0.x >= 0) && (P0.x < width)) && ((P0.y >= 0) && (P0.y < height));
    bool test1 = ((P1.x >= 0) && (P1.x < width)) && ((P1.y >= 0) && (P1.y < height));
    bool test2 = ((P2.x >= 0) && (P2.x < width)) && ((P2.y >= 0) && (P2.y < height));
    bool test3 = ((P3.x >= 0) && (P3.x < width)) && ((P3.y >= 0) && (P3.y < height));

    if (test0 || test1 || test2 || test3)
    {
      // The quad is convex. If any of its vertices is inside the view, then the quad is visible.
      return true;
    }

    return false;
  }

  bool Coverflow::Impl::CoverAtPoint(int x, int y, Cover& out_cover)
  {
    Cover best;

    CoverList::iterator it;
    for (it = last_covers_.begin(); it != last_covers_.end(); ++it)
    {
      Cover cover = *it;
      if (cover.item->GetTexture().IsNull())
        continue;
      if (cover.position.rot > 0 && TestMouseOverCover(mouse_position_.x, mouse_position_.y, cover))
      {
        best = cover;
      }
    }

    CoverList::reverse_iterator rit;
    for (rit = last_covers_.rbegin(); rit != last_covers_.rend(); ++rit)
    {
      Cover cover = *rit;
      if (cover.item->GetTexture().IsNull())
        continue;
      if (cover.position.rot <= 0 && TestMouseOverCover(mouse_position_.x, mouse_position_.y, cover))
      {
        best = cover;
      }
    }

    if (best.item)
    {
      out_cover = best;
      return true;
    }

    return false;
  }

  void Coverflow::Impl::HandleKeyDown(unsigned long   eventType  , /*event type*/
                                  unsigned long   keysym     , /*event keysym*/
                                  unsigned long   state      , /*event state*/
                                  const char*     character  , /*character*/
                                  unsigned short  keyCount     /*key repeat count*/)
  {
    switch (keysym)
    {
      case NUX_VK_LEFT:
        SetPosition(RoundFloor(position_ - 1.0f), true);
        break;
      case NUX_VK_RIGHT:
        SetPosition(RoundFloor(position_ + 1.0f), true);
        break;

    }
  }

  void Coverflow::Impl::HandleMouseClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (std::abs(mouse_down_position_.x - x) > 10 || std::abs(mouse_down_position_.y - y) > 10)
      return;

    Cover best;
    if (CoverAtPoint(x, y, best))
    {
      if (abs(best.position.rot) <= .001)
        best.item->Activate();
      else
        SetPosition((float)parent_->model()->IndexOf(best.item), true);
    }
  }

  void Coverflow::Impl::HandleKeyUp(unsigned int keysym,
                                  unsigned long x11_key_code,
                                  unsigned long special_keys_state)
  {
    switch (keysym)
    {
      case NUX_VK_ENTER:
      case NUX_KP_ENTER:
      {
        Cover best;
        if (CoverAtPoint(0, 0, best))
          best.item->Activate();
        break;
      }
      default:
        break;
    }
  }

  void Coverflow::Impl::HandleMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    mouse_position_.x = x;
    mouse_position_.y = y;
    MaybeQueueDraw();
  }

  void Coverflow::Impl::HandleMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    mouse_position_.x = x;
    mouse_position_.y = y;
    mouse_inside_view_ = true;
    MaybeQueueDraw();
  }

  void Coverflow::Impl::HandleMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    mouse_position_.x = 0xFFFFFFFF;
    mouse_position_.y = 0xFFFFFFFF;
    mouse_inside_view_ = false;
    MaybeQueueDraw();
  }

  void Coverflow::Impl::HandleMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    mouse_down_position_.x = x;
    mouse_down_position_.y = y;
    MaybeQueueDraw();
  }

  void Coverflow::Impl::HandleMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    MaybeQueueDraw();

    velocity_ = 0;
    gint64 current_time = g_get_monotonic_time();

    std::vector<VelocityEvent>::iterator it;
    for (it = velocity_events_.begin(); it != velocity_events_.end(); ++it)
    {
      VelocityEvent event = *it;
      int ms = (current_time - event.time) / 1000;
      if (ms > 32)
        continue;
      
      velocity_ += event.velocity;
    }
    velocity_ = velocity_ / 2; // 16ms timestep
    velocity_events_.clear();

    if (velocity_ != 0 && !velocity_handle_)
    {
      velocity_handle_ = g_timeout_add(16, &Coverflow::Impl::OnTimeout, this);
    } 
    else if (!velocity_handle_)
    {
      SetPosition(RoundFloor(position_), true);
    }
  }

  void Coverflow::Impl::HandleMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    nux::Point2 top_left, bottom_right;
    Get3DBoundingBox(camera_position_.z, top_left, bottom_right);
    nux::Geometry geo = parent_->GetGeometry();

    float scalar = (((float)bottom_right.x - (float)top_left.x) / (float)geo.width) / parent_->space_between_icons;
    SetPosition(position_ - (dx * scalar * parent_->mouse_drag_rate), false);

    VelocityEvent ve;
    ve.velocity = position_ - last_position_;
    ve.time = g_get_monotonic_time();
    velocity_events_.push_back(ve);
  }

  void Coverflow::Impl::HandleMouseWheel(int x, int y, int wheel_delta, unsigned long button_flags, unsigned long key_flags)
  {
    // do nothing yet
  }

  void Coverflow::Impl::HandleGeometryChange(Area* area, Geometry geo)
  {
    GetFlatCoverScreenSize(flat_screen_width_, flat_screen_height_);
  }

  CoverList Coverflow::Impl::GetCoverList(float animation_progress, gint64 timestep)
  {
    CoverList results;

    animation_progress = std::min<float>(1.0f, animation_progress);

    float coverflow_position = position_ - ((position_ - saved_position_) * (1.0f - animation_progress));

    size_t i = 0;
    float flat_right = parent_->space_between_icons * (parent_->flat_icons) + .1;
    float flat_left = -flat_right;

    CoverflowModel::CoverflowItemList::const_iterator it;
    for (it = parent_->model()->Items().begin(); it != parent_->model()->Items().end(); ++it)
    {
      CoverflowItem::Ptr item = *it;
      float item_position = (float)i - coverflow_position;

      Cover cover;
      cover.opacity = 1.0f;
      cover.selected = i == RoundFloor(coverflow_position);
      cover.item = item;
      cover.position = { 0, 0, 0, 0 };

      float x = item_position * parent_->space_between_icons;
      cover.position.x = x;

      float neg_pinch = 1.0f - parent_->pinching;

      if (x < flat_left)
      {
        cover.position.rot = (atan((x - flat_left) * parent_->folding_rate) / (nux::Const::pi / 2)) * -parent_->folding_angle;
        float scale_in_factor = neg_pinch + (1.0f - (std::abs(cover.position.rot) / parent_->folding_angle)) * parent_->pinching;
        cover.position.x = flat_left - ((flat_left - x) * scale_in_factor);
      }
      else if (x > flat_right)
      {
        cover.position.rot = (atan((x - flat_right) * parent_->folding_rate) / (nux::Const::pi / 2)) * -parent_->folding_angle;
        float scale_in_factor = neg_pinch+ (1.0f - (std::abs(cover.position.rot) / parent_->folding_angle)) * parent_->pinching;
        cover.position.x = flat_right + ((x - flat_right) * scale_in_factor);
      }

      if (cover.selected && parent_->pop_out_selected)
        cover.position.z = 0.3f;
      
      float fold_progress = std::abs(cover.position.rot / parent_->folding_angle());
      cover.position.z -= parent_->folding_depth() * fold_progress;

      nux::Point2 top_left, bottom_right;
      Get3DBoundingBox(camera_position_.z, top_left, bottom_right);

      float window_width = bottom_right.x - top_left.x;
      float distance_from_edge = std::max(0.0f, std::abs(bottom_right.x) - std::abs(cover.position.x));
      cover.opacity = std::min(1.0f, distance_from_edge / (window_width * parent_->edge_fade));

      results.push_back(cover);
      ++i;
    }

    last_position_ = coverflow_position;
    return results;
  }

  void Coverflow::Impl::SetPosition(float position, bool animate)
  {
    position = std::max(0.0f, std::min((float)parent_->model()->Items().size() - 1.0f, position));
    if (position == position_)
      return;

    if (animate)
    {
      position_set_time_ = g_get_monotonic_time();
      saved_position_ = last_position_;
    }
    else
    {
      position_set_time_ = 0;
    }

    position_ = position;
    MaybeQueueDraw();
  }

  void Coverflow::Impl::OnItemAdded(CoverflowModel* owner, CoverflowItem::Ptr new_item)
  {
    MaybeQueueDraw();
  }

  void Coverflow::Impl::OnItemRemoved(CoverflowModel* owner, CoverflowItem::Ptr old_item)
  {
    MaybeQueueDraw();
  }

  void Coverflow::Impl::MaybeQueueDraw()
  {
    if (!animation_handle_)
      parent_->QueueDraw();
  }

  void Coverflow::Impl::DrawCoverHighlight(nux::GraphicsEngine& graphics_engine, Cover const& cover)
  {
    if (cover.item->GetTexture().IsNull())
      return;
    
    int width = parent_->GetBaseWidth();
    int height = parent_->GetBaseHeight();

    ObjectPtr<IOpenGLBaseTexture> texture = cover.item->GetTexture()->GetDeviceTexture();

    modelview_ = nux::Matrix4::TRANSLATE(-camera_position_.x, -camera_position_.y, -camera_position_.z) *
                 nux::Matrix4::ROTATEX(DEGTORAD(-camera_rotation_.x)) *
                 nux::Matrix4::ROTATEY(DEGTORAD(camera_drift_factor_ * parent_->camera_motion_drift_angle + -camera_rotation_.y)) *
                 nux::Matrix4::ROTATEZ(DEGTORAD(-camera_rotation_.z)) *
                 nux::Matrix4::TRANSLATE(cover.position.x, cover.position.y, cover.position.z);

    nux::Matrix4 m = nux::Matrix4::ROTATEY(DEGTORAD(cover.position.rot));
    nux::Matrix4 combined_matrix = modelview_ * m;

    nux::TexCoordXForm texxform;
    nux::Color color = nux::color::Red;

    QRP_Compute_Texture_Coord(width, height, texture, texxform);

    float ratio = texture->GetWidth()/(float)texture->GetHeight();

    float fx = (cover_width_in_3d_space_/2.0f);
    float fy = (cover_width_in_3d_space_/2.0f) * (1.0f/ratio);

    highlight_shader_program_->Begin();

    int TextureObjectLocation = highlight_shader_program_->GetUniformLocationARB("TextureObject0");
    int VertexLocation        = highlight_shader_program_->GetAttributeLocation ("iVertex");
    int TextureCoord0Location = highlight_shader_program_->GetAttributeLocation ("iTextureCoord0");
    int VertexColorLocation   = highlight_shader_program_->GetAttributeLocation ("iVertexColor");

    texture->SetFiltering (GL_LINEAR, GL_LINEAR);
    graphics_engine.SetTexture(GL_TEXTURE0, texture);
    CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

    int VPMatrixLocation = highlight_shader_program_->GetUniformLocationARB("ViewProjectionMatrix");
    nux::Matrix4 MVPMatrix = perspective_ * combined_matrix;

    highlight_shader_program_->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, true, (GLfloat*) &(MVPMatrix.m));

    float VtxBuffer[] =
    {
      -fx, fy,  0.0f, 1.0f, texxform.u0, texxform.v0, 0, 0, color.red, color.green, color.blue, color.alpha,
      -fx, -fy, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 0, color.red, color.green, color.blue, color.alpha,
      fx,  -fy, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 0, color.red, color.green, color.blue, color.alpha,
      fx,  fy,  0.0f, 1.0f, texxform.u1, texxform.v0, 0, 0, color.red, color.green, color.blue, color.alpha,
    };

    CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
    CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

    if (TextureCoord0Location != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
      CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
    }

    if (VertexColorLocation != -1)
    {
      CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
      CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
    }

    CHECKGL(glDrawArrays(GL_LINE_LOOP, 0, 4)); // Compatible with OpenGL ES 2.0.

    CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

    if (TextureCoord0Location != -1)
      CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

    if (VertexColorLocation != -1)
      CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

    highlight_shader_program_->End();
  }

  void Coverflow::Impl::DrawCover(nux::GraphicsEngine& graphics_engine, Cover const& cover)
  {
    if (cover.item->GetTexture().IsNull() || !TestCoverVisible(cover))
      return;

    int width = parent_->GetBaseWidth();
    int height = parent_->GetBaseHeight();

    ObjectPtr<IOpenGLBaseTexture> texture = cover.item->GetTexture()->GetDeviceTexture();

    modelview_ = nux::Matrix4::TRANSLATE(-camera_position_.x, -camera_position_.y, -camera_position_.z) *
      nux::Matrix4::ROTATEX(DEGTORAD(-camera_rotation_.x)) *
      nux::Matrix4::ROTATEY(DEGTORAD(camera_drift_factor_ * parent_->camera_motion_drift_angle + -camera_rotation_.y)) *
      nux::Matrix4::ROTATEZ(DEGTORAD(-camera_rotation_.z)) *
      nux::Matrix4::TRANSLATE(cover.position.x, cover.position.y, cover.position.z);

    nux::Matrix4 m = nux::Matrix4::ROTATEY(DEGTORAD(cover.position.rot));
    nux::Matrix4 combined_matrix = modelview_ * m;

    float cover_bottom = 0.0f;
    {
      nux::TexCoordXForm texxform;
      nux::Color color = nux::color::White;

      QRP_Compute_Texture_Coord(width, height, texture, texxform);

      float ratio = texture->GetWidth()/(float)texture->GetHeight();

      float fx = cover_width_in_3d_space_/2.0f;
      float fy = (cover_width_in_3d_space_/2.0f) * (1.0f/ratio);
      
      cover_bottom = -fy;

      cover_shader_program_->Begin();

      int TextureObjectLocation = cover_shader_program_->GetUniformLocationARB("TextureObject0");
      int VertexLocation = cover_shader_program_->GetAttributeLocation("iVertex");
      int TextureCoord0Location = cover_shader_program_->GetAttributeLocation("iTextureCoord0");
      int VertexColorLocation = cover_shader_program_->GetAttributeLocation("iVertexColor");

      texture->SetFiltering (GL_LINEAR, GL_LINEAR);
      graphics_engine.SetTexture(GL_TEXTURE0, texture);
      CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

      int VPMatrixLocation = cover_shader_program_->GetUniformLocationARB("ViewProjectionMatrix");
      nux::Matrix4 MVPMatrix = perspective_ * combined_matrix;

      cover_shader_program_->SetUniformLocMatrix4fv((GLint) VPMatrixLocation, 1, true, (GLfloat*) &(MVPMatrix.m));

      {
        color = color * cover.opacity;
        float VtxBuffer[] =
        {
          -fx, fy,  0.0f, 1.0f, texxform.u0, texxform.v0, 0, 0, color.red, color.green, color.blue, color.alpha,
          -fx, -fy, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 0, color.red, color.green, color.blue, color.alpha,
          fx,  -fy, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 0, color.red, color.green, color.blue, color.alpha,
          fx,  fy,  0.0f, 1.0f, texxform.u1, texxform.v0, 0, 0, color.red, color.green, color.blue, color.alpha,
        };

        CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
        CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

        if (TextureCoord0Location != -1)
        {
          CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
          CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
        }

        if (VertexColorLocation != -1)
        {
          CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
          CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
        }

        CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));
      }

      // **** Reflection opacity ****
      {
        float angular_opacity = 1.0f;
        if (parent_->folding_angle == 0)
        {
          angular_opacity = 1.0f;
        }
        else
        {
          float rotation_ratio = std::abs(cover.position.rot / parent_->folding_angle);
          angular_opacity = 1.0f - rotation_ratio;
        }

        float opacity = 1.0f - std::abs(cover.position.x) / (parent_->reflection_fadeout_distance);
        if (opacity < 0.0f)
        {
          opacity = 0.0f;
        }

        opacity *= angular_opacity;

        texxform.flip_v_coord = true;
        QRP_Compute_Texture_Coord(width, height, texture, texxform);

        float opacity_start = opacity*parent_->reflection_strength * cover.opacity;
        float opacity_end   = 0.0f;
        float VtxBuffer[] =
        {
          -fx, -fy,      0.0f, 1.0f, texxform.u0, texxform.v0, 0, 0, opacity_start, opacity_start, opacity_start, opacity_start,
          -fx, -fy-2*fy, 0.0f, 1.0f, texxform.u0, texxform.v1, 0, 0, opacity_end,   opacity_end,   opacity_end,   opacity_end,
          fx,  -fy-2*fy, 0.0f, 1.0f, texxform.u1, texxform.v1, 0, 0, opacity_end,   opacity_end,   opacity_end,   opacity_end,
          fx,  -fy,      0.0f, 1.0f, texxform.u1, texxform.v0, 0, 0, opacity_start, opacity_start, opacity_start, opacity_start,
        };

        CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
        CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

        if (TextureCoord0Location != -1)
        {
          CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
          CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
        }

        if (VertexColorLocation != -1)
        {
          CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
          CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
        }

        CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));
      }

      // **** Render Cover label ****
      if (parent_->show_covers_label())
      {
        if (!cover.item->text_texture().IsValid())
        {
          text_loader_.text = cover.item->name();
          cover.item->text_texture = text_loader_.CreateTexture();
        }

        if (cover.item->text_texture().IsValid())
        {
          ObjectPtr<IOpenGLBaseTexture> label_texture = cover.item->text_texture()->GetDeviceTexture();
          QRP_Compute_Texture_Coord(label_texture->GetWidth(), label_texture->GetHeight(), label_texture, texxform);

          ratio = label_texture->GetWidth() / (float)label_texture->GetHeight();

          //float offset = cover_bottom;
          float fx = cover_width_in_3d_space_ / 2.0f;
          float fy = (cover_width_in_3d_space_) * (1.0f / ratio);
          
          label_texture->SetFiltering(GL_LINEAR, GL_LINEAR);
          graphics_engine.SetTexture(GL_TEXTURE0, label_texture);
          CHECKGL(glUniform1iARB(TextureObjectLocation, 0));

          
          float angular_opacity = 1.0f;
          if (parent_->folding_angle == 0)
          {
            angular_opacity = 1.0f;
          }
          else
          {
            float rotation_ratio = std::abs(cover.position.rot / parent_->folding_angle);
            angular_opacity = 1.0f - rotation_ratio;
          }

          float opacity = 1.0f - std::abs(cover.position.x) / (parent_->reflection_fadeout_distance);
          if (opacity < 0.0f)
          {
            opacity = 0.0f;
          }

          //opacity *= angular_opacity;

          texxform.flip_v_coord = true;
          QRP_Compute_Texture_Coord(width, height, texture, texxform);

          float opacity_start = opacity * angular_opacity;
          float opacity_end   = opacity * angular_opacity;
          float VtxBuffer[] =
          {
            -fx, cover_bottom,    0.0f, 1.0f, texxform.u0, texxform.v1, 0, 0, opacity_start, opacity_start, opacity_start, opacity_start,
            -fx, cover_bottom-fy, 0.0f, 1.0f, texxform.u0, texxform.v0, 0, 0, opacity_end,   opacity_end,   opacity_end,   opacity_end,
            fx,  cover_bottom-fy, 0.0f, 1.0f, texxform.u1, texxform.v0, 0, 0, opacity_end,   opacity_end,   opacity_end,   opacity_end,
            fx,  cover_bottom,    0.0f, 1.0f, texxform.u1, texxform.v1, 0, 0, opacity_start, opacity_start, opacity_start, opacity_start,
          };

          CHECKGL(glEnableVertexAttribArrayARB(VertexLocation));
          CHECKGL(glVertexAttribPointerARB((GLuint) VertexLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer));

          if (TextureCoord0Location != -1)
          {
            CHECKGL(glEnableVertexAttribArrayARB(TextureCoord0Location));
            CHECKGL(glVertexAttribPointerARB((GLuint) TextureCoord0Location, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 4));
          }

          if (VertexColorLocation != -1)
          {
            CHECKGL(glEnableVertexAttribArrayARB(VertexColorLocation));
            CHECKGL(glVertexAttribPointerARB((GLuint) VertexColorLocation, 4, GL_FLOAT, GL_FALSE, 48, VtxBuffer + 8));
          }

          CHECKGL(glDrawArrays(GL_TRIANGLE_FAN, 0, 4));
        }
      }

      CHECKGL(glDisableVertexAttribArrayARB(VertexLocation));

      if (TextureCoord0Location != -1)
        CHECKGL(glDisableVertexAttribArrayARB(TextureCoord0Location));

      if (VertexColorLocation != -1)
        CHECKGL(glDisableVertexAttribArrayARB(VertexColorLocation));

      cover_shader_program_->End();
    }

    if (cover.selected)
    {
      DrawCoverHighlight(graphics_engine, cover);
    }
  }

  gboolean Coverflow::Impl::OnAnimationTimeout(gpointer data)
  {
    Coverflow::Impl* self = static_cast<Coverflow::Impl*> (data);
    self->parent_->QueueDraw();
    self->animation_handle_ = 0;
    return FALSE;
  }

  gboolean Coverflow::Impl::OnTimeout(gpointer data)
  {
    Coverflow::Impl* self = static_cast<Coverflow::Impl*>(data);
    if (self->velocity_ == 0)
    {
      self->SetPosition(RoundFloor(self->position_), true);
      self->velocity_handle_ = 0;
      return FALSE;
    }

    self->SetPosition(self->position_ + self->velocity_, false);
    self->velocity_ = (std::max(0.0f, std::abs(self->velocity_) - self->parent_->kinetic_scroll_rate)) * (self->velocity_ / std::abs(self->velocity_));
    self->MaybeQueueDraw();
    return TRUE;
  }

  NUX_IMPLEMENT_OBJECT_TYPE(Coverflow);

  Coverflow::Coverflow()
    : animation_length(250)
    , camera_motion_drift_angle(30.0f)
    , camera_motion_drift_enabled(false)
    , edge_fade(0.07)
    , flat_icons(2)
    , folding_angle(90.0f)
    , folding_depth(0.0f)
    , folding_rate(2)
    , fov(45)
    , kinetic_scroll_rate(0.1f)
    , mouse_drag_rate(1)
    , pinching(.6)
    , pop_out_selected(false)
    , reflection_fadeout_distance(4.0f)
    , reflection_strength(0.45f)
    , space_between_icons(1.25f)
    , model(CoverflowModel::Ptr(new CoverflowModel()))
    , show_covers_label(false)
    , pimpl(new Impl(this))
  {
    SetAcceptKeyboardEvent(true);

    model()->item_added.connect(sigc::mem_fun(pimpl, &Coverflow::Impl::OnItemAdded));
    model()->item_removed.connect(sigc::mem_fun(pimpl, &Coverflow::Impl::OnItemRemoved));
  }

  Coverflow::~Coverflow()
  {
    delete pimpl;
  }

  void Coverflow::SetCameraDistance(float distance)
  {
    pimpl->camera_position_.z = distance;
    pimpl->MaybeQueueDraw();
  }

  bool Coverflow::InspectKeyEvent(unsigned int eventType, unsigned int keysym, const char* character)
  {
    return true;
  }

  void Coverflow::ClientDraw(nux::GraphicsEngine& graphics_engine, nux::DrawAreaContext &ctx, bool force_draw)
  {
    gint64 current_time = g_get_monotonic_time();
    float animation_progress = std::min(1.0f, (current_time - pimpl->position_set_time_) / static_cast<float>(animation_length() * 1000));
    gint64 timestep = (current_time - pimpl->last_draw_time_) / 1000;

    nux::GetPainter().PaintBackground(graphics_engine, GetGeometry());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT  | GL_STENCIL_BUFFER_BIT);

    glViewport(0, 0, ctx.width, ctx.height);

    pimpl->perspective_.Perspective(DEGTORAD(fov()), (float)ctx.width / (float)ctx.height, pimpl->near_clip_plan_, pimpl->far_clip_plan_);

    graphics_engine.GetRenderStates().SetBlend(true);
    graphics_engine.GetRenderStates().SetPremultipliedBlend(SRC_OVER);

    CoverList covers = pimpl->GetCoverList(EaseSin(animation_progress), timestep);
    CoverList::iterator it;
    for (it = covers.begin(); it != covers.end(); ++it)
    {
      Cover cover = *it;
      if (cover.position.rot > 0)
        pimpl->DrawCover(graphics_engine, cover);
      else
        break;
    }

    CoverList::reverse_iterator rit;
    for (rit = covers.rbegin(); rit != covers.rend(); ++rit)
    {
      Cover cover = *rit;
      if (cover.position.rot <= 0)
        pimpl->DrawCover(graphics_engine, cover);
    }

    //graphics_engine.GetRenderStates().SetBlend(false);

    if (animation_progress < 1.0f && !pimpl->animation_handle_)
    {
      pimpl->animation_handle_ = g_timeout_add(0, &Coverflow::Impl::OnAnimationTimeout, pimpl);
    }

    pimpl->last_draw_time_ = current_time;
    pimpl->last_covers_ = covers;
  }

  bool Coverflow::AcceptKeyNavFocus()
  {
    return true;
  }

} // namespace nux
