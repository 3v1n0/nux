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

#include "BezierCurveControl2.h"
#include "NuxCore/Math/Bezier.h"

namespace nux
{

  static const int KNOT_SIZE = 2;
  static const int KNOT_HIT_TEST = 4;
  static const int CURVE_DEGREE = 3;
  static const int GRAPH_MARGIN = 2;
  static const int UNIT_REFERENCE = 15;

  unsigned long S_KEY = 0;

  BezierCurveControl2::BezierCurveControl2 (NUX_FILE_LINE_DECL)
    :   View (NUX_FILE_LINE_PARAM)
    ,   m_minX (-0.10f),
    m_minY (-0.100f),
    m_maxX (1.100f),
    m_maxY (1.100f),
    m_FunctionCallback (0),
    m_bControlPointSelected (0),
    m_bPanningEnabled (true),
    m_bZoomingEnabled (true)
  {
    SetMinimumSize (180, 180);

    m_control_knot.push_back (Knot2 (0.0f, 0.0f) );
    m_control_knot.push_back (Knot2 (0.1f, 0.0f) );
    m_control_knot.push_back (Knot2 (0.7f, 0.9f) );
    m_control_knot.push_back (Knot2 (1.0f, 1.0f) );

    mouse_down.connect (sigc::mem_fun (this, &BezierCurveControl2::RecvMouseDown) );
    mouse_up.connect (sigc::mem_fun (this, &BezierCurveControl2::RecvMouseUp) );
    mouse_drag.connect (sigc::mem_fun (this, &BezierCurveControl2::RecvMouseDrag) );

    key_down.connect (sigc::mem_fun (this, &BezierCurveControl2::RecvKeyEvent) );

    NTextureData image;
    MakeCheckBoardImage (image.GetSurface (0), 64, 64, Color (0xff323232), Color (0xff535353), 8, 8);
    BaseTexture* m_CheckboardPattern = GetGraphicsDisplay()->GetGpuDevice()->CreateSystemCapableTexture ();
    m_CheckboardPattern->Update (&image);

    TexCoordXForm texxform;
    texxform.SetTexCoordType (TexCoordXForm::OFFSET_COORD);
    texxform.SetWrap (TEXWRAP_REPEAT, TEXWRAP_REPEAT);
    m_Background = new TextureLayer (m_CheckboardPattern->GetDeviceTexture(), texxform, color::White);

    m_CheckboardPattern->UnReference ();
  }

  BezierCurveControl2::~BezierCurveControl2()
  {
    delete m_Background;
  }

  long BezierCurveControl2::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
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

