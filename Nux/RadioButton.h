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


#ifndef RADIO_BUTTON_H
#define RADIO_BUTTON_H

#include "AbstractButton.h"

namespace nux
{
  class HLayout;
  class InputArea;
  class StaticText;
  class RadioButtonGroup;


  class RadioButton: public AbstractButton
  {
    NUX_DECLARE_OBJECT_TYPE(RadioButton, AbstractButton);
  public:
    RadioButton(const std::string &str, bool state = false, NUX_FILE_LINE_PROTO);
    virtual ~RadioButton();

    //! Emitted when the button is clicked.
    sigc::signal<void, RadioButton*> click;

    //! Emitted when the active state changes.
    /*!
        Emitted when the active state changes, as a result of a mouse click or an API call.\n
        \sa Activate, Deactivate.
    */
    sigc::signal<void, RadioButton*> state_change;

    //! Set the label.
    /*!
        Set the label of this RadioButton. If the \a label argument is an empty string, then the the RadioButton label is destroyed,
        and the content of the RadioButton is re-arranged accordingly.

        @param label The label of the RadioButton.
    */
    void SetLabel(const std::string &checkbox_label);

    //!Return the label of this RadioButton.
    /*!
        Return the label of this RadioButton.

        @return The RadioButton label string.
    */
    std::string GetLabel() const;

    //! Activate the radio button.
    /*!
         Activate the radio button.
    */
    virtual void Activate();

    //! Deactivate the radio button.
    /*!
         Deactivate the radio button.
    */
    virtual void Deactivate();

  protected:
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);

    bool block_changed_signal_;

    HLayout   *hlayout_;
    InputArea *check_area_;

    void SetRadioGroupSelector(RadioButtonGroup *RadioSelector);
    ObjectWeakPtr<RadioButtonGroup> GetRadioGroupSelector();

    //! Intended for RadioButtonGroup only.
    void SetStatePrivate(bool State);
    //! Intended for RadioButtonGroup only.
    void SetStatePrivate(bool State, bool EmitSignal);
    ObjectWeakPtr<RadioButtonGroup> radio_button_group_;
    int radio_group_index_;

    friend class RadioButtonGroup;
  };
}

#endif // RADIO_BUTTON_H
