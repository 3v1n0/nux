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

#include "AbstractCheckedButton.h"

namespace nux
{
  class HLayout;
  class InputArea;
  class StaticText;
  class RadioButtonGroup;

  //! RadioButton class
  /*!
      A RadioButton class. Work with the RadioButtonGroup to allow "one to many" selection.\n
      The RadioButton class cannot be vertically resized. It can only be resized horizontally.
      The vertical size is always match the size of the content (check area + label).
  */
  class RadioButton: public AbstractCheckedButton
  {
    NUX_DECLARE_OBJECT_TYPE(RadioButton, AbstractCheckedButton);
  public:
    RadioButton(const std::string& str, bool state = false, NUX_FILE_LINE_PROTO);
    virtual ~RadioButton();

    //! Emitted when the button is clicked.
    sigc::signal<void, RadioButton*> click;

    //! Emitted when the active state changes.
    /*!
        Emitted when the active state changes, as a result of a mouse click or an API call.\n
        \sa Activate, Deactivate.
    */
    sigc::signal<void, RadioButton*> state_change;

    //! Activate the radio button.
    /*!
         Activate the radio button. If this radio button is part of a RadioButtonGroup, it will emit the signal state_change.
    */
    virtual void Activate();

    //! Deactivate the radio button.
    /*!
         Deactivate the radio button.
    */
    virtual void Deactivate();

  protected:
    virtual void Draw(GraphicsEngine& graphics_engine, bool force_draw);
    virtual void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);

    bool block_changed_signal_;

    void SetRadioButtonGroup(RadioButtonGroup* radio_button_group);
    ObjectWeakPtr<RadioButtonGroup> GetRadioButtonGroup();

    //! Intended for RadioButtonGroup only.
    void SetStatePrivate(bool state);
    //! Intended for RadioButtonGroup only.
    void SetStatePrivate(bool state, bool emit_signal);
    ObjectWeakPtr<RadioButtonGroup> radio_button_group_;
    int radio_group_index_;

  private:
    //! Override of Area::SetMinimumHeight and made private.
    /*!
        Prevent changing the minimum height of the StaticText view.
    */
    virtual void SetMinimumHeight(int h){};

    //! Override of Area::SetMaximumHeight and made private.
    /*!
        Prevent changing the maximum height of the StaticText view.
    */
    virtual void SetMaximumHeight(int h){};

    friend class RadioButtonGroup;
  };
}

#endif // RADIO_BUTTON_H
