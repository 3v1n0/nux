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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */

#ifndef XINPUTWINDOW_H
#define XINPUTWINDOW_H

/* Xlib.h is the default header that is included and has the core functionallity */
#include <X11/Xlib.h>
/* Xatom.h includes functionallity for creating new protocol messages */
#include <X11/Xatom.h>
/* keysym.h contains keysymbols which we use to resolv what keys that are being pressed */
#include <X11/keysym.h>

#include <X11/extensions/xf86vmode.h>

#include "NuxCore/NKernel.h"
#include "NuxCore/Rect.h"

namespace nux
{

  class XInputWindow
  {
  public:
    XInputWindow();
    ~XInputWindow();
    
    //! Set the position and size of the window
    void SetGeometry(const Rect& geo);

    //! Set the position and size of the window
    void SetGeometry(int x, int y, int width, int height);
    
    //! Get the window geometry.
    Rect GetGeometry() const;
    
    
  private:
  
    Window _xwindow;
    int _x;
    int _y;
    int _width;
    int _height;
    
  };
}

#endif // XINPUTWINDOW_H