    S_KEY = ievent.GetVirtualKeyState (NUX_VK_S);
    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }


  void BezierCurveControl2::Draw (GraphicsEngine &GfxContext, bool force_draw)
  {
    Geometry base = GetGeometry();

    GetPainter().PaintBackground (GfxContext, base);
    GetPainter().Paint2DQuadColor (GfxContext, base, Color (COLOR_BACKGROUND_PRIMARY) );
    base.OffsetPosition (GRAPH_MARGIN, GRAPH_MARGIN);
    base.OffsetSize (-2 * GRAPH_MARGIN, -2 * GRAPH_MARGIN);

    int W = GetBaseWidth() - 2 * GRAPH_MARGIN;
    int H = GetBaseHeight() - 2 * GRAPH_MARGIN;
    int X = GetBaseX() + GRAPH_MARGIN;
    int Y = GetBaseY() + GRAPH_MARGIN;

    t_u32 i;
    int nsample = base.GetWidth();
    double t;
    t_u32 nbKnot = (t_u32) m_control_knot.size();

    GfxContext.PushClippingRectangle (base);

    GetPainter().PushDrawLayer (GfxContext, base, m_Background);
    GetPainter().PopBackground();

    //DrawGrid(GfxContext);
    DrawCoordinateSystem (GfxContext);

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

      double xprev, yprev;
      Bezier_XY (CURVE_DEGREE, 0.0, xcon, ycon, &xprev, &yprev);

      int X0, Y0, X1, Y1, X2, Y2;

      glEnable (GL_POINT_SMOOTH);
      glEnable (GL_LINE_SMOOTH);
      glLineWidth (1);
      glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);	// Make round points, not square points
      glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);		// Antialias the lines
      GfxContext.GetRenderStates().SetBlend (TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      for (i = 1; i < (t_u32) nsample; i++ )
      {
        t = ( double ) ( i ) / ( double ) ( nsample - 1 );
        Bezier_XY ( CURVE_DEGREE, t, xcon, ycon, &xval, &yval );

        X0 = X + W * (xprev - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (yprev - m_minY) / (m_maxY - m_minY) );
        X1 = X + W * (xval - m_minX) / (m_maxX - m_minX);
        Y1 = Y + H * ( 1 - (yval - m_minY) / (m_maxY - m_minY) );

        GetPainter().Draw2DLine (GfxContext, X0, Y0, X1, Y1, Color (0xFFFFFFFF) );

        xprev = xval;
        yprev = yval;
      }

      if (m_control_knot[0].m_X > m_minX)
      {
        // draw line from the left border to the left most control point.

        X0 = X + W * (m_control_knot[0].m_X - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (m_control_knot[0].m_Y - m_minY) / (m_maxY - m_minY) );

        GetPainter().Draw2DLine (GfxContext, base.x, Y0, X0, Y0, Color (0xFFFFFFFF) );
      }

      if (m_control_knot[nbKnot-1].m_X < m_maxX)
      {
        // draw line from the right most control point to the left border.

        X0 = X + W * (m_control_knot[nbKnot-1].m_X - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (m_control_knot[nbKnot-1].m_Y - m_minY) / (m_maxY - m_minY) );

        GetPainter().Draw2DLine (GfxContext, X0, Y0, base.x + base.GetWidth(), Y0, Color (0xFFFFFFFF) );
      }

      glDisable (GL_POINT_SMOOTH);
      glDisable (GL_LINE_SMOOTH);
      GfxContext.GetRenderStates().SetBlend (FALSE);

      for (i = 0; i <= nbKnot - 1; i += CURVE_DEGREE)
      {
        // Only the endpoint of cubic bezier.

        X0 = X + W * (m_control_knot[i].m_X - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (m_control_knot[i].m_Y - m_minY) / (m_maxY - m_minY) );

        if (i < nbKnot - 1)
        {
          X1 = X + W * (m_control_knot[i+1].m_X - m_minX) / (m_maxX - m_minX);
          Y1 = Y + H * ( 1 - (m_control_knot[i+1].m_Y - m_minY) / (m_maxY - m_minY) );

          GetPainter().Draw2DLine (GfxContext, X0, Y0, X1, Y1, Color (0xFF0000FF) );
        }

        if ( i > 0)
        {
          X2 = X + W * (m_control_knot[i-1].m_X - m_minX) / (m_maxX - m_minX);
          Y2 = Y + H * ( 1 - (m_control_knot[i-1].m_Y - m_minY) / (m_maxY - m_minY) );

          GetPainter().Draw2DLine (GfxContext, X0, Y0, X2, Y2, Color (0xFF0000FF) );
        }
      }

      Geometry ShapeGeo = GetTheme().GetImageGeometry (eDOT6x6);

      for (i = 0; i < nbKnot; i++)
      {
        int X0, Y0;
        X0 = X + W * (m_control_knot[i].m_X - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (m_control_knot[i].m_Y - m_minY) / (m_maxY - m_minY) );

        if (m_control_knot[i].m_IsSelected)
        {
          GetPainter().PaintShape (GfxContext,
                               Geometry (X0 - ShapeGeo.GetWidth() / 2, Y0 - ShapeGeo.GetHeight() / 2, ShapeGeo.GetWidth(), ShapeGeo.GetHeight() ),
                               Color (0xFF44FF44), eDOT6x6);
        }
        else
        {
          GetPainter().PaintShape (GfxContext,
                               Geometry (X0 - ShapeGeo.GetWidth() / 2, Y0 - ShapeGeo.GetHeight() / 2, ShapeGeo.GetWidth(), ShapeGeo.GetHeight() ),
                               Color (0xFFFFFFFF), eDOT6x6);
        }
      }

      delete[] xcon;
      delete[] ycon;
    }

    GetPainter().Paint2DQuadWireframe (GfxContext, base, Color (0xFF000000) );

    DrawRuler (GfxContext);
    GfxContext.PopClippingRectangle();
  }

  void BezierCurveControl2::DrawRuler (GraphicsEngine &GfxContext)
  {
    Geometry base = GetGeometry();

    base.OffsetPosition (GRAPH_MARGIN, GRAPH_MARGIN);
    base.OffsetSize (-2 * GRAPH_MARGIN, -2 * GRAPH_MARGIN);

    // Draw Horizontal ruler

    static float Funit = 1.0;
    float rangex = m_maxX - m_minX;
    int Iunit = Funit * base.GetWidth() / rangex;

    if (Iunit > 2 * UNIT_REFERENCE)
    {
      Funit = rangex * UNIT_REFERENCE / base.GetWidth();
    }
    else if (Iunit < UNIT_REFERENCE)
    {
      Funit = rangex * 2 * UNIT_REFERENCE / base.GetWidth();
    }

    if ( (m_minX < 0) && (m_maxX > 0) )
    {
      int center = (0 - m_minX) * (float) base.GetWidth() / (float) (m_maxX - m_minX);
      int start_x = base.x + center - Funit * base.GetWidth() / rangex;
      int count = 0;

      while (start_x > base.x)
      {
        GetPainter().Draw2DLine (GfxContext, start_x, base.y + base.GetHeight(), start_x, base.y + base.GetHeight() - 5, Color (0xFF000000) );
        count++;
        start_x -= Funit * base.GetWidth() / rangex;
      }

      start_x = base.x + center + Funit * base.GetWidth() / rangex;
      count = 0;

      while (start_x < base.x + base.GetWidth() )
      {
        GetPainter().Draw2DLine (GfxContext, start_x, base.y + base.GetHeight(), start_x, base.y + base.GetHeight() - 5, Color (0xFF000000) );
        count++;
        start_x += Funit * base.GetWidth() / rangex;
      }
    }

    if (0 <= m_minX)
    {
      int center = (m_minX - 0) * (float) base.GetWidth() / (float) (m_maxX - m_minX);
      int start_x = base.x - center + Funit * base.GetWidth() / rangex;
      int count = 0;

      while (start_x < base.x + base.GetWidth() )
      {
        GetPainter().Draw2DLine (GfxContext, start_x, base.y + base.GetHeight(), start_x, base.y + base.GetHeight() - 5, Color (0xFF000000) );
        count++;
        start_x += Funit * base.GetWidth() / rangex;
      }
    }

    if (m_maxX <= 0)
    {
      int center = (0 - m_minX) * (float) base.GetWidth() / (float) (m_maxX - m_minX);
      int start_x = base.x + center - Funit * base.GetWidth() / rangex;
      int count = 0;

      while (start_x > base.x)
      {
        GetPainter().Draw2DLine (GfxContext, start_x, base.y + base.GetHeight(), start_x, base.y + base.GetHeight() - 5, Color (0xFF000000) );
        count++;
        start_x -= Funit * base.GetWidth() / rangex;
      }
    }

    // Draw Vertical ruler

    static float FunitY = 1.0;
    float rangey = m_maxY - m_minY;
    int IunitY = FunitY * base.GetHeight() / rangey;

    if (IunitY > 2 * UNIT_REFERENCE)
    {
      FunitY = rangey * UNIT_REFERENCE / base.GetHeight();
    }
    else if (IunitY < UNIT_REFERENCE)
    {
      FunitY = rangey * 2 * UNIT_REFERENCE / base.GetHeight();
    }

    if ( (m_minY < 0) && (m_maxY > 0) )
    {
      int center = (0 - m_minY) * (float) base.GetHeight() / (float) (m_maxY - m_minY);
      int start_y = base.y + base.GetHeight() - center - FunitY * base.GetHeight() / rangey;
      int count = 0;

      while (start_y > base.y)
      {
        GetPainter().Draw2DLine (GfxContext, base.x, start_y , base.x + 5, start_y, Color (0xFF000000) );
        count++;
        start_y -= FunitY * base.GetHeight() / rangey;
      }

      start_y = base.y + base.GetHeight() - center + FunitY * base.GetHeight() / rangey;
      count = 0;

      while (start_y < base.y + base.GetHeight() )
      {
        GetPainter().Draw2DLine (GfxContext, base.x, start_y , base.x + 5, start_y, Color (0xFF000000) );
        count++;
        start_y += FunitY * base.GetHeight() / rangey;
      }
    }

    if (0 <= m_minY)
    {
      int center = (m_minY - 0) * (float) base.GetHeight() / (float) (m_maxY - m_minY);
      int start_y = base.y + base.GetHeight() + center - FunitY * base.GetHeight() / rangey;
      int count = 0;

      while (start_y > base.y)
      {
        GetPainter().Draw2DLine (GfxContext, base.x, start_y , base.x + 5, start_y, Color (0xFF000000) );
        count++;
        start_y -= FunitY * base.GetHeight() / rangey;
      }
    }

    if (m_maxY <= 0)
    {
      int center = (0 - m_minY) * (float) base.GetHeight() / (float) (m_maxY - m_minY);
      int start_y = base.y + base.GetHeight() - center + FunitY * base.GetHeight() / rangey;
      int count = 0;

      while (start_y < base.y + base.GetHeight() )
      {
        GetPainter().Draw2DLine (GfxContext, base.x, start_y , base.x + 5, start_y, Color (0xFF000000) );
        count++;
        start_y += FunitY * base.GetHeight() / rangey;
      }
    }
  }

  void BezierCurveControl2::DrawGrid (GraphicsEngine &GfxContext)
  {
    Geometry base = GetGeometry();

    base.OffsetPosition (GRAPH_MARGIN, GRAPH_MARGIN);
    base.OffsetSize (-2 * GRAPH_MARGIN, -2 * GRAPH_MARGIN);

    // Draw Vertical Grid

    static float Funit = 1.0;
    float rangex = m_maxX - m_minX;
    int Iunit = Funit * base.GetWidth() / rangex;

    if (Iunit > 2 * UNIT_REFERENCE)
    {
      Funit = rangex * UNIT_REFERENCE / base.GetWidth();
    }
    else if (Iunit < UNIT_REFERENCE)
    {
      Funit = rangex * 2 * UNIT_REFERENCE / base.GetWidth();
    }

    if ( (m_minX < 0) && (m_maxX > 0) )
    {
      int center = (0 - m_minX) * (float) base.GetWidth() / (float) (m_maxX - m_minX);
      int start_x = base.x + center - Funit * base.GetWidth() / rangex;
      int count = 0;

      while (start_x > base.x)
      {
        // vertical grid
        GetPainter().Draw2DLine (GfxContext, start_x, base.y,
                             start_x, base.y + base.GetHeight(), Color (0xFF5F5F5F) );

        count++;
        start_x -= Funit * base.GetWidth() / rangex;
      }

      start_x = base.x + center + Funit * base.GetWidth() / rangex;
      count = 0;

      while (start_x < base.x + base.GetWidth() )
      {
        // vertical grid
        GetPainter().Draw2DLine (GfxContext, start_x, base.y,
                             start_x, base.y + base.GetHeight(), Color (0xFF5F5F5F) );

        count++;
        start_x += Funit * base.GetWidth() / rangex;
      }
    }

    if (0 <= m_minX)
    {
      int center = (m_minX - 0) * (float) base.GetWidth() / (float) (m_maxX - m_minX);
      int start_x = base.x - center + Funit * base.GetWidth() / rangex;
      int count = 0;

      while (start_x < base.x + base.GetWidth() )
      {
        // vertical grid
        GetPainter().Draw2DLine (GfxContext, start_x, base.y,
                             start_x, base.y + base.GetHeight(), Color (0xFF5F5F5F) );

        count++;
        start_x += Funit * base.GetWidth() / rangex;
      }
    }

    if (m_maxX <= 0)
    {
      int center = (0 - m_minX) * (float) base.GetWidth() / (float) (m_maxX - m_minX);
      int start_x = base.x + center - Funit * base.GetWidth() / rangex;
      int count = 0;

      while (start_x > base.x)
      {
        // vertical grid
        GetPainter().Draw2DLine (GfxContext, start_x, base.y,
                             start_x, base.y + base.GetHeight(), Color (0xFF5F5F5F) );

        count++;
        start_x -= Funit * base.GetWidth() / rangex;
      }
    }

    // Draw Horizontal Grid

    static float FunitY = 1.0;
    float rangey = m_maxY - m_minY;
    int IunitY = FunitY * base.GetHeight() / rangey;

    if (IunitY > 2 * UNIT_REFERENCE)
    {
      FunitY = rangey * UNIT_REFERENCE / base.GetHeight();
    }
    else if (IunitY < UNIT_REFERENCE)
    {
      FunitY = rangey * 2 * UNIT_REFERENCE / base.GetHeight();
    }

    if ( (m_minY < 0) && (m_maxY > 0) )
    {
      int center = (0 - m_minY) * (float) base.GetHeight() / (float) (m_maxY - m_minY);
      int start_y = base.y + base.GetHeight() - center - FunitY * base.GetHeight() / rangey;
      int count = 0;

      while (start_y > base.y)
      {
        // horizontal grid
        GetPainter().Draw2DLine (GfxContext, base.x, start_y,
                             base.x + base.GetWidth(), start_y, Color (0xFF5F5F5F) );

        count++;
        start_y -= FunitY * base.GetHeight() / rangey;
      }

      start_y = base.y + base.GetHeight() - center + FunitY * base.GetHeight() / rangey;
      count = 0;

      while (start_y < base.y + base.GetHeight() )
      {
        GetPainter().Draw2DLine (GfxContext, base.x, start_y,
                             base.x + base.GetWidth(), start_y, Color (0xFF5F5F5F) );

        count++;
        start_y += FunitY * base.GetHeight() / rangey;
      }
    }

    if (0 <= m_minY)
    {
      int center = (m_minY - 0) * (float) base.GetHeight() / (float) (m_maxY - m_minY);
      int start_y = base.y + base.GetHeight() + center - FunitY * base.GetHeight() / rangey;
      int count = 0;

      while (start_y > base.y)
      {
        GetPainter().Draw2DLine (GfxContext, base.x, start_y,
                             base.x + base.GetWidth(), start_y, Color (0xFF5F5F5F) );

        count++;
        start_y -= FunitY * base.GetHeight() / rangey;
      }
    }

    if (m_maxY <= 0)
    {
      int center = (0 - m_minY) * (float) base.GetHeight() / (float) (m_maxY - m_minY);
      int start_y = base.y + base.GetHeight() - center + FunitY * base.GetHeight() / rangey;
      int count = 0;

      while (start_y < base.y + base.GetHeight() )
      {
        GetPainter().Draw2DLine (GfxContext, base.x, start_y,
                             base.x + base.GetWidth(), start_y, Color (0xFF5F5F5F) );

        count++;
        start_y += FunitY * base.GetHeight() / rangey;
      }
    }
  }

  void BezierCurveControl2::DrawCoordinateSystem (GraphicsEngine &GfxContext)
  {
    int W = GetBaseWidth() - 2 * GRAPH_MARGIN;
    int H = GetBaseHeight() - 2 * GRAPH_MARGIN;
    int X = GetBaseX() + GRAPH_MARGIN;
    int Y = GetBaseY() + GRAPH_MARGIN;

    int X0, Y0;
    X0 = X + W * (0 - m_minX) / (m_maxX - m_minX);
    Y0 = Y + H * ( 1 - (0 - m_minY) / (m_maxY - m_minY) );

    GetPainter().Draw2DLine (GfxContext, X0, Y, X0, Y + H, Color (0xFF222222) );
    GetPainter().Draw2DLine (GfxContext, X, Y0, X + W, Y0, Color (0xFF000000) );
  }

  void BezierCurveControl2::DrawContent (GraphicsEngine &GfxContext, bool force_draw)
  {

  }

  void BezierCurveControl2::PostDraw (GraphicsEngine &GfxContext, bool force_draw)
  {

  }


  void BezierCurveControl2::SetXAxisBounds (float minX, float maxX)
  {
    m_minX = minX;
    m_maxX = maxX;
    QueueDraw();
  }

  void BezierCurveControl2::SetYAxisBounds (float minY, float maxY)
  {
    m_minY = minY;
    m_maxY = maxY;
    QueueDraw();
  }

  void BezierCurveControl2::SetFunctionCallback (FunctionCallback f)
  {
    m_FunctionCallback = f;
    QueueDraw();
  }

  float BezierCurveControl2::EvalFunction (float x)
  {
    if (m_FunctionCallback != 0)
      return (*m_FunctionCallback) (x);

    return 0;
  }

  void BezierCurveControl2::UpdateGraph()
  {
    QueueDraw();
  }


