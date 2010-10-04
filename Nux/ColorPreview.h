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


#ifndef COLORPREVIEW_H
#define COLORPREVIEW_H

#include "NuxGraphics/GLDeviceFactory.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "TimerProc.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class HLayout;
class StaticTextBox;
class ColorEditor;
class TimerFunctor;
class TimerHandle;
class ColorDialogProxy;

class ColorPreview : public View
{
public:
    ColorPreview(float red = 0.0f, float green = 0.0f, float blue = 0.0f,  eColorModel colormodel = CM_RGB, NUX_FILE_LINE_PROTO);
    ~ColorPreview();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    sigc::signal< void, ColorEditor* > sigColorChanged;

    void SetRed(double r);
    void SetGreen(double g);
    void SetBlue(double b);

    void SetRGB(double r, double g, double b);
    void SetRGBA(double r, double g, double b, double a);
    void SetColor(Color rgb);
    Color GetRGBColor() const;

private:
    void RecvTimer(void* v);
    void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);

    TimerFunctor*       m_ChangeDetectionTimer;
    TimerHandle         m_ChangeTimerHandler;

    Color               m_Color;
    HLayout*            m_hlayout;
    CoreArea*           m_ColorArea;
    StaticTextBox*      m_ColorValue;
    ColorDialogProxy*   m_DialogThreadProxy;
};


} //NUX_NAMESPACE_END

#endif // COLORPREVIEW_H
