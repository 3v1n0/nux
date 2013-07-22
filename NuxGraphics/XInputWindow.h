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
#include <X11/Xregion.h>

#include <X11/extensions/xf86vmode.h>

#include "NuxCore/NuxCore.h"
#include "NuxCore/Rect.h"


namespace nux
{

  class XInputWindow
  {
  public:
    XInputWindow(const char* title,
                 bool        take_focus = False,
                 int         override_redirect = 0);
    ~XInputWindow();

    static std::vector<Window> const& NativeHandleList();

    void EnableStruts(bool enable);
    bool StrutsEnabled();

    void EnableOverlayStruts(bool enable);
    bool OverlayStrutsEnabled();

    void EnsureInputs();

    void EnableTakeFocus ();

    //! Set the position and size of the window
    void SetGeometry(const Rect& geo);

    //! Set the position and size of the window
    void SetGeometry(int x, int y, int width, int height);

    //! Get the window geometry.
    Rect const& GetGeometry() const;

    //! Get X11 the Window.
    Window GetWindow ();

    void SetInputFocus ();

    void Hide ();
    void Show ();

  private:

    std::vector<long int> GetStrutsData();

    void SetStruts ();
    void UnsetStruts ();

    void SetOverlayStruts ();
    void UnsetOverlayStruts ();

    void EnableDnd ();
    void DisableDnd ();

    static std::vector<Window> native_windows_;

    bool strutsEnabled_;
    bool overlayStrutsEnabled_;
    Window window_;
    Display *display_;
    Rect geometry_;
    bool shown_;
    bool mapped_;
  };
}

#endif // XINPUTWINDOW_H
