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


#include "Nux.h"
#include "NuxGraphics/GLTextureResourceManager.h"
#include "HLayout.h"
#include "VLayout.h"
#include "EditTextBox.h"
#include "Button.h"
#include "HexRegExpValidator.h"
#include "IntegerValidator.h"
#include "DoubleValidator.h"
#include "RGBValuator.h"

namespace nux
{

  RGBValuator::RGBValuator (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_color_model (CM_RGB)
    ,   m_color_format (COLORFORMAT_FLOAT)
  {
    InitializeLayout();
    SetColorModel (m_color_model);
    SetColorFormat (m_color_format);
    SetRGBA (1.0f, 1.0f, 1.0f, 1.0f);
    InitializeWidgets();
  }

  RGBValuator::RGBValuator (float red, float green, float blue, float alpha, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_color_model (CM_RGB)
    ,   m_color_format (COLORFORMAT_FLOAT)
  {
    InitializeLayout();
    SetColorModel (m_color_model);
    SetColorFormat (m_color_format);
    SetRGBA (red, green, blue, alpha);
    InitializeWidgets();
  }

  RGBValuator::RGBValuator (Color color, NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_color_model (CM_RGB)
    ,   m_color_format (COLORFORMAT_FLOAT)
  {
    InitializeLayout();
    SetColorModel (m_color_model);
    SetColorFormat (m_color_format);
    SetRGBA (color);
    InitializeWidgets();
  }

  RGBValuator::RGBValuator (eColorModel colorModel, float x, float y, float z, float alpha,    NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_color_model (colorModel)
    ,   m_color_format (COLORFORMAT_FLOAT)
  {
    InitializeLayout();

    switch (m_color_model)
    {
      case CM_HSV:
      {
        SetColorModel (CM_HSV);
        SetHSV (x, y, z);
        SetAlpha (alpha);
      }
      case CM_HLS:
      {
        SetColorModel (CM_HLS);
        SetHLS (x, y, z);
        SetAlpha (alpha);
      }
      default:
      case CM_RGB:
      {
        SetColorModel (CM_RGB);
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
    greenlayout         = new HLayout (TEXT (""), NUX_TRACKER_LOCATION);
    bluelayout          = new HLayout (TEXT (""), NUX_TRACKER_LOCATION);
    alphalayout         = new HLayout (TEXT (""), NUX_TRACKER_LOCATION);
    vlayout             = new VLayout (TEXT ("RGBVLayout"), NUX_TRACKER_LOCATION);
    colormodel_layout   = new VLayout (TEXT ("ColorModel"), NUX_TRACKER_LOCATION);

    m_ColorModel        = new Button();
    m_ColorFormat       = new Button();

    m_RedCaption        = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_GreenCaption      = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_BlueCaption       = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);
    m_AlphaCaption      = new EditTextBox (TEXT (""), NUX_TRACKER_LOCATION);

    m_RedValuator       = new CoreArea (NUX_TRACKER_LOCATION);
    m_GreenValuator     = new CoreArea (NUX_TRACKER_LOCATION);
    m_BlueValuator      = new CoreArea (NUX_TRACKER_LOCATION);
    m_AlphaValuator     = new CoreArea (NUX_TRACKER_LOCATION);
    m_ColorSquare       = new CoreArea (NUX_TRACKER_LOCATION);

    m_ComponentLabel0   = new CoreArea (NUX_TRACKER_LOCATION);
    m_ComponentLabel1   = new CoreArea (NUX_TRACKER_LOCATION);
    m_ComponentLabel2   = new CoreArea (NUX_TRACKER_LOCATION);
    m_ComponentAlpha    = new CoreArea (NUX_TRACKER_LOCATION);
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
    m_RedValuator->OnMouseDown.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDown_Red) );
    m_GreenValuator->OnMouseDown.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDown_Green) );
    m_BlueValuator->OnMouseDown.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDown_Blue) );
    m_AlphaValuator->OnMouseDown.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDown_Alpha) );
    m_RedValuator->OnMouseDrag.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDrag_Red) );
    m_GreenValuator->OnMouseDrag.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDrag_Green) );
    m_BlueValuator->OnMouseDrag.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDrag_Blue) );
    m_AlphaValuator->OnMouseDrag.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseDrag_Alpha) );
    m_ColorModel->sigClick.connect (sigc::mem_fun (this, &RGBValuator::OnChangeColorModel) );
    m_ColorFormat->sigClick.connect (sigc::mem_fun (this, &RGBValuator::OnChangeColorFormat) );
