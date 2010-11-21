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


#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H
#include "AbstractButton.h"

namespace nux
{
  class HLayout;

  class PushButton: public AbstractButton
  {
  public:
    PushButton (const TCHAR *Caption = TEXT (""), NUX_FILE_LINE_PROTO);
    ~PushButton();

    virtual void SetCaption (const TCHAR *Caption);
    virtual const NString &GetCaption() const;

    virtual void SetState (bool State);
    virtual void SetState (bool State, bool EmitSignal);
    virtual bool GetState() const;

    sigc::signal<void> sigClick;

  protected:
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw (GraphicsEngine &GfxContext, bool force_draw);
    virtual void DrawContent (GraphicsEngine &GfxContext, bool force_draw);
    virtual void PostDraw (GraphicsEngine &GfxContext, bool force_draw);

    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

    void RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseMove (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseEnter (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvClick (int x, int y, unsigned long button_flags, unsigned long key_flags);

  private:
  };

}

#endif // PUSHBUTTON_H
