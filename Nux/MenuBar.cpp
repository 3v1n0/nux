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
#include "WindowCompositor.h"
#include "HLayout.h"
#include "MenuBar.h"

NAMESPACE_BEGIN_GUI

static const UINT MENU_MINIMUM_WIDTH = 10;
static const UINT MENU_MINIMUM_HEIGHT = 16;

static const UINT MENUBAR_ICON_WIDTH = 24;
static const UINT MENUBAR_ICON_HEIGHT = 24;

IMPLEMENT_ROOT_OBJECT_TYPE(MenuBarItem);
IMPLEMENT_OBJECT_TYPE(MenuBar);

MenuBarItem::MenuBarItem()
{
    area = smptr(CoreArea)(new CoreArea());
    icon = 0;
}

MenuBarItem::~MenuBarItem()
{
    menu = smptr(MenuPage)(0);
    area = smptr(CoreArea)(0);
}

MenuBar::MenuBar()
:   m_MenuIsActive(false)
//,   m_CurrentMenu(0)
,   m_IsOpeningMenu(false)
,   m_MenuBarWindow(0)
{ 
    m_hlayout = smptr(HLayout)(new HLayout());
    m_hlayout->SetHorizontalInternalMargin(4);
    m_hlayout->SetHorizontalExternalMargin(2);

    SetMinimumSize(24, 24);
    SetMaximumSize(BASEOBJECT_MAXWIDTH, 24);
    setGeometry(Geometry(0, 0, 200, 20));

    m_hlayout->SetHorizontalInternalMargin(4);
    m_hlayout->SetVerticalExternalMargin(0);
    m_hlayout->SetContentStacking(eStackLeft);
    SetCompositionLayout(m_hlayout);
}

