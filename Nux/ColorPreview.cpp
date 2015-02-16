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
#include "ColorPreview.h"
#include "ColorEditor.h"
#include "HLayout.h"
#include "TimerProc.h"
#include "StaticTextBox.h"

#include <sstream>
#include <iomanip>

namespace nux
{

  ColorPreview::ColorPreview(Color const& c, NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , m_Color(c)
  {
    //setSize(200, 100);
    m_hlayout           = new HLayout(NUX_TRACKER_LOCATION);
    m_ColorArea         = new BasicView(NUX_TRACKER_LOCATION);
    m_ColorValue        = new StaticTextBox("", NUX_TRACKER_LOCATION);
    m_DialogThreadProxy = new ColorDialogProxy(true);

    SetMaximumHeight(18);
    m_ColorArea->SetMaximumHeight(18);
    m_ColorArea->SetMinimumWidth(32);
    m_ColorArea->SetMaximumWidth(32);
    m_ColorValue->SetTextColor(Color(0xFFFFFFFF));
    m_ColorValue->SetFont(GetSysBoldFont());
    m_ColorValue->SetMinimumWidth(128);

    std::stringstream s;
    s << "[ R:" << (int)(m_Color.red * 255)
      << ", G:" << (int)(m_Color.green * 255)
      << ", B:" << (int)(m_Color.blue * 255)
      << " ]";
    m_ColorValue->SetText(s.str());

    m_ColorArea->mouse_click.connect(sigc::mem_fun(this, &ColorPreview::RecvClick));

    m_hlayout->AddView(m_ColorArea, 0);
    m_hlayout->AddView(m_ColorValue, 1);
    m_hlayout->SetHorizontalInternalMargin(4);
    SetCompositionLayout(m_hlayout);

    m_ChangeDetectionTimer = new TimerFunctor();
    m_ChangeDetectionTimer->tick.connect(sigc::mem_fun(this, &ColorPreview::RecvTimer));
    m_ChangeTimerHandler = 0;
  }

  ColorPreview::~ColorPreview()
  {
    delete m_ChangeDetectionTimer;

    if (m_ChangeTimerHandler.IsValid())
      GetTimer().RemoveTimerHandler(m_ChangeTimerHandler);

    delete m_DialogThreadProxy;
  }

  void ColorPreview::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground(graphics_engine, base);
    GetPainter().PaintShape(graphics_engine, m_ColorArea->GetGeometry(), m_Color, eSHAPE_CORNER_ROUND4, false);
    //GetPainter().Paint2DQuadWireFrameColor(graphics_engine, base, Color(COLOR_BACKGROUND_SECONDARY));
    m_ColorValue->QueueDraw();
  }

  void ColorPreview::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    m_ColorValue->ProcessDraw(graphics_engine, force_draw);
  }

  void ColorPreview::RecvClick(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    m_DialogThreadProxy->SetColor(m_Color);
    m_DialogThreadProxy->Start();

    m_ChangeTimerHandler = GetTimer().AddOneShotTimer(33, m_ChangeDetectionTimer, this);
  }

  void ColorPreview::RecvTimer(void * /* v */)
  {
    if (m_DialogThreadProxy->m_bDialogChange && m_DialogThreadProxy->m_bDialogRunning)
    {
      m_DialogThreadProxy->m_bDialogChange = false;
      m_Color = m_DialogThreadProxy->GetColor();
      QueueDraw();
    }

    if (m_DialogThreadProxy->IsActive())
    {
      m_ChangeTimerHandler = GetTimer().AddOneShotTimer(33, m_ChangeDetectionTimer, this);
    }
    else
    {
      if (m_ChangeTimerHandler.IsValid())
        GetTimer().RemoveTimerHandler(m_ChangeTimerHandler);

      m_ChangeTimerHandler = 0;
      m_Color = m_DialogThreadProxy->GetColor();
      QueueDraw();
    }
  }

  Color const& ColorPreview::GetRGBColor() const
  {
    return m_Color;
  }

  void ColorPreview::SetColor(Color const& color)
  {
    m_Color = color;
  }

}
