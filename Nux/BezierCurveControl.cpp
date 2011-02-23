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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "NuxCore/Math/Constants.h"
#include "NuxCore/Math/Spline.h"
#include "NuxCore/Math/Bezier.h"
#include "BezierCurveControl.h"

namespace nux
{

  const int KNOT_SIZE = 2;
  const int KNOT_HIT_TEST = 4;

  BezierCurveControl::BezierCurveControl (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_minX (0.0f),
    m_minY (0.0f),
    m_maxX (1.0f),
    m_maxY (1.0f),
    m_FunctionCallback (0)
  {
    SetMinimumSize (400, 300);
    SetBaseSize (400, 300);

    m_control_knot.push_back (Knot (0.0f, 0.0f) );
    m_control_knot.push_back (Knot (0.1f, 0.0f) );
    m_control_knot.push_back (Knot (0.7f, 0.9f) );
    m_control_knot.push_back (Knot (1.0f, 1.0f) );

    OnMouseDown.connect (sigc::mem_fun (this, &BezierCurveControl::RecvMouseDown) );
    OnMouseUp.connect (sigc::mem_fun (this, &BezierCurveControl::RecvMouseUp) );
    OnMouseDrag.connect (sigc::mem_fun (this, &BezierCurveControl::RecvMouseDrag) );

  }

  BezierCurveControl::~BezierCurveControl()
  {


  }


  long BezierCurveControl::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      if (!GetGeometry().IsPointInside (ievent.e_x, ievent.e_y) )
      {
        //ProcEvInfo = eDoNotProcess;
        //return TraverseInfo;
      }
    }

    ret = PostProcessEvent2 (ievent, ret, 0);
    return ret;
  }


  void BezierCurveControl::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground (GfxContext, base);
    GetPainter().Paint2DQuadWireframe (GfxContext, base, Color (COLOR_BACKGROUND_SECONDARY) );

#define N 3
    t_u32 i;
    t_u32 nsample = 30;
    double t;
    t_u32 nbKnot = (t_u32) m_control_knot.size();

    if (nbKnot > 0)
    {
      double *xcon = new double[nbKnot];
      double xval;
      double *ycon = new double[nbKnot];
      double yval;


      for (i = 0; i < nbKnot; i++)
      {
        xcon[i] = m_control_knot[i].m_X;
        ycon[i] = m_control_knot[i].m_Y;
      }

      int W = GetBaseWidth() - 2;
      int H = GetBaseHeight() - 2;
      int X = GetBaseX() + 1;
      int Y = GetBaseY() + 1;

      double xprev, yprev;
      Bezier_XY (N, 0.0, xcon, ycon, &xprev, &yprev);

      //GetPainter().Draw2DLine(X, Y, X+W, Y+H, Color(0xFFFF0000));

      base.OffsetPosition (1, 1);
      base.OffsetSize (-2, -2);

      GfxContext.PushClippingRectangle (base);

      GfxContext.GetRenderStates().EnableLineSmooth (TRUE, 1, GL_FASTEST);
      GfxContext.GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      for (i = 1; i < nsample; i++)
      {
        t = ( double ) ( i ) / ( double ) ( nsample - 1 );
        Bezier_XY ( N, t, xcon, ycon, &xval, &yval );

        int X0, Y0, X1, Y1;
        X0 = X + W * (xprev - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (yprev - m_minY) / (m_maxY - m_minY) );
        X1 = X + W * (xval - m_minX) / (m_maxX - m_minX);
        Y1 = Y + H * ( 1 - (yval - m_minY) / (m_maxY - m_minY) );

        GetPainter().Draw2DLine (GfxContext, X0, Y0, X1, Y1, Color (0xFFFF0000) );

        xprev = xval;
        yprev = yval;
      }

      GfxContext.GetRenderStates().EnableLineSmooth (FALSE);
      GfxContext.GetRenderStates().SetBlend (GL_FALSE);

      for (i = 0; i < nbKnot - 1; i++)
      {
        int X0, Y0, X1, Y1;
        X0 = X + W * (m_control_knot[i].m_X - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (m_control_knot[i].m_Y - m_minY) / (m_maxY - m_minY) );
        X1 = X + W * (m_control_knot[i+1].m_X - m_minX) / (m_maxX - m_minX);
        Y1 = Y + H * ( 1 - (m_control_knot[i+1].m_Y - m_minY) / (m_maxY - m_minY) );

        GetPainter().Draw2DLine (GfxContext, X0, Y0, X1, Y1, Color (0xFF0000FF) );

      }

      for (i = 0; i < nbKnot; i++)
      {
        int X0, Y0;
        X0 = X + W * (m_control_knot[i].m_X - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (m_control_knot[i].m_Y - m_minY) / (m_maxY - m_minY) );

        if (m_control_knot[i].m_IsSelected)
          GetPainter().Paint2DQuadColor (GfxContext, X0 - KNOT_SIZE, Y0 - KNOT_SIZE, 2 * KNOT_SIZE, 2 * KNOT_SIZE, Color (0xFF00FF00) );
        else
          GetPainter().Paint2DQuadColor (GfxContext, X0 - KNOT_SIZE, Y0 - KNOT_SIZE, 2 * KNOT_SIZE, 2 * KNOT_SIZE, Color (0xFF777777) );
      }

      delete xcon;
      delete ycon;
    }

