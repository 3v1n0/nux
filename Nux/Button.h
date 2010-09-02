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


#ifndef BUTTON_H
#define BUTTON_H
#include "AbstractButton.h"

NAMESPACE_BEGIN_GUI

class HLayout;

class Button: public AbstractButton
{
public:
    static const bool Checked = true;
    static const bool Unchecked = false;

public:
    Button(const TCHAR* Caption = TEXT(""), bool Checkable = false, bool m_State = Unchecked);
    ~Button();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);


    //+++ Inherited from AbstractButton
    virtual bool IsCheckable() const {return m_Checkable;}

    virtual void SetCaption(const TCHAR* Caption);
    virtual const NString& GetCaption() const;

    virtual void SetState(bool State);
    virtual void SetState(bool State, bool EmitSignal);
    virtual bool GetState() const;
    //---


    void SetCheckable(bool b);
    void SetCheckable(bool b, bool State, bool EmitSignal);

    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);

    sigc::signal<void> sigClick;
    sigc::signal<void> sigToggled;
    sigc::signal<void, bool> sigStateChanged;

protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

private:
    smptr(HLayout) m_hlayout;
    bool m_Checkable;

};

NAMESPACE_END_GUI

#endif // BUTTON_H
