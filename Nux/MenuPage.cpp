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

#include "MenuPage.h"
#include "WindowCompositor.h"
#include "ActionItem.h"
#include "VLayout.h"
#include "StaticText.h"

namespace nux
{

  NUX_IMPLEMENT_OBJECT_TYPE(MenuItem);
  NUX_IMPLEMENT_OBJECT_TYPE(MenuSeparator);
  NUX_IMPLEMENT_OBJECT_TYPE(MenuPage);

  static const int MENU_ICONE_WIDTH = 18;
  static const int MENU_ITEM_MIN_WIDTH = 90;
  static const int MENU_ITEM_MIN_HEIGHT = 20;
  static const int MENU_ITEM_BORDER_TO_ICON_MARGIN = 5;
  static const int MENU_ITEM_ICON_TO_TEXT_MARGIN = 5;
  static const int MENU_ITEM_TEXT_TO_BORDER_MARGIN = 5;

// Algorithm:
// To initiate the menu, the mouse has to hit one of the menu bar item(they hold the name of the menu that are displayed).
// When that happens, the menu attached to the MenuBarItem gets the focus and becomes visible. Before the mouse is released,
// the flag m_NextMouseUpMeanStop is set to FALSE, meaning that the following mouse up will not be processed by the menu, even if the released happened
// outside of any of the menu forming the menu cascade. This way, the menu will remain open. There are exceptions to this however.
// After the firs mouse down on the MenuBarItem and before the mouse up event, if the mouse over the the opened menu,
// then m_NextMouseUpMeanStop is set to TRUE. If the mouse is released over an active menu item, the menu cascade will close itself.
// If the mouse is released outside any of the menu forming the menu cascade, the menu will close itself.
//
//


