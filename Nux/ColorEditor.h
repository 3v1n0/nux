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


#ifndef COLOREDITOR_H
#define COLOREDITOR_H

#include "NuxGraphics/GLDeviceFactory.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "NuxGraphics/GLSh_DrawFunction.h"
#include "DoubleValidator.h"
#include "TimerProc.h"

NAMESPACE_BEGIN_GUI

class GLSh_ColorPicker;
class HLayout;
class VLayout;
class EditTextBox;
class CheckBox;
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

    void RecvDialogOk(const weaksmptr(ColorEditor) coloreditor);
    void RecvDialogCancel(const weaksmptr(ColorEditor) coloreditor);
    void RecvDialogChange(const weaksmptr(ColorEditor) coloreditor);

    void Start();
    bool IsActive();
    void StopThreadMonitoring();
    
    void SetColor(Color color);
    Color GetColor();
    void SetPreviousColor(Color color);
    Color GetPreviousColor();
    void SetColorModel(eColorModel color_model);
    eColorModel GetColorModel();
    void SetColorChannel(eColorChannel color_model);
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
    NThread* m_Thread;

    friend class ColorPreview;
};

class ColorEditor : public ActiveInterfaceObject
{
public:
    ColorEditor();
    ~ColorEditor();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);


    void SetRed(double r);
    void SetGreen(double g);
    void SetBlue(double b);
    void SetHue(double h);
    void SetSaturation(double s);
    void SetValue(double v);

    void SetRGB(double r, double g, double b);
    void SetHSV(double h, double s, double v);
    void SetRGB(Color rgb);
    void SetHSV(Color hsv);
    Color GetRGBColor() const;
    Color GetHSVColor() const;

    void SetColorModel(eColorModel, eColorChannel);
    eColorModel GetColorModel() const;
    eColorChannel GetColorChannel() const;

    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void RecvPickerMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvPickerMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvPickerMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void RecvCheckColorModel(bool, eColorModel, eColorChannel);

    sigc::signal< void, const weaksmptr(ColorEditor) > sigChange;
private:
    void DrawBaseChannelMarker(GraphicsContext& GfxContext);
    void DrawRGB(GraphicsContext& GfxContext, bool force_draw);
    void DrawHSV(GraphicsContext& GfxContext, bool force_draw);

    eColorChannel m_ColorChannel;
    eColorModel m_ColorModel;
    smptr(CoreArea) m_PickerArea;
    smptr(CoreArea) m_BaseChannelArea;
    smptr(CoreArea) m_ColorSquare;
    smptr(HLayout) m_hlayout;
    smptr(VLayout) ctrllayout;

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


    smptr(HLayout) redlayout;
    smptr(HLayout) greenlayout;
    smptr(HLayout) bluelayout;

    smptr(RadioButton) redcheck;
    smptr(EditTextBox) redtext;
    smptr(RadioButton) greencheck;
    smptr(EditTextBox) greentext;
    smptr(RadioButton) bluecheck;
    smptr(EditTextBox) bluetext;

    smptr(HLayout) huelayout;
    smptr(HLayout) saturationlayout;
    smptr(HLayout) valuelayout;

    smptr(RadioButton) huecheck;
    smptr(EditTextBox) huetext;
    smptr(RadioButton) saturationcheck;
    smptr(EditTextBox) saturationtext;
    smptr(RadioButton) valuecheck;
    smptr(EditTextBox) valuetext;

    smptr(Button) OkButton;
    smptr(Button) CancelButton;
    RadioButtonGroup* radiogroup;

    DoubleValidator m_Validator;

};


NAMESPACE_END_GUI

#endif // COLOREDITOR_H

