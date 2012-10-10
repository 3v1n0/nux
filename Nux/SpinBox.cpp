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
#include "TimerProc.h"
#include "HLayout.h"
#include "VLayout.h"
#include "IntegerValidator.h"
#include "SpinBox.h"

namespace nux
{

  const Color SPINBOX_BUTTON_COLOR = Color(0xFF4D4D4D);
  const Color SPINBOX_BUTTON_MOUSEOVER_COLOR = Color(0xFF222222);

  SpinBox::SpinBox(int Value, int Step, int MinValue, int MaxValue, NUX_FILE_LINE_DECL)
    : SpinBox_Logic(NUX_FILE_LINE_PARAM)
    , m_IntValidator(MinValue, MaxValue)
    , m_Step(Step)
  {
    hlayout = new HLayout(NUX_TRACKER_LOCATION);
    vlayout = new VLayout(NUX_TRACKER_LOCATION);

    m_EditLine->SetValidator(&m_IntValidator);
    m_EditLine->SetSuffix("");
    m_EditLine->SetPrefix("");
    m_EditLine->SetText(std::to_string((long long)m_IntValidator.GetMinimum()));

    m_EditLine->SetMinimumSize(1.5 * DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    m_EditLine->SetGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    m_SpinnerUpBtn->SetMinimumSize(15, 10);
    m_SpinnerUpBtn->SetGeometry(Geometry(0, 0, 15, 10));
    m_SpinnerDownBtn->SetMinimumSize(15, 10);
    m_SpinnerDownBtn->SetGeometry(Geometry(0, 0, 15, 10));

    // Set the minimum size of this widget.
    // This is use by TextLineEditPropertyItem::GetItemBestHeight
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    hlayout->AddView(m_EditLine, 1);

    vlayout->AddView(m_SpinnerUpBtn, 1);
    vlayout->AddView(m_SpinnerDownBtn, 1);
    hlayout->AddLayout(vlayout, 0);

    SetLayout(hlayout);

    SetValue(Value);
  }

  SpinBox::~SpinBox()
  {
  }

  void SpinBox::InitializeWidgets()
  {
  }

  void SpinBox::InitializeLayout()
  {
  }

  void SpinBox::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    GetPainter().PaintBackground(graphics_engine, base);

    if (m_EditLine->IsMouseInside() || m_SpinnerUpBtn->IsMouseInside() || m_SpinnerDownBtn->IsMouseInside())
    {

      GetPainter().PaintShapeCorner(graphics_engine, m_SpinnerUpBtn->GetGeometry(), SPINBOX_BUTTON_MOUSEOVER_COLOR, eSHAPE_CORNER_ROUND4,
                                 eCornerTopRight, false);
      GetPainter().PaintShapeCorner(graphics_engine, m_SpinnerDownBtn->GetGeometry(), SPINBOX_BUTTON_MOUSEOVER_COLOR, eSHAPE_CORNER_ROUND4,
                                 eCornerBottomRight, false);
    }
    else
    {
      GetPainter().PaintShapeCorner(graphics_engine, m_SpinnerUpBtn->GetGeometry(), SPINBOX_BUTTON_COLOR, eSHAPE_CORNER_ROUND4,
                                 eCornerTopRight, false);
      GetPainter().PaintShapeCorner(graphics_engine, m_SpinnerDownBtn->GetGeometry(), SPINBOX_BUTTON_COLOR, eSHAPE_CORNER_ROUND4,
                                 eCornerBottomRight, false);
    }

    GeometryPositioning gp(eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning(m_SpinnerUpBtn->GetGeometry(), GetTheme().GetImageGeometry(eSPINER_UP), gp);

    if (m_SpinnerUpBtn->IsMouseInside())
      GetPainter().PaintShape(graphics_engine, GeoPo, Color(0xFFFFFFFF), eSPINER_UP);
    else
      GetPainter().PaintShape(graphics_engine, GeoPo, Color(0xFFFFFFFF), eSPINER_UP);


    gp.SetAlignment(eHACenter, eVACenter);
    GeoPo = ComputeGeometryPositioning(m_SpinnerDownBtn->GetGeometry(), GetTheme().GetImageGeometry(eSPINER_DOWN), gp);

    if (m_SpinnerDownBtn->IsMouseInside())
      GetPainter().PaintShape(graphics_engine, GeoPo, Color(0xFFFFFFFF), eSPINER_DOWN);
    else
      GetPainter().PaintShape(graphics_engine, GeoPo, Color(0xFFFFFFFF), eSPINER_DOWN);

    m_EditLine->QueueDraw();
  }

  void SpinBox::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    m_EditLine->ProcessDraw(graphics_engine, force_draw);
  }

