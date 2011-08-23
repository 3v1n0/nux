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
#include "NuxImage/BitmapFormats.h"
#include "NuxGraphics/GraphicsDisplay.h"
#include "NuxGraphics/GLWindowManager.h"
#include "NuxGraphics/GraphicsEngine.h"

/*
 * Tests: 
 *  - frame buffer object
 *  - Set a texture in the fbo
 *  - Set fbo as a render target
 *  - Render Quad to fbo
 *  - Use fbo texture to draw a full screen quad on the default render target
 */
 
void RenderToFrameBufferObject ()
{
  nux::GraphicsDisplay* graphics_display = gGLWindowManager.CreateGLWindow("Window", 570, 270, nux::WINDOWSTYLE_NORMAL, 0, false);
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
  graphics_engine->Push2DWindow(w, h);

  nux::IEvent event;
  memset(&event, 0, sizeof(nux::IEvent));

  do
  {
    CHECKGL( glClearColor(0, 0, 0, 1) );
    CHECKGL( glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT) );

    graphics_display->GetSystemEvent(&event);
    if(event.e_event == nux::NUX_SIZE_CONFIGURATION)
    {
      graphics_engine->DisableAllTextureMode(0);
      graphics_engine->DisableAllTextureMode(1);
      graphics_engine->DisableAllTextureMode(2);
      graphics_engine->DisableAllTextureMode(3);
      graphics_engine->GetWindowSize(w, h);
      graphics_engine->SetViewport(0, 0, w, h);
      graphics_engine->SetScissor(0, 0, w, h);
      graphics_engine->SetContext(0, 0, w, h);
      graphics_engine->Push2DWindow(w, h);

      fbo         = graphics_display->GetGpuDevice ()->CreateFrameBufferObject ();
      texture_rt  = graphics_display->GetGpuDevice ()->CreateSystemCapableDeviceTexture (graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), 1, nux::BITFMT_R8G8B8A8);
      depth_rt    = graphics_display->GetGpuDevice ()->CreateSystemCapableDeviceTexture (graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), 1, nux::BITFMT_D24S8);
    }

    fbo->FormatFrameBufferObject (graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), nux::BITFMT_R8G8B8A8);
    fbo->SetRenderTarget (0, texture_rt->GetSurfaceLevel (0));
    fbo->SetDepthSurface (depth_rt->GetSurfaceLevel (0));
    fbo->Activate();

    graphics_engine->GetWindowSize(w, h);
    graphics_engine->SetViewport(0, 0, w, h);
    graphics_engine->SetContext(0, 0, w, h);
    graphics_engine->Push2DWindow(w, h);

    nux::Rect geo (nux::RandomUInt(graphics_display->GetWindowWidth()),
      nux::RandomUInt(graphics_display->GetWindowHeight()),
      nux::RandomUInt(200),
      nux::RandomUInt(200));

    graphics_engine->QRP_Color(geo.x, geo.y, geo.width, geo.height, nux::color::RandomColor());

    graphics_display->GetGpuDevice ()->DeactivateFrameBuffer ();

    nux::TexCoordXForm texxform;
    graphics_engine->QRP_1Tex(0, 0, graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), texture_rt, texxform, nux::color::White);

    graphics_display->SwapBuffer();
  } while(event.e_event != nux::NUX_TERMINATE_APP);

  fbo.Release ();
  texture_rt.Release ();
  depth_rt.Release ();

  delete graphics_display;
}

int main(int argc, char **argv)
{
  nux::NuxCoreInitialize(0);
  nux::NuxGraphicsInitialize();

  RenderToFrameBufferObject ();

  return 0;
}

