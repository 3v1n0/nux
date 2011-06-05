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

  AbstractButton::AbstractButton(NUX_FILE_LINE_DECL)
      : View(NUX_FILE_LINE_PARAM)
      , togglable(this, "togglable")
      , active(this, "active") {
    Init();
  }

  AbstractButton::~AbstractButton() {
  }

  void AbstractButton::Init () {
    EnableDoubleClick (false);
  }

  void AbstractButton::RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (togglable) {
      active = !active;
    } else {
      active = true;
    }

    Activated.emit (this);

    if (togglable == false)
      active = false;

    NeedRedraw();

  }

  long AbstractButton::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return PostProcessEvent2 (ievent, TraverseInfo, ProcessEventInfo);
  }

  void AbstractButton::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags) {
    state = NUX_STATE_PRELIGHT;
    //state = 1;
    NeedRedraw();
  }

  void AbstractButton::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags) {
    state = NUX_STATE_ACTIVE;
    //state = 0;
    NeedRedraw();
  }

  void AbstractButton::RecvMouseMove (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags) {

  }

  void AbstractButton::RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags) {
    state = NUX_STATE_PRELIGHT;
    //state = 1;
    NeedRedraw();
  }

  void AbstractButton::RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags) {
    state = NUX_STATE_NORMAL;
    //state = 3;
    NeedRedraw();
  }
}
