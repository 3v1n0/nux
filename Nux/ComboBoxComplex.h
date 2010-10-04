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


#ifndef COMBOBOX_COMPLEX_H
#define COMBOBOX_COMPLEX_H

#include "AbstractComboBox.h"

namespace nux { //NUX_NAMESPACE_BEGIN

class Menu;
class ActionItem;
class PopUpWindow;
class ListControl;
class TableCtrl;
class TableItem;
class HLayout;

class ComboBoxComplex : public AbstractComboBox
{
public:
    ComboBoxComplex(NUX_FILE_LINE_PROTO);
    ~ComboBoxComplex();
    virtual long ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo);

//    // make the class abstract
//    virtual void Draw(GraphicsContext& GfxContext, bool force_draw) = 0;
//    virtual void DrawContent(GraphicsContext& GfxContext, bool force_draw) = 0;
//    virtual void PostDraw(GraphicsContext& GfxContext, bool force_draw) = 0;

    void SetPopupWindowSize(unsigned int width, unsigned int height);
    void SetPopupWidth(unsigned int width);

protected:
    virtual long PostLayoutManagement(long LayoutResult);

public:
    void AddItem(TableItem* item);
    void RemoveItem(TableItem* item);
    void RemoveAllItem();

    // emitters
    void OnMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void OnPopupStop();

    // signals
    void RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags);
    void RecvMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags);
    void RecvSigActionTriggered(TableCtrl& table, TableItem& item, unsigned int row, unsigned int column);
    void RecvSigTerminateMenuCascade();
    
    sigc::signal<void, TableItem&> sigSelectedItem;

    const TableItem* GetSelection() const {return m_SelectedTreeNode;}

protected:
    void InitializeWidgets();
    void InitializeLayout();
    void DestroyLayout();

protected:
//    CoreArea m_ComboArea;
//    CoreArea m_Button;
//    bool m_MenuIsActive;
    const TableItem* m_SelectedTreeNode;
//     bool m_IsOpeningMenu;

    PopUpWindow*    m_PopupWindow;
    ListControl*    m_ListBox;
    HLayout*        m_Layout;
};

} //NUX_NAMESPACE_END

#endif // COMBOBOX_COMPLEX_H