//    for(int i = 1; i < GetWidth(); i++)
//    {
//        float x1, y1;
//
//        x1 = x0 + dX;
//        y1 = EvalFunction(x1);
//
//        int X0, Y0, X1, Y1;
//        X0 = X + W * (x0 - m_minX) / (m_maxX - m_minX);
//        Y0 = Y - H * (y0 + m_minY) / (m_maxY - m_minY);
//        X1 = X + W * (x1 - m_minX) / (m_maxX - m_minX);
//        Y1 = Y - H * (y1 + m_minY) / (m_maxY - m_minY);
//        GetPainter().Draw2DLine(X0, Y0, X1, Y1, Color(0xFFFF0000));
//
//        x0 = x1;
//        y0 = y1;
//
//    }
    GfxContext.PopClippingRectangle();
  }


  void BezierCurveControl::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void BezierCurveControl::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }


  void BezierCurveControl::SetXAxisBounds (float minX, float maxX)
  {
    m_minX = minX;
    m_maxX = maxX;
    NeedRedraw();
  }

  void BezierCurveControl::SetYAxisBounds (float minY, float maxY)
  {
    m_minY = minY;
    m_maxY = maxY;
    NeedRedraw();
  }

  void BezierCurveControl::SetFunctionCallback (FunctionCallback f)
  {
    m_FunctionCallback = f;
    NeedRedraw();
  }

  float BezierCurveControl::EvalFunction (float x)
  {
    if (m_FunctionCallback != 0)
      return (*m_FunctionCallback) (x);

    return 0;
  }

  void BezierCurveControl::UpdateGraph()
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

  void BezierCurveControl::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    NeedRedraw();
  }

  void BezierCurveControl::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    t_u32 nbKnot = (t_u32) m_control_knot.size();

    for (t_u32 i = 0; i < nbKnot; i++)
    {
      m_control_knot[i].m_IsSelected = false;
    }

    int W = GetBaseWidth() - 2;
    int H = GetBaseHeight() - 2;
    int X = GetBaseX() + 1;
    int Y = GetBaseY() + 1;

    bool b = PT_INSIDE_BOX (X + x, Y + y, X, X + W, Y, Y + H);

    if (b == false)
      return;

    X = GetBaseX();
    Y = GetBaseY();

    for (t_u32 i = 0; i < nbKnot; i++)
    {
      int Xp, Yp;
      Xp = X + W * (m_control_knot[i].m_X - m_minX) / (m_maxX - m_minX);
      Yp = Y + H * ( 1 - (m_control_knot[i].m_Y - m_minY) / (m_maxY - m_minY) );

      if (PT_INSIDE_BOX (X + x, Y + y, Xp - KNOT_HIT_TEST, Xp + KNOT_HIT_TEST, Yp - KNOT_HIT_TEST, Yp + KNOT_HIT_TEST) )
      {
        m_control_knot[i].m_IsSelected = true;
        break;
      }
    }

    NeedRedraw();
  }

  void BezierCurveControl::RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    float xp, yp;

    int W = GetBaseWidth();
    int H = GetBaseHeight();
    t_u32 nbKnot = (t_u32) m_control_knot.size();

    xp = m_minX + (m_maxX - m_minX) * dx / W;
    yp = m_minY + (m_maxY - m_minY) * dy / H;



    for (t_u32 i = 0; i < nbKnot; i++)
    {
      if (m_control_knot[i].m_IsSelected)
      {
        m_control_knot[i].m_X += xp;
        m_control_knot[i].m_Y -= yp;

        if (m_control_knot[i].m_X < m_minX)
        {
          m_control_knot[i].m_X = m_minX;
        }

        if (m_control_knot[i].m_X > m_maxX)
        {
          m_control_knot[i].m_X = m_maxX;
        }

        if (m_control_knot[i].m_Y < m_minY)
        {
          m_control_knot[i].m_Y = m_minY;
        }

        if (m_control_knot[i].m_Y > m_maxY)
        {
          m_control_knot[i].m_Y = m_maxY;
        }

        break;
      }
    }

    NeedRedraw();
  }


}