// check if a value lies within a closed interval
#ifndef INSIDE_BOUNDS
#define INSIDE_BOUNDS( x, lo, hi ) ( (x) >= (lo) && (x) <= (hi) )
#endif

//check if a 2D point lies within a 2D box
#ifndef PT_INSIDE_BOX
#define PT_INSIDE_BOX( x, y, lo_x, hi_x, lo_y, hi_y ) ( INSIDE_BOUNDS(x,lo_x,hi_x) && INSIDE_BOUNDS(y,lo_y,hi_y) )
#endif

  void BezierCurveControl2::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    QueueDraw();
  }

  void BezierCurveControl2::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (! (button_flags & NUX_EVENT_BUTTON1) )
    {
      return;
    }

    m_bControlPointSelected = false;
    t_u32 nbKnot = (t_u32) m_control_knot.size();

    for (t_u32 i = 0; i < nbKnot; i++)
    {
      m_control_knot[i].m_IsSelected = false;
    }

    int W = GetBaseWidth() - 2 * GRAPH_MARGIN;
    int H = GetBaseHeight() - 2 * GRAPH_MARGIN;
    int X = GetBaseX() + GRAPH_MARGIN;
    int Y = GetBaseY() + GRAPH_MARGIN;

    bool b = PT_INSIDE_BOX (X - GRAPH_MARGIN + x, Y - GRAPH_MARGIN + y, X - GRAPH_MARGIN, X + W + 2 * GRAPH_MARGIN, Y - GRAPH_MARGIN, Y + H + 2 * GRAPH_MARGIN);

    if (b == false)
      return;

