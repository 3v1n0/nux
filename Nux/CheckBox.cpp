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
#include "CheckBox.h"
#include "StaticText.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(CheckBox);
  
  CheckBox::CheckBox (const std::string &str, bool state, NUX_FILE_LINE_DECL)
    : AbstractButton(NUX_FILE_LINE_PARAM)
  {
    label_ = str;
    active_ = state;
    hlayout_ = 0;

    static_text_  = new StaticText(label_, NUX_TRACKER_LOCATION);
    static_text_->SetTextColor(label_color_);
    hlayout_      = new HLayout(NUX_TRACKER_LOCATION);
    check_area_   = new InputArea(NUX_TRACKER_LOCATION);

    check_area_->SetInputEventSensitivity(false);
    static_text_->SetInputEventSensitivity(false);

    // Set Geometry
    check_area_->SetMinimumSize(14, 14);
    check_area_->SetMaximumSize(14, 14);

    hlayout_->SetHorizontalInternalMargin (4);
    hlayout_->SetContentDistribution(MAJOR_POSITION_CENTER);
    hlayout_->AddView(check_area_, 0, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);
    hlayout_->AddView(static_text_, 1, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);

//     // This is convenient to make the layout and the CheckBox fit the check area and the caption area.
//     // Since the check area is bigger than 4x4, it will force the layout and the CheckBox to grow.
//     // This is useful if the CheckBox is put in a vertical layout and it has a stretch factor of 0. Then the width of the CheckBox
//     // will be adjusted to fit the minimum width of the check area and the caption area.
//     {
//       hlayout_->SetMinimumSize (1, 1);
//       SetMinimumSize (14, 14);
//       ApplyMinWidth();
//       ApplyMinHeight();
//     }

    SetLayout (hlayout_);

  }

  CheckBox::~CheckBox()
  {
  }

  void CheckBox::SetLabel(const std::string &checkbox_label)
  {
    label_ = checkbox_label;
    static_text_->SetText(label_);
    QueueDraw();
  }

  std::string CheckBox::GetLabel() const
  {
    return label_;
  }

  void CheckBox::Draw (GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground(graphics_engine, base);

    InteractState is;
    is.is_on = active_;

    if(visual_state_ == VISUAL_STATE_PRESSED)
    {
      is.is_focus = true;
    }
    else if(visual_state_ == VISUAL_STATE_PRELIGHT)
    {
      is.is_prelight = true;
    }
    else
    {
      is.is_focus = false;
      is.is_prelight = false;
    }

    GetPainter().PaintCheckBox (graphics_engine, check_area_->GetGeometry(), is, Color (0xff000000) );

    static_text_->ProcessDraw(graphics_engine, true);
  }

  void CheckBox::RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    active_ = !active_;

    click.emit(this);
    state_change.emit(this);
    QueueDraw();
  }

  void CheckBox::Activate()
  {
    if (active_ == true)
    {
      // already active
      return;
    }

    active_ = true;
    state_change.emit(this);
    QueueDraw();
  }

  void CheckBox::Deactivate()
  {
    if (active_ == false)
      return;

    if (active_ == false)
    {
      // already deactivated
      return;
    }

    active_ = false;
    state_change.emit(this);
    QueueDraw();
  }
}
