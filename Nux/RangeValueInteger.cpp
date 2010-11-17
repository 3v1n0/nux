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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "Nux.h"
#include "EditTextBox.h"
#include "HLayout.h"
#include "RangeValueInteger.h"

namespace nux
{

  RangeValueInteger::RangeValueInteger (int Value, int MinValue, int MaxValue, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    m_min                   = MinValue;
    m_max                   = MaxValue;
    m_StartColor            = Color (0xff202020);
    m_EndColor              = Color (0xff202020);
    m_ProgressColor         = Color (0xff606060);
    m_EnableDrawProgress    = true;
    m_CTRL_KEY              = 0;
    m_MarkerPosition        = 0;

    InitializeLayout();
    InitializeWidgets();

    SetValue (Value);
  }

  RangeValueInteger::~RangeValueInteger()
  {
    DestroyLayout();
  }

  void RangeValueInteger::InitializeWidgets()
  {
    //////////////////
    // Set Signals  //
    //////////////////
    m_Percentage->OnMouseDown.connect ( sigc::mem_fun (this, &RangeValueInteger::OnReceiveMouseDown) );
    m_Percentage->OnMouseUp.connect ( sigc::mem_fun (this, &RangeValueInteger::OnReceiveMouseUp) );
    m_Percentage->OnMouseDrag.connect ( sigc::mem_fun (this, &RangeValueInteger::OnReceiveMouseDrag) );

    m_ValueString->sigValidateKeyboardEntry.connect (sigc::mem_fun (this, &RangeValueInteger::OnValidateKeyboardEntry) );

    //////////////////
    // Set Geometry //
    //////////////////
    m_ValueString->SetMinimumSize (DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    m_ValueString->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT) );

    m_Percentage->SetMinimumSize (2 * DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT);
    m_Percentage->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT) );

    // Set the minimum size of this widget.
    // This is use by RangeValuePropertyItem::GetItemBestHeight
    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);


    hlayout->AddView (m_ValueString, 0, eCenter, eFull);
    hlayout->AddView (m_Percentage, 4, eCenter, eFull);
    //hlayout->AddLayout(&vlayout, 4);
    hlayout->SetHorizontalExternalMargin (0);
    hlayout->SetHorizontalInternalMargin (2);
    hlayout->SetVerticalExternalMargin (0);
    SetCompositionLayout (hlayout);
  }

  void RangeValueInteger::InitializeLayout()
  {
    hlayout         = new HLayout (TEXT (""), NUX_TRACKER_LOCATION);
    m_Percentage    = new CoreArea (NUX_TRACKER_LOCATION);
    m_ValueString   = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
  }

  void RangeValueInteger::DestroyLayout()
  {
  }


  long RangeValueInteger::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    m_CTRL_KEY = ievent.GetVirtualKeyState (NUX_VK_CONTROL);

    long ret;
    ret = m_Percentage->OnEvent (ievent, TraverseInfo, ProcessEventInfo);
    ret = m_ValueString->ProcessEvent (ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);

    if (m_ValueString->IsRedrawNeeded() )
    {
      NeedRedraw();
    }

    return ret;
  }

  void RangeValueInteger::DrawMarker (GraphicsEngine &GfxContext)
  {
    int marker_position_x;
    int marker_position_y;

    GfxContext.PushClippingRectangle (m_Percentage->GetGeometry() );

    marker_position_x = m_Percentage->GetBaseX() + (m_MarkerPosition - m_min) * m_Percentage->GetBaseWidth() * 1 / (m_max - m_min);
    marker_position_y = m_Percentage->GetBaseY() + m_Percentage->GetBaseHeight();
    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color (0.0f, 0.0f, 0.0f, 1.0f) );

    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color (0.7f, 0.7f, 0.7f, 1.0f) );

    GfxContext.PopClippingRectangle();
  }

  void RangeValueInteger::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    bool highlighted;
    Geometry base = GetGeometry();

    // Percentage
    Geometry P = m_Percentage->GetGeometry();
    GetPainter().Paint2DQuadColor (GfxContext, P, m_StartColor, m_StartColor, m_EndColor, m_EndColor);

    if (m_EnableDrawProgress)
    {
      P.SetWidth ( (m_MarkerPosition - m_min) * (float) P.GetWidth() / (m_max - m_min) );
      GetPainter().Paint2DQuadColor (GfxContext, P, m_ProgressColor);
    }


    if (m_ValueString->IsMouseInside() )
      highlighted = true;
    else
      highlighted = false;

    DrawMarker (GfxContext);
  }

  void RangeValueInteger::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    m_ValueString->ProcessDraw (GfxContext, force_draw);
  }

  void RangeValueInteger::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void RangeValueInteger::SetRange (int min_value, int max_value)
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

    SetValue (m_Value);
  }

  void RangeValueInteger::SetValue (int value)
  {
    if (value < m_min)
      m_Value = m_min;
    else if (value > m_max)
      m_Value = m_max;
    else
      m_Value = value;

    m_MarkerPosition = m_Value;
    m_ValueString->SetText (inlPrintf ("%d", m_Value) );
    NeedRedraw();
  }

  int RangeValueInteger::GetValue() const
  {
    return m_Value;
  }

