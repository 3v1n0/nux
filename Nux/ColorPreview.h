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


#ifndef COLORPREVIEW_H
#define COLORPREVIEW_H

#include "NuxCore/Color.h"
#include "NuxGraphics/GpuDevice.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "TimerProc.h"

namespace nux
{

  class HLayout;
  class StaticTextBox;
  class ColorEditor;
  class TimerHandle;
  class ColorDialogProxy;

  class ColorPreview : public View
  {
  public:
    ColorPreview(Color const& c = nux::color::Transparent, NUX_FILE_LINE_PROTO);
    ~ColorPreview();
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    sigc::signal< void, ColorEditor * > sigColorChanged;

    void SetColor(Color const& rgb);
    Color const& GetRGBColor() const;

  private:
    void RecvTimer(void *v);
    void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);

    TimerFunctor       *m_ChangeDetectionTimer;
    TimerHandle         m_ChangeTimerHandler;

    Color               m_Color;
    HLayout            *m_hlayout;
    InputArea           *m_ColorArea;
    StaticTextBox      *m_ColorValue;
    ColorDialogProxy   *m_DialogThreadProxy;
  };


}

#endif // COLORPREVIEW_H
