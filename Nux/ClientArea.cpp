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

#if defined(NUX_OS_WINDOWS)
  #include "NuxGraphics/GraphicsDisplay.h"
#elif defined(NUX_OS_LINUX)
  #include "NuxGraphics/GraphicsDisplay.h"
#endif

#include "NuxGraphics/GraphicsEngine.h"
#include "TimerProc.h"
#include "ClientArea.h"

namespace nux
{

  ClientArea::ClientArea(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
  {
    m_IsClientAreaEnabled = false;
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, 4 * PRACTICAL_WIDGET_HEIGHT);

    mouse_down.connect(sigc::mem_fun(this, &ClientArea::RecvMouseDown));
    mouse_up.connect(sigc::mem_fun(this, &ClientArea::RecvMouseUp));
    mouse_drag.connect(sigc::mem_fun(this, &ClientArea::RecvMouseDrag));
    mouse_move.connect(sigc::mem_fun(this, &ClientArea::RecvMouseMove));
    key_down.connect(sigc::mem_fun(this, &ClientArea::RecvKeyEvent));
  }

  ClientArea::~ClientArea()
  {
  }

  void ClientArea::BeginDraw(GraphicsEngine& graphics_engine, bool force_draw)
  {
    // if ((IsRedrawNeeded() == false) && (force_draw == false))
    //   return;

    // Save blend states
    unsigned int current_alpha_blend;
    unsigned int current_src_blend_factor;
    unsigned int current_dest_blend_factor;
    graphics_engine.GetRenderStates().GetBlend(current_alpha_blend, current_src_blend_factor, current_dest_blend_factor);

    ObjectPtr<IOpenGLFrameBufferObject> prev_fbo_ = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();
    Geometry prev_viewport_ = graphics_engine.GetViewportRect();

    if (GetWindowThread()->GetGraphicsDisplay().HasFrameBufferSupport())
    {
      int width = GetWidth();
      int height = GetHeight();

      m_ctx.x = GetX();
      m_ctx.y = GetY();
      m_ctx.width  = width;
      m_ctx.height = height;

      // A is obtained from graphics_engine. So A dimension's are in relative window coordinates.
      Rect A = graphics_engine.GetClippingRegion();
      Rect B = Rect(m_ctx.x, m_ctx.y, m_ctx.width, m_ctx.height);
      Rect C = A.Intersect(B);

      m_ctx.x_clipregion = C.x;
      m_ctx.y_clipregion = C.y;
      m_ctx.width_clipregion  = C.GetWidth();
      m_ctx.height_clipregion = C.GetHeight();

      //ObjectPtr<IOpenGLFrameBufferObject> prevFBO = GetGraphicsDisplay()->GetGpuDevice()->GetCurrentFrameBufferObject();

      if (m_FrameBufferObject.IsNull())
      {
        // Create the fbo before using it for the first time.
        m_FrameBufferObject = GetGraphicsDisplay()->GetGpuDevice()->CreateFrameBufferObject();
      }

      if (!m_MainColorRT.IsValid() || (m_MainColorRT->GetWidth() != width) || (m_MainColorRT->GetHeight() != height))
      {
        // Create or resize the color and depth textures before using them.
        m_MainColorRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, BITFMT_R8G8B8A8, NUX_TRACKER_LOCATION);
        m_MainDepthRT = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableDeviceTexture(width, height, 1, BITFMT_D24S8, NUX_TRACKER_LOCATION);
      }

      m_FrameBufferObject->FormatFrameBufferObject(width, height, BITFMT_R8G8B8A8);
      m_FrameBufferObject->EmptyClippingRegion();
      m_FrameBufferObject->SetTextureAttachment(0, m_MainColorRT, 0);
      m_FrameBufferObject->SetDepthTextureAttachment(m_MainDepthRT, 0);
      m_FrameBufferObject->Activate();

      graphics_engine.SetViewport(0, 0, width, height);


      ClientDraw(graphics_engine, m_ctx, force_draw);
    }

    if (prev_fbo_.IsValid())
    {
      // Restore the previous fbo
      prev_fbo_->Activate();

      prev_fbo_->ApplyClippingRegion();
    }

    // Restore the matrices and the view port.
    graphics_engine.ApplyModelViewMatrix();
    graphics_engine.SetOrthographicProjectionMatrix(prev_viewport_.width, prev_viewport_.height);
    graphics_engine.SetViewport(prev_viewport_.x, prev_viewport_.y, prev_viewport_.width, prev_viewport_.height);    

    {
      unsigned int w, h;
      w = m_MainColorRT->GetWidth();
      h = m_MainColorRT->GetHeight();
      int x = m_ctx.x;
      int y = m_ctx.y;

      TexCoordXForm texxform0;
      texxform0.uwrap = TEXWRAP_CLAMP;
      texxform0.vwrap = TEXWRAP_CLAMP;
      texxform0.FlipVCoord(true);
      GetGraphicsDisplay()->GetGraphicsEngine()->QRP_1Tex(x, y, w, h, m_MainColorRT, texxform0, Color(color::White));
    }

    // restore blend states
    graphics_engine.GetRenderStates().SetBlend(current_alpha_blend, current_src_blend_factor, current_dest_blend_factor);
  }

  void ClientArea::Draw(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {
    // don't draw here or we risk drawing more than one time.
    //BeginDraw(graphics_engine, force_draw);
  }

  void ClientArea::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    BeginDraw(graphics_engine, force_draw);

  }

  void ClientArea::ClientDraw(GraphicsEngine & /* graphics_engine */, DrawAreaContext & /* ctx */, bool /* force_draw */)
  {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void ClientArea::SetClientViewport(GraphicsEngine &graphics_engine)
  {
    if (GetWindowThread()->GetGraphicsDisplay().HasFrameBufferSupport())
    {
      graphics_engine.SetViewport(0, 0, m_FrameBufferObject->GetWidth(), m_FrameBufferObject->GetHeight());
      m_FrameBufferObject->EmptyClippingRegion();
    }
    else
    {
      int window_height = graphics_engine.GetViewportHeight();

      graphics_engine.SetViewport(
        m_ctx.x, window_height - m_ctx.y - m_ctx.height, m_ctx.width, m_ctx.height);

      graphics_engine.SetOpenGLClippingRectangle(
        m_ctx.x_clipregion,
        window_height - m_ctx.y_clipregion - m_ctx.height_clipregion,
        m_ctx.width_clipregion,
        m_ctx.height_clipregion);
    }
  }

  void ClientArea::Setup2DMode(GraphicsEngine &graphics_engine)
  {
    //Restore 2D ViewPort
    graphics_engine.SetViewport(0, 0, GetBaseWidth(), GetBaseHeight());
    graphics_engine.Push2DWindow(GetBaseWidth(), GetBaseHeight());

  }


  void ClientArea::RecvMouseDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {

  }

  void ClientArea::RecvMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {

  }

  void ClientArea::RecvMouseDrag(int /* x */, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {

  }

  void ClientArea::RecvMouseMove(int /* x */, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {

  }

  void ClientArea::RecvKeyEvent(
    unsigned long     /* event_type */,    /*event type*/
    unsigned long     /* GetKeySym */,    /*event keysym*/
    unsigned long     /* event_state */,    /*event state*/
    const char*      /* event_char */,    /*character*/
    unsigned short    /* repeat_count */     /*key repeat count*/
  )
  {

  }

  bool ClientArea::AcceptKeyNavFocus()
  {
    return false;
  }
}
