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

#include <cairo.h>

/*
 * Tests: 
 *  - load 2d textures of various size from the hard drive
 *  - textures have power of two size 256x256, 128x128, ...., 2x2, 1x1
 *  - manually load bitmap data into device texture: IOpenGLTexture2D
 *  - Lock/Unlock device texture data pointer
 *  - Use immediate mode rendering with glBegin, glEnd
 */

void RenderTexturePowerOfTwo ()
{
  nux::GraphicsDisplay* m_GLWindow = gGLWindowManager.CreateGLWindow("Window", 570, 270, nux::WINDOWSTYLE_NORMAL, 0, false);
  nux::GraphicsEngine* m_GraphicsContext = m_GLWindow->GetGraphicsEngine();

  m_GLWindow->ShowWindow();

  const TCHAR* texture_list [] =
  {
    TEXT("./data/mipmap256x256.png"),
    TEXT("./data/mipmap128x128.png"),
    TEXT("./data/mipmap64x64.png"),
    TEXT("./data/mipmap32x32.png"),
    TEXT("./data/mipmap16x16.png"),
    TEXT("./data/mipmap8x8.png"),
    TEXT("./data/mipmap4x4.png"),
    TEXT("./data/mipmap2x2.png"),
    TEXT("./data/mipmap1x1.png")
  };

  nux::ObjectPtr<nux::IOpenGLTexture2D> tex [9];

  for (int i = 0; i < 9; i++)
  {
    nux::NBitmapData *bitmap = nux::LoadImageFile (texture_list[i]);
    nux::ImageSurface surface = bitmap->GetSurface (0);

    surface.GetFormat ();

    tex[i] = nux::GetGraphicsDisplay()->GetGpuDevice()->CreateTexture (
      surface.GetWidth(),
      surface.GetHeight (),
      1,
      surface.GetFormat ());

    nux::SURFACE_LOCKED_RECT lockrect;
    tex[i]->LockRect(0, &lockrect, 0);

    BYTE	           *dest       = (BYTE *) lockrect.pBits;
    const BYTE       *src        = surface.GetPtrRawData();
    int              RowByteSize = surface.GetPitch();
    int              num_row     = surface.GetBlockHeight();

    for (int Y = 0; Y < num_row; Y++ )
    {
      // Take Min(RowByteSize, StrideY): the source and the destination may not have the same Pitch but
      // they contain the same amount of valid data since they have the same width, height and format.
      nux::Memcpy (dest + Y * lockrect.Pitch, &src[Y * RowByteSize], nux::Min (RowByteSize, lockrect.Pitch) );
    }
    tex[i]->UnlockRect (0);
  }

  int w, h;
  m_GraphicsContext->GetWindowSize(w, h);
  m_GraphicsContext->SetViewport(0, 0, w, h);
  m_GraphicsContext->SetContext(0, 0, w, h);
  m_GraphicsContext->Push2DWindow(w, h);

  nux::Event event;
  memset(&event, 0, sizeof(nux::Event));

  
  {
    CHECKGL( glClearColor(0, 0, 0, 1) );
    CHECKGL( glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT) );

    m_GLWindow->GetSystemEvent(&event);
    if(event.type == nux::NUX_SIZE_CONFIGURATION)
    {
      m_GraphicsContext->DisableAllTextureMode(0);
      m_GraphicsContext->DisableAllTextureMode(1);
      m_GraphicsContext->DisableAllTextureMode(2);
      m_GraphicsContext->DisableAllTextureMode(3);
      m_GraphicsContext->GetWindowSize(w, h);
      m_GraphicsContext->SetViewport(0, 0, w, h);
      m_GraphicsContext->SetScissor(0, 0, w, h);
      m_GraphicsContext->SetContext(0, 0, w, h);
      m_GraphicsContext->Push2DWindow(w, h);
    }

    int level = 0;
    int width = 0;
    int height = 0;
    int format = 0;

    unsigned char* data = (unsigned char*) tex [6]->GetSurfaceData (level, width, height, format);
    
    cairo_surface_t *surface;

    surface = cairo_image_surface_create_for_data (data, CAIRO_FORMAT_ARGB32, width, height, width*4);
    cairo_surface_write_to_png (surface, "tmp.png");

    cairo_surface_destroy (surface);

    nuxDebugMsg (TEXT("size: %dx%d"), width, height);
    delete data;
  }

  for (int i = 0; i < 9; i++)
  {
    tex[i].Release ();
  }
  delete m_GLWindow;
}


int main()

{
  nux::NuxCoreInitialize(0);
  nux::NuxGraphicsInitialize();

  RenderTexturePowerOfTwo ();

  return 0;
}


