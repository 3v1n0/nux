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


#ifndef GDKGRAPHICS_H
#define GDKGRAPHICS_H

namespace nux
{
  class GdkGraphics
  {
  public:
    GdkGraphics ();
    GdkGraphics (const TCHAR* Filename);
    GdkGraphics (void* GdkPixbufPtr);
    ~GdkGraphics ();

    bool LoadImage (const TCHAR* Filename);
    void LoadGdkPixbuf (void* GdkPixbufPtr);

    NBitmapData *GetBitmap();
  private:

  void* _gdkpixbuf_ptr;

  };

}

#endif // GDKGRAPHICS_H
