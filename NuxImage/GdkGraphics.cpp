/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "NuxCore/NuxCore.h"
#include "ImageSurface.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "GdkGraphics.h"

namespace nux
{
  GdkGraphics::GdkGraphics ()
  {
    _gdkpixbuf_ptr = 0;
  }
  
  GdkGraphics::GdkGraphics (TCHAR* Filename)
  {
    _gdkpixbuf_ptr = 0;
  }

  GdkGraphics::GdkGraphics (void* GdkPixbufPtr)
  {
    _gdkpixbuf_ptr = 0;
    LoadGdkPixbuf (GdkPixbufPtr);
  }

  GdkGraphics::~GdkGraphics ()
  {

  }

  bool GdkGraphics::LoadImage (TCHAR* Filename)
  {
    if (_gdkpixbuf_ptr)
    {
      g_object_unref (_gdkpixbuf_ptr);
      _gdkpixbuf_ptr = 0;
    }

    GError *error = 0;
    _gdkpixbuf_ptr = gdk_pixbuf_new_from_file (Filename, &error);

    if (_gdkpixbuf_ptr == 0)
    {
      if (error->domain == G_FILE_ERROR)
      {
        nuxDebugMsg (TEXT ("[GdkGraphics::GdkGraphics] File error: %d: %s"), error->code, error->message);
      }
      else if (error->domain == GDK_PIXBUF_ERROR)
      {
        nuxDebugMsg (TEXT ("[GdkGraphics::GdkGraphics] GDK Pixbuf error: %d: %s"), error->code, error->message);
      }
      else
      {
        nuxDebugMsg (TEXT ("[GdkGraphics::GdkGraphics] Unknown error"));
      }
      return false;
    }
    return true;
  }

  void GdkGraphics::LoadGdkPixbuf (void* GdkPixbufPtr)
  {
    if (GdkPixbufPtr == 0)
    {
      g_object_unref (_gdkpixbuf_ptr);
      _gdkpixbuf_ptr = 0;
      return;
    }

    _gdkpixbuf_ptr = GdkPixbufPtr;
  }

  NBitmapData* GdkGraphics::GetBitmap()
  {
    unsigned int width = gdk_pixbuf_get_width (NUX_STATIC_CAST (GdkPixbuf *, _gdkpixbuf_ptr));
    unsigned int height = gdk_pixbuf_get_height (NUX_STATIC_CAST (GdkPixbuf *, _gdkpixbuf_ptr));
    unsigned int row_bytes = gdk_pixbuf_get_rowstride (NUX_STATIC_CAST (GdkPixbuf *, _gdkpixbuf_ptr));

    int channels = gdk_pixbuf_get_n_channels (NUX_STATIC_CAST (GdkPixbuf *, _gdkpixbuf_ptr));
    int src_pitch = gdk_pixbuf_get_rowstride (NUX_STATIC_CAST (GdkPixbuf *, _gdkpixbuf_ptr));

    NTextureData *Texture = NULL;

    if (channels == 4)
    {      
      Texture = new NTextureData (BITFMT_R8G8B8A8, width, height, 1);
    }
    else if (channels == 3)
    {
      Texture = new NTextureData (BITFMT_R8G8B8, width, height, 1);
    }
    else if (channels == 1)
    {
      Texture = new NTextureData (BITFMT_A8, width, height, 1);
    }
    else
    {
      nuxDebugMsg (TEXT("[GdkGraphics::GetBitmap] Invalid number of channels."));
      return 0;
    }

    ImageSurface &image_surface = Texture->GetSurface (0);
    t_u8* dest = image_surface.GetPtrRawData ();
    int dest_pitch = image_surface.GetPitch ();

    t_u8* dst = dest;
    guchar *src = gdk_pixbuf_get_pixels (NUX_STATIC_CAST (GdkPixbuf *, _gdkpixbuf_ptr));

    for (unsigned int i = 0; i < height; i++)
    {
      Memcpy (dst, src + i*src_pitch, width*channels);
      dst += dest_pitch;
    }
    return Texture;
  }
}

