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


#ifndef MENUBAR_H
#define MENUBAR_H

#include "ScrollView.h"
#include "ToolButton.h"
#include "MenuPage.h"
#include "Painter.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class PopupBox;
class BaseWindow;
class MenuBar;
class HLayout;


class MenuBarItem: public NuxObject
{
public:
    NUX_DECLARE_OBJECT_TYPE(MenuBarItem, NuxObject);

    MenuBarItem(NUX_FILE_LINE_PROTO);
    ~MenuBarItem();
private:
    smptr(CoreArea) area;
    smptr(MenuPage) menu;
    NTexture *icon; // should be 24x24

    friend class MenuBar;
};

class MenuBar: public ActiveInterfaceObject
{
    NUX_DECLARE_OBJECT_TYPE(MenuBar, ActiveInterfaceObject);

public:
    MenuBar(NUX_FILE_LINE_PROTO);
    ~MenuBar();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);
    
    //void AddActionItem(ActionItem* actionItem);

    void AddMenu(const TCHAR* MenuLabel, smptr(MenuPage) popup);
    void AddMenu(const TCHAR* MenuLabel, smptr(MenuPage) menu, NTexture *icon);


    /////////////////
    //  EMITERS    //
    /////////////////

private:
    void EmitItemMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item);
    void EmitItemMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item);
    void EmitItemMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item);
    void EmitItemMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item);
    void RecvItemMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item);
    void RecvSigActionTriggered(smptr(MenuPage), const smptr(ActionItem));
    void RecvSigTerminateMenuCascade();

    //! Process a mouse down event outside of the menu cascade.
    /*
        Process a mouse down event outside of the menu cascade. The menu bar checks if the mouse down happened on one of its menu bar item.
        If yes, it let the menu bar item process the event. if no, it will initiate the closure of the menu cascade.
        
        \param menu menu item
        \param x    coordinate of the mouse down event
        \param y    coordinate of the mouse down event
    */
    void RecvSigMouseDownOutsideMenuCascade(smptr(MenuPage) menu, int x, int y);

protected:


private:
    std::list< smptr(MenuBarItem) > m_MenuBarItemList;

    smptr(HLayout) m_hlayout;
    bool m_MenuIsActive;
    weaksmptr(MenuBarItem) m_CurrentMenu;
    bool m_IsOpeningMenu;
    smptr(BaseWindow) m_MenuBarWindow;
};

} //NUX_NAMESPACE_END

#endif // MENUBAR_H
