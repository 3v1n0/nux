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


#ifndef PANEL_H
#define PANEL_H

#include "ScrollView.h"

NAMESPACE_BEGIN_GUI

class Layout;

class Panel: public ScrollView
{
public:
    Panel();
    ~Panel();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

    //void AddWidget(smptr(ActiveInterfaceObject) ic);
    void AddWidget(smptr(ActiveInterfaceObject) ic, int stretchfactor = 1);
    void AddWidget(std::list<smptr(ActiveInterfaceObject)> *InterfaceControlList);
    void SetLayout(smptr(Layout) layout);
    void clearContent();

    // Get a change to do any work on an element.
    // Here we need to position the header by hand because it is not under the control of vlayout.
    virtual void PreLayoutManagement();
    // Get a change to do any work on an element.
    // Here we need to position the header by hand because it is not under the control of vlayout.
    virtual long PostLayoutManagement(long LayoutResult);
    // Get a change to do any work on an element.
    // Here we need to position the header by hand because it is not under the control of vlayout.
    virtual void PositionChildLayout(float offsetX, float offsetY);

protected:

    virtual void ScrollLeft(float stepx, int mousedx);
    virtual void ScrollRight(float stepx, int mousedx);
    virtual void ScrollUp(float stepy, int mousedy);
    virtual void ScrollDown(float stepy, int mousedy);
private:

    std::list<smptr(ActiveInterfaceObject)> m_InterfaceObject;
    smptr(Layout) m_layout;
};

NAMESPACE_END_GUI
#endif // PANEL_H