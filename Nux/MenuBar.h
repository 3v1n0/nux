/*
 * Copyright 2010 Inalogic® Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License, as
 * published by the  Free Software Foundation; either version 2.1 or 3.0
 * of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License along with this program. If not, see <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef MENUBAR_H
#define MENUBAR_H

#include "ScrollView.h"
#include "ToolButton.h"
#include "MenuPage.h"
#include "Painter.h"

namespace nux
{

  class PopupBox;
  class BaseWindow;
  class MenuBar;
  class HLayout;


  class MenuBarItem: public Object
  {
  public:
    NUX_DECLARE_OBJECT_TYPE(MenuBarItem, Object);

    MenuBarItem(NUX_FILE_LINE_PROTO);
    ~MenuBarItem();
  private:
    BasicView   *area;
    MenuPage   *menu;
    BaseTexture   *icon; // should be 24x24

    friend class MenuBar;
  };

  class MenuBar: public View
  {
    NUX_DECLARE_OBJECT_TYPE(MenuBar, View);

  public:
    MenuBar(NUX_FILE_LINE_PROTO);
    ~MenuBar();

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

    //void AddActionItem(ActionItem* actionItem);

    void AddMenu(const char *MenuLabel, MenuPage *popup);
    void AddMenu(const char *MenuLabel, MenuPage *menu, BaseTexture *icon);


    /////////////////
    //  EMITERS    //
    /////////////////

  private:
    void EmitItemMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags, MenuBarItem *menubar_item);
    void EmitItemMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags, MenuBarItem *menubar_item);
    void EmitItemMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags, MenuBarItem *menubar_item);
    void EmitItemMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags, MenuBarItem *menubar_item);
    void RecvItemMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags, MenuBarItem *menubar_item);
    void RecvSigActionTriggered(MenuPage *, ActionItem *);
    void RecvSigTerminateMenuCascade();

    //! Process a mouse down event outside of the menu cascade.
    /*
        Process a mouse down event outside of the menu cascade. The menu bar checks if the mouse down happened on one of its menu bar item.
        If yes, it let the menu bar item process the event. if no, it will initiate the closure of the menu cascade.

        \param menu menu item
        \param x    coordinate of the mouse down event
        \param y    coordinate of the mouse down event
    */
    void RecvSigMouseDownOutsideMenuCascade(MenuPage *menu, int x, int y);

  protected:


  private:
    std::list< MenuBarItem * > m_MenuBarItemList;

    HLayout *m_hlayout;
    bool m_MenuIsActive;
    MenuBarItem *m_CurrentMenu;
    bool m_IsOpeningMenu;
    BaseWindow *m_MenuBarWindow;
  };

}

#endif // MENUBAR_H
