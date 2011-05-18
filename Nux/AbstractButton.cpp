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
 *              Gordon Allott <gord.allott@canonical.com>
 *
 */


#include "Nux.h"

#include "AbstractButton.h"
#include "HLayout.h"

namespace nux
{

  AbstractButton::AbstractButton(NUX_FILE_LINE_PROTO)
      : View (NUX_FILE_LINE_PARAM) {
    Init();
  }

  AbstractButton::~AbstractButton()
      : View (NUX_FILE_LINE_PARAM) {
    Init();
  }

  AbstractButton::Init () {
    EnableDoubleClick (false);
  }

  void AbstractButton::RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    Activated.emit (this);
    NeedRedraw();
  }

  void AbstractButton::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags) {
    NeedRedraw();
  }

  void AbstractButton::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags) {
    NeedRedraw();
  }

  void AbstractButton::RecvMouseMove (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags) {

  }

  void AbstractButton::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags) {
    NeedRedraw();
  }

  void AbstractButton::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags) {
    NeedRedraw();
  }
}
