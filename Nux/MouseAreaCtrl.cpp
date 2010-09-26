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
#include "MouseAreaCtrl.h"

NAMESPACE_BEGIN_GUI

MouseAreaCtrl::MouseAreaCtrl(NUX_FILE_LINE_DECL)
:   ActiveInterfaceObject(NUX_FILE_LINE_PARAM)
{
    // Set Original State
    m_vlayout = smptr(VLayout)(new VLayout());
    m_Area = smptr(CoreArea)(new CoreArea());

    // Set Signals
    m_Area->OnMouseDown.connect(sigc::mem_fun(this, &MouseAreaCtrl::MouseDown));
    m_Area->OnMouseUp.connect(sigc::mem_fun(this, &MouseAreaCtrl::MouseUp));
    m_Area->OnMouseDrag.connect(sigc::mem_fun(this, &MouseAreaCtrl::MouseDrag));

    // Set Geometry
    m_Area->SetMinimumSize(100, 100);
    m_Area->setGeometry(Geometry(0, 0, 200, 400));

    m_vlayout->AddActiveInterfaceObject(m_Area, 1);
    m_vlayout->SetVerticalExternalMargin(6);
    m_vlayout->SetHorizontalExternalMargin(6);
    SetCompositionLayout(m_vlayout);
}

MouseAreaCtrl::~MouseAreaCtrl()
{ 
}

long MouseAreaCtrl::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;
    ret = m_Area->OnEvent(ievent, ret, ProcessEventInfo);
    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);

    NeedRedraw();
    return ret;
}

void MouseAreaCtrl::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    gPainter.PaintShape(GfxContext, base, Color(COLOR_BACKGROUND_SECONDARY),  eSHAPE_CORNER_ROUND10);

    sigDraw.emit(force_draw);
}

void MouseAreaCtrl::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    sigDraw.emit(force_draw);
}

void MouseAreaCtrl::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

int MouseAreaCtrl::getAreaPosX()
{
    return m_Area->GetBaseX();
}
int MouseAreaCtrl::getAreaPosY()
{
    return m_Area->GetBaseY();
}
int MouseAreaCtrl::getAreaWidth()
{
    return m_Area->GetBaseWidth();
}
int MouseAreaCtrl::getAreaHeight()
{
    return m_Area->GetBaseHeight();
}

/////////////////
//  EMITERS    //
/////////////////
void MouseAreaCtrl::MouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    sigMouseDown.emit(x, y, button_flags);
}
void MouseAreaCtrl::MouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    sigMouseUp.emit(x, y, button_flags);
}
void MouseAreaCtrl::MouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    sigMouseDrag.emit(x, y, dx, dy, button_flags);
}



NAMESPACE_END_GUI
