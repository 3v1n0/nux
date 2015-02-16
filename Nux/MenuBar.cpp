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
#include "WindowCompositor.h"
#include "HLayout.h"
#include "MenuBar.h"

namespace nux
{

  static const unsigned int MENU_MINIMUM_WIDTH = 10;

  static const unsigned int MENUBAR_ICON_WIDTH = 24;
  static const unsigned int MENUBAR_ICON_HEIGHT = 24;

  NUX_IMPLEMENT_ROOT_OBJECT_TYPE(MenuBarItem);
  NUX_IMPLEMENT_OBJECT_TYPE(MenuBar);

  MenuBarItem::MenuBarItem(NUX_FILE_LINE_DECL)
    :   Object(true, NUX_FILE_LINE_PARAM)
  {
    // This area is added to the layout of the MenuBar. The Menubar will will ref/unref it.
    area = new BasicView(NUX_TRACKER_LOCATION);
    icon = 0;
  }

  MenuBarItem::~MenuBarItem()
  {
    if (menu)
      menu->UnReference();
    if (icon)
      icon->UnReference();

    // The Area is owned by the MenuBar: do nothing for area.
  }

  MenuBar::MenuBar(NUX_FILE_LINE_DECL)
    :   View(NUX_FILE_LINE_PARAM)
    ,   m_MenuIsActive(false)
//,   m_CurrentMenu(0)
    ,   m_IsOpeningMenu(false)
  {
    m_CurrentMenu = NULL;
    m_MenuBarWindow = NULL;
    m_hlayout = new HLayout(NUX_TRACKER_LOCATION);
    m_hlayout->SetHorizontalInternalMargin(4);
    m_hlayout->SetHorizontalExternalMargin(2);

    SetMinimumSize(24, 24);
    SetMaximumSize(AREA_MAX_WIDTH, 24);
    SetGeometry(Geometry(0, 0, 200, 20));

    m_hlayout->SetHorizontalInternalMargin(4);
    m_hlayout->SetVerticalExternalMargin(0);
    m_hlayout->SetContentDistribution(eStackLeft);
    SetCompositionLayout(m_hlayout);
  }

