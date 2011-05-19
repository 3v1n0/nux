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
#include "ToggleButton.h"
#include "HLayout.h"

namespace nux
{
  ToggleButton::ToggleButton (TextureArea *image, NUX_FILE_FILE_PROTO)
  : AbstractButton (NUX_FILE_LINE_PARAM) {
    Init();
    this->image = image;
  }

  ToggleButton::ToggleButton (const std::string label, NUX_FILE_LINE_PROTO)
  : AbstractButton (NUX_FILE_LINE_PARAM) {
    Init();
    this->label = label;
  }

  ToggleButton::ToggleButton (const std::string label, BaseTexture *image, NUX_FILE_LINE_PROTO)
  : AbstractButton (NUX_FILE_LINE_PARAM) {
    Init();
    this->label = label;
    this->image = image;
  }

  ToggleButton:ToggleButton (NUX_FILE_LINE_DECL)
      : Button (Caption, NUX_FILE_LINE_PARAM) {
    togglable = true;
  }

  ToggleButton::~ToggleButton() {
  }
}
