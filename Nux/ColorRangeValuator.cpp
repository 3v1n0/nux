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
#include "EditTextBox.h"
#include "HLayout.h"
#include "RangeValue.h"
#include "ColorRangeValuator.h"

namespace nux
{

  ColorRangeValuator::ColorRangeValuator (float value, Color StartColor, Color EndColor)
    :   RangeValue (value, 0.0f, 1.0f)
    ,   m_StartColor (StartColor)
    ,   m_EndColor (EndColor)
  {
    InitializeLayout();
    InitializeWidgets();
  }

  ColorRangeValuator::~ColorRangeValuator()
  {
  }

  void ColorRangeValuator::InitializeWidgets()
  {

  }

  void ColorRangeValuator::InitializeLayout()
  {

  }

  Color ColorRangeValuator::GetColor() const
  {
    return m_Value * m_EndColor + (1.0f - m_Value) * m_StartColor;
  }

  void ColorRangeValuator::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GfxContext.PushClippingRectangle (base);

    Geometry P = m_Percentage->GetGeometry();

    GetPainter().Paint2DQuadColor (GfxContext, P, m_StartColor, m_StartColor, m_EndColor, m_EndColor);

    m_ValueString->NeedRedraw();
    DrawMarker (GfxContext);

    GfxContext.PopClippingRectangle();
  }


}
