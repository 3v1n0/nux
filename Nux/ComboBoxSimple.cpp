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
#include "ComboBoxSimple.h"
#include "MenuPage.h"
#include "ActionItem.h"
#include "StaticText.h"


namespace nux
{
  ComboBoxSimple::ComboBoxSimple(NUX_FILE_LINE_DECL)
    :   AbstractComboBox(NUX_FILE_LINE_PARAM)
  {
    m_SelectedAction    = 0;
    m_CurrentMenu       = 0;

    m_CurrentMenu = new MenuPage("", NUX_TRACKER_LOCATION);
    m_CurrentMenu->SinkReference();
    m_CurrentMenu->SetParentObject(this);
    
    // Set Signals
    _combo_box_opening_area->mouse_down.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvMouseDown));
    _combo_box_opening_area->mouse_up.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvMouseUp));
    _combo_box_area->mouse_down.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvMouseDown));
    _combo_box_area->mouse_up.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvMouseUp));

    //m_Popup.sigPopupStop.connect(sigc::mem_fun(this, &ComboBox::OnPopupStop));

    // Set Geometry
    _combo_box_opening_area->SetGeometry(Geometry(0, 0, 20, DEFAULT_WIDGET_HEIGHT));
    //_combo_box_opening_area->SetMaximumSize(20, DEFAULT_WIDGET_HEIGHT);
    _combo_box_opening_area->SetMinimumSize(20, DEFAULT_WIDGET_HEIGHT);

    _combo_box_area->SetMinimumSize(2 * DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
    _combo_box_area->SetGeometry(Geometry(0, 0, 3 * DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT));
    //_pango_static_text->SetClipping(_combo_box_area->GetBaseWidth());
    _combo_box_area->geometry_changed.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvGeometryChanged));

    //m_CurrentMenu = new MenuPage;
    m_CurrentMenu->SetParentMenu(0);
    m_CurrentMenu->sigActionTriggered.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvSigActionTriggered));
    m_CurrentMenu->sigTerminateMenuCascade.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvSigTerminateMenuCascade));
    m_CurrentMenu->sigClosingMenu.connect(sigc::mem_fun(this, &ComboBoxSimple::RecvClosingMenuSignal));
  }

  ComboBoxSimple::~ComboBoxSimple()
  {
    m_CurrentMenu->UnParentObject();
    m_CurrentMenu->UnReference();
  }

  Area* ComboBoxSimple::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    NUX_RETURN_VALUE_IF_TRUE(_combo_box_opening_area->TestMousePointerInclusion(mouse_position, event_type), _combo_box_opening_area);
    NUX_RETURN_VALUE_IF_TRUE(_combo_box_area->TestMousePointerInclusion(mouse_position, event_type), _combo_box_area);

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  void ComboBoxSimple::MoveSelectionUp()
  {
    int current_index = GetSelectionIndex();
    SetSelectionIndex(current_index - 1);
  }

  void ComboBoxSimple::MoveSelectionDown()
  {
    int current_index = GetSelectionIndex();
    SetSelectionIndex(current_index + 1);
  }

  ActionItem *ComboBoxSimple::AddItem(const char *label, int Uservalue)
  {
    if (m_CurrentMenu->GetNumItem() == 0)
    {
      // The first element added is the element featured on the combo box when it is closed.
      m_SelectedAction = m_CurrentMenu->AddAction(label, Uservalue);
      _combo_box_area->SetBaseString(m_SelectedAction->GetLabel());

      _pango_static_text->SetText(m_SelectedAction->GetLabel());

      return m_SelectedAction;
    }
    else
    {
      return m_CurrentMenu->AddAction(label, Uservalue);
    }
  }

  void ComboBoxSimple::RemoveItem(ActionItem *item)
  {

  }

  void ComboBoxSimple::RemoveAllItem()
  {
    m_CurrentMenu->RemoveAllItem();
  }

  void ComboBoxSimple::RecvMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_MenuIsActive == false)
    {
      // Open the MenuPage
      m_MenuIsActive = true;
      m_IsOpeningMenu = true;

      Geometry geo = m_CurrentMenu->GetGeometry();
      geo.SetX(_combo_box_area->GetAbsoluteX());
      geo.SetY(_combo_box_area->GetAbsoluteY() + _combo_box_area->GetBaseHeight());
      geo.SetWidth(_combo_box_area->GetBaseWidth());
      //m_CurrentMenu->SetMinimumWidth(geo.width);
      //m_CurrentMenu->SetMaximumWidth(geo.width);
      m_CurrentMenu->SetGeometry(geo);
      m_CurrentMenu->ComputeContentSize();
      m_CurrentMenu->StartMenu(_combo_box_area->GetAbsoluteX(),
                                _combo_box_area->GetAbsoluteY() + _combo_box_area->GetBaseHeight(),
                                0, 0);
    }
    else
    {
      // If the mouse up that follows happen inside the area, then it is going to close the menu.
      m_IsOpeningMenu = false;
    }

    QueueDraw();
  }

  void ComboBoxSimple::RecvMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_MenuIsActive)
    {
      if (_combo_box_area->IsMouseInside() || _combo_box_opening_area->IsMouseInside())
      {
        if (m_IsOpeningMenu == false)
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

        if (b || (hit_inside_a_menu == false))
        {
          RecvSigTerminateMenuCascade();
          m_MenuIsActive = false;
        }
      }
    }

    QueueDraw();
  }

  void ComboBoxSimple::RecvSigActionTriggered(MenuPage *menu, ActionItem *action)
  {
    m_MenuIsActive = false;
    m_CurrentMenu->StopMenu();

    m_SelectedAction = action;
    _combo_box_area->SetBaseString(m_SelectedAction->GetLabel());
    m_IsOpeningMenu = false;

    _pango_static_text->SetText(m_SelectedAction->GetLabel());

    sigTriggered.emit(this);
    sigActionTriggered.emit(m_SelectedAction);

    QueueDraw();
    // You can do something if you want with the menu* and the action*
  }

  void ComboBoxSimple::RecvSigActionTriggered2(TableCtrl *table, TableItem *item, unsigned int row, unsigned int column)
  {
    m_MenuIsActive = false;
    m_CurrentMenu->StopMenu();
    m_IsOpeningMenu = false;
    QueueDraw();
    // You can do something if you want with the menu* and the action*
  }

  void ComboBoxSimple::RecvSigTerminateMenuCascade()
  {
    //m_MenuIsActive = false;
    m_CurrentMenu->StopMenu();
    m_IsOpeningMenu = false;
  }

  void ComboBoxSimple::RecvClosingMenuSignal(MenuPage* menu_page)
  {
    nuxAssert(menu_page == m_CurrentMenu);
    m_IsOpeningMenu = false;
    m_MenuIsActive = false;

    // When the menu is closing check if the mouse is still inside the combo box surface 
    // and set the _current_mouse_in flag accordingly.  
    if (!_combo_box_area->TestMousePointerInclusion(GetWindowThread()->GetWindowCompositor().GetMousePosition(), NUX_NO_EVENT))
    {
      _combo_box_area->mouse_in_ = false;
    }

    if (!_combo_box_opening_area->TestMousePointerInclusion(GetWindowThread()->GetWindowCompositor().GetMousePosition(), NUX_NO_EVENT))
    {
      _combo_box_opening_area->mouse_in_ = false;
    }

    QueueDraw();
  }

  void ComboBoxSimple::RecvGeometryChanged(Area *area, Geometry &geo)
  {
    //_pango_static_text->SetClipping(geo.width);
  }


  const char *ComboBoxSimple::GetSelectionLabel() const
  {
    if (m_SelectedAction)
      return m_SelectedAction->GetLabel();

    return 0;
  }

  int ComboBoxSimple::GetSelectionUserValue() const
  {
    if (m_SelectedAction)
      return m_SelectedAction->GetUserValue();

    return 0;
  }

  int ComboBoxSimple::GetNumItem() const
  {
    return m_CurrentMenu->GetNumItem();
  }

  ActionItem *ComboBoxSimple::GetItem(int index) const
  {
    return m_CurrentMenu->GetActionItem(index);
  }

  int ComboBoxSimple::GetSelectionIndex() const
  {
    if (m_SelectedAction)
      return m_CurrentMenu->GetActionItemIndex(m_SelectedAction);

    return -1;
  }

  void ComboBoxSimple::SetSelectionIndex(int index)
  {
    if ((index >= 0) && (index < m_CurrentMenu->GetNumItem()))
    {
      m_SelectedAction = m_CurrentMenu->GetActionItem(index);
      _combo_box_area->SetBaseString(m_SelectedAction->GetLabel());

      _pango_static_text->SetText(m_SelectedAction->GetLabel());

      QueueDraw();
    }
    else if (m_CurrentMenu->GetNumItem() > 0)
    {
      // index is negative
      m_SelectedAction = m_CurrentMenu->GetActionItem(0);
      _combo_box_area->SetBaseString(m_SelectedAction->GetLabel());

      _pango_static_text->SetText(m_SelectedAction->GetLabel());

      QueueDraw();
    }
    else
    {
      m_SelectedAction = 0;
    }
  }


}
