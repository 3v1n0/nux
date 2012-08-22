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

#ifndef XICCLIENT_H
#define XICCLIENT_H

/* Xlib.h is the default header that is included and has the core functionallity */
#include <X11/Xlib.h>

class XICClient
{
public:
  XICClient(Window window);
  ~XICClient();

  XIC& GetXIC();

  void ResetXIC(XIM xim, XIMStyle style);
  void FocusInXIC();
  void FocusOutXIC();
  bool IsFocused() const;

  void Destroy();
private:
  void SetupXIC(XIM xim, XIMStyle style);

  Window window_;
  XIC xic_;
  bool focused_;
};

#endif // XICClient.h
