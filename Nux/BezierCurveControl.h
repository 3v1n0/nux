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


#ifndef BEZIERCURVECONTROL_H
#define BEZIERCURVECONTROL_H

namespace nux { //NUX_NAMESPACE_BEGIN

class Knot
{
public :

    float  m_X;
    float  m_Y;

    bool m_IsSelected;

public :

    Knot()
        :   m_X(0)
        ,   m_Y(0)
        ,   m_IsSelected(false)
    {}	//Constructors	
    Knot(float ptX, float ptY)
        :   m_X(ptX)
        ,   m_Y(ptY)
        ,   m_IsSelected(false)
    {}

    void setPoint(float x, float y)  {m_X = x; m_Y = y;}	//Setting
    

    //Operator overloading
    void operator = (Knot knot)   {m_X = knot.m_X; m_Y = knot.m_Y;}
    bool operator != (Knot knot)
    {
        bool b;
        b = ((m_X != knot.m_X) || (m_Y != knot.m_Y)) ? true : false;
        return b;
    }
};


typedef float (*FunctionCallback)(float);  

class BezierCurveControl : public ActiveInterfaceObject
{
public:
    BezierCurveControl(NUX_FILE_LINE_PROTO);
    ~BezierCurveControl();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);


    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
private:
    void SetXAxisBounds(float minX, float maxX);
    void SetYAxisBounds(float minY, float maxY);
    void SetFunctionCallback(FunctionCallback f);
    float EvalFunction(float x);
    void UpdateGraph();

    std::vector<Knot> m_control_knot;

    float m_minX, m_minY, m_maxX, m_maxY;
    FunctionCallback m_FunctionCallback;
};


} //NUX_NAMESPACE_END

#endif // BEZIERCURVECONTROL_H
