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

#ifndef MENUPAGE_H
#define MENUPAGE_H

#include "ActionItem.h"
#include "FloatingWindow.h"

namespace nux
{
  class StaticText;
  class MenuPage;
  class VLayout;
  class MenuBar;

  class MenuItem: public View
  {
    NUX_DECLARE_OBJECT_TYPE(MenuItem, View);
  public:
    MenuItem(const char *label, int UserValue, NUX_FILE_LINE_PROTO);
    ~MenuItem();

    void DrawAsMenuItem(GraphicsEngine &graphics_engine, const Color &textcolor, bool is_highlighted, bool isFirstItem, bool isLastItem, bool draw_icone);

    //const ActionItem& GetItem() const {return m_ActionItem;}
    ActionItem *GetActionItem() const;
    //ActionItem* GetActionItem();

    int GetTextWidth();
    int GetTextHeight();

    StaticText * GetStaticText()
    {
      return _pango_static_text;
    }

  private:
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine & /* graphics_engine */, bool /* force_draw */) {};

    void SetChildMenu(MenuPage *menu);
    MenuPage *GetChildMenu() const;
    void SetActionItem(ActionItem *menu);

    MenuPage    *_child_menu;
    ActionItem  *_action_item;
    StaticText  *_pango_static_text;
    friend class MenuPage;
  };

  class MenuSeparator: public View
  {
    NUX_DECLARE_OBJECT_TYPE(MenuSeparator, View);
  public:
    MenuSeparator(NUX_FILE_LINE_PROTO);
    ~MenuSeparator();

    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine & /* graphics_engine */, bool /* force_draw */) {};

  private:
    friend class MenuPage;
  };

  class MenuPage: public View
  {
    NUX_DECLARE_OBJECT_TYPE(MenuPage, View);
  public:
    MenuPage(const char *title = "", NUX_FILE_LINE_PROTO);
    ~MenuPage();

//    void SetName(const char* name);
    const char *GetName() const;

    ActionItem *AddAction(const char *label = 0, int UserValue = 0);
    //void AddActionItem(ActionItem* actionItem);
    void AddSeparator();
    MenuPage *AddMenu(const char *label);
    ActionItem *AddSubMenu(const char *label, MenuPage *menu);

    void RemoveItem(ActionItem *item);
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
        When this function is called, the menu becomes visible and appear at position(MenuXPosition, MenuYPosition).
        The menu also assumes it has received a mouse down event at coordinates(x, y).
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

    void SetFontName(char *font_name);

        //! Let regular widgets process a mouse down event that closes the menu chain without a menu item selection.
    /*!
        When a menu chain closes as a result of a mouse down event outside of the menu chain,
        there is still the possibility to let regular widgets process the event.
        The flag is tested on the top level MenuPage of the menu chain.

        @param propagate_when_closing_without_action Set to True to propagate the event to regular widgets
        if the menu chain closes as a result of a mouse down event outside the menu chain.
    */
    void SetOnClosureContinueEventCycle(bool on_closure_continue_with_event);

    //! Returns True if a mouse down that closes the menu chain can be processed by regular widgets.
    /*!
        When a menu chain closes as a result of a mouse down event outside of the menu chain,
        there is still the possibility to let regular widgets process the event.
        The flag is tested on the top level MenuPage of the menu chain.

        @return True is a mouse down event that closes a menu chain without an item selection,
        can be passed down the event cycle.
    */
    bool OnClosureContinueEventCycle() const;

  public:
    void StopActionSubMenu();
    void ExecuteActionItem(MenuItem *menuItem);

    void NotifyActionTriggeredToParent(MenuPage *, MenuItem *menuItem);
    void NotifyTerminateMenuCascade();
    void NotifyMouseDownOutsideMenuCascade(int x, int y);

    void SetParentMenu(MenuPage *);
    MenuPage *GetParentMenu();

    void setShowItemIcon(bool b)
    {
      m_show_item_icon = b;
    }
    bool ShowItemIcon()
    {
      return m_show_item_icon;
    }
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
    sigc::signal<void, MenuPage *, ActionItem * > sigActionTriggered;

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
    sigc::signal<void, MenuPage *, int, int> sigMouseDownOutsideMenuCascade;

    sigc::signal<void, MenuPage *> sigOpeningMenu;
    sigc::signal<void, MenuPage *> sigClosingMenu;

    void SetActive(bool b)
    {
      m_IsActive = b;

      if (b)
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
    int GetNumItem() const
    {
      return m_numItem;
    }
    ActionItem *GetActionItem(int i) const;

    /*!
        Get the index of and item in the menu.
        @return the index of the ActionItem in the menu. -1 if the Action Item is not found.
    */
    int GetActionItemIndex(ActionItem *action) const;

    //! Return the position of this object with regard to its top left corner of the physical window.
    /*!
        Return the position of the Area inside the physical window.
        For the main layout set in WindowThread, The following functions are equivalent:
        \li GetGeometry()
        \li GetRootGeometry()
        \li GetAbsoluteGeometry()
    */
    virtual Geometry GetAbsoluteGeometry() const;

    //! Return the position of this object with regard to its top level parent(the main layout or a BaseWindow).
    /*!
        Return the position of the Area inside the physical window.
        For the main layout set in WindowThread or for a BaseWindow, GetRootGeometry() is equivalent to GetGeometry().
    */
    virtual Geometry GetRootGeometry() const;

  protected:
    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);
    virtual void Draw(GraphicsEngine &graphics_engine, bool force_draw);
    virtual void DrawContent(GraphicsEngine &graphics_engine, bool force_draw);

  private:

    bool on_closure_continue_with_event_;
    int m_numItem;
    int m_HighlightedItem;
    bool m_IsActive;
    VLayout* _vlayout;
    bool m_NextMouseUpMeanStop;
    MenuItem *m_SubMenuAction;
    std::string m_Name;

    bool m_Action_Triggered;
    MenuPage *m_Parent;

    // Set to TRUE if one of the MenuItem processed the mouse event.
    bool m_MouseEventProcessed;

    int m_item_width;
    int m_item_height;
    bool m_show_item_icon;
    std::vector<MenuItem *> m_MenuItemVector;
    std::vector< MenuSeparator * > m_MenuSeparatorVector;
    BaseWindow *m_MenuWindow;

    //! This parameter is True if this MenuPage is at the top of a menu chain attached to a MenuBar.
    bool m_IsTopOfMenuChain;

    char *_font_name;

  public:
    ///////////////////////////////////////////////////////
    // AbstractInterfaceObject
    ///////////////////////////////////////////////////////

    virtual long ComputeContentSize();
    virtual void SetGeometry(const Geometry &geo);

    friend class MenuBar;
    friend class WindowCompositor;
  };

}

#endif // MENUPAGE_H
