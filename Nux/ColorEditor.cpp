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

#include "NuxGraphics/GLSh_ColorPicker.h"
#include "VLayout.h"
#include "HLayout.h"
#include "CheckBox.h"
#include "EditTextBox.h"
#include "RadioButton.h"
#include "RadioButtonGroup.h"
#include "ToggleButton.h"
#include "Layout.h"
#include "ColorEditor.h"

namespace nux
{
  Size ColorEditor::picker_area_size = Size(200, 200);
  int ColorEditor::channel_area_width = 12;

  static void ThreadColorEditorDialog(NThread *thread, void *InitData)
  {
    VLayout *MainLayout(new VLayout(NUX_TRACKER_LOCATION));
    ColorEditor *coloreditor(new ColorEditor());
    coloreditor->ComputeContentSize(); // necessary so all element of the widget get their rightful size.
    ColorDialogProxy *coloreditorproxy = static_cast<ColorDialogProxy *> (InitData);

    if (coloreditorproxy)
    {
      coloreditor->SetRGB(coloreditorproxy->GetColor());
      coloreditor->SetColorModel(coloreditorproxy->GetColorModel(), coloreditorproxy->GetColorChannel());
      coloreditor->sigChange.connect(sigc::mem_fun(coloreditorproxy, &ColorDialogProxy::RecvDialogChange));
    }

    HLayout *ButtonLayout(new HLayout("Dialog Buttons", NUX_TRACKER_LOCATION));

    ToggleButton *OkButton(new ToggleButton("OK", NUX_TRACKER_LOCATION));
    OkButton->SetMinimumWidth(60);
    OkButton->SetMinimumHeight(20);

    ToggleButton *CancelButton(new ToggleButton("Cancel", NUX_TRACKER_LOCATION));
    CancelButton->SetMinimumWidth(60);
    CancelButton->SetMinimumHeight(20);

    //FIXME - OkButton->sigClick.connect(sigc::mem_fun(static_cast<WindowThread *> (thread), &WindowThread::TerminateThread));
    //FIXME - OkButton->sigClick.connect(sigc::bind(sigc::mem_fun(coloreditorproxy, &ColorDialogProxy::RecvDialogOk), coloreditor));
    //FIXME - CancelButton->sigClick.connect(sigc::bind(sigc::mem_fun(coloreditorproxy, &ColorDialogProxy::RecvDialogCancel), coloreditor));
    //FIXME - CancelButton->sigClick.connect(sigc::mem_fun(static_cast<WindowThread *> (thread), &WindowThread::TerminateThread));

    ButtonLayout->SetHorizontalInternalMargin(6);
    ButtonLayout->SetVerticalExternalMargin(2);
    ButtonLayout->AddView(OkButton, 0);
    ButtonLayout->AddView(CancelButton, 0);

    MainLayout->AddView(coloreditor);
    MainLayout->AddLayout(ButtonLayout, 0);
    static_cast<WindowThread *> (thread)->SetLayout(MainLayout);

    MainLayout->SetBaseWidth(1);
    MainLayout->SetBaseHeight(1);
    MainLayout->ComputeContentSize();
    static_cast<WindowThread *> (thread)->SetWindowSize(MainLayout->GetBaseWidth(), MainLayout->GetBaseHeight());

    // Call StopThreadMonitoring in case the dialog was close by clicking the window close button.
    //coloreditorproxy->StopThreadMonitoring();
  }

  ColorDialogProxy::ColorDialogProxy(bool ModalWindow)
  {
    m_bDialogChange     = false;
    m_bDialogRunning    = false;
    m_ModalWindow       = ModalWindow;
    m_RGBColor          = Color(1.0f, 1.0f, 1.0f, 1.0f);
    m_ColorModel        = color::RGB;
    color_channel_      = color::RED;
  }

  ColorDialogProxy::~ColorDialogProxy()
  {
  }

  void ColorDialogProxy::Start()
  {
    m_PreviousRGBColor = m_RGBColor;

    int Width = 290;
    int Height = 230;
    m_Thread = CreateModalWindowThread(WINDOWSTYLE_TOOL, "Color Editor", Width, Height, GetWindowThread(),
                                        ThreadColorEditorDialog,
                                        this);

    if (m_Thread)
    {
      //todo(jaytaoko): m_DialogThreadID = m_Thread->GetThreadId();
      m_Thread->Start(0);
    }

    m_bDialogRunning = true;
  }

  bool ColorDialogProxy::IsActive()
  {
    return (m_Thread && (m_Thread->GetThreadState() != THREADSTOP) && m_bDialogRunning);
  }

  void ColorDialogProxy::RecvDialogOk(ColorEditor *coloreditor)
  {
    m_RGBColor = coloreditor->GetRGBColor();
    m_PreviousRGBColor = m_RGBColor;
    m_bDialogChange = true;
    m_bDialogRunning = false;
  }

  void ColorDialogProxy::RecvDialogCancel(ColorEditor* /* coloreditor */)
  {
    m_RGBColor = m_PreviousRGBColor;
    m_bDialogChange = true;
    m_bDialogRunning = false;
  }

  void ColorDialogProxy::RecvDialogChange(ColorEditor *coloreditor)
  {
    m_RGBColor = coloreditor->GetRGBColor();
    m_bDialogChange = true;
  }

  void ColorDialogProxy::StopThreadMonitoring()
  {
    m_RGBColor = m_PreviousRGBColor;
    m_bDialogChange = true;
    m_bDialogRunning = false;
    m_Thread = 0;
    m_DialogThreadID = 0;
  }

