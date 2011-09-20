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
      RadioButtonGroup does not hold reference on radio buttons.
  */
  class RadioButtonGroup: public InitiallyUnownedObject
  {
    NUX_DECLARE_OBJECT_TYPE (RadioButtonGroup, InitiallyUnownedObject);
  public:
    RadioButtonGroup (NUX_FILE_LINE_PROTO);
    ~RadioButtonGroup();

    //! Add a radio button to the group.
    /*
        The first radio button added to the group gets its check state set to true.
        When a radio button is added to a group that is not empty, its check state is set to false.
    */
    void ConnectButton (RadioButton *radio);

    void ActivateButton (RadioButton *radio);

    //! Remove a radio button from the group.
    /*
        When a radio button is removed, the previous button in the group gets its check state set to true;
    */
    void DisconnectButton (RadioButton *radio);

    void SetActiveButton (RadioButton *radio, bool EmitSignal);

  private:
    void NotifyClick (RadioButton *radio);
    std::vector<ObjectWeakPtr<RadioButton> > m_RadioButtonArray;
    int m_ActiveRadioButtonIndex;

    friend class RadioButton;
  };


}

#endif // RADIOBUTTONLOGIC_H
