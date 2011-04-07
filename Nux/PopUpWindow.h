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


#ifndef POPUPWINDOW_H
#define POPUPWINDOW_H

#include "FloatingWindow.h"

namespace nux
{

  class Layout;

// This class implements a window without a taskbar. It can contain any can of layout.
// The purpose is to implement a window that pop ups with complexe widget inside.
// The basic popup widget only displays text options.
  class PopUpWindow : public FloatingWindow
  {
  public:
    PopUpWindow();
    ~PopUpWindow();

    void Show();
    void Hide();
  private:
  };


}

#endif // POPUPWINDOW_H

