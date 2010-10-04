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
#include "Matrix4Preview.h"
#include "StaticTextBox.h"

namespace nux { //NUX_NAMESPACE_BEGIN

static const int GRAPH_MARGIN = 1;

Matrix4Preview::Matrix4Preview(Matrix4 matrix, NUX_FILE_LINE_DECL)
:   ActiveInterfaceObject(NUX_FILE_LINE_PARAM)
{
    m_DialogThreadProxy = new Matrix4DialogProxy(true);

    SetMinMaxSize(30, 30);
    
    OnMouseClick.connect(sigc::mem_fun(this, &Matrix4Preview::RecvClick));

    m_ChangeDetectionTimer = new TimerFunctor();
    m_ChangeDetectionTimer->OnTimerExpired.connect(sigc::mem_fun(this, &Matrix4Preview::RecvTimer));
    m_ChangeTimerHandler = 0;
}

Matrix4Preview::~Matrix4Preview()
{
    if(m_ChangeTimerHandler.IsValid())
        GetThreadTimer().RemoveTimerHandler(m_ChangeTimerHandler);
    NUX_SAFE_DELETE(m_DialogThreadProxy);
}

long Matrix4Preview::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;
    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
}

void Matrix4Preview::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();

    gPainter.PaintBackground(GfxContext, base);
    //gPainter.PaintShape(GfxContext, base, 0xFF4D4D4D, eSHAPE_CORNER_ROUND4, false);

    GeometryPositioning gp(eHACenter, eVACenter);
    Geometry GeoPo = ComputeGeometryPositioning(base, gTheme.GetImageGeometry(eMATRIX4PREVIEW), gp);
    gPainter.PaintShape(GfxContext, GeoPo, Color(0xFFFFFFFF), eMATRIX4PREVIEW);

    gPainter.Paint2DQuadWireframe(GfxContext, base, Color(COLOR_BACKGROUND_SECONDARY));

    //gPainter.Paint2DQuadWireframe(GfxContext, base, Color(COLOR_BACKGROUND_SECONDARY));
}

void Matrix4Preview::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
}

void Matrix4Preview::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void Matrix4Preview::RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    m_DialogThreadProxy->Start();
}

void Matrix4Preview::RecvTimer(void* v)
{
    if(m_DialogThreadProxy->m_bDialogChange && m_DialogThreadProxy->m_bDialogRunning)
    {
        m_DialogThreadProxy->m_bDialogChange = false;
        m_Matrix = m_DialogThreadProxy->GetMatrix();
    }
    if(m_DialogThreadProxy->IsActive())
    {
        m_ChangeTimerHandler = GetThreadTimer().AddTimerHandler(33, m_ChangeDetectionTimer, this);
    }
    else
    {
        if(m_ChangeTimerHandler.IsValid())
            GetThreadTimer().RemoveTimerHandler(m_ChangeTimerHandler);
        m_ChangeTimerHandler = 0;

        m_Matrix = m_DialogThreadProxy->GetMatrix();
    }
}

void Matrix4Preview::RecvDialogChange(Matrix4Editor* matrixeditor)
{
}

void Matrix4Preview::SetMatrix(Matrix4 matrix)
{
    m_Matrix = matrix;
}

Matrix4 Matrix4Preview::GetMatrix() const
{
    return m_Matrix;
}


} //NUX_NAMESPACE_END
