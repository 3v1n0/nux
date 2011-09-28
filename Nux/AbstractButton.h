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

#ifndef ABSTRACTBUTTON_H
#define ABSTRACTBUTTON_H

#include "View.h"
#include "NuxCore/Property.h"

namespace nux
{
  enum ButtonVisualState
  {
    STATE_PRESSED = 0, //STATE_ACTIVE = 0,
    STATE_NORMAL = 1,
    STATE_PRELIGHT = 2,
    STATE_SELECTED = 3,
    STATE_INSENSITIVE = 4
  };

  //! The base class of Button widgets.
  class AbstractButton : public View
  {
    NUX_DECLARE_OBJECT_TYPE(AbstractButton, View);
  public:


    AbstractButton(NUX_FILE_LINE_PROTO);
    ~AbstractButton();

    //! Signal emitted when the button is activated.
    Property<bool> active;

    //! Signal emitted when the button is activated.
    sigc::signal<void, AbstractButton*> activated;
    sigc::signal<void, AbstractButton*> changed_visual_state;

    ButtonVisualState GetVisualState();
    bool IsActive();

  protected:
    bool persistent_active_state_;
    bool active_;
    ButtonVisualState visual_state_;

    void RecvMouseUp    (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown  (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseMove  (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvClick      (int x, int y, unsigned long button_flags, unsigned long key_flags);
  };
}

#endif // ABSTRACTBUTTON_H
