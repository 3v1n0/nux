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

#include "NuxCore/Math/Spline.h"

#include "NuxGraphics/GLDeviceFactory.h"
#include "NuxGraphics/GLDeviceObjects.h"
#include "NuxGraphics/GLSh_DrawFunction.h"

#include "SplineCurveEditor.h"

namespace nux
{

  static const int KNOT_SIZE = 2;
  static const int KNOT_HIT_TEST = 4;
  static const int GRAPH_MARGIN = 2;

  unsigned long CTRL_KEY = 0;
// todo InitWidget
  SplineCurveEditor::SplineCurveEditor (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
  {
    m_minX              = 0.0f;
    m_minY              = 0.0f;
    m_maxX              = 1.0f;
    m_maxY              = 1.0f;
    m_FunctionCallback  = 0;
    m_Background        = 0;

    InitializeLayout();
    InitializeWidgets();

    m_control_knot.Reset();
    m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );


    m_Texture = GetThreadGLDeviceFactory()->CreateSystemCapableDeviceTexture (256, 4, 0, BITFMT_R8G8B8A8);
    m_DrawFunctionShader = new GLSh_DrawFunction();

    NTextureData image;
    MakeCheckBoardImage (image.GetSurface (0), 64, 64, Color (0xff323232), Color (0xff535353), 8, 8);
    BaseTexture* CheckboardPattern = GetThreadGLDeviceFactory()->CreateSystemCapableTexture ();
    CheckboardPattern->Update(&image);

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_Background = new TextureLayer (CheckboardPattern->GetDeviceTexture(), texxform, Color::White);

    CheckboardPattern->UnReference ();;
//     m_Background = PaintLayer(m_CheckboardPattern);
//     m_Background.SetTileTexture(true);
  }

  SplineCurveEditor::~SplineCurveEditor()
  {
    DestroyLayout();
    NUX_SAFE_DELETE (m_DrawFunctionShader);
    NUX_SAFE_DELETE (m_Background);
  }

  void SplineCurveEditor::InitializeWidgets()
  {

  }

  void SplineCurveEditor::InitializeLayout()
  {
    SetMinimumSize (200, 200);
    OnMouseDown.connect (sigc::mem_fun (this, &SplineCurveEditor::RecvMouseDown) );
    OnMouseUp.connect (sigc::mem_fun (this, &SplineCurveEditor::RecvMouseUp) );
    OnMouseDrag.connect (sigc::mem_fun (this, &SplineCurveEditor::RecvMouseDrag) );
    OnKeyEvent.connect (sigc::mem_fun (this, &SplineCurveEditor::RecvKeyEvent) );
  }

  void SplineCurveEditor::DestroyLayout()
  {
  }

