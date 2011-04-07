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


#include "Nux.h"

#include "ComboBoxComplex.h"
#include "PopUpWindow.h"
#include "MenuPage.h"
#include "TableItem.h"
#include "ListControl.h"
#include "ActionItem.h"
#include "PropertyList.h"
#include "WindowCompositor.h"

namespace nux
{

  ComboBoxComplex::ComboBoxComplex (NUX_FILE_LINE_DECL)
    :   AbstractComboBox (NUX_FILE_LINE_PARAM)
  {
    m_ListBox       = 0;
    m_PopupWindow   = 0;
    m_Layout        = 0;

    InitializeLayout();
    InitializeWidgets();
  }

  ComboBoxComplex::~ComboBoxComplex()
  {
    DestroyLayout();
  }

  void ComboBoxComplex::InitializeWidgets()
  {
    // Set Signals
    _combo_box_opening_area->OnMouseDown.connect (sigc::mem_fun (this, &ComboBoxComplex::RecvMouseDown) );
    _combo_box_opening_area->OnMouseUp.connect (sigc::mem_fun (this, &ComboBoxComplex::RecvMouseUp) );
    _combo_box_opening_area->OnMouseDrag.connect (sigc::mem_fun (this, &ComboBoxComplex::RecvMouseDrag) );

    _combo_box_area->OnMouseDown.connect (sigc::mem_fun (this, &ComboBoxComplex::RecvMouseDown) );
    _combo_box_area->OnMouseUp.connect (sigc::mem_fun (this, &ComboBoxComplex::RecvMouseUp) );
    _combo_box_area->OnMouseDrag.connect (sigc::mem_fun (this, &ComboBoxComplex::RecvMouseDrag) );

    // Set Geometry
    _combo_box_opening_area->SetGeometry (Geometry (0, 0, 20, DEFAULT_WIDGET_HEIGHT) );
    _combo_box_opening_area->SetMinimumSize (20, DEFAULT_WIDGET_HEIGHT);

    _combo_box_area->SetMinimumSize (2 * DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    _combo_box_area->SetGeometry (Geometry (0, 0, 3 * DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT) );

    m_ListBox->UsedForComboBox (true);

    // Remove horizontal scrollbar
    m_ListBox->EnableHorizontalScrollBar (false);
    // Force the list size to respect the size of its layout.
    m_ListBox->SetSizeMatchContent (true);
    m_ListBox->SetViewContentLeftMargin (0);
    m_ListBox->SetViewContentRightMargin (0);
    m_ListBox->SetViewContentTopMargin (0);
    m_ListBox->SetViewContentBottomMargin (0);
    m_ListBox->SetMouseOverColor (Color (0xff868686) );
    m_ListBox->SetRowColorEven (Color (0xff999999) );
    m_ListBox->SetRowColorOdd (Color (0xff999999) );
    m_ListBox->EnableMouseOverColor (true);
    m_ListBox->ShowHorizontalSeparationLine (false);
    m_ListBox->SetMaximumHeight (200);

    m_ListBox->sigTableItemSelected.connect (sigc::mem_fun (this, &ComboBoxComplex::RecvSigActionTriggered) );
    m_ListBox->SetGeometry (Geometry (0, 0, 100, 400) );
    m_Layout->AddView (m_ListBox, 1, eAbove, eFull);
    // Setting the layout stretch factor to 0 forces the list TableCtrl to have always use its minimum width.
    // This is how the PopupWindow width is controlled. SetPopupWidth define the minimum width of the TableCtrl.
    // Because m_Layout(a HLayout) has a stretch factor of 0, it will force the m_ListBox to its minimum width.
    m_Layout->SetStretchFactor (0);
    m_PopupWindow->SetLayout (m_Layout);

    // Call PopupWindow::SetWindowSizeMatchLayout with TRUE as argument and the popupWindow will preserve the size of its layout.
    m_PopupWindow->SetWindowSizeMatchLayout (true);
  }

  void ComboBoxComplex::InitializeLayout()
  {
    m_Layout        = new HLayout(NUX_TRACKER_LOCATION);
    m_ListBox       = new ListControl(false);
    m_PopupWindow   = new PopUpWindow();
  }

  void ComboBoxComplex::DestroyLayout()
  {
    m_PopupWindow->Dispose ();
  }

  long ComboBoxComplex::ProcessEvent (IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
  {
    long ret = TraverseInfo;

    ret = _combo_box_opening_area->OnEvent (ievent, ret, ProcessEventInfo);
    ret = _combo_box_area->OnEvent (ievent, ret, ProcessEventInfo);

    if (ievent.e_event == NUX_MOUSE_RELEASED)
    {
      // Cancel m_IsOpeningMenu in case it hasn't been changed in _combo_box_opening_area->OnEvent or _combo_box_area->OnEvent.
      // This can happen if the table area of the m_ListBox get the NUX_MOUSE_RELEASED event first and sets
      // TraverseInfo to MouseEventSolved.
      if (m_IsOpeningMenu == true)
        m_IsOpeningMenu = false;
    }

    if (ievent.e_event == NUX_MOUSE_PRESSED)
    {
      // The mouse down can also happen the first time to initiate the opening of the menu. That is why we need
      // the flag m_IsOpeningMenu;
      bool mouse_down_on_menu_item = false;

      if (m_MenuIsActive == true)
      {
        // The menu is opened.
        // Did we hit the button (the one with the arrow pointing down) or did we hit the rest
        // of the Combo area?
        if (_combo_box_opening_area->IsMouseInside() || _combo_box_area->IsMouseInside() )
          mouse_down_on_menu_item = true;

        if (mouse_down_on_menu_item == false)
        {
          // We did not hit the permanently visible area of the combo widget.
          // Did we hit the area of the popup menu?
          Geometry window_geometry = m_PopupWindow->GetGeometry();

          if (!window_geometry.IsPointInside (ievent.GetX(), ievent.GetY() ) && (m_IsOpeningMenu == false) )
          {
            m_PopupWindow->Hide();
            m_MenuIsActive = false;
          }

          //m_MenuIsActive = false;
        }
        else
        {
          if (m_IsOpeningMenu == false)
          {
            m_PopupWindow->Hide();
            m_MenuIsActive = false;
          }
        }
      }
    }

    ret = PostProcessEvent2 (ievent, ret, ProcessEventInfo);
    return ret;
  }

  void ComboBoxComplex::AddItem (TableItem *item)
  {
    if (item == 0)
      return;

    if (m_ListBox && (m_ListBox->GetNumRootChild() == 0) )
    {
      // The first element added is the element featured on the combobox when it is closed.
      m_ListBox->setTableItems (item);
      m_SelectedTreeNode = item;
      _combo_box_area->SetBaseString (m_SelectedTreeNode->GetName() );
    }
    else if (m_ListBox)
    {
      m_ListBox->setTableItems (item);
    }
  }

  void ComboBoxComplex::RemoveItem (TableItem *item)
  {

  }

  void ComboBoxComplex::RemoveAllItem()
  {

  }

  void ComboBoxComplex::RecvMouseDown (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_MenuIsActive == false)
    {
      // Open the Menu
      m_MenuIsActive = true;
      m_IsOpeningMenu = true;

      m_PopupWindow->SetBaseXY (_combo_box_area->GetBaseX(), _combo_box_area->GetBaseY() + _combo_box_area->GetBaseHeight() );
      m_PopupWindow->Show();

      if (m_ListBox)
      {
        m_ListBox->ResetItems();
      }

    }
    else
    {
      // If the mouse up that follows happen inside the area, then it is going to close the menu.
      m_IsOpeningMenu = false;
    }

    NeedRedraw();
  }

  void ComboBoxComplex::RecvMouseUp (int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_MenuIsActive)
    {
      if (_combo_box_area->IsMouseInside() || _combo_box_opening_area->IsMouseInside() )
      {
        if (m_IsOpeningMenu == false)
        {
          // close the Menu that is Open
          //m_CurrentMenu->StopMenu(0, 0);
          m_MenuIsActive = false;
        }
        else
        {
          // The MousePress before this MouseRelease, caused the Menu to open.
          // Set m_IsOpeningMenu so the next mouse release will close the menu.
          m_IsOpeningMenu = false;
          m_MenuIsActive = true;
        }
      }
      else
      {
        int row, column;
        TableItem *item = 0;
        m_ListBox->GetBaseX();
        m_ListBox->GetBaseY();
        int X = x + _combo_box_area->GetBaseX() - m_PopupWindow->GetBaseX() - m_ListBox->GetBaseX();
        int Y = y + _combo_box_area->GetBaseY() - m_PopupWindow->GetBaseY() - m_ListBox->GetBaseY();
        m_ListBox->FindItemUnderPointer (X, Y, &item, row, column);

        if (item)
        {
          RecvSigActionTriggered (*m_ListBox, *item, row, column);
        }
        else
        {
          m_MenuIsActive = false;
          m_PopupWindow->Hide();
        }

//            bool hit_inside_a_menu = false;
//            bool b = m_CurrentMenu->TestMouseUp(x, y, button_flags, hit_inside_a_menu);
//            if(b || (hit_inside_a_menu == false))
//            {
//                RecvSigTerminateMenuCascade();
//                m_MenuIsActive = false;
//                m_PopupWindow->Hide();
//            }
      }
    }

    NeedRedraw();
  }

