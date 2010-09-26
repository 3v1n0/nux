/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef RADIOBUTTONLOGIC_H
#define RADIOBUTTONLOGIC_H

NAMESPACE_BEGIN_GUI

class RadioButton;

class RadioButtonGroup: public NuxObject, public sigc::trackable
{
public:
    RadioButtonGroup(NUX_FILE_LINE_PROTO);
    ~RadioButtonGroup();

    //! Add a radio button to the group.
    /*
        The first radio button added to the group gets its check state set to true.
        When a radio button is added to a group that is not empty, its check state is set to false.
    */
    void ConnectButton(smptr(RadioButton) radio);

    void ActivateButton(smptr(RadioButton) radio);

    //! Remove a radio button from the group.
    /*
        When a radio button is removed, the previous button in the group gets its check state set to true;
    */
    void DisconnectButton(smptr(RadioButton) radio);

    void SetActiveButton(smptr(RadioButton) radio, bool EmitSignal);

private:
    void NotifyClick(smptr(RadioButton) radio);
    std::vector<smptr(RadioButton)> m_RadioButtonArray;
    int m_ActiveRadioButtonIndex;

    friend class RadioButton;
};


NAMESPACE_END_GUI

#endif // RADIOBUTTONLOGIC_H