  void SplineCurveEditor::SetControlPoints (const SplineKnot &splineKnot)
  {
    m_control_knot.Reset();
    m_control_knot = splineKnot;
    m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );
  }

  const SplineKnot &SplineCurveEditor::GetControlPoints() const
  {
    return m_control_knot;
  }

  void SplineCurveEditor::AddKnot (double x, double y, bool selected)
  {
    m_control_knot.AddKnot (x, y, selected);
    m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );
    sigCurveChange.emit (this);
    NeedRedraw();
  }

  void SplineCurveEditor::Reset()
  {
    m_control_knot.Reset();
  }

  double SplineCurveEditor::Eval (double t)
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

  long SplineCurveEditor::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      if (!m_Geometry.IsPointInside (ievent.e_x, ievent.e_y) )
      {
        //return TraverseInfo;
      }
    }

    CTRL_KEY = ievent.GetVirtualKeyState (NUX_VK_CONTROL);
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }


  void SplineCurveEditor::Draw (GraphicsContext &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    gPainter.PaintBackground (GfxContext, base);
    gPainter.Paint2DQuadColor (GfxContext, base, Color (COLOR_BACKGROUND_PRIMARY) );

    base.OffsetPosition (GRAPH_MARGIN, GRAPH_MARGIN);
    base.OffsetSize (-2 * GRAPH_MARGIN, -2 * GRAPH_MARGIN);

    int i;
    int nsample = base.GetWidth();
    int nbKnot = m_control_knot.GetNumKnot();

    if (1)
    {
      double *t = 0;
      double *y = 0;

      if (nbKnot > 1)
      {
        t = new double[nbKnot];
        y = new double[nbKnot];
      }

      std::vector<double> a;
      std::vector<double> b;

      if (nbKnot > 1)
      {
        for (i = 0; i < nbKnot; i++)
        {
          t[i] = m_control_knot[i].GetX();
          y[i] = m_control_knot[i].GetY();
          a.push_back (m_control_knot[i].GetX() );
          b.push_back (m_control_knot[i].GetY() );
        }
      }

      int W = GetBaseWidth() - 2 * GRAPH_MARGIN;
      int H = GetBaseHeight() - 2 * GRAPH_MARGIN;
      int X = GetBaseX() + GRAPH_MARGIN;
      int Y = GetBaseY() + GRAPH_MARGIN;

      double tval_prev, val_prev;
      double tval, val;
      tval_prev = 0.0;

      val_prev = Eval (tval_prev);

      gPainter.PushDrawLayer (GfxContext, base, m_Background);
      gPainter.PopBackground();

      GfxContext.PushClippingRectangle (base);

      GfxContext.GetRenderStates().EnableLineSmooth (TRUE, 1, GL_FASTEST);
      GfxContext.GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      GfxContext.GetRenderStates().SetColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);

      if (nbKnot <= 1)
        gPainter.Draw2DLine (GfxContext, X, Y + H - 1, X + W, Y + H - 1, Color (0xFFFFFFFF) );
      else
      {
        float tex_dx = (m_maxX - m_minX) / m_Texture->GetWidth();
        SURFACE_LOCKED_RECT lockrect;
        m_Texture->LockRect (0, &lockrect, 0);
        BYTE *dest = (BYTE *) lockrect.pBits;

        for (t_s32 i = 0; i < m_Texture->GetWidth(); i++)
        {
          float y = m_CubicSpline.Eval (m_minX + i * tex_dx);
          y = (y - m_minY) / (m_maxY - m_minY);

          for (t_s32 j = 0; j < m_Texture->GetHeight(); j++)
          {
            dest[4*i + 0 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
            dest[4*i + 1 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
            dest[4*i + 2 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
            dest[4*i + 3 + j *lockrect.Pitch] = 255 * Clamp<float> (y, 0.0f, 1.0f);
          }
        }

        m_Texture->UnlockRect (0);

        m_DrawFunctionShader->SetTransformMatrix (GfxContext.GetProjectionMatrix() );
        m_DrawFunctionShader->SetTextureFunction (m_Texture);
        m_DrawFunctionShader->SetBackgroundColor (0.1f, 0.1f, 0.1f, 0.6f);
        m_DrawFunctionShader->Render (X, Y, 0, W, H, GfxContext.GetWindowWidth(), GfxContext.GetWindowHeight() );
        //m_DrawFunctionShader->End();

        for ( int i = 1; i < nsample; i++ )
        {
          tval = ( double ) ( i ) / ( double ) ( nsample - 1 );
          val = m_CubicSpline.Eval (tval);

          if (val > m_maxY)
            val = m_maxY;

          if (val <= m_minY)
            val = m_minY + 1.0f / H;

          int X0, Y0, X1, Y1;
          X0 = X + W * (tval_prev - m_minX) / (m_maxX - m_minX);
          Y0 = Y + H * ( 1 - (val_prev - m_minY) / (m_maxY - m_minY) );
          X1 = X + W * (tval - m_minX) / (m_maxX - m_minX);
          Y1 = Y + H * ( 1 - (val - m_minY) / (m_maxY - m_minY) );

          gPainter.Draw2DLine (GfxContext, X0, Y0, X1, Y1, Color (0xFFFFFFFF) );

          tval_prev = tval;
          val_prev = val;
        }

      }

      GfxContext.GetRenderStates().EnableLineSmooth (GL_FALSE);
      GfxContext.GetRenderStates().SetBlend (GL_FALSE);
      GfxContext.GetRenderStates().SetColorMask (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

      GfxContext.PopClippingRectangle();

      for (i = 0; i < nbKnot; i++)
      {
        int X0, Y0;
        X0 = X + W * (m_control_knot[i].GetX() - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (m_control_knot[i].GetY() - m_minY) / (m_maxY - m_minY) );

        Geometry ShapeGeo = gTheme.GetImageGeometry (eDOT6x6);

        if (m_control_knot.isKnotSelected (i) )
        {
          gPainter.PaintShape (GfxContext,
                               Geometry (X0 - ShapeGeo.GetWidth() / 2, Y0 - ShapeGeo.GetHeight() / 2, ShapeGeo.GetWidth(), ShapeGeo.GetHeight() ),
                               Color (0xFF44FF44), eDOT6x6);
        }
        else
        {
          gPainter.PaintShape (GfxContext,
                               Geometry (X0 - ShapeGeo.GetWidth() / 2, Y0 - ShapeGeo.GetHeight() / 2, ShapeGeo.GetWidth(), ShapeGeo.GetHeight() ),
                               Color (0xFFFFFFFF), eDOT6x6);

        }
      }

      delete t;
      delete y;
    }

    // We do some ajustment here because when a knot is at one of the border, we still want to see the entire knot and not have the wireframe
    // square draw itself over it.
    gPainter.Paint2DQuadWireframe (GfxContext, Geometry (base.x - KNOT_SIZE,
                                   base.y - KNOT_SIZE,
                                   base.GetWidth() + 2 * KNOT_SIZE,
                                   base.GetHeight() + 2 * KNOT_SIZE), Color (0xFF000000) );
  }


  void SplineCurveEditor::DrawContent (GraphicsContext &GfxContext, bool force_draw)
  {

  }

  void SplineCurveEditor::PostDraw (GraphicsContext &GfxContext, bool force_draw)
  {

  }


  void SplineCurveEditor::SetXAxisBounds (float minX, float maxX)
  {
    m_minX = minX;
    m_maxX = maxX;
    NeedRedraw();
  }

  void SplineCurveEditor::SetYAxisBounds (float minY, float maxY)
  {
    m_minY = minY;
    m_maxY = maxY;
    NeedRedraw();
  }

  void SplineCurveEditor::SetFunctionCallback (SplineFunctionCallback f)
  {
    m_FunctionCallback = f;
    NeedRedraw();
  }

  float SplineCurveEditor::EvalFunction (float x)
  {
    if (m_FunctionCallback != 0)
      return (*m_FunctionCallback) (x);

    return 0;
  }

  void SplineCurveEditor::UpdateGraph()
  {
    NeedRedraw();
  }


// check if a value lies within a closed interval
#ifndef INSIDE_BOUNDS
#define INSIDE_BOUNDS( x, lo, hi ) ( (x) >= (lo) && (x) <= (hi) )
#endif

//check if a 2D point lies within a 2D box
#ifndef PT_INSIDE_BOX
#define PT_INSIDE_BOX( x, y, lo_x, hi_x, lo_y, hi_y ) ( INSIDE_BOUNDS(x,lo_x,hi_x) && INSIDE_BOUNDS(y,lo_y,hi_y) )
#endif

  void SplineCurveEditor::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();

    if (m_control_knot.GetNumSelectedKnot() > 0)
      sigCurveChange.emit (this);
  }

  void SplineCurveEditor::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    int nbKnot = m_control_knot.GetNumKnot();

    if (CTRL_KEY == 0)
    {
      m_control_knot.UnSelectAllKnot();
    }

    int W = GetBaseWidth() - 2 * GRAPH_MARGIN;
    int H = GetBaseHeight() - 2 * GRAPH_MARGIN;
    int X = GetBaseX() + GRAPH_MARGIN;
    int Y = GetBaseY() + GRAPH_MARGIN;

    bool b = PT_INSIDE_BOX (X - GRAPH_MARGIN + x, Y - GRAPH_MARGIN + y, X - GRAPH_MARGIN, X + W + 2 * GRAPH_MARGIN, Y - GRAPH_MARGIN, Y + H + 2 * GRAPH_MARGIN);

    if (b == false)
      return;

    X = GetBaseX();
    Y = GetBaseY();

    double new_x, new_y;
    new_x = (double) (x - GRAPH_MARGIN) / (double) W;
    new_y = 1.0 - (double) (y - GRAPH_MARGIN) / (double) H;

    hit_point_dx = 0;
    hit_point_dy = 0;

    for (int i = 0; i < nbKnot; i++)
    {
      int Xp, Yp;
      Xp = X + W * (m_control_knot[i].GetX() - m_minX) / (m_maxX - m_minX);
      Yp = Y + H * ( 1 - (m_control_knot[i].GetY() - m_minY) / (m_maxY - m_minY) );

      int tx, ty;
      tx = X + x - GRAPH_MARGIN;
      ty = Y + y - GRAPH_MARGIN;

      if (PT_INSIDE_BOX (tx, ty, Xp - KNOT_HIT_TEST, Xp + KNOT_HIT_TEST, Yp - KNOT_HIT_TEST, Yp + KNOT_HIT_TEST) )
      {
        hit_point_dx = (tx) - Xp;
        hit_point_dy = Yp - (ty);

        m_control_knot.SelectKnot (i, TRUE);
        m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );
        NeedRedraw();
        return;
      }
    }

    double eval = Eval (new_x);

    if (new_y >= eval - 4.0 / H && (new_y <= eval + 4.0 / H) )
    {
      m_control_knot.AddKnot (new_x, new_y, TRUE);
      m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );
      sigCurveChange.emit (this);
      NeedRedraw();
    }
  }

  void SplineCurveEditor::RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    float xp, yp;
    int W = GetBaseWidth() - 2 * GRAPH_MARGIN;
    int H = GetBaseHeight() - 2 * GRAPH_MARGIN;
    int X = GetBaseX() + GRAPH_MARGIN;
    int Y = GetBaseY() + GRAPH_MARGIN;
    int nbKnot = m_control_knot.GetNumKnot();

    if (nbKnot <= 1)
      return;     // no drag when there is not enough knots.

    xp = m_minX + (m_maxX - m_minX) * dx / W;
    yp = m_minY + (m_maxY - m_minY) * dy / H;



    for (int i = 0; i < nbKnot; i++)
    {
      if (m_control_knot.isKnotSelected (i) )
      {
        int tempx, tempy;
        tempx = X + x - GRAPH_MARGIN;
        tempy = Y + y - GRAPH_MARGIN;

        int Xp, Yp;
        Xp = X + W * (m_control_knot[i].GetX() - m_minX) / (m_maxX - m_minX);
        Yp = Y + H * ( 1 - (m_control_knot[i].GetY() - m_minY) / (m_maxY - m_minY) );

        if (dx > 0)
        {
          if (tempx > Xp + hit_point_dx)
          {
            xp = /*m_minX +*/ (m_maxX - m_minX) * (tempx - Xp - hit_point_dx) / W;
          }
          else
          {
            xp = 0;
          }
        }
        else if (dx < 0)
        {
          if (tempx < Xp + hit_point_dx)
          {
            xp = /*m_minX +*/ (m_maxX - m_minX) * (tempx - Xp - hit_point_dx) / W;
          }
          else
          {
            xp = 0;
          }
        }

        if (dy > 0)
        {
          if (tempy > Yp - hit_point_dy)
          {
            yp = /*m_minY +*/ (m_maxY - m_minY) * (tempy - Yp + hit_point_dy) / H;
          }
          else
          {
            yp = 0;
          }
        }
        else if (dy < 0)
        {
          if (tempy < Yp - hit_point_dy)
          {
            yp = /*m_minY +*/ (m_maxY - m_minY) * (tempy - Yp + hit_point_dy) / H;
          }
          else
          {
            yp = 0;
          }
        }

        m_control_knot[i].SetX (m_control_knot[i].GetX() + xp);
        m_control_knot[i].SetY (m_control_knot[i].GetY() - yp);

        if (m_control_knot[i].GetX() < m_minX)
        {
          m_control_knot[i].SetX (m_minX);
        }

        if (m_control_knot[i].GetX() > m_maxX)
        {
          m_control_knot[i].SetX (m_maxX);
        }

        if (m_control_knot[i].GetY() < m_minY)
        {
          m_control_knot[i].SetY (m_minY);
        }

        if (m_control_knot[i].GetY() > m_maxY)
        {
          m_control_knot[i].SetY (m_maxY);
        }


        if (i == 0)
        {
          if ( m_control_knot[i].GetX() > m_control_knot[i + 1].GetX() - 0.01)
            m_control_knot[i].SetX (m_control_knot[i + 1].GetX() - 0.01);
        }
        else if (i == nbKnot - 1)
        {
          if (m_control_knot[i].GetX() < m_control_knot[i - 1].GetX() + 0.01)
          {
            m_control_knot[i].SetX (m_control_knot[i - 1].GetX() + 0.01);
          }
        }
        else
        {
          if (m_control_knot[i].GetX() > m_control_knot[i + 1].GetX() - 0.01)
          {
            m_control_knot[i].SetX (m_control_knot[i + 1].GetX() - 0.01);
          }

          if (m_control_knot[i].GetX() < m_control_knot[i - 1].GetX() + 0.01)
          {
            m_control_knot[i].SetX (m_control_knot[i - 1].GetX() + 0.01);
          }
        }

        break;
      }
    }

    m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );
    NeedRedraw();
  }

  void SplineCurveEditor::RecvKeyEvent (
    GraphicsContext &GfxContext , /*Graphics Context for text operation*/
    unsigned long    eventType  , /*event type*/
    unsigned long    keysym     , /*event keysym*/
    unsigned long    state      , /*event state*/
    const char      *character  , /*character*/
    bool             isRepeated , /*true if the key is repeated more than once*/
    unsigned short   keyCount     /*key repeat count*/
  )
  {
    if (!HasKeyboardFocus() )
    {
      return;
    }

    if ( (keysym == NUX_VK_DELETE) || (keysym == NUX_KP_DELETE) || (keysym == NUX_VK_BACKSPACE) )
    {

      int kn = m_control_knot.GetNumSelectedKnot();

      m_control_knot.EraseSelectedKnot();
      m_CubicSpline.Set (m_control_knot.GetNumKnot(), m_control_knot.GetXArray(), m_control_knot.GetYArray() );

      if (kn > 0)
        sigCurveChange.emit (this);

      NeedRedraw();
    }
  }


}
