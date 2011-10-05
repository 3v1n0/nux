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


#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "AbstractButton.h"

namespace nux
{
  class HLayout;
  class InputArea;
  class StaticText;

  class CheckBox: public AbstractButton
  {
    NUX_DECLARE_OBJECT_TYPE(CheckBox, AbstractButton);
  public:
    CheckBox (const std::string &str, bool state = false, NUX_FILE_LINE_PROTO);
    virtual ~CheckBox();

    //! Emitted when the button is clicked.
    sigc::signal<void, CheckBox*> click;

    //! Emitted when the active state changes.
    /*!
        Emitted when the active state changes, as a result of a mouse click or an API call.\n
        \sa Activate, Deactivate.
    */
    sigc::signal<void, CheckBox*> state_change;

    //! Set the label.
    /*!
        Set the label of this CheckBox. If the \a label argument is an empty string, then the the CheckBox label is destroyed,
        and the content of the CheckBox is re-arranged accordingly.

        @param label The label of the CheckBox.
    */
    void SetLabel(const std::string &checkbox_label);

    //!Return the label of this CheckBox.
    /*!
        Return the label of this CheckBox.

        @return The CheckBox label string.
    */
    std::string GetLabel() const;

    //! Activate the check box.
    /*!
         Activate the check box.
    */
    virtual void Activate();

    //! Deactivate the check box.
    /*!
         Deactivate the check box.
    */
    virtual void Deactivate();

  protected:
    virtual void Draw (GraphicsEngine &graphics_engine, bool force_draw);
    virtual void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);

    HLayout   *hlayout_;
    InputArea *check_area_;
  };

}

#endif // CHECKBOX_H
