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

#include "Nux.h"
#include "RadioButton.h"
#include "RadioButtonGroup.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(RadioButtonGroup);

  RadioButtonGroup::RadioButtonGroup(NUX_FILE_LINE_DECL)
    : InitiallyUnownedObject(NUX_FILE_LINE_PARAM)  // RadioButtonGroup is created as unowned
  {
    m_ActiveRadioButtonIndex = 0;
  }

  RadioButtonGroup::~RadioButtonGroup()
  {
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it;

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ((*it).IsValid())
      {
        (*it)->SetRadioGroupSelector(NULL);
        (*it)->radio_group_index_ = -1;
      }
    }
  }


  void RadioButtonGroup::ConnectButton(RadioButton *radio)
  {
    NUX_RETURN_IF_NULL(radio);

    if (!radio->Type().IsDerivedFromType(RadioButton::StaticObjectType))
    {
      return;
    }

    std::vector<ObjectWeakPtr<RadioButton> >::iterator it;

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ((*it).IsValid() && ((*it).GetPointer() == radio))
      {
        // already in
        return;
      }
    }

    size_t index = (unsigned int) m_RadioButtonArray.size();

    if (index == 0)
    {
      // Inserting the first radio button
      if (radio->GetRadioGroupSelector().IsValid())
      {
        // Disconnect from the other selector
        radio->GetRadioGroupSelector()->DisconnectButton(radio);
      }

      radio->SetRadioGroupSelector(this);
      radio->radio_group_index_ = 0;
      radio->SetStatePrivate(true);
      m_RadioButtonArray.push_back(ObjectWeakPtr<RadioButton>(radio));
      m_ActiveRadioButtonIndex = 0;
    }
    else
    {
      if (radio->GetRadioGroupSelector().IsValid())
      {
        // Disconnect from the other selector
        radio->GetRadioGroupSelector()->DisconnectButton(radio);
      }

      radio->SetRadioGroupSelector(this);
      radio->radio_group_index_ = index;
      // When a radio button is added to a group that is not empty, its check state is set to false.
      radio->SetStatePrivate(false);
      m_RadioButtonArray.push_back(ObjectWeakPtr<RadioButton>(radio));
    }
  }

  void RadioButtonGroup::ActivateButton(RadioButton *radio)
  {
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it = find(m_RadioButtonArray.begin(), m_RadioButtonArray.end(), radio);

    if (it == m_RadioButtonArray.end())
      return;

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ((*it).IsValid() && ((*it).GetPointer() != radio))
      {
        (*it)->SetStatePrivate(false, true);
      }
    }

    radio->SetStatePrivate(true, true);
  }

  void RadioButtonGroup::DisconnectButton(RadioButton *radio)
  {
    bool found = false;
    size_t array_size = (unsigned int) m_RadioButtonArray.size();
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it;
    it = m_RadioButtonArray.begin();
    size_t i;

    for (i = 0; i < array_size; i++, it++)
    {
      if (m_RadioButtonArray[i] == radio)
      {
        radio->radio_group_index_ = -1;
        radio->SetStatePrivate(false);
        m_RadioButtonArray.erase(it);
        found = true;
        break;
      }
    }

    if (found && (i - 1 > 0) && (m_RadioButtonArray.size() >0))
    {
      // The previous button becomes active
      m_RadioButtonArray[i]->SetStatePrivate(true);
    }
  }

  void RadioButtonGroup::NotifyClick(RadioButton *radio)
  {
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it;

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ((*it).GetPointer() != radio)
      {
        if ((*it).IsValid())
        {
          (*it)->SetStatePrivate(false, true);
        }
      }
      else
      {
        if ((*it).IsValid())
        {
          (*it)->SetStatePrivate(true,  true);
        }
      }
    }
  }

  void RadioButtonGroup::SetActiveButton(RadioButton *radio, bool EmitSignal)
  {
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it = find(m_RadioButtonArray.begin(), m_RadioButtonArray.end(), radio);

    if (it == m_RadioButtonArray.end())
      return;

    if ((*it).IsValid() == false)
    {
      m_RadioButtonArray.erase(it);
      return;
    }

    if ((*it)->Active() && (EmitSignal == false))
    {
      // The radio is already active.
      return;
    }

    for (it = m_RadioButtonArray.begin(); it != m_RadioButtonArray.end(); it++)
    {
      if ((*it) != radio)
      {
        if ((*it).IsValid())
        {
          (*it)->SetStatePrivate(false, EmitSignal);
        }
      }
      else
      {
        if ((*it).IsValid())
        {
          (*it)->SetStatePrivate(true,  EmitSignal);
        }
      }
    }
  }

}
