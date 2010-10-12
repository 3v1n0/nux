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

#include "NuxGraphics/GLDeviceFactory.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "NuxGraphics/GLSh_DrawFunction.h"

#include "Button.h"
#include "Dialog.h"
#include "SplineCurveEditor.h"
#include "SplineCurvePreview.h"

namespace nux
{

  static const int GRAPH_MARGIN = 1;

  static void ThreadWidgetInitDialog (NThread *thread, void *InitData)
  {
    VLayout *MainLayout (new VLayout (TEXT (""), NUX_TRACKER_LOCATION) );
    SplineCurveEditor *SplineControl (new SplineCurveEditor (NUX_TRACKER_LOCATION) );
    SplineCurveDialogProxy *splinecurveproxy = static_cast<SplineCurveDialogProxy *> (InitData);

    if (splinecurveproxy)
    {
      SplineControl->SetControlPoints (splinecurveproxy->GetControlPoints() );
      SplineControl->sigCurveChange.connect (sigc::mem_fun (splinecurveproxy, &SplineCurveDialogProxy::RecvDialogChange) );
    }
    else
    {
      SplineControl->Reset();
      SplineControl->AddKnot (0.0, 0.0);
      SplineControl->AddKnot (1.0, 1.0);
    }

    HLayout *ButtonLayout = new HLayout (TEXT ("Dialog Buttons"), NUX_TRACKER_LOCATION);

    Button *OkButton = new Button (TEXT ("OK"), NUX_TRACKER_LOCATION);
    OkButton->SetMinimumWidth (60);
    OkButton->SetMinimumHeight (20);

    Button *CancelButton = new Button (TEXT ("Cancel"), NUX_TRACKER_LOCATION);
    CancelButton->SetMinimumWidth (60);
    CancelButton->SetMinimumHeight (20);

    OkButton->sigClick.connect (sigc::mem_fun (static_cast<WindowThread *> (thread), &WindowThread::TerminateThread) );
    OkButton->sigClick.connect (sigc::bind (sigc::mem_fun (splinecurveproxy, &SplineCurveDialogProxy::RecvDialogOk), SplineControl) );
    CancelButton->sigClick.connect (sigc::bind (sigc::mem_fun (splinecurveproxy, &SplineCurveDialogProxy::RecvDialogCancel), SplineControl) );
    CancelButton->sigClick.connect (sigc::mem_fun (static_cast<WindowThread *> (thread), &WindowThread::TerminateThread) );

    ButtonLayout->SetHorizontalInternalMargin (6);
    ButtonLayout->SetVerticalExternalMargin (2);
    ButtonLayout->AddView (OkButton, 0);
    ButtonLayout->AddView (CancelButton, 0);

    MainLayout->AddView (SplineControl);
    MainLayout->AddLayout (ButtonLayout, 0);
    static_cast<WindowThread *> (thread)->SetLayout (MainLayout);

    MainLayout->SetBaseWidth (1);
    MainLayout->SetBaseHeight (1);
    MainLayout->ComputeLayout2();
    static_cast<WindowThread *> (thread)->SetWindowSize (MainLayout->GetBaseWidth(), MainLayout->GetBaseHeight() );

    // Call StopThreadMonitoring in case the dialog was close by clicking the window close button.
    splinecurveproxy->StopThreadMonitoring();
  }

  SplineCurveDialogProxy::SplineCurveDialogProxy (bool ModalWindow)
    :   m_bDialogChange (false)
    ,   m_bDialogRunning (false)
    ,   m_ModalWindow (ModalWindow)
  {
    m_ControlPoints.Reset();
  }

  SplineCurveDialogProxy::~SplineCurveDialogProxy()
  {
  }

  void SplineCurveDialogProxy::Start()
  {
    m_PreviousControlPoints = m_ControlPoints;

    m_Thread = CreateModalWindowThread (WINDOWSTYLE_TOOL, TEXT ("Spline Curve"), 200, 200, GetGraphicsThread(),
                                        ThreadWidgetInitDialog,
                                        this);

    if (m_Thread)
    {
      m_DialogThreadID = m_Thread->GetThreadId();
      m_Thread->Start (0);
    }

    m_bDialogRunning = true;
  }

  bool SplineCurveDialogProxy::IsActive()
  {
    return (m_Thread && (m_Thread->GetThreadState() != THREADSTOP) && m_bDialogRunning);
  }

  void SplineCurveDialogProxy::RecvDialogOk (SplineCurveEditor *splinecurve)
  {
    m_ControlPoints = splinecurve->GetControlPoints();
    m_PreviousControlPoints = m_ControlPoints;
    m_bDialogChange = true;
    m_bDialogRunning = false;
  }

  void SplineCurveDialogProxy::RecvDialogCancel (SplineCurveEditor *splinecurve)
  {
    m_ControlPoints = m_PreviousControlPoints;
    m_bDialogChange = true;
    m_bDialogRunning = false;
  }

