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
#include "NuxGraphics/GLTextureResourceManager.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"
#include "ActionItem.h"
#include "ToolButton.h"

NAMESPACE_BEGIN_GUI

ToolButton::ToolButton(const TCHAR* BitmapFilename)
:   m_ActionItem(0)
,   m_Bitmap()
{
    if(BitmapFilename)
        m_Bitmap.Update(BitmapFilename);

    // Set Original State
    SetBaseString(TEXT("ToolButton"));

    // Set Signals
    OnMouseClick.connect(sigc::mem_fun(this, &ToolButton::EmitClick));
    OnMouseDoubleClick.connect(sigc::mem_fun(this, &ToolButton::RecvMouseDoubleClick));
    OnMouseDown.connect(sigc::mem_fun(this, &ToolButton::RecvMouseDown));
    OnMouseUp.connect(sigc::mem_fun(this, &ToolButton::RecvMouseUp));
    OnMouseEnter.connect(sigc::mem_fun(this, &ToolButton::RecvMouseEnter));
    OnMouseLeave.connect(sigc::mem_fun(this, &ToolButton::RecvMouseLeave));

    SetMinimumSize(28, 28);
    setGeometry(Geometry(0, 0, 24, 24));
    SetSizePolicy(eSizeFixed);
}

ToolButton::~ToolButton()
{
}

long ToolButton::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;

    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
}

void ToolButton::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    bool highlighted = false;

    Geometry base = GetGeometry();
    if(IsMouseInside() && !HasMouseFocus())
    {
        gPainter.PaintBackground(GfxContext, base);
        gPainter.PaintShape(GfxContext, base, Color(COLOR_BACKGROUND_SECONDARY),  eSHAPE_CORNER_ROUND2);
    }
    else if(HasMouseFocus())
    {
        gPainter.PaintBackground(GfxContext, base);
        gPainter.PaintShape(GfxContext, base, Color(0xFF2A2A2A),  eSHAPE_CORNER_ROUND2);
    }
    else
    {
        gPainter.PaintBackground(GfxContext, base);
        gPainter.PaintShape(GfxContext, base, Color(COLOR_BACKGROUND_PRIMARY),  eSHAPE_CORNER_ROUND2);
    }
    
    if(!m_Bitmap.IsNull())
        gPainter.Draw2DTextureAligned(GfxContext, &m_Bitmap, base, TextureAlignmentStyle(eTACenter, eTACenter));
}

void ToolButton::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{

}

void ToolButton::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void ToolButton::SetState(bool b)
{

}

void ToolButton::SetBitmap(const NTexture2D& Bitmap)
{
    m_Bitmap = Bitmap;
}

void ToolButton::EmitClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    sigClick.emit();
    m_ActionItem.Trigger();
}

void ToolButton::RecvMouseDoubleClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void ToolButton::RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void ToolButton::RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void ToolButton::RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void ToolButton::RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void ToolButton::SetAction(const ActionItem& action)
{
    m_ActionItem = action;
}

NAMESPACE_END_GUI