//    m_ColorModel->OnMouseDown.connect(sigc::mem_fun(this, &RGBValuator::RecvColorModelEvent));
//    m_ColorModel->OnMouseUp.connect(sigc::mem_fun(this, &RGBValuator::RecvColorModelEvent));
//    m_ColorModel->OnMouseEnter.connect(sigc::mem_fun(this, &RGBValuator::RecvColorModelEvent));

    m_ColorModel->SetFont (GetSysBoldFont() );
    m_ColorFormat->SetFont (GetSysBoldFont() );

    m_RedValuator->OnMouseUp.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseUp_Red) );
    m_GreenValuator->OnMouseUp.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseUp_Green) );
    m_BlueValuator->OnMouseUp.connect (sigc::mem_fun (this, &RGBValuator::OnReceiveMouseUp_Blue) );

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
    BaseTexture* CheckboardPattern = GetThreadGLDeviceFactory()->CreateSystemCapableTexture ();
    CheckboardPattern->Update (&image);

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_CheckboardLayer = new TextureLayer (CheckboardPattern->GetDeviceTexture(), texxform, Color::White);

    CheckboardPattern->UnReference ();
  }

  void RGBValuator::DestroyLayout()
  {
  }

  RGBValuator::~RGBValuator()
  {
    NUX_SAFE_DELETE (m_CheckboardLayer);
    DestroyLayout();
  }

  long RGBValuator::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = m_RedValuator->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_GreenValuator->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_BlueValuator->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_AlphaValuator->OnEvent (ievent, ret, ProcessEventInfo);
    ret = m_ColorModel->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_ColorFormat->BaseProcessEvent (ievent, ret, ProcessEventInfo);

    ret = m_RedCaption->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_GreenCaption->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_BlueCaption->BaseProcessEvent (ievent, ret, ProcessEventInfo);
    ret = m_AlphaCaption->BaseProcessEvent (ievent, ret, ProcessEventInfo);

    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void RGBValuator::DrawRedMarker (GraphicsEngine &GfxContext)
  {
    int marker_position_x;
    int marker_position_y;

    float percent = 0.0f;

    if (m_color_model == CM_RGB)
      percent = m_Red;

    if (m_color_model == CM_HSV)
      percent = m_HSVHue;

    if (m_color_model == CM_HLS)
      percent = m_HLSHue;

    GfxContext.PushClippingRectangle (m_RedValuator->GetGeometry() );

    marker_position_x = m_RedValuator->GetBaseX() + percent * m_RedValuator->GetBaseWidth();
    marker_position_y = m_RedValuator->GetBaseY() + m_RedValuator->GetBaseHeight();
    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color (0xFF000000) );

    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color (0.7f, 0.7f, 0.7f, 1.0f) );

    GfxContext.PopClippingRectangle();
  }

  void RGBValuator::DrawGreenMarker (GraphicsEngine &GfxContext)
  {
    int marker_position_x;
    int marker_position_y;

    float percent = 0.0f;

    if (m_color_model == CM_RGB)
      percent = m_Green;

    if (m_color_model == CM_HSV)
      percent = m_HSVSaturation;

    if (m_color_model == CM_HLS)
      percent = m_HLSLight;

    GfxContext.PushClippingRectangle (m_GreenValuator->GetGeometry() );

    marker_position_x = m_GreenValuator->GetBaseX() + percent * m_GreenValuator->GetBaseWidth();
    marker_position_y = m_GreenValuator->GetBaseY() + m_GreenValuator->GetBaseHeight();
    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color (0.0f, 0.0f, 0.0f, 1.0f) );

    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color (0.7f, 0.7f, 0.7f, 1.0f) );

    GfxContext.PopClippingRectangle();
  }

  void RGBValuator::DrawBlueMarker (GraphicsEngine &GfxContext)
  {
    int marker_position_x;
    int marker_position_y;

    float percent = 0.0f;

    if (m_color_model == CM_RGB)
      percent = m_Blue;

    if (m_color_model == CM_HSV)
      percent = m_HSVValue;

    if (m_color_model == CM_HLS)
      percent = m_HLSSaturation;

    GfxContext.PushClippingRectangle (m_BlueValuator->GetGeometry() );

    marker_position_x = m_BlueValuator->GetBaseX() + percent * m_BlueValuator->GetBaseWidth();
    marker_position_y = m_BlueValuator->GetBaseY() + m_BlueValuator->GetBaseHeight();
    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color (0.0f, 0.0f, 0.0f, 1.0f) );

    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color (0.7f, 0.7f, 0.7f, 1.0f) );

    GfxContext.PopClippingRectangle();
  }

  void RGBValuator::DrawAlphaMarker (GraphicsEngine &GfxContext)
  {
    int marker_position_x;
    int marker_position_y;

    GfxContext.PushClippingRectangle (m_AlphaValuator->GetGeometry() );

    marker_position_x = m_AlphaValuator->GetBaseX() + m_Alpha * m_AlphaValuator->GetBaseWidth();
    marker_position_y = m_AlphaValuator->GetBaseY() + m_AlphaValuator->GetBaseHeight();
    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 5, marker_position_y,
                                  marker_position_x, marker_position_y - 5,
                                  marker_position_x + 5, marker_position_y, Color (0.0f, 0.0f, 0.0f, 1.0f) );

    GetPainter().Draw2DTriangleColor (GfxContext, marker_position_x - 4, marker_position_y,
                                  marker_position_x, marker_position_y - 4,
                                  marker_position_x + 4, marker_position_y, Color (0.7f, 0.7f, 0.7f, 1.0f) );

    GfxContext.PopClippingRectangle();
  }

  void RGBValuator::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GfxContext.PushClippingRectangle (base);
    GetPainter().PushDrawShapeLayer (GfxContext, vlayout->GetGeometry(), eSHAPE_CORNER_ROUND4, Color (0xFF000000), eAllCorners);

    if (m_color_model == CM_RGB)
    {
      DrawRGB (GfxContext);
    }
    else if (m_color_model == CM_HSV)
    {
      DrawHSV (GfxContext);
    }
    else if (m_color_model == CM_HLS)
    {
      DrawHLS (GfxContext);
    }

    GetPainter().PaintTextLineStatic (GfxContext, GetSysBoldFont(), m_ComponentLabel0->GetGeometry(), m_ComponentLabel0->GetBaseString().GetTCharPtr(), Color (0xFFFFFFFF) );
    GetPainter().PaintTextLineStatic (GfxContext, GetSysBoldFont(), m_ComponentLabel1->GetGeometry(), m_ComponentLabel1->GetBaseString().GetTCharPtr(), Color (0xFFFFFFFF) );
    GetPainter().PaintTextLineStatic (GfxContext, GetSysBoldFont(), m_ComponentLabel2->GetGeometry(), m_ComponentLabel2->GetBaseString().GetTCharPtr(), Color (0xFFFFFFFF) );
    GetPainter().PaintTextLineStatic (GfxContext, GetSysBoldFont(), m_ComponentAlpha->GetGeometry(), m_ComponentAlpha->GetBaseString().GetTCharPtr(), Color (0xFFFFFFFF) );

    DrawRedMarker (GfxContext);
    DrawGreenMarker (GfxContext);
    DrawBlueMarker (GfxContext);
    DrawAlphaMarker (GfxContext);

    m_RedCaption->NeedRedraw();
    m_GreenCaption->NeedRedraw();
    m_BlueCaption->NeedRedraw();
    m_AlphaCaption->NeedRedraw();
    m_ColorModel->NeedRedraw();
    m_ColorFormat->NeedRedraw();

    GetPainter().PopBackground();
    GfxContext.PopClippingRectangle();
  }

  void RGBValuator::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle (base);

    m_RedCaption->ProcessDraw (GfxContext, force_draw);
    m_GreenCaption->ProcessDraw (GfxContext, force_draw);
    m_BlueCaption->ProcessDraw (GfxContext, force_draw);
    m_AlphaCaption->ProcessDraw (GfxContext, force_draw);

    m_ColorModel->ProcessDraw (GfxContext, force_draw); // the button has round corner. That is why we need to push the background.
    m_ColorFormat->ProcessDraw (GfxContext, force_draw); // the button has round corner. That is why we need to push the background.
    GfxContext.PopClippingRectangle();
  }

  void RGBValuator::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void RGBValuator::DrawRGB (GraphicsEngine &GfxContext)
  {
    // Red
    Geometry P = m_RedValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (0.0f, m_Green, m_Blue), Color (0.0f, m_Green, m_Blue),
                               Color (1.0f, m_Green, m_Blue), Color (1.0f, m_Green, m_Blue) );

    // Green
    P = m_GreenValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (m_Red, 0.0f, m_Blue), Color (m_Red, 0.0f, m_Blue),
                               Color (m_Red, 1.0f, m_Blue), Color (m_Red, 1.0f, m_Blue) );

    // Blue
    P = m_BlueValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (m_Red, m_Green, 0.0f), Color (m_Red, m_Green, 0.0f),
                               Color (m_Red, m_Green, 1.0f), Color (m_Red, m_Green, 1.0f) );

    // Alpha
    P = m_AlphaValuator->GetGeometry();
    m_CheckboardLayer->SetGeometry (P);
    m_CheckboardLayer->Renderlayer (GfxContext);

    GfxContext.GetRenderStates().SetBlend (true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (0.0f, 0.0f, 0.0f, 0.0f), Color (0.0f, 0.0f, 0.0f, 0.0f),
                               Color (m_Red, m_Green, m_Blue, 1.0f), Color (m_Red, m_Green, m_Blue, 1.0f) );
    GfxContext.GetRenderStates().SetBlend (false);

    P = m_ColorSquare->GetGeometry();
    Geometry shadow_quad = P;
    P.OffsetSize (-2, -2);
    shadow_quad.OffsetSize (-2, -2);
    shadow_quad.OffsetPosition (2, 2);
    GetPainter().Paint2DQuadColor (GfxContext, shadow_quad, Color (0, 0, 0) );
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (m_Red, m_Green, m_Blue), Color (m_Red, m_Green, m_Blue),
                               Color (m_Red, m_Green, m_Blue), Color (m_Red, m_Green, m_Blue) );
    GetPainter().Paint2DQuadWireframe (GfxContext, P, Color (0, 0, 0) );

    m_ColorModel->ProcessDraw (GfxContext, true);
    m_ColorFormat->ProcessDraw (GfxContext, true);
  }
  void RGBValuator::DrawHSV (GraphicsEngine &GfxContext)
  {
    // Red
    Geometry P = m_RedValuator->GetGeometry();
    float s = 0; //XSI: 1.0f - m_HSVSaturation;
    float v = 1; //XSI: m_HSVValue;
    float fw = P.GetWidth() / 6;

    Geometry p = Geometry (P.x, P.y, fw, P.GetHeight() );
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (1.0f * v, s * v, s * v), Color (1.0f * v, s * v, s * v), Color (1.0f * v, 1.0f * v, s * v), Color (1.0f * v, 1.0f * v, s * v) );
    p.SetX (P.x + fw);
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (1.0f * v, 1.0f * v, s * v), Color (1.0f * v, 1.0f * v, s * v), Color (s * v, 1.0f * v, s * v), Color (s * v, 1.0f * v, s * v) );

    p.SetX (P.x + 2 * fw);
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (s * v, 1.0f * v, s * v), Color (s * v, 1.0f * v, s * v), Color (s * v, 1.0f * v, 1.0f * v), Color (s * v, 1.0f * v, 1.0f * v) );
    p.SetX (P.x + 3 * fw);
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (s * v, 1.0f * v, 1.0f * v), Color (s * v, 1.0f * v, 1.0f * v), Color (s * v, s * v, 1.0f * v), Color (s * v, s * v, 1.0f * v) );

    p.SetX (P.x + 4 * fw);
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (s * v, s * v, 1.0f * v), Color (s * v, s * v, 1.0f * v), Color (1.0f * v, s * v, 1.0f * v), Color (1.0f * v, s * v, 1.0f * v) );
    p.SetX (P.x + 5 * fw);
    p.SetWidth (P.GetWidth() - 5 * fw); // correct rounding errors
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (1.0f * v, s * v, 1.0f * v), Color (1.0f * v, s * v, 1.0f * v), Color (1.0f * v, s * v, s * v), Color (1.0f * v, s * v, s * v) );

    s = 1.0f - m_HSVSaturation;
    v = m_HSVValue;

    float r, g, b;
    float Hue;

    if (m_HSVHue == 1.0f)
      Hue = 0.0f;
    else
      Hue = m_HSVHue;

    HSVtoRGB (r, g, b, Hue, 1.0f, 1.0f);

    // Green
    P = m_GreenValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (1.0f * v, 1.0f * v, 1.0f * v), Color (1.0f * v, 1.0f * v, 1.0f * v), Color (r * v, g * v, b * v), Color (r * v, g * v, b * v) );

    HSVtoRGB (r, g, b, Hue, m_HSVSaturation, 1.0f);
    // Blue
    P = m_BlueValuator->GetGeometry();
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (0.0f, 0.0f, 0.0f), Color (0.0f, 0.0f, 0.0f), Color (r, g, b), Color (r, g, b) );

    HSVtoRGB (r, g, b, Hue, m_HSVSaturation, m_HSVValue);

    // Alpha
    P = m_AlphaValuator->GetGeometry();
    m_CheckboardLayer->SetGeometry (P);
    m_CheckboardLayer->Renderlayer (GfxContext);

    GfxContext.GetRenderStates().SetBlend (true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (0.0f, 0.0f, 0.0f, 0.0f), Color (0.0f, 0.0f, 0.0f, 0.0f), Color (r, g, b, 1.0f), Color (r, g, b, 1.0f) );
    GfxContext.GetRenderStates().SetBlend (false);


    P = m_ColorSquare->GetGeometry();
    Geometry shadow_quad = P;
    P.OffsetSize (-2, -2);
    shadow_quad.OffsetSize (-2, -2);
    shadow_quad.OffsetPosition (2, 2);
    GetPainter().Paint2DQuadColor (GfxContext, shadow_quad, Color (0, 0, 0) );
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (r, g, b) );
    GetPainter().Paint2DQuadWireframe (GfxContext, P, Color (0, 0, 0) );

    m_ColorModel->ProcessDraw (GfxContext, true);
    m_ColorFormat->ProcessDraw (GfxContext, true);
  }

  void RGBValuator::DrawHLS (GraphicsEngine &GfxContext)
  {
    // Red
    Geometry P = m_RedValuator->GetGeometry();
    float s = 0; //XSI: 1.0f - m_HLSSaturation;
    float l = 1; //XSI: m_HLSLight;
    float fw = P.GetWidth() / 6;

    Geometry p = Geometry (P.x, P.y, fw, P.GetHeight() );
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (1.0f * l, s * l, s * l), Color (1.0f * l, s * l, s * l), Color (1.0f * l, 1.0f * l, s * l), Color (1.0f * l, 1.0f * l, s * l) );
    p.SetX (P.x + fw);
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (1.0f * l, 1.0f * l, s * l), Color (1.0f * l, 1.0f * l, s * l), Color (s * l, 1.0f * l, s * l), Color (s * l, 1.0f * l, s * l) );

    p.SetX (P.x + 2 * fw);
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (s * l, 1.0f * l, s * l), Color (s * l, 1.0f * l, s * l), Color (s * l, 1.0f * l, 1.0f * l), Color (s * l, 1.0f * l, 1.0f * l) );
    p.SetX (P.x + 3 * fw);
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (s * l, 1.0f * l, 1.0f * l), Color (s * l, 1.0f * l, 1.0f * l), Color (s * l, s * l, 1.0f * l), Color (s * l, s * l, 1.0f * l) );

    p.SetX (P.x + 4 * fw);
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (s * l, s * l, 1.0f * l), Color (s * l, s * l, 1.0f * l), Color (1.0f * l, s * l, 1.0f * l), Color (1.0f * l, s * l, 1.0f * l) );
    p.SetX (P.x + 5 * fw);
    p.SetWidth (P.GetWidth() - 5 * fw); // correct rounding errors
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (1.0f * l, s * l, 1.0f * l), Color (1.0f * l, s * l, 1.0f * l), Color (1.0f * l, s * l, s * l), Color (1.0f * l, s * l, s * l) );

    s = 1.0f - m_HLSSaturation;
    l = m_HLSLight;

    float r, g, b;
    float Hue;

    if (m_HLSHue == 1.0f)
      Hue = 0.0f;
    else
      Hue = m_HLSHue;

    // Need to use HSVtoRGB to compute the color
    HSVtoRGB (r, g, b, Hue, 1.0f, 1.0f);

    // Green
    P = m_GreenValuator->GetGeometry();
    fw = P.GetWidth() / 2;
    p = Geometry (P.x, P.y, fw, P.GetHeight() );
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (0.0f, 0.0f, 0.0f), Color (0.0f, 0.0f, 0.0f), Color (r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s), Color (r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s) );
    p.SetX (P.x + fw);
    GetPainter().Paint2DQuadColor (GfxContext, p, Color (r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s), Color (r* (1 - s) + 0.5f * s, g* (1 - s) + 0.5f * s, b* (1 - s) + 0.5f * s), Color (1.0f, 1.0f, 1.0f), Color (1.0f, 1.0f, 1.0f) );


    //HLStoRGB(r, g, b, Hue, m_HSVSaturation, 1.0f);
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
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (l, l, l), Color (l, l, l), Color (cr, cg, cb), Color (cr, cg, cb) );

    HLStoRGB (r, g, b, Hue, m_HLSLight, m_HLSSaturation);

    // Alpha
    P = m_AlphaValuator->GetGeometry();
    m_CheckboardLayer->SetGeometry (P);
    m_CheckboardLayer->Renderlayer (GfxContext);

    GfxContext.GetRenderStates().SetBlend (true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (0.0f, 0.0f, 0.0f, 0.0f), Color (0.0f, 0.0f, 0.0f, 0.0f), Color (r, g, b, 1.0f), Color (r, g, b, 1.0f) );
    GfxContext.GetRenderStates().SetBlend (false);


    P = m_ColorSquare->GetGeometry();
    Geometry shadow_quad = P;
    P.OffsetSize (-2, -2);
    shadow_quad.OffsetSize (-2, -2);
    shadow_quad.OffsetPosition (2, 2);
    GetPainter().Paint2DQuadColor (GfxContext, shadow_quad, Color (0, 0, 0) );
    GetPainter().Paint2DQuadColor (GfxContext, P, Color (r, g, b) );
    GetPainter().Paint2DQuadWireframe (GfxContext, P, Color (0, 0, 0) );

    m_ColorModel->ProcessDraw (GfxContext, true);
    m_ColorFormat->ProcessDraw (GfxContext, true);
  }

  void RGBValuator::SetRGBA (Color color)
  {
    SetRGBA (color.R(), color.G(), color.B(), color.A() );
  }

  void RGBValuator::SetRGBA (float r, float g, float b, float a)
  {
    SetRGB (r, g, b);
    SetAlpha (a);
  }

  void RGBValuator::SetRGB (Color color)
  {
    SetRGB (color.R(), color.G(), color.B() );
  }

  void RGBValuator::SetRGB (float r, float g, float b)
  {
    m_Red   = Clamp (r, 0.0f, 1.0f);
    m_Green = Clamp (g, 0.0f, 1.0f);
    m_Blue  = Clamp (b, 0.0f, 1.0f);

    bool RedEditSelected = m_RedCaption->IsTextSelected();
    bool GreenEditSelected = m_GreenCaption->IsTextSelected();
    bool BlueEditSelected = m_BlueCaption->IsTextSelected();

    if (m_color_format == COLORFORMAT_HEX)
    {
      m_RedCaption->SetText (inlPrintf ("%x", (int) (m_Red * 255) ) );
      m_GreenCaption->SetText (inlPrintf ("%x", (int) (m_Green * 255) ) );
      m_BlueCaption->SetText (inlPrintf ("%x", (int) (m_Blue * 255) ) );
    }
    else if (m_color_format == COLORFORMAT_INT)
    {
      m_RedCaption->SetText (inlPrintf ("%d", (int) (m_Red * 255) ) );
      m_GreenCaption->SetText (inlPrintf ("%d", (int) (m_Green * 255) ) );
      m_BlueCaption->SetText (inlPrintf ("%d", (int) (m_Blue * 255) ) );
    }
    else
    {
      m_RedCaption->SetText (inlPrintf ("%.3f", m_Red) );
      m_GreenCaption->SetText (inlPrintf ("%.3f", m_Green) );
      m_BlueCaption->SetText (inlPrintf ("%.3f", m_Blue) );
    }

    // Restore text selection if necessary.
    // This solves a problem when an EditTextBox of the widget gets the focus and has its text selected
    // but another who is losing the focus will cause it to lose the text selection by calling SetRGB.
    if (RedEditSelected) m_RedCaption->m_KeyboardHandler.SelectAllText();

    if (GreenEditSelected) m_GreenCaption->m_KeyboardHandler.SelectAllText();

    if (BlueEditSelected) m_BlueCaption->m_KeyboardHandler.SelectAllText();

    sigColorChanged.emit (m_Red, m_Green, m_Blue, m_Alpha);
  }

  void RGBValuator::SetHSV (float h, float s, float v)
  {
    m_HSVHue        = Clamp (h, 0.0f, 1.0f);
    m_HSVSaturation = Clamp (s, 0.0f, 1.0f);
    m_HSVValue      = Clamp (v, 0.0f, 1.0f);

    bool RedEditSelected = m_RedCaption->IsTextSelected();
    bool GreenEditSelected = m_GreenCaption->IsTextSelected();
    bool BlueEditSelected = m_BlueCaption->IsTextSelected();

    if (m_color_format == COLORFORMAT_HEX)
    {
      m_RedCaption->SetText (inlPrintf ("%x", (int) (m_HSVHue * 255) ) );
      m_GreenCaption->SetText (inlPrintf ("%x", (int) (m_HSVSaturation * 255) ) );
      m_BlueCaption->SetText (inlPrintf ("%x", (int) (m_HSVValue * 255) ) );
    }
    else if (m_color_format == COLORFORMAT_INT)
    {
      m_RedCaption->SetText (inlPrintf ("%d", (int) (m_HSVHue * 255) ) );
      m_GreenCaption->SetText (inlPrintf ("%d", (int) (m_HSVSaturation * 255) ) );
      m_BlueCaption->SetText (inlPrintf ("%d", (int) (m_HSVValue * 255) ) );
    }
    else
    {
      m_RedCaption->SetText (inlPrintf ("%.3f", m_HSVHue) );
      m_GreenCaption->SetText (inlPrintf ("%.3f", m_HSVSaturation) );
      m_BlueCaption->SetText (inlPrintf ("%.3f", m_HSVValue) );
    }

    if (m_HSVHue >= 1.0f)
    {
      m_HSVHue = 1.0f;
      // XSI: m_HSVHue = 0.0f;
    }

    if (RedEditSelected) m_RedCaption->m_KeyboardHandler.SelectAllText();

    if (GreenEditSelected) m_GreenCaption->m_KeyboardHandler.SelectAllText();

    if (BlueEditSelected) m_BlueCaption->m_KeyboardHandler.SelectAllText();

    HSVtoRGB (m_Red, m_Green, m_Blue, m_HSVHue, m_HSVSaturation, m_HSVValue);
    sigColorChanged.emit (m_Red, m_Green, m_Blue, m_Alpha);
  }

  void RGBValuator::SetHLS (float h, float l, float s)
  {

    m_HLSHue        = Clamp (h, 0.0f, 1.0f);
    m_HLSLight      = Clamp (l, 0.0f, 1.0f);
    m_HLSSaturation = Clamp (s, 0.0f, 1.0f);

    bool RedEditSelected = m_RedCaption->IsTextSelected();
    bool GreenEditSelected = m_GreenCaption->IsTextSelected();
    bool BlueEditSelected = m_BlueCaption->IsTextSelected();

    if (m_color_format == COLORFORMAT_HEX)
    {
      m_RedCaption->SetText (inlPrintf (TEXT ("%x"), (int) (m_HLSHue * 255) ) );
      m_GreenCaption->SetText (inlPrintf ("%x", (int) (m_HLSLight * 255) ) );
      m_BlueCaption->SetText (inlPrintf ("%x", (int) (m_HLSSaturation * 255) ) );
    }
    else if (m_color_format == COLORFORMAT_INT)
    {
      m_RedCaption->SetText (inlPrintf (TEXT ("%d"), (int) (m_HLSHue * 255) ) );
      m_GreenCaption->SetText (inlPrintf ("%d", (int) (m_HLSLight * 255) ) );
      m_BlueCaption->SetText (inlPrintf ("%d", (int) (m_HLSSaturation * 255) ) );
    }
    else
    {
      m_RedCaption->SetText (inlPrintf (TEXT ("%.3f"), m_HLSHue) );
      m_GreenCaption->SetText (inlPrintf ("%.3f", m_HLSLight) );
      m_BlueCaption->SetText (inlPrintf ("%.3f", m_HLSSaturation) );
    }

    if (m_HLSHue >= 1.0f)
    {
      m_HLSHue = 1.0f;
      // XSI: m_HLSHue = 0.0f;
    }

    if (RedEditSelected) m_RedCaption->m_KeyboardHandler.SelectAllText();

    if (GreenEditSelected) m_GreenCaption->m_KeyboardHandler.SelectAllText();

    if (BlueEditSelected) m_BlueCaption->m_KeyboardHandler.SelectAllText();

    HLStoRGB (m_Red, m_Green, m_Blue, m_HLSHue, m_HLSLight, m_HLSSaturation);
    sigColorChanged.emit (m_Red, m_Green, m_Blue, m_Alpha);
  }

  void RGBValuator::SetAlpha (float alpha)
  {
    m_Alpha = Clamp (alpha, 0.0f, 1.0f);


    if (m_color_format == COLORFORMAT_HEX)
    {
      m_AlphaCaption->SetText (inlPrintf ("%x", (int) (m_Alpha * 255) ) );
    }
    else if (m_color_format == COLORFORMAT_INT)
    {
      m_AlphaCaption->SetText (inlPrintf ("%d", (int) (m_Alpha * 255) ) );
    }
    else
    {
      m_AlphaCaption->SetText (inlPrintf ("%.3f", m_Alpha) );
    }

    sigColorChanged.emit (m_Red, m_Green, m_Blue, m_Alpha);
  }

  void RGBValuator::OnReceiveMouseDown_Red (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_color_model == CM_RGB)
    {
      if (x < 0)
        m_Red = 0.0f;
      else if (x > m_RedValuator->GetBaseWidth() )
        m_Red = 1.0f;
      else
        m_Red = (float) x / (float) m_RedValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetRGB (m_Red, m_Red, m_Red);
      }
      else
      {
        SetRGB (m_Red, m_Green, m_Blue);
      }
    }
    else if (m_color_model == CM_HSV)
    {
      if (x < 0)
        m_HSVHue = 0.0f;
      else if (x > m_RedValuator->GetBaseWidth() )
        m_HSVHue = 1.0f;
      else
        m_HSVHue = (float) x / (float) m_RedValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHSV (m_HSVHue, m_HSVHue, m_HSVHue);
      }
      else
      {
        SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
      }
    }
    else if (m_color_model == CM_HLS)
    {
      if (x < 0)
        m_HLSHue = 0.0f;
      else if (x > m_RedValuator->GetBaseWidth() )
        m_HLSHue = 1.0f;
      else
        m_HLSHue = (float) x / (float) m_RedValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHLS (m_HLSHue, m_HLSHue, m_HLSHue);
      }
      else
      {
        SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
      }
    }

    NeedRedraw();
  }

  void RGBValuator::OnReceiveMouseDown_Green (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_color_model == CM_RGB)
    {
      if (x < 0)
        m_Green = 0.0f;
      else if (x > m_GreenValuator->GetBaseWidth() )
        m_Green = 1.0f;
      else
        m_Green = (float) x / (float) m_GreenValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetRGB (m_Green, m_Green, m_Green);
      }
      else
      {
        SetRGB (m_Red, m_Green, m_Blue);
      }
    }
    else if (m_color_model == CM_HSV)
    {
      if (x < 0)
        m_HSVSaturation = 0.0f;
      else if (x > m_GreenValuator->GetBaseWidth() )
        m_HSVSaturation = 1.0f;
      else
        m_HSVSaturation = (float) x / (float) m_GreenValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHSV (m_HSVSaturation, m_HSVSaturation, m_HSVSaturation);
      }
      else
      {
        SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
      }
    }
    else if (m_color_model == CM_HLS)
    {
      if (x < 0)
        m_HLSLight = 0.0f;
      else if (x > m_GreenValuator->GetBaseWidth() )
        m_HLSLight = 1.0f;
      else
        m_HLSLight = (float) x / (float) m_GreenValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHLS (m_HLSLight, m_HLSLight, m_HLSLight);
      }
      else
      {
        SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
      }
    }

    NeedRedraw();
  }

  void RGBValuator::OnReceiveMouseDown_Blue (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_color_model == CM_RGB)
    {
      if (x < 0)
        m_Blue = 0.0f;
      else if (x > m_BlueValuator->GetBaseWidth() )
        m_Blue = 1.0f;
      else
        m_Blue = (float) x / (float) m_BlueValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetRGB (m_Blue, m_Blue, m_Blue);
      }
      else
      {
        SetRGB (m_Red, m_Green, m_Blue);
      }
    }
    else if (m_color_model == CM_HSV)
    {
      if (x < 0)
        m_HSVValue = 0.0f;
      else if (x > m_BlueValuator->GetBaseWidth() )
        m_HSVValue = 1.0f;
      else
        m_HSVValue = (float) x / (float) m_BlueValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHSV (m_HSVValue, m_HSVValue, m_HSVValue);
      }
      else
      {
        SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
      }
    }
    else if (m_color_model == CM_HLS)
    {
      if (x < 0)
        m_HLSSaturation = 0.0f;
      else if (x > m_BlueValuator->GetBaseWidth() )
        m_HLSSaturation = 1.0f;
      else
        m_HLSSaturation = (float) x / (float) m_BlueValuator->GetBaseWidth();

      if (key_flags & NUX_STATE_CTRL)
      {
        SetHLS (m_HLSSaturation, m_HLSSaturation, m_HLSSaturation);
      }
      else
      {
        SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
      }
    }

    NeedRedraw();
  }

  void RGBValuator::OnReceiveMouseDown_Alpha (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (x < 0)
      m_Alpha = 0.0f;
    else if (x > m_AlphaValuator->GetBaseWidth() )
      m_Alpha = 1.0f;
    else
      m_Alpha = (float) x / (float) m_AlphaValuator->GetBaseWidth();

    SetAlpha (m_Alpha);
    NeedRedraw();
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
    if (m_color_model == CM_HSV)
    {
      if (m_HSVHue >= 1.0f)
      {
        m_HSVHue = 1.0f;
        // XSI: m_HSVHue = 0.0f;
      }

      if (m_HSVSaturation <= 0.0f)
      {
        // XSI: m_HSVHue = 0.0f;
      }

      if (m_HSVValue <= 0.0f)
      {
        // XSI: m_HSVHue = 0.0f;
        // XSI: m_HSVSaturation = 0.0f;
      }

      SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
    }

    if (m_color_model == CM_HLS)
    {
      if (m_HLSHue >= 1.0f)
      {
        m_HLSHue = 1.0f;
        // XSI: m_HLSHue = 0.0f;
      }

      if (m_HLSSaturation <= 0.0f)
      {
        // XSI: m_HLSHue = 0.0f;
      }

      if (m_HLSLight <= 0.0f || m_HLSLight >= 1.0f)
      {
        // XSI: m_HLSHue = 0.0f;
        // XSI: m_HLSSaturation = 0.0f;
      }

      SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
    }

    NeedRedraw();
  }

  void RGBValuator::OnReceiveMouseUp_Green     (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_color_model == CM_HSV)
    {
      if (m_HSVHue >= 1.0f)
      {
        m_HSVHue = 1.0f;
        // XSI: m_HSVHue = 0.0f;
      }

      if (m_HSVSaturation <= 0.0f)
      {
        // XSI: m_HSVHue = 0.0f;
      }

      if (m_HSVValue <= 0.0f)
      {
        // XSI: m_HSVHue = 0.0f;
        // XSI: m_HSVSaturation = 0.0f;
      }

      SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
    }

    if (m_color_model == CM_HLS)
    {
      if (m_HLSHue >= 1.0f)
      {
        m_HLSHue = 1.0f;
        // XSI: m_HLSHue = 0.0f;
      }

      if (m_HLSSaturation <= 0.0f)
      {
        // XSI: m_HLSHue = 0.0f;
      }

      if (m_HLSLight <= 0.0f || m_HLSLight >= 1.0f)
      {
        // XSI: m_HLSHue = 0.0f;
        // XSI: m_HLSSaturation = 0.0f;
      }

      SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
    }

    NeedRedraw();
  }

  void RGBValuator::OnReceiveMouseUp_Blue (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_color_model == CM_HSV)
    {
      if (m_HSVHue >= 1.0f)
      {
        m_HSVHue = 1.0f;
        // XSI: m_HSVHue = 0.0f;
      }

      if (m_HSVSaturation <= 0.0f)
      {
        // XSI: m_HSVHue = 0.0f;
      }

      if (m_HSVValue <= 0.0f)
      {
        // XSI: m_HSVHue = 0.0f;
        // XSI: m_HSVSaturation = 0.0f;
      }

      SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
    }

    if (m_color_model == CM_HLS)
    {
      if (m_HLSHue >= 1.0f)
      {
        m_HLSHue = 1.0f;
        // XSI: m_HLSHue = 0.0f;
      }

      if (m_HLSSaturation <= 0.0f)
      {
        // XSI: m_HLSHue = 0.0f;
      }

      if (m_HLSLight <= 0.0f || m_HLSLight >= 1.0f)
      {
        // XSI: m_HLSHue = 0.0f;
        // XSI: m_HLSSaturation = 0.0f;
      }

      SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
    }

    NeedRedraw();
  }

  void RGBValuator::RecvMouseDownColorModel (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void RGBValuator::OnChangeColorModel()
  {
    if (m_color_model == CM_RGB)
    {
      SetColorModel (CM_HLS);
      RGBtoHLS (m_Red, m_Green, m_Blue, m_HLSHue, m_HLSLight, m_HLSSaturation);
      SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
    }
    else if (m_color_model == CM_HLS)
    {
      SetColorModel (CM_HSV);
      HLStoRGB (m_Red, m_Green, m_Blue, m_HLSHue, m_HLSLight, m_HLSSaturation);
      RGBtoHSV (m_Red, m_Green, m_Blue, m_HSVHue, m_HSVSaturation, m_HSVValue);
      SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);

      if (m_HSVHue == -1.0f)
      {
        m_HSVHue = 0;
      }
    }
    else if (m_color_model == CM_HSV)
    {
      SetColorModel (CM_RGB);
      HSVtoRGB (m_Red, m_Green, m_Blue, m_HSVHue, m_HSVSaturation, m_HSVValue);
      SetRGB (m_Red, m_Green, m_Blue);
    }

    NeedRedraw();
  }

  void RGBValuator::OnChangeColorFormat()
  {
    if (m_color_format == COLORFORMAT_FLOAT)
    {
      SetColorFormat (COLORFORMAT_INT);
    }
    else if (m_color_format == COLORFORMAT_INT)
    {
      SetColorFormat (COLORFORMAT_HEX);
    }
    else if (m_color_format == COLORFORMAT_HEX)
    {
      SetColorFormat (COLORFORMAT_FLOAT);
    }

  }

  void RGBValuator::OnComponentInput (EditTextBox *textbox, const NString &s, int componentIndex)
  {
    float f = 0;

    if ( (m_color_format == COLORFORMAT_HEX) && (m_HexRegExp.Validate (s.GetTCharPtr() ) == Validator::Acceptable) )
    {
      f = (float) m_HexRegExp.ToInteger (s.GetTCharPtr() ) / 255.0f;
    }
    else if ( (m_color_format == COLORFORMAT_INT) && (m_IntRegExp.Validate (s.GetTCharPtr() ) == Validator::Acceptable) )
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

      if (m_color_model == CM_RGB)
      {
        m_Red = f;
        SetRGB (m_Red, m_Green, m_Blue);
      }
      else if (m_color_model == CM_HSV)
      {
        m_HSVHue = f;
        OnReceiveMouseUp_Red (0, 0, 0, 0);
        SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
      }
      else if (m_color_model == CM_HLS)
      {
        m_HLSHue = f;
        OnReceiveMouseUp_Red (0, 0, 0, 0);
        SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
      }
    }

    if (componentIndex == 1)
    {
      f = Clamp (f, 0.0f, 1.0f);

      if (m_color_model == CM_RGB)
      {
        m_Green = f;
        SetRGB (m_Red, m_Green, m_Blue);
      }
      else if (m_color_model == CM_HSV)
      {
        m_HSVSaturation = f;
        OnReceiveMouseUp_Green (0, 0, 0, 0);
        SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
      }
      else if (m_color_model == CM_HLS)
      {
        m_HLSLight = f;
        OnReceiveMouseUp_Green (0, 0, 0, 0);
        SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
      }
    }

    if (componentIndex == 2)
    {
      f = Clamp (f, 0.0f, 1.0f);

      if (m_color_model == CM_RGB)
      {
        m_Blue = f;
        SetRGB (m_Red, m_Green, m_Blue);
      }
      else if (m_color_model == CM_HSV)
      {
        m_HSVValue = f;
        OnReceiveMouseUp_Blue (0, 0, 0, 0);
        SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
      }
      else if (m_color_model == CM_HLS)
      {
        m_HLSSaturation = f;
        OnReceiveMouseUp_Blue (0, 0, 0, 0);
        SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
      }
    }

    if (componentIndex == 3)
    {
      float f = 0;
      f = CharToDouble (s.GetTCharPtr() );
      f = Clamp (f, 0.0f, 1.0f);
      //if(m_color_model == CM_RGB)
      {
        m_Alpha = f;
        SetAlpha (m_Alpha);
      }
    }

    NeedRedraw();
  }


  void RGBValuator::SetColorModel (eColorModel cm)
  {
    if (cm == CM_RGB)
    {
      m_color_model = CM_RGB;
      m_ColorModel->SetCaption (TEXT ("RGB") );

      m_ComponentLabel0->SetBaseString (TEXT ("R") );
      m_ComponentLabel1->SetBaseString (TEXT ("G") );
      m_ComponentLabel2->SetBaseString (TEXT ("B") );
      m_ComponentAlpha->SetBaseString (TEXT ("A") );

    }

    if (cm == CM_HSV)
    {
      m_color_model = CM_HSV;
      m_ColorModel->SetCaption (TEXT ("HSV") );

      m_ComponentLabel0->SetBaseString (TEXT ("H") );
      m_ComponentLabel1->SetBaseString (TEXT ("S") );
      m_ComponentLabel2->SetBaseString (TEXT ("V") );
      m_ComponentAlpha->SetBaseString (TEXT ("A") );
    }

    if (cm == CM_HLS)
    {
      m_color_model = CM_HLS;
      m_ColorModel->SetCaption (TEXT ("HLS") );

      m_ComponentLabel0->SetBaseString (TEXT ("H") );
      m_ComponentLabel1->SetBaseString (TEXT ("L") );
      m_ComponentLabel2->SetBaseString (TEXT ("S") );
      m_ComponentAlpha->SetBaseString (TEXT ("A") );
    }

    if (cm == CM_YUV)
    {
      m_color_model = CM_YUV;
      m_ColorModel->SetBaseString (TEXT ("YUV") );

      m_ComponentLabel0->SetBaseString (TEXT ("Y") );
      m_ComponentLabel1->SetBaseString (TEXT ("U") );
      m_ComponentLabel2->SetBaseString (TEXT ("V") );
      m_ComponentAlpha->SetBaseString (TEXT ("A") );
    }
  }

  void RGBValuator::SetColorFormat (ColorFormat cf)
  {
    if (cf == COLORFORMAT_FLOAT)
    {
      m_color_format = COLORFORMAT_FLOAT;
      m_ColorFormat->SetBaseString (TEXT ("float") );
      m_RedCaption->SetKeyEntryType (BaseKeyboardHandler::eAlphaNumeric);
      m_GreenCaption->SetKeyEntryType (BaseKeyboardHandler::eAlphaNumeric);
      m_BlueCaption->SetKeyEntryType (BaseKeyboardHandler::eAlphaNumeric);

      m_RedCaption->SetPrefix (TEXT ("") );
      m_GreenCaption->SetPrefix (TEXT ("") );
      m_BlueCaption->SetPrefix (TEXT ("") );
      m_AlphaCaption->SetPrefix (TEXT ("") );
    }

    if (cf == COLORFORMAT_INT)
    {
      m_color_format = COLORFORMAT_INT;
      m_ColorFormat->SetBaseString (TEXT ("int") );
      m_RedCaption->SetKeyEntryType (BaseKeyboardHandler::eIntegerNumber);
      m_GreenCaption->SetKeyEntryType (BaseKeyboardHandler::eIntegerNumber);
      m_BlueCaption->SetKeyEntryType (BaseKeyboardHandler::eIntegerNumber);

      m_RedCaption->SetPrefix (TEXT ("") );
      m_GreenCaption->SetPrefix (TEXT ("") );
      m_BlueCaption->SetPrefix (TEXT ("") );
      m_AlphaCaption->SetPrefix (TEXT ("") );
    }

    if (cf == COLORFORMAT_HEX)
    {
      m_color_format = COLORFORMAT_HEX;
      m_ColorFormat->SetBaseString (TEXT ("hex") );
      m_RedCaption->SetKeyEntryType (BaseKeyboardHandler::eHexadecimalNumber);
      m_GreenCaption->SetKeyEntryType (BaseKeyboardHandler::eHexadecimalNumber);
      m_BlueCaption->SetKeyEntryType (BaseKeyboardHandler::eHexadecimalNumber);

      m_RedCaption->SetPrefix (TEXT ("0x") );
      m_GreenCaption->SetPrefix (TEXT ("0x") );
      m_BlueCaption->SetPrefix (TEXT ("0x") );
      m_AlphaCaption->SetPrefix (TEXT ("0x") );
    }

    if (m_color_model == CM_RGB)
    {
      SetRGBA (m_Red, m_Green, m_Blue, m_Alpha);
    }
    else if (m_color_model == CM_HLS)
    {
      SetHLS (m_HLSHue, m_HLSLight, m_HLSSaturation);
      SetAlpha (m_Alpha);
    }
    else if (m_color_model == CM_HSV)
    {
      SetHSV (m_HSVHue, m_HSVSaturation, m_HSVValue);
      SetAlpha (m_Alpha);
    }
  }

  long RGBValuator::ComputeChildLayout()
  {
    return View::ComputeChildLayout();
  }

  Color RGBValuator::GetColor()
  {
    return Color (m_Red, m_Green, m_Blue, m_Alpha);
  }

  float RGBValuator::GetRed()
  {
    return m_Red;
  }

  float RGBValuator::GetGreen()
  {
    return m_Green;
  }

  float RGBValuator::GetBlue()
  {
    return m_Blue;
  }

  float RGBValuator::GetAlpha()
  {
    return m_Alpha;
  }

  void RGBValuator::EmitColorChangedSignal()
  {
    sigColorChanged.emit (m_Red, m_Green, m_Blue, m_Alpha);
  }

}
