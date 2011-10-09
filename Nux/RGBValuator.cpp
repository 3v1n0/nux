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

namespace nux
{
  NUX_IMPLEMENT_OBJECT_TYPE(RGBValuator);

  RGBValuator::RGBValuator (NUX_FILE_LINE_DECL)
    : View(NUX_FILE_LINE_PARAM)
    , rgb_(1, 1, 1)
    , hsv_(rgb_)
    , hls_(rgb_)
    , m_color_model(color::RGB)
    , m_color_format(color::FLOAT)
  {
    InitializeLayout();
    SetColorModel (m_color_model);
    SetColorFormat (m_color_format);
    SetRGBA (1.0f, 1.0f, 1.0f, 1.0f);
    InitializeWidgets();
  }

  RGBValuator::RGBValuator (Color const& color, NUX_FILE_LINE_DECL)
    : View (NUX_FILE_LINE_PARAM)
    , rgb_(1, 1, 1)
    , hsv_(rgb_)
    , hls_(rgb_)
    , m_color_model (color::RGB)
    , m_color_format (color::FLOAT)
  {
    InitializeLayout();
    SetColorModel (m_color_model);
    SetColorFormat (m_color_format);
    SetRGBA (color);
    InitializeWidgets();
  }

  RGBValuator::RGBValuator (color::Model colorModel, float x, float y, float z, float alpha,    NUX_FILE_LINE_DECL)
    : View (NUX_FILE_LINE_PARAM)
    , rgb_(1, 1, 1)
    , hsv_(rgb_)
    , hls_(rgb_)
    , m_color_model (colorModel)
    , m_color_format (color::FLOAT)
  {
    InitializeLayout();

    switch (m_color_model)
    {
      case color::HSV:
      {
        SetColorModel (color::HSV);
        SetHSV (x, y, z);
        SetAlpha (alpha);
      }
      case color::HLS:
      {
        SetColorModel (color::HLS);
        SetHLS (x, y, z);
        SetAlpha (alpha);
      }
      default:
      case color::RGB:
      {
        SetColorModel (color::RGB);
        SetRGBA (x, y, z, alpha);
      }
    }

    SetColorFormat (m_color_format);

    InitializeWidgets();

  }
  void RGBValuator::InitializeLayout()
  {
    hlayout             = new HLayout (TEXT ("RGBValuatorLayout"), NUX_TRACKER_LOCATION);
    redlayout           = new HLayout (TEXT ("RedLayout"), NUX_TRACKER_LOCATION);
    greenlayout         = new HLayout (TEXT ("GreenLayout"), NUX_TRACKER_LOCATION);
    bluelayout          = new HLayout (TEXT ("BlueLayout"), NUX_TRACKER_LOCATION);
    alphalayout         = new HLayout (TEXT ("AlphaLayout"), NUX_TRACKER_LOCATION);
    vlayout             = new VLayout (TEXT ("RGBVLayout"), NUX_TRACKER_LOCATION);
    colormodel_layout   = new VLayout (TEXT ("ColorModel"), NUX_TRACKER_LOCATION);

    m_ColorModel        = new ToggleButton();
    m_ColorFormat       = new ToggleButton();

    m_RedCaption        = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_GreenCaption      = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_BlueCaption       = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_AlphaCaption      = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);

    m_RedValuator       = new InputArea (NUX_TRACKER_LOCATION);
    m_GreenValuator     = new InputArea (NUX_TRACKER_LOCATION);
    m_BlueValuator      = new InputArea (NUX_TRACKER_LOCATION);
    m_AlphaValuator     = new InputArea (NUX_TRACKER_LOCATION);
    m_ColorSquare       = new InputArea (NUX_TRACKER_LOCATION);

    m_ComponentLabel0   = new InputArea (NUX_TRACKER_LOCATION);
    m_ComponentLabel1   = new InputArea (NUX_TRACKER_LOCATION);
    m_ComponentLabel2   = new InputArea (NUX_TRACKER_LOCATION);
    m_ComponentAlpha    = new InputArea (NUX_TRACKER_LOCATION);
  }

  void RGBValuator::InitializeWidgets()
  {
    m_HexRegExp.SetMaximum (255);
    m_HexRegExp.SetMinimum (0);
    m_IntRegExp.SetMaximum (255);
    m_IntRegExp.SetMinimum (0);
    m_DoubleRegExp.SetMaximum (1.0);
    m_DoubleRegExp.SetMinimum (0);

    // Set Signals
    m_RedValuator->mouse_down.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDown_Red) );
    m_GreenValuator->mouse_down.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDown_Green) );
    m_BlueValuator->mouse_down.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDown_Blue) );
    m_AlphaValuator->mouse_down.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDown_Alpha) );
    m_RedValuator->mouse_drag.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDrag_Red) );
    m_GreenValuator->mouse_drag.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDrag_Green) );
    m_BlueValuator->mouse_drag.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDrag_Blue) );
    m_AlphaValuator->mouse_drag.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDrag_Alpha) );
    m_ColorModel->click.connect (sigc::mem_fun (this, &RGBValuator::OnChangeColorModel) );
    m_ColorFormat->click.connect (sigc::mem_fun (this, &RGBValuator::OnChangeColorFormat) );
