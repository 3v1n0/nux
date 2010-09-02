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


#ifndef MENUPAGE_H
#define MENUPAGE_H

#include "ActionItem.h"
#include "FloatingWindow.h"

NAMESPACE_BEGIN_GUI

class MenuPage;
class VLayout;
class MenuBar;

class MenuItem: public ActiveInterfaceObject
{
    DECLARE_OBJECT_TYPE(MenuItem, ActiveInterfaceObject);
public:
    MenuItem(const TCHAR* label, int UserValue);
    ~MenuItem();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw) {};
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw) {};

    void DrawAsMenuItem(GraphicsContext& GfxContext, const Color& textcolor, bool is_highlighted, bool isFirstItem, bool isLastItem, bool draw_icone);

    //const ActionItem& GetItem() const {return m_ActionItem;}
    const smptr(ActionItem) GetActionItem() const;
    //smptr(ActionItem) GetActionItem();

private:
    void SetChildMenu(smptr(MenuPage) menu);
    smptr(MenuPage) GetChildMenu() const;
    void SetActionItem(smptr(ActionItem) menu);

    smptr(MenuPage) m_ChildMenu;
    smptr(ActionItem) m_ActionItem;
    friend class MenuPage;
};

class MenuSeparator: public ActiveInterfaceObject
{
    DECLARE_OBJECT_TYPE(MenuSeparator, ActiveInterfaceObject);
public:
    MenuSeparator();
    ~MenuSeparator();

    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);
    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw) {};
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw) {};

private:
    friend class MenuPage;
};

class MenuPage: public ActiveInterfaceObject
{
    DECLARE_OBJECT_TYPE(MenuPage, ActiveInterfaceObject);
public:
    MenuPage(const TCHAR* title = TEXT(""));
    ~MenuPage();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) ;


    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

//    void SetName(const TCHAR* name);
    const TCHAR* GetName() const;

    smptr(ActionItem) AddAction(const TCHAR* label = 0, int UserValue = 0);
    //void AddActionItem(smptr(ActionItem) actionItem);
    void AddSeparator();
    smptr(MenuPage) AddMenu(const TCHAR* label);
    smptr(ActionItem) AddSubMenu(const TCHAR* label, smptr(MenuPage) menu);

    void RemoveItem(smptr(ActionItem) item);
    void RemoveAllItem();

    bool CanClose() const;
    // emitters
    void EmitMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void EmitMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void EmitMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void EmitMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags);

//private:
    //! Start the MenuPage iteration and show it.
    /*
        Start the MenuPage iteration and show it.
        When this function is called, the menu becomes visible and appear at position (MenuXPosition, MenuYPosition).
        The menu also assumes it has received a mouse down event at coordinates (x, y).
        \param MenuXPosition: the position of the menu.
        \param MenuYPosition: the position of the menu.
        \param x: the simulate position where the mouse down happened on the menu area.
        \param y: the simulate position where the mouse down happened on the menu area.
        \param TakeMousefocus: if true, the MenuPage area will take the mouse focus.
    */
    void StartMenu(int MenuXPosition, int MenuYPosition, int x = 0, int y = 0, bool OverrideCurrentMenuChain = true);
    
    //! Stop the MenuPage iteration and hide it.
    /*
        Stop the MenuPage iteration and hide it.
        \param x: the simulate position where the mouse down happened on the menu area.
        \param y: the simulate position where the mouse down happened on the menu area.

    */
    void StopMenu(int x = 0, int y = 0);

public:
    void StopActionSubMenu();
    void ExecuteActionItem(smptr(MenuItem) menuItem);

    void NotifyActionTriggeredToParent(smptr(MenuPage), smptr(MenuItem) menuItem);
    void NotifyTerminateMenuCascade();
    void NotifyMouseDownOutsideMenuCascade(int x, int y);

    void SetParentMenu(smptr(MenuPage));
    smptr(MenuPage) GetParentMenu();

    void setShowItemIcon(bool b) { m_show_item_icon = b; }
    bool ShowItemIcon() { return m_show_item_icon; }
    bool TestMouseDown();
    bool TestMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags, bool &hit_inside_a_menu);

    // Never call this function directly
    void Terminate(int x, int y, unsigned long button_flags, unsigned long key_flags);

    // signals
    sigc::signal<void, int> sigItemSelected;
    //! Send an action Signal.
    /*
        The MenuPage object sends this signal when an action is triggered.
        \param MenuPage the menu object sending the signal.
        \param ActionItem the action object that was triggered in the menu.
    */
    sigc::signal<void, smptr(MenuPage), const smptr(ActionItem) > sigActionTriggered;

    //! Terminate the menu and its sub menu cascade.
    /*
        The MenuPage object send this signal to inform that it needs to be close.
        The receiving object must close the MenuPage by calling the member function StopMenu().
        Any object that controls a menu should intercept sigTerminateMenuCascade and sigMouseDownOutsideMenuCascade.
    */
    sigc::signal<void> sigTerminateMenuCascade;

    //! Notify that a mouse down event happened outside the menu cascade.
    /*
        Notify that a mouse down event happened outside the menu cascade. This event is processed by the MenuPage bar. The menu bar
        checks if the mouse down happened on one of its menu bar item. If yes, it let the menu bar item process the event.
        if no, it will initiate the closure of the menu cascade.
        Any object that controls a menu should intercept sigTerminateMenuCascade and sigMouseDownOutsideMenuCascade.
    */
    sigc::signal<void, smptr(MenuPage), int, int> sigMouseDownOutsideMenuCascade;

    void SetActive(bool b)
    {
        m_IsActive = b;
        if(b)
            /*m_PopupArea.*/CaptureMouseDownAnyWhereElse(true);
        else
            /*m_PopupArea.*/CaptureMouseDownAnyWhereElse(false);
    }

    bool IsActive() const
    {
        return m_IsActive;
    }

    /*!
        Return the number of items in the menu.
    */
    int GetNumItem() const {return m_numItem;}
    smptr(ActionItem) GetActionItem(int i) const;

    /*!
        Get the index of and item in the menu.
        @return the index of the ActionItem in the menu. -1 if the Action Item is not found.
    */
    int GetActionItemIndex(const smptr(ActionItem) action) const;
private:
    int m_numItem;
    int m_HighlightedItem;
    bool m_IsActive;
    smptr(VLayout) vlayout;
    bool m_NextMouseUpMeanStop;
    smptr(MenuItem) m_SubMenuAction;
    NString m_Name;

    bool m_Action_Triggered;
    smptr(MenuPage) m_Parent;
    
    // Set to TRUE if one of the MenuItem processed the mouse event.
    UBOOL m_MouseEventProcessed;

    int m_item_width;
    int m_item_height;
    bool m_show_item_icon;
    std::vector< smptr(MenuItem) > m_MenuItemVector;
    std::vector< smptr(MenuSeparator) > m_MenuSeparatorVector;
    smptr(BaseWindow) m_MenuWindow;

    //! This parameter is True if this MenuPage is at the top of a menu chain attached to a MenuBar.
    bool m_IsTopOfMenuChain;

public:
    ///////////////////////////////////////////////////////
    // AbstractInterfaceObject
    ///////////////////////////////////////////////////////

    virtual long ComputeChildLayout();
    virtual void setGeometry(const Geometry& geo);

    friend class MenuBar;
};

NAMESPACE_END_GUI

#endif // MENUPAGE_H