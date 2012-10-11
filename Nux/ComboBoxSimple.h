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


#ifndef COMBOBOX_LOGIC_H
#define COMBOBOX_LOGIC_H

#include "AbstractComboBox.h"
#include "ActionItem.h"

namespace nux
{

  class MenuPage;
  class ActionItem;
  class TableItem;
  class TableCtrl;

  class ComboBoxSimple : public AbstractComboBox
  {
  public:
    ComboBoxSimple(NUX_FILE_LINE_PROTO);
    ~ComboBoxSimple();

    // make the class abstract
//    virtual void Draw(GraphicsEngine& graphics_engine, bool force_draw);
//    virtual void DrawContent(GraphicsEngine& graphics_engine, bool force_draw);

  public:
    ActionItem *AddItem(const char *label, int Uservalue = 0);
    void RemoveItem(ActionItem *item);
    void RemoveAllItem();

    // emitters
    void OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnPopupStop();

    // signals
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvSigActionTriggered(MenuPage *, ActionItem *);
    void RecvSigActionTriggered2(TableCtrl *table, TableItem *item, unsigned int row, unsigned int column);
    void RecvSigTerminateMenuCascade();
    void RecvGeometryChanged(Area *area, Geometry &geo);

    const char *GetSelectionLabel() const;
    int GetSelectionUserValue() const;
    int GetNumItem() const;
    ActionItem *GetItem(int index) const;
    int GetSelectionIndex() const;
    void SetSelectionIndex(int index);

    // moves the currently selected item up/down - just shorthand for SetSelectionIndex
    void MoveSelectionUp();
    void MoveSelectionDown();

    MenuPage * GetMenuPage()
    {
      return m_CurrentMenu;
    }

    sigc::signal<void, ComboBoxSimple *> sigTriggered;
    sigc::signal<void, ActionItem *> sigActionTriggered;

  protected:

    virtual Area* FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type);

    void RecvClosingMenuSignal(MenuPage* menu_page);

    MenuPage   *m_CurrentMenu;
    ActionItem *m_SelectedAction;
  };

}

#endif // COMBOBOX_LOGIC_H
