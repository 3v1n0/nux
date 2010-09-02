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


#ifndef BEZIERCURVECONTROL2_H
#define BEZIERCURVECONTROL2_H

#include "PaintLayer.h"

NAMESPACE_BEGIN_GUI

class Knot2
{
public :

    float  m_X;
    float  m_Y;

    bool m_IsSelected;

public :

    Knot2() : m_X(0), m_Y(0), m_IsSelected(false) {}	//Constructors	
    Knot2(float ptX, float ptY) : 
    m_X(ptX), m_Y(ptY), m_IsSelected(false) {}

    void setPoint(float x, float y)  {m_X = x; m_Y = y;}	//Setting
    

    //Operator overloading
    void operator = (Knot2 knot)   {m_X = knot.m_X; m_Y = knot.m_Y;}
    bool operator != (Knot2 knot)
    {
        bool b;
        b = ((m_X != knot.m_X) || (m_Y != knot.m_Y)) ? true : false;
        return b;
    }
};


typedef float (*FunctionCallback)(float);  

class BezierCurveControl2 : public ActiveInterfaceObject
{
public:
    BezierCurveControl2();
    ~BezierCurveControl2();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void EnablePanning(bool b)      { m_bPanningEnabled = b; }
    bool IsPanningEnabled() const   { return  m_bPanningEnabled; }
    void EnableZooming(bool b)      { m_bZoomingEnabled = b; }
    bool IsZoomingEnable() const    { return m_bZoomingEnabled; }

    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    void RecvKeyEvent
        (
        GraphicsContext& GfxContext , /*Graphics Context for text operation*/
        unsigned long    eventType  , /*event type*/
        unsigned long    keysym     , /*event keysym*/
        unsigned long    state      , /*event state*/
        const char*      character  , /*character*/
        bool             isRepeated , /*true if the key is repeated more than once*/
        unsigned short   keyCount     /*key repeat count*/
        );

private:
    void DrawRuler(GraphicsContext& GfxContext);
    void DrawGrid(GraphicsContext& GfxContext);
    void DrawCoordinateSystem(GraphicsContext& GfxContext);

    void SetXAxisBounds(float minX, float maxX);
    void SetYAxisBounds(float minY, float maxY);
    void SetFunctionCallback(FunctionCallback f);
    float EvalFunction(float x);
    void UpdateGraph();

    void ManipulateBezier(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void ProcessPanning(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void ProcessZooming(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    std::vector<Knot2> m_control_knot;

    float m_minX, m_minY, m_maxX, m_maxY;
    FunctionCallback m_FunctionCallback;
    
    AbstractPaintLayer* m_Background;

    bool m_bControlPointSelected;
    bool m_bPanningEnabled;
    bool m_bZoomingEnabled;

    float hit_point_dx;
    float hit_point_dy;


};


NAMESPACE_END_GUI

#endif // BEZIERCURVECONTROL2_H