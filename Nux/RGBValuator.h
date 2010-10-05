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


#ifndef RGBGEVALUATOR_H
#define RGBGEVALUATOR_H

#include "HexRegExpValidator.h"
#include "IntegerValidator.h"
#include "DoubleValidator.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  class NTexture2D;
  class Button;
  class VLayout;
  class HLayout;
  class EditTextBox;

  class RGBValuator : public View //public ValuatorAbstraction
  {
  public:

    enum ColorFormat
    {
      COLORFORMAT_FLOAT = 0,
      COLORFORMAT_HEX,
      COLORFORMAT_INT,
    };

    RGBValuator (NUX_FILE_LINE_PROTO);
    RGBValuator (Color color, NUX_FILE_LINE_PROTO);
    /*!
        Create an initialize the widget with the CM_RGB color model and value.

        @param red
        @param green
        @param blue
        @param alpha
    */
    RGBValuator (float red, float green, float blue, float alpha, NUX_FILE_LINE_PROTO);
    /*!
        Create an initialize the widget with the appropriate color model and value.

        @param ColorModel The color model (CM_RGB, CM_HSV, CM_HLS)
        @param x Red if CM_RGB, Hue if CM_HSV, Hue if CM_HLS
        @param y Green if CM_RGB, Saturation if CM_HSV, Light if CM_HLS
        @param z Blue if CM_RGB, Value if CM_HSV, Saturation if CM_HLS
        @param alpha
    */
    RGBValuator (eColorModel colorModel, float x, float y, float z, float alpha, NUX_FILE_LINE_PROTO);

    ~RGBValuator();

    // API
    void SetColorModel (eColorModel cm);
    void SetColorFormat (ColorFormat cf);
    Color GetColor();
    float GetRed();
    float GetGreen();
    float GetBlue();
    float GetAlpha();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual void Draw (GraphicsContext &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsContext &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsContext &GfxContext, bool force_draw);

    void SetRGB (Color color);
    void SetRGB (float r, float g, float b);
    void SetAlpha (float alpha);
    void SetRGBA (Color color);
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
    void DrawRedMarker (GraphicsContext &GfxContext);
    void DrawGreenMarker (GraphicsContext &GfxContext);
    void DrawBlueMarker (GraphicsContext &GfxContext);
    void DrawAlphaMarker (GraphicsContext &GfxContext);

    void DrawRGB (GraphicsContext &GfxContext);
    void DrawHSV (GraphicsContext &GfxContext);
    void DrawHLS (GraphicsContext &GfxContext);

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
    CoreArea *m_RedValuator;
    CoreArea *m_GreenValuator;
    CoreArea *m_BlueValuator;
    CoreArea *m_AlphaValuator;
    CoreArea *m_ColorSquare;

    CoreArea *m_ComponentLabel0;
    CoreArea *m_ComponentLabel1;
    CoreArea *m_ComponentLabel2;
    CoreArea *m_ComponentAlpha;

    TextureLayer *m_CheckboardLayer;
    float m_Red;
    float m_Green;
    float m_Blue;
    float m_Alpha;

    float m_HSVHue;
    float m_HSVSaturation;
    float m_HSVValue;

    float m_HLSHue;
    float m_HLSLight;
    float m_HLSSaturation;

    Button *m_ColorModel;
    Button *m_ColorFormat;

    enum eColorModel m_color_model;
    enum ColorFormat m_color_format;

    HexRegExpValidator m_HexRegExp;
    IntegerValidator m_IntRegExp;
    DoubleValidator m_DoubleRegExp;
    virtual long ComputeChildLayout();
  };

} //NUX_NAMESPACE_END

#endif // RGBGEVALUATOR_H


