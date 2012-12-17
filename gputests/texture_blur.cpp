/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore/NuxCore.h"
#include "NuxGraphics/BitmapFormats.h"
#include "NuxGraphics/GraphicsDisplay.h"
#include "NuxGraphics/GLWindowManager.h"
#include "NuxGraphics/GraphicsEngine.h"

/*
 * Tests:
 *  - frame buffer object
 *  - Set a texture in the fbo
 *  - Set fbo as a render target
 *  - Render Quad to fbo
 *  - Deactivate fbo
 *  - Blur the render target texture
 *  - Render to default back buffer
 */

void RenderBlurredTexture ()
{
  nux::GraphicsDisplay* graphics_display = gGLWindowManager.CreateGLWindow("Window", 600, 300, nux::WINDOWSTYLE_NORMAL, 0, false);
  nux::GraphicsEngine* graphics_engine = graphics_display->GetGraphicsEngine();

  graphics_display->ShowWindow();

  nux::ObjectPtr<nux::IOpenGLFrameBufferObject> fbo;
  nux::ObjectPtr<nux::IOpenGLBaseTexture> texture_rt;
  nux::ObjectPtr<nux::IOpenGLBaseTexture> depth_rt;

  fbo         = graphics_display->GetGpuDevice ()->CreateFrameBufferObject ();
  texture_rt  = graphics_display->GetGpuDevice ()->CreateSystemCapableDeviceTexture (graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), 1, nux::BITFMT_R8G8B8A8);
  depth_rt    = graphics_display->GetGpuDevice ()->CreateSystemCapableDeviceTexture (graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), 1, nux::BITFMT_D24S8);


  int w, h;
  graphics_engine->GetWindowSize(w, h);
  graphics_engine->SetViewport(0, 0, w, h);
  graphics_engine->SetContext(0, 0, w, h);
  graphics_engine->SetScissor(0, 0, w, h);
  graphics_engine->Push2DWindow(w, h);

  nux::Event event;
  memset(&event, 0, sizeof(nux::Event));
  
  /*char fps [25];
  int frame_counter = 0;
  int frame_periode = 0;
  float frame_rate = 0;
  float periode_time = 0;
  */
  bool first_time = true;
  do
  {
    CHECKGL( glClearColor(0, 0, 0, 1) );
    CHECKGL( glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT) );

    graphics_display->GetSystemEvent(&event);
    if(first_time || (event.type == nux::NUX_SIZE_CONFIGURATION))
    {
      first_time = false;
      graphics_engine->DisableAllTextureMode(0);
      graphics_engine->DisableAllTextureMode(1);
      graphics_engine->DisableAllTextureMode(2);
      graphics_engine->DisableAllTextureMode(3);
      graphics_engine->GetWindowSize(w, h);
      graphics_engine->SetViewport(0, 0, w, h);
      graphics_engine->SetScissor(0, 0, w, h);
      graphics_engine->SetContext(0, 0, w, h);
      graphics_engine->Push2DWindow(w, h);

      fbo         = graphics_display->GetGpuDevice()->CreateFrameBufferObject();
      texture_rt  = graphics_display->GetGpuDevice()->CreateSystemCapableDeviceTexture(graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), 1, nux::BITFMT_R8G8B8A8);
      depth_rt    = graphics_display->GetGpuDevice()->CreateSystemCapableDeviceTexture(graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), 1, nux::BITFMT_D24S8);
    }

    fbo->FormatFrameBufferObject(graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), nux::BITFMT_R8G8B8A8);
    if (texture_rt.IsValid())
    {
      fbo->SetRenderTarget(0, texture_rt->GetSurfaceLevel (0));
    }

    if (depth_rt.IsValid())
    {
      fbo->SetDepthSurface(depth_rt->GetSurfaceLevel (0));
    }
    fbo->Activate();

    graphics_engine->GetWindowSize(w, h);
    graphics_engine->SetViewport(0, 0, w, h);
    graphics_engine->SetContext(0, 0, w, h);
    graphics_engine->Push2DWindow(w, h);

    for (int i = 0; i < 1; i++)
    {
      nux::Rect geo (nux::RandomUInt(graphics_display->GetWindowWidth()),
        nux::RandomUInt(graphics_display->GetWindowHeight()),
        nux::RandomUInt(200),
        nux::RandomUInt(200));

      graphics_engine->QRP_Color(geo.x, geo.y, geo.width, geo.height, nux::color::RandomColor());
    }

    // Restore the back buffer
    graphics_display->GetGpuDevice()->DeactivateFrameBuffer();

    nux::ObjectPtr <nux::IOpenGLBaseTexture> tex_blur;

    nux::TexCoordXForm texxform;
    //Make a blurred copy of the previous render target

    if (texture_rt.IsValid() && graphics_engine)
    {
      tex_blur = graphics_engine->QRP_GLSL_GetBlurTexture(
              0, 0, texture_rt->GetWidth(), texture_rt->GetHeight(),
              texture_rt, texxform, nux::color::White, 7.0f);
    }

    graphics_engine->QRP_1Tex(0, 0, tex_blur->GetWidth(), tex_blur->GetHeight(), tex_blur, texxform, nux::color::White);

    /*
    sprintf(fps, "FPS: %3.2f", frame_rate);
    nux::PageBBox page;
    page.xmin = 0;
    page.xmax = 100;
    page.ymin = 0;
    page.ymax = 20;
    page.x_margin = 0;
    page.y_margin = 0;
    graphics_engine->RenderColorTextLineStatic(graphics_engine->GetBoldFont (), page, fps, nux::color::White, false, nux::eAlignTextLeft);
    
    float frame_time = graphics_display->GetFrameTime();
    graphics_display->ResetFrameTime();
    periode_time += frame_time;

    frame_counter++;
    frame_periode++;

    if (frame_periode >= 100)
    {
      frame_rate = frame_periode * 1000.0f / periode_time;
      periode_time = 0.0f;
      frame_periode = 0;
    }
    */
  
    graphics_display->SwapBuffer();
  } while((event.type != nux::NUX_TERMINATE_APP));

  fbo.Release ();
  texture_rt.Release ();
  depth_rt.Release ();

  delete graphics_display;
}


int main()
{
  nux::NuxCoreInitialize(0);
  nux::NuxGraphicsInitialize();

  RenderBlurredTexture ();

  return 0;
}

