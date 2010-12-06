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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "NuxImage/ImageSurface.h"
#include "ColorArea.h"

namespace nux
{

  ColorArea::ColorArea (Color color, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_Color (color)
  {
    SetMinMaxSize (50, 50);
  }

  ColorArea::~ColorArea()
  {
  }

  long ColorArea::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return TraverseInfo;
  }

  void ColorArea::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    GetPainter().Paint2DQuadColor (GfxContext, GetGeometry(), m_Color);
  }

  void ColorArea::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void ColorArea::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void ColorArea::SetColor (Color color)
  {
    m_Color = color;
    NeedRedraw();
  }


}