  void ComboBoxComplex::RecvMouseDrag (int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
  {
//    int row, column;
//    TableItem* item = 0;
//    int X = x + _combo_box_area->GetX() - m_PopupWindow->GetX() - m_ListBox->GetX();
//    int Y = y + _combo_box_area->GetY() - m_PopupWindow->GetY() - m_ListBox->GetY();
//    m_ListBox->FindItemUnderPointer(X, Y, &item, row, column);
//    m_ListBox->HighlightItem(row, column);

    if (GetWindowCompositor().GetCurrentEvent() )
    {
      m_PopupWindow->ProcessEvent (const_cast<IEvent &> (*GetWindowCompositor().GetCurrentEvent() ), 0, 0);
    }
  }

  void ComboBoxComplex::RecvSigActionTriggered (TableCtrl &table, TableItem &item, unsigned int row, unsigned int column)
  {
    m_MenuIsActive = false;
    //m_CurrentMenu->StopMenu();

    m_IsOpeningMenu = false;

    m_SelectedTreeNode = &item;
    _combo_box_area->SetBaseString (m_SelectedTreeNode->GetName() );
    sigSelectedItem.emit (item);

    m_PopupWindow->Hide();
    NeedRedraw();
    // You can do something if you want with the menu* and the action*
  }

  void ComboBoxComplex::RecvSigTerminateMenuCascade()
  {
    //m_MenuIsActive = false;
    //m_CurrentMenu->StopMenu();
    m_IsOpeningMenu = false;
  }

  void ComboBoxComplex::SetPopupWindowSize (unsigned int width, unsigned int height)
  {
    m_ListBox->SetMaximumSize (width, height);
  }

  void ComboBoxComplex::SetPopupWidth (unsigned int width)
  {
    m_ListBox->SetMinimumWidth (width);
  }

  long ComboBoxComplex::PostLayoutManagement (long LayoutResult)
  {
    View::PostLayoutManagement (LayoutResult);
    Geometry geo = GetGeometry();

    // If we call m_ListBox->SetSizeMatchContent(false) in the constructor, then the m_ListBox will use its minimum height because
    // m_Layout has a stretch factor of 0 and the m_ListBox height is set to eFull;
    m_ListBox->SetMinimumHeight (60);
//    m_ListBox->SetMaximumHeight(300);
    m_ListBox->SetMinimumWidth (GetBaseWidth() );

    return LayoutResult;
  }

}
