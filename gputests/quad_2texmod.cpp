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
 *  - Create a check board texture
 *  - Create a second texture and fill it manually with color values
 *  - Render full window background quad
 *  - enable blending
 *  - Render the two textures with QRP_2TexMod: result = (Tex0*Color0) * (Tex1*color1)
 *  - Disable blending
 */

void QuadRendering_2TexMod ()
{
  nux::GraphicsDisplay* graphics_display = gGLWindowManager.CreateGLWindow("QuadRendering: 2TexMod", 570, 270, nux::WINDOWSTYLE_NORMAL, 0, false);
  nux::GraphicsEngine* graphics_engine = graphics_display->GetGraphicsEngine();

  graphics_display->ShowWindow();

  int size = 256;

  nux::NTextureData checkboard_texture_data;
  MakeCheckBoardImage (checkboard_texture_data.GetSurface (0), size, size, nux::Color (0xff000000), nux::color::LightSeaGreen, 4, 4);
  nux::BaseTexture* checkboard_texture = graphics_display->GetGpuDevice ()->CreateSystemCapableTexture ();
  checkboard_texture->Update (&checkboard_texture_data);


  // Create a Texture map of size 256x1
  nux::NTextureData texture_data (nux::BITFMT_R8G8B8A8, size, 1, 1);
  // Get the first surface (mip 0) of the texture map
  nux::ImageSurface surface = texture_data.GetSurface (0);

  nux::ObjectPtr<nux::IOpenGLTexture2D> gradient_texture = graphics_display->GetGpuDevice ()->CreateSystemCapableDeviceTexture (
    texture_data.GetWidth (),
    texture_data.GetHeight (),
    1,
    texture_data.GetFormat ());


  nux::SURFACE_LOCKED_RECT lockrect;
  gradient_texture->LockRect (0, &lockrect, 0);

  BYTE *dest       = (BYTE *) lockrect.pBits;
  int  num_row     = surface.GetBlockHeight ();

  // Fill the gradient texture with White and an alpha value going from 1.0f to 0.0f
  for (int y = 0; y < num_row; y++)
  {
    // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
    // they contain the same amount of valid data since they have the same width, height and format.
    for (int x = 0; x < texture_data.GetWidth (); x++)
    {
      *(dest + y * lockrect.Pitch + 4*x + 0) = 0xff; //red
      *(dest + y * lockrect.Pitch + 4*x + 1) = 0xff; //green
      *(dest + y * lockrect.Pitch + 4*x + 2) = 0xff; //blue
      *(dest + y * lockrect.Pitch + 4*x + 3) = 255 - 255 * ((float)x/(float)texture_data.GetWidth ());
    }
  }

  gradient_texture->UnlockRect (0);


  int w, h;
  nux::Event event;
  memset(&event, 0, sizeof(nux::Event));
  graphics_engine->GetWindowSize(w, h);
  graphics_engine->SetViewport(0, 0, w, h);
  graphics_engine->SetScissor(0, 0, w, h);
  graphics_engine->SetContext(0, 0, w, h);
  graphics_engine->Push2DWindow(w, h);

  do
  {
    CHECKGL( glClearColor(0, 0, 0, 1) );
    CHECKGL( glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT) );

    graphics_display->GetSystemEvent(&event);
    if(event.type == nux::NUX_SIZE_CONFIGURATION)
    {
      graphics_engine->GetWindowSize(w, h);
      graphics_engine->SetViewport(0, 0, w, h);
      graphics_engine->SetScissor(0, 0, w, h);
      graphics_engine->SetContext(0, 0, w, h);
      graphics_engine->Push2DWindow(w, h);
    }

    nux::Rect geo (
      ((int)graphics_display->GetWindowWidth() - size) /2,
      ((int)graphics_display->GetWindowHeight() - size) /2,
      size,
      size);

    // Paint a rectangular quad over the entire window
    graphics_engine->QRP_Color(0, 0, graphics_display->GetWindowWidth(), graphics_display->GetWindowHeight(), nux::color::Aubergine);

    // Enable blending
    graphics_display->GetGraphicsEngine()->GetRenderStates ().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    nux::TexCoordXForm texxform0;
    nux::TexCoordXForm texxform1;

    // Modulate the checkboard and the gradient texture
    graphics_engine->QRP_2TexMod(geo.x, geo.y, geo.width, geo.height,
      gradient_texture, texxform0, nux::color::White,
      checkboard_texture->GetDeviceTexture (), texxform1, nux::color::White);

    graphics_display->GetGraphicsEngine()->GetRenderStates ().SetBlend(false);

    graphics_display->SwapBuffer();
  } while(event.type != nux::NUX_TERMINATE_APP);

  gradient_texture.Release ();
  checkboard_texture->UnReference ();

  delete graphics_display;
}


int main()

{
  nux::NuxCoreInitialize(0);
  nux::NuxGraphicsInitialize();

  QuadRendering_2TexMod ();

  return 0;
}

