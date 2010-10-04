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
#include "ColorPreview.h"
#include "ColorEditor.h"
#include "HLayout.h"
#include "TimerProc.h"
#include "StaticTextBox.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  static const int GRAPH_MARGIN = 1;

  ColorPreview::ColorPreview (float red, float green, float blue, eColorModel colormodel, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_Color (1.0f, 1.0f, 1.0f, 1.0f)
  {
    //setSize(200, 100);
    m_hlayout           = new HLayout();
    m_ColorArea         = new CoreArea();
    m_ColorValue        = new StaticTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_DialogThreadProxy = new ColorDialogProxy (true);

    SetMaximumHeight (18);
    m_ColorArea->SetMaximumHeight (18);
    m_ColorArea->SetMinimumWidth (32);
    m_ColorArea->SetMaximumWidth (32);
    m_ColorValue->SetTextColor (Color (0xFFFFFFFF) );
    m_ColorValue->SetFont (GetThreadBoldFont() );
    m_ColorValue->SetMinimumWidth (128);

    if (colormodel == CM_HSV)
    {
//         m_ColorEditor->SetHSV(red, green, blue);
//         m_ColorEditor->SetColorModel(CM_HSV, CC_HUE);
    }
    else //(colormodel == RGB)
    {
//         m_ColorEditor->SetRGB(red, green, blue);
//         m_ColorEditor->SetColorModel(CM_RGB, CC_RED);
    }

    NString text = inlPrintf (TEXT ("[ R:%d, G:%d, B:%d ]"), (int) (m_Color.R() * 255), (int) (m_Color.G() * 255), (int) (m_Color.B() * 255) );
    m_ColorValue->SetText (text);


    m_ColorArea->OnMouseClick.connect (sigc::mem_fun (this, &ColorPreview::RecvClick) );
    //m_ColorValue->OnMouseClick.connect(sigc::mem_fun(this, &ColorPreview::RecvClick));
    //OnMouseClick.connect(sigc::mem_fun(this, &ColorPreview::RecvClick));

    m_hlayout->AddView (m_ColorArea, 0);
    m_hlayout->AddView (m_ColorValue, 1);
    m_hlayout->SetHorizontalInternalMargin (4);
    SetCompositionLayout (m_hlayout);

    m_ChangeDetectionTimer = new TimerFunctor();
    m_ChangeDetectionTimer->OnTimerExpired.connect (sigc::mem_fun (this, &ColorPreview::RecvTimer) );
    m_ChangeTimerHandler = 0;
  }

  ColorPreview::~ColorPreview()
  {
    if (m_ChangeTimerHandler.IsValid() )
      GetThreadTimer().RemoveTimerHandler (m_ChangeTimerHandler);

    NUX_SAFE_DELETE (m_DialogThreadProxy);
  }

  long ColorPreview::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = m_ColorArea->OnEvent (ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void ColorPreview::Draw (GraphicsContext &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    gPainter.PaintBackground (GfxContext, base);
    gPainter.PaintShape (GfxContext, m_ColorArea->GetGeometry(), m_Color, eSHAPE_CORNER_ROUND4, false);
    //gPainter.Paint2DQuadWireFrameColor(GfxContext, base, Color(COLOR_BACKGROUND_SECONDARY));
    m_ColorValue->NeedRedraw();
  }

  void ColorPreview::DrawContent (GraphicsContext &GfxContext, bool force_draw)
  {
    m_ColorValue->ProcessDraw (GfxContext, force_draw);
  }

  void ColorPreview::PostDraw (GraphicsContext &GfxContext, bool force_draw)
  {

  }

  void ColorPreview::RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    m_DialogThreadProxy->SetColor (m_Color);
    m_DialogThreadProxy->Start();

    m_ChangeTimerHandler = GetThreadTimer().AddTimerHandler (33, m_ChangeDetectionTimer, this);
  }

  void ColorPreview::RecvTimer (void *v)
  {
    if (m_DialogThreadProxy->m_bDialogChange && m_DialogThreadProxy->m_bDialogRunning)
    {
      m_DialogThreadProxy->m_bDialogChange = false;
      m_Color = m_DialogThreadProxy->GetColor();
      NeedRedraw();
    }

    if (m_DialogThreadProxy->IsActive() )
    {
      m_ChangeTimerHandler = GetThreadTimer().AddTimerHandler (33, m_ChangeDetectionTimer, this);
    }
    else
    {
      if (m_ChangeTimerHandler.IsValid() )
        GetThreadTimer().RemoveTimerHandler (m_ChangeTimerHandler);

      m_ChangeTimerHandler = 0;
      m_Color = m_DialogThreadProxy->GetColor();
      NeedRedraw();
    }
  }

  Color ColorPreview::GetRGBColor() const
  {
    return m_Color;
  }

  void ColorPreview::SetRed (double r)
  {
    m_Color.SetRed (r);
  }

  void ColorPreview::SetGreen (double g)
  {
    m_Color.SetGreen (g);
  }

  void ColorPreview::SetBlue (double b)
  {
    m_Color.SetBlue (b);
  }

  void ColorPreview::SetRGB (double r, double g, double b)
  {
    m_Color.SetRGB (r, g, b);
  }

  void ColorPreview::SetRGBA (double r, double g, double b, double a)
  {
    m_Color.SetRGBA (r, g, b, a);
  }

  void ColorPreview::SetColor (Color color)
  {
    m_Color = color;
  }

} //NUX_NAMESPACE_END
