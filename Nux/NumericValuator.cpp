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
#include "EditTextBox.h"
#include "DoubleValidator.h"
#include "NumericValuator.h"

#include <string>

namespace nux
{

  const int BTN_WIDTH = 14;
  const int BTN_HEIGHT = 14;

  NumericValuator::NumericValuator()
    :   m_DoubleValidator(0.0, 100.0)
    ,   m_Step(0.1f)
  {
    InitializeLayout();
    InitializeWidgets();
  }

  NumericValuator::~NumericValuator()
  {
  }

  void NumericValuator::InitializeWidgets()
  {
    m_EditLine->SetValidator(&m_DoubleValidator);
    m_EditLine->SetText(std::to_string((long double)m_DoubleValidator.GetMinimum()));

    m_EditLine->SetMinimumSize(2 * DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    m_EditLine->SetGeometry(Geometry(0, 0, 2 * DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT));

    m_SpinnerDownBtn->SetMinimumSize(BTN_WIDTH, BTN_HEIGHT);
    m_SpinnerDownBtn->SetGeometry(Geometry(0, 0, BTN_WIDTH, BTN_HEIGHT));
    m_SpinnerUpBtn->SetMinimumSize(BTN_WIDTH, BTN_HEIGHT);
    m_SpinnerUpBtn->SetGeometry(Geometry(0, 0, BTN_WIDTH, BTN_HEIGHT));

    hlayout->AddView(m_SpinnerDownBtn, 0);
    hlayout->AddView(m_EditLine, 1);
    hlayout->AddView(m_SpinnerUpBtn, 0);
    hlayout->SetContentDistribution(eStackLeft);

    SetCompositionLayout(hlayout);
  }

  void NumericValuator::InitializeLayout()
  {
    hlayout = new HLayout(NUX_TRACKER_LOCATION);
  }

  void NumericValuator::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    GeometryPositioning gp(eHALeft, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning(m_SpinnerUpBtn->GetGeometry(), GetTheme().GetImageGeometry(eTRIANGLE_RIGHT), gp);
    GetPainter().PaintShape(graphics_engine, GeoPo, Color(0xFFFFFFFF), eTRIANGLE_RIGHT);

    GeoPo = ComputeGeometryPositioning(m_SpinnerDownBtn->GetGeometry(), GetTheme().GetImageGeometry(eTRIANGLE_LEFT), gp);
    GetPainter().PaintShape(graphics_engine, GeoPo, Color(0xFFFFFFFF), eTRIANGLE_LEFT);

    m_EditLine->QueueDraw();
  }

  void NumericValuator::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    m_EditLine->ProcessDraw(graphics_engine, force_draw);
  }

  void NumericValuator::SetValue(float value)
  {
    m_fValue = value;

    if (m_fValue < m_DoubleValidator.GetMinimum())
      m_fValue = m_DoubleValidator.GetMinimum();

    if (m_fValue > m_DoubleValidator.GetMaximum())
      m_fValue = m_DoubleValidator.GetMaximum();

    m_EditLine->SetText(std::to_string((long double)m_fValue));
  }

  float NumericValuator::GetValue() const
  {
    return m_fValue;
  }

  void NumericValuator::SetStep(float f)
  {
    m_Step = f;
  }

  float NumericValuator::GetStep()
  {
    return m_Step;
  }


  void NumericValuator::ImplementIncrementBtn()
  {
    SetValue(m_fValue + m_Step);
    sigIncrement.emit();
    sigValueChanged.emit(m_fValue);

    if (m_fValue < m_DoubleValidator.GetMaximum())
    {
      m_UpTimerHandler = GetTimer().AddOneShotTimer(100, m_UpTimerCallback, 0);
      QueueDraw();
    }
  }

  void NumericValuator::ImplementDecrementBtn()
  {
    SetValue(m_fValue - m_Step);
    sigDecrement.emit();
    sigValueChanged.emit(m_fValue);

    if (m_fValue > m_DoubleValidator.GetMinimum())
    {
      m_DownTimerHandler = GetTimer().AddOneShotTimer(100, m_DownTimerCallback, 0);
      QueueDraw();
    }
  }

  void NumericValuator::ImplementValidateEntry()
  {
    double ret = 0;
    ret = CharToDouble(m_EditLine->GetCleanText().c_str());
    {
      m_fValue = ret;

      if (m_fValue < m_DoubleValidator.GetMinimum())
      {
        m_fValue = m_DoubleValidator.GetMinimum();
        m_EditLine->SetText(std::to_string((long double)m_fValue));
      }

      if (m_fValue > m_DoubleValidator.GetMaximum())
      {
        m_fValue = m_DoubleValidator.GetMaximum();
        m_EditLine->SetText(std::to_string((long double)m_fValue));
      }
    }
//     else
//     {
//         m_EditLine->SetText(std::string::Printf("%f", m_fValue));
//     }
  }

}