  void SplineCurveDialogProxy::RecvDialogChange (SplineCurveEditor *splinecurve)
  {
    m_ControlPoints = splinecurve->GetControlPoints();
    m_bDialogChange = true;
  }

  void SplineCurveDialogProxy::StopThreadMonitoring()
  {
    m_ControlPoints = m_PreviousControlPoints;
    m_bDialogChange = true;
    m_bDialogRunning = false;
    m_DialogThreadID = 0;
    m_Thread = 0;
  }

  const SplineKnot &SplineCurveDialogProxy::GetControlPoints() const
  {
    return m_ControlPoints;
  }

  void SplineCurveDialogProxy::SetControlPoints (const SplineKnot &splineknot)
  {
    m_ControlPoints = splineknot;
  }

  SplineCurvePreview::SplineCurvePreview (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_minX (0.0f)
    ,   m_minY (0.0f)
    ,   m_maxX (1.0f)
    ,   m_maxY (1.0f)
  {
    SetMinMaxSize (32, 32);
    //setSize(200, 100);

    m_control_knot.Reset();
    m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );

    Texture = GetThreadGLDeviceFactory()->CreateSystemCapableDeviceTexture (256, 4, 0, BITFMT_R8G8B8A8);
    m_DrawFunctionShader = new GLSh_DrawFunction();

    m_DialogThreadProxy = new SplineCurveDialogProxy (true);

    OnMouseClick.connect (sigc::mem_fun (this, &SplineCurvePreview::RecvClick) );

    NTextureData image;
    MakeCheckBoardImage (image.GetSurface (0), 64, 64, Color (0xff323232), Color (0xff535353), 4, 4);
    NTexture* BackgroundTexture = GetThreadGLDeviceFactory()->CreateSystemCapableTexture ();
    BackgroundTexture->Update (&image);

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_BackgroundLayer = new TextureLayer (BackgroundTexture->GetDeviceTexture(), texxform, Color::White);

    m_ChangeDetectionTimer = new TimerFunctor();
    m_ChangeDetectionTimer->OnTimerExpired.connect (sigc::mem_fun (this, &SplineCurvePreview::RecvTimer) );
    m_ChangeTimerHandler = 0;

    delete BackgroundTexture;
  }

  SplineCurvePreview::~SplineCurvePreview()
  {
    NUX_SAFE_DELETE (m_BackgroundLayer);
    NUX_SAFE_DELETE (m_DrawFunctionShader);
    NUX_SAFE_DELETE (m_DialogThreadProxy);
    NUX_SAFE_DELETE (m_ChangeDetectionTimer);
  }

