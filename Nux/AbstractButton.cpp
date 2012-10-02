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
 *              Gordon Allott <gord.allott@canonical.com>
 *
 */

#include "Nux.h"
#include "AbstractButton.h"
#include "StaticText.h"

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(AbstractButton);

  AbstractButton::AbstractButton(NUX_FILE_LINE_DECL)
  : View(NUX_FILE_LINE_PARAM)
  , visual_state_(VISUAL_STATE_NORMAL)
  {
    active_ = false;
    mouse_pressed_ = false;
    static_text_ = NULL;
    label_color_ = color::White;
    same_size_as_content_ = false;
    label_font_size_ = 0; // use default point size defined in nux::StaticText

    mouse_click.connect(sigc::mem_fun(this, &AbstractButton::RecvClick));
    mouse_down.connect(sigc::mem_fun(this, &AbstractButton::RecvMouseDown));
    mouse_double_click.connect(sigc::mem_fun(this, &AbstractButton::RecvMouseDown));
    mouse_up.connect(sigc::mem_fun(this, &AbstractButton::RecvMouseUp));
    mouse_move.connect(sigc::mem_fun(this, &AbstractButton::RecvMouseMove));
    mouse_enter.connect(sigc::mem_fun(this, &AbstractButton::RecvMouseEnter));
    mouse_leave.connect(sigc::mem_fun(this, &AbstractButton::RecvMouseLeave));
  }

  AbstractButton::~AbstractButton()
  {

  }

  bool AbstractButton::Active() const
  {
    return active_;
  }

  void AbstractButton::SetActive(bool active)
  {
    if (active)
      Activate();
    else
      Deactivate();
  }

  ButtonVisualState AbstractButton::GetVisualState()
  {
    return visual_state_;
  }

  void AbstractButton::RecvMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (IsMousePointerInside())
    {
      visual_state_ = VISUAL_STATE_PRELIGHT;
    }
    else
    {
      visual_state_ = VISUAL_STATE_NORMAL;
    }

    mouse_pressed_ = false;
    visual_state_change.emit(this);
    QueueDraw();
  }

  void AbstractButton::RecvMouseDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    visual_state_ = VISUAL_STATE_PRESSED;
    mouse_pressed_ = true;
    visual_state_change.emit(this);
    QueueDraw();
  }

  void AbstractButton::RecvMouseMove(int /* x */, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {

  }

  void AbstractButton::RecvMouseEnter(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (mouse_pressed_)
    {
      visual_state_ = VISUAL_STATE_PRESSED;
    }
    else
    {
      visual_state_ = VISUAL_STATE_PRELIGHT;
    }

    visual_state_change.emit(this);
    QueueDraw();
  }

  void AbstractButton::RecvMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    visual_state_ = VISUAL_STATE_NORMAL;
    visual_state_change.emit(this);
    QueueDraw();
  }

  void AbstractButton::SetLabelColor(const Color &color)
  {
    label_color_ = color;
    if (static_text_)
      static_text_->SetTextColor(label_color_);
    QueueDraw();
  }

  Color AbstractButton::GetLabelColor()
  {
    return label_color_;
  }

  ButtonVisualState AbstractButton::GetVisualState() const
  {
    return visual_state_;
  }

  void AbstractButton::SetLabelFontSize(int point)
  {
    if (point < 0)
      return;

    label_font_size_ = point;
    if (static_text_)
    {
      static_text_->SetTextPointSize(label_font_size_);
      ComputeContentSize();
      QueueDraw();
    }
  }

  int AbstractButton::GetLabelFontSize() const
  {
    return label_font_size_;
  }
}
