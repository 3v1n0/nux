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
  : InitiallyUnownedObject(NUX_FILE_LINE_PARAM)  // RadioButtonGroup is created as un-owned
  , active_button_index_(0)
  {
  }

  RadioButtonGroup::~RadioButtonGroup()
  {
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it;

    for (it = radio_button_array_.begin(); it != radio_button_array_.end(); it++)
    {
      if ((*it).IsValid())
      {
        (*it)->SetRadioButtonGroup(NULL);
        (*it)->radio_group_index_ = -1;
      }
    }
  }


  void RadioButtonGroup::ConnectButton(RadioButton* radio)
  {
    if (radio == NULL)
    {
      return;
    }

    if (!radio->Type().IsDerivedFromType(RadioButton::StaticObjectType))
    {
      return;
    }

    std::vector<ObjectWeakPtr<RadioButton> >::iterator it;

    for (it = radio_button_array_.begin(); it != radio_button_array_.end(); it++)
    {
      if ((*it).IsValid() && ((*it).GetPointer() == radio))
      {
        // already in
        return;
      }
    }

    size_t index = (unsigned int) radio_button_array_.size();

    if (index == 0)
    {
      // Inserting the first radio button
      if (radio->GetRadioButtonGroup().IsValid())
      {
        // Disconnect from the other selector
        radio->GetRadioButtonGroup()->DisconnectButton(radio);
      }

      radio->SetRadioButtonGroup(this);
      radio->radio_group_index_ = 0;
      radio->SetStatePrivate(true);
      radio_button_array_.push_back(ObjectWeakPtr<RadioButton>(radio));
      active_button_index_ = 0;
    }
    else
    {
      if (radio->GetRadioButtonGroup().IsValid())
      {
        // Disconnect from the other selector
        radio->GetRadioButtonGroup()->DisconnectButton(radio);
      }

      radio->SetRadioButtonGroup(this);
      radio->radio_group_index_ = index;
      // When a radio button is added to a group that is not empty, its check state is set to false.
      radio->SetStatePrivate(false);
      radio_button_array_.push_back(ObjectWeakPtr<RadioButton>(radio));
    }
  }

  void RadioButtonGroup::DisconnectButton(RadioButton* radio)
  {
    bool found = false;
    size_t array_size = (unsigned int) radio_button_array_.size();
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it;
    it = radio_button_array_.begin();
    size_t i;

    for (i = 0; i < array_size; i++, it++)
    {
      if (radio_button_array_[i] == radio)
      {
        radio->radio_group_index_ = -1;
        radio->SetStatePrivate(false);
        radio_button_array_.erase(it);
        found = true;
        break;
      }
    }

    if (found && (i - 1 > 0) && (!radio_button_array_.empty()))
    {
      // The previous button becomes active
      radio_button_array_[i]->SetStatePrivate(true);
    }
  }

  int RadioButtonGroup::GetNumButtons()
  {
    int count = 0;
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it;
    for (it = radio_button_array_.begin(); it != radio_button_array_.end(); ++it)
    {
      if ((*it).IsValid())
      {
        ++count;
      }
    }
    return count;
  }

  void RadioButtonGroup::NotifyClick(RadioButton* radio)
  {
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it;

    for (it = radio_button_array_.begin(); it != radio_button_array_.end(); ++it)
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

  void RadioButtonGroup::SetActiveButton(RadioButton* radio, bool emit_signal)
  {
    std::vector<ObjectWeakPtr<RadioButton> >::iterator it = find(radio_button_array_.begin(), radio_button_array_.end(), radio);

    if (it == radio_button_array_.end())
      return;

    if ((*it).IsValid() == false)
    {
      // The button has been destroyed.
      // Remove it from the array
      radio_button_array_.erase(it);
      return;
    }

    for (it = radio_button_array_.begin(); it != radio_button_array_.end(); ++it)
    {
      if ((*it).IsValid() && ((*it).GetPointer() != radio))
      {
        (*it)->SetStatePrivate(false, emit_signal);
      }
    }

    radio->SetStatePrivate(true, emit_signal);
  }
}