MenuBar::~MenuBar()
{
    if(m_CurrentMenu.IsValid())
    {
//         m_CurrentMenu->area->OnMouseEnter.clear();
//         m_CurrentMenu->area->OnMouseEnter.clear();
//         m_CurrentMenu->area->OnMouseLeave.clear();
//         m_CurrentMenu->area->OnMouseDown.clear();
//         m_CurrentMenu->area->OnMouseDrag.clear();
//         m_CurrentMenu->area->OnMouseUp.clear();
    }

    std::list< smptr(MenuBarItem) >::iterator it;
    for(it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
    {
//         (*it)->area->OnMouseEnter.clear();
//         (*it)->area->OnMouseEnter.clear();
//         (*it)->area->OnMouseLeave.clear();
//         (*it)->area->OnMouseDown.clear();
//         (*it)->area->OnMouseDrag.clear();
//         (*it)->area->OnMouseUp.clear();

        (*it) = smptr(MenuBarItem)(0);
    }
    m_MenuBarItemList.clear();
}

long MenuBar::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    long ret = TraverseInfo;
    long ProcEvInfo = 0;

    ret = TraverseInfo;  // <<---- never forget this

    std::list< smptr(MenuBarItem) >::iterator it;
    
    for(it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
    {
        (*it)->menu->m_MenuWindow = m_MenuBarWindow;
        ret = (*it)->area->OnEvent(ievent, ret, ProcessEventInfo);
    }

    if(ievent.e_event == NUX_MOUSE_PRESSED)
    {
        bool mouse_down_on_menu_item = false;
        if(m_MenuIsActive == true)
        {
            for(it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
            {
                if((*it)->area->IsMouseInside())
                {
                    mouse_down_on_menu_item = true;
                    break;
                }
            }
            if(mouse_down_on_menu_item == false)
            {
                if(m_CurrentMenu->menu->TestMouseDown() == false)
                {
                    RecvSigTerminateMenuCascade();
                }
            }
        }
    }
    

    // PostProcessEvent2 must always have its last parameter set to 0
    // because the m_BackgroundArea is the real physical limit of the window.
    // So the previous test about IsPointInside do not prevail over m_BackgroundArea 
    // testing the event by itself.
    ret = PostProcessEvent2(ievent, ret, 0);
    return ret;
}

void MenuBar::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    GfxContext.PushClippingRectangle(base);

    Geometry item_geometry;
    std::list< smptr(MenuBarItem) >::iterator it;
    for(it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
    {
        smptr(CoreArea) area = (*it)->area;
        item_geometry = area->GetGeometry();
        if(area->IsMouseInside())
        {
            
            gPainter.PaintBackground(GfxContext, item_geometry);
            if(!m_MenuIsActive)
            {
                gPainter.Paint2DQuadColor(GfxContext, item_geometry, Color(0xFF000000));
                //gPainter.PaintShape(GfxContext, item_geometry, Color(0xFF000000), eSHAPE_CORNER_ROUND2);
            }
            else
            {
                gPainter.Paint2DQuadColor(GfxContext, item_geometry, Color(0xFF000000));
                //gPainter.PaintShapeCorner(GfxContext, item_geometry, Color(0xFF000000), eSHAPE_CORNER_ROUND2,
                //eCornerTopLeft|eCornerTopRight, false);
            }

            if((*it)->icon)
            {
                GfxContext.GetRenderStates().SetBlend(TRUE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                GfxContext.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                gPainter.Draw2DTexture(GfxContext, (*it)->icon, item_geometry.x, item_geometry.y);
                GfxContext.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                GfxContext.GetRenderStates().SetBlend(GL_FALSE);
            }
            else
            {
                gPainter.PaintTextLineStatic(GfxContext, GetFont(), item_geometry, area->GetBaseString().GetTCharPtr(), GetTextColor(), true, eAlignTextCenter);
            }
        }
        else
        {
            gPainter.PaintBackground(GfxContext, item_geometry);

            if((*it)->icon)
            {
                GfxContext.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                GfxContext.GetRenderStates().SetBlend(TRUE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
                gPainter.Draw2DTexture(GfxContext, (*it)->icon, item_geometry.x, item_geometry.y);
                GfxContext.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
                GfxContext.GetRenderStates().SetBlend(GL_FALSE);
            }
            else
            {
                gPainter.PaintTextLineStatic(GfxContext, GetFont(), item_geometry, area->GetBaseString().GetTCharPtr(), GetTextColor(), true, eAlignTextCenter);
            }
        }
    }
    if(m_MenuIsActive)
    {
        smptr(CoreArea) area(m_CurrentMenu->area);
        item_geometry = area->GetGeometry();
        gPainter.PaintBackground(GfxContext, item_geometry);
        gPainter.Paint2DQuadColor(GfxContext, item_geometry, Color(0xFF000000));
        //gPainter.PaintShapeCorner(GfxContext, item_geometry, Color(0xFF000000), eSHAPE_CORNER_ROUND2, eCornerTopLeft|eCornerTopRight, true);

        if(m_CurrentMenu->icon)
        {
            GfxContext.GetRenderStates().SetBlend(TRUE, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
            GfxContext.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            gPainter.Draw2DTexture(GfxContext, m_CurrentMenu->icon, item_geometry.x, item_geometry.y);
            GfxContext.GetRenderStates().SetColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
            GfxContext.GetRenderStates().SetBlend(GL_FALSE);
        }
        else
        {
            gPainter.PaintTextLineStatic(GfxContext, GetFont(), item_geometry, area->GetBaseString().GetTCharPtr(), GetTextColor(), true, eAlignTextCenter);
        }
    }
    GfxContext.PopClippingRectangle();
}

void MenuBar::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{
    GfxContext.PushClippingRectangle(GetGeometry());
    GfxContext.PopClippingRectangle();
}

void MenuBar::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

void MenuBar::AddMenu(const TCHAR* MenuLabel, smptr(MenuPage) menu)
{
    AddMenu(MenuLabel, menu, 0);
}

void MenuBar::AddMenu(const TCHAR* MenuLabel, smptr(MenuPage) menu, NTexture *icon)
{
    // Need to add NUX_RETURN_IF_TRUE, NUX_RETURN_IF_FALSE
    NUX_RETURN_IF_TRUE(menu.IsNull());

    smptr(MenuBarItem) menubar_item(new MenuBarItem());

    menu->m_IsTopOfMenuChain = true;
    menubar_item->area->SetBaseString(MenuLabel);
    menubar_item->menu = menu;
    menubar_item->icon = icon;

    m_MenuBarItemList.push_back(menubar_item);
    //menubar_item->area->SetMinimumSize(DEFAULT_WIDGET_WIDTH, 40);
    if(!icon)
    {
        menubar_item->area->SetMinimumSize(Max(MENU_MINIMUM_WIDTH, (UINT)(10 + GetFont()->GetStringWidth(MenuLabel))), Max(MENU_MINIMUM_WIDTH, (UINT)16));
    }
    else
    {
        menubar_item->area->SetMinMaxSize(MENUBAR_ICON_WIDTH, MENUBAR_ICON_HEIGHT);
    }

    menubar_item->area->OnMouseEnter.connect(sigc::bind( sigc::mem_fun(this, &MenuBar::EmitItemMouseEnter), weaksmptr(MenuBarItem)(menubar_item)));
    menubar_item->area->OnMouseLeave.connect(sigc::bind( sigc::mem_fun(this, &MenuBar::EmitItemMouseLeave), weaksmptr(MenuBarItem)(menubar_item)));
    menubar_item->area->OnMouseDown.connect(sigc::bind( sigc::mem_fun(this, &MenuBar::EmitItemMouseDown), weaksmptr(MenuBarItem)(menubar_item)));
    menubar_item->area->OnMouseDrag.connect(sigc::bind( sigc::mem_fun(this, &MenuBar::RecvItemMouseDrag), weaksmptr(MenuBarItem)(menubar_item)));
    menubar_item->area->OnMouseUp.connect(sigc::bind( sigc::mem_fun(this, &MenuBar::EmitItemMouseUp), weaksmptr(MenuBarItem)(menubar_item)));

    menubar_item->menu->SetParentMenu(smptr(MenuPage)(0));
    menubar_item->menu->sigActionTriggered.connect(sigc::mem_fun(this, &MenuBar::RecvSigActionTriggered));
    menubar_item->menu->sigTerminateMenuCascade.connect(sigc::mem_fun(this, &MenuBar::RecvSigTerminateMenuCascade));
    menubar_item->menu->sigMouseDownOutsideMenuCascade.connect(sigc::mem_fun(this, &MenuBar::RecvSigMouseDownOutsideMenuCascade));

    m_hlayout->AddActiveInterfaceObject(menubar_item->area, 0, eCenter);
    GetGraphicsThread()->ComputeElementLayout(m_hlayout);
}

void MenuBar::EmitItemMouseEnter(int x, int y, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item)
{
    if(m_MenuIsActive)
    {
        if(m_CurrentMenu.IsValid() && (m_CurrentMenu->menu != menubar_item->menu))
            m_CurrentMenu->menu->StopMenu(0, 0);

        Geometry geo = menubar_item->menu->GetGeometry();
        menubar_item->menu->m_MenuWindow = m_MenuBarWindow;
        menubar_item->menu->StartMenu(menubar_item->area->GetBaseX(), 
            menubar_item->area->GetBaseY() + menubar_item->area->GetBaseHeight(), 0, 0);

        m_CurrentMenu = menubar_item;

        m_IsOpeningMenu = true;
    }
    NeedRedraw();
}

void MenuBar::EmitItemMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item)
{
    NeedRedraw();
}
void MenuBar::EmitItemMouseDown(int x, int y, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item)
{
    m_MenuBarWindow = GetThreadWindowCompositor().GetCurrentWindow();
    if(m_MenuIsActive == false)
    {
        // Open the corresponding MenuPage
        if(m_CurrentMenu.IsValid())
        {
            // This should never happen
            nuxAssert(0);
            m_CurrentMenu->menu->StopMenu();
        }

        m_MenuIsActive = true;
        m_CurrentMenu = menubar_item;
        m_CurrentMenu->menu->m_MenuWindow = m_MenuBarWindow;
        m_IsOpeningMenu = true;

        //m_CurrentMenu->area->ForceStopFocus(0, 0);
        m_CurrentMenu->menu->StartMenu(menubar_item->area->GetBaseX(), 
            menubar_item->area->GetBaseY() + menubar_item->area->GetBaseHeight(), 0, 0);
    }
    else
    {
        // If the mouse up that follows happen inside the area, then it is going to close the menu.
        m_IsOpeningMenu = false;
    }
    NeedRedraw();
}

void MenuBar::EmitItemMouseUp(int x, int y, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item)
{
    if(m_MenuIsActive)
    {
        if(m_CurrentMenu->area->IsMouseInside())
        {
            if(m_IsOpeningMenu == false)
            {
                // close the MenuPage that is Open
                if(m_CurrentMenu.IsValid())
                    m_CurrentMenu->menu->StopMenu(0, 0);
                m_MenuIsActive = false;
                m_CurrentMenu = smptr(MenuBarItem)(0);
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
            if(b || (hit_inside_a_menu == false))
            {
                RecvSigTerminateMenuCascade();
            }
        }
    }
    NeedRedraw();
}

void MenuBar::RecvItemMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags, weaksmptr(MenuBarItem) menubar_item)
{
    // Transition between one menu bar item to another
    if(GetThreadWindowCompositor().GetMouseFocusArea() == menubar_item->area)
    {
        if(!menubar_item->area->IsMouseInside())  // can also test GetThreadWindowCompositor().GetMouseOverArea() != &menubar_item->area
        {
            &menubar_item->area;
            std::list< smptr(MenuBarItem) >::iterator it;
            // compute window coordinates x and y;
            int winx = menubar_item->area->GetBaseX() + x; 
            int winy = menubar_item->area->GetBaseY() + y;
            for(it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
            {
                smptr(CoreArea) area = (*it)->area;
                Geometry geometry = area->GetGeometry();
                if(geometry.IsPointInside(winx, winy))
                {
                    // Close the menu below menubar_item (the one that has the focus
                    menubar_item->area->ForceStopFocus(0, 0);

                    // EmitItemMouseEnter is going to open the menu below (*it)
                    {
                        EmitItemMouseEnter(winx, winy, button_flags, key_flags, (*it));
                        m_IsOpeningMenu = true;
                        area->ForceStartFocus(0, 0);

                        GetThreadWindowCompositor().SetMouseFocusArea(area);
                        GetThreadWindowCompositor().SetMouseOverArea(area);
                    }
                    
                    break;
                }
            }
        }
    }
}

void MenuBar::RecvSigActionTriggered(smptr(MenuPage) menu, const smptr(ActionItem) action)
{
    m_MenuIsActive = false;
    if(m_CurrentMenu.IsValid())
    {
        m_CurrentMenu->menu->StopMenu();
        NeedRedraw();
    }
    m_CurrentMenu = smptr(MenuBarItem)(0);
    m_IsOpeningMenu = false;

    // You can do something if you want with the menu* and the action*
}


void MenuBar::RecvSigTerminateMenuCascade()
{
    m_MenuIsActive = false;
    if(m_CurrentMenu.IsValid())
    {
        m_CurrentMenu->menu->StopMenu();
    }
    m_CurrentMenu = smptr(MenuBarItem)(0);
    m_IsOpeningMenu = false;

    NeedRedraw();
}

void MenuBar::RecvSigMouseDownOutsideMenuCascade(smptr(MenuPage) menu, int x, int y)
{
    Geometry geometry;
    std::list< smptr(MenuBarItem) >::iterator it;
    UBOOL TerminateMenuCascade = 1;
    for(it = m_MenuBarItemList.begin(); it != m_MenuBarItemList.end(); it++)
    {
        smptr(CoreArea) area = (*it)->area;
        geometry = area->GetGeometry();
        if(geometry.IsPointInside(x, y))
        {
            // The event landed on one of the MenuBar item.
            // Do nothing. This will be handled in the ProcessEvent of the MenuBar item where the mouse down landed.
            TerminateMenuCascade = 0;
            break;
        }
    }
    if(TerminateMenuCascade)
        RecvSigTerminateMenuCascade();
}


NAMESPACE_END_GUI
