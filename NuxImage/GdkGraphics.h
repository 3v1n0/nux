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

#ifndef NUXIMAGE_GDKGRAPHICS_H
#define NUXIMAGE_GDKGRAPHICS_H

#include <gdk-pixbuf/gdk-pixbuf.h>

namespace nux
{
  class NBitmapData;
  class GdkGraphics
  {
  public:
    GdkGraphics();
    explicit GdkGraphics(const char* filename);
    // Takes ownership of the GdkPixbuf.
    explicit GdkGraphics(GdkPixbuf* pixbuf);

    ~GdkGraphics();

    bool LoadImage(const char* filename);

    NBitmapData* GetBitmap() const;

  private:
    GdkPixbuf* pixbuf_;
  };

}

#endif // NUXIMAGE_GDKGRAPHICS_H
