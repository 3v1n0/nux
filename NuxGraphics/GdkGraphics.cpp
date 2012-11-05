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


#include "NuxCore/NuxCore.h"
#include "ImageSurface.h"
#include "GdkGraphics.h"

#include "NuxCore/Logger.h"

namespace nux
{
DECLARE_LOGGER(logger, "nux.image");

  GdkGraphics::GdkGraphics()
    : pixbuf_(0)
  {
  }

  GdkGraphics::GdkGraphics(GdkPixbuf* pixbuf)
    : pixbuf_(pixbuf)
  {
  }

  GdkGraphics::GdkGraphics(const char* filename)
    : pixbuf_(0)
  {
    LoadImage(filename);
  }

  GdkGraphics::~GdkGraphics()
  {
    if (pixbuf_)
      g_object_unref(pixbuf_);
  }

  bool GdkGraphics::LoadImage(const char* filename)
  {
    if (pixbuf_)
      g_object_unref(pixbuf_);

    GError* error = 0;
    pixbuf_ = gdk_pixbuf_new_from_file(filename, &error);

    if (error)
    {
      LOG_ERROR(logger) << error->message;
      g_error_free(error);
      return false;
    }

    return true;
  }

  NBitmapData* GdkGraphics::GetBitmap() const
  {
    if (!pixbuf_)
    {
      LOG_WARN(logger) << "No pixbuf loaded";
      return 0;
    }

    unsigned int width = gdk_pixbuf_get_width(pixbuf_);
    unsigned int height = gdk_pixbuf_get_height(pixbuf_);
    int channels = gdk_pixbuf_get_n_channels(pixbuf_);
    int src_pitch = gdk_pixbuf_get_rowstride(pixbuf_);

    NTextureData* texture = NULL;

    if (channels == 4)
    {
      texture = new NTextureData(BITFMT_R8G8B8A8, width, height, 1);
    }
    else if (channels == 3)
    {
      texture = new NTextureData(BITFMT_R8G8B8, width, height, 1);
    }
    else if (channels == 1)
    {
      texture = new NTextureData(BITFMT_A8, width, height, 1);
    }
    else
    {
      LOG_ERROR(logger) << __func__ << ": Invalid number of channels";
      return 0;
    }

    ImageSurface& image_surface = texture->GetSurface(0);
    unsigned char* dest = image_surface.GetPtrRawData();
    int dest_pitch = image_surface.GetPitch();

    guchar* src = gdk_pixbuf_get_pixels(pixbuf_);

    for (unsigned int i = 0; i < height; ++i)
    {
      Memcpy(dest, src + i*src_pitch, width*channels);
      dest += dest_pitch;
    }
    return texture;
  }
}

