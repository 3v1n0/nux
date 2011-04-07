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


#ifndef BEZIERCURVECONTROL2_H
#define BEZIERCURVECONTROL2_H

#include "PaintLayer.h"

namespace nux
{

  class Knot2
  {
  public :

    float  m_X;
    float  m_Y;

    bool m_IsSelected;

  public :

    Knot2() : m_X (0), m_Y (0), m_IsSelected (false) {}	//Constructors
    Knot2 (float ptX, float ptY) :
      m_X (ptX), m_Y (ptY), m_IsSelected (false) {}

    void setPoint (float x, float y)
    {
      m_X = x;  //Setting
      m_Y = y;
    }


    //Operator overloading
    void operator = (Knot2 knot)
    {
      m_X = knot.m_X;
      m_Y = knot.m_Y;
    }
    bool operator != (Knot2 knot)
    {
      bool b;
      b = ( (m_X != knot.m_X) || (m_Y != knot.m_Y) ) ? true : false;
      return b;
    }
  };


  typedef float (*FunctionCallback) (float);

  class BezierCurveControl2 : public View
  {
  public:
    BezierCurveControl2 (NUX_FILE_LINE_PROTO);
    ~BezierCurveControl2();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void EnablePanning (bool b)
    {
      m_bPanningEnabled = b;
    }
    bool IsPanningEnabled() const
    {
      return  m_bPanningEnabled;
    }
    void EnableZooming (bool b)
    {
      m_bZoomingEnabled = b;
    }
    bool IsZoomingEnable() const
    {
      return m_bZoomingEnabled;
    }

    void RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void RecvKeyEvent
    (
      GraphicsEngine &GfxContext , /*Graphics Context for text operation*/
      unsigned long    eventType  , /*event type*/
      unsigned long    keysym     , /*event keysym*/
      unsigned long    state      , /*event state*/
      const TCHAR*     character  , /*character*/
      unsigned short   keyCount     /*key repeat count*/
    );

  private:
    void DrawRuler (GraphicsEngine &GfxContext);
    void DrawGrid (GraphicsEngine &GfxContext);
    void DrawCoordinateSystem (GraphicsEngine &GfxContext);

    void SetXAxisBounds (float minX, float maxX);
    void SetYAxisBounds (float minY, float maxY);
    void SetFunctionCallback (FunctionCallback f);
    float EvalFunction (float x);
    void UpdateGraph();

    void ManipulateBezier (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void ProcessPanning (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void ProcessZooming (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    std::vector<Knot2> m_control_knot;

    float m_minX, m_minY, m_maxX, m_maxY;
    FunctionCallback m_FunctionCallback;

    AbstractPaintLayer *m_Background;

    bool m_bControlPointSelected;
    bool m_bPanningEnabled;
    bool m_bZoomingEnabled;

    float hit_point_dx;
    float hit_point_dy;


  };


}

#endif // BEZIERCURVECONTROL2_H
