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
    ~CheckBox();

    sigc::signal<void, CheckBox *> sigStateToggled;
    sigc::signal<void> sigToggled;
    sigc::signal<void, bool> sigStateChanged;

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

    void SetState (bool State);
    void SetState (bool State, bool EmitSignal);
    bool GetState() const;

  protected:
    virtual void Draw (GraphicsEngine &graphics_engine, bool force_draw);

    std::string label_;
    StaticText *static_text_;

    HLayout   *hlayout_;
    InputArea *text_area_;
    InputArea *check_area_;
    bool      state_;
  };

}

#endif // CHECKBOX_H
