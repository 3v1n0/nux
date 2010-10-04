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
#include "Button.h"
#include "HLayout.h"

namespace nux { //NUX_NAMESPACE_BEGIN

Button::Button(const TCHAR* Caption, NUX_FILE_LINE_DECL)
:   AbstractButton(Caption, NUX_FILE_LINE_PARAM)
{
    m_hlayout   = 0;
    m_Checkable = false;
    m_State     = false;

    InitializeLayout();
    InitializeWidgets();
    SetCheckable(m_Checkable, m_State, false);
    SetCaption(Caption);
}

Button::~Button()
{
    DestroyLayout();
}

void Button::InitializeWidgets()
{
    // Set Signals
    OnMouseClick.connect(sigc::mem_fun(this, &Button::RecvClick));
    OnMouseDown.connect(sigc::mem_fun(this, &Button::RecvMouseDown));
    OnMouseDoubleClick.connect(sigc::mem_fun(this, &Button::RecvMouseDown));
    OnMouseUp.connect(sigc::mem_fun(this, &Button::RecvMouseUp));
    OnMouseMove.connect(sigc::mem_fun(this, &Button::RecvMouseMove));
    OnMouseEnter.connect(sigc::mem_fun(this, &Button::RecvMouseEnter));
    OnMouseLeave.connect(sigc::mem_fun(this, &Button::RecvMouseLeave));

    // Set Geometry
    SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    m_hlayout->SetVerticalExternalMargin(0);
    m_hlayout->SetHorizontalExternalMargin(0);
    SetCompositionLayout(m_hlayout);
    SetTextColor(Color::Black);
}

void Button::InitializeLayout()
{
    m_hlayout = new HLayout();
}

void Button::DestroyLayout()
{
}

long Button::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;
    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
}

void Button::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();

    InteractState is;
    is.is_on = m_State;
    is.is_focus = HasMouseFocus();

    is.is_prelight = IsMouseInside();

    if(m_Checkable)
    {
        if(m_State)
        {
            gPainter.PushDrawSliceScaledTextureLayer(GfxContext, base, eBUTTON_FOCUS, Color::White, eAllCorners);
            gPainter.PopBackground();
        }
        else
        {
            gPainter.PushDrawSliceScaledTextureLayer(GfxContext, base, eBUTTON_NORMAL, Color::White, eAllCorners);
            gPainter.PopBackground();
        }
    }
    else
    {
        if(is.is_focus)
        {
            gPainter.PushDrawSliceScaledTextureLayer(GfxContext, base, eBUTTON_FOCUS, Color::White, eAllCorners);
            gPainter.PopBackground();
        }
        else if(is.is_prelight)
        {
            gPainter.PushDrawSliceScaledTextureLayer(GfxContext, base, eBUTTON_PRELIGHT, Color::White, eAllCorners);
            gPainter.PopBackground();
        }
        else
        {
            gPainter.PushDrawSliceScaledTextureLayer(GfxContext, base, eBUTTON_NORMAL, Color::White, eAllCorners);
            gPainter.PopBackground();
        }
    }
    gPainter.PaintTextLineStatic(GfxContext, GetFont(), base, GetBaseString().GetTCharPtr(), GetTextColor(), true, eAlignTextCenter);
}

void Button::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{

}

void Button::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void Button::SetCaption(const TCHAR* Caption)
{
    if(Caption == 0 || (StringLength(Caption) == 0))
    {
        SetBaseString(TEXT(""));
    }
    else
        SetBaseString(Caption);
}

const NString& Button::GetCaption() const
{
    return GetBaseString();
}

void Button::SetState(bool b)
{
    m_State = b;
    NeedRedraw();
}

void Button::SetState(bool State, bool EmitSignal)
{
    m_State = State;
    if(EmitSignal)
    {
        sigClick.emit();
        if(m_Checkable)
        {
            sigToggled.emit();
            sigStateChanged.emit(m_State);
        }
    }
    NeedRedraw();
}

bool Button::GetState() const
{
    return m_State;
}

void Button::SetCheckable(bool b)
{
    m_Checkable = b;
    NeedRedraw();
}

void Button::SetCheckable(bool b, bool CurrentState, bool EmitSignal)
{
    m_Checkable = b;
    m_State = CurrentState;
    if(EmitSignal)
    {       
        sigClick.emit();
    }
    NeedRedraw();
}

void Button::RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    m_State = !m_State;
    sigClick.emit();
    NeedRedraw();
}

void Button::RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void Button::RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void Button::RecvMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{

}

void Button::RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}

void Button::RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    NeedRedraw();
}




} //NUX_NAMESPACE_END