  MenuItem::MenuItem(const char *label, int UserValue, NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
  {
    _child_menu     = 0;
    _action_item    = new ActionItem(label, UserValue, NUX_TRACKER_LOCATION);
    
    _pango_static_text = new StaticText(label, NUX_TRACKER_LOCATION);
    _pango_static_text->SetTextColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
  }

  MenuItem::~MenuItem()
  {
    _pango_static_text->Dispose();

    if (_action_item)
      _action_item->UnReference();
    if (_child_menu)
      _child_menu->UnReference();
  }

  void MenuItem::SetChildMenu(MenuPage *menu)
  {
    nuxAssert(menu);
    NUX_RETURN_IF_NULL(menu);

    if (_child_menu)
      _child_menu->UnReference();
    _child_menu = menu;
    _child_menu->Reference();
  }

  MenuPage *MenuItem::GetChildMenu() const
  {
    return _child_menu;
  }

  void MenuItem::SetActionItem(ActionItem *action)
  {
    nuxAssertMsg(action != 0, "[MenuItem::SetActionItem] Parameter is Null.");

    if (action == 0)
      return;

    _pango_static_text->Dispose();
    if (_action_item)
      _action_item->UnReference();
    _action_item = action;
    _action_item->Reference();

    _pango_static_text = new StaticText(_action_item->GetLabel(), NUX_TRACKER_LOCATION);
  }

  ActionItem *MenuItem::GetActionItem() const
  {
    return _action_item;
  }

  int MenuItem::GetTextWidth()
  {
    if (_pango_static_text)
    {
      int w, h;
      _pango_static_text->GetTextLayoutSize(w, h);
      return w;
    }
    return 0;
  }

  int MenuItem::GetTextHeight()
  {
    if (_pango_static_text)
    {
      int w, h;
      _pango_static_text->GetTextLayoutSize(w, h);
      return h;
    }
    return 0;
  }


//ActionItem* MenuItem::GetActionItem()
//{
//    return &_action_item;
//}


  void MenuItem::Draw(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {

  }

  void MenuItem::DrawAsMenuItem(GraphicsEngine &graphics_engine, const Color & /* textcolor */, bool is_highlighted, bool /* isFirstItem */, bool /* isLastItem */, bool /* draw_icone */)
  {
    Geometry geo = GetGeometry();
    Geometry icon_geo(0, 0, 20, 20);
    Geometry text_geo = geo;

    text_geo.OffsetPosition(24, 0);
    text_geo.OffsetSize(2 * 24, 0);

    icon_geo.SetX(geo.x + 2);
    icon_geo.SetY(geo.y + 0);

    const char *label = _action_item->GetLabel();

    if (is_highlighted)
    {
      /*
          if (isFirstItem && isLastItem)
              GetPainter().PaintShape(graphics_engine, geo, COLOR_FOREGROUND_SECONDARY, eSHAPE_CORNER_ROUND4);
          else if (isFirstItem)
              GetPainter().PaintShapeCorner(graphics_engine, geo, COLOR_FOREGROUND_SECONDARY, eSHAPE_CORNER_ROUND4, eCornerTopLeft | eCornerTopRight);
          else if (isLastItem)
              GetPainter().PaintShapeCorner(graphics_engine, geo, COLOR_FOREGROUND_SECONDARY, eSHAPE_CORNER_ROUND4, eCornerBottomLeft | eCornerBottomRight);
          else
              GetPainter().Paint2DQuadColor(graphics_engine, geo, COLOR_FOREGROUND_SECONDARY);
      */
      GetPainter().Paint2DQuadColor(graphics_engine, geo, Color(0x44000000) /*COLOR_FOREGROUND_SECONDARY*/);
      _pango_static_text->SetTextColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
    }
    else
    {
      _pango_static_text->SetTextColor(Color(0.0f, 0.0f, 0.0f, 1.0f));
      //GetPainter().Paint2DQuadColor(graphics_engine, geo, Color(0xFF868686));
    }

    //if(m_Icon)
    {
      //GetPainter().Draw2DTextureAligned(graphics_engine, &_action_item->GetIcon(), icon_geo, TextureAlignmentStyle(eTACenter, eTACenter));
    }

    if (label)
    {
      //GetPainter().PaintTextLineStatic(graphics_engine, GetFont(), text_geo, std::string(label), textcolor, eAlignTextLeft);

      _pango_static_text->SetGeometry(text_geo);
      _pango_static_text->ProcessDraw(graphics_engine, true);
    }
  }

  MenuSeparator::MenuSeparator(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
  {

  }

  MenuSeparator::~MenuSeparator()
  {

  }

  void MenuSeparator::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    int y0 = base.y + base.GetHeight() / 2;
    GetPainter().Draw2DLine(graphics_engine, base.x, y0, base.x + base.GetWidth(), y0, Color(0xFF222222));
    GetPainter().Draw2DLine(graphics_engine, base.x, y0 + 1, base.x + base.GetWidth(), y0 + 1, Color(0xFFAAAAAA));
  }

  MenuPage::MenuPage(const char *title, NUX_FILE_LINE_DECL)
  : View(NUX_FILE_LINE_PARAM)
  {
    m_Parent = 0;
    m_item_width = MENU_ITEM_MIN_WIDTH;
    m_item_height = MENU_ITEM_MIN_HEIGHT;
    m_show_item_icon = true;
    m_MenuWindow = 0;
    m_Name = title;
    m_IsTopOfMenuChain = false;
    _font_name = g_strdup("Ubuntu 12");

    on_closure_continue_with_event_ = false;

    // Set Original State

    // Set Signals
    mouse_move.connect(sigc::mem_fun(this, &MenuPage::EmitMouseMove));
    mouse_drag.connect(sigc::mem_fun(this, &MenuPage::EmitMouseDrag));
    mouse_down.connect(sigc::mem_fun(this, &MenuPage::EmitMouseDown));
    mouse_up.connect(sigc::mem_fun(this, &MenuPage::EmitMouseUp));
    mouse_leave.connect(sigc::mem_fun(this, &MenuPage::RecvMouseLeave));
    mouse_down_outside_pointer_grab_area.connect(sigc::mem_fun(this, &MenuPage::Terminate));

    // Set Geometry
    SetGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    // Set layout

    m_numItem = 0;
    m_HighlightedItem = -1;
    m_IsActive = false;
    m_NextMouseUpMeanStop = false;
    m_SubMenuAction = 0;

    _vlayout = new VLayout(NUX_TRACKER_LOCATION);
    // No Need to set a composition layout.
    // The MenuPage is floating above everything else.
    SetLayout(_vlayout);

    SetTextColor(color::Black);
  }

  MenuPage::~MenuPage()
  {
//     std::vector <MenuItem*>::iterator it;
// 
//     for (it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
//     {
//       (*it)->UnReference();
//     }
    m_MenuItemVector.clear();
    m_MenuSeparatorVector.clear();
    RemoveLayout();
  }

//void MenuPage::SetName(const char* name)
//{
//    m_Name = name;
//}
//
  const char *MenuPage::GetName() const
  {
    return m_Name.c_str();
  }

  bool MenuPage::CanClose() const
  {
    return m_Action_Triggered;
  }

  Area* MenuPage::FindAreaUnderMouse(const Point& mouse_position, NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return NULL;

    if ((event_type == NUX_MOUSE_WHEEL) && (!AcceptMouseWheelEvent()))
      return NULL;
    return this;
  }

  void MenuPage::Draw(GraphicsEngine &graphics_engine, bool force_draw)
  {
    if (m_IsActive)
    {
      Geometry base = GetGeometry();
      Geometry shadow;
      shadow = base;
      shadow.OffsetPosition(4, 4);
      //GetPainter().PaintShape(graphics_engine, shadow, Color(0xFF000000), eSHAPE_CORNER_ROUND4_SHADOW);

      graphics_engine.PushClippingRectangle(base);
      graphics_engine.GetRenderStates().SetBlend(GL_TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      GetPainter().Paint2DQuadColor(graphics_engine, base, Color(0xCCFFFFFF));
      graphics_engine.GetRenderStates().SetBlend(GL_FALSE);

      Geometry text_area;
      text_area.SetX(base.x);
      text_area.SetY(base.y);
      text_area.SetWidth(base.GetWidth());
      text_area.SetHeight(PRACTICAL_WIDGET_HEIGHT);

      int i;
      std::vector<MenuItem *>::iterator it;
      int numItem = (int) m_MenuItemVector.size();

      for (it = m_MenuItemVector.begin(), i = 0; it != m_MenuItemVector.end(); it++, i++)
      {
        bool is_highlighted = (m_HighlightedItem == i);
        (*it)->DrawAsMenuItem(graphics_engine, Color(0xFFFFFFFF) /*GetTextColor()*/, is_highlighted, i == 0, i == (numItem - 1), true);
      }

      std::vector<MenuSeparator* >::iterator separator_iterator;

      for (separator_iterator = m_MenuSeparatorVector.begin(); separator_iterator != m_MenuSeparatorVector.end(); separator_iterator++)
      {
        (*separator_iterator)->Draw(graphics_engine, force_draw);
      }

      graphics_engine.PopClippingRectangle();
    }
  }

  void MenuPage::DrawContent(GraphicsEngine & /* graphics_engine */, bool /* force_draw */)
  {

  }

  void MenuPage::SetFontName(char *font_name)
  {
    std::vector<MenuItem *>::iterator it;

    for (it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); ++it)
    {
      (*it)->GetStaticText()->SetFontName(font_name);
    }

    g_free(_font_name);
    _font_name = g_strdup(font_name);
  }

  ActionItem *MenuPage::AddAction(const char *label, int UserValue)
  {
    // pMenuItem if added to the layout do not sink the Reference.
    MenuItem *pMenuItem(new MenuItem(label, UserValue, NUX_TRACKER_LOCATION));
    pMenuItem->GetStaticText()->SetFontName(_font_name);

    m_MenuItemVector.push_back(pMenuItem);
    pMenuItem->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    int new_item_width = pMenuItem->GetTextWidth(); //GetFont()->GetStringWidth(pMenuItem->GetActionItem()->GetLabel());

    if (new_item_width < m_item_width)
    {
      std::vector<MenuItem *>::iterator it;
      for (it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
      {
        if (ShowItemIcon())
        {
          pMenuItem->SetMinMaxSize(MENU_ITEM_BORDER_TO_ICON_MARGIN
                                    + MENU_ICONE_WIDTH
                                    + MENU_ITEM_ICON_TO_TEXT_MARGIN
                                    + m_item_width
                                    + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
        }
        else
        {
          pMenuItem->SetMinMaxSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                                    + m_item_width
                                    + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
        }
      }
    }
    else
    {
      std::vector<MenuItem *>::iterator it;
      for (it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
      {
        if (ShowItemIcon())
        {
          (*it)->SetMinMaxSize(MENU_ITEM_BORDER_TO_ICON_MARGIN
                                    + MENU_ICONE_WIDTH
                                    + MENU_ITEM_ICON_TO_TEXT_MARGIN
                                    + new_item_width
                                    + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
        }
        else
        {
          (*it)->SetMinMaxSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                                    + new_item_width
                                    + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
        }

        (*it)->SetBaseSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                            + new_item_width
                            + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }

      m_item_width = new_item_width;
//       SetBaseWidth(MENU_ITEM_ICON_TO_TEXT_MARGIN
//                     + m_item_width
//                     + MENU_ITEM_TEXT_TO_BORDER_MARGIN);
    }

    if (pMenuItem->GetChildMenu() != 0)
    {
      pMenuItem->GetChildMenu()->SetParentMenu(this);
    }

    m_numItem = (int) m_MenuItemVector.size();
    _vlayout->AddView(pMenuItem, 0, eLeft, eFix);
    ComputeContentSize();

    return pMenuItem->GetActionItem();
  }

//void MenuPage::AddActionItem(ActionItem* actionItem)
//{
//    nuxAssertMsg(actionItem != 0, "[MenuPage::AddActionItem] Parameter is Null.");
//    if (actionItem == 0)
//        return;
//
//     MenuItem* pMenuItem = new MenuItem(actionItem->GetLabel(), actionItem->GetUserValue(), NUX_TRACKER_LOCATION);
//     pMenuItem->SinkReference();
//    pMenuItem->SetActionItem(actionItem);
//
//    m_MenuItemVector.push_back(pMenuItem);
//    pMenuItem->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
//
//    int new_item_width = GetFont()->GetStringWidth(actionItem->GetLabel());
//    if (new_item_width < m_item_width)
//    {
//        if (ShowItemIcon())
//        {
//            pMenuItem->SetMinMaxSize(MENU_ITEM_BORDER_TO_ICON_MARGIN
//                + MENU_ICONE_WIDTH
//                + MENU_ITEM_ICON_TO_TEXT_MARGIN
//                + m_item_width
//                + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
//        }
//        else
//        {
//            pMenuItem->SetMinMaxSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
//                + m_item_width
//                + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
//        }
//    }
//    else
//    {
//        if (ShowItemIcon())
//        {
//            pMenuItem->SetMinMaxSize(MENU_ITEM_BORDER_TO_ICON_MARGIN
//                + MENU_ICONE_WIDTH
//                + MENU_ITEM_ICON_TO_TEXT_MARGIN
//                + new_item_width
//                + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
//        }
//        else
//        {
//            pMenuItem->SetMinMaxSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
//                + new_item_width
//                + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
//        }
//
//        std::vector<MenuItem*>::iterator it;
//        for (it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
//        {
//            (*it)->setSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
//                + new_item_width
//                + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
//        }
//
//        m_item_width = new_item_width;
//        SetWidth(MENU_ITEM_ICON_TO_TEXT_MARGIN
//            + m_item_width
//            + MENU_ITEM_TEXT_TO_BORDER_MARGIN);
//    }
//
//    if (pMenuItem->GetChildMenu() != 0)
//    {
//        pMenuItem->GetChildMenu()->SetParentMenu(this);
//    }
//
//    m_numItem = (int)m_MenuItemVector.size();
//    _vlayout->AddView(pMenuItem, 0, eLeft, eFix);
//    ComputeContentSize();
//}

  MenuPage *MenuPage::AddMenu(const char *label)
  {
    // pMenuItem if added to the layout do not sink the Reference.
    MenuItem *pMenuItem(new MenuItem(label, 0, NUX_TRACKER_LOCATION));

    pMenuItem->SetChildMenu(new MenuPage(label));
    //pMenuItem->SetActionItem(new ActionItem());
    //pMenuItem->GetActionItem()->SetLabel(label);
    m_MenuItemVector.push_back(pMenuItem);
    pMenuItem->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    int new_item_width = pMenuItem->GetTextWidth(); //GetFont()->GetStringWidth(label);

    if (new_item_width < m_item_width)
    {
      if (ShowItemIcon())
      {
        pMenuItem->SetMinMaxSize(MENU_ITEM_BORDER_TO_ICON_MARGIN
                                  + MENU_ICONE_WIDTH
                                  + MENU_ITEM_ICON_TO_TEXT_MARGIN
                                  + m_item_width
                                  + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }
      else
      {
        pMenuItem->SetMinMaxSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                                  + m_item_width
                                  + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }
    }
    else
    {
      if (ShowItemIcon())
      {
        pMenuItem->SetMinMaxSize(MENU_ITEM_BORDER_TO_ICON_MARGIN
                                  + MENU_ICONE_WIDTH
                                  + MENU_ITEM_ICON_TO_TEXT_MARGIN
                                  + new_item_width
                                  + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }
      else
      {
        pMenuItem->SetMinMaxSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                                  + new_item_width
                                  + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }

      std::vector< MenuItem * >::iterator it;

      for (it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
      {
        (*it)->SetBaseSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                            + new_item_width
                            + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }

      m_item_width = new_item_width;
      SetBaseWidth(MENU_ITEM_ICON_TO_TEXT_MARGIN
                    + m_item_width
                    + MENU_ITEM_TEXT_TO_BORDER_MARGIN);
    }

    if (pMenuItem->GetChildMenu() != 0)
    {
      pMenuItem->GetChildMenu()->SetParentMenu(this);
    }

    m_numItem = (int) m_MenuItemVector.size();
    _vlayout->AddView(pMenuItem, 0, eLeft, eFix);
    ComputeContentSize();

    return pMenuItem->GetChildMenu();
  }

  ActionItem *MenuPage::AddSubMenu(const char * /* label */, MenuPage *menu)
  {
    menu->m_IsTopOfMenuChain = false;
    // pMenuItem if added to the layout do not sink the Reference.
    MenuItem *pMenuItem(new MenuItem(menu->GetName(), 0, NUX_TRACKER_LOCATION));
    
    pMenuItem->SetChildMenu(menu);
    m_MenuItemVector.push_back(pMenuItem);
    pMenuItem->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    int new_item_width = pMenuItem->GetTextWidth(); //GetFont()->GetStringWidth(label);

    if (new_item_width < m_item_width)
    {
      if (ShowItemIcon())
      {
        pMenuItem->SetMinMaxSize(MENU_ITEM_BORDER_TO_ICON_MARGIN
                                  + MENU_ICONE_WIDTH
                                  + MENU_ITEM_ICON_TO_TEXT_MARGIN
                                  + m_item_width
                                  + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }
      else
      {
        pMenuItem->SetMinMaxSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                                  + m_item_width
                                  + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }
    }
    else
    {
      if (ShowItemIcon())
      {
        pMenuItem->SetMinMaxSize(MENU_ITEM_BORDER_TO_ICON_MARGIN
                                  + MENU_ICONE_WIDTH
                                  + MENU_ITEM_ICON_TO_TEXT_MARGIN
                                  + new_item_width
                                  + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }
      else
      {
        pMenuItem->SetMinMaxSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                                  + new_item_width
                                  + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }

      std::vector< MenuItem * >::iterator it;

      for (it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
      {
        (*it)->SetBaseSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                            + new_item_width
                            + MENU_ITEM_TEXT_TO_BORDER_MARGIN, m_item_height);
      }

      m_item_width = new_item_width;
      SetBaseWidth(MENU_ITEM_ICON_TO_TEXT_MARGIN
                    + m_item_width
                    + MENU_ITEM_TEXT_TO_BORDER_MARGIN);
    }

    if (pMenuItem->GetChildMenu() != 0)
    {
      pMenuItem->GetChildMenu()->SetParentMenu(this);
    }

    m_numItem = (int) m_MenuItemVector.size();
    _vlayout->AddView(pMenuItem, 0, eLeft, eFix);
    ComputeContentSize();

    return pMenuItem->GetActionItem();
  }

  void MenuPage::AddSeparator()
  {
    // pMenuSeparator if added to the layout do not sink the Reference.
    MenuSeparator *pMenuSeparator(new MenuSeparator(NUX_TRACKER_LOCATION));

    m_MenuSeparatorVector.push_back(pMenuSeparator);

    if (ShowItemIcon())
    {
      pMenuSeparator->SetMinMaxSize(MENU_ITEM_BORDER_TO_ICON_MARGIN
                                     + MENU_ICONE_WIDTH
                                     + MENU_ITEM_ICON_TO_TEXT_MARGIN
                                     + m_item_width
                                     + MENU_ITEM_TEXT_TO_BORDER_MARGIN, 4);
    }
    else
    {
      pMenuSeparator->SetMinMaxSize(MENU_ITEM_ICON_TO_TEXT_MARGIN
                                     + m_item_width
                                     + MENU_ITEM_TEXT_TO_BORDER_MARGIN, 4);
    }

    _vlayout->AddView(pMenuSeparator, 0, eLeft, eFix);
    ComputeContentSize();
  }

  void MenuPage::RemoveItem(ActionItem * /* item */)
  {
  }

  void MenuPage::RemoveAllItem()
  {
    m_MenuSeparatorVector.clear();
    m_MenuItemVector.clear();
    m_numItem = 0;
    _vlayout->Clear();
    ComputeContentSize();
   
    //FIXME - Hack to fix a bug with the menu height not being reset after removing items
    Geometry base = GetGeometry();
    base.height = 0;
    View::SetGeometry(base);
  }

  void MenuPage::EmitMouseMove(int /* x */, int y, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    if (IsMouseInside())
    {
      m_NextMouseUpMeanStop = true;
      // Find on which item the mouse is
      std::vector< MenuItem * >::iterator item_iterator;
      unsigned int i = 0;
      m_HighlightedItem = -1;

      for (item_iterator = m_MenuItemVector.begin(), i = 0; item_iterator != m_MenuItemVector.end(); item_iterator++, i++)
      {
        int MenuY = GetBaseY();
        int py = (*item_iterator)->GetBaseY() - MenuY;
        int height = (*item_iterator)->GetBaseHeight();

        if ((y >= py) && (y < py + height))
        {
          m_HighlightedItem = i;
          QueueDraw();
          break;
        }
      }
    }
    else
    {
      if (m_HighlightedItem != -1)
      {
        m_HighlightedItem = -1;
        QueueDraw();
      }
    }

    if (m_HighlightedItem >= 0)
    {
      MenuItem *selected_action = m_MenuItemVector[m_HighlightedItem];

      if ((selected_action->GetChildMenu() != 0) && selected_action->GetActionItem()->isEnabled())
      {
        // This MenuItem has a sub-MenuPage. Start it.
        Geometry geo = selected_action->GetGeometry();
        selected_action->GetChildMenu()->m_MenuWindow = selected_action->GetChildMenu()->GetParentMenu()->m_MenuWindow;
        selected_action->GetChildMenu()->StartMenu(geo.x + geo.GetWidth() - 5, geo.y, 0, 0);

        // The current SubMenu is not the same as the new one...
        if (m_SubMenuAction != selected_action)
        {
          // If m_SubMenuAction is not null then stop the sub menu
          StopActionSubMenu();
          // Set the Action that holds the SubMenu.
          m_SubMenuAction = selected_action;
        }
      }
      else
      {
        StopActionSubMenu();
      }
    }
  }

  void MenuPage::EmitMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    m_NextMouseUpMeanStop = true;

    EmitMouseMove(x, y, 0, 0, button_flags, key_flags);
  }

  void MenuPage::EmitMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags)
  {
    if (m_NextMouseUpMeanStop == false)
    {
      m_NextMouseUpMeanStop = true;
      return;
    }

    m_Action_Triggered = false;

    bool hit_inside_a_menu = false;

    if (m_SubMenuAction)
    {
      m_Action_Triggered = m_SubMenuAction->GetChildMenu()->TestMouseUp(x, y, button_flags, key_flags, hit_inside_a_menu);
    }

    if (m_Action_Triggered == false)
    {
      if (IsMouseInside())
      {
        if (m_SubMenuAction)
        {
          m_Action_Triggered = false;
        }
        else if (m_HighlightedItem != -1)
        {
          if (m_MenuItemVector[m_HighlightedItem]->GetActionItem()->isEnabled() == false)
          {
            // Do nothing. We don't want to close the menu when we release the mouse over an action that is not Enabled.
            m_Action_Triggered = false;
          }
          else
          {
            m_Action_Triggered = true;
            // Fire the Action Here
            ExecuteActionItem(m_MenuItemVector[m_HighlightedItem]);
            NotifyActionTriggeredToParent(this, m_MenuItemVector[m_HighlightedItem]);
          }
        }
      }
      else
      {
        // Bellow the Mouse, there was no MenuPage when the MouseUp happened.
        if (hit_inside_a_menu == false)
        {
          // Terminate the MenuPage cascade
          NotifyTerminateMenuCascade();
        }
      }
    }
  }

  bool MenuPage::TestMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags, bool &hit_inside_a_menu)
  {
    m_Action_Triggered = false;

    if (m_SubMenuAction)
    {
      m_Action_Triggered = m_SubMenuAction->GetChildMenu()->TestMouseUp(x, y, button_flags, key_flags, hit_inside_a_menu);
    }

    if (m_Action_Triggered == false)
    {
      if (IsMouseInside())
      {
        hit_inside_a_menu = true;

        if (m_SubMenuAction)
        {
          m_Action_Triggered = false;
          // Do nothing. We don't want to close the menu when we are above an action that has a submenu.
        }
        else if (m_HighlightedItem != -1)
        {
          if (m_MenuItemVector[m_HighlightedItem]->GetActionItem()->isEnabled() == false)
          {
            // Do nothing. We don't want to close the menu when we release the mouse over an action that is not Enabled.
            m_Action_Triggered = false;
          }
          else
          {
            m_Action_Triggered = true;
            // Fire the Action Here
            ExecuteActionItem(m_MenuItemVector[m_HighlightedItem]);
            NotifyActionTriggeredToParent(this, m_MenuItemVector[m_HighlightedItem]);
            // But Do not emit the Stop
            //sigPopupStop.emit();
          }
        }
      }
    }

    if (m_Action_Triggered)
      return true;

    return false;
  }

  bool MenuPage::TestMouseDown()
  {
    bool b = false;

    if (m_SubMenuAction)
    {
      b = m_SubMenuAction->GetChildMenu()->TestMouseDown();
    }

    if (b)
    {
      return true;
    }
    else
    {
      return IsMouseInside();
    }
  }

  void MenuPage::EmitMouseDrag(int x, int y, int /* dx */, int /* dy */, unsigned long button_flags, unsigned long key_flags)
  {
    if (IsMouseInside())
      EmitMouseMove(x, y, 0, 0, button_flags, key_flags);
  }

  void MenuPage::RecvMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
    // Cancel selected item when the mouse is out.
    if (m_HighlightedItem != -1)
    {
      MenuItem *item = m_MenuItemVector[m_HighlightedItem];

      if (item->GetChildMenu())
      {
        if (!item->GetChildMenu()->IsActive())
        {
          m_HighlightedItem = -1;
        }
      }
      else
      {
        m_HighlightedItem = -1;
      }
    }

    QueueDraw();
  }

  void MenuPage::StartMenu(int MenuXPosition, int MenuYPosition, int /* x*/, int /* y */, bool /* OverrideCurrentMenuChain */)
  {
    Geometry base = GetGeometry();
    base.SetX(MenuXPosition);
    base.SetY(MenuYPosition);

    int WindowWidth = GetWindowThread()->GetGraphicsEngine().GetWindowWidth();
    int WindowHeight = GetWindowThread()->GetGraphicsEngine().GetWindowHeight();

    // Correct the position of the MenuPage if is going out of the screen
    // The first page of a menu chain has less opportunities to be adjusted than its child pages.
    // The first page of a menu chain has(GetParentMenu() == 0) return true.
    int MenuLeft = base.x + base.GetWidth() + 5;

    if (MenuLeft > WindowWidth)
    {
      base.OffsetPosition(WindowWidth - MenuLeft, 0);
    }

    int MenuRight = base.x;

    if ((MenuRight < 0) && (GetParentMenu() != 0))
    {
      base.OffsetPosition(-MenuRight, 0);
    }

    int MenuBottom = base.y + base.GetHeight() + 5;

    if ((MenuBottom > WindowHeight) && (GetParentMenu() != 0))
    {
      base.OffsetPosition(0, WindowHeight - MenuBottom);
    }

    int MenuTop = base.y - 5;

    if ((MenuTop < 0) && (GetParentMenu() != 0))
    {
      base.OffsetPosition(0, -MenuTop);
    }

    SetGeometry(base);

    SetActive(true);

    // Add the menu to the stack manager popup list.
    if (GetParentMenu() == 0)
    {
      // This is the head of the menu chain
      m_MenuWindow = GetWindowThread()->GetWindowCompositor().GetProcessingTopView();
    }

    GetWindowThread()->GetWindowCompositor().AddMenu(this, m_MenuWindow/*, OverrideCurrentMenuChain*/);
    m_NextMouseUpMeanStop = false;

    StopActionSubMenu();
  }

  void MenuPage::StopMenu(int /* x */, int /* y */)
  {
    SetActive(false);
    // The Stack Manager will remove all popup that are not visible anymore
    m_NextMouseUpMeanStop = false;
    m_HighlightedItem = -1;

    StopActionSubMenu();

    // The MenuPage does not need to be redrawn, but in embedded mode, this triggers a dirty Area on 
    // the BaseWindow and it informs the system to update this egion of the screen
    // (where the menu is about to disappear).
    QueueDraw();

    /*Area *top_area = GetWindowThread()->GetWindowCompositor().GetProcessingTopView();
    if (top_area)
    {
      if (top_area->IsView())
      {
        NUX_STATIC_CAST(View*, top_area)->QueueDraw();
      }
      
      if (top_area->IsLayout())
      {
        NUX_STATIC_CAST(Layout*, top_area)->QueueDraw();
      }
    }*/
  }

// Never call this function directly
  void MenuPage::Terminate(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */)
  {
  }

  void MenuPage::StopActionSubMenu()
  {
    if (m_SubMenuAction)
    {
      if (m_SubMenuAction->GetChildMenu())
      {
        m_SubMenuAction->GetChildMenu()->StopMenu(0, 0);
      }
    }

    m_SubMenuAction = 0;
  }

  void MenuPage::ExecuteActionItem(MenuItem *menuItem)
  {
    menuItem->GetActionItem()->Trigger();
  }

  void MenuPage::NotifyActionTriggeredToParent(MenuPage *menu, MenuItem *menuItem)
  {
    if (m_Parent)
    {
      m_Parent->NotifyActionTriggeredToParent(menu, menuItem);
    }
    else
    {
      sigActionTriggered.emit(menu, menuItem->GetActionItem());
    }

    StopMenu();
  }

  void MenuPage::NotifyTerminateMenuCascade()
  {
    if (m_Parent)
    {
      m_Parent->NotifyTerminateMenuCascade();
    }
    else
    {
      sigTerminateMenuCascade.emit();
    }

//     if (IsActive())
//     {
//         StopMenu();
//     }
  }

  void MenuPage::NotifyMouseDownOutsideMenuCascade(int x, int y)
  {
    if (m_Parent)
    {
      m_Parent->NotifyMouseDownOutsideMenuCascade(x, y);
    }
    else
    {
      // This is the top MenuPage in a menu chain.
      // If this MenuPage has been registered with a MenuBar, then the MenuBar will intercept this signal
      // and terminate the menu chain.
      sigMouseDownOutsideMenuCascade.emit(this, x, y);

      // It is also possible that this MenuPage is not associated to a MenuBar(called directly for a contextual menu)
      if (m_IsTopOfMenuChain == false)
      {
        // This MenuPage is not attached to a MenuBar
        if (IsActive())
        {
          //StopMenu();
        }
      }
    }
  }

  void MenuPage::SetParentMenu(MenuPage *parent)
  {
    m_Parent = parent;
  }

  MenuPage *MenuPage::GetParentMenu()
  {
    return m_Parent;
  }

  long MenuPage::ComputeContentSize()
  {
    return View::ComputeContentSize();
  }

  void MenuPage::SetGeometry(const Geometry &geo)
  {
    // The MenuPage widget cannot be resized by the client. The menu widget controls its own size.

    Geometry base = GetGeometry();
    // We are only interested in the position X/Y. The popup figures out its width and height by itself.
    base.SetX(geo.x);
    base.SetY(geo.y);

    base.SetWidth(geo.GetWidth());
    base.SetHeight(m_numItem * PRACTICAL_WIDGET_HEIGHT);

    SetBaseXY(geo.x, geo.y);

    ComputeContentPosition(0, 0);
  }

  ActionItem *MenuPage::GetActionItem(int index) const
  {
    nuxAssert(index >= 0);

    if (index >= (int) m_MenuItemVector.size())
      return 0;

    return m_MenuItemVector[index]->GetActionItem();
  }

  int MenuPage::GetActionItemIndex(ActionItem *action) const
  {
    if (action == 0)
      return -1;

    for (int i = 0; i < (int) m_MenuItemVector.size(); i++)
    {
      if (action == m_MenuItemVector[i]->GetActionItem())
      {
        return i;
      }
    }

    return -1;
  }

  Geometry MenuPage::GetAbsoluteGeometry() const
  {
    return GetGeometry();  
  }

  Geometry MenuPage::GetRootGeometry() const
  {
    return GetGeometry();
  }

  void MenuPage::SetOnClosureContinueEventCycle(bool on_closure_continue_with_event)
  {
    on_closure_continue_with_event_ = on_closure_continue_with_event;
  }

  bool MenuPage::OnClosureContinueEventCycle() const
  {
    return on_closure_continue_with_event_;
  }
}
