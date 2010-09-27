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


#ifndef MOUSEAREACTRL_H
#define MOUSEAREACTRL_H

#include "EditTextBox.h"
#include "Layout.h"
#include "HLayout.h"
#include "VLayout.h"

namespace nux { //NUX_NAMESPACE_BEGIN

//replace MouseAreaCtrl with GfxViewCtrl
class MouseAreaCtrl: public ActiveInterfaceObject  
{
public:
    MouseAreaCtrl(NUX_FILE_LINE_PROTO);
    ~MouseAreaCtrl();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    int getAreaPosY();
    int getAreaPosX();
    int getAreaWidth();
    int getAreaHeight();

    /////////////////
    //  EMITERS    //
    /////////////////
    void MouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void MouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void MouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);

    /////////////////
    //  SIGNALS    //
    /////////////////
    sigc::signal<void, int, int, long> sigMouseDown;
    sigc::signal<void, int, int, long> sigMouseUp;
    sigc::signal<void, int, int, int, int, long> sigMouseDrag;
    sigc::signal<void, bool> sigDraw;

private:
    smptr(VLayout) m_vlayout;
    smptr(CoreArea) m_Area;

public:
    virtual void setGeometry(const Geometry& geo)
    {
        BaseObject::setGeometry(geo);
        ComputeChildLayout();
    }

};


} //NUX_NAMESPACE_END

#endif // MOUSEAREACTRL_H
