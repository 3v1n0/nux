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


#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "AbstractButton.h"

namespace nux
{
  class HLayout;
  class InputArea;

  class CheckBox: public AbstractButton
  {
  public:
    CheckBox (const TCHAR *Caption = 0, bool state = false, NUX_FILE_LINE_PROTO);
    ~CheckBox();

    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void RecvMouseMove (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);
    /*
        Signal emitted if a click happen. The state change and the check box need to redraw itself.
    */
    void RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags);
    /*
        Signal emitted if the mouse is released. Whether a click happened or not,
        the check box need to redraw itself.
    */
    void RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);

    sigc::signal<void, CheckBox *> sigStateToggled;
    sigc::signal<void> sigToggled;
    sigc::signal<void, bool> sigStateChanged;

    void EmitStateSignal();

    virtual void SetCaption (const TCHAR *Caption);
    virtual const NString &GetCaption() const;

    virtual void SetState (bool State);
    virtual void SetState (bool State, bool EmitSignal);
    virtual bool GetState() const;
    //---

  protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

  private:
    HLayout    *m_hlayout;
    CoreArea   *m_TextArea;
    CoreArea   *m_CheckArea;
    bool        m_State;
  };
}

#endif // CHECKBOX_H
