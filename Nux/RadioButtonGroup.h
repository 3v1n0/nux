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


#ifndef RADIOBUTTONLOGIC_H
#define RADIOBUTTONLOGIC_H

namespace nux
{

  class RadioButton;

  //! RadioButtonGroup select one of many radio button in a group.
  /*!
      RadioButtonGroup does not hold a strong reference on radio buttons.
  */
  class RadioButtonGroup: public InitiallyUnownedObject
  {
    NUX_DECLARE_OBJECT_TYPE(RadioButtonGroup, InitiallyUnownedObject);
  public:
    RadioButtonGroup(NUX_FILE_LINE_PROTO);
    ~RadioButtonGroup();

    //! Add a radio button to the group.
    /*
        The first radio button added to the group gets its check state set to true.
        When a radio button is added to a group that is not empty, its check state is set to false.
    */
    void ConnectButton(RadioButton* radio);

    //! Remove a radio button from the group.
    /*
        When a radio button is removed, the previous button in the group gets its check state set to true;
    */
    void DisconnectButton(RadioButton* radio);

    //! Set the active radio button.
    /*
        Set the active radio button in the group. If \i emit_signal is true, all the button in the group
        emit their \i state_changed signal.\n
        If the parameter \i radio is already active and \i emit_signal is true, the \i state_changed signal will be emitted.\n
        The active button is the last one to emit its \i state_changed signal.

        @param radio The button to activate.
        @param emit_signal If true, emit the state_changed signal of all the buttons in the group.
    */
    void SetActiveButton(RadioButton* radio, bool emit_signal = true);

  private:
    //! Return the number of buttons in the group.
    /*!
        Return the number of buttons in the group.

        @return The number of buttons in the group.
    */
    int GetNumButtons();
    void NotifyClick(RadioButton* radio);
    std::vector<ObjectWeakPtr<RadioButton> > radio_button_array_;
    int active_button_index_;

    friend class RadioButton;
  };


}

#endif // RADIOBUTTONLOGIC_H
