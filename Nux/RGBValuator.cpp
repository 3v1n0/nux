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
#include "NuxGraphics/GLTextureResourceManager.h"
#include "HLayout.h"
#include "VLayout.h"
#include "EditTextBox.h"
#include "ToggleButton.h"
#include "HexRegExpValidator.h"
#include "IntegerValidator.h"
#include "DoubleValidator.h"
#include "RGBValuator.h"

#include <sstream>
#include <iomanip>

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(RGBValuator);

  RGBValuator::RGBValuator(NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , rgb_(1, 1, 1)
    , hsv_(rgb_)
    , hls_(rgb_)
    , m_color_model(color::RGB)
    , m_color_format(color::FLOAT)
  {
    InitializeLayout();
    SetColorModel(m_color_model);
    SetColorFormat(m_color_format);
    SetRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    InitializeWidgets();
  }

  RGBValuator::RGBValuator(Color const& color, NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , rgb_(1, 1, 1)
    , hsv_(rgb_)
    , hls_(rgb_)
    , m_color_model(color::RGB)
    , m_color_format(color::FLOAT)
  {
    InitializeLayout();
    SetColorModel(m_color_model);
    SetColorFormat(m_color_format);
    SetRGBA(color);
    InitializeWidgets();
  }

  RGBValuator::RGBValuator(color::Model colorModel, float x, float y, float z, float alpha,    NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , rgb_(1, 1, 1)
    , hsv_(rgb_)
    , hls_(rgb_)
    , m_color_model(colorModel)
    , m_color_format(color::FLOAT)
  {
    InitializeLayout();

    switch(m_color_model)
    {
      case color::HSV:
      {
        SetColorModel(color::HSV);
        SetHSV(x, y, z);
        SetAlpha(alpha);
      }
      case color::HLS:
      {
        SetColorModel(color::HLS);
        SetHLS(x, y, z);
        SetAlpha(alpha);
      }
      default:
      case color::RGB:
      {
        SetColorModel(color::RGB);
        SetRGBA(x, y, z, alpha);
      }
    }

    SetColorFormat(m_color_format);

    InitializeWidgets();

  }
  void RGBValuator::InitializeLayout()
  {
    hlayout             = new HLayout("RGBValuatorLayout", NUX_TRACKER_LOCATION);
    redlayout           = new HLayout("RedLayout", NUX_TRACKER_LOCATION);
    greenlayout         = new HLayout("GreenLayout", NUX_TRACKER_LOCATION);
    bluelayout          = new HLayout("BlueLayout", NUX_TRACKER_LOCATION);
    alphalayout         = new HLayout("AlphaLayout", NUX_TRACKER_LOCATION);
    vlayout             = new VLayout("RGBVLayout", NUX_TRACKER_LOCATION);
    colormodel_layout   = new VLayout("ColorModel", NUX_TRACKER_LOCATION);

    m_ColorModel        = new ToggleButton();
    m_ColorFormat       = new ToggleButton();

    red_caption_        = new EditTextBox("", NUX_TRACKER_LOCATION);
    green_caption_      = new EditTextBox("", NUX_TRACKER_LOCATION);
    blue_caption_       = new EditTextBox("", NUX_TRACKER_LOCATION);
    alpha_caption_      = new EditTextBox("", NUX_TRACKER_LOCATION);

    red_valuator_       = new BasicView(NUX_TRACKER_LOCATION);
    green_valuator_     = new BasicView(NUX_TRACKER_LOCATION);
    blue_valuator_      = new BasicView(NUX_TRACKER_LOCATION);
    alpha_valuator_     = new BasicView(NUX_TRACKER_LOCATION);
    color_square_       = new BasicView(NUX_TRACKER_LOCATION);

    m_ComponentLabel0   = new BasicView(NUX_TRACKER_LOCATION);
    m_ComponentLabel1   = new BasicView(NUX_TRACKER_LOCATION);
    m_ComponentLabel2   = new BasicView(NUX_TRACKER_LOCATION);
    m_ComponentAlpha    = new BasicView(NUX_TRACKER_LOCATION);
  }

  void RGBValuator::InitializeWidgets()
  {
    m_HexRegExp.SetMaximum(255);
    m_HexRegExp.SetMinimum(0);
    m_IntRegExp.SetMaximum(255);
    m_IntRegExp.SetMinimum(0);
    m_DoubleRegExp.SetMaximum(1.0);
    m_DoubleRegExp.SetMinimum(0);

    // Set Signals
    red_valuator_->mouse_down.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseDown_Red));
    green_valuator_->mouse_down.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseDown_Green));
    blue_valuator_->mouse_down.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseDown_Blue));
    alpha_valuator_->mouse_down.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseDown_Alpha));
    red_valuator_->mouse_drag.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseDrag_Red));
    green_valuator_->mouse_drag.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseDrag_Green));
    blue_valuator_->mouse_drag.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseDrag_Blue));
    alpha_valuator_->mouse_drag.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseDrag_Alpha));
    m_ColorModel->click.connect(sigc::mem_fun(this, &RGBValuator::OnChangeColorModel));
    m_ColorFormat->click.connect(sigc::mem_fun(this, &RGBValuator::OnChangeColorFormat));

    m_ColorModel->SetFont(GetSysBoldFont());
    m_ColorFormat->SetFont(GetSysBoldFont());

    red_valuator_->mouse_up.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseUp_Red));
    green_valuator_->mouse_up.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseUp_Green));
    blue_valuator_->mouse_up.connect(sigc::mem_fun(this, &RGBValuator::OnReceiveMouseUp_Blue));

    red_caption_->sigValidateKeyboardEntry.connect(sigc::bind(sigc::mem_fun(this, &RGBValuator::OnComponentInput), 0));
    green_caption_->sigValidateKeyboardEntry.connect(sigc::bind(sigc::mem_fun(this, &RGBValuator::OnComponentInput), 1));
    blue_caption_->sigValidateKeyboardEntry.connect(sigc::bind(sigc::mem_fun(this, &RGBValuator::OnComponentInput), 2));
    alpha_caption_->sigValidateKeyboardEntry.connect(sigc::bind(sigc::mem_fun(this, &RGBValuator::OnComponentInput), 3));

    // Set Geometry

    red_caption_->SetGeometry(Geometry(0, 0, 42, DEFAULT_WIDGET_HEIGHT));
    red_caption_->SetMaximumHeight(15);

    green_caption_->SetGeometry(Geometry(0, 0, 42, DEFAULT_WIDGET_HEIGHT));
    green_caption_->SetMaximumHeight(15);

    blue_caption_->SetGeometry(Geometry(0, 0, 42, DEFAULT_WIDGET_HEIGHT));
    blue_caption_->SetMaximumHeight(15);

    alpha_caption_->SetGeometry(Geometry(0, 0, 42, DEFAULT_WIDGET_HEIGHT));
    alpha_caption_->SetMaximumHeight(15);

    m_ComponentLabel0->SetMinimumSize(10, DEFAULT_WIDGET_HEIGHT);
    m_ComponentLabel0->SetMaximumHeight(15);
    //m_ComponentLabel0->SetGeometry(Geometry(0, 0, 15, DEFAULT_WIDGET_HEIGHT));

    m_ComponentLabel1->SetMinimumSize(10, DEFAULT_WIDGET_HEIGHT);
    m_ComponentLabel1->SetMaximumHeight(15);
    //m_ComponentLabel1->SetGeometry(Geometry(0, 0, 15, DEFAULT_WIDGET_HEIGHT));

    m_ComponentLabel2->SetMinimumSize(10, DEFAULT_WIDGET_HEIGHT);
    m_ComponentLabel2->SetMaximumHeight(15);
    //m_ComponentLabel2->SetGeometry(Geometry(0, 0, 15, DEFAULT_WIDGET_HEIGHT));

    m_ComponentAlpha->SetMinimumSize(10, DEFAULT_WIDGET_HEIGHT);
    m_ComponentAlpha->SetMaximumHeight(15);
    //m_ComponentAlpha->SetGeometry(Geometry(0, 0, 15, DEFAULT_WIDGET_HEIGHT));

    red_valuator_->SetMinimumSize(3 * DEFAULT_WIDGET_WIDTH, 15);
    red_valuator_->SetMaximumHeight(15);
    red_valuator_->SetGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    green_valuator_->SetMinimumSize(3 * DEFAULT_WIDGET_WIDTH, 15);
    green_valuator_->SetMaximumHeight(15);
    green_valuator_->SetGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    blue_valuator_->SetMinimumSize(3 * DEFAULT_WIDGET_WIDTH, 15);
    blue_valuator_->SetMaximumHeight(15);
    blue_valuator_->SetGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    alpha_valuator_->SetMinimumSize(3 * DEFAULT_WIDGET_WIDTH, 15);
    alpha_valuator_->SetMaximumHeight(15);
    alpha_valuator_->SetGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    color_square_->SetMinimumSize(32, 32);
    color_square_->SetGeometry(Geometry(0, 0, 32, 32));
    m_ColorModel->SetMinimumSize(20, 16);
    m_ColorModel->SetLayoutPadding(0, 0, 0, 0);
    m_ColorFormat->SetMinimumSize(20, 16);
    m_ColorFormat->SetLayoutPadding(0, 0, 0, 0);

    redlayout->AddView(m_ComponentLabel0, 0, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);
    redlayout->AddView(red_caption_, 0);
    redlayout->AddView(red_valuator_, 1);
    redlayout->SetSpaceBetweenChildren(4);

    greenlayout->AddView(m_ComponentLabel1, 0, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);
    greenlayout->AddView(green_caption_, 0);
    greenlayout->AddView(green_valuator_, 1);
    greenlayout->SetSpaceBetweenChildren(4);

    bluelayout->AddView(m_ComponentLabel2, 0, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);
    bluelayout->AddView(blue_caption_, 0);
    bluelayout->AddView(blue_valuator_, 1);
    bluelayout->SetSpaceBetweenChildren(4);

    alphalayout->AddView(m_ComponentAlpha, 0, MINOR_POSITION_CENTER, MINOR_SIZE_MATCHCONTENT);
    alphalayout->AddView(alpha_caption_, 0);
    alphalayout->AddView(alpha_valuator_, 1);
    alphalayout->SetSpaceBetweenChildren(4);

    // Set layout
    colormodel_layout->AddView(color_square_, 0, eCenter, eFix);
    colormodel_layout->AddView(m_ColorModel, 0, eCenter, eFull);
    colormodel_layout->AddView(m_ColorFormat, 0, eCenter, eFull);
    colormodel_layout->SetSpaceBetweenChildren(2);

    hlayout->AddView(colormodel_layout, 0, MINOR_POSITION_START, MINOR_SIZE_MATCHCONTENT);

    vlayout->AddLayout(redlayout, 0);
    vlayout->AddLayout(greenlayout, 0);
    vlayout->AddLayout(bluelayout, 0);
    vlayout->AddLayout(alphalayout, 0);
    vlayout->SetSpaceBetweenChildren(2);
    vlayout->SetTopAndBottomPadding(4);
    vlayout->SetLeftAndRightPadding(4);
    vlayout->SetContentDistribution(MAJOR_POSITION_CENTER);


    hlayout->AddLayout(vlayout, 1, MINOR_POSITION_CENTER, MINOR_SIZE_FULL);
    hlayout->SetSpaceBetweenChildren(2);
    hlayout->SetLeftAndRightPadding(6, 6);
    hlayout->SetTopAndBottomPadding(0);

    SetCompositionLayout(hlayout);

    NTextureData image;
    MakeCheckBoardImage(image.GetSurface(0), 64, 64, Color(0xff000000), Color(0xff323232), 4, 4);
    BaseTexture* CheckboardPattern = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture();
    CheckboardPattern->Update(&image);

    TexCoordXForm texxform;
    texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap(TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_CheckboardLayer = new TextureLayer(CheckboardPattern->GetDeviceTexture(), texxform, color::White);

    CheckboardPattern->UnReference();
  }

  RGBValuator::~RGBValuator()
  {
    NUX_SAFE_DELETE(m_CheckboardLayer);
  }

  void RGBValuator::DrawRedMarker(GraphicsEngine &graphics_engine)
  {
    int marker_position_x;
    int marker_position_y;

    float percent = 0.0f;

    if (m_color_model == color::RGB)
      percent = rgb_.red;

    if (m_color_model == color::HSV)
      percent = hsv_.hue;

    if (m_color_model == color::HLS)
      percent = hls_.hue;

    graphics_engine.PushClippingRectangle(red_valuator_->GetGeometry());

    marker_position_x = red_valuator_->GetBaseX() + percent * red_valuator_->GetBaseWidth();
    marker_position_y = red_valuator_->GetBaseY() + red_valuator_->GetBaseHeight();
    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color(0xFF000000));

    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color(0.7f, 0.7f, 0.7f, 1.0f));

    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::DrawGreenMarker(GraphicsEngine &graphics_engine)
  {
    int marker_position_x;
    int marker_position_y;

    float percent = 0.0f;

    if (m_color_model == color::RGB)
      percent = rgb_.green;

    if (m_color_model == color::HSV)
      percent = hsv_.saturation;

    if (m_color_model == color::HLS)
      percent = hls_.lightness;

    graphics_engine.PushClippingRectangle(green_valuator_->GetGeometry());

    marker_position_x = green_valuator_->GetBaseX() + percent * green_valuator_->GetBaseWidth();
    marker_position_y = green_valuator_->GetBaseY() + green_valuator_->GetBaseHeight();
    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color(0.0f, 0.0f, 0.0f, 1.0f));

    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color(0.7f, 0.7f, 0.7f, 1.0f));

    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::DrawBlueMarker(GraphicsEngine &graphics_engine)
  {
    int marker_position_x;
    int marker_position_y;

    float percent = 0.0f;

    if (m_color_model == color::RGB)
      percent = rgb_.blue;

    if (m_color_model == color::HSV)
      percent = hsv_.value;

    if (m_color_model == color::HLS)
      percent = hls_.saturation;

    graphics_engine.PushClippingRectangle(blue_valuator_->GetGeometry());

    marker_position_x = blue_valuator_->GetBaseX() + percent * blue_valuator_->GetBaseWidth();
    marker_position_y = blue_valuator_->GetBaseY() + blue_valuator_->GetBaseHeight();
    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color(0.0f, 0.0f, 0.0f, 1.0f));

    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color(0.7f, 0.7f, 0.7f, 1.0f));

    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::DrawAlphaMarker(GraphicsEngine &graphics_engine)
  {
    int marker_position_x;
    int marker_position_y;

    graphics_engine.PushClippingRectangle(alpha_valuator_->GetGeometry());

    marker_position_x = alpha_valuator_->GetBaseX() + alpha_ * alpha_valuator_->GetBaseWidth();
    marker_position_y = alpha_valuator_->GetBaseY() + alpha_valuator_->GetBaseHeight();
    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color(0.0f, 0.0f, 0.0f, 1.0f));

    GetPainter().Draw2DTriangleColor(graphics_engine, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color(0.7f, 0.7f, 0.7f, 1.0f));

    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();

    graphics_engine.PushClippingRectangle(base);
    GetPainter().PushDrawShapeLayer(graphics_engine, vlayout->GetGeometry(), eSHAPE_CORNER_ROUND4, Color(0xFF000000), eAllCorners, true);

    if (m_color_model == color::RGB)
    {
      DrawRGB(graphics_engine);
    }
    else if (m_color_model == color::HSV)
    {
      DrawHSV(graphics_engine);
    }
    else if (m_color_model == color::HLS)
    {
      DrawHLS(graphics_engine);
    }

    GetPainter().PaintTextLineStatic(graphics_engine, GetSysBoldFont(), m_ComponentLabel0->GetGeometry(), m_ComponentLabel0->GetBaseString(), Color(0xFFFFFFFF));
    GetPainter().PaintTextLineStatic(graphics_engine, GetSysBoldFont(), m_ComponentLabel1->GetGeometry(), m_ComponentLabel1->GetBaseString(), Color(0xFFFFFFFF));
    GetPainter().PaintTextLineStatic(graphics_engine, GetSysBoldFont(), m_ComponentLabel2->GetGeometry(), m_ComponentLabel2->GetBaseString(), Color(0xFFFFFFFF));
    GetPainter().PaintTextLineStatic(graphics_engine, GetSysBoldFont(), m_ComponentAlpha->GetGeometry(), m_ComponentAlpha->GetBaseString(), Color(0xFFFFFFFF));

    DrawRedMarker(graphics_engine);
    DrawGreenMarker(graphics_engine);
    DrawBlueMarker(graphics_engine);
    DrawAlphaMarker(graphics_engine);

    GetPainter().PopBackground();
    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::DrawContent(GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();
    graphics_engine.PushClippingRectangle(base);

    red_caption_->ProcessDraw(graphics_engine, force_draw);
    green_caption_->ProcessDraw(graphics_engine, force_draw);
    blue_caption_->ProcessDraw(graphics_engine, force_draw);
    alpha_caption_->ProcessDraw(graphics_engine, force_draw);

    m_ColorModel->ProcessDraw(graphics_engine, force_draw); // the button has round corner. That is why we need to push the background.
    m_ColorFormat->ProcessDraw(graphics_engine, force_draw); // the button has round corner. That is why we need to push the background.
    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::DrawRGB(GraphicsEngine &graphics_engine)
  {
    // Red
    Geometry P = red_valuator_->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(0.0f, rgb_.green, rgb_.blue), Color(0.0f, rgb_.green, rgb_.blue),
                               Color(1.0f, rgb_.green, rgb_.blue), Color(1.0f, rgb_.green, rgb_.blue));

    // Green
    P = green_valuator_->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(rgb_.red, 0.0f, rgb_.blue), Color(rgb_.red, 0.0f, rgb_.blue),
                               Color(rgb_.red, 1.0f, rgb_.blue), Color(rgb_.red, 1.0f, rgb_.blue));

    // Blue
    P = blue_valuator_->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(rgb_.red, rgb_.green, 0.0f), Color(rgb_.red, rgb_.green, 0.0f),
                               Color(rgb_.red, rgb_.green, 1.0f), Color(rgb_.red, rgb_.green, 1.0f));

    // Alpha
    P = alpha_valuator_->GetGeometry();
    m_CheckboardLayer->SetGeometry(P);
    m_CheckboardLayer->Renderlayer(graphics_engine);

    graphics_engine.GetRenderStates().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(0.0f, 0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f, 0.0f),
                               Color(rgb_.red, rgb_.green, rgb_.blue, 1.0f), Color(rgb_.red, rgb_.green, rgb_.blue, 1.0f));
    graphics_engine.GetRenderStates().SetBlend(false);

    P = color_square_->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(rgb_.red, rgb_.green, rgb_.blue), Color(rgb_.red, rgb_.green, rgb_.blue),
                               Color(rgb_.red, rgb_.green, rgb_.blue), Color(rgb_.red, rgb_.green, rgb_.blue));

    m_ColorModel->ProcessDraw(graphics_engine, true);
    m_ColorFormat->ProcessDraw(graphics_engine, true);
  }

  void RGBValuator::DrawHSV(GraphicsEngine &graphics_engine)
  {
    // Red
    Geometry P = red_valuator_->GetGeometry();
    float s = 0; //XSI: 1.0f - hsv_.saturation;
    float v = 1; //XSI: hsv_.value;
    float fw = P.GetWidth() / 6;

    Geometry p = Geometry(P.x, P.y, fw, P.GetHeight());
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(1.0f * v, s * v, s * v), Color(1.0f * v, s * v, s * v), Color(1.0f * v, 1.0f * v, s * v), Color(1.0f * v, 1.0f * v, s * v));
    p.SetX(P.x + fw);
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(1.0f * v, 1.0f * v, s * v), Color(1.0f * v, 1.0f * v, s * v), Color(s * v, 1.0f * v, s * v), Color(s * v, 1.0f * v, s * v));

    p.SetX(P.x + 2 * fw);
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(s * v, 1.0f * v, s * v), Color(s * v, 1.0f * v, s * v), Color(s * v, 1.0f * v, 1.0f * v), Color(s * v, 1.0f * v, 1.0f * v));
    p.SetX(P.x + 3 * fw);
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(s * v, 1.0f * v, 1.0f * v), Color(s * v, 1.0f * v, 1.0f * v), Color(s * v, s * v, 1.0f * v), Color(s * v, s * v, 1.0f * v));

    p.SetX(P.x + 4 * fw);
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(s * v, s * v, 1.0f * v), Color(s * v, s * v, 1.0f * v), Color(1.0f * v, s * v, 1.0f * v), Color(1.0f * v, s * v, 1.0f * v));
    p.SetX(P.x + 5 * fw);
    p.SetWidth(P.GetWidth() - 5 * fw); // correct rounding errors
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(1.0f * v, s * v, 1.0f * v), Color(1.0f * v, s * v, 1.0f * v), Color(1.0f * v, s * v, s * v), Color(1.0f * v, s * v, s * v));

    s = 1.0f - hsv_.saturation;
    v = hsv_.value;

    float hue = hsv_.hue;
    if (hue == 1.0f)
      hue = 0.0f;

    color::RedGreenBlue rgb(color::HueSaturationValue(hue, 1, 1));
    Color value_gray(v, v, v);
    Color value_color(Color(rgb) * v);
    // Green
    P = green_valuator_->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, value_gray, value_gray, value_color, value_color);

    rgb = color::RedGreenBlue(color::HueSaturationValue(hue, hsv_.saturation, 1));
    // Blue
    P = blue_valuator_->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, color::Black, color::Black, Color(rgb), Color(rgb));

    rgb = color::RedGreenBlue(color::HueSaturationValue(hue, hsv_.saturation, hsv_.value));

    // Alpha
    P = alpha_valuator_->GetGeometry();
    m_CheckboardLayer->SetGeometry(P);
    m_CheckboardLayer->Renderlayer(graphics_engine);

    graphics_engine.GetRenderStates().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(0.0f, 0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f, 0.0f), Color(rgb), Color(rgb));
    graphics_engine.GetRenderStates().SetBlend(false);

    P = color_square_->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(rgb));

    m_ColorModel->ProcessDraw(graphics_engine, true);
    m_ColorFormat->ProcessDraw(graphics_engine, true);
  }

  void RGBValuator::DrawHLS(GraphicsEngine &graphics_engine)
  {
    // Red
    Geometry P = red_valuator_->GetGeometry();
    float s = 0; //XSI: 1.0f - hls_.saturation;
    float l = 1; //XSI: hls_.lightness;
    float fw = P.GetWidth() / 6;

    Geometry p = Geometry(P.x, P.y, fw, P.GetHeight());
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(1.0f * l, s * l, s * l), Color(1.0f * l, s * l, s * l), Color(1.0f * l, 1.0f * l, s * l), Color(1.0f * l, 1.0f * l, s * l));
    p.SetX(P.x + fw);
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(1.0f * l, 1.0f * l, s * l), Color(1.0f * l, 1.0f * l, s * l), Color(s * l, 1.0f * l, s * l), Color(s * l, 1.0f * l, s * l));

    p.SetX(P.x + 2 * fw);
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(s * l, 1.0f * l, s * l), Color(s * l, 1.0f * l, s * l), Color(s * l, 1.0f * l, 1.0f * l), Color(s * l, 1.0f * l, 1.0f * l));
    p.SetX(P.x + 3 * fw);
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(s * l, 1.0f * l, 1.0f * l), Color(s * l, 1.0f * l, 1.0f * l), Color(s * l, s * l, 1.0f * l), Color(s * l, s * l, 1.0f * l));

    p.SetX(P.x + 4 * fw);
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(s * l, s * l, 1.0f * l), Color(s * l, s * l, 1.0f * l), Color(1.0f * l, s * l, 1.0f * l), Color(1.0f * l, s * l, 1.0f * l));
    p.SetX(P.x + 5 * fw);
    p.SetWidth(P.GetWidth() - 5 * fw); // correct rounding errors
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(1.0f * l, s * l, 1.0f * l), Color(1.0f * l, s * l, 1.0f * l), Color(1.0f * l, s * l, s * l), Color(1.0f * l, s * l, s * l));

    s = 1.0f - hls_.saturation;
    l = hls_.lightness;

    float Hue;

    if (hls_.hue == 1.0f)
      Hue = 0.0f;
    else
      Hue = hls_.hue;

    // TODO: Tim Penhey 2011-05-13
    // refactor this code to use the same methods as the RGB(A)Property classes.
    color::RedGreenBlue rgb(color::HueSaturationValue(Hue, 1, 1));
    // Need to use HSVtoRGB to compute the color
    float r = rgb.red;
    float g = rgb.green;
    float b = rgb.blue;

    // Green
    P = green_valuator_->GetGeometry();
    fw = P.GetWidth() / 2;
    p = Geometry(P.x, P.y, fw, P.GetHeight());
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f), Color(r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s), Color(r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s));
    p.SetX(P.x + fw);
    GetPainter().Paint2DQuadColor(graphics_engine, p, Color(r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s), Color(r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s), Color(1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f));


    //HLStoRGB(r, g, b, Hue, hsv_.saturation, 1.0f);
    // Blue
    float cr, cg, cb;

    if (l > 0.5)
    {
      float factor = (l - 0.5f) / 0.5f;
      cr = (1 - factor) * r * (1 - s) + 0.5 * s + factor * 1.0f;
      cg = (1 - factor) * g * (1 - s) + 0.5 * s + factor * 1.0f;
      cb = (1 - factor) * b * (1 - s) + 0.5 * s + factor * 1.0f;
    }
    else
    {
      float factor = l / 0.5f;
      cr = (factor) * r * (1 - s) + 0.5 * s;
      cg = (factor) * g * (1 - s) + 0.5 * s;
      cb = (factor) * b * (1 - s) + 0.5 * s;
    }

    P = blue_valuator_->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(l, l, l), Color(l, l, l), Color(cr, cg, cb), Color(cr, cg, cb));

    // TODO: Tim Penhey 2011-05-13
    // Can we just use the rgb_ member variable?  Check later.
    rgb = color::RedGreenBlue(hls_);

    // Alpha
    P = alpha_valuator_->GetGeometry();
    m_CheckboardLayer->SetGeometry(P);
    m_CheckboardLayer->Renderlayer(graphics_engine);

    graphics_engine.GetRenderStates().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(0.0f, 0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f, 0.0f), Color(rgb), Color(rgb));
    graphics_engine.GetRenderStates().SetBlend(false);

    P = color_square_->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(rgb));

    m_ColorModel->ProcessDraw(graphics_engine, true);
    m_ColorFormat->ProcessDraw(graphics_engine, true);
  }

  void RGBValuator::SetRGBA(Color const& color)
  {
    SetRGBA(color.red, color.green, color.blue, color.alpha);
  }

  void RGBValuator::SetRGBA(float r, float g, float b, float a)
  {
    SetRGB(r, g, b);
    SetAlpha(a);
  }

  void RGBValuator::SetRGB(Color const& color)
  {
    SetRGB(color.red, color.green, color.blue);
  }

  std::string as_hex(float f)
  {
    std::ostringstream s;
    s << std::hex << (int)f;
    return s.str();
  }

  std::string as_dec(float f)
  {
    std::ostringstream s;
    s << (int)f;
    return s.str();
  }

  std::string as_float(float f)
  {
    std::ostringstream s;
    s << std::setprecision(3) << f;
    return s.str();
  }

  void RGBValuator::SetRGB(float r, float g, float b)
  {
    rgb_.red   = Clamp(r, 0.0f, 1.0f);
    rgb_.green = Clamp(g, 0.0f, 1.0f);
    rgb_.blue  = Clamp(b, 0.0f, 1.0f);

    bool RedEditSelected = red_caption_->IsTextSelected();
    bool GreenEditSelected = green_caption_->IsTextSelected();
    bool BlueEditSelected = blue_caption_->IsTextSelected();

    if (m_color_format == color::HEX)
    {
      red_caption_->SetText(as_hex(rgb_.red * 255));
      green_caption_->SetText(as_hex(rgb_.green * 255));
      blue_caption_->SetText(as_hex(rgb_.blue * 255));
    }
    else if (m_color_format == color::INT)
    {
      red_caption_->SetText(as_dec(rgb_.red * 255));
      green_caption_->SetText(as_dec(rgb_.green * 255));
      blue_caption_->SetText(as_dec(rgb_.blue * 255));
    }
    else
    {
      red_caption_->SetText(as_float(rgb_.red));
      green_caption_->SetText(as_float(rgb_.green));
      blue_caption_->SetText(as_float(rgb_.blue));
    }

    // Restore text selection if necessary.
    // This solves a problem when an EditTextBox of the widget gets the focus and has its text selected
    // but another who is losing the focus will cause it to lose the text selection by calling SetRGB.
    if (RedEditSelected) red_caption_->m_KeyboardHandler.SelectAllText();

    if (GreenEditSelected) green_caption_->m_KeyboardHandler.SelectAllText();

    if (BlueEditSelected) blue_caption_->m_KeyboardHandler.SelectAllText();

    sigColorChanged.emit(rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  void RGBValuator::SetHSV(float h, float s, float v)
  {
    hsv_.hue        = Clamp(h, 0.0f, 1.0f);
    hsv_.saturation = Clamp(s, 0.0f, 1.0f);
    hsv_.value      = Clamp(v, 0.0f, 1.0f);

    bool RedEditSelected = red_caption_->IsTextSelected();
    bool GreenEditSelected = green_caption_->IsTextSelected();
    bool BlueEditSelected = blue_caption_->IsTextSelected();

    if (m_color_format == color::HEX)
    {
      red_caption_->SetText(as_hex(hsv_.hue * 255));
      green_caption_->SetText(as_hex(hsv_.saturation * 255));
      blue_caption_->SetText(as_hex(hsv_.value * 255));
    }
    else if (m_color_format == color::INT)
    {
      red_caption_->SetText(as_dec(hsv_.hue * 255));
      green_caption_->SetText(as_dec(hsv_.saturation * 255));
      blue_caption_->SetText(as_dec(hsv_.value * 255));
    }
    else
    {
      red_caption_->SetText(as_float(hsv_.hue));
      green_caption_->SetText(as_float(hsv_.saturation));
      blue_caption_->SetText(as_float(hsv_.value));
    }

    if (hsv_.hue >= 1.0f)
    {
      hsv_.hue = 1.0f;
      // XSI: hsv_.hue = 0.0f;
    }

    if (RedEditSelected) red_caption_->m_KeyboardHandler.SelectAllText();

    if (GreenEditSelected) green_caption_->m_KeyboardHandler.SelectAllText();

    if (BlueEditSelected) blue_caption_->m_KeyboardHandler.SelectAllText();

    rgb_ = color::RedGreenBlue(hsv_);
    sigColorChanged.emit(rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  void RGBValuator::SetHLS(float h, float l, float s)
  {
    hls_.hue        = Clamp(h, 0.0f, 1.0f);
    hls_.lightness      = Clamp(l, 0.0f, 1.0f);
    hls_.saturation = Clamp(s, 0.0f, 1.0f);

    bool RedEditSelected = red_caption_->IsTextSelected();
    bool GreenEditSelected = green_caption_->IsTextSelected();
    bool BlueEditSelected = blue_caption_->IsTextSelected();

    if (m_color_format == color::HEX)
    {
      red_caption_->SetText(as_hex(hls_.hue * 255));
      green_caption_->SetText(as_hex(hls_.lightness * 255));
      blue_caption_->SetText(as_hex(hls_.saturation * 255));
    }
    else if (m_color_format == color::INT)
    {
      red_caption_->SetText(as_dec(hls_.hue * 255));
      green_caption_->SetText(as_dec(hls_.lightness * 255));
      blue_caption_->SetText(as_dec(hls_.saturation * 255));
    }
    else
    {
      red_caption_->SetText(as_float(hls_.hue));
      green_caption_->SetText(as_float(hls_.lightness));
      blue_caption_->SetText(as_float(hls_.saturation));
    }

    if (hls_.hue >= 1.0f)
    {
      hls_.hue = 1.0f;
      // XSI: hls_.hue = 0.0f;
    }

    if (RedEditSelected) red_caption_->m_KeyboardHandler.SelectAllText();

    if (GreenEditSelected) green_caption_->m_KeyboardHandler.SelectAllText();

    if (BlueEditSelected) blue_caption_->m_KeyboardHandler.SelectAllText();

    rgb_ = color::RedGreenBlue(hls_);
    sigColorChanged.emit(rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  void RGBValuator::SetAlpha(float alpha)
  {
    alpha_ = Clamp(alpha, 0.0f, 1.0f);

    if (m_color_format == color::HEX)
    {
      alpha_caption_->SetText(as_hex(alpha_ * 255));
    }
    else if (m_color_format == color::INT)
    {
      alpha_caption_->SetText(as_dec(alpha_ * 255));
    }
    else
    {
      alpha_caption_->SetText(as_float(alpha_));
    }

    sigColorChanged.emit(rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  void RGBValuator::OnReceiveMouseDown_Red(int x, int /* y */, unsigned long /* button_flags */, unsigned long key_flags)
  {
    if (m_color_model == color::RGB)
    {
      if (x < 0)
        rgb_.red = 0.0f;
      else if (x > red_valuator_->GetBaseWidth())
        rgb_.red = 1.0f;
      else
        rgb_.red = (float) x / (float) red_valuator_->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetRGB(rgb_.red, rgb_.red, rgb_.red);
      }
      else
      {
        SetRGB(rgb_.red, rgb_.green, rgb_.blue);
      }
    }
    else if (m_color_model == color::HSV)
    {
      if (x < 0)
        hsv_.hue = 0.0f;
      else if (x > red_valuator_->GetBaseWidth())
        hsv_.hue = 1.0f;
      else
        hsv_.hue = (float) x / (float) red_valuator_->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHSV(hsv_.hue, hsv_.hue, hsv_.hue);
      }
      else
      {
        SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
      }
    }
    else if (m_color_model == color::HLS)
    {
      if (x < 0)
        hls_.hue = 0.0f;
      else if (x > red_valuator_->GetBaseWidth())
        hls_.hue = 1.0f;
      else
        hls_.hue = (float) x / (float) red_valuator_->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHLS(hls_.hue, hls_.hue, hls_.hue);
      }
      else
      {
        SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseDown_Green(int x, int /* y */, unsigned long /* button_flags */, unsigned long key_flags)
  {
    if (m_color_model == color::RGB)
    {
      if (x < 0)
        rgb_.green = 0.0f;
      else if (x > green_valuator_->GetBaseWidth())
        rgb_.green = 1.0f;
      else
        rgb_.green = (float) x / (float) green_valuator_->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetRGB(rgb_.green, rgb_.green, rgb_.green);
      }
      else
      {
        SetRGB(rgb_.red, rgb_.green, rgb_.blue);
      }
    }
    else if (m_color_model == color::HSV)
    {
      if (x < 0)
        hsv_.saturation = 0.0f;
      else if (x > green_valuator_->GetBaseWidth())
        hsv_.saturation = 1.0f;
      else
        hsv_.saturation = (float) x / (float) green_valuator_->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHSV(hsv_.saturation, hsv_.saturation, hsv_.saturation);
      }
      else
      {
        SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
      }
    }
    else if (m_color_model == color::HLS)
    {
      if (x < 0)
        hls_.lightness = 0.0f;
      else if (x > green_valuator_->GetBaseWidth())
        hls_.lightness = 1.0f;
      else
        hls_.lightness = (float) x / (float) green_valuator_->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHLS(hls_.lightness, hls_.lightness, hls_.lightness);
      }
      else
      {
        SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseDown_Blue(int x, int /* y */, unsigned long /* button_flags */, unsigned long key_flags)
  {
    if (m_color_model == color::RGB)
    {
      if (x < 0)
        rgb_.blue = 0.0f;
      else if (x > blue_valuator_->GetBaseWidth())
        rgb_.blue = 1.0f;
      else
        rgb_.blue = (float) x / (float) blue_valuator_->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetRGB(rgb_.blue, rgb_.blue, rgb_.blue);
      }
      else
      {
        SetRGB(rgb_.red, rgb_.green, rgb_.blue);
      }
    }
    else if (m_color_model == color::HSV)
    {
      if (x < 0)
        hsv_.value = 0.0f;
      else if (x > blue_valuator_->GetBaseWidth())
        hsv_.value = 1.0f;
      else
        hsv_.value = (float) x / (float) blue_valuator_->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHSV(hsv_.value, hsv_.value, hsv_.value);
      }
      else
      {
        SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
      }
    }
    else if (m_color_model == color::HLS)
    {
      if (x < 0)
        hls_.saturation = 0.0f;
      else if (x > blue_valuator_->GetBaseWidth())
        hls_.saturation = 1.0f;
      else
        hls_.saturation = (float) x / (float) blue_valuator_->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHLS(hls_.saturation, hls_.saturation, hls_.saturation);
      }
      else
      {
        SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseDown_Alpha(int x, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (x < 0)
      alpha_ = 0.0f;
    else if (x > alpha_valuator_->GetBaseWidth())
      alpha_ = 1.0f;
    else
      alpha_ = (float) x / (float) alpha_valuator_->GetBaseWidth();

    SetAlpha(alpha_);
    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseDrag_Red(int x, int y, int /* dx */, int /* dy */, unsigned long button_flags, unsigned long key_flags)
  {
    OnReceiveMouseDown_Red(x, y, button_flags, key_flags);
  }

  void RGBValuator::OnReceiveMouseDrag_Green(int x, int y, int /* dx */, int /* dy */, unsigned long button_flags, unsigned long key_flags)
  {
    OnReceiveMouseDown_Green(x, y, button_flags, key_flags);
  }

  void RGBValuator::OnReceiveMouseDrag_Blue(int x, int y, int /* dx */, int /* dy */, unsigned long button_flags, unsigned long key_flags)
  {
    OnReceiveMouseDown_Blue(x, y, button_flags, key_flags);
  }

  void RGBValuator::OnReceiveMouseDrag_Alpha(int x, int y, int /* dx */, int /* dy */, unsigned long button_flags, unsigned long key_flags)
  {
    OnReceiveMouseDown_Alpha(x, y, button_flags, key_flags);
  }

  void RGBValuator::OnReceiveMouseUp_Red       (int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_color_model == color::HSV)
    {
      if (hsv_.hue >= 1.0f)
      {
        hsv_.hue = 1.0f;
        // XSI: hsv_.hue = 0.0f;
      }

      if (hsv_.saturation <= 0.0f)
      {
        // XSI: hsv_.hue = 0.0f;
      }

      if (hsv_.value <= 0.0f)
      {
        // XSI: hsv_.hue = 0.0f;
        // XSI: hsv_.saturation = 0.0f;
      }

      SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
    }

    if (m_color_model == color::HLS)
    {
      if (hls_.hue >= 1.0f)
      {
        hls_.hue = 1.0f;
        // XSI: hls_.hue = 0.0f;
      }

      if (hls_.saturation <= 0.0f)
      {
        // XSI: hls_.hue = 0.0f;
      }

      if (hls_.lightness <= 0.0f || hls_.lightness >= 1.0f)
      {
        // XSI: hls_.hue = 0.0f;
        // XSI: hls_.saturation = 0.0f;
      }

      SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseUp_Green     (int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_color_model == color::HSV)
    {
      if (hsv_.hue >= 1.0f)
      {
        hsv_.hue = 1.0f;
        // XSI: hsv_.hue = 0.0f;
      }

      if (hsv_.saturation <= 0.0f)
      {
        // XSI: hsv_.hue = 0.0f;
      }

      if (hsv_.value <= 0.0f)
      {
        // XSI: hsv_.hue = 0.0f;
        // XSI: hsv_.saturation = 0.0f;
      }

      SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
    }

    if (m_color_model == color::HLS)
    {
      if (hls_.hue >= 1.0f)
      {
        hls_.hue = 1.0f;
        // XSI: hls_.hue = 0.0f;
      }

      if (hls_.saturation <= 0.0f)
      {
        // XSI: hls_.hue = 0.0f;
      }

      if (hls_.lightness <= 0.0f || hls_.lightness >= 1.0f)
      {
        // XSI: hls_.hue = 0.0f;
        // XSI: hls_.saturation = 0.0f;
      }

      SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseUp_Blue(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (m_color_model == color::HSV)
    {
      if (hsv_.hue >= 1.0f)
      {
        hsv_.hue = 1.0f;
        // XSI: hsv_.hue = 0.0f;
      }

      if (hsv_.saturation <= 0.0f)
      {
        // XSI: hsv_.hue = 0.0f;
      }

      if (hsv_.value <= 0.0f)
      {
        // XSI: hsv_.hue = 0.0f;
        // XSI: hsv_.saturation = 0.0f;
      }

      SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
    }

    if (m_color_model == color::HLS)
    {
      if (hls_.hue >= 1.0f)
      {
        hls_.hue = 1.0f;
        // XSI: hls_.hue = 0.0f;
      }

      if (hls_.saturation <= 0.0f)
      {
        // XSI: hls_.hue = 0.0f;
      }

      if (hls_.lightness <= 0.0f || hls_.lightness >= 1.0f)
      {
        // XSI: hls_.hue = 0.0f;
        // XSI: hls_.saturation = 0.0f;
      }

      SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
    }

    QueueDraw();
  }

  void RGBValuator::RecvMouseDownColorModel(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    QueueDraw();
  }

  void RGBValuator::OnChangeColorModel(AbstractButton* /* button */)
  {
    if (m_color_model == color::RGB)
    {
      SetColorModel(color::HLS);
      hls_ = color::HueLightnessSaturation(rgb_);
      SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
    }
    else if (m_color_model == color::HLS)
    {
      SetColorModel(color::HSV);
      rgb_ = color::RedGreenBlue(hls_);
      hsv_ = color::HueSaturationValue(rgb_);
      SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);

      if (hsv_.hue == -1.0f)
      {
        hsv_.hue = 0;
      }
    }
    else if (m_color_model == color::HSV)
    {
      SetColorModel(color::RGB);
      rgb_ = color::RedGreenBlue(hsv_);
      SetRGB(rgb_.red, rgb_.green, rgb_.blue);
    }

    QueueDraw();
  }

  void RGBValuator::OnChangeColorFormat(AbstractButton* /* button */)
  {
    if (m_color_format == color::FLOAT)
    {
      SetColorFormat(color::INT);
    }
    else if (m_color_format == color::INT)
    {
      SetColorFormat(color::HEX);
    }
    else if (m_color_format == color::HEX)
    {
      SetColorFormat(color::FLOAT);
    }

  }

  void RGBValuator::OnComponentInput(EditTextBox* /* textbox */, const std::string &s, int componentIndex)
  {
    float f = 0;

    if ((m_color_format == color::HEX) && (m_HexRegExp.Validate(s.c_str()) == Validator::Acceptable))
    {
      f = (float) m_HexRegExp.ToInteger(s.c_str()) / 255.0f;
    }
    else if ((m_color_format == color::INT) && (m_IntRegExp.Validate(s.c_str()) == Validator::Acceptable))
    {
      f = (float) m_IntRegExp.ToInteger(s.c_str()) / 255.0f;
    }
    else
    {
      f = (float) m_DoubleRegExp.ToDouble(s.c_str());
    }

    f = Clamp(f, 0.0f, 1.0f);

    if (componentIndex == 0)
    {
      f = Clamp(f, 0.0f, 1.0f);

      if (m_color_model == color::RGB)
      {
        rgb_.red = f;
        SetRGB(rgb_.red, rgb_.green, rgb_.blue);
      }
      else if (m_color_model == color::HSV)
      {
        hsv_.hue = f;
        OnReceiveMouseUp_Red(0, 0, 0, 0);
        SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
      }
      else if (m_color_model == color::HLS)
      {
        hls_.hue = f;
        OnReceiveMouseUp_Red(0, 0, 0, 0);
        SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    if (componentIndex == 1)
    {
      f = Clamp(f, 0.0f, 1.0f);

      if (m_color_model == color::RGB)
      {
        rgb_.green = f;
        SetRGB(rgb_.red, rgb_.green, rgb_.blue);
      }
      else if (m_color_model == color::HSV)
      {
        hsv_.saturation = f;
        OnReceiveMouseUp_Green(0, 0, 0, 0);
        SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
      }
      else if (m_color_model == color::HLS)
      {
        hls_.lightness = f;
        OnReceiveMouseUp_Green(0, 0, 0, 0);
        SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    if (componentIndex == 2)
    {
      f = Clamp(f, 0.0f, 1.0f);

      if (m_color_model == color::RGB)
      {
        rgb_.blue = f;
        SetRGB(rgb_.red, rgb_.green, rgb_.blue);
      }
      else if (m_color_model == color::HSV)
      {
        hsv_.value = f;
        OnReceiveMouseUp_Blue(0, 0, 0, 0);
        SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
      }
      else if (m_color_model == color::HLS)
      {
        hls_.saturation = f;
        OnReceiveMouseUp_Blue(0, 0, 0, 0);
        SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    if (componentIndex == 3)
    {
      float f = 0;
      f = CharToDouble(s.c_str());
      f = Clamp(f, 0.0f, 1.0f);
      //if(m_color_model == color::RGB)
      {
        alpha_ = f;
        SetAlpha(alpha_);
      }
    }

    QueueDraw();
  }


  void RGBValuator::SetColorModel(color::Model cm)
  {
    if (cm == color::RGB)
    {
      m_color_model = color::RGB;
      m_ColorModel->SetLabel("RGB");

      m_ComponentLabel0->SetBaseString("R");
      m_ComponentLabel1->SetBaseString("G");
      m_ComponentLabel2->SetBaseString("B");
      m_ComponentAlpha->SetBaseString("A");

    }

    if (cm == color::HSV)
    {
      m_color_model = color::HSV;
      m_ColorModel->SetLabel("HSV");

      m_ComponentLabel0->SetBaseString("H");
      m_ComponentLabel1->SetBaseString("S");
      m_ComponentLabel2->SetBaseString("V");
      m_ComponentAlpha->SetBaseString("A");
    }

    if (cm == color::HLS)
    {
      m_color_model = color::HLS;
      m_ColorModel->SetLabel("HLS");

      m_ComponentLabel0->SetBaseString("H");
      m_ComponentLabel1->SetBaseString("L");
      m_ComponentLabel2->SetBaseString("S");
      m_ComponentAlpha->SetBaseString("A");
    }

    if (cm == color::YUV)
    {
      m_color_model = color::YUV;
      m_ColorModel->SetLabel("YUV");

      m_ComponentLabel0->SetBaseString("Y");
      m_ComponentLabel1->SetBaseString("U");
      m_ComponentLabel2->SetBaseString("V");
      m_ComponentAlpha->SetBaseString("A");
    }
  }

  void RGBValuator::SetColorFormat(color::Format cf)
  {
    if (cf == color::FLOAT)
    {
      m_color_format = color::FLOAT;
      m_ColorFormat->SetLabel("float");
      red_caption_->SetKeyEntryType(BaseKeyboardHandler::eAlphaNumeric);
      green_caption_->SetKeyEntryType(BaseKeyboardHandler::eAlphaNumeric);
      blue_caption_->SetKeyEntryType(BaseKeyboardHandler::eAlphaNumeric);

      red_caption_->SetPrefix("");
      green_caption_->SetPrefix("");
      blue_caption_->SetPrefix("");
      alpha_caption_->SetPrefix("");
    }

    if (cf == color::INT)
    {
      m_color_format = color::INT;
      m_ColorFormat->SetLabel("int");
      red_caption_->SetKeyEntryType(BaseKeyboardHandler::eIntegerNumber);
      green_caption_->SetKeyEntryType(BaseKeyboardHandler::eIntegerNumber);
      blue_caption_->SetKeyEntryType(BaseKeyboardHandler::eIntegerNumber);

      red_caption_->SetPrefix("");
      green_caption_->SetPrefix("");
      blue_caption_->SetPrefix("");
      alpha_caption_->SetPrefix("");
    }

    if (cf == color::HEX)
    {
      m_color_format = color::HEX;
      m_ColorFormat->SetLabel("hex");
      red_caption_->SetKeyEntryType(BaseKeyboardHandler::eHexadecimalNumber);
      green_caption_->SetKeyEntryType(BaseKeyboardHandler::eHexadecimalNumber);
      blue_caption_->SetKeyEntryType(BaseKeyboardHandler::eHexadecimalNumber);

      red_caption_->SetPrefix("0x");
      green_caption_->SetPrefix("0x");
      blue_caption_->SetPrefix("0x");
      alpha_caption_->SetPrefix("0x");
    }

    if (m_color_model == color::RGB)
    {
      SetRGBA(rgb_.red, rgb_.green, rgb_.blue, alpha_);
    }
    else if (m_color_model == color::HLS)
    {
      SetHLS(hls_.hue, hls_.lightness, hls_.saturation);
      SetAlpha(alpha_);
    }
    else if (m_color_model == color::HSV)
    {
      SetHSV(hsv_.hue, hsv_.saturation, hsv_.value);
      SetAlpha(alpha_);
    }
  }

  void RGBValuator::PreLayoutManagement()
  {
    View::PreLayoutManagement();
    if (view_layout_)
    {
      // Constrain the vertical expansion of the color selector.
      view_layout_->SetBaseHeight(1);
    }
  }

  Color RGBValuator::GetColor() const
  {
    return Color(rgb_, alpha_);
  }

  void RGBValuator::EmitColorChangedSignal()
  {
    sigColorChanged.emit(rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  bool RGBValuator::AcceptKeyNavFocus()
  {
    return false;
  }
}
