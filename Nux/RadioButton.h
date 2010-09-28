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


#ifndef RADIOBUTTON_H
#define RADIOBUTTON_H

#include "AbstractButton.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class RadioButton;
class RadioButtonGroup;
class HLayout;

class RadioButton: public AbstractButton
{
public:
    RadioButton(const TCHAR* Caption = 0, bool state = false);

    ~RadioButton();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    void RecvMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);
    /*
        Signal emitted if a click happen. The state change and the check box need to redraw itself.
    */
    void RecvClick(int x, int y, unsigned long button_flags, unsigned long key_flags);
    /*
        Signal emitted if the mouse is released. Whether a click happened or not, 
        the check box need to redraw itself.
    */
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);

    sigc::signal<void, const weaksmptr(RadioButton)> sigStateToggled;
    sigc::signal<void> sigToggled;
    sigc::signal<void, bool> sigStateChanged;

    void EmitStateChangedSignal();

    //+++ Inherited from AbstractButton
    virtual bool IsCheckable() const {return true;}

    virtual void SetCaption(const TCHAR* Caption);
    virtual const NString& GetCaption() const;

    virtual void SetState(bool State);
    virtual void SetState(bool State, bool EmitSignal);
    virtual bool GetState() const;
    //---

protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

private:
    //! Intended for RadioButtonGroup only.
    void SetStatePrivate(bool State);
    //! Intended for RadioButtonGroup only.
    void SetStatePrivate(bool State, bool EmitSignal);


    smptr(HLayout) m_hlayout;
    smptr(CoreArea) m_TextArea;
    smptr(CoreArea) m_CheckArea;
    bool m_State;

    RadioButtonGroup* m_Group;
    int m_GroupId;

public:
    friend class RadioButtonGroup;
};

} //NUX_NAMESPACE_END

#endif // RADIOBUTTON_H
