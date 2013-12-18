/*
* Copyright 2012-2013 Inalogic® Inc.
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

#ifndef XIMCONTROLLER_H
#define XIMCONTROLLER_H

#include "XICClient.h"

/* Xlib.h is the default header that is included and has the core functionallity */
#include <X11/Xlib.h>

namespace nux
{
  class TextEntry;

class XIMController
{
public:
  XIMController(Display* display);
  ~XIMController();

  void SetFocusedWindow(Window window);
  void SetCurrentTextEntry(TextEntry* text_entry_);
  void RemoveFocusedWindow();

  bool IsXICValid() const;
  XIC GetXIC() const;

  void FocusInXIC();
  void FocusOutXIC();

  Window GetCurrentWindow() const;

private:
  void InitXIMCallback();
  static void SetupXIMClientCallback(Display* dpy, XPointer client_data, XPointer call_data);
  static void EndXIMClientCallback(Display* dpy, XPointer client_data, XPointer call_data);

  void SetupXIM();
  void SetupXIMDestroyedCallback();

  Display* display_;
  Window window_;
  XIM xim_;
  XICClient xic_client_;
};

} //namespace nux

#endif // XIMController.h
