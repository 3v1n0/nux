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


#include "Nux.h"
#include "ComboBoxSimple.h"
#include "MenuPage.h"
#include "ActionItem.h"
#include "TableItem.h"
#include "TableCtrl.h"

NAMESPACE_BEGIN_GUI

ComboBoxSimple::ComboBoxSimple()
:   m_SelectedAction(0)
,   m_CurrentMenu(0)
{
    InitializeLayout();
    InitializeWidgets();
}

ComboBoxSimple::~ComboBoxSimple()
{
    DestroyLayout();
}

void ComboBoxSimple::InitializeWidgets()
{
    // Set Signals
    m_Button->OnMouseDown.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvMouseDown));
    m_Button->OnMouseUp.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvMouseUp));
    m_ComboArea->OnMouseDown.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvMouseDown));
    m_ComboArea->OnMouseUp.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvMouseUp));

    //m_Popup.sigPopupStop.connect(sigc::mem_fun(this, &ComboBox::OnPopupStop));

    // Set Geometry
    m_Button->setGeometry(Geometry(0, 0, 20, DEFAULT_WIDGET_HEIGHT));
    //m_Button->SetMaximumSize(20, DEFAULT_WIDGET_HEIGHT);
    m_Button->SetMinimumSize(20, DEFAULT_WIDGET_HEIGHT);
    m_Button->SetSizePolicy(eSizeFixed);

    m_ComboArea->SetMinimumSize(2*DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    m_ComboArea->setGeometry(Geometry(0, 0, 3*DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT));

    //m_CurrentMenu = new MenuPage;
    m_CurrentMenu->SetParentMenu(smptr(MenuPage)(0));
    m_CurrentMenu->sigActionTriggered.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvSigActionTriggered));
    m_CurrentMenu->sigTerminateMenuCascade.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvSigTerminateMenuCascade));
}

void ComboBoxSimple::InitializeLayout()
{
    m_CurrentMenu = smptr(MenuPage)(new MenuPage());
}

void ComboBoxSimple::DestroyLayout()
{
}

long ComboBoxSimple::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;

    //ret = m_Popup.ProcessEvent(ievent, ret, ProcessEventInfo); // implement isVisible on BaseArea. If invisible, no event processed.
    ret = m_Button->OnEvent(ievent, ret, ProcessEventInfo);
    ret = m_ComboArea->OnEvent(ievent, ret, ProcessEventInfo);

    if(ievent.e_event == NUX_MOUSE_PRESSED)
    {
        bool mouse_down_on_menu_item = false;
        if(m_MenuIsActive == true)
        {
            if(m_Button->IsMouseInside() || m_ComboArea->IsMouseInside())
                mouse_down_on_menu_item = true;
            
            if(mouse_down_on_menu_item == false)
            {
                if(m_CurrentMenu->TestMouseDown() == false)
                {
                    RecvSigTerminateMenuCascade();
                    m_MenuIsActive = false;
                }
            }
        }
    }

    ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
    return ret;
}

smptr(ActionItem) ComboBoxSimple::AddItem(const TCHAR* label, int Uservalue)
{
    if(m_CurrentMenu->GetNumItem() == 0)
    {
        // The first element added is the element featured on the combo box when it is closed.
        m_SelectedAction = m_CurrentMenu->AddAction(label, Uservalue);
        m_ComboArea->SetBaseString(m_SelectedAction->GetLabel());
        return m_SelectedAction;
    }
    else
    {
        return m_CurrentMenu->AddAction(label, Uservalue);
    }
}

void ComboBoxSimple::RemoveItem(smptr(ActionItem) item)
{

}

void ComboBoxSimple::RemoveAllItem()
{
    m_CurrentMenu->RemoveAllItem();
}

void ComboBoxSimple::RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    if(m_MenuIsActive == false)
    {
        // Open the MenuPage
        m_MenuIsActive = true;
        m_IsOpeningMenu = true;

        Geometry geo = m_CurrentMenu->GetGeometry();
        geo.SetX(m_ComboArea->GetBaseX());
        geo.SetY(m_ComboArea->GetBaseY() + m_ComboArea->GetBaseHeight());
        geo.SetWidth(m_ComboArea->GetBaseWidth());
        m_CurrentMenu->setGeometry(geo);
        m_CurrentMenu->StartMenu(m_ComboArea->GetBaseX(), 
            m_ComboArea->GetBaseY() + m_ComboArea->GetBaseHeight(),
            0, 0);
    }
    else
    {
        // If the mouse up that follows happen inside the area, then it is going to close the menu.
        m_IsOpeningMenu = false;
    }

    NeedRedraw();
}

