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


#include "Nux/Nux.h"
#include "line-separator.h"

namespace nux
{
  HSeparator::HSeparator()
  {
    SetMinimumHeight (1);
    SetMaximumHeight (1);
  }

  HSeparator::HSeparator (const Color &color, float Alpha0, float Alpha1, int Border)
    : AbstractSeparator (color, Alpha0, Alpha1, Border)
  {
    SetMinimumHeight (1);
    SetMaximumHeight (1);
  }

  HSeparator::~HSeparator()
  {
  }

  void HSeparator::Draw (GraphicsEngine &GfxContext, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    base.OffsetPosition(3, 0);
    base.OffsetSize(-6, 0);
    int y0 = base.y + base.GetHeight() / 2;

    GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (base.GetWidth() - 2 * m_BorderSize > 0)
    {
      Color color0 = m_Color;
      Color color1 = m_Color;
      color0.alpha = m_Alpha0;
      color1.alpha = m_Alpha1;
      GetPainter().Draw2DLine (GfxContext, base.x, y0, base.x + m_BorderSize, y0, color0, color1);
      GetPainter().Draw2DLine (GfxContext, base.x + m_BorderSize, y0, base.x + base.GetWidth() - m_BorderSize, y0, color1, color1);
      GetPainter().Draw2DLine (GfxContext, base.x + base.GetWidth() - m_BorderSize, y0, base.x + base.GetWidth(), y0, color1, color0);
    }
    else
    {
      Color color1 = m_Color;
      color1.alpha = m_Alpha1;
      GetPainter().Draw2DLine (GfxContext, base.x, y0, base.x + base.GetWidth(), y0, color1, color1);
    }

    GetGraphicsDisplay()->GetGraphicsEngine()->GetRenderStates().SetBlend (FALSE);
  }
}
