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
  class AbstractButton;
  class RadioButton;
  class RadioButtonGroup;
  class Button;

  class ColorEditor;
  class ColorPreview;

  class ColorDialogProxy
  {
  public:
    ColorDialogProxy(bool ModalWindow);
    ~ColorDialogProxy();

    void RecvDialogOk(ColorEditor *coloreditor);
    void RecvDialogCancel(ColorEditor *coloreditor);
    void RecvDialogChange(ColorEditor *coloreditor);

    void Start();
    bool IsActive();
    void StopThreadMonitoring();

    void SetColor(Color color);
    Color GetColor();
    void SetPreviousColor(Color color);
    Color GetPreviousColor();
    void SetColorModel(color::Model color_model);
    color::Model GetColorModel();
    void SetColorChannel(color::Channel color_model);
    color::Channel GetColorChannel();

  private:
    bool m_bDialogChange;
    bool m_bDialogRunning;
    unsigned int m_DialogThreadID;
    Color m_RGBColor;
    Color m_PreviousRGBColor;
    color::Model m_ColorModel;
    color::Channel color_channel_;
    bool m_ModalWindow;
    NThread *m_Thread;

    friend class ColorPreview;
  };

  class ColorEditor : public View
  {
  public:
    ColorEditor(NUX_FILE_LINE_PROTO);
    ~ColorEditor();

    void SetRed(double r);
    void SetGreen(double g);
    void SetBlue(double b);
    void SetHue(double h);
    void SetSaturation(double s);
    void SetValue(double v);

    void SetRGB(double r, double g, double b);
    void SetHSV(double h, double s, double v);
    void SetRGB(Color const& rgb);
    Color GetRGBColor() const;

    void SetColorModel(color::Model, color::Channel);
    color::Model GetColorModel() const;
    color::Channel GetColorChannel() const;

    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void RecvPickerMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvPickerMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvPickerMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void RecvCheckColorModel(bool, color::Model, color::Channel);
    void RecvCheckColorModel0(AbstractButton *button, color::Model color_mode, color::Channel channel);

    sigc::signal< void, ColorEditor * > sigChange;

  protected:
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void PreLayoutManagement();
    virtual bool AcceptKeyNavFocus();
  private:

    //! Override of Area::SetMinimumHeight and made private.
    /*!
        Prevent changing the minimum height of the ColorEditor view.
    */
    virtual void SetMinimumHeight(int h){};

    //! Override of Area::SetMaximumHeight and made private.
    /*!
        Prevent changing the maximum height of the ColorEditor view.
    */
    virtual void SetMaximumHeight(int h){};

    //! Override of Area::SetMinimumWidth and made private.
    /*!
        Prevent changing the minimum width of the ColorEditor view.
    */
    virtual void SetMinimumWidth(int w){};

    //! Override of Area::SetMaximumWidth and made private.
    /*!
        Prevent changing the maximum width of the ColorEditor view.
    */
    virtual void SetMaximumWidth(int w){};

    void DrawBaseChannelMarker(GraphicsEngine &graphics_engine);
    void DrawRGB(GraphicsEngine &graphics_engine, bool force_draw);
    void DrawHSV(GraphicsEngine &graphics_engine, bool force_draw);

    color::Channel   color_channel_;
    color::Model     m_ColorModel;
    InputArea       *picker_area_;
    InputArea       *channel_area_;
    InputArea       *selected_color_area_;
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

    color::RedGreenBlue rgb_;
    color::HueSaturationValue hsv_;

    HLayout *redlayout;
    HLayout *greenlayout;
    HLayout *bluelayout;

    RadioButton *redcheck;
    EditTextBox *redtext;
    RadioButton *greencheck;
    EditTextBox *greentext;
    RadioButton *bluecheck;
    EditTextBox *bluetext;

    HLayout *huelayout;
    HLayout *saturationlayout;
    HLayout *valuelayout;

    RadioButton *huecheck;
    EditTextBox *hue_text_entry_;
    RadioButton *saturationcheck;
    EditTextBox *saturation_text_entry_;
    RadioButton *valuecheck;
    EditTextBox *value_text_entry_;

    RadioButtonGroup *radiogroup;

    DoubleValidator m_Validator;

    static Size picker_area_size;
    static int channel_area_width;
  };


}

#endif // COLOREDITOR_H