//    m_ColorModel->mouse_down.connect(sigc::mem_fun(this, &RGBValuator::RecvColorModelEvent));
//    m_ColorModel->mouse_up.connect(sigc::mem_fun(this, &RGBValuator::RecvColorModelEvent));
//    m_ColorModel->mouse_enter.connect(sigc::mem_fun(this, &RGBValuator::RecvColorModelEvent));


    m_ColorModel->SetFont (GetSysBoldFont() );
    m_ColorFormat->SetFont (GetSysBoldFont() );

    m_RedValuator->mouse_up.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseUp_Red) );
    m_GreenValuator->mouse_up.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseUp_Green) );
    m_BlueValuator->mouse_up.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseUp_Blue) );

    m_RedCaption->sigValidateKeyboardEntry.connect (sigc::bind (sigc::mem_fun (this, &RGBValuator::OnComponentInput), 0) );
    m_GreenCaption->sigValidateKeyboardEntry.connect (sigc::bind (sigc::mem_fun (this, &RGBValuator::OnComponentInput), 1) );
    m_BlueCaption->sigValidateKeyboardEntry.connect (sigc::bind (sigc::mem_fun (this, &RGBValuator::OnComponentInput), 2) );
    m_AlphaCaption->sigValidateKeyboardEntry.connect (sigc::bind (sigc::mem_fun (this, &RGBValuator::OnComponentInput), 3) );

    // Set Geometry

    m_RedCaption->SetGeometry (Geometry (0, 0, 42, DEFAULT_WIDGET_HEIGHT) );
    m_RedCaption->SetMaximumHeight (15);

    m_GreenCaption->SetGeometry (Geometry (0, 0, 42, DEFAULT_WIDGET_HEIGHT) );
    m_GreenCaption->SetMaximumHeight (15);

    m_BlueCaption->SetGeometry (Geometry (0, 0, 42, DEFAULT_WIDGET_HEIGHT) );
    m_BlueCaption->SetMaximumHeight (15);

    m_AlphaCaption->SetGeometry (Geometry (0, 0, 42, DEFAULT_WIDGET_HEIGHT) );
    m_AlphaCaption->SetMaximumHeight (15);

    m_ComponentLabel0->SetMinimumSize (10, DEFAULT_WIDGET_HEIGHT);
    m_ComponentLabel0->SetMaximumHeight (15);
    //m_ComponentLabel0->SetGeometry(Geometry(0, 0, 15, DEFAULT_WIDGET_HEIGHT));

    m_ComponentLabel1->SetMinimumSize (10, DEFAULT_WIDGET_HEIGHT);
    m_ComponentLabel1->SetMaximumHeight (15);
    //m_ComponentLabel1->SetGeometry(Geometry(0, 0, 15, DEFAULT_WIDGET_HEIGHT));

    m_ComponentLabel2->SetMinimumSize (10, DEFAULT_WIDGET_HEIGHT);
    m_ComponentLabel2->SetMaximumHeight (15);
    //m_ComponentLabel2->SetGeometry(Geometry(0, 0, 15, DEFAULT_WIDGET_HEIGHT));

    m_ComponentAlpha->SetMinimumSize (10, DEFAULT_WIDGET_HEIGHT);
    m_ComponentAlpha->SetMaximumHeight (15);
    //m_ComponentAlpha->SetGeometry(Geometry(0, 0, 15, DEFAULT_WIDGET_HEIGHT));

    m_RedValuator->SetMinimumSize (3 * DEFAULT_WIDGET_WIDTH, 15);
    m_RedValuator->SetMaximumHeight (15);
    m_RedValuator->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );

    m_GreenValuator->SetMinimumSize (3 * DEFAULT_WIDGET_WIDTH, 15);
    m_GreenValuator->SetMaximumHeight (15);
    m_GreenValuator->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );

    m_BlueValuator->SetMinimumSize (3 * DEFAULT_WIDGET_WIDTH, 15);
    m_BlueValuator->SetMaximumHeight (15);
    m_BlueValuator->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );

    m_AlphaValuator->SetMinimumSize (3 * DEFAULT_WIDGET_WIDTH, 15);
    m_AlphaValuator->SetMaximumHeight (15);
    m_AlphaValuator->SetGeometry (Geometry (0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT) );

    m_ColorSquare->SetMinimumSize (40, 40);
    m_ColorSquare->SetGeometry (Geometry (0, 0, 40, 40) );
    m_ColorModel->SetMinimumSize (20, 16 /*PRACTICAL_WIDGET_HEIGHT*/);
    m_ColorFormat->SetMinimumSize (20, 16 /*PRACTICAL_WIDGET_HEIGHT*/);

    redlayout->AddView (m_ComponentLabel0, 0);
    redlayout->AddView (m_RedCaption, 0);
    redlayout->AddView (m_RedValuator, 1);
    redlayout->SetHorizontalInternalMargin (4);

    greenlayout->AddView (m_ComponentLabel1, 0);
    greenlayout->AddView (m_GreenCaption, 0);
    greenlayout->AddView (m_GreenValuator, 1);
    greenlayout->SetHorizontalInternalMargin (4);

    bluelayout->AddView (m_ComponentLabel2, 0);
    bluelayout->AddView (m_BlueCaption, 0);
    bluelayout->AddView (m_BlueValuator, 1);
    bluelayout->SetHorizontalInternalMargin (4);

    alphalayout->AddView (m_ComponentAlpha, 0);
    alphalayout->AddView (m_AlphaCaption, 0);
    alphalayout->AddView (m_AlphaValuator, 1);
    alphalayout->SetHorizontalInternalMargin (4);

    // Set layout
    colormodel_layout->AddView (m_ColorSquare, 0, eCenter, eFix);
    colormodel_layout->AddView (m_ColorModel, 0, eCenter, eFull);
    colormodel_layout->AddView (m_ColorFormat, 0, eCenter, eFull);
    colormodel_layout->SetVerticalInternalMargin (2);

    hlayout->AddView (colormodel_layout, 0, eCenter , eFull);

    vlayout->AddLayout (redlayout, 1);
    vlayout->AddLayout (greenlayout, 1);
    vlayout->AddLayout (bluelayout, 1);
    vlayout->AddLayout (alphalayout, 1);
    vlayout->SetVerticalInternalMargin (2);
    vlayout->SetVerticalExternalMargin (4);
    vlayout->SetHorizontalExternalMargin (4);


    hlayout->AddLayout (vlayout, 10);
    hlayout->SetHorizontalInternalMargin (2);
    hlayout->SetHorizontalExternalMargin (6);
    hlayout->SetVerticalExternalMargin (4);

    SetCompositionLayout (hlayout);

    NTextureData image;
    MakeCheckBoardImage (image.GetSurface (0), 64, 64, Color (0xff000000), Color (0xff323232), 4, 4);
    BaseTexture* CheckboardPattern = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture ();
    CheckboardPattern->Update (&image);

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_CheckboardLayer = new TextureLayer (CheckboardPattern->GetDeviceTexture(), texxform, color::White);

    CheckboardPattern->UnReference ();
  }

  RGBValuator::~RGBValuator()
  {
    NUX_SAFE_DELETE (m_CheckboardLayer);
  }

  void RGBValuator::DrawRedMarker (GraphicsEngine &graphics_engine)
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

    graphics_engine.PushClippingRectangle (m_RedValuator->GetGeometry() );

    marker_position_x = m_RedValuator->GetBaseX() + percent * m_RedValuator->GetBaseWidth();
    marker_position_y = m_RedValuator->GetBaseY() + m_RedValuator->GetBaseHeight();
    GetPainter().Draw2DTriangleColor (graphics_engine, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color (0xFF000000) );

    GetPainter().Draw2DTriangleColor (graphics_engine, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color (0.7f, 0.7f, 0.7f, 1.0f) );

    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::DrawGreenMarker (GraphicsEngine &graphics_engine)
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

    graphics_engine.PushClippingRectangle (m_GreenValuator->GetGeometry() );

    marker_position_x = m_GreenValuator->GetBaseX() + percent * m_GreenValuator->GetBaseWidth();
    marker_position_y = m_GreenValuator->GetBaseY() + m_GreenValuator->GetBaseHeight();
    GetPainter().Draw2DTriangleColor (graphics_engine, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color (0.0f, 0.0f, 0.0f, 1.0f) );

    GetPainter().Draw2DTriangleColor (graphics_engine, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color (0.7f, 0.7f, 0.7f, 1.0f) );

    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::DrawBlueMarker (GraphicsEngine &graphics_engine)
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

    graphics_engine.PushClippingRectangle (m_BlueValuator->GetGeometry() );

    marker_position_x = m_BlueValuator->GetBaseX() + percent * m_BlueValuator->GetBaseWidth();
    marker_position_y = m_BlueValuator->GetBaseY() + m_BlueValuator->GetBaseHeight();
    GetPainter().Draw2DTriangleColor (graphics_engine, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color (0.0f, 0.0f, 0.0f, 1.0f) );

    GetPainter().Draw2DTriangleColor (graphics_engine, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color (0.7f, 0.7f, 0.7f, 1.0f) );

    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::DrawAlphaMarker (GraphicsEngine &graphics_engine)
  {
    int marker_position_x;
    int marker_position_y;

    graphics_engine.PushClippingRectangle (m_AlphaValuator->GetGeometry() );

    marker_position_x = m_AlphaValuator->GetBaseX() + alpha_ * m_AlphaValuator->GetBaseWidth();
    marker_position_y = m_AlphaValuator->GetBaseY() + m_AlphaValuator->GetBaseHeight();
    GetPainter().Draw2DTriangleColor (graphics_engine, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color (0.0f, 0.0f, 0.0f, 1.0f) );

    GetPainter().Draw2DTriangleColor (graphics_engine, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color (0.7f, 0.7f, 0.7f, 1.0f) );

    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::Draw (GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();

    graphics_engine.PushClippingRectangle (base);
    GetPainter().PushDrawShapeLayer (graphics_engine, vlayout->GetGeometry(), eSHAPE_CORNER_ROUND4, Color (0xFF000000), eAllCorners);

    if (m_color_model == color::RGB)
    {
      DrawRGB (graphics_engine);
    }
    else if (m_color_model == color::HSV)
    {
      DrawHSV (graphics_engine);
    }
    else if (m_color_model == color::HLS)
    {
      DrawHLS (graphics_engine);
    }

    GetPainter().PaintTextLineStatic (graphics_engine, GetSysBoldFont(), m_ComponentLabel0->GetGeometry(), m_ComponentLabel0->GetBaseString().GetTCharPtr(), Color (0xFFFFFFFF) );
    GetPainter().PaintTextLineStatic (graphics_engine, GetSysBoldFont(), m_ComponentLabel1->GetGeometry(), m_ComponentLabel1->GetBaseString().GetTCharPtr(), Color (0xFFFFFFFF) );
    GetPainter().PaintTextLineStatic (graphics_engine, GetSysBoldFont(), m_ComponentLabel2->GetGeometry(), m_ComponentLabel2->GetBaseString().GetTCharPtr(), Color (0xFFFFFFFF) );
    GetPainter().PaintTextLineStatic (graphics_engine, GetSysBoldFont(), m_ComponentAlpha->GetGeometry(), m_ComponentAlpha->GetBaseString().GetTCharPtr(), Color (0xFFFFFFFF) );

    DrawRedMarker (graphics_engine);
    DrawGreenMarker (graphics_engine);
    DrawBlueMarker (graphics_engine);
    DrawAlphaMarker (graphics_engine);

    m_RedCaption->QueueDraw();
    m_GreenCaption->QueueDraw();
    m_BlueCaption->QueueDraw();
    m_AlphaCaption->QueueDraw();
    m_ColorModel->QueueDraw();
    m_ColorFormat->QueueDraw();

    GetPainter().PopBackground();
    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::DrawContent (GraphicsEngine &graphics_engine, bool force_draw)
  {
    Geometry base = GetGeometry();
    graphics_engine.PushClippingRectangle (base);

    m_RedCaption->ProcessDraw (graphics_engine, force_draw);
    m_GreenCaption->ProcessDraw (graphics_engine, force_draw);
    m_BlueCaption->ProcessDraw (graphics_engine, force_draw);
    m_AlphaCaption->ProcessDraw (graphics_engine, force_draw);

    m_ColorModel->ProcessDraw (graphics_engine, force_draw); // the button has round corner. That is why we need to push the background.
    m_ColorFormat->ProcessDraw (graphics_engine, force_draw); // the button has round corner. That is why we need to push the background.
    graphics_engine.PopClippingRectangle();
  }

  void RGBValuator::PostDraw (GraphicsEngine &graphics_engine, bool force_draw)
  {

  }

  void RGBValuator::DrawRGB (GraphicsEngine &graphics_engine)
  {
    // Red
    Geometry P = m_RedValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (graphics_engine, P, Color (0.0f, rgb_.green, rgb_.blue), Color (0.0f, rgb_.green, rgb_.blue),
                               Color (1.0f, rgb_.green, rgb_.blue), Color (1.0f, rgb_.green, rgb_.blue) );

    // Green
    P = m_GreenValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (graphics_engine, P, Color (rgb_.red, 0.0f, rgb_.blue), Color (rgb_.red, 0.0f, rgb_.blue),
                               Color (rgb_.red, 1.0f, rgb_.blue), Color (rgb_.red, 1.0f, rgb_.blue) );

    // Blue
    P = m_BlueValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (graphics_engine, P, Color (rgb_.red, rgb_.green, 0.0f), Color (rgb_.red, rgb_.green, 0.0f),
                               Color (rgb_.red, rgb_.green, 1.0f), Color (rgb_.red, rgb_.green, 1.0f) );

    // Alpha
    P = m_AlphaValuator->GetGeometry();
    m_CheckboardLayer->SetGeometry (P);
    m_CheckboardLayer->Renderlayer (graphics_engine);

    graphics_engine.GetRenderStates().SetBlend (true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetPainter().Paint2DQuadColor (graphics_engine, P, Color (0.0f, 0.0f, 0.0f, 0.0f), Color (0.0f, 0.0f, 0.0f, 0.0f),
                               Color (rgb_.red, rgb_.green, rgb_.blue, 1.0f), Color (rgb_.red, rgb_.green, rgb_.blue, 1.0f) );
    graphics_engine.GetRenderStates().SetBlend (false);

    P = m_ColorSquare->GetGeometry();
    Geometry shadow_quad = P;
    P.OffsetSize (-2, -2);
    shadow_quad.OffsetSize (-2, -2);
    shadow_quad.OffsetPosition (2, 2);
    GetPainter().Paint2DQuadColor (graphics_engine, shadow_quad, Color (0, 0, 0) );
    GetPainter().Paint2DQuadColor (graphics_engine, P, Color (rgb_.red, rgb_.green, rgb_.blue), Color (rgb_.red, rgb_.green, rgb_.blue),
                               Color (rgb_.red, rgb_.green, rgb_.blue), Color (rgb_.red, rgb_.green, rgb_.blue) );
    GetPainter().Paint2DQuadWireframe (graphics_engine, P, Color (0, 0, 0) );

    m_ColorModel->ProcessDraw (graphics_engine, true);
    m_ColorFormat->ProcessDraw (graphics_engine, true);
  }

  void RGBValuator::DrawHSV (GraphicsEngine &graphics_engine)
  {
    // Red
    Geometry P = m_RedValuator->GetGeometry();
    float s = 0; //XSI: 1.0f - hsv_.saturation;
    float v = 1; //XSI: hsv_.value;
    float fw = P.GetWidth() / 6;

    Geometry p = Geometry (P.x, P.y, fw, P.GetHeight() );
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (1.0f * v, s * v, s * v), Color (1.0f * v, s * v, s * v), Color (1.0f * v, 1.0f * v, s * v), Color (1.0f * v, 1.0f * v, s * v) );
    p.SetX (P.x + fw);
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (1.0f * v, 1.0f * v, s * v), Color (1.0f * v, 1.0f * v, s * v), Color (s * v, 1.0f * v, s * v), Color (s * v, 1.0f * v, s * v) );

    p.SetX (P.x + 2 * fw);
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (s * v, 1.0f * v, s * v), Color (s * v, 1.0f * v, s * v), Color (s * v, 1.0f * v, 1.0f * v), Color (s * v, 1.0f * v, 1.0f * v) );
    p.SetX (P.x + 3 * fw);
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (s * v, 1.0f * v, 1.0f * v), Color (s * v, 1.0f * v, 1.0f * v), Color (s * v, s * v, 1.0f * v), Color (s * v, s * v, 1.0f * v) );

    p.SetX (P.x + 4 * fw);
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (s * v, s * v, 1.0f * v), Color (s * v, s * v, 1.0f * v), Color (1.0f * v, s * v, 1.0f * v), Color (1.0f * v, s * v, 1.0f * v) );
    p.SetX (P.x + 5 * fw);
    p.SetWidth (P.GetWidth() - 5 * fw); // correct rounding errors
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (1.0f * v, s * v, 1.0f * v), Color (1.0f * v, s * v, 1.0f * v), Color (1.0f * v, s * v, s * v), Color (1.0f * v, s * v, s * v) );

    s = 1.0f - hsv_.saturation;
    v = hsv_.value;

    float hue = hsv_.hue;
    if (hue == 1.0f)
      hue = 0.0f;

    color::RedGreenBlue rgb(color::HueSaturationValue(hue, 1, 1));
    Color value_gray(v, v, v);
    Color value_color(Color(rgb) * v);
    // Green
    P = m_GreenValuator->GetGeometry();
    GetPainter().Paint2DQuadColor(graphics_engine, P, value_gray, value_gray, value_color, value_color);

    rgb = color::RedGreenBlue(color::HueSaturationValue(hue, hsv_.saturation, 1));
    // Blue
    P = m_BlueValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (graphics_engine, P, color::Black, color::Black, Color(rgb), Color(rgb) );

    rgb = color::RedGreenBlue(color::HueSaturationValue(hue, hsv_.saturation, hsv_.value));

    // Alpha
    P = m_AlphaValuator->GetGeometry();
    m_CheckboardLayer->SetGeometry (P);
    m_CheckboardLayer->Renderlayer (graphics_engine);

    graphics_engine.GetRenderStates().SetBlend (true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetPainter().Paint2DQuadColor (graphics_engine, P, Color(0.0f, 0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 0.0f, 0.0f), Color(rgb), Color(rgb) );
    graphics_engine.GetRenderStates().SetBlend (false);

    P = m_ColorSquare->GetGeometry();
    Geometry shadow_quad = P;
    P.OffsetSize (-2, -2);
    shadow_quad.OffsetSize (-2, -2);
    shadow_quad.OffsetPosition (2, 2);
    GetPainter().Paint2DQuadColor (graphics_engine, shadow_quad, Color (0, 0, 0) );
    GetPainter().Paint2DQuadColor (graphics_engine, P, Color (rgb) );
    GetPainter().Paint2DQuadWireframe (graphics_engine, P, Color (0, 0, 0) );

    m_ColorModel->ProcessDraw (graphics_engine, true);
    m_ColorFormat->ProcessDraw (graphics_engine, true);
  }

  void RGBValuator::DrawHLS (GraphicsEngine &graphics_engine)
  {
    // Red
    Geometry P = m_RedValuator->GetGeometry();
    float s = 0; //XSI: 1.0f - hls_.saturation;
    float l = 1; //XSI: hls_.lightness;
    float fw = P.GetWidth() / 6;

    Geometry p = Geometry (P.x, P.y, fw, P.GetHeight() );
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (1.0f * l, s * l, s * l), Color (1.0f * l, s * l, s * l), Color (1.0f * l, 1.0f * l, s * l), Color (1.0f * l, 1.0f * l, s * l) );
    p.SetX (P.x + fw);
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (1.0f * l, 1.0f * l, s * l), Color (1.0f * l, 1.0f * l, s * l), Color (s * l, 1.0f * l, s * l), Color (s * l, 1.0f * l, s * l) );

    p.SetX (P.x + 2 * fw);
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (s * l, 1.0f * l, s * l), Color (s * l, 1.0f * l, s * l), Color (s * l, 1.0f * l, 1.0f * l), Color (s * l, 1.0f * l, 1.0f * l) );
    p.SetX (P.x + 3 * fw);
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (s * l, 1.0f * l, 1.0f * l), Color (s * l, 1.0f * l, 1.0f * l), Color (s * l, s * l, 1.0f * l), Color (s * l, s * l, 1.0f * l) );

    p.SetX (P.x + 4 * fw);
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (s * l, s * l, 1.0f * l), Color (s * l, s * l, 1.0f * l), Color (1.0f * l, s * l, 1.0f * l), Color (1.0f * l, s * l, 1.0f * l) );
    p.SetX (P.x + 5 * fw);
    p.SetWidth (P.GetWidth() - 5 * fw); // correct rounding errors
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (1.0f * l, s * l, 1.0f * l), Color (1.0f * l, s * l, 1.0f * l), Color (1.0f * l, s * l, s * l), Color (1.0f * l, s * l, s * l) );

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
    P = m_GreenValuator->GetGeometry();
    fw = P.GetWidth() / 2;
    p = Geometry (P.x, P.y, fw, P.GetHeight() );
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (0.0f, 0.0f, 0.0f), Color (0.0f, 0.0f, 0.0f), Color (r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s), Color (r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s) );
    p.SetX (P.x + fw);
    GetPainter().Paint2DQuadColor (graphics_engine, p, Color (r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s), Color (r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s), Color (1.0f, 1.0f, 1.0f), Color (1.0f, 1.0f, 1.0f) );


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

    P = m_BlueValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (graphics_engine, P, Color (l, l, l), Color (l, l, l), Color (cr, cg, cb), Color (cr, cg, cb) );

    // TODO: Tim Penhey 2011-05-13
    // Can we just use the rgb_ member variable?  Check later.
    rgb = color::RedGreenBlue(hls_);

    // Alpha
    P = m_AlphaValuator->GetGeometry();
    m_CheckboardLayer->SetGeometry (P);
    m_CheckboardLayer->Renderlayer (graphics_engine);

    graphics_engine.GetRenderStates().SetBlend (true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetPainter().Paint2DQuadColor(graphics_engine, P, Color(0.0f, 0.0f, 0.0f, 0.0f), Color (0.0f, 0.0f, 0.0f, 0.0f), Color(rgb), Color(rgb) );
    graphics_engine.GetRenderStates().SetBlend (false);

    P = m_ColorSquare->GetGeometry();
    Geometry shadow_quad = P;
    P.OffsetSize (-2, -2);
    shadow_quad.OffsetSize (-2, -2);
    shadow_quad.OffsetPosition (2, 2);
    GetPainter().Paint2DQuadColor (graphics_engine, shadow_quad, Color (0, 0, 0) );
    GetPainter().Paint2DQuadColor (graphics_engine, P, Color (rgb) );
    GetPainter().Paint2DQuadWireframe (graphics_engine, P, Color (0, 0, 0) );

    m_ColorModel->ProcessDraw (graphics_engine, true);
    m_ColorFormat->ProcessDraw (graphics_engine, true);
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

  void RGBValuator::SetRGB (float r, float g, float b)
  {
    rgb_.red   = Clamp (r, 0.0f, 1.0f);
    rgb_.green = Clamp (g, 0.0f, 1.0f);
    rgb_.blue  = Clamp (b, 0.0f, 1.0f);

    bool RedEditSelected = m_RedCaption->IsTextSelected();
    bool GreenEditSelected = m_GreenCaption->IsTextSelected();
    bool BlueEditSelected = m_BlueCaption->IsTextSelected();

    if (m_color_format == color::HEX)
    {
      m_RedCaption->SetText (NString::Printf ("%x", (int) (rgb_.red * 255) ) );
      m_GreenCaption->SetText (NString::Printf ("%x", (int) (rgb_.green * 255) ) );
      m_BlueCaption->SetText (NString::Printf ("%x", (int) (rgb_.blue * 255) ) );
    }
    else if (m_color_format == color::INT)
    {
      m_RedCaption->SetText (NString::Printf ("%d", (int) (rgb_.red * 255) ) );
      m_GreenCaption->SetText (NString::Printf ("%d", (int) (rgb_.green * 255) ) );
      m_BlueCaption->SetText (NString::Printf ("%d", (int) (rgb_.blue * 255) ) );
    }
    else
    {
      m_RedCaption->SetText (NString::Printf ("%.3f", rgb_.red) );
      m_GreenCaption->SetText (NString::Printf ("%.3f", rgb_.green) );
      m_BlueCaption->SetText (NString::Printf ("%.3f", rgb_.blue) );
    }

    // Restore text selection if necessary.
    // This solves a problem when an EditTextBox of the widget gets the focus and has its text selected
    // but another who is losing the focus will cause it to lose the text selection by calling SetRGB.
    if (RedEditSelected) m_RedCaption->m_KeyboardHandler.SelectAllText();

    if (GreenEditSelected) m_GreenCaption->m_KeyboardHandler.SelectAllText();

    if (BlueEditSelected) m_BlueCaption->m_KeyboardHandler.SelectAllText();

    sigColorChanged.emit (rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  void RGBValuator::SetHSV (float h, float s, float v)
  {
    hsv_.hue        = Clamp (h, 0.0f, 1.0f);
    hsv_.saturation = Clamp (s, 0.0f, 1.0f);
    hsv_.value      = Clamp (v, 0.0f, 1.0f);

    bool RedEditSelected = m_RedCaption->IsTextSelected();
    bool GreenEditSelected = m_GreenCaption->IsTextSelected();
    bool BlueEditSelected = m_BlueCaption->IsTextSelected();

    if (m_color_format == color::HEX)
    {
      m_RedCaption->SetText (NString::Printf ("%x", (int) (hsv_.hue * 255) ) );
      m_GreenCaption->SetText (NString::Printf ("%x", (int) (hsv_.saturation * 255) ) );
      m_BlueCaption->SetText (NString::Printf ("%x", (int) (hsv_.value * 255) ) );
    }
    else if (m_color_format == color::INT)
    {
      m_RedCaption->SetText (NString::Printf ("%d", (int) (hsv_.hue * 255) ) );
      m_GreenCaption->SetText (NString::Printf ("%d", (int) (hsv_.saturation * 255) ) );
      m_BlueCaption->SetText (NString::Printf ("%d", (int) (hsv_.value * 255) ) );
    }
    else
    {
      m_RedCaption->SetText (NString::Printf ("%.3f", hsv_.hue) );
      m_GreenCaption->SetText (NString::Printf ("%.3f", hsv_.saturation) );
      m_BlueCaption->SetText (NString::Printf ("%.3f", hsv_.value) );
    }

    if (hsv_.hue >= 1.0f)
    {
      hsv_.hue = 1.0f;
      // XSI: hsv_.hue = 0.0f;
    }

    if (RedEditSelected) m_RedCaption->m_KeyboardHandler.SelectAllText();

    if (GreenEditSelected) m_GreenCaption->m_KeyboardHandler.SelectAllText();

    if (BlueEditSelected) m_BlueCaption->m_KeyboardHandler.SelectAllText();

    rgb_ = color::RedGreenBlue(hsv_);
    sigColorChanged.emit (rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  void RGBValuator::SetHLS (float h, float l, float s)
  {
    hls_.hue        = Clamp (h, 0.0f, 1.0f);
    hls_.lightness      = Clamp (l, 0.0f, 1.0f);
    hls_.saturation = Clamp (s, 0.0f, 1.0f);

    bool RedEditSelected = m_RedCaption->IsTextSelected();
    bool GreenEditSelected = m_GreenCaption->IsTextSelected();
    bool BlueEditSelected = m_BlueCaption->IsTextSelected();

    if (m_color_format == color::HEX)
    {
      m_RedCaption->SetText (NString::Printf (TEXT ("%x"), (int) (hls_.hue * 255) ) );
      m_GreenCaption->SetText (NString::Printf ("%x", (int) (hls_.lightness * 255) ) );
      m_BlueCaption->SetText (NString::Printf ("%x", (int) (hls_.saturation * 255) ) );
    }
    else if (m_color_format == color::INT)
    {
      m_RedCaption->SetText (NString::Printf (TEXT ("%d"), (int) (hls_.hue * 255) ) );
      m_GreenCaption->SetText (NString::Printf ("%d", (int) (hls_.lightness * 255) ) );
      m_BlueCaption->SetText (NString::Printf ("%d", (int) (hls_.saturation * 255) ) );
    }
    else
    {
      m_RedCaption->SetText (NString::Printf (TEXT ("%.3f"), hls_.hue) );
      m_GreenCaption->SetText (NString::Printf ("%.3f", hls_.lightness) );
      m_BlueCaption->SetText (NString::Printf ("%.3f", hls_.saturation) );
    }

    if (hls_.hue >= 1.0f)
    {
      hls_.hue = 1.0f;
      // XSI: hls_.hue = 0.0f;
    }

    if (RedEditSelected) m_RedCaption->m_KeyboardHandler.SelectAllText();

    if (GreenEditSelected) m_GreenCaption->m_KeyboardHandler.SelectAllText();

    if (BlueEditSelected) m_BlueCaption->m_KeyboardHandler.SelectAllText();

    rgb_ = color::RedGreenBlue(hls_);
    sigColorChanged.emit (rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  void RGBValuator::SetAlpha(float alpha)
  {
    alpha_ = Clamp (alpha, 0.0f, 1.0f);

    if (m_color_format == color::HEX)
    {
      m_AlphaCaption->SetText (NString::Printf ("%x", (int) (alpha_ * 255) ) );
    }
    else if (m_color_format == color::INT)
    {
      m_AlphaCaption->SetText (NString::Printf ("%d", (int) (alpha_ * 255) ) );
    }
    else
    {
      m_AlphaCaption->SetText (NString::Printf ("%.3f", alpha_) );
    }

    sigColorChanged.emit(rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  void RGBValuator::OnReceiveMouseDown_Red (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_color_model == color::RGB)
    {
      if (x < 0)
        rgb_.red = 0.0f;
      else if (x > m_RedValuator->GetBaseWidth() )
        rgb_.red = 1.0f;
      else
        rgb_.red = (float) x / (float) m_RedValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetRGB (rgb_.red, rgb_.red, rgb_.red);
      }
      else
      {
        SetRGB (rgb_.red, rgb_.green, rgb_.blue);
      }
    }
    else if (m_color_model == color::HSV)
    {
      if (x < 0)
        hsv_.hue = 0.0f;
      else if (x > m_RedValuator->GetBaseWidth() )
        hsv_.hue = 1.0f;
      else
        hsv_.hue = (float) x / (float) m_RedValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHSV (hsv_.hue, hsv_.hue, hsv_.hue);
      }
      else
      {
        SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
      }
    }
    else if (m_color_model == color::HLS)
    {
      if (x < 0)
        hls_.hue = 0.0f;
      else if (x > m_RedValuator->GetBaseWidth() )
        hls_.hue = 1.0f;
      else
        hls_.hue = (float) x / (float) m_RedValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHLS (hls_.hue, hls_.hue, hls_.hue);
      }
      else
      {
        SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseDown_Green (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_color_model == color::RGB)
    {
      if (x < 0)
        rgb_.green = 0.0f;
      else if (x > m_GreenValuator->GetBaseWidth() )
        rgb_.green = 1.0f;
      else
        rgb_.green = (float) x / (float) m_GreenValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetRGB (rgb_.green, rgb_.green, rgb_.green);
      }
      else
      {
        SetRGB (rgb_.red, rgb_.green, rgb_.blue);
      }
    }
    else if (m_color_model == color::HSV)
    {
      if (x < 0)
        hsv_.saturation = 0.0f;
      else if (x > m_GreenValuator->GetBaseWidth() )
        hsv_.saturation = 1.0f;
      else
        hsv_.saturation = (float) x / (float) m_GreenValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHSV (hsv_.saturation, hsv_.saturation, hsv_.saturation);
      }
      else
      {
        SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
      }
    }
    else if (m_color_model == color::HLS)
    {
      if (x < 0)
        hls_.lightness = 0.0f;
      else if (x > m_GreenValuator->GetBaseWidth() )
        hls_.lightness = 1.0f;
      else
        hls_.lightness = (float) x / (float) m_GreenValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHLS (hls_.lightness, hls_.lightness, hls_.lightness);
      }
      else
      {
        SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseDown_Blue (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_color_model == color::RGB)
    {
      if (x < 0)
        rgb_.blue = 0.0f;
      else if (x > m_BlueValuator->GetBaseWidth() )
        rgb_.blue = 1.0f;
      else
        rgb_.blue = (float) x / (float) m_BlueValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetRGB (rgb_.blue, rgb_.blue, rgb_.blue);
      }
      else
      {
        SetRGB (rgb_.red, rgb_.green, rgb_.blue);
      }
    }
    else if (m_color_model == color::HSV)
    {
      if (x < 0)
        hsv_.value = 0.0f;
      else if (x > m_BlueValuator->GetBaseWidth() )
        hsv_.value = 1.0f;
      else
        hsv_.value = (float) x / (float) m_BlueValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHSV (hsv_.value, hsv_.value, hsv_.value);
      }
      else
      {
        SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
      }
    }
    else if (m_color_model == color::HLS)
    {
      if (x < 0)
        hls_.saturation = 0.0f;
      else if (x > m_BlueValuator->GetBaseWidth() )
        hls_.saturation = 1.0f;
      else
        hls_.saturation = (float) x / (float) m_BlueValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHLS (hls_.saturation, hls_.saturation, hls_.saturation);
      }
      else
      {
        SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseDown_Alpha (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (x < 0)
      alpha_ = 0.0f;
    else if (x > m_AlphaValuator->GetBaseWidth() )
      alpha_ = 1.0f;
    else
      alpha_ = (float) x / (float) m_AlphaValuator->GetBaseWidth();

    SetAlpha (alpha_);
    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseDrag_Red (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    OnReceiveMouseDown_Red (x, y, button_flags, key_flags);
  }

  void RGBValuator::OnReceiveMouseDrag_Green (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    OnReceiveMouseDown_Green (x, y, button_flags, key_flags);
  }

  void RGBValuator::OnReceiveMouseDrag_Blue (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    OnReceiveMouseDown_Blue (x, y, button_flags, key_flags);
  }

  void RGBValuator::OnReceiveMouseDrag_Alpha (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    OnReceiveMouseDown_Alpha (x, y, button_flags, key_flags);
  }

  void RGBValuator::OnReceiveMouseUp_Red       (int x, int y, unsigned long button_flags, unsigned long key_flags)
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

      SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
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

      SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseUp_Green     (int x, int y, unsigned long button_flags, unsigned long key_flags)
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

      SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
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

      SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
    }

    QueueDraw();
  }

  void RGBValuator::OnReceiveMouseUp_Blue (int x, int y, unsigned long button_flags, unsigned long key_flags)
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

      SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
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

      SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
    }

    QueueDraw();
  }

  void RGBValuator::RecvMouseDownColorModel (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    QueueDraw();
  }

  void RGBValuator::OnChangeColorModel(AbstractButton* button)
  {
    if (m_color_model == color::RGB)
    {
      SetColorModel (color::HLS);
      hls_ = color::HueLightnessSaturation(rgb_);
      SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
    }
    else if (m_color_model == color::HLS)
    {
      SetColorModel (color::HSV);
      rgb_ = color::RedGreenBlue(hls_);
      hsv_ = color::HueSaturationValue(rgb_);
      SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);

      if (hsv_.hue == -1.0f)
      {
        hsv_.hue = 0;
      }
    }
    else if (m_color_model == color::HSV)
    {
      SetColorModel (color::RGB);
      rgb_ = color::RedGreenBlue(hsv_);
      SetRGB (rgb_.red, rgb_.green, rgb_.blue);
    }

    QueueDraw();
  }

  void RGBValuator::OnChangeColorFormat(AbstractButton* button)
  {
    if (m_color_format == color::FLOAT)
    {
      SetColorFormat (color::INT);
    }
    else if (m_color_format == color::INT)
    {
      SetColorFormat (color::HEX);
    }
    else if (m_color_format == color::HEX)
    {
      SetColorFormat (color::FLOAT);
    }

  }

  void RGBValuator::OnComponentInput (EditTextBox *textbox, const NString &s, int componentIndex)
  {
    float f = 0;

    if ( (m_color_format == color::HEX) && (m_HexRegExp.Validate (s.GetTCharPtr() ) == Validator::Acceptable) )
    {
      f = (float) m_HexRegExp.ToInteger (s.GetTCharPtr() ) / 255.0f;
    }
    else if ( (m_color_format == color::INT) && (m_IntRegExp.Validate (s.GetTCharPtr() ) == Validator::Acceptable) )
    {
      f = (float) m_IntRegExp.ToInteger (s.GetTCharPtr() ) / 255.0f;
    }
    else
    {
      f = (float) m_DoubleRegExp.ToDouble (s.GetTCharPtr() );
    }

    f = Clamp (f, 0.0f, 1.0f);

    if (componentIndex == 0)
    {
      f = Clamp (f, 0.0f, 1.0f);

      if (m_color_model == color::RGB)
      {
        rgb_.red = f;
        SetRGB (rgb_.red, rgb_.green, rgb_.blue);
      }
      else if (m_color_model == color::HSV)
      {
        hsv_.hue = f;
        OnReceiveMouseUp_Red (0, 0, 0, 0);
        SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
      }
      else if (m_color_model == color::HLS)
      {
        hls_.hue = f;
        OnReceiveMouseUp_Red (0, 0, 0, 0);
        SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    if (componentIndex == 1)
    {
      f = Clamp (f, 0.0f, 1.0f);

      if (m_color_model == color::RGB)
      {
        rgb_.green = f;
        SetRGB (rgb_.red, rgb_.green, rgb_.blue);
      }
      else if (m_color_model == color::HSV)
      {
        hsv_.saturation = f;
        OnReceiveMouseUp_Green (0, 0, 0, 0);
        SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
      }
      else if (m_color_model == color::HLS)
      {
        hls_.lightness = f;
        OnReceiveMouseUp_Green (0, 0, 0, 0);
        SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    if (componentIndex == 2)
    {
      f = Clamp (f, 0.0f, 1.0f);

      if (m_color_model == color::RGB)
      {
        rgb_.blue = f;
        SetRGB (rgb_.red, rgb_.green, rgb_.blue);
      }
      else if (m_color_model == color::HSV)
      {
        hsv_.value = f;
        OnReceiveMouseUp_Blue (0, 0, 0, 0);
        SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
      }
      else if (m_color_model == color::HLS)
      {
        hls_.saturation = f;
        OnReceiveMouseUp_Blue (0, 0, 0, 0);
        SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
      }
    }

    if (componentIndex == 3)
    {
      float f = 0;
      f = CharToDouble (s.GetTCharPtr() );
      f = Clamp (f, 0.0f, 1.0f);
      //if(m_color_model == color::RGB)
      {
        alpha_ = f;
        SetAlpha (alpha_);
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

  void RGBValuator::SetColorFormat (color::Format cf)
  {
    if (cf == color::FLOAT)
    {
      m_color_format = color::FLOAT;
      m_ColorFormat->SetLabel("float");
      m_RedCaption->SetKeyEntryType (BaseKeyboardHandler::eAlphaNumeric);
      m_GreenCaption->SetKeyEntryType (BaseKeyboardHandler::eAlphaNumeric);
      m_BlueCaption->SetKeyEntryType (BaseKeyboardHandler::eAlphaNumeric);

      m_RedCaption->SetPrefix("");
      m_GreenCaption->SetPrefix("");
      m_BlueCaption->SetPrefix("");
      m_AlphaCaption->SetPrefix("");
    }

    if (cf == color::INT)
    {
      m_color_format = color::INT;
      m_ColorFormat->SetLabel("int");
      m_RedCaption->SetKeyEntryType (BaseKeyboardHandler::eIntegerNumber);
      m_GreenCaption->SetKeyEntryType (BaseKeyboardHandler::eIntegerNumber);
      m_BlueCaption->SetKeyEntryType (BaseKeyboardHandler::eIntegerNumber);

      m_RedCaption->SetPrefix (TEXT ("") );
      m_GreenCaption->SetPrefix (TEXT ("") );
      m_BlueCaption->SetPrefix (TEXT ("") );
      m_AlphaCaption->SetPrefix (TEXT ("") );
    }

    if (cf == color::HEX)
    {
      m_color_format = color::HEX;
      m_ColorFormat->SetLabel(TEXT ("hex") );
      m_RedCaption->SetKeyEntryType (BaseKeyboardHandler::eHexadecimalNumber);
      m_GreenCaption->SetKeyEntryType (BaseKeyboardHandler::eHexadecimalNumber);
      m_BlueCaption->SetKeyEntryType (BaseKeyboardHandler::eHexadecimalNumber);

      m_RedCaption->SetPrefix (TEXT ("0x") );
      m_GreenCaption->SetPrefix (TEXT ("0x") );
      m_BlueCaption->SetPrefix (TEXT ("0x") );
      m_AlphaCaption->SetPrefix (TEXT ("0x") );
    }

    if (m_color_model == color::RGB)
    {
      SetRGBA (rgb_.red, rgb_.green, rgb_.blue, alpha_);
    }
    else if (m_color_model == color::HLS)
    {
      SetHLS (hls_.hue, hls_.lightness, hls_.saturation);
      SetAlpha (alpha_);
    }
    else if (m_color_model == color::HSV)
    {
      SetHSV (hsv_.hue, hsv_.saturation, hsv_.value);
      SetAlpha (alpha_);
    }
  }

  long RGBValuator::ComputeContentSize()
  {
    return View::ComputeContentSize();
  }

  Color RGBValuator::GetColor() const
  {
    return Color(rgb_, alpha_);
  }

  void RGBValuator::EmitColorChangedSignal()
  {
    sigColorChanged.emit (rgb_.red, rgb_.green, rgb_.blue, alpha_);
  }

  bool RGBValuator::AcceptKeyNavFocus()
  {
    return false;
  }
}
