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
#include "TimerProc.h"
#include "WindowCompositor.h"
#include "EditTextBox.h"
#include "StaticTextBox.h"
#include "Vector3Valuator.h"
#include "HLayout.h"
#include "Vector3ValuatorDouble.h"

namespace nux
{

  const Color SPINBOX_BUTTON_COLOR = Color (0xFF4D4D4D);
  const Color SPINBOX_BUTTON_MOUSEOVER_COLOR = Color (0xFF222222);

  Vector3DoubleValuator::Vector3DoubleValuator (double X, double Y, double Z, double Step, double MinValue, double MaxValue)
    :   m_DoubleValidator (MinValue, MaxValue)
  {
    m_Step = Step;
    m_X = X;
    m_Y = Y;
    m_Z = Z;

    m_hlayout   = new HLayout (TEXT (""), NUX_TRACKER_LOCATION);
    m_XEdit     = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_YEdit     = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_ZEdit     = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);

    m_DoubleValidator.SetDecimals (3);
    m_XEdit->SetValidator (&m_DoubleValidator);
    m_XEdit->SetSuffix (TEXT ("") );
    m_XEdit->SetPrefix (TEXT ("") );
    m_XEdit->SetText (m_DoubleValidator.ToString (m_X) );

    m_YEdit->SetValidator (&m_DoubleValidator);
    m_YEdit->SetSuffix (TEXT ("") );
    m_YEdit->SetPrefix (TEXT ("") );
    m_YEdit->SetText (m_DoubleValidator.ToString (m_Y) );

    m_ZEdit->SetValidator (&m_DoubleValidator);
    m_ZEdit->SetSuffix (TEXT ("") );
    m_ZEdit->SetPrefix (TEXT ("") );
    m_ZEdit->SetText (m_DoubleValidator.ToString (m_Z) );

