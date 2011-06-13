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


#ifndef RGBGEVALUATOR_H
#define RGBGEVALUATOR_H

#include "NuxCore/Color.h"
#include "HexRegExpValidator.h"
#include "IntegerValidator.h"
#include "DoubleValidator.h"

namespace nux
{

  class ToggleButton;
  class VLayout;
  class HLayout;
  class EditTextBox;

  class RGBValuator : public View //public ValuatorAbstraction
  {
  public:
    RGBValuator(NUX_FILE_LINE_PROTO);
    RGBValuator(Color const& color, NUX_FILE_LINE_PROTO);
    /*!
        Create an initialize the widget with the appropriate color model and value.

        @param ColorModel The color model (CM_RGB, CM_HSV, CM_HLS)
        @param x Red if CM_RGB, Hue if CM_HSV, Hue if CM_HLS
        @param y Green if CM_RGB, Saturation if CM_HSV, Light if CM_HLS
        @param z Blue if CM_RGB, Value if CM_HSV, Saturation if CM_HLS
        @param alpha
    */
    RGBValuator(color::Model colorModel, float x, float y, float z, float alpha, NUX_FILE_LINE_PROTO);

    ~RGBValuator();

    // API
    void SetColorModel(color::Model cm);
    void SetColorFormat(color::Format cf);
    Color GetColor() const;
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void SetRGB (Color const& color);
    void SetRGB (float r, float g, float b);
    void SetAlpha (float alpha);
    void SetRGBA (Color const& color);
    void SetRGBA (float r, float g, float b, float a);

    void SetHSV (float h, float s, float v);
    void SetHLS (float h, float l, float s);

    // emitters
    void OnReceiveMouseDown_Red     (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseDown_Green   (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseDown_Blue    (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseDown_Alpha   (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseDrag_Red     (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseDrag_Green   (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseDrag_Blue    (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseDrag_Alpha   (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void OnChangeColorModel();
    void OnChangeColorFormat();
    void RecvMouseDownColorModel (int x, int y, unsigned long button_flags, unsigned long key_flags);

    void OnReceiveMouseUp_Red       (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseUp_Green     (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnReceiveMouseUp_Blue      (int x, int y, unsigned long button_flags, unsigned long key_flags);

    void OnComponentInput           (EditTextBox *textbox, const NString &s, int componentIndex);
    void OnChannelKeyboardFocus();
    void OnChannelLostKeyboardFocus();
    void OnChannelValidateKeyboardEntry();
    void OnChannelCharacter (unsigned int character, int componentIndex);

    //! Cause the widget to emit sigColorChanged signal.
    /*!
        Cause the widget to emit sigColorChanged signal.
        Useful for initialization of the signal receivers.
    */
    void EmitColorChangedSignal();

    // signals
    //sigc::signal<void, float, float, float> sigSetRGB;
    sigc::signal<void, float, float, float, float> sigColorChanged;

  protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

  private:
    void DrawRedMarker (GraphicsEngine &GfxContext);
    void DrawGreenMarker (GraphicsEngine &GfxContext);
    void DrawBlueMarker (GraphicsEngine &GfxContext);
    void DrawAlphaMarker (GraphicsEngine &GfxContext);

    void DrawRGB (GraphicsEngine &GfxContext);
    void DrawHSV (GraphicsEngine &GfxContext);
    void DrawHLS (GraphicsEngine &GfxContext);

    HLayout *hlayout;
    HLayout *redlayout;
    HLayout *greenlayout;
    HLayout *bluelayout;
    HLayout *alphalayout;
    VLayout *vlayout;
    VLayout *colormodel_layout;

    EditTextBox *m_RedCaption;
    EditTextBox *m_GreenCaption;
    EditTextBox *m_BlueCaption;
    EditTextBox *m_AlphaCaption;
    InputArea *m_RedValuator;
    InputArea *m_GreenValuator;
    InputArea *m_BlueValuator;
    InputArea *m_AlphaValuator;
    InputArea *m_ColorSquare;

    InputArea *m_ComponentLabel0;
    InputArea *m_ComponentLabel1;
    InputArea *m_ComponentLabel2;
    InputArea *m_ComponentAlpha;

    TextureLayer *m_CheckboardLayer;

    color::RedGreenBlue rgb_;
    color::HueSaturationValue hsv_;
    color::HueLightnessSaturation hls_;
    float alpha_;

    ToggleButton *m_ColorModel;
    ToggleButton *m_ColorFormat;

    color::Model m_color_model;
    color::Format m_color_format;

    HexRegExpValidator m_HexRegExp;
    IntegerValidator m_IntRegExp;
    DoubleValidator m_DoubleRegExp;
    virtual long ComputeChildLayout();
  };

}

#endif // RGBGEVALUATOR_H