  long SplineCurvePreview::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void SplineCurvePreview::Draw (GraphicsContext &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    gPainter.PaintBackground (GfxContext, base);
    gPainter.Paint2DQuadColor (GfxContext, base, Color (0xFFAAAAAA) );
    gPainter.Paint2DQuadWireframe (GfxContext, base, Color (COLOR_BACKGROUND_SECONDARY) );

    base.OffsetPosition (GRAPH_MARGIN, GRAPH_MARGIN);
    base.OffsetSize (-2 * GRAPH_MARGIN, -2 * GRAPH_MARGIN);

    int W = base.GetWidth();
    int H = base.GetHeight();
    int X = base.x;
    int Y = base.y;

    float dX = (m_maxX - m_minX) / (W - 1);

    float x0, y0;
    x0 = m_minX;

    if (m_control_knot.GetNumKnot() <= 1)
      y0 = 0.0;
    else
      y0 = m_CubicSpline.Eval (x0);

    GfxContext.PushClippingRectangle (base);

    float tex_dx = (m_maxX - m_minX) / Texture->GetWidth();
    SURFACE_LOCKED_RECT lockrect;
    Texture.Handle->LockRect (0, &lockrect, 0);
    BYTE *dest = (BYTE *) lockrect.pBits;

    for (t_s32 i = 0; i < Texture->GetWidth(); i++)
    {
      float y = 0.0;

      if (m_control_knot.GetNumKnot() <= 1)
        y = 0.0;
      else
        y = m_CubicSpline.Eval (m_minX + i * tex_dx);

      y = (y - m_minY) / (m_maxY - m_minY);

      for (t_s32 j = 0; j < Texture->GetHeight(); j++)
      {
        dest[4*i + 0 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
        dest[4*i + 1 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
        dest[4*i + 2 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
        dest[4*i + 3 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
      }
    }

    Texture->UnlockRect (0);

    m_BackgroundLayer->SetGeometry (base);
    m_BackgroundLayer->Renderlayer (GfxContext);

    GfxContext.GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_DrawFunctionShader->SetTransformMatrix (GfxContext.GetProjectionMatrix() );

    m_DrawFunctionShader->SetTextureFunction (Texture);
    m_DrawFunctionShader->SetBackgroundColor (0.1f, 0.1f, 0.1f, 0.6f);
    m_DrawFunctionShader->Render (X, Y, 0, W, H, GfxContext.GetWindowWidth(), GfxContext.GetWindowHeight() );

    GfxContext.GetRenderStates().EnableLineSmooth (TRUE, 1, GL_FASTEST);   //You need this blending formula to get anti-aliased lines
    GfxContext.GetRenderStates().SetColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

    for (int i = 1; i < W; i++)
    {
      float x1, y1;
      x1 = x0 + dX;

      if (m_control_knot.GetNumKnot() <= 1)
        y1 = 0.0;
      else
        y1 = m_CubicSpline.Eval (x1);

      int X0, Y0, X1, Y1;
      X0 = X + W * (x0 - m_minX) / (m_maxX - m_minX);
      Y0 = Y + H * ( 1 - (y0 - m_minY) / (m_maxY - m_minY) );
      X1 = X + W * (x1 - m_minX) / (m_maxX - m_minX);
      Y1 = Y + H * ( 1 - (y1 - m_minY) / (m_maxY - m_minY) );
      gPainter.Draw2DLine (GfxContext, X0, Y0, X1, Y1, Color (0xFFFFFFFF) );

      x0 = x1;
      y0 = y1;
    }

    GfxContext.GetRenderStates().EnableLineSmooth (FALSE);
    GfxContext.GetRenderStates().SetBlend (GL_FALSE);
    GfxContext.GetRenderStates().SetColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);


    GfxContext.PopClippingRectangle();
  }

  void SplineCurvePreview::DrawContent (GraphicsContext &GfxContext, bool force_draw)
  {

  }

  void SplineCurvePreview::PostDraw (GraphicsContext &GfxContext, bool force_draw)
  {

  }

  void SplineCurvePreview::SetXAxisBounds (float minX, float maxX)
  {
    m_minX = minX;
    m_maxX = maxX;
    NeedRedraw();
  }

  void SplineCurvePreview::SetYAxisBounds (float minY, float maxY)
  {
    m_minY = minY;
    m_maxY = maxY;
    NeedRedraw();
  }

  void SplineCurvePreview::UpdateGraph()
  {
    NeedRedraw();
  }

  void SplineCurvePreview::RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    m_DialogThreadProxy->Start();
    m_ChangeTimerHandler = GetThreadTimer().AddTimerHandler (33, m_ChangeDetectionTimer, this);
  }

  void SplineCurvePreview::RecvTimer (void *v)
  {
    if (m_DialogThreadProxy->m_bDialogChange && m_DialogThreadProxy->m_bDialogRunning)
    {
      m_DialogThreadProxy->m_bDialogChange = false;
      m_control_knot = m_DialogThreadProxy->GetControlPoints();

      if (m_control_knot.GetNumKnot() == 0)
      {
        NUX_HARDWARE_BREAK;
      }

      m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );
      NeedRedraw();
    }

    if (m_DialogThreadProxy->IsActive() )
    {
      m_ChangeTimerHandler = GetThreadTimer().AddTimerHandler (30, m_ChangeDetectionTimer, this);
    }
    else
    {
      if (m_ChangeTimerHandler.IsValid() )
        GetThreadTimer().RemoveTimerHandler (m_ChangeTimerHandler);

      m_ChangeTimerHandler = 0;

      m_control_knot = m_DialogThreadProxy->GetControlPoints();

      if (m_control_knot.GetNumKnot() == 0)
      {
        NUX_HARDWARE_BREAK;
      }

      m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );

      NeedRedraw();
    }
  }

  void SplineCurvePreview::RecvDialogChange (SplineCurveEditor *splinecurve)
  {
    //sigSplineChanged.emit(m_control_knot);
    NeedRedraw();
  }

  int SplineCurvePreview::GetNumKnot() const
  {
    return m_control_knot.GetNumKnot();
  }

  SplineKnot::Knot SplineCurvePreview::GetKnot (int i) const
  {
    return m_control_knot.GetKnot (i);
  }

  double SplineCurvePreview::Eval (double t)
  {
    double val;
    int nbKnot = m_control_knot.GetNumKnot();

    if (nbKnot <= 1)
      return 0.0;

    if (t < m_control_knot[0].GetX() )
      val = m_control_knot[0].GetY();
    else if (t > m_control_knot[nbKnot-1].GetX() )
      val = m_control_knot[nbKnot-1].GetY();
    else
      val = m_CubicSpline.Eval (t);

    if (val > m_maxY)
      val = m_maxY;

    if (val < m_minY)
      val = m_minY;

    return val;
  }

  const SplineKnot &SplineCurvePreview::GetControlPoints() const
  {
    return m_control_knot;
  }

  void SplineCurvePreview::SetControlPoints (const SplineKnot &splineknot)
  {
    m_control_knot = splineknot;
    m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );
  }

  void SplineCurvePreview::Reset()
  {
    m_control_knot.Reset();
    m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );
  }

  void SplineCurvePreview::AddKnot (double x, double y, bool bSelected)
  {
    m_control_knot.AddKnot (x, y, bSelected);
    m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );
  }

}
