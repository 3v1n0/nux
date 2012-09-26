/*
 * Copyright (C) 2012 - Canonical Ltd.
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
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 */

#include <Nux/Nux.h>
#include "TestButton.h"

#include <Nux/Layout.h>

using namespace nux;

TestButton::TestButton(const std::string& button_label, NUX_FILE_LINE_DECL)
  : nux::Button(button_label, NUX_FILE_LINE_PARAM)
{
  persistent_active_state_ = true;
}

TestButton::~TestButton()
{
}

void TestButton::Draw(GraphicsEngine &graphics_engine, bool force_draw)
{
  Geometry base = GetGeometry();

  graphics_engine.PushClippingRectangle(base);
  GetPainter().PaintBackground(graphics_engine, base);

  if (visual_state_ == VISUAL_STATE_PRESSED)
  {
    GetPainter().PaintTextureShape(graphics_engine, base, eBUTTON_FOCUS);
  }
  else if (visual_state_ == VISUAL_STATE_PRELIGHT)
  {
    GetPainter().PaintTextureShape(graphics_engine, base, eBUTTON_PRELIGHT);
  }
  else if (!active_)
  {
    GetPainter().PaintTextureShape(graphics_engine, base, eBUTTON_NORMAL);
  }

  if (GetCompositionLayout())
  {
    GetPainter().PushPaintLayerStack();
    {
      Geometry clip_geo = base;
      clip_geo.OffsetPosition(left_clip_, top_clip_);
      clip_geo.OffsetSize(-left_clip_ - right_clip_, -top_clip_ - bottom_clip_);

      graphics_engine.PushClippingRectangle(clip_geo);
      GetPainter().PushPaintLayerStack();
      GetCompositionLayout()->ProcessDraw(graphics_engine, force_draw);
      GetPainter().PopPaintLayerStack();
      graphics_engine.PopClippingRectangle();
    }
    GetPainter().PopPaintLayerStack();
  }
  graphics_engine.PopClippingRectangle();
}