void ComboBoxSimple::RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    if(m_MenuIsActive)
    {
        if(m_ComboArea->IsMouseInside() || m_Button->IsMouseInside())
        {
            if(m_IsOpeningMenu == false)
            {
                // close the MenuPage that is Open
                m_CurrentMenu->StopMenu(0, 0);
                m_MenuIsActive = false;
            }
            else
            {
                // The MousePress before this MouseRelease, caused the MenuPage to open.
                // Set m_IsOpeningMenu so the next mouse release will close the menu.
                m_IsOpeningMenu = false;
                m_MenuIsActive = true;
            }
        }
        else
        {
            bool hit_inside_a_menu = false;
            bool b = m_CurrentMenu->TestMouseUp(x, y, button_flags, key_flags, hit_inside_a_menu);
            if(b || (hit_inside_a_menu == false))
            {
                RecvSigTerminateMenuCascade();
                m_MenuIsActive = false;
            }
        }
    }
    NeedRedraw();
}

void ComboBoxSimple::RecvSigActionTriggered(smptr(MenuPage) menu, const smptr(ActionItem) action)
{
    m_MenuIsActive = false;
    m_CurrentMenu->StopMenu();

    m_SelectedAction = smptr(ActionItem)(action);
    m_ComboArea->SetBaseString(m_SelectedAction->GetLabel());
    m_IsOpeningMenu = false;
    
    sigTriggered.emit(smptr(ComboBoxSimple)(this, false));
    sigActionTriggered.emit(*m_SelectedAction);

    NeedRedraw();
    // You can do something if you want with the menu* and the action*
}

void ComboBoxSimple::RecvSigActionTriggered2(smptr(TableCtrl) table, TableItem* item, unsigned int row, unsigned int column)
{
    m_MenuIsActive = false;
    m_CurrentMenu->StopMenu();
    m_IsOpeningMenu = false;
    NeedRedraw();
    // You can do something if you want with the menu* and the action*
}

void ComboBoxSimple::RecvSigTerminateMenuCascade()
{
    //m_MenuIsActive = false;
    m_CurrentMenu->StopMenu();
    m_IsOpeningMenu = false;
}


const TCHAR* ComboBoxSimple::GetSelectionLabel() const
{
    if(m_SelectedAction.IsValid())
        return m_SelectedAction->GetLabel();
    return 0;
}

int ComboBoxSimple::GetSelectionUserValue() const
{
    if(m_SelectedAction.IsValid())
        return m_SelectedAction->GetUserValue();
    return 0;
}

int ComboBoxSimple::GetNumItem() const
{
    return m_CurrentMenu->GetNumItem();
}

const smptr(ActionItem) ComboBoxSimple::GetItem(int index) const
{
    return m_CurrentMenu->GetActionItem(index);
}

int ComboBoxSimple::GetSelectionIndex() const
{
    if(m_SelectedAction.IsValid())
        return m_CurrentMenu->GetActionItemIndex(m_SelectedAction);
    return -1;
}

void ComboBoxSimple::SetSelectionIndex(int index)
{
    if((index >= 0) && (index < m_CurrentMenu->GetNumItem()))
    {
        m_SelectedAction = m_CurrentMenu->GetActionItem(index);
        m_ComboArea->SetBaseString(m_SelectedAction->GetLabel());
        NeedRedraw();
    }
    else if(m_CurrentMenu->GetNumItem() > 0)
    {
        // index is negative
        m_SelectedAction = m_CurrentMenu->GetActionItem(0);
        m_ComboArea->SetBaseString(m_SelectedAction->GetLabel());
        NeedRedraw();
    }
    else
    {
        m_SelectedAction = smptr(ActionItem)(0);
    }
}


NAMESPACE_END_GUI
