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
  
  RadioButton::RadioButton(const std::string &str, bool state, NUX_FILE_LINE_DECL)
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

    check_area_->SetInputEventSensitivity(false);
    static_text_->SetInputEventSensitivity(false);

    // Set Geometry
    check_area_->SetMinimumSize(14, 14);
    check_area_->SetMaximumSize(14, 14);

    hlayout_->SetHorizontalInternalMargin(4);
    hlayout_->SetContentDistribution(MAJOR_POSITION_LEFT);
    hlayout_->AddView(check_area_, 0, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);
    hlayout_->AddView(static_text_, 1, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);

//     // This is convenient to make the layout and the RadioButton fit the check area and the caption area.
//     // Since the check area is bigger than 4x4, it will force the layout and the RadioButton to grow.
//     // This is useful if the RadioButton is put in a vertical layout and it has a stretch factor of 0. Then the width of the RadioButton
//     // will be adjusted to fit the minimum width of the check area and the caption area.
//     {
//       hlayout_->SetMinimumSize(1, 1);
//       SetMinimumSize(14, 14);
//       ApplyMinWidth();
//       ApplyMinHeight();
//     }

    hlayout_->SetScaleFactor(0);
    SetLayout(hlayout_);

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

  void RadioButton::Draw(GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();
    graphics_engine.PushClippingRectangle(base);

    GetPainter().PaintBackground(graphics_engine, base);

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

  long RadioButton::ComputeContentSize()
  {
    if (view_layout_)
    {
      PreLayoutManagement();

      int old_width = GetBaseWidth();
      int old_height = GetBaseHeight();

      // Let the text view be as large as possible.
      static_text_->SetMaximumWidth(AREA_MAX_WIDTH);

      // Constrain the vertical expansion of the color selector.
      view_layout_->SetBaseHeight(1);
      long ret = view_layout_->ComputeContentSize();

      PostLayoutManagement(ret);

      {
        // Check if the text view goes out of the CheckBox area.
        Geometry base = GetGeometry();
        Geometry text_geo = static_text_->GetGeometry();

        // Intersect the CheckBox and the text view
        Geometry intersection = base.Intersect(text_geo);
        if (intersection != text_geo)
        {
          // The text view goes outside of the CheckBox area. We have to clip it
          static_text_->SetMaximumWidth(intersection.width);

          // Assign a size of 1 to the layout and call ComputeContentSize.
          // Inside the StaticText::ComputeContentSize there is code that takes care of size negociation.
          view_layout_->SetBaseWidth(1);
          ret = view_layout_->ComputeContentSize();

          // Assign the layout geometry to the CheckBox view.
          PostLayoutManagement(ret);
        }
      }

      int new_width = GetBaseWidth();
      int new_height = GetBaseHeight();

      long size_compliance = 0;

      // The layout has been resized to tightly pack its content
      if (new_width > old_width)
      {
        size_compliance |= eLargerWidth; // need scrollbar
      }
      else if (new_width < old_width)
      {
        size_compliance |= eSmallerWidth;
      }
      else
      {
        size_compliance |= eCompliantWidth;
      }

      // The layout has been resized to tightly pack its content
      if (new_height > old_height)
      {
        size_compliance |= eLargerHeight; // need scrollbar
      }
      else if (new_height < old_height)
      {
        size_compliance |= eSmallerHeight;
      }
      else
      {
        size_compliance |= eCompliantHeight;
      }

      return size_compliance;
    }
    else
    {
      PreLayoutManagement();
      int ret = PostLayoutManagement(eCompliantHeight | eCompliantWidth);
      return ret;
    }

    return 0;
  }


  void RadioButton::RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (radio_button_group_.IsValid())
    {
      block_changed_signal_ = true;
      radio_button_group_->NotifyClick(this);
      block_changed_signal_ = false;

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

  void RadioButton::SetRadioGroupSelector(RadioButtonGroup *RadioSelector)
  {
    if (radio_button_group_.IsValid() && radio_button_group_.GetPointer() == RadioSelector)
      return;

    if (radio_button_group_.IsValid())
    {
      radio_button_group_.Release();
    }

    if (RadioSelector)
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
      state_change.emit(this);
    }
    QueueDraw();
  }
}