//    X = GetX();
//    Y = GetY();
    for (t_u32 i = 0; i < nbKnot; i++)
    {
      int Xp, Yp;
      Xp = X + W * (m_control_knot[i].m_X - m_minX) / (m_maxX - m_minX);
      Yp = Y + H * ( 1 - (m_control_knot[i].m_Y - m_minY) / (m_maxY - m_minY) );

      int tx, ty;
      tx = X + x - GRAPH_MARGIN;
      ty = Y + y - GRAPH_MARGIN;

      if (PT_INSIDE_BOX (tx, ty, Xp - KNOT_HIT_TEST, Xp + KNOT_HIT_TEST, Yp - KNOT_HIT_TEST, Yp + KNOT_HIT_TEST) )
      {
        m_control_knot[i].m_IsSelected = true;
        hit_point_dx = (tx) - Xp;
        hit_point_dy = Yp - (ty);

        m_bControlPointSelected = true;

        if ( (i % CURVE_DEGREE) == 0)
        {
          // this is a control point at the begining of a cubic bezier curve
          // Also select the control points defining the entering and exiting tangent at this point.
        }

        break;
      }
    }

    QueueDraw();
  }

  int ClosestCubicBezierEndPoint (int i)
  {
    if ( (i % CURVE_DEGREE) == 0)
    {
      return i;
    }

    if ( ( (i + 1) % CURVE_DEGREE) == 0)
    {
      return i + 1;
    }

    if ( ( (i - 1) % CURVE_DEGREE) == 0)
    {
      return i - 1;
    }

    return 0;
  }

  void BezierCurveControl2::RecvKeyEvent (
    unsigned long   eventType  , /*event type*/
    unsigned long   keysym     , /*event keysym*/
    unsigned long   state      , /*event state*/
    const TCHAR*    character  , /*character*/
    unsigned short  keyCount     /*key repeat count*/
  )
  {

  }

  void BezierCurveControl2::RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    if ( (m_bControlPointSelected == false) && (button_flags & NUX_STATE_BUTTON1_DOWN) && S_KEY)
    {
      if (m_bPanningEnabled)
      {
        ProcessPanning (x, y, dx, dy, button_flags, key_flags);
        return;
      }
    }

    if ( (m_bControlPointSelected == false) && (button_flags & NUX_STATE_BUTTON3_DOWN) && S_KEY)
    {
      if (m_bZoomingEnabled)
      {
        ProcessZooming (x, y, dx, dy, button_flags, key_flags);
        return;
      }
    }

    if (button_flags & NUX_STATE_BUTTON1_DOWN)
      ManipulateBezier (x, y, dx, dy, button_flags, key_flags);
  }

  void BezierCurveControl2::ManipulateBezier (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    float xp, yp;
    int W = GetBaseWidth() - 2 * GRAPH_MARGIN;
    int H = GetBaseHeight() - 2 * GRAPH_MARGIN;
    int X = GetBaseX() + GRAPH_MARGIN;
    int Y = GetBaseY() + GRAPH_MARGIN;

    t_s32 nbKnot = (t_s32) m_control_knot.size();

    xp = /*m_minX +*/ dx * (m_maxX - m_minX) / W;
    yp = /*m_minY +*/ dy * (m_maxY - m_minY) / H;

    nuxAssert (nbKnot >= CURVE_DEGREE);
    int x_border = 0;
    int y_border = 0;

    for (t_s32 i = 0; i < nbKnot; i++)
    {
      if (m_control_knot[i].m_IsSelected)
      {
        int tempx, tempy;
        tempx = X + x - GRAPH_MARGIN;
        tempy = Y + y - GRAPH_MARGIN;

        int Xp, Yp;
        Xp = X + W * (m_control_knot[i].m_X - m_minX) / (m_maxX - m_minX);
        Yp = Y + H * ( 1 - (m_control_knot[i].m_Y - m_minY) / (m_maxY - m_minY) );

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

        if (m_bPanningEnabled == false)
        {
          if (m_control_knot[i].m_X + xp < m_minX)
          {
            xp = m_minX - m_control_knot[i].m_X;
            x_border = -1;
          }
          else if (m_control_knot[i].m_X + xp > m_maxX)
          {
            xp = m_maxX - m_control_knot[i].m_X;
            x_border = +1;
          }

          if (m_control_knot[i].m_Y - yp < m_minY)
          {
            yp = m_control_knot[i].m_Y - m_minY;
            y_border = -1;
          }
          else if (m_control_knot[i].m_Y - yp > m_maxY)
          {
            yp = m_control_knot[i].m_Y - m_maxY;
            y_border = +1;
          }
        }


        if ( (i % CURVE_DEGREE) == 0)
        {
          if (y_border == 0)
            m_control_knot[i].m_Y -= yp;
          else if (y_border == -1)
            m_control_knot[i].m_Y = m_minY;
          else if (y_border == +1)
            m_control_knot[i].m_Y = m_maxY;

          // Only endpoint of cubic bezier curves.
          if (i == 0)
          {
            if (x_border == 0)
            {
              if ( m_control_knot[i].m_X + xp > m_control_knot[i + CURVE_DEGREE].m_X - 0.01)
              {
                xp = (m_control_knot[i + CURVE_DEGREE].m_X - 0.01) - m_control_knot[i].m_X;
                m_control_knot[i].m_X = m_control_knot[i + CURVE_DEGREE].m_X - 0.01;
              }
              else
              {
                m_control_knot[i].m_X += xp;
              }
            }
            else
            {
              if (x_border == -1)
                m_control_knot[i].m_X = m_minX;
              else if (x_border == +1)
                m_control_knot[i].m_X = m_maxX;

              if ( m_control_knot[i].m_X > m_control_knot[i + CURVE_DEGREE].m_X - 0.01)
              {
                xp += (m_control_knot[i + CURVE_DEGREE].m_X - 0.01) - m_control_knot[i].m_X;
                m_control_knot[i].m_X = m_control_knot[i + CURVE_DEGREE].m_X - 0.01;
              }
            }

            // move the right tangent points
            {
              m_control_knot[i+1].m_X += xp;
              m_control_knot[i+1].m_Y -= yp;

              if (m_control_knot[i+1].m_X < m_control_knot[i].m_X)
              {
                m_control_knot[i+1].m_X = m_control_knot[i].m_X;
              }
            }
          }
          else if (i == nbKnot - 1)
          {
            if (x_border == 0)
            {
              if (m_control_knot[i].m_X + xp < m_control_knot[i - CURVE_DEGREE].m_X + 0.01)
              {
                xp = - (m_control_knot[i].m_X - (m_control_knot[i - CURVE_DEGREE].m_X + 0.01) );
                m_control_knot[i].m_X = m_control_knot[i - CURVE_DEGREE].m_X + 0.01;
              }
              else
              {
                m_control_knot[i].m_X += xp;
              }
            }
            else
            {
              if (x_border == -1)
                m_control_knot[i].m_X = m_minX;
              else if (x_border == +1)
                m_control_knot[i].m_X = m_maxX;

              if (m_control_knot[i].m_X < m_control_knot[i - CURVE_DEGREE].m_X + 0.01)
              {
                xp += (m_control_knot[i - CURVE_DEGREE].m_X + 0.01) - m_control_knot[i].m_X;
                m_control_knot[i].m_X = m_control_knot[i - CURVE_DEGREE].m_X + 0.01;
              }
            }

            // move the left tangent points
            {
              m_control_knot[i-1].m_X += xp;
              m_control_knot[i-1].m_Y -= yp;

              if (m_control_knot[i-1].m_X > m_control_knot[i].m_X)
              {
                m_control_knot[i-1].m_X = m_control_knot[i].m_X;
              }
            }
          }
          else
          {
            if (m_control_knot[i].m_X + xp > m_control_knot[i + CURVE_DEGREE].m_X - 0.01)
            {
              xp = (m_control_knot[i + CURVE_DEGREE].m_X - 0.01) - m_control_knot[i].m_X;
              m_control_knot[i].m_X = m_control_knot[i + CURVE_DEGREE].m_X - 0.01;
            }
            else if (m_control_knot[i].m_X + xp < m_control_knot[i - CURVE_DEGREE].m_X + 0.01)
            {
              xp = m_control_knot[i].m_X - (m_control_knot[i - CURVE_DEGREE].m_X + 0.01);
              m_control_knot[i].m_X = m_control_knot[i - CURVE_DEGREE].m_X + 0.01;
            }
            else
            {
              m_control_knot[i].m_X += xp;
            }

            // move the left and right tangent points
            {
              m_control_knot[i-1].m_X += xp;
              m_control_knot[i+1].m_X += xp;
              m_control_knot[i-1].m_Y -= yp;
              m_control_knot[i+1].m_Y -= yp;

              if (m_control_knot[i-1].m_X > m_control_knot[i].m_X)
              {
                m_control_knot[i-1].m_X = m_control_knot[i].m_X;
              }

              if (m_control_knot[i+1].m_X < m_control_knot[i].m_X)
              {
                m_control_knot[i+1].m_X = m_control_knot[i].m_X;
              }
            }
          }
        }
        else
        {
          m_control_knot[i].m_X += xp;
          m_control_knot[i].m_Y -= yp;

          t_s32 index = (t_s32) ClosestCubicBezierEndPoint (i);

          if (index > i)
          {
            if (m_control_knot[i].m_X > m_control_knot[index].m_X)
            {
              m_control_knot[i].m_X = m_control_knot[index].m_X;
            }
          }
          else if ( index < i)
          {
            if (m_control_knot[i].m_X < m_control_knot[index].m_X)
            {
              m_control_knot[i].m_X = m_control_knot[index].m_X;
            }
          }
        }

        break;
      }
    }

    QueueDraw();
  }

  void BezierCurveControl2::ProcessPanning (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    float xp, yp;
    int W = GetBaseWidth() - 2 * GRAPH_MARGIN;
    int H = GetBaseHeight() - 2 * GRAPH_MARGIN;

    xp = /*m_minX +*/ dx * (m_maxX - m_minX) / W;
    yp = /*m_minY +*/ dy * (m_maxY - m_minY) / H;

    if (m_bControlPointSelected == false)
    {
      if (m_bPanningEnabled)
      {
        m_minX -= xp;
        m_maxX -= xp;
        m_minY += yp;
        m_maxY += yp;

        QueueDraw();
        return;
      }
    }
  }

  void BezierCurveControl2::ProcessZooming (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
    int H = GetBaseHeight() - 2 * GRAPH_MARGIN;

    float yp = dy * (m_maxY - m_minY) / H;

    if ( (m_minX + yp >= m_maxX - yp) || (m_minY + yp >= m_maxY - yp) )
    {
      yp = yp / 2.0f;
    }

    if (m_bControlPointSelected == false)
    {
      if (m_bZoomingEnabled)
      {
        m_minX += yp;
        m_maxX -= yp;
        m_minY += yp;
        m_maxY -= yp;

        QueueDraw();
        return;
      }
    }
  }
}
