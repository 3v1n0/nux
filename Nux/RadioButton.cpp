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
  
  RadioButton::RadioButton(const std::string& str, bool state, NUX_FILE_LINE_DECL)
  : AbstractCheckedButton(str, state, NUX_FILE_LINE_PARAM)
  {
    block_changed_signal_ = false;
    radio_group_index_ = -1;
  }

  RadioButton::~RadioButton()
  {
    if (radio_button_group_.IsValid() && radio_group_index_ != -1)
    {
      radio_button_group_->DisconnectButton(this);

      if (radio_button_group_->GetNumButtons() == 0)
      {
        // The last button in the group calls UnReference on the RadioButtonGroup.
        radio_button_group_->UnReference();
      }
    }
  }

  void RadioButton::Draw(GraphicsEngine& graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    graphics_engine.PushClippingRectangle(base);

    InteractState is;
    is.is_on = active_;

    if (visual_state_ == VISUAL_STATE_PRESSED)
    {
      is.is_focus = true;
    }
    else if (visual_state_ == VISUAL_STATE_PRELIGHT)
    {
      is.is_prelight = true;
    }
    else
    {
      is.is_focus = false;
      is.is_prelight = false;
    }

    GetPainter().PushPaintLayerStack();
    {
      GetPainter().PaintRadioButton(graphics_engine, check_area_->GetGeometry(), is, Color(0xff000000));
      static_text_->ProcessDraw(graphics_engine, true);
    }
    GetPainter().PopPaintLayerStack();
  
    graphics_engine.PopClippingRectangle();
  }

  void RadioButton::RecvClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (radio_button_group_.IsValid())
    {
      block_changed_signal_ = true;
      radio_button_group_->NotifyClick(this);
      block_changed_signal_ = false;

      state_change.emit(this);
      click.emit(this);
    }
    else
    {
      active_ = !active_;
      click.emit(this);
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

  void RadioButton::SetRadioButtonGroup(RadioButtonGroup* radio_button_group)
  {
    if (radio_button_group_.IsValid() && radio_button_group_.GetPointer() == radio_button_group)
      return;

    if (radio_button_group_.IsValid())
    {
      radio_button_group_.Release();
    }

    if (radio_button_group)
    {
      radio_button_group_ = ObjectWeakPtr<RadioButtonGroup>(radio_button_group);
    }
  }

  ObjectWeakPtr<RadioButtonGroup> RadioButton::GetRadioButtonGroup()
  {
    return radio_button_group_;
  }

  void RadioButton::SetStatePrivate(bool state)
  {
    active_ = state;
    QueueDraw();
  }

  void RadioButton::SetStatePrivate(bool state, bool emit_signal)
  {
    active_ = state;
    if (emit_signal && !block_changed_signal_)
    {
      state_change.emit(this);
    }
    QueueDraw();
  }
}