    m_XEdit->SetMinimumSize (DEFAULT_WIDGET_WIDTH - 10, DEFAULT_WIDGET_HEIGHT);
    m_XEdit->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );
    m_YEdit->SetMinimumSize (DEFAULT_WIDGET_WIDTH - 10, DEFAULT_WIDGET_HEIGHT);
    m_YEdit->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );
    m_ZEdit->SetMinimumSize (DEFAULT_WIDGET_WIDTH - 10, DEFAULT_WIDGET_HEIGHT);
    m_ZEdit->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );

    m_MouseControlledButton->SetMinimumSize (16 + 9, 10);
    m_MouseControlledButton->SetGeometry (Geometry (0, 0, 15, 10) );

    // Set the minimum size of this widget.
    // This is use by TextLineEditPropertyItem::GetItemBestHeight
    SetMinimumSize (DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    m_XLabel = new StaticTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_YLabel = new StaticTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_ZLabel = new StaticTextBox (TEXT (""), NUX_TRACKER_LOCATION);

    m_XLabel->SetText (TEXT ("X") );
    m_YLabel->SetText (TEXT ("Y") );
    m_ZLabel->SetText (TEXT ("Z") );
    m_XLabel->SetMinimumWidth (12);
    m_YLabel->SetMinimumWidth (12);
    m_ZLabel->SetMinimumWidth (12);
    m_XLabel->SetTextColor (Color (0xFFCCCCCC) );
    m_YLabel->SetTextColor (Color (0xFFCCCCCC) );
    m_ZLabel->SetTextColor (Color (0xFFCCCCCC) );

    m_XLabel->SetFont (GetThreadBoldFont() );
    m_YLabel->SetFont (GetThreadBoldFont() );
    m_ZLabel->SetFont (GetThreadBoldFont() );


    //SpaceLayout* spacex = new SpaceLayout;
    //spacex->SetMinimumWidth(4);
    //SpaceLayout* spacey = new SpaceLayout;
    //spacey->SetMinimumWidth(4);

    SpaceLayout *spacez = new SpaceLayout();

    spacez->SetMinimumWidth (4);
    m_hlayout->AddView (m_MouseControlledButton, 0);
    m_hlayout->AddView (m_XLabel, 0);
    //m_hlayout->AddLayout(spacex, 0);
    m_hlayout->AddView (m_XEdit, 1);
    m_hlayout->AddView (m_YLabel, 0);
    //m_hlayout->AddLayout(spacey, 0);
    m_hlayout->AddView (m_YEdit, 1);
    m_hlayout->AddView (m_ZLabel, 0);
    m_hlayout->AddView (m_ZEdit, 1);
    m_hlayout->AddLayout (spacez, 0);
    m_hlayout->SetHorizontalInternalMargin (4);
    SetCompositionLayout (m_hlayout);
  }

  Vector3DoubleValuator::~Vector3DoubleValuator()
  {
  }

  long Vector3DoubleValuator::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = m_MouseControlledButton->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_XEdit->ProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_YEdit->ProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_ZEdit->ProcessEvent (ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void Vector3DoubleValuator::Draw (GraphicsContext &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    gPainter.PaintBackground (GfxContext, base);


    Geometry geo = m_MouseControlledButton->GetGeometry();
    geo.OffsetSize (5, 0);

    if (m_XEdit->IsMouseInside() ||
        m_YEdit->IsMouseInside() ||
        m_ZEdit->IsMouseInside() ||
        m_MouseControlledButton->HasMouseFocus() || m_MouseControlledButton->IsMouseInside() )
    {

      gPainter.PushDrawShapeLayer (GfxContext, geo, eSHAPE_CORNER_ROUND4, SPINBOX_BUTTON_MOUSEOVER_COLOR, eCornerTopLeft | eCornerBottomLeft);
    }
    else
    {
      gPainter.PushDrawShapeLayer (GfxContext, geo, eSHAPE_CORNER_ROUND4, SPINBOX_BUTTON_COLOR,
                                   eCornerTopLeft | eCornerBottomLeft);
    }

    geo = base;
    geo.OffsetSize (-m_MouseControlledButton->GetBaseWidth(), 0);
    geo.OffsetPosition (+m_MouseControlledButton->GetBaseWidth(), 0);

    gPainter.PushDrawShapeLayer (GfxContext, geo, eSHAPE_CORNER_ROUND4, SPINBOX_BUTTON_COLOR,
                                 eCornerTopLeft | eCornerBottomLeft | eCornerTopRight | eCornerBottomRight);

    GeometryPositioning gp (eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning (m_MouseControlledButton->GetGeometry(), gTheme.GetImageGeometry (eVALUATORMOVE), gp);

    if (m_Mouse == 1)
    {
      if (m_MouseControlledButton->IsMouseInside() )
        gPainter.PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eVALUATORHORIZONTALMOVE);
      else
        gPainter.PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eVALUATORHORIZONTALMOVE);
    }
    else if (m_Mouse == 3)
    {
      if (m_MouseControlledButton->IsMouseInside() )
        gPainter.PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eVALUATORVERTICALMOVE);
      else
        gPainter.PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eVALUATORVERTICALMOVE);
    }
    else
    {
      if (m_MouseControlledButton->IsMouseInside() )
        gPainter.PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eVALUATORMOVE);
      else
        gPainter.PaintShape (GfxContext, GeoPo, Color (0xFFFFFFFF), eVALUATORMOVE);
    }



    geo = m_XEdit->GetGeometry();
    geo.OffsetPosition (-4, 0);
    geo.OffsetSize (+8, 0);
    gPainter.PaintShapeCorner (GfxContext, geo, m_XEdit->GetTextBackgroundColor(), eSHAPE_CORNER_ROUND4,
                               eCornerTopLeft | eCornerBottomLeft | eCornerTopRight | eCornerBottomRight, false);

    geo = m_YEdit->GetGeometry();
    geo.OffsetPosition (-4, 0);
    geo.OffsetSize (+8, 0);
    gPainter.PaintShapeCorner (GfxContext, geo, m_YEdit->GetTextBackgroundColor(), eSHAPE_CORNER_ROUND4,
                               eCornerTopLeft | eCornerBottomLeft | eCornerTopRight | eCornerBottomRight, false);

    geo = m_ZEdit->GetGeometry();
    geo.OffsetPosition (-4, 0);
    geo.OffsetSize (+8, 0);
    gPainter.PaintShapeCorner (GfxContext, geo, m_ZEdit->GetTextBackgroundColor(), eSHAPE_CORNER_ROUND4,
                               eCornerTopLeft | eCornerBottomLeft | eCornerTopRight | eCornerBottomRight, false);

