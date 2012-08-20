/*
* Copyright 2010-2012 Inalogic® Inc.
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
* Authored by: Brandon Schaefer <brandon.schaefer@canonical.com>
*
*/

#ifndef XIMCLIENT_H
#define XIMCLIENT_H

/* Xlib.h is the default header that is included and has the core functionallity */
#include <X11/Xlib.h>
/* Xatom.h includes functionallity for creating new protocol messages */
#include <X11/Xatom.h>
/* keysym.h contains keysymbols which we use to resolv what keys that are being pressed */
#include <X11/keysym.h>


#include <X11/extensions/xf86vmode.h>
#include <X11/extensions/Xinerama.h>

class XIMClient
{
public:
  XIMClient(Display* display, Window window);

  XIC& GetXIC();

  void FocusInXIC();
  void FocusOutXIC();
  bool IsFocused() const;
private:

  void SetupCallback();
  static void SetupXIMClientCallback(Display *dpy, XPointer client_data, XPointer call_data);

  void SetupXIMClient();
  void SetupXIM();
  bool CheckRootStyleSupport() const;
  void SetupXIC();

  Display *display_;
  Window window_;

  XIM xim_;
  XIC xic_;
  XIMStyle default_style_;

  bool focused_;
};

#endif // XIMClient.h
