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

#include <sstream>
#include <iomanip>

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(RangeValue);

  RangeValue::RangeValue(float Value, float MinValue, float MaxValue, NUX_FILE_LINE_DECL)
  : View(NUX_FILE_LINE_PARAM)
  {
    m_min           = MinValue;
    m_max           = MaxValue;
    m_StartColor    = Color(0xff202020);
    m_EndColor      = Color(0xff202020);
    m_ProgressColor = Color(0xff606060);
    m_EnableDrawProgress    = true;
    m_CTRL_KEY      = 0;

    InitializeLayout();
    InitializeWidgets();
    SetValue(Value);
  }

  RangeValue::~RangeValue()
  {
  }

  void RangeValue::InitializeWidgets()
  {
    //////////////////
    // Set Signals  //
    //////////////////
    m_Percentage->mouse_down.connect(sigc::mem_fun(this, &RangeValue::OnReceiveMouseDown));
    m_Percentage->mouse_up.connect(sigc::mem_fun(this, &RangeValue::OnReceiveMouseUp));
    m_Percentage->mouse_drag.connect(sigc::mem_fun(this, &RangeValue::OnReceiveMouseDrag));

    m_ValueString->sigValidateKeyboardEntry.connect(sigc::mem_fun(this, &RangeValue::OnValidateKeyboardEntry));

    //////////////////
    // Set Geometry //
    //////////////////
    m_ValueString->SetMinimumSize(DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    m_ValueString->SetGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT));

    m_Percentage->SetMinimumSize(2 * DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    m_Percentage->SetGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT));

    // Set the minimum size of this widget.
    // This is use by RangeValuePropertyItem::GetItemBestHeight
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    hlayout->AddView(m_ValueString, 0, eCenter, eFull);
    hlayout->AddView(m_Percentage, 4, eCenter, eFull);
    //hlayout->AddLayout(&vlayout, 4);
    hlayout->SetHorizontalExternalMargin(0);
    hlayout->SetHorizontalInternalMargin(2);
    hlayout->SetVerticalExternalMargin(0);
    SetCompositionLayout(hlayout);
  }

  void RangeValue::InitializeLayout()
  {
    hlayout         = new HLayout(NUX_TRACKER_LOCATION);
    m_Percentage    = new BasicView(NUX_TRACKER_LOCATION);
    m_ValueString   = new EditTextBox("", NUX_TRACKER_LOCATION);
  }

  void RangeValue::DrawMarker(GraphicsEngine &graphics_engine)
  {
    int marker_position_x;
    int marker_position_y;

    graphics_engine.PushClippingRectangle(m_Percentage->GetGeometry());

    marker_position_x = m_Percentage->GetBaseX() + (m_Value - m_min) * m_Percentage->GetBaseWidth() * 1 / (m_max - m_min);
    marker_position_y = m_Percentage->GetBaseY() + m_Percentage->GetBaseHeight();
    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color(0.0f, 0.0f, 0.0f, 1.0f));

    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color(0.7f, 0.7f, 0.7f, 1.0f));

    graphics_engine.PopClippingRectangle();
  }


  void RangeValue::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    // Percentage
    Geometry P = m_Percentage->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, m_StartColor, m_StartColor, m_EndColor, m_EndColor);

    if (m_EnableDrawProgress)
    {
      P.SetWidth((m_Value - m_min) * (float) P.GetWidth() / (m_max - m_min));
      GetPainter().Paint2DQuadColor(graphics_engine, P, m_ProgressColor);
    }

    m_ValueString->ProcessDraw(graphics_engine, true);

    DrawMarker(graphics_engine);
  }

  void RangeValue::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    m_ValueString->ProcessDraw(graphics_engine, force_draw);
  }

/////////////////
//  RECEIVERS  //
/////////////////
  void RangeValue::SetRange(float min_value, float max_value)
  {
    if (min_value < max_value)
    {
      m_min = min_value;
      m_max = max_value;
    }
    else
    {
      m_min = max_value;
      m_max = min_value;
    }

    if (m_Value < m_min)
      m_Value = m_min;
    else if (m_Value > m_max)
      m_Value = m_max;

    SetValue(m_Value);
  }

  void RangeValue::SetValue(float value)
  {
    if (value < m_min)
      m_Value = m_min;
    else if (value > m_max)
      m_Value = m_max;
    else
      m_Value = value;

    m_ValueString->SetText(std::to_string((long double)m_Value));
    QueueDraw();
  }

  float RangeValue::GetValue() const
  {
    return m_Value;
  }

////////////////
//  EMITTERS  //
////////////////
  void RangeValue::OnReceiveMouseDown(int x, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (x < 0)
      m_Value = m_min;
    else if (x > m_Percentage->GetBaseWidth())
      m_Value = m_max;
    else
      m_Value = m_min + (m_max - m_min) * (float) x / (float) m_Percentage->GetBaseWidth();

    m_ValueString->SetText(std::to_string((long double)m_Value));
    sigValueChanged.emit(this);
    sigFloatChanged.emit(m_Value);
    sigMouseDown.emit(m_Value);

    QueueDraw();
  }

  void RangeValue::OnReceiveMouseUp(int x, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {

    if (x < 0)
      m_Value = m_min;
    else if (x > m_Percentage->GetBaseWidth())
      m_Value = m_max;
    else
      m_Value = m_min + (m_max - m_min) * (float) x / (float) m_Percentage->GetBaseWidth();

    std::stringstream s;
    s << std::setprecision(3) << m_Value;
    m_ValueString->SetText(s.str());
    sigValueChanged.emit(this);
    sigFloatChanged.emit(m_Value);
    sigMouseUp.emit(m_Value);

    QueueDraw();
  }

  void RangeValue::OnReceiveMouseDrag(int x, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (x < 0)
      m_Value = m_min;
    else if (x > m_Percentage->GetBaseWidth())
      m_Value = m_max;
    else
      m_Value = m_min + (m_max - m_min) * (float) x / (float) m_Percentage->GetBaseWidth();

    std::stringstream s;
    s << std::setprecision(3) << m_Value;
    m_ValueString->SetText(s.str());
    sigValueChanged.emit(this);
    sigFloatChanged.emit(m_Value);
    sigMouseDrag.emit(m_Value);

    QueueDraw();
  }

  void RangeValue::OnKeyboardFocus()
  {

  }

  void RangeValue::OnLostKeyboardFocus()
  {

  }

  void RangeValue::OnValidateKeyboardEntry(EditTextBox* /* textbox */, const std::string &text)
  {
    float f;
    f = CharToDouble(text.c_str());
    SetValue(f);
    sigValueChanged.emit(this);
    sigFloatChanged.emit(m_Value);
    sigSetTypedValue.emit(f);
    QueueDraw();
  }

  void RangeValue::EmitFloatChangedSignal()
  {
    sigFloatChanged.emit(m_Value);
  }

  void RangeValue::SetBackgroundColor(const Color &color)
  {
    m_ValueString->SetTextBackgroundColor(color);
  }

  const Color RangeValue::GetBackgroundColor() const
  {
    return m_ValueString->GetTextBackgroundColor();
  }

}