//    gPainter.PaintShape(GfxContext, m_XLabel->GetGeometry(), m_XEdit->GetTextBackgroundColor(), eVECTORXLABEL);
//    gPainter.PaintShape(GfxContext, m_YLabel->GetGeometry(), m_YEdit->GetTextBackgroundColor(), eVECTORYLABEL);
//    gPainter.PaintShape(GfxContext, m_ZLabel->GetGeometry(), m_ZEdit->GetTextBackgroundColor(), eVECTORZLABEL);


    m_XEdit->NeedRedraw();
    m_YEdit->NeedRedraw();
    m_ZEdit->NeedRedraw();
    m_XLabel->NeedRedraw();
    m_YLabel->NeedRedraw();
    m_ZLabel->NeedRedraw();
  }

  void Vector3DoubleValuator::DrawContent (GraphicsContext &GfxContext, bool force_draw)
  {
    m_XEdit->ProcessDraw (GfxContext, force_draw);
    m_YEdit->ProcessDraw (GfxContext, force_draw);
    m_ZEdit->ProcessDraw (GfxContext, force_draw);
    m_XLabel->ProcessDraw (GfxContext, force_draw);
    m_YLabel->ProcessDraw (GfxContext, force_draw);
    m_ZLabel->ProcessDraw (GfxContext, force_draw);
  }

  void Vector3DoubleValuator::PostDraw (GraphicsContext &GfxContext, bool force_draw)
  {

  }

  void Vector3DoubleValuator::SetValue (double x, double y, double z)
  {
    SetVectorX (x);
    SetVectorY (y);
    SetVectorZ (z);
  }

  void Vector3DoubleValuator::SetVectorX (double value)
  {
    m_X = m_DoubleValidator.Validate (value);
    m_XEdit->SetText (inlPrintf ("%.3f", m_X) );
    sigValueChanged.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
    NeedRedraw();
  }

  void Vector3DoubleValuator::SetVectorY (double value)
  {
    m_Y = m_DoubleValidator.Validate (value);
    m_YEdit->SetText (inlPrintf ("%.3f", m_Y) );
    sigValueChanged.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
    NeedRedraw();
  }

  void Vector3DoubleValuator::SetVectorZ (double value)
  {
    m_Z = m_DoubleValidator.Validate (value);
    m_ZEdit->SetText (inlPrintf ("%.3f", m_Z) );
    sigValueChanged.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
    NeedRedraw();
  }

  double Vector3DoubleValuator::GetVectorX() const
  {
    return m_X;
  }

  double Vector3DoubleValuator::GetVectorY() const
  {
    return m_Y;
  }

  double Vector3DoubleValuator::GetVectorZ() const
  {
    return m_Z;
  }

  void Vector3DoubleValuator::SetStep (double i)
  {
    m_Step = i;

    if (m_Step <= 0)
      m_Step = 1;

    NeedRedraw();
  }

  double Vector3DoubleValuator::GetStep() const
  {
    return m_Step;
  }

  double Vector3DoubleValuator::GetMinValue() const
  {
    return m_DoubleValidator.GetMinimum();
  }

  double Vector3DoubleValuator::GetMaxValue() const
  {
    return m_DoubleValidator.GetMaximum();
  }

  void Vector3DoubleValuator::SetRange (double MinValue, double Maxvalue)
  {
    m_DoubleValidator.SetMinimum (MinValue);
    m_DoubleValidator.SetMaximum (Maxvalue);
    m_X = m_DoubleValidator.Validate (m_X);
    m_Y = m_DoubleValidator.Validate (m_Y);
    m_Z = m_DoubleValidator.Validate (m_Z);
    sigValueChanged.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
    NeedRedraw();
  }

  void Vector3DoubleValuator::ImplementIncrementX()
  {
    SetBaseX (m_X + m_Step);

    if (m_X < m_DoubleValidator.GetMaximum() )
    {
      NeedRedraw();
    }

    sigValueChanged.emit (this);
    sigIncrement.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
  }

  void Vector3DoubleValuator::ImplementDecrementX()
  {
    SetBaseX (m_X - m_Step);

    if (m_X > m_DoubleValidator.GetMinimum() )
    {
      NeedRedraw();
    }

    sigValueChanged.emit (this);
    sigDecrement.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
  }

  void Vector3DoubleValuator::ImplementIncrementY()
  {
    SetBaseY (m_Y + m_Step);

    if (m_Y < m_DoubleValidator.GetMaximum() )
    {
      NeedRedraw();
    }

    sigValueChanged.emit (this);
    sigIncrement.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
  }

  void Vector3DoubleValuator::ImplementDecrementY()
  {
    SetVectorY (m_Y - m_Step);

    if (m_Y > m_DoubleValidator.GetMinimum() )
    {
      NeedRedraw();
    }

    sigValueChanged.emit (this);
    sigDecrement.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
  }

  void Vector3DoubleValuator::ImplementIncrementZ()
  {
    SetVectorZ (m_Z + m_Step);

    if (m_Z < m_DoubleValidator.GetMaximum() )
    {
      NeedRedraw();
    }

    sigValueChanged.emit (this);
    sigIncrement.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
  }

  void Vector3DoubleValuator::ImplementDecrementZ()
  {
    SetVectorZ (m_Z - m_Step);

    if (m_Z > m_DoubleValidator.GetMinimum() )
    {
      NeedRedraw();
    }

    sigValueChanged.emit (this);
    sigDecrement.emit (this);
    sigValue.emit (m_X, m_Y, m_Z);
  }

  void Vector3DoubleValuator::ImplementValidateEntryX()
  {
    double ret = 0;
    ret = CharToDouble (m_XEdit->GetCleanText().GetTCharPtr() );
    {
      m_X = m_DoubleValidator.Validate (ret);
      m_XEdit->SetText (inlPrintf ("%.3f", m_X) );
      sigValueChanged.emit (this);
      sigValue.emit (m_X, m_Y, m_Z);
    }
//     else
//     {
//         m_XEdit->SetText(inlPrintf("%.3f", m_X));
//         sigValueChanged.emit(this);
//         sigValue.emit(m_X, m_Y, m_Z);
//     }
  }

  void Vector3DoubleValuator::ImplementValidateEntryY()
  {
    double ret = 0;
    ret = CharToDouble (m_YEdit->GetCleanText().GetTCharPtr() );
    {
      m_Y = m_DoubleValidator.Validate (ret);
      m_YEdit->SetText (inlPrintf ("%.3f", m_Y) );
      sigValueChanged.emit (this);
      sigValue.emit (m_X, m_Y, m_Z);
    }
//     else
//     {
//         m_YEdit->SetText(inlPrintf("%.3f", m_Y));
//         sigValueChanged.emit(this);
//         sigValue.emit(m_X, m_Y, m_Z);
//     }
  }

  void Vector3DoubleValuator::ImplementValidateEntryZ()
  {
    double ret = 0;
    ret = CharToDouble (m_ZEdit->GetCleanText().GetTCharPtr() );
    {
      m_Z = m_DoubleValidator.Validate (ret);
      m_ZEdit->SetText (inlPrintf ("%.3f", m_Z) );
      sigValueChanged.emit (this);
      sigValue.emit (m_X, m_Y, m_Z);
    }
//     else
//     {
//         m_ZEdit->SetText(inlPrintf("%.3f", m_Z));
//         sigValueChanged.emit(this);
//         sigValue.emit(m_X, m_Y, m_Z);
//     }
  }


}