  void ColorDialogProxy::SetColor(Color color)
  {
    m_RGBColor = color;
  }

  Color ColorDialogProxy::GetColor()
  {
    return m_RGBColor;
  }

  void ColorDialogProxy::SetPreviousColor(Color color)
  {
    m_PreviousRGBColor = color;
  }

  Color ColorDialogProxy::GetPreviousColor()
  {
    return m_PreviousRGBColor;
  }

  void ColorDialogProxy::SetColorModel(color::Model color_model)
  {
    m_ColorModel = color_model;
  }

  color::Model ColorDialogProxy::GetColorModel()
  {
    return m_ColorModel;
  }

  void ColorDialogProxy::SetColorChannel(color::Channel color_channel)
  {
    color_channel_ = color_channel;
  }

  color::Channel ColorDialogProxy::GetColorChannel()
  {
    return color_channel_;
  }

  ColorEditor::ColorEditor(NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , rgb_(1.0f, 1.0f, 0.0f)
    , hsv_(rgb_)
  {
    m_ColorModel = color::RGB;
    color_channel_ = color::RED;
    m_MarkerPosition = Point(0, 0);
    m_VertMarkerPosition = Point(0, 0);

    m_Validator.SetMinimum(0.0);
    m_Validator.SetMaximum(1.0);
    m_Validator.SetDecimals(2);

    picker_area_        = new BasicView(NUX_TRACKER_LOCATION);
    channel_area_   = new BasicView(NUX_TRACKER_LOCATION);
    selected_color_area_       = new BasicView(NUX_TRACKER_LOCATION);
    m_hlayout           = new HLayout(NUX_TRACKER_LOCATION);

    channel_area_->mouse_down.connect(sigc::mem_fun(this, &ColorEditor::RecvMouseDown));
    channel_area_->mouse_up.connect(sigc::mem_fun(this, &ColorEditor::RecvMouseUp));
    channel_area_->mouse_drag.connect(sigc::mem_fun(this, &ColorEditor::RecvMouseDrag));

    picker_area_->mouse_down.connect(sigc::mem_fun(this, &ColorEditor::RecvPickerMouseDown));
    picker_area_->mouse_up.connect(sigc::mem_fun(this, &ColorEditor::RecvPickerMouseUp));
    picker_area_->mouse_drag.connect(sigc::mem_fun(this, &ColorEditor::RecvPickerMouseDrag));

    selected_color_area_->SetMinMaxSize(62, 32);
    picker_area_->SetMinMaxSize(picker_area_size.width, picker_area_size.height);
    channel_area_->SetMaximumHeight(picker_area_size.height);
    channel_area_->SetMinimumWidth(channel_area_width);
    channel_area_->SetMaximumWidth(channel_area_width);

    m_hlayout->AddView(picker_area_, 1);
    m_hlayout->AddLayout(new SpaceLayout(5, 5, 20, 20), 0);
    m_hlayout->AddView(channel_area_, 0, eAbove, eFull);
    SetCompositionLayout(m_hlayout);

    // RGB
    {
      redlayout = new HLayout(NUX_TRACKER_LOCATION);
      {
        //FIXME - change to radio button
        redcheck = new RadioButton("R");
        redcheck->SetMinimumWidth(32);
        redtext = new EditTextBox("", NUX_TRACKER_LOCATION);
        redtext->SetMinimumWidth(36);
        redlayout->AddView(redcheck, 0);
        redlayout->AddView(redtext, 0);
        redcheck->click.connect(sigc::bind(sigc::bind(sigc::mem_fun(this, &ColorEditor::RecvCheckColorModel0), color::RED), color::RGB));
      }
      greenlayout = new HLayout(NUX_TRACKER_LOCATION);
      {
        //FIXME - Change to radio button
        greencheck = new RadioButton("G");
        greencheck->SetMinimumWidth(32);
        greentext = new EditTextBox("", NUX_TRACKER_LOCATION);
        greentext->SetMinimumWidth(36);
        greenlayout->AddView(greencheck, 0);
        greenlayout->AddView(greentext, 0);
        greencheck->click.connect(sigc::bind(sigc::bind(sigc::mem_fun(this, &ColorEditor::RecvCheckColorModel0), color::GREEN), color::RGB));

      }
      bluelayout = new HLayout(NUX_TRACKER_LOCATION);
      {
        //FIXME - change to radio button
        bluecheck = new RadioButton("B");
        bluecheck->SetMinimumWidth(32);
        bluetext = new EditTextBox("", NUX_TRACKER_LOCATION);
        bluetext->SetMinimumWidth(36);
        bluelayout->AddView(bluecheck, 0);
        bluelayout->AddView(bluetext, 0);
        bluecheck->click.connect(sigc::bind(sigc::bind(sigc::mem_fun(this, &ColorEditor::RecvCheckColorModel0), color::BLUE), color::RGB));
      }
    }

    // HSV
    {
      huelayout = new HLayout(NUX_TRACKER_LOCATION);
      {
        huecheck = new RadioButton("H");
        huecheck->SetMinimumWidth(32);
        hue_text_entry_ = new EditTextBox("", NUX_TRACKER_LOCATION);
        hue_text_entry_->SetMinimumWidth(36);
        huelayout->AddView(huecheck, 0);
        huelayout->AddView(hue_text_entry_, 0);
        huecheck->click.connect(sigc::bind(sigc::bind(sigc::mem_fun(this, &ColorEditor::RecvCheckColorModel0), color::HUE), color::HSV ));
      }
      saturationlayout = new HLayout(NUX_TRACKER_LOCATION);
      {
        saturationcheck = new RadioButton("S");
        saturationcheck->SetMinimumWidth(32);
        saturation_text_entry_ = new EditTextBox("", NUX_TRACKER_LOCATION);
        saturation_text_entry_->SetMinimumWidth(36);
        saturationlayout->AddView(saturationcheck, 0);
        saturationlayout->AddView(saturation_text_entry_, 0);
        saturationcheck->click.connect(sigc::bind(sigc::bind(sigc::mem_fun(this, &ColorEditor::RecvCheckColorModel0), color::SATURATION), color::HSV ));
      }
      valuelayout = new HLayout(NUX_TRACKER_LOCATION);
      {
        valuecheck = new RadioButton("V");
        valuecheck->SetMinimumWidth(32);
        value_text_entry_ = new EditTextBox("", NUX_TRACKER_LOCATION);
        value_text_entry_->SetMinimumWidth(36);
        valuelayout->AddView(valuecheck, 0);
        valuelayout->AddView(value_text_entry_, 0);
        valuecheck->click.connect(sigc::bind(sigc::bind(sigc::mem_fun(this, &ColorEditor::RecvCheckColorModel0), color::VALUE), color::HSV ));
      }
    }

    ctrllayout = new VLayout(NUX_TRACKER_LOCATION);
    ctrllayout->AddView(selected_color_area_);
    ctrllayout->AddView(new SpaceLayout(20, 20, 10, 10), 1);
    ctrllayout->AddLayout(redlayout, 0);
    ctrllayout->AddLayout(greenlayout, 0);
    ctrllayout->AddLayout(bluelayout, 0);
    ctrllayout->AddLayout(new SpaceLayout(10, 10, 10, 10));
    ctrllayout->AddLayout(huelayout, 0);
    ctrllayout->AddLayout(saturationlayout, 0);
    ctrllayout->AddLayout(valuelayout, 0);
    ctrllayout->SetHorizontalExternalMargin(2);
    ctrllayout->SetVerticalInternalMargin(2);

    m_hlayout->AddLayout(ctrllayout, 0);

    radiogroup = new RadioButtonGroup(NUX_TRACKER_LOCATION);
    radiogroup->ConnectButton(redcheck);
    radiogroup->ConnectButton(greencheck);
    radiogroup->ConnectButton(bluecheck);
    radiogroup->ConnectButton(huecheck);
    radiogroup->ConnectButton(saturationcheck);
    radiogroup->ConnectButton(valuecheck);

    m_RedShader = new GLSh_ColorPicker(color::RED);
    m_GreenShader = new GLSh_ColorPicker(color::GREEN);
    m_BlueShader = new GLSh_ColorPicker(color::BLUE);
    m_HueShader = new GLSh_ColorPicker(color::HUE);
    m_SaturationShader = new GLSh_ColorPicker(color::SATURATION);
    m_ValueShader = new GLSh_ColorPicker(color::VALUE);

    redtext->SetText(m_Validator.ToString(255 * rgb_.red));
    greentext->SetText(m_Validator.ToString(255 * rgb_.green));
    bluetext->SetText(m_Validator.ToString(255 * rgb_.blue));
    hue_text_entry_->SetText(m_Validator.ToString(360 * hsv_.hue));
    saturation_text_entry_->SetText(m_Validator.ToString(100 * hsv_.saturation));
    value_text_entry_->SetText(m_Validator.ToString(100 * hsv_.value));
  }

  ColorEditor::~ColorEditor()
  {
    delete m_RedShader;
    delete m_GreenShader;
    delete m_BlueShader;
    delete m_HueShader;
    delete m_SaturationShader;
    delete m_ValueShader;
    radiogroup->UnReference();
  }

  void ColorEditor::PreLayoutManagement()
  {
    View::PreLayoutManagement();
    if (view_layout_)
    {
      // Constrain the vertical expansion of the color picker.
      view_layout_->SetBaseWidth(1);
      view_layout_->SetBaseHeight(1);
    }
  }

  void ColorEditor::Draw(GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();

    base.OffsetPosition(1, 1);
    base.OffsetSize(-2, -2);

    graphics_engine.PushClippingRectangle(base);
    GetPainter().PushDrawShapeLayer(graphics_engine, base, eSHAPE_CORNER_ROUND4, Color(0xFF000000), eAllCorners, true);

    if (m_ColorModel == color::RGB)
    {
      DrawRGB(graphics_engine, force_draw);
    }
    else
    {
      DrawHSV(graphics_engine, force_draw);
    }

    GetPainter().PopBackground();
    graphics_engine.PopClippingRectangle();
  }

  void ColorEditor::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();
    GetPainter().PushShapeLayer(graphics_engine, base, eSHAPE_CORNER_ROUND4, Color(0xFF000000), eAllCorners, true);

    bool force = force_draw || IsFullRedraw();
    redcheck->ProcessDraw(graphics_engine, force);
    redtext->ProcessDraw(graphics_engine, force);
    greencheck->ProcessDraw(graphics_engine, force);
    greentext->ProcessDraw(graphics_engine, force);
    bluecheck->ProcessDraw(graphics_engine, force);
    bluetext->ProcessDraw(graphics_engine, force);

    huecheck->ProcessDraw(graphics_engine, force);
    hue_text_entry_->ProcessDraw(graphics_engine, force);
    saturationcheck->ProcessDraw(graphics_engine, force);
    saturation_text_entry_->ProcessDraw(graphics_engine, force);
    valuecheck->ProcessDraw(graphics_engine, force);
    value_text_entry_->ProcessDraw(graphics_engine, force);

    GetPainter().PopBackground();
  }

// Draw Marker on Base Chanel Area
  void ColorEditor::DrawBaseChannelMarker(GraphicsEngine &graphics_engine)
  {
    int marker_position_x;
    int marker_position_y;

    graphics_engine.PushClippingRectangle(channel_area_->GetGeometry());

    marker_position_x = channel_area_->GetBaseX();
    marker_position_y = channel_area_->GetBaseY() + m_VertMarkerPosition.y;
    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y,
                                  marker_position_x, marker_position_y + 5, Color(0.0f, 0.0f, 0.0f, 1.0f));

    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y,
                                  marker_position_x, marker_position_y + 4, Color(0.7f, 0.7f, 0.7f, 1.0f));

    marker_position_x = channel_area_->GetBaseX() + channel_area_->GetBaseWidth();
    marker_position_y = channel_area_->GetBaseY() + m_VertMarkerPosition.y;
    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x, marker_position_y - 5,
                                  marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y + 5, Color(0.0f, 0.0f, 0.0f, 1.0f));

    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x, marker_position_y - 4,
                                  marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y + 4, Color(0.7f, 0.7f, 0.7f, 1.0f));
    graphics_engine.PopClippingRectangle();
  }

  void ColorEditor::DrawRGB(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    if (m_ColorModel == color::RGB)
    {
      GetPainter().Paint2DQuadColor(graphics_engine, selected_color_area_->GetGeometry(), Color(rgb_));
      Color BaseChannelTop;
      Color BaseChannelBottom;

      if (color_channel_ == color::RED)
      {
        m_RedShader->SetColor(rgb_.red, rgb_.green, rgb_.blue, 1.0f);
        m_RedShader->SetScreenPositionOffset(graphics_engine.GetViewportX(), graphics_engine.GetViewportY());
        BaseChannelTop = Color(1.0f, rgb_.green, rgb_.blue, 1.0f);
        BaseChannelBottom = Color(0.0f, rgb_.green, rgb_.blue, 1.0f);
        m_RedShader->Render(
          picker_area_->GetBaseX(),
          picker_area_->GetBaseY(),
          0,
          picker_area_->GetBaseWidth(),
          picker_area_->GetBaseHeight(),
          graphics_engine.GetViewportWidth(), graphics_engine.GetViewportHeight()
        );
      }
      else if (color_channel_ == color::GREEN)
      {
        m_GreenShader->SetColor(rgb_.red, rgb_.green, rgb_.blue, 1.0f);
        m_GreenShader->SetScreenPositionOffset(graphics_engine.GetViewportX(), graphics_engine.GetViewportY());
        BaseChannelTop = Color(rgb_.red, 1.0f, rgb_.blue, 1.0f);
        BaseChannelBottom = Color(rgb_.red, 0.0f, rgb_.blue, 1.0f);
        m_GreenShader->Render(
          picker_area_->GetBaseX(),
          picker_area_->GetBaseY(),
          0,
          picker_area_->GetBaseWidth(),
          picker_area_->GetBaseHeight(),
          graphics_engine.GetViewportWidth(), graphics_engine.GetViewportHeight()
        );
      }
      else if (color_channel_ == color::BLUE)
      {
        m_BlueShader->SetColor(rgb_.red, rgb_.green, rgb_.blue, 1.0f);
        m_BlueShader->SetScreenPositionOffset(graphics_engine.GetViewportX(), graphics_engine.GetViewportY());
        BaseChannelTop = Color(rgb_.red, rgb_.green, 1.0f, 1.0f);
        BaseChannelBottom = Color(rgb_.red, rgb_.green, 0.0f, 1.0f);
        m_BlueShader->Render(
          picker_area_->GetBaseX(),
          picker_area_->GetBaseY(),
          0,
          picker_area_->GetBaseWidth(),
          picker_area_->GetBaseHeight(),
          graphics_engine.GetViewportWidth(), graphics_engine.GetViewportHeight()
        );
      }

      Geometry pickermarker = Geometry(GetBaseX() + m_MarkerPosition.x - 2, GetBaseY() + m_MarkerPosition.y - 2, 5, 5);

      Color color(rgb_.red, rgb_.green, rgb_.blue);
      float luma = color::LumaRed * rgb_.red + color::LumaGreen * rgb_.green + color::LumaBlue * rgb_.blue;
      Color one_minus_luma(1.0f - luma, 1.0f - luma, 1.0f - luma);

      GetPainter().Paint2DQuadWireframe(graphics_engine, pickermarker, one_minus_luma);

      GetPainter().Paint2DQuadColor(graphics_engine, channel_area_->GetGeometry(), BaseChannelTop, BaseChannelBottom, BaseChannelBottom, BaseChannelTop);
      // Draw Marker on Base Chanel Area
      DrawBaseChannelMarker(graphics_engine);
    }
  }

  void ColorEditor::DrawHSV(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    if (m_ColorModel == color::HSV)
    {
      color::RedGreenBlue rgb(hsv_);
      GetPainter().Paint2DQuadColor(graphics_engine, selected_color_area_->GetGeometry(), Color(rgb));

      Color BaseChannelTop;
      Color BaseChannelBottom;

      if (color_channel_ == color::HUE)
      {
        m_HueShader->SetColor(hsv_.hue, hsv_.saturation, hsv_.value, 1.0f);
        m_HueShader->SetScreenPositionOffset(graphics_engine.GetViewportX(), graphics_engine.GetViewportY());
        m_HueShader->Render(
          picker_area_->GetBaseX(),
          picker_area_->GetBaseY(),
          0,
          picker_area_->GetBaseWidth(),
          picker_area_->GetBaseHeight(),
          graphics_engine.GetViewportWidth(), graphics_engine.GetViewportHeight()
        );

        Geometry P = channel_area_->GetGeometry();

        float s = 1.0f - 1.0f;
        float v = 1.0f;
        float fw = P.GetHeight() / 6;

        Geometry p = Geometry(P.x, P.y, P.GetWidth(), fw);
        GetPainter().Paint2DQuadVGradient(graphics_engine, p, Color(1.0f * v, s * v, s * v), Color(1.0f * v, s * v, 1.0f * v));
        p.SetY(P.y + fw);
        GetPainter().Paint2DQuadVGradient(graphics_engine, p, Color(1.0f * v, s * v, 1.0f * v), Color(s * v, s * v, 1.0f * v));
        p.SetY(P.y + 2 * fw);
        GetPainter().Paint2DQuadVGradient(graphics_engine, p, Color(s * v, s * v, 1.0f * v), Color(s * v, 1.0f * v, 1.0f * v));
        p.SetY(P.y + 3 * fw);
        GetPainter().Paint2DQuadVGradient(graphics_engine, p, Color(s * v, 1.0f * v, 1.0f * v), Color(s * v, 1.0f * v, s * v));
        p.SetY(P.y + 4 * fw);
        GetPainter().Paint2DQuadVGradient(graphics_engine, p, Color(s * v, 1.0f * v, s * v), Color(1.0f * v, 1.0f * v, s * v));
        p.SetY(P.y + 5 * fw);
        p.SetHeight(P.GetHeight() - 5 * fw); // correct rounding errors
        GetPainter().Paint2DQuadVGradient(graphics_engine, p, Color(1.0f * v, 1.0f * v, s * v), Color(1.0f * v, s * v, s * v));

        Geometry pickermarker = Geometry(GetBaseX() + m_MarkerPosition.x - 2, GetBaseY() + m_MarkerPosition.y - 2, 5, 5);

        float luma = color::LumaRed * rgb_.red + color::LumaGreen * rgb_.green + color::LumaBlue * rgb_.blue;
        Color one_minus_luma(1.0f - luma, 1.0f - luma, 1.0f - luma);
        GetPainter().Paint2DQuadWireframe(graphics_engine, pickermarker, one_minus_luma);
      }
      else if (color_channel_ == color::SATURATION)
      {
        float value = hsv_.value;
        if (value < 0.3f) value = 0.3f;

        m_SaturationShader->SetColor(hsv_.hue, hsv_.saturation, hsv_.value, 1.0f);
        m_SaturationShader->SetScreenPositionOffset(graphics_engine.GetViewportX(), graphics_engine.GetViewportY());
        BaseChannelTop = Color(color::RedGreenBlue(color::HueSaturationValue(hsv_.hue, 1.0f, value)));
        BaseChannelBottom = Color(value, value, value, 1.0f);
        m_SaturationShader->Render(
          picker_area_->GetBaseX(),
          picker_area_->GetBaseY(),
          0,
          picker_area_->GetBaseWidth(),
          picker_area_->GetBaseHeight(),
          graphics_engine.GetViewportWidth(), graphics_engine.GetViewportHeight()
        );

        //Geometry pickermarker = Geometry(GetX() + x - 2, GetY() + y -2, 5, 5);
        Geometry pickermarker = Geometry(GetBaseX() + m_MarkerPosition.x - 2, GetBaseY() + m_MarkerPosition.y - 2, 5, 5);

        float luma = color::LumaRed * rgb_.red + color::LumaGreen * rgb_.green + color::LumaBlue * rgb_.blue;
        Color one_minus_luma(1.0f - luma, 1.0f - luma, 1.0f - luma);

        GetPainter().Paint2DQuadWireframe(graphics_engine, pickermarker, one_minus_luma);
        GetPainter().Paint2DQuadColor(graphics_engine, channel_area_->GetGeometry(), BaseChannelTop, BaseChannelBottom, BaseChannelBottom, BaseChannelTop);
      }
      else if (color_channel_ == color::VALUE)
      {
        m_ValueShader->SetColor(hsv_.hue, hsv_.saturation, hsv_.value, 1.0f);
        m_ValueShader->SetScreenPositionOffset(graphics_engine.GetViewportX(), graphics_engine.GetViewportY());
        BaseChannelTop = Color(color::RedGreenBlue(color::HueSaturationValue(hsv_.hue, hsv_.saturation, 1.0f)));
        BaseChannelBottom = Color(color::RedGreenBlue(color::HueSaturationValue(hsv_.hue, hsv_.saturation, 0.0f)));
        m_ValueShader->Render(
          picker_area_->GetBaseX(),
          picker_area_->GetBaseY(),
          0,
          picker_area_->GetBaseWidth(),
          picker_area_->GetBaseHeight(),
          graphics_engine.GetViewportWidth(), graphics_engine.GetViewportHeight()
        );

        //Geometry pickermarker = Geometry(GetX() + x - 2, GetY() + y -2, 5, 5);
        Geometry pickermarker = Geometry(GetBaseX() + m_MarkerPosition.x - 2, GetBaseY() + m_MarkerPosition.y - 2, 5, 5);

        float luma = color::LumaRed * rgb_.red + color::LumaGreen * rgb_.green + color::LumaBlue * rgb_.blue;
        Color one_minus_luma(1.0f - luma, 1.0f - luma, 1.0f - luma);

        GetPainter().Paint2DQuadWireframe(graphics_engine, pickermarker, one_minus_luma);
        GetPainter().Paint2DQuadColor(graphics_engine, channel_area_->GetGeometry(), BaseChannelTop, BaseChannelBottom, BaseChannelBottom, BaseChannelTop);
      }

      // Draw Marker on Base Chanel Area
      DrawBaseChannelMarker(graphics_engine);
    }
  }

  void ColorEditor::RecvMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    float BaseValue;

    if (m_ColorModel == color::RGB)
    {
      if (y < 0)
        BaseValue = 1.0f;
      else if (y > channel_area_->GetBaseHeight())
        BaseValue = 0.0f;
      else
        BaseValue = 1.0f - (float) y / (float) channel_area_->GetBaseHeight();

      if (color_channel_ == color::RED)
        rgb_.red = BaseValue;
      else if (color_channel_ == color::GREEN)
        rgb_.green = BaseValue;
      else if (color_channel_ == color::BLUE)
        rgb_.blue = BaseValue;

      hsv_ = color::HueSaturationValue(rgb_);
    }

    if (m_ColorModel == color::HSV)
    {
      if (y < 0)
        BaseValue = 1.0f;
      else if (y > channel_area_->GetBaseHeight())
        BaseValue = 0.0f;
      else
        BaseValue = 1.0f - (float) y / (float) channel_area_->GetBaseHeight();

      if (color_channel_ == color::HUE)
      {
        hsv_.hue = BaseValue;

        if (hsv_.hue >= 1.0f) hsv_.hue = 0.0f;
      }
      else if (color_channel_ == color::SATURATION)
        hsv_.saturation = BaseValue;
      else if (color_channel_ == color::VALUE)
        hsv_.value = BaseValue;

      rgb_ = color::RedGreenBlue(hsv_);
    }

    redtext->SetText(m_Validator.ToString(255 * rgb_.red));
    greentext->SetText(m_Validator.ToString(255 * rgb_.green));
    bluetext->SetText(m_Validator.ToString(255 * rgb_.blue));
    hue_text_entry_->SetText(m_Validator.ToString(360 * hsv_.hue));
    saturation_text_entry_->SetText(m_Validator.ToString(100 * hsv_.saturation));
    value_text_entry_->SetText(m_Validator.ToString(100 * hsv_.value));
    m_VertMarkerPosition = Point(Clamp<int> (x, 0, channel_area_->GetBaseWidth() - 1), Clamp<int> (y, 0, channel_area_->GetBaseHeight() - 1));

    sigChange.emit(this);
    QueueDraw();
  }

  void ColorEditor::RecvMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void ColorEditor::RecvMouseDrag(int x, int y, int /* dx */, int /* dy */, unsigned long button_flags, unsigned long key_flags)
  {
    RecvMouseDown(x, y, button_flags, key_flags);
  }

  void ColorEditor::RecvPickerMouseDown(int x, int y, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_ColorModel == color::RGB)
    {
      if (color_channel_ == color::RED)
      {
        if (y < 0)
          rgb_.green = 1.0f;
        else if (y > picker_area_->GetBaseHeight())
          rgb_.green = 0.0f;
        else
          rgb_.green = 1.0f - (float) y / (float) picker_area_->GetBaseHeight();

        if (x < 0)
          rgb_.blue = 0.0f;
        else if (x > picker_area_->GetBaseWidth())
          rgb_.blue = 1.0f;
        else
          rgb_.blue = (float) x / (float) picker_area_->GetBaseWidth();

      }

      if (color_channel_ == color::GREEN)
      {
        if (y < 0)
          rgb_.red = 1.0f;
        else if (y > picker_area_->GetBaseHeight())
          rgb_.red = 0.0f;
        else
          rgb_.red = 1.0f - (float) y / (float) picker_area_->GetBaseHeight();

        if (x < 0)
          rgb_.blue = 0.0f;
        else if (x > picker_area_->GetBaseWidth())
          rgb_.blue = 1.0f;
        else
          rgb_.blue = (float) x / (float) picker_area_->GetBaseWidth();

      }

      if (color_channel_ == color::BLUE)
      {
        if (x < 0)
          rgb_.red = 0.0f;
        else if (x > picker_area_->GetBaseWidth())
          rgb_.red = 1.0f;
        else
          rgb_.red = (float) x / (float) picker_area_->GetBaseWidth();

        if (y < 0)
          rgb_.green = 1.0f;
        else if (y > picker_area_->GetBaseHeight())
          rgb_.green = 0.0f;
        else
          rgb_.green = 1.0f - (float) y / (float) picker_area_->GetBaseHeight();
      }

      hsv_ = color::HueSaturationValue(rgb_);
      m_MarkerPosition = Point(Clamp<int> (x, 0, picker_area_->GetBaseWidth() - 1), Clamp<int> (y, 0, picker_area_->GetBaseHeight() - 1));
    }

    if (m_ColorModel == color::HSV)
    {
      if (color_channel_ == color::HUE)
      {
        if (y < 0)
          hsv_.value = 1.0f;
        else if (y > picker_area_->GetBaseHeight())
          hsv_.value = 0.0f;
        else
          hsv_.value = 1.0f - (float) y / (float) picker_area_->GetBaseHeight();

        if (x < 0)
          hsv_.saturation = 0.0f;
        else if (x > picker_area_->GetBaseWidth())
          hsv_.saturation = 1.0f;
        else
          hsv_.saturation = (float) x / (float) picker_area_->GetBaseWidth();

      }

      if (color_channel_ == color::SATURATION)
      {
        if (y < 0)
          hsv_.value = 1.0f;
        else if (y > picker_area_->GetBaseHeight())
          hsv_.value = 0.0f;
        else
          hsv_.value = 1.0f - (float) y / (float) picker_area_->GetBaseHeight();

        if (x < 0)
          hsv_.hue = 0.0f;
        else if (x >= picker_area_->GetBaseWidth())
          hsv_.hue = 0.0f;
        else
          hsv_.hue = (float) x / (float) picker_area_->GetBaseWidth();

      }

      if (color_channel_ == color::VALUE)
      {
        if (x < 0)
          hsv_.hue = 0.0f;
        else if (x >= picker_area_->GetBaseWidth())
          hsv_.hue = 0.0f;
        else
          hsv_.hue = (float) x / (float) picker_area_->GetBaseWidth();

        if (y < 0)
          hsv_.saturation = 1.0f;
        else if (y > picker_area_->GetBaseHeight())
          hsv_.saturation = 0.0f;
        else
          hsv_.saturation = 1.0f - (float) y / (float) picker_area_->GetBaseHeight();
      }

      rgb_ = color::RedGreenBlue(hsv_);
      m_MarkerPosition = Point(Clamp<int> (x, 0, picker_area_->GetBaseWidth() - 1), Clamp<int> (y, 0, picker_area_->GetBaseHeight() - 1));
    }


    redtext->SetText(m_Validator.ToString(255 * rgb_.red));
    greentext->SetText(m_Validator.ToString(255 * rgb_.green));
    bluetext->SetText(m_Validator.ToString(255 * rgb_.blue));
    hue_text_entry_->SetText(m_Validator.ToString(360 * hsv_.hue));
    saturation_text_entry_->SetText(m_Validator.ToString(100 * hsv_.saturation));
    value_text_entry_->SetText(m_Validator.ToString(100 * hsv_.value));

    sigChange.emit(this);
    QueueDraw();
  }

  void ColorEditor::RecvPickerMouseUp(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void ColorEditor::RecvPickerMouseDrag(int x, int y, int /* dx */, int /* dy */, unsigned long button_flags, unsigned long key_flags)
  {
    RecvPickerMouseDown(x, y, button_flags, key_flags);
  }

  void ColorEditor::RecvCheckColorModel0(AbstractButton * /* button */, color::Model color_mode, color::Channel color_channel)
  {
    RecvCheckColorModel(true, color_mode, color_channel);
  }

  void ColorEditor::RecvCheckColorModel(bool b, color::Model color_mode, color::Channel color_channel)
  {
    if (b)
    {
      if ((color_mode == color::HSV) && (m_ColorModel == color::RGB))
      {
        hsv_ = color::HueSaturationValue(rgb_);
      }

      if ((color_mode == color::RGB) && (m_ColorModel == color::HSV))
      {
        rgb_ = color::RedGreenBlue(hsv_);
      }

      m_ColorModel = color_mode;
      color_channel_ = color_channel;
    }

    if (b && (color_mode == color::RGB))
    {
      int x = 0;
      int y = 0;
      int z = 1;

      if (color_channel_ == color::RED)
      {
        z = (1.0f - rgb_.red) * picker_area_->GetBaseHeight();
        y = (1.0f - rgb_.green) * picker_area_->GetBaseHeight();
        x = rgb_.blue * picker_area_->GetBaseWidth();
      }

      if (color_channel_ == color::GREEN)
      {
        z = (1.0f - rgb_.green) * picker_area_->GetBaseHeight();
        y = (1.0f - rgb_.red) * picker_area_->GetBaseHeight();
        x = rgb_.blue * picker_area_->GetBaseWidth();
      }

      if (color_channel_ == color::BLUE)
      {
        z = (1.0f - rgb_.blue) * picker_area_->GetBaseHeight();
        y = (1.0f - rgb_.green) * picker_area_->GetBaseHeight();
        x = rgb_.red * picker_area_->GetBaseWidth();
      }

      m_VertMarkerPosition = Point(Clamp<int> (0, 0, channel_area_->GetBaseWidth() - 1), Clamp<int> (z, 0, channel_area_->GetBaseHeight() - 1));
      m_MarkerPosition = Point(Clamp<int> (x, 0, picker_area_->GetBaseWidth() - 1), Clamp<int> (y, 0, picker_area_->GetBaseHeight() - 1));

      redtext->SetText(m_Validator.ToString(255 * rgb_.red));
      greentext->SetText(m_Validator.ToString(255 * rgb_.green));
      bluetext->SetText(m_Validator.ToString(255 * rgb_.blue));
    }

    if (b && (color_mode == color::HSV))
    {
      int x = 0;
      int y = 0;
      int z = 1;

      if (color_channel_ == color::HUE)
      {
        z = (1.0f - hsv_.hue) * picker_area_->GetBaseHeight();
        y = (1.0f - hsv_.value) * picker_area_->GetBaseHeight();
        x = hsv_.saturation * picker_area_->GetBaseWidth();
      }

      if (color_channel_ == color::SATURATION)
      {
        z = (1.0f - hsv_.saturation) * picker_area_->GetBaseHeight();
        y = (1.0f - hsv_.value) * picker_area_->GetBaseHeight();
        x = hsv_.hue * picker_area_->GetBaseWidth();
      }

      if (color_channel_ == color::VALUE)
      {
        z = (1.0f - hsv_.value) * picker_area_->GetBaseHeight();
        y = (1.0f - hsv_.saturation) * picker_area_->GetBaseHeight();
        x = hsv_.hue * picker_area_->GetBaseWidth();
      }

      m_VertMarkerPosition = Point(Clamp<int> (0, 0, channel_area_->GetBaseWidth() - 1), Clamp<int> (z, 0, channel_area_->GetBaseHeight() - 1));
      m_MarkerPosition = Point(Clamp<int> (x, 0, picker_area_->GetBaseWidth() - 1), Clamp<int> (y, 0, picker_area_->GetBaseHeight() - 1));

      hue_text_entry_->SetText(m_Validator.ToString(360 * hsv_.hue));
      saturation_text_entry_->SetText(m_Validator.ToString(100 * hsv_.saturation));
      value_text_entry_->SetText(m_Validator.ToString(100 * hsv_.value));
    }

    QueueDraw();
  }

  Color ColorEditor::GetRGBColor() const
  {
    return Color(rgb_);
  }

  void ColorEditor::SetRGB(Color const& rgb)
  {
    SetRGB(rgb.red, rgb.green, rgb.blue );
  }

  void ColorEditor::SetRGB(double r, double g, double b)
  {
    rgb_ = color::RedGreenBlue(Clamp<double>(r, 0.0, 1.0),
                               Clamp<double>(g, 0.0, 1.0),
                               Clamp<double>(b, 0.0, 1.0));
    hsv_ = color::HueSaturationValue(rgb_);
    RecvCheckColorModel(true, m_ColorModel, color_channel_);
    sigChange.emit(this);
  }

  void ColorEditor::SetHSV(double h, double s, double v)
  {
    hsv_ = color::HueSaturationValue(Clamp<double>(h, 0.0, 1.0),
                                     Clamp<double>(s, 0.0, 1.0),
                                     Clamp<double>(v, 0.0, 1.0));
    rgb_ = color::RedGreenBlue(hsv_);
    RecvCheckColorModel(true, m_ColorModel, color_channel_);
    sigChange.emit(this);
  }

  void ColorEditor::SetRed(double red)
  {
    SetRGB(red, rgb_.green, rgb_.blue);
  }

  void ColorEditor::SetGreen(double green)
  {
    SetRGB(rgb_.red, green, rgb_.blue);
  }

  void ColorEditor::SetBlue(double blue)
  {
    SetRGB(rgb_.red, rgb_.green, blue);
  }

  void ColorEditor::SetHue(double hue)
  {
    SetHSV(hue, hsv_.saturation, hsv_.value);
  }

  void ColorEditor::SetSaturation(double saturation)
  {
    SetHSV(hsv_.hue, saturation, hsv_.value);
  }

  void ColorEditor::SetValue(double value)
  {
    SetHSV(hsv_.hue, hsv_.saturation, value);
  }

  void ColorEditor::SetColorModel(color::Model colormodel,
                                  color::Channel colorchannel)
  {
    if (colormodel == color::HSV)
    {
      if ((colorchannel != color::HUE) &&
           (colorchannel != color::SATURATION) &&
           (colorchannel != color::VALUE))
      {
        nuxDebugMsg("[ColorEditor::SetColorModel] The color model(HSV) and the color channel don't match.");
        return;
      }
    }

    if (colormodel == color::RGB)
    {
      if ((colorchannel != color::RED) &&
           (colorchannel != color::GREEN) &&
           (colorchannel != color::BLUE))
      {
        nuxDebugMsg("[ColorEditor::SetColorModel] The color model(RGB) and the color channel don't match.");
        return;
      }
    }

    m_ColorModel = colormodel;
    color_channel_ = colorchannel;
    RecvCheckColorModel(true, m_ColorModel, color_channel_);

    /*FIXME - disabled because we lost radiogroup 
    if (color_channel_ == color::RED)
      radiogroup->ActivateButton(redcheck);
    else if (color_channel_ == color::GREEN)
      radiogroup->ActivateButton(greencheck);
    else if (color_channel_ == color::BLUE)
      radiogroup->ActivateButton(bluecheck);
    else if (color_channel_ == color::HUE)
      radiogroup->ActivateButton(huecheck);
    else if (color_channel_ == color::SATURATION)
      radiogroup->ActivateButton(saturationcheck);
    else if (color_channel_ == color::VALUE)
      radiogroup->ActivateButton(valuecheck);
    */

  }

  color::Model ColorEditor::GetColorModel() const
  {
    return m_ColorModel;
  }

  color::Channel ColorEditor::GetColorChannel() const
  {
    return color_channel_;
  }

  bool ColorEditor::AcceptKeyNavFocus()
  {
    return false;
  }
}
