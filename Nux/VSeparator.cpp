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

#include "AbstractSeparator.h"
#include "VSeparator.h"

namespace nux
{

  VSeparator::VSeparator()
//:   AbstractSeparator(0xFF999999, 0.0f, 151.0f, 10)
  {
    InitializeWidgets();
    InitializeLayout();
  }

  VSeparator::VSeparator (const Color &color, float Alpha0, float Alpha1, int Border)
    :   AbstractSeparator (color, Alpha0, Alpha1, Border)
  {
    InitializeWidgets();
    InitializeLayout();
  }

  VSeparator::~VSeparator()
  {

  }

  void VSeparator::InitializeWidgets()
  {
    SetMinimumWidth (3);
    SetMaximumWidth (3);
  }

  void VSeparator::InitializeLayout()
  {

  }

  void VSeparator::DestroyLayout()
  {

  }

  long VSeparator::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    return TraverseInfo;
  }

  void VSeparator::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    int x0 = base.x + base.GetWidth() / 2;
    base.OffsetPosition (0, 3);
    base.OffsetSize (0, -6);
    GetThreadGraphicsContext()->GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (base.GetHeight() - 2 * m_BorderSize > 0)
    {
      // TODO: Tim Penhey 2011-05-13
      // This code is a copy of some other code somewhere... we should make sure there
      // is only one implementation.
      Color color0 = m_Color;
      Color color1 = m_Color;
      color0.alpha = m_Alpha0;
      color1.alpha = m_Alpha1;
      GetPainter().Draw2DLine (GfxContext, x0, base.y, x0, base.y + m_BorderSize, color0, color1);
      GetPainter().Draw2DLine (GfxContext, x0, base.y + m_BorderSize, x0, base.y + base.GetHeight() - m_BorderSize, color1, color1);
      GetPainter().Draw2DLine (GfxContext, x0, base.y + base.GetHeight() - m_BorderSize, x0, base.y + base.GetHeight(), color1, color0);
    }
    else
    {
      Color color1 = m_Color;
      color1.alpha = m_Alpha1;
      GetPainter().Draw2DLine (GfxContext, x0, base.y, x0, base.y + base.GetHeight(), color1);
    }

    GetThreadGraphicsContext()->GetRenderStates().SetBlend (FALSE);
  }
}