////////////////
//  EMITTERS  //
////////////////
  void RangeValueInteger::OnReceiveMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (x < 0)
    {
      m_Value = m_min;
      m_MarkerPosition = m_Value;
    }
    else if (x > m_Percentage->GetBaseWidth() )
    {
      m_Value = m_max;
      m_MarkerPosition = m_Value;
    }
    else
    {
      m_Value = m_min + (m_max - m_min) * (float) x / (float) m_Percentage->GetBaseWidth();
      m_MarkerPosition = m_min + (m_max - m_min) * (float) x / (float) m_Percentage->GetBaseWidth();

      if (m_MarkerPosition - m_Value > 0.5f)
        m_Value++;
    }

    m_ValueString->SetText (inlPrintf ("%d", m_Value) );
    sigValueChanged.emit (this);
    sigMouseDown.emit (m_Value);

    NeedRedraw();
  }

  void RangeValueInteger::OnReceiveMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (x < 0)
      m_Value = m_min;
    else if (x > m_Percentage->GetBaseWidth() )
      m_Value = m_max;
    else
    {
      m_Value = m_min + (m_max - m_min) * (float) x / (float) m_Percentage->GetBaseWidth();

      if (m_MarkerPosition - m_Value > 0.5f)
        m_Value++;
    }

    m_MarkerPosition = m_Value;
    m_ValueString->SetText (inlPrintf ("%d", m_Value) );
    sigValueChanged.emit (this);
    sigMouseUp.emit (m_Value);

    NeedRedraw();
  }

  void RangeValueInteger::OnReceiveMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    if (x < 0)
    {
      m_Value = m_min;
      m_MarkerPosition = m_Value;
    }
    else if (x > m_Percentage->GetBaseWidth() )
    {
      m_Value = m_max;
      m_MarkerPosition = m_Value;
    }
    else
    {
      m_Value = m_min + (m_max - m_min) * (float) x / (float) m_Percentage->GetBaseWidth();
      m_MarkerPosition = m_min + (m_max - m_min) * (float) x / (float) m_Percentage->GetBaseWidth();

      if (m_MarkerPosition - m_Value > 0.5f)
        m_Value++;
    }

    m_ValueString->SetText (inlPrintf ("%d", m_Value) );
    sigValueChanged.emit (this);
    sigMouseDrag.emit (m_Value);

    NeedRedraw();
  }

  void RangeValueInteger::OnKeyboardFocus()
  {

  }

  void RangeValueInteger::OnLostKeyboardFocus()
  {

  }

  void RangeValueInteger::OnValidateKeyboardEntry (EditTextBox *textbox, const NString &text)
  {
    int i;
    i = CharToInteger (text.GetTCharPtr() );
    SetValue (i);
    sigValueChanged.emit (this);
    sigSetTypedValue.emit (i);
    NeedRedraw();
  }

}