  MenuBar::~MenuBar()
  {
    std::list< MenuBarItem * >::iterator it;
    for (it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
    {
      (*it)->UnReference();
    }
    m_MenuBarItemList.clear();
  }

  void MenuBar::Draw(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    Geometry base = GetGeometry();
    graphics_engine.PushClippingRectangle(base);

    Geometry item_geometry;
    std::list< MenuBarItem * >::iterator it;

    for (it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
    {
      BasicView *area = (*it)->area;
      item_geometry = area->GetGeometry();

      if (area->IsMouseInside())
      {

        GetPainter().PaintBackground(graphics_engine, item_geometry);

        if (!m_MenuIsActive)
        {
          GetPainter().Paint2DQuadColor(graphics_engine, item_geometry, Color(0xFF000000));
          //GetPainter().PaintShape(graphics_engine, item_geometry, Color(0xFF000000), eSHAPE_CORNER_ROUND2);
        }
        else
        {
          GetPainter().Paint2DQuadColor(graphics_engine, item_geometry, Color(0xFF000000));
          //GetPainter().PaintShapeCorner(graphics_engine, item_geometry, Color(0xFF000000), eSHAPE_CORNER_ROUND2,
          //eCornerTopLeft|eCornerTopRight, false);
        }

        if ((*it)->icon)
        {
          graphics_engine.GetRenderStates().SetBlend(TRUE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
          graphics_engine.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
          GetPainter().Draw2DTexture(graphics_engine, (*it)->icon, item_geometry.x, item_geometry.y);
          graphics_engine.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
          graphics_engine.GetRenderStates().SetBlend(GL_FALSE);
        }
        else
        {
          GetPainter().PaintTextLineStatic(graphics_engine, GetFont(), item_geometry, area->GetBaseString(), GetTextColor(), true, eAlignTextCenter);
        }
      }
      else
      {
        GetPainter().PaintBackground(graphics_engine, item_geometry);

        if ((*it)->icon)
        {
          graphics_engine.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
          graphics_engine.GetRenderStates().SetBlend(TRUE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
          GetPainter().Draw2DTexture(graphics_engine, (*it)->icon, item_geometry.x, item_geometry.y);
          graphics_engine.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
          graphics_engine.GetRenderStates().SetBlend(GL_FALSE);
        }
        else
        {
          GetPainter().PaintTextLineStatic(graphics_engine, GetFont(), item_geometry, area->GetBaseString(), GetTextColor(), true, eAlignTextCenter);
        }
      }
    }

    if (m_MenuIsActive)
    {
      BasicView *area = m_CurrentMenu->area;
      item_geometry = area->GetGeometry();
      GetPainter().PaintBackground(graphics_engine, item_geometry);
      GetPainter().Paint2DQuadColor(graphics_engine, item_geometry, Color(0xFF000000));
      //GetPainter().PaintShapeCorner(graphics_engine, item_geometry, Color(0xFF000000), eSHAPE_CORNER_ROUND2, eCornerTopLeft|eCornerTopRight, true);

      if (m_CurrentMenu->icon)
      {
        graphics_engine.GetRenderStates().SetBlend(TRUE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        graphics_engine.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        GetPainter().Draw2DTexture(graphics_engine, m_CurrentMenu->icon, item_geometry.x, item_geometry.y);
        graphics_engine.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        graphics_engine.GetRenderStates().SetBlend(GL_FALSE);
      }
      else
      {
        GetPainter().PaintTextLineStatic(graphics_engine, GetFont(), item_geometry, area->GetBaseString(), GetTextColor(), true, eAlignTextCenter);
      }
    }

    graphics_engine.PopClippingRectangle();
  }

  void MenuBar::DrawContent(GraphicsEngine &graphics_engine, bool /* force_draw */)
  {
    graphics_engine.PushClippingRectangle(GetGeometry());
    graphics_engine.PopClippingRectangle();
  }

  void MenuBar::AddMenu(const char *MenuLabel, MenuPage *menu)
  {
    NUX_RETURN_IF_NULL(menu);
    AddMenu(MenuLabel, menu, 0);
  }

  void MenuBar::AddMenu(const char *MenuLabel, MenuPage *menu, BaseTexture *icon)
  {
    NUX_RETURN_IF_NULL(menu);

    // MenuBarItem inherits from Object: no need to Sink the reference.
    MenuBarItem *menubar_item(new MenuBarItem(NUX_TRACKER_LOCATION));

    menu->m_IsTopOfMenuChain = true;
    menubar_item->area->SetBaseString(MenuLabel);
    menubar_item->menu = menu;
    if (menubar_item->menu)
      menubar_item->menu->Reference();

    menubar_item->icon = icon;
    if (menubar_item->icon)
      menubar_item->icon->Reference();

    m_MenuBarItemList.push_back(menubar_item);

    //menubar_item->area->SetMinimumSize(DEFAULT_WIDGET_WIDTH, 40);
    if (!icon)
    {
      menubar_item->area->SetMinimumSize(Max(MENU_MINIMUM_WIDTH, (unsigned int) (10 + GetFont()->GetStringWidth(MenuLabel))), Max(MENU_MINIMUM_WIDTH, (unsigned int) 16));
    }
    else
    {
      menubar_item->area->SetMinMaxSize(MENUBAR_ICON_WIDTH, MENUBAR_ICON_HEIGHT);
    }

    menubar_item->area->mouse_enter.connect(sigc::bind(sigc::mem_fun(this, &MenuBar::EmitItemMouseEnter), menubar_item));
    menubar_item->area->mouse_leave.connect(sigc::bind(sigc::mem_fun(this, &MenuBar::EmitItemMouseLeave), menubar_item));
    menubar_item->area->mouse_down.connect(sigc::bind(sigc::mem_fun(this, &MenuBar::EmitItemMouseDown), menubar_item));
    menubar_item->area->mouse_drag.connect(sigc::bind(sigc::mem_fun(this, &MenuBar::RecvItemMouseDrag), menubar_item));
    menubar_item->area->mouse_up.connect(sigc::bind(sigc::mem_fun(this, &MenuBar::EmitItemMouseUp), menubar_item));

    menubar_item->menu->SetParentMenu(0);
    menubar_item->menu->sigActionTriggered.connect(sigc::mem_fun(this, &MenuBar::RecvSigActionTriggered));
    menubar_item->menu->sigTerminateMenuCascade.connect(sigc::mem_fun(this, &MenuBar::RecvSigTerminateMenuCascade));
    menubar_item->menu->sigMouseDownOutsideMenuCascade.connect(sigc::mem_fun(this, &MenuBar::RecvSigMouseDownOutsideMenuCascade));

    m_hlayout->AddView(menubar_item->area, 0, eCenter);
    GetWindowThread()->ComputeElementLayout(m_hlayout);
  }

  void MenuBar::EmitItemMouseEnter(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */, MenuBarItem *menubar_item)
  {
    if (m_MenuIsActive)
    {
      if (m_CurrentMenu && (m_CurrentMenu->menu != menubar_item->menu))
        m_CurrentMenu->menu->StopMenu(0, 0);

      menubar_item->menu->m_MenuWindow = m_MenuBarWindow;
      menubar_item->menu->StartMenu(menubar_item->area->GetBaseX(),
                                     menubar_item->area->GetBaseY() + menubar_item->area->GetBaseHeight(), 0, 0);

      m_CurrentMenu = menubar_item;

      m_IsOpeningMenu = true;
    }

    QueueDraw();
  }

  void MenuBar::EmitItemMouseLeave(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */, MenuBarItem * /* menubar_item */)
  {
    QueueDraw();
  }
  void MenuBar::EmitItemMouseDown(int /* x */, int /* y */, unsigned long /* button_flags */, unsigned long /* key_flags */, MenuBarItem *menubar_item)
  {
    m_MenuBarWindow = GetWindowThread()->GetWindowCompositor().GetProcessingTopView();

    if (m_MenuIsActive == false)
    {
      // Open the corresponding MenuPage
      if (m_CurrentMenu)
      {
        // This should never happen
        nuxAssert(0);
        m_CurrentMenu->menu->StopMenu();
      }

      m_MenuIsActive = true;
      m_CurrentMenu = menubar_item;
      m_CurrentMenu->menu->m_MenuWindow = m_MenuBarWindow;
      m_IsOpeningMenu = true;

      m_CurrentMenu->menu->StartMenu(menubar_item->area->GetBaseX(),
                                      menubar_item->area->GetBaseY() + menubar_item->area->GetBaseHeight(), 0, 0);
    }
    else
    {
      // If the mouse up that follows happen inside the area, then it is going to close the menu.
      m_IsOpeningMenu = false;
    }

    QueueDraw();
  }

  void MenuBar::EmitItemMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags, MenuBarItem * /* menubar_item */)
  {
    if (m_MenuIsActive)
    {
      if (m_CurrentMenu->area->IsMouseInside())
      {
        if (m_IsOpeningMenu == false)
        {
          // close the MenuPage that is Open
          if (m_CurrentMenu)
            m_CurrentMenu->menu->StopMenu(0, 0);

          m_MenuIsActive = false;
          m_CurrentMenu = 0;
        }
        else
        {
          // The MousePress before this MouseRelease, caused the MenuPage to open.
          // Set m_IsOpeningMenu so the next mouse release will close the menu.
          m_IsOpeningMenu = false;
        }
      }
      else
      {
        bool hit_inside_a_menu = false;
        bool b = m_CurrentMenu->menu->TestMouseUp(x, y, button_flags, key_flags, hit_inside_a_menu);

        if (b || (hit_inside_a_menu == false))
        {
          RecvSigTerminateMenuCascade();
        }
      }
    }

    QueueDraw();
  }

  void MenuBar::RecvItemMouseDrag(int /* x */, int /* y */, int /* dx */, int /* dy */, unsigned long /* button_flags */, unsigned long /* key_flags */, MenuBarItem * /* menubar_item */)
  {
    // TODO: Port to new event architecture
//     // Transition between one menu bar item to another
//     if (GetWindowThread()->GetWindowCompositor().GetMouseFocusArea() == menubar_item->area)
//     {
//       if (!menubar_item->area->IsMouseInside()) // can also test GetWindowThread()->GetWindowCompositor().GetMouseOverArea() != &menubar_item->area
//       {
//         std::list< MenuBarItem * >::iterator it;
//         // compute window coordinates x and y;
//         int winx = menubar_item->area->GetBaseX() + x;
//         int winy = menubar_item->area->GetBaseY() + y;
// 
//         for (it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
//         {
//           BasicView *area = (*it)->area;
//           Geometry geometry = area->GetGeometry();
// 
//           if (geometry.IsPointInside(winx, winy))
//           {
//             // Close the menu below menubar_item(the one that has the focus
//             menubar_item->area->ForceStopFocus(0, 0);
// 
//             // EmitItemMouseEnter is going to open the menu below(*it)
//             {
//               EmitItemMouseEnter(winx, winy, button_flags, key_flags, (*it));
//               m_IsOpeningMenu = true;
//               area->ForceStartFocus(0, 0);
// 
//               GetWindowThread()->GetWindowCompositor().SetMouseFocusArea(area);
//               GetWindowThread()->GetWindowCompositor().SetMouseOverArea(area);
//             }
// 
//             break;
//           }
//         }
//       }
//     }
  }

  void MenuBar::RecvSigActionTriggered(MenuPage * /* menu */, ActionItem * /* action */)
  {
    m_MenuIsActive = false;

    if (m_CurrentMenu)
    {
      m_CurrentMenu->menu->StopMenu();
      QueueDraw();
    }

    m_CurrentMenu = 0;
    m_IsOpeningMenu = false;

    // You can do something if you want with the menu* and the action*
  }


  void MenuBar::RecvSigTerminateMenuCascade()
  {
    m_MenuIsActive = false;

    if (m_CurrentMenu)
    {
      m_CurrentMenu->menu->StopMenu();
    }

    m_CurrentMenu = 0;
    m_IsOpeningMenu = false;

    QueueDraw();
  }

  void MenuBar::RecvSigMouseDownOutsideMenuCascade(MenuPage * /* menu */, int x, int y)
  {
    Geometry geometry;
    std::list< MenuBarItem * >::iterator it;
    bool TerminateMenuCascade = 1;

    for (it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
    {
      BasicView *area = (*it)->area;
      geometry = area->GetGeometry();

      if (geometry.IsPointInside(x, y))
      {
        // The event landed on one of the MenuBar item.
        // Do nothing. This will be handled in the ProcessEvent of the MenuBar item where the mouse down landed.
        TerminateMenuCascade = 0;
        break;
      }
    }

    if (TerminateMenuCascade)
      RecvSigTerminateMenuCascade();
  }


}
