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


#ifndef COLOREDITOR_H
#define COLOREDITOR_H

#include "NuxGraphics/GpuDevice.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "NuxGraphics/GLSh_DrawFunction.h"
#include "DoubleValidator.h"
#include "TimerProc.h"

namespace nux
{

  class GLSh_ColorPicker;
  class HLayout;
  class VLayout;
  class EditTextBox;
  class CheckBox;
  class ToggleButton;
  class Button;

  class ColorEditor;
  class ColorPreview;

  class ColorDialogProxy
  {
  public:
    ColorDialogProxy (bool ModalWindow);
    ~ColorDialogProxy();

    void RecvDialogOk (ColorEditor *coloreditor);
    void RecvDialogCancel (ColorEditor *coloreditor);
    void RecvDialogChange (ColorEditor *coloreditor);

    void Start();
    bool IsActive();
    void StopThreadMonitoring();

    void SetColor (Color color);
    Color GetColor();
    void SetPreviousColor (Color color);
    Color GetPreviousColor();
    void SetColorModel (eColorModel color_model);
    eColorModel GetColorModel();
    void SetColorChannel (eColorChannel color_model);
    eColorChannel GetColorChannel();

  private:
    bool m_bDialogChange;
    bool m_bDialogRunning;
    unsigned int m_DialogThreadID;
    Color m_RGBColor;
    Color m_PreviousRGBColor;
    eColorModel m_ColorModel;
    eColorChannel m_ColorChannel;
    bool m_ModalWindow;
    NThread *m_Thread;

    friend class ColorPreview;
  };

  class ColorEditor : public View
  {
  public:
    ColorEditor (NUX_FILE_LINE_PROTO);
    ~ColorEditor();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);


    void SetRed (double r);
    void SetGreen (double g);
    void SetBlue (double b);
    void SetHue (double h);
    void SetSaturation (double s);
    void SetValue (double v);

    void SetRGB (double r, double g, double b);
    void SetHSV (double h, double s, double v);
    void SetRGB (Color rgb);
    void SetHSV (Color hsv);
    Color GetRGBColor() const;
    Color GetHSVColor() const;

    void SetColorModel (eColorModel, eColorChannel);
    eColorModel GetColorModel() const;
    eColorChannel GetColorChannel() const;

    void RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void RecvPickerMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvPickerMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvPickerMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void RecvCheckColorModel (bool, eColorModel, eColorChannel);

    sigc::signal< void, ColorEditor * > sigChange;
  private:
    void DrawBaseChannelMarker (GraphicsEngine &GfxContext);
    void DrawRGB (GraphicsEngine &GfxContext, bool force_draw);
    void DrawHSV (GraphicsEngine &GfxContext, bool force_draw);

    eColorChannel   m_ColorChannel;
    eColorModel     m_ColorModel;
    InputArea       *m_PickerArea;
    InputArea       *m_BaseChannelArea;
    InputArea       *m_ColorSquare;
    HLayout        *m_hlayout;
    VLayout        *ctrllayout;

    GLSh_ColorPicker *m_RedShader;
    GLSh_ColorPicker *m_GreenShader;
    GLSh_ColorPicker *m_BlueShader;

    GLSh_ColorPicker *m_HueShader;
    GLSh_ColorPicker *m_SaturationShader;
    GLSh_ColorPicker *m_ValueShader;

    Point m_MarkerPosition;
    Point m_VertMarkerPosition;

    float m_Red;
    float m_Green;
    float m_Blue;
    float m_Hue;
    float m_Saturation;
    float m_Value;


    HLayout *redlayout;
    HLayout *greenlayout;
    HLayout *bluelayout;

    Button *redcheck;
    EditTextBox *redtext;
    Button *greencheck;
    EditTextBox *greentext;
    Button *bluecheck;
    EditTextBox *bluetext;

    HLayout *huelayout;
    HLayout *saturationlayout;
    HLayout *valuelayout;

    Button *huecheck;
    EditTextBox *huetext;
    Button *saturationcheck;
    EditTextBox *saturationtext;
    Button *valuecheck;
    EditTextBox *valuetext;

    ToggleButton *OkButton;
    ToggleButton *CancelButton;
    //RadioButtonGroup *radiogroup;

    DoubleValidator m_Validator;

  };


}

#endif // COLOREDITOR_H

