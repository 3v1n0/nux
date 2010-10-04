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


#ifndef COMBOBOX_LOGIC_H
#define COMBOBOX_LOGIC_H

#include "AbstractComboBox.h"
#include "ActionItem.h"

namespace nux   //NUX_NAMESPACE_BEGIN
{

  class MenuPage;
  class ActionItem;
  class TableItem;
  class TableCtrl;

  class ComboBoxSimple : public AbstractComboBox
  {
  public:
    ComboBoxSimple (NUX_FILE_LINE_PROTO);
    ~ComboBoxSimple();
    virtual long ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

    // make the class abstract
//    virtual void Draw(GraphicsContext& GfxContext, bool force_draw);
//    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw);
//    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw);

  public:
    ActionItem *AddItem (const TCHAR *label, int Uservalue = 0);
    void RemoveItem (ActionItem *item);
    void RemoveAllItem();

    // emitters
    void OnMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnPopupStop();

    // signals
    void RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvSigActionTriggered (MenuPage *, ActionItem *);
    void RecvSigActionTriggered2 (TableCtrl *table, TableItem *item, unsigned int row, unsigned int column);
    void RecvSigTerminateMenuCascade();

    const TCHAR *GetSelectionLabel() const;
    int GetSelectionUserValue() const;
    int GetNumItem() const;
    ActionItem *GetItem (int index) const;
    int GetSelectionIndex() const;
    void SetSelectionIndex (int index);

    sigc::signal<void, ComboBoxSimple *> sigTriggered;
    sigc::signal<void, ActionItem *> sigActionTriggered;

  protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

  protected:
    MenuPage   *m_CurrentMenu;
    ActionItem *m_SelectedAction;

  };

} //NUX_NAMESPACE_END

#endif // COMBOBOX_LOGIC_H
