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
#include "HLayout.h"
#include "RadioButton.h"
#include "StaticText.h"
#include "RadioButtonGroup.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(RadioButton);
  
  RadioButton::RadioButton (const std::string &str, bool state, NUX_FILE_LINE_DECL)
    : AbstractButton(NUX_FILE_LINE_PARAM)
  {
    label_ = str;
    active_ = state;
    hlayout_ = 0;
    block_changed_signal_ = false;
    radio_group_index_ = -1;

    static_text_  = new StaticText(label_, NUX_TRACKER_LOCATION);
    static_text_->SetTextColor(label_color_);
    hlayout_      = new HLayout(NUX_TRACKER_LOCATION);
    check_area_   = new InputArea(NUX_TRACKER_LOCATION);

    check_area_->SetSensitive(false);
    static_text_->SetSensitive(false);

    // Set Geometry
    check_area_->SetMinimumSize(14, 14);
    check_area_->SetMaximumSize(14, 14);

    hlayout_->SetHorizontalInternalMargin (4);
    hlayout_->SetContentDistribution(MAJOR_POSITION_CENTER);
    hlayout_->AddView(check_area_, 0, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);
    hlayout_->AddView(static_text_, 1, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);

//     // This is convenient to make the layout and the RadioButton fit the check area and the caption area.
//     // Since the check area is bigger than 4x4, it will force the layout and the RadioButton to grow.
//     // This is useful if the RadioButton is put in a vertical layout and it has a stretch factor of 0. Then the width of the RadioButton
//     // will be adjusted to fit the minimum width of the check area and the caption area.
//     {
//       hlayout_->SetMinimumSize (1, 1);
//       SetMinimumSize (14, 14);
//       ApplyMinWidth();
//       ApplyMinHeight();
//     }

    SetLayout (hlayout_);

  }

  RadioButton::~RadioButton()
  {
    if (radio_button_group_.IsValid() && radio_group_index_ != -1)
    {
      radio_button_group_->DisconnectButton(this);
      radio_button_group_.Release();
    }
  }

  void RadioButton::SetLabel(const std::string &checkbox_label)
  {
    label_ = checkbox_label;
    static_text_->SetText(label_);
    QueueDraw();
  }

  std::string RadioButton::GetLabel() const
  {
    return label_;
  }

  void RadioButton::Draw (GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground(graphics_engine, base);

    InteractState is;
    is.is_on = active_;

    if(visual_state_ == STATE_PRESSED)
    {
      is.is_focus = true;
    }
    else if(visual_state_ == STATE_PRELIGHT)
    {
      is.is_prelight = true;
    }
    else
    {
      is.is_focus = false;
      is.is_prelight = false;
    }

    GetPainter().PaintRadioButton(graphics_engine, check_area_->GetGeometry(), is, Color(0xff000000));

    static_text_->ProcessDraw(graphics_engine, true);
  }

  void RadioButton::RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (radio_button_group_.IsValid())
    {
      block_changed_signal_ = true;
      radio_button_group_->NotifyClick (this);
      block_changed_signal_ = false;

      clicked.emit(this);
    }
    else
    {
      active_ = !active_;
      clicked.emit(this);
    }
    QueueDraw();
  }

  void RadioButton::Activate()
  {
    if (radio_button_group_.IsValid())
    {
      radio_button_group_->SetActiveButton(this, true);
      return;
    }
    SetStatePrivate(true);
  }

  void RadioButton::Deactivate()
  {
    if (radio_button_group_.IsValid())
    {
      // The RadioButton is part of a group. To deactivate it, activate another radio button in that group.";
      return;
    }
    SetStatePrivate(false);
  }

  void RadioButton::SetRadioGroupSelector (RadioButtonGroup *RadioSelector)
  {
    if (radio_button_group_.IsValid() && radio_button_group_.GetPointer() == RadioSelector)
      return;

    if(radio_button_group_.IsValid())
    {
      radio_button_group_.Release();
    }

    if(RadioSelector)
    {
      radio_button_group_ = ObjectWeakPtr<RadioButtonGroup>(RadioSelector);
    }
  }

  ObjectWeakPtr<RadioButtonGroup> RadioButton::GetRadioGroupSelector()
  {
    return radio_button_group_;
  }

  void RadioButton::SetStatePrivate(bool state)
  {
    active_ = state;
    QueueDraw();
  }

  void RadioButton::SetStatePrivate(bool state, bool EmitSignal)
  {
    active_ = state;
    if (EmitSignal && !block_changed_signal_)
    {
      changed.emit(this);
    }
    QueueDraw();
  }
}
