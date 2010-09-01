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
#include "NuxCore/Math/Bezier.h"
#include "CurveControl.h"

NAMESPACE_BEGIN_GUI

CurveControl::CurveControl()
:m_minX(0.0f),
m_minY(0.0f),
m_maxX(1.0f),
m_maxY(1.0f),
m_FunctionCallback(0)
{
    SetMinimumSize(200, 100);
    SetBaseSize(200, 100);
}

CurveControl::~CurveControl()
{


}


long CurveControl::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{

    return TraverseInfo;
}


void CurveControl::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();

    gPainter.PaintBackground(GfxContext, base);
    gPainter.Paint2DQuadWireframe(GfxContext, base, Color(COLOR_BACKGROUND_SECONDARY));

#define N 2
    int i;
    int nsample = 10;
    double t;
    double xcon[N+1] = { 0.0, 0.5, 1.0 };
    double xval;
    double ycon[N+1] = { 0.0, 1.0,  0.0 };
    double yval;


    int W = GetBaseWidth()- 2;
    int H = GetBaseHeight() - 2;
    int X = GetBaseX() + 1;
    int Y = GetBaseY() + 1;


    float dX = (m_maxX - m_minX) / W;
    float dY = (m_maxY - m_minY) / H;

    double xprev, yprev;
    Bezier_XY(N, 0.0, xcon, ycon, &xprev, &yprev);

    //gPainter.Draw2DLine(X, Y, X+W, Y+H, Color(0xFFFF0000));

    base.OffsetPosition(1, 1);
    base.OffsetSize(-2, -2);

    GfxContext.PushClippingRectangle(base);

    for ( i = 1; i < nsample; i++ )
    {
        t = ( double ) ( i ) / ( double ) ( nsample - 1 );
        Bezier_XY ( N, t, xcon, ycon, &xval, &yval );

        int X0, Y0, X1, Y1;
        X0 = X + W * (xprev - m_minX) / (m_maxX - m_minX);
        Y0 = Y + H * ( 1 - (yprev - m_minY) / (m_maxY - m_minY));
        X1 = X + W * (xval - m_minX) / (m_maxX - m_minX);
        Y1 = Y + H * ( 1 - (yval - m_minY) / (m_maxY - m_minY));
        
        gPainter.Draw2DLine(GfxContext, X0, Y0, X1, Y1, Color(0xFFFF0000));

        xprev = xval;
        yprev = yval;
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
//        gPainter.Draw2DLine(X0, Y0, X1, Y1, Color(0xFFFF0000));
//
//        x0 = x1;
//        y0 = y1;
//
//    }
    GfxContext.PopClippingRectangle();
}


void CurveControl::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{

}

void CurveControl::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}


void CurveControl::SetXAxisBounds(float minX, float maxX)
{
    m_minX = minX;
    m_maxX = maxX;
    NeedRedraw();
}

void CurveControl::SetYAxisBounds(float minY, float maxY)
{
    m_minY = minY;
    m_maxY = maxY;
    NeedRedraw();
}

void CurveControl::SetFunctionCallback(FunctionCallback f)
{
    m_FunctionCallback = f;
    NeedRedraw();
}

float CurveControl::EvalFunction(float x)
{
    if(m_FunctionCallback != 0)
        return (*m_FunctionCallback)(x);

    return 0;
}

void CurveControl::UpdateGraph()
{
    NeedRedraw();
}


NAMESPACE_END_GUI
