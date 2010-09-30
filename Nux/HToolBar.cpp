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

#include "HToolBar.h"
#include "HLayout.h"
#include "ToolButton.h"
#include "ActionItem.h"
#include "AbstractSeparator.h"
#include "VSeparator.h"
#include "EditTextBox.h"
#include "StaticTextBox.h"

namespace nux { //NUX_NAMESPACE_BEGIN

HToolBar::HToolBar(NUX_FILE_LINE_DECL)
:   ActiveInterfaceObject(NUX_FILE_LINE_PARAM)
,   m_hlayout(0)
{ 
    InitializeLayout();
    InitializeWidgets();
}

HToolBar::~HToolBar()
{
    DestroyLayout();
}

void HToolBar::InitializeWidgets()
{
    m_hlayout->SetHorizontalInternalMargin(2);
    m_hlayout->SetHorizontalExternalMargin(0);
    m_hlayout->SetVerticalExternalMargin(0);
    m_hlayout->SetContentStacking(eStackLeft);
    SetMinimumSize(200, 32);
    SetMaximumHeight(32);
    SetGeometry(Geometry(0, 0, 200, 20));
    SetCompositionLayout(m_hlayout);
}

void HToolBar::InitializeLayout()
{
    m_hlayout = smptr(HLayout)(new HLayout());
}

void HToolBar::DestroyLayout()
{
}

long HToolBar::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;

    ret = m_hlayout->ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);

    // PostProcessEvent2 must always have its last parameter set to 0
    // because the m_BackgroundArea is the real physical limit of the window.
    // So the previous test about IsPointInside do not prevail over m_BackgroundArea 
    // testing the event by itself.
    ret = PostProcessEvent2(ievent, ret, 0);
    return ret;
}

void HToolBar::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle(base);
    GfxContext.SetEnvModeSelectColor(GL_TEXTURE0);
    GfxContext.QRP_GLSL_Color(base.x, base.y, base.GetWidth(), base.GetHeight(), COLOR_BACKGROUND_PRIMARY);

    GfxContext.PopClippingRectangle();
}

void HToolBar::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle(base);
    gPainter.PushColorLayer(GfxContext, base, COLOR_BACKGROUND_PRIMARY);
    m_hlayout->ProcessDraw(GfxContext, force_draw);
    gPainter.PopBackground();
    GfxContext.PopClippingRectangle();
}

void HToolBar::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void HToolBar::AddToolButton(smptr(ToolButton) toolbutton)
{
    nuxAssert(toolbutton.IsValid());
    if(toolbutton == 0)
        return;

    m_hlayout->AddActiveInterfaceObject(toolbutton, 0, eCenter, eFix);
    // 0: the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
    // and the children will take their natural size by expending WidgetLayout.
    // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
    ComputeChildLayout();
}

void HToolBar::AddAction(smptr(ActionItem) action)
{
//    nuxAssert(action);
//    if(action == 0)
//        return;

    smptr(ToolButton) tool_button(new ToolButton);
    tool_button->SetAction(action);

    m_hlayout->AddActiveInterfaceObject(tool_button, 0, eCenter, eFix);
    // 0: the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
    // and the children will take their natural size by expending WidgetLayout.
    // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
    ComputeChildLayout();
}

void HToolBar::AddSpace(int size)
{
    smptr(SpaceLayout) layout(new SpaceLayout());
    layout->SetMaximumWidth(size);
    layout->SetMinimumWidth(size);
    m_hlayout->AddLayout(layout);
    ComputeChildLayout();
}

void HToolBar::AddSeparator()
{
    smptr(VSeparator) separator(new VSeparator());
    m_hlayout->AddActiveInterfaceObject(separator, 0);
    ComputeChildLayout();
}

void HToolBar::AddEditTextLine(smptr(EditTextBox) edittextline)
{
    nuxAssert(edittextline.IsValid());
    if(edittextline == 0)
        return;

    m_hlayout->AddActiveInterfaceObject(edittextline, 0, eCenter, eFix);
    // 0: the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
    // and the children will take their natural size by expending WidgetLayout.
    // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
    ComputeChildLayout();
}

void HToolBar::AddStaticTextLine(smptr(StaticTextBox) statictextline)
{
    nuxAssert(statictextline.IsValid());
    if(statictextline == 0)
        return;

    m_hlayout->AddActiveInterfaceObject(statictextline, 0, eCenter, eFix);
    // 0: the WidgetLayout geometry will be set to SetGeometry(0,0,1,1);
    // and the children will take their natural size by expending WidgetLayout.
    // If the parent of WidgetLayout offers more space, it won't be used by WidgetLayout.
    ComputeChildLayout();
}

void HToolBar::ClearWidget()
{
    m_CompositionLayout->Clear();
}

void HToolBar::RecvMouseDownOnIcon(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void HToolBar::RecvMouseUpOnIcon(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void HToolBar::RecvMouseMoveOnIcon(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void HToolBar::RecvMouseDragOnIcon(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void HToolBar::RecvMouseEnterIcon(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void HToolBar::RecvMouseLeaveIcon(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}
} //NUX_NAMESPACE_END