  void SpinBox::SetValue(int value)
  {
    m_iValue = m_IntValidator.GetClampedValue(value);
    m_EditLine->SetText(std::to_string((long long)m_iValue));
    sigValueChanged.emit(this);
    sigValue.emit(m_iValue);
    QueueDraw();
  }

  int SpinBox::GetValue() const
  {
    return m_iValue;
  }

  void SpinBox::SetStep(int i)
  {
    m_Step = i;

    if (m_Step <= 0)
      m_Step = 1;

    QueueDraw();
  }

  int SpinBox::GetStep() const
  {
    return m_Step;
  }

  int SpinBox::GetMinValue() const
  {
    return m_IntValidator.GetMinimum();
  }

  int SpinBox::GetMaxValue() const
  {
    return m_IntValidator.GetMaximum();
  }

  void SpinBox::SetRange(int MinValue, int Maxvalue)
  {
    m_IntValidator.SetMinimum(MinValue);
    m_IntValidator.SetMaximum(Maxvalue);
    m_iValue = m_IntValidator.GetClampedValue(m_iValue);
    sigValueChanged.emit(this);
    sigValue.emit(m_iValue);
    QueueDraw();
  }

  void SpinBox::ImplementIncrementBtn()
  {
    SetValue(m_iValue + m_Step);

    if (m_iValue < m_IntValidator.GetMaximum())
    {
      if (m_UpTimerHandler.IsValid())
        m_UpTimerHandler = GetTimer().AddOneShotTimer(100, m_UpTimerCallback, 0);
      else
        m_UpTimerHandler = GetTimer().AddOneShotTimer(800, m_UpTimerCallback, 0);

      QueueDraw();
    }

    sigValueChanged.emit(this);
    sigIncrement.emit(this);
    sigValue.emit(m_iValue);
  }

  void SpinBox::ImplementDecrementBtn()
  {
    SetValue(m_iValue - m_Step);

    if (m_iValue > m_IntValidator.GetMinimum())
    {
      if (m_DownTimerHandler.IsValid())
        m_DownTimerHandler = GetTimer().AddOneShotTimer(100, m_DownTimerCallback, 0);
      else
        m_DownTimerHandler = GetTimer().AddOneShotTimer(800, m_DownTimerCallback, 0);

      QueueDraw();
    }

    sigValueChanged.emit(this);
    sigDecrement.emit(this);
    sigValue.emit(m_iValue);
  }

  void SpinBox::ImplementValidateEntry()
  {
    double ret = 0;
    ret = CharToDouble(m_EditLine->GetCleanText().c_str());
    {
      m_iValue = m_IntValidator.GetClampedValue(ret);
      m_EditLine->SetText(std::to_string((long long)m_iValue));
      sigValueChanged.emit(this);
      sigValue.emit(m_iValue);
//
//        if (m_iValue < m_IntValidator.GetMinimum())
//        {
//            m_iValue = m_IntValidator.GetMinimum();
//            m_EditLine->SetText(std::string::Printf("%d", m_iValue));
//        }
//        if (m_iValue > m_IntValidator.GetMaximum())
//        {
//            m_iValue = m_IntValidator.GetMaximum();
//            m_EditLine->SetText(std::string::Printf("%d", m_iValue));
//        }
    }
//     else
//     {
//         m_EditLine->SetText(std::string::Printf("%d", m_iValue));
//         sigValueChanged.emit(this);
//         sigValue.emit(m_iValue);
//     }
  }

}
