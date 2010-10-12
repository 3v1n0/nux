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


#include "Nux.h"
#include "RadioButton.h"
#include "RadioButtonGroup.h"

namespace nux
{

  RadioButtonGroup::RadioButtonGroup (NUX_FILE_LINE_DECL)
    :   Object (false, NUX_FILE_LINE_PARAM)  // RadioButtonGroup is created as unowned
  {
    m_ActiveRadioButtonIndex = 0;
  }

  RadioButtonGroup::~RadioButtonGroup()
  {
    std::vector<RadioButton *>::iterator it;

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ( (*it) )
      {
        (*it)->m_Group = 0;
        (*it)->m_GroupId = -1;
      }
    }
  }


  void RadioButtonGroup::ConnectButton (RadioButton *radio)
  {
    std::vector<RadioButton *>::iterator it;

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ( (*it) && ( (*it) == radio) )
      {
        // already in
        return;
      }
    }

    t_u32 index = (t_u32) m_RadioButtonArray.size();

    if (index == 0)
    {
      // Inserting the first radio button
      if (radio->m_Group)
      {
        // Disconnect from the other selector
        radio->m_Group->DisconnectButton (radio);
      }

      radio->SetRadioGroupSelector (this);
      radio->m_GroupId = 0;
      radio->SetStatePrivate (true);
      m_RadioButtonArray.push_back (radio);
      m_ActiveRadioButtonIndex = 0;
    }
    else
    {
      if (radio->m_Group)
      {
        // Disconnect from the other selector
        radio->m_Group->DisconnectButton (radio);
      }

      radio->SetRadioGroupSelector (this);
      radio->m_GroupId = index;
      // When a radio button is added to a group that is not empty, its check state is set to false.
      radio->SetStatePrivate (false);
      m_RadioButtonArray.push_back (radio);
    }
  }

  void RadioButtonGroup::ActivateButton (RadioButton *radio)
  {
    std::vector<RadioButton *>::iterator it = find (m_RadioButtonArray.begin(), m_RadioButtonArray.end(), radio);

    if (it == m_RadioButtonArray.end() )
      return;

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ( (*it) && ( (*it) != radio) )
      {
        (*it)->SetStatePrivate (false, true);
      }
    }

    radio->SetStatePrivate (true, true);
  }

  void RadioButtonGroup::DisconnectButton (RadioButton *radio)
  {
    bool found = false;
    t_u32 array_size = (t_u32) m_RadioButtonArray.size();
    std::vector<RadioButton *>::iterator it;
    it = m_RadioButtonArray.begin();
    t_s32 i;

    for (i = 0; i < (t_s32) array_size; i++, it++)
    {
      if (m_RadioButtonArray[i] == radio)
      {
        radio->m_Group = 0;
        radio->m_GroupId = -1;
        radio->SetStatePrivate (false);
        m_RadioButtonArray.erase (it);
        found = true;
        break;
      }
    }

    if (found && (i - 1 > 0) )
    {
      // The previous button becomes active
      m_RadioButtonArray[i]->SetStatePrivate (true);
    }
  }

  void RadioButtonGroup::NotifyClick (RadioButton *radio)
  {
    std::vector<RadioButton *>::iterator it;

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ( (*it) != radio)
      {
        if ( (*it) )
        {
          (*it)->SetStatePrivate (false, true);
        }
      }
      else
      {
        if ( (*it) )
        {
          (*it)->SetStatePrivate (true,  true);
        }
      }
    }
  }

  void RadioButtonGroup::SetActiveButton (RadioButton *radio, bool EmitSignal)
  {
    std::vector<RadioButton *>::iterator it = find (m_RadioButtonArray.begin(), m_RadioButtonArray.end(), radio);

    if (it == m_RadioButtonArray.end() )
      return;

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ( (*it) != radio)
      {
        if ( (*it) )
        {
          (*it)->SetStatePrivate (false, EmitSignal);
        }
      }
      else
      {
        if ( (*it) )
        {
          (*it)->SetStatePrivate (true,  EmitSignal);
        }
      }
    }
  }

}
