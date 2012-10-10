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
  
  CheckBox::CheckBox(const std::string &str, bool state, NUX_FILE_LINE_DECL)
    : AbstractCheckedButton(str, state, NUX_FILE_LINE_PARAM)
  {
  }

  CheckBox::~CheckBox()
  {
  }

  void CheckBox::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
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
      GetPainter().PaintCheckBox(graphics_engine, check_area_->GetGeometry(), is, Color(0xff000000));
      static_text_->ProcessDraw(graphics_engine, true);
    }
    GetPainter().PopPaintLayerStack();

    graphics_engine.PopClippingRectangle();
  }

  void CheckBox::RecvClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
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
