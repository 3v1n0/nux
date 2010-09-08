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

#include "MenuPage.h"
#include "WindowCompositor.h"
#include "ActionItem.h"
#include "VLayout.h"

NAMESPACE_BEGIN_GUI

IMPLEMENT_OBJECT_TYPE(MenuItem);
IMPLEMENT_OBJECT_TYPE(MenuSeparator);
IMPLEMENT_OBJECT_TYPE(MenuPage);

static const int MENU_ICONE_WIDTH = 18;
static const int MENU_ICON_HEIGHT = 18;
static const int MENU_ITEM_MIN_WIDTH = 90;
static const int MENU_ITEM_MIN_HEIGHT = 20;
static const int MENU_ITEM_BORDER_TO_ICON_MARGIN = 5;
static const int MENU_ITEM_ICON_TO_TEXT_MARGIN = 5;
static const int MENU_ITEM_TEXT_TO_BORDER_MARGIN = 5;

// Algorithm:
// To initiate the menu, the mouse has to hit one of the menu bar item (they hold the name of the menu that are displayed).
// When that happens, the menu attached to the MenuBarItem gets the focus and becomes visible. Before the mouse is released, 
// the flag m_NextMouseUpMeanStop is set to FALSE, meaning that the following mouse up will not be processed by the menu, even if the released happened 
// outside of any of the menu forming the menu cascade. This way, the menu will remain open. There are exceptions to this however.
// After the firs mouse down on the MenuBarItem and before the mouse up event, if the mouse over the the opened menu,
// then m_NextMouseUpMeanStop is set to TRUE. If the mouse is released over an active menu item, the menu cascade will close itself.
// If the mouse is released outside any of the menu forming the menu cascade, the menu will close itself.
//
//


MenuItem::MenuItem(const TCHAR* label, int UserValue)
:   m_ChildMenu(0)
,   m_ActionItem(0)
{
    m_ActionItem = smptr(ActionItem)(new ActionItem(label, UserValue));
}

MenuItem::~MenuItem()
{
}

void MenuItem::SetChildMenu(smptr(MenuPage) menu)
{
    //nuxAssert(menu)
    m_ChildMenu = menu;
}

smptr(MenuPage) MenuItem::GetChildMenu() const
{
    //return m_ActionItem.GetMenu();
    return m_ChildMenu;
}

void MenuItem::SetActionItem(smptr(ActionItem) action)
{
    nuxAssertMsg(action != 0, TEXT("[MenuItem::SetActionItem] Parameter is Null."));
    if(action == 0)
        return;
    m_ActionItem = action;
}

const smptr(ActionItem) MenuItem::GetActionItem() const
{
    return m_ActionItem;
}

//smptr(ActionItem) MenuItem::GetActionItem()
//{
//    return &m_ActionItem;
//}

long MenuItem::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    return PostProcessEvent2(ievent, TraverseInfo, ProcessEventInfo);
}

void MenuItem::Draw(GraphicsContext& GfxContext, bool force_draw)
{

}

void MenuItem::DrawAsMenuItem(GraphicsContext& GfxContext, const Color& textcolor, bool is_highlighted, bool isFirstItem, bool isLastItem, bool draw_icone)
{
    Geometry geo = GetGeometry();
    Geometry icon_geo(0, 0, 20, 20);
    Geometry text_geo = geo;

    text_geo.OffsetPosition(24, 0);
    text_geo.OffsetSize(2 * 24, 0);

    icon_geo.SetX(geo.x+ 2);
    icon_geo.SetY(geo.y+ 0);

    const TCHAR* label = m_ActionItem->GetLabel();

    if(is_highlighted)
    {
        /*
            if(isFirstItem && isLastItem)
                gPainter.PaintShape(GfxContext, geo, COLOR_FOREGROUND_SECONDARY, eSHAPE_CORNER_ROUND4);
            else if(isFirstItem)
                gPainter.PaintShapeCorner(GfxContext, geo, COLOR_FOREGROUND_SECONDARY, eSHAPE_CORNER_ROUND4, eCornerTopLeft | eCornerTopRight);
            else if(isLastItem)
                gPainter.PaintShapeCorner(GfxContext, geo, COLOR_FOREGROUND_SECONDARY, eSHAPE_CORNER_ROUND4, eCornerBottomLeft | eCornerBottomRight);
            else
                gPainter.Paint2DQuadColor(GfxContext, geo, COLOR_FOREGROUND_SECONDARY);
        */
        gPainter.Paint2DQuadColor(GfxContext, geo, Color(0xFF32DE01) /*COLOR_FOREGROUND_SECONDARY*/);
    }
    else
    {
        //gPainter.Paint2DQuadColor(GfxContext, geo, Color(0xFF868686));
    }

    //if(m_Icon)
    {
        //gPainter.Draw2DTextureAligned(GfxContext, &m_ActionItem->GetIcon(), icon_geo, TextureAlignmentStyle(eTACenter, eTACenter));
    }

    if(label)
        gPainter.PaintTextLineStatic(GfxContext, GetFont(), text_geo, std::string(label), textcolor, eAlignTextLeft);
}

MenuSeparator::MenuSeparator()
{

}

MenuSeparator::~MenuSeparator()
{

}

long MenuSeparator::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{
    return PostProcessEvent2(ievent, TraverseInfo, ProcessEventInfo);
}

void MenuSeparator::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    Geometry base = GetGeometry();
    int y0 = base.y + base.GetHeight()/2;
    gPainter.Draw2DLine(GfxContext, base.x, y0, base.x + base.GetWidth(), y0, Color(0xFF222222));
    gPainter.Draw2DLine(GfxContext, base.x, y0+1, base.x + base.GetWidth(), y0+1, Color(0xFFAAAAAA));
}

MenuPage::MenuPage(const TCHAR* title)
:   m_Parent(0)
,   m_item_width(MENU_ITEM_MIN_WIDTH)
,   m_item_height(MENU_ITEM_MIN_HEIGHT)
,   m_show_item_icon(true)
,   m_MenuWindow(0)
,   m_Name(title)
,   m_IsTopOfMenuChain(false)
{
    // Set Original State

    // Set Signals
    OnMouseMove.connect(sigc::mem_fun(this, &MenuPage::EmitMouseMove));
    OnMouseDrag.connect(sigc::mem_fun(this, &MenuPage::EmitMouseDrag));
    OnMouseDown.connect(sigc::mem_fun(this, &MenuPage::EmitMouseDown));
    OnMouseUp.connect(sigc::mem_fun(this, &MenuPage::EmitMouseUp));
    OnMouseLeave.connect(sigc::mem_fun(this, &MenuPage::RecvMouseLeave));
    OnCaptureMouseDownAnyWhereElse.connect(sigc::mem_fun(this, &MenuPage::Terminate));

    // Set Geometry
    setGeometry(Geometry(0, 0, DEFAULT_WIDGET_WIDTH, DEFAULT_WIDGET_HEIGHT));

    // Set layout

    m_numItem = 0;
    m_HighlightedItem = -1;
    m_IsActive = false;
    m_NextMouseUpMeanStop = false;
    m_SubMenuAction = smptr(MenuItem)(0);

    vlayout = smptr(VLayout)(new VLayout());
    // No Need to set a composition layout.
    // The MenuPage is floating above everything else.
    SetCompositionLayout(vlayout);

    SetTextColor(Color::Black);
}

MenuPage::~MenuPage()
{
    std::vector< smptr(MenuItem) >::iterator it;
    for(it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
    {
        //delete (*it);
    }
    m_MenuItemVector.clear();

    std::vector< smptr(MenuSeparator) >::iterator it2;
    for(it2 = m_MenuSeparatorVector.begin(); it2 != m_MenuSeparatorVector.end(); it2++)
    {
        //delete (*it2);
    }
    m_MenuSeparatorVector.clear();
}

//void MenuPage::SetName(const TCHAR* name)
//{
//    m_Name = name;
//}
//
const TCHAR* MenuPage::GetName() const
{
    return m_Name.GetTCharPtr();
}

bool MenuPage::CanClose() const
{
    return m_Action_Triggered;
}

long MenuPage::ProcessEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo) 
{
    long ret = TraverseInfo;
    if(m_IsActive)
    {
        if(ievent.e_event == NUX_MOUSE_RELEASED)
        {
            Geometry geo = GetThreadGLWindow()->GetWindowGeometry();
            geo.SetX(0); geo.SetY(0);
            if(!geo.IsPointInside(ievent.e_x, ievent.e_y) /*ievent.e_x < 0 || ievent.e_y < 0*/)
            {
                // the event happened outside the window.
                NotifyTerminateMenuCascade();
            }
            else
            {
                EmitMouseUp(ievent.e_x - GetBaseX(), ievent.e_y - GetBaseY(), ievent.event_mouse_state(), ievent.event_key_state());
            }
        }
        else if(ievent.e_event == NUX_MOUSE_PRESSED)
        {
            Geometry geo = GetThreadGLWindow()->GetWindowGeometry();
            geo.SetX(0); geo.SetY(0);
            if(!geo.IsPointInside(ievent.e_x, ievent.e_y) /*ievent.e_x < 0 || ievent.e_y < 0*/)
            {
                // the event happened outside the window.
                NotifyTerminateMenuCascade();
            }
            else
            {
                ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
            }
        }
        else if(ievent.e_event == NUX_WINDOW_CONFIGURATION)
        {
            NotifyTerminateMenuCascade();
        }
        else if(ievent.e_event == NUX_WINDOW_EXIT_FOCUS)
        {
            NotifyTerminateMenuCascade();
        }
        else
        {
            if(ievent.e_event == NUX_MOUSE_MOVE)
                NeedRedraw();

            ret = PostProcessEvent2(ievent, ret, ProcessEventInfo);
        }
    }
    if(GetGeometry().IsPointInside(ievent.event_x(), ievent.event_y()))
    {
        ret |= eMouseEventSolved;
    }
    return ret;
};

void MenuPage::Draw(GraphicsContext& GfxContext, bool force_draw)
{
    if(m_IsActive)
    {
        Geometry base = GetGeometry();
        Geometry shadow;
        shadow = base;
        shadow.OffsetPosition(4, 4);
        //gPainter.PaintShape(GfxContext, shadow, Color(0xFF000000), eSHAPE_CORNER_ROUND4_SHADOW);

        int window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
        int window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();

        int ctxX = GetGraphicsThread()->GetGraphicsContext().GetContextX();
        int ctxY = GetGraphicsThread()->GetGraphicsContext().GetContextY();

        TexCoordXForm texxform;
        texxform.uoffset = (float)base.x/(float)GetThreadWindowCompositor().GetScreenBlurTexture()->GetWidth();
        texxform.voffset = (float)base.y/(float)GetThreadWindowCompositor().GetScreenBlurTexture()->GetHeight();
        texxform.SetTexCoordType(TexCoordXForm::OFFSET_COORD);

        gPainter.PushDrawTextureLayer(GfxContext, base, GetThreadWindowCompositor().GetScreenBlurTexture(), texxform, Color::White, true);

        GfxContext.GetRenderStates().SetBlend(GL_TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gPainter.Paint2DQuadColor(GfxContext, base, Color(0xA0404040));
        GfxContext.GetRenderStates().SetBlend(GL_FALSE);

        Geometry text_area;
        text_area.SetX(base.x);
        text_area.SetY(base.y);
        text_area.SetWidth(base.GetWidth());
        text_area.SetHeight(PRACTICAL_WIDGET_HEIGHT);

        int i;
        std::vector< smptr(MenuItem) >::iterator it;
        int numItem = (int)m_MenuItemVector.size();
        for(it = m_MenuItemVector.begin(), i = 0; it != m_MenuItemVector.end(); it++, i++)
        {
            bool is_highlighted = (m_HighlightedItem == i);
            (*it)->DrawAsMenuItem(GfxContext, Color(0xFFFFFFFF)/*GetTextColor()*/, is_highlighted, i==0, i==(numItem-1), true);
        }

        std::vector< smptr(MenuSeparator) >::iterator separator_iterator;
        for(separator_iterator = m_MenuSeparatorVector.begin(); separator_iterator != m_MenuSeparatorVector.end(); separator_iterator++)
        {
            (*separator_iterator)->Draw(GfxContext, force_draw);
        }
    }
}

void MenuPage::DrawContent(GraphicsContext& GfxContext, bool force_draw)
{

}

void MenuPage::PostDraw(GraphicsContext& GfxContext, bool force_draw)
{

}

smptr(ActionItem) MenuPage::AddAction(const TCHAR* label, int UserValue)
{
    smptr(MenuItem) pMenuItem(new MenuItem(label, UserValue));

    m_MenuItemVector.push_back(pMenuItem);
    pMenuItem->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    int new_item_width = GetFont()->GetStringWidth(pMenuItem->GetActionItem()->GetLabel());
    if(new_item_width < m_item_width)
    {
        if(ShowItemIcon())
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
        if(ShowItemIcon())
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

        std::vector< smptr(MenuItem) >::iterator it;
        for(it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
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

    if(pMenuItem->GetChildMenu() != 0)
    {
        pMenuItem->GetChildMenu()->SetParentMenu(smptr(MenuPage)(this, false));
    }

    m_numItem = (int)m_MenuItemVector.size();
    vlayout->AddActiveInterfaceObject(pMenuItem, 0, eCenter, eFix);
    ComputeChildLayout();

    return pMenuItem->GetActionItem();
}

//void MenuPage::AddActionItem(smptr(ActionItem) actionItem)
//{
//    nuxAssertMsg(actionItem != 0, TEXT("[MenuPage::AddActionItem] Parameter is Null."));
//    if(actionItem == 0)
//        return;
//
//     smptr(MenuItem)  pMenuItem = new MenuItem(actionItem->GetLabel(), actionItem->GetUserValue());
//    pMenuItem->SetActionItem(actionItem);
//
//    m_MenuItemVector.push_back(pMenuItem);
//    pMenuItem->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);
//
//    int new_item_width = GetFont()->GetStringWidth(actionItem->GetLabel());
//    if(new_item_width < m_item_width)
//    {
//        if(ShowItemIcon())
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
//        if(ShowItemIcon())
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
//        std::vector< smptr(MenuItem) >::iterator it;
//        for(it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
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
//    if(pMenuItem->GetChildMenu() != 0)
//    {
//        pMenuItem->GetChildMenu()->SetParentMenu(this);
//    }
//
//    m_numItem = (int)m_MenuItemVector.size();
//    vlayout->AddActiveInterfaceObject(pMenuItem, 0, eCenter, eFix);
//    ComputeChildLayout();
//}

smptr(MenuPage) MenuPage::AddMenu(const TCHAR* label)
{
    smptr(MenuItem) pMenuItem(new MenuItem(label, 0));

    pMenuItem->SetChildMenu(smptr(MenuPage)(new MenuPage(label)));
    //pMenuItem->SetActionItem(new ActionItem());
    //pMenuItem->GetActionItem()->SetLabel(label);
    m_MenuItemVector.push_back(pMenuItem);
    pMenuItem->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    int new_item_width = GetFont()->GetStringWidth(label);
    if(new_item_width < m_item_width)
    {
        if(ShowItemIcon())
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
        if(ShowItemIcon())
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

        std::vector< smptr(MenuItem) >::iterator it;
        for(it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
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

    if(pMenuItem->GetChildMenu() != 0)
    {
        pMenuItem->GetChildMenu()->SetParentMenu(smptr(MenuPage)(this, false));
    }

    m_numItem = (int)m_MenuItemVector.size();
    vlayout->AddActiveInterfaceObject(pMenuItem, 0, eCenter, eFix);
    ComputeChildLayout();

    return pMenuItem->GetChildMenu();
}

smptr(ActionItem) MenuPage::AddSubMenu(const TCHAR* label, smptr(MenuPage) menu)
{
    menu->m_IsTopOfMenuChain = false;
    smptr(MenuItem) pMenuItem(new MenuItem(menu->GetName(), 0));

    pMenuItem->SetChildMenu(menu);
    m_MenuItemVector.push_back(pMenuItem);
    pMenuItem->SetMinimumSize(DEFAULT_WIDGET_WIDTH, PRACTICAL_WIDGET_HEIGHT);

    int new_item_width = GetFont()->GetStringWidth(label);
    if(new_item_width < m_item_width)
    {
        if(ShowItemIcon())
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
        if(ShowItemIcon())
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

        std::vector< smptr(MenuItem) >::iterator it;
        for(it = m_MenuItemVector.begin(); it != m_MenuItemVector.end(); it++)
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

    if(pMenuItem->GetChildMenu() != 0)
    {
        pMenuItem->GetChildMenu()->SetParentMenu(smptr(MenuPage)(this, false));
    }

    m_numItem = (int)m_MenuItemVector.size();
    vlayout->AddActiveInterfaceObject(pMenuItem, 0, eCenter, eFix);
    ComputeChildLayout();

    return pMenuItem->GetActionItem();
}

void MenuPage::AddSeparator()
{
    smptr(MenuSeparator) pMenuSeparator(new MenuSeparator());
    m_MenuSeparatorVector.push_back(pMenuSeparator);

    if(ShowItemIcon())
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

    vlayout->AddActiveInterfaceObject(pMenuSeparator, 0, eCenter, eFix);
    ComputeChildLayout();
}

void MenuPage::RemoveItem(smptr(ActionItem) item)
{
}

void MenuPage::RemoveAllItem()
{
    m_MenuSeparatorVector.clear();
    m_MenuItemVector.clear();
    vlayout->Clear();
    ComputeChildLayout();
}

void MenuPage::EmitMouseMove(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    if(IsMouseInside())
    {
        m_NextMouseUpMeanStop = true;
        // Find on which item the mouse is
        std::vector< smptr(MenuItem) >::iterator item_iterator;
        UINT i = 0;
        m_HighlightedItem = -1;
        for(item_iterator = m_MenuItemVector.begin(), i = 0; item_iterator != m_MenuItemVector.end(); item_iterator++, i++)
        {
            int MenuY = GetBaseY();
            int py = (*item_iterator)->GetBaseY() - MenuY;
            int height = (*item_iterator)->GetBaseHeight();
            if((y >= py) && (y < py + height))
            {
                m_HighlightedItem = i;
                break;
            }
        }
    }
    else
    {
        m_HighlightedItem = -1;
    }
    
    if(m_HighlightedItem >=0)
    {
        int index = m_HighlightedItem;

        smptr(MenuItem) selected_action = m_MenuItemVector[m_HighlightedItem];
        if((selected_action->GetChildMenu() != 0) && selected_action->GetActionItem()->isEnabled())
        {
            // This MenuItem has a sub-MenuPage. Start it.
            Geometry geo = selected_action->GetGeometry();
            selected_action->GetChildMenu()->m_MenuWindow = selected_action->GetChildMenu()->GetParentMenu()->m_MenuWindow;
            selected_action->GetChildMenu()->StartMenu(geo.x + geo.GetWidth() - 5, geo.y, 0, 0);

            // The current SubMenu is not the same as the new one...
            if(m_SubMenuAction != selected_action)
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
    if(m_NextMouseUpMeanStop == false)
    {
        ForceStopFocus(0, 0);
        m_NextMouseUpMeanStop = true;
        return;
    }

    m_Action_Triggered = false;

    bool hit_inside_a_menu = false;

    if(m_SubMenuAction.IsValid())
    {
        m_Action_Triggered = m_SubMenuAction->GetChildMenu()->TestMouseUp(x, y, button_flags, key_flags, hit_inside_a_menu);
    }

    if(m_Action_Triggered == false)
    {
        if(IsMouseInside())
        {
            if(m_SubMenuAction.IsValid())
            {
                m_Action_Triggered = false;
            }
            else if(m_HighlightedItem != -1)
            {
                if(m_MenuItemVector[m_HighlightedItem]->GetActionItem()->isEnabled() == false)
                {
                    // Do nothing. We don't want to close the menu when we release the mouse over an action that is not Enabled.
                    m_Action_Triggered = false;
                }
                else
                {
                    m_Action_Triggered = true;
                    // Fire the Action Here
                    ExecuteActionItem(m_MenuItemVector[m_HighlightedItem]);
                    NotifyActionTriggeredToParent(smptr(MenuPage)(this, false), m_MenuItemVector[m_HighlightedItem]);
                }
            }
        }
        else
        {
            // Bellow the Mouse, there was no MenuPage when the MouseUp happened.
            if(hit_inside_a_menu == false)
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
    if(m_SubMenuAction.IsValid())
    {
        m_Action_Triggered = m_SubMenuAction->GetChildMenu()->TestMouseUp(x, y, button_flags, key_flags, hit_inside_a_menu);
    }

    if(m_Action_Triggered == false)
    {
        if(IsMouseInside())
        {
            hit_inside_a_menu = true;

            if(m_SubMenuAction.IsValid())
            {
                m_Action_Triggered = false;
                // Do nothing. We don't want to close the menu when we are above an action that has a submenu.
            }
            else if(m_HighlightedItem != -1)
            {
                if(m_MenuItemVector[m_HighlightedItem]->GetActionItem()->isEnabled() == false)
                {
                    // Do nothing. We don't want to close the menu when we release the mouse over an action that is not Enabled.
                    m_Action_Triggered = false;
                }
                else
                {
                    m_Action_Triggered = true;
                    // Fire the Action Here
                    ExecuteActionItem(m_MenuItemVector[m_HighlightedItem]);
                    NotifyActionTriggeredToParent(smptr(MenuPage)(this, false), m_MenuItemVector[m_HighlightedItem]);
                    // But Do not emit the Stop
                    //sigPopupStop.emit();
                }
            }
        }
    }
    if(m_Action_Triggered)
        return true;
    return false;
}

bool MenuPage::TestMouseDown()
{
    bool b = false;
    if(m_SubMenuAction.IsValid())
    {
        b = m_SubMenuAction->GetChildMenu()->TestMouseDown();
    }
    if(b)
    {
        return true;
    }
    else
    {
        return IsMouseInside();
    }
}

void MenuPage::EmitMouseDrag(int x, int y, int dx, int dy, unsigned long button_flags, unsigned long key_flags)
{
    if(IsMouseInside())
        EmitMouseMove(x, y, 0, 0, button_flags, key_flags);
}

void MenuPage::RecvMouseLeave(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
    // Cancel selected item when the mouse is out.
    if(m_HighlightedItem != -1)
    {
        smptr(MenuItem) item = m_MenuItemVector[m_HighlightedItem];
        if(item->GetChildMenu().IsValid())
        {
            if(!item->GetChildMenu()->IsActive())
                m_HighlightedItem = -1;
        }
        else
            m_HighlightedItem = -1;
    }
}

void MenuPage::StartMenu(int MenuXPosition, int MenuYPosition, int x, int y, bool OverrideCurrentMenuChain)
{
    Geometry base = GetGeometry();
    base.SetX(MenuXPosition);
    base.SetY(MenuYPosition);

    int WindowWidth = GetGraphicsContext().GetWindowWidth();
    int WindowHeight = GetGraphicsContext().GetWindowHeight();

    // Correct the position of the MenuPage if is going out of the screen
    // The first page of a menu chain has less opportunities to be adjusted than its child pages.
    // The first page of a menu chain has (GetParentMenu() == 0) return true.
    int MenuLeft = base.x + base.GetWidth() + 5;
    if(MenuLeft > WindowWidth)
    {
        base.OffsetPosition(WindowWidth - MenuLeft, 0);
    }
    int MenuRight = base.x;
    if((MenuRight < 0) && (GetParentMenu() != 0))
    {
        base.OffsetPosition(-MenuRight, 0);
    }

    int MenuBottom = base.y + base.GetHeight() + 5;
    if((MenuBottom > WindowHeight) && (GetParentMenu() != 0))
    {
        base.OffsetPosition(0, WindowHeight - MenuBottom);
    }
    int MenuTop = base.y - 5;
    if((MenuTop < 0) && (GetParentMenu() != 0))
    {
        base.OffsetPosition(0, -MenuTop);
    }

    setGeometry(base);

    SetActive(true);

    // Add the menu to the stack manager popup list.  
    if(GetParentMenu() == 0)
    {
        // This is the head of the menu chain
        m_MenuWindow = GetThreadWindowCompositor().GetCurrentWindow();
    }
    GetThreadWindowCompositor().AddMenu(this, m_MenuWindow/*, OverrideCurrentMenuChain*/);
    m_NextMouseUpMeanStop = false;
    
    StopActionSubMenu();
}

void MenuPage::StopMenu(int x, int y)
{
    SetActive(false);
    ForceStopFocus(x, y);
    // The Stack Manager will remove all popup that are not visible anymore
    m_NextMouseUpMeanStop = false;
    m_HighlightedItem = -1;

    StopActionSubMenu();
}

// Never call this function directly
void MenuPage::Terminate(int x, int y, unsigned long button_flags, unsigned long key_flags)
{
}

void MenuPage::StopActionSubMenu()
{
    if(m_SubMenuAction.IsValid())
    {
        if(m_SubMenuAction->GetChildMenu().IsValid())
        {
            m_SubMenuAction->GetChildMenu()->StopMenu(0, 0);
        }
    }
    m_SubMenuAction = smptr(MenuItem)(0);
}

void MenuPage::ExecuteActionItem(smptr(MenuItem) menuItem)
{
    menuItem->GetActionItem()->Trigger();
}

void MenuPage::NotifyActionTriggeredToParent(smptr(MenuPage) menu, smptr(MenuItem) menuItem)
{
    if(m_Parent.IsValid())
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
    if(m_Parent.IsValid())
    {
        m_Parent->NotifyTerminateMenuCascade();
    }
    else
    {
        sigTerminateMenuCascade.emit();
    }

//     if(IsActive())
//     {
//         StopMenu();
//     }
}

void MenuPage::NotifyMouseDownOutsideMenuCascade(int x, int y)
{
    if(m_Parent.IsValid())
    {
        m_Parent->NotifyMouseDownOutsideMenuCascade(x, y);
    }
    else
    {
        // This is the top MenuPage in a menu chain.
        // If this MenuPage has been registered with a MenuBar, then the MenuBar will intercept this signal
        // and terminate the menu chain.
        sigMouseDownOutsideMenuCascade.emit(smptr(MenuPage)(this, false), x, y);
        // It is also possible that this MenuPage is not associated to a MenuBar (called directly for a contextual menu)
        if(m_IsTopOfMenuChain == false)
        {
            // This MenuPage is not attached to a MenuBar
            if(IsActive())
            {
                //StopMenu();
            }
        }
    }
}

void MenuPage::SetParentMenu(smptr(MenuPage) parent)
{
    m_Parent = parent;
}

smptr(MenuPage) MenuPage::GetParentMenu()
{
    return m_Parent;
}

long MenuPage::ComputeChildLayout()
{
    return ActiveInterfaceObject::ComputeChildLayout();
}

void MenuPage::setGeometry(const Geometry& geo)
{
    // The MenuPage widget cannot be resized by the client. The menu widget controls its own size.
    
    Geometry base = GetGeometry();
    // We are only interested in the position X/Y. The popup figures out its width and height by itself.
    base.SetX(geo.x);
    base.SetY(geo.y);

    base.SetWidth(geo.GetWidth());
    base.SetHeight(m_numItem * PRACTICAL_WIDGET_HEIGHT);

    SetBaseXY(geo.x, geo.y);

    PositionChildLayout(0, 0);
}

smptr(ActionItem) MenuPage::GetActionItem(int index) const
{
    nuxAssert(index >= 0);
    if(index >= (int)m_MenuItemVector.size())
        return smptr(ActionItem)(0);

    return m_MenuItemVector[index]->GetActionItem();
}

int MenuPage::GetActionItemIndex(const smptr(ActionItem) action) const
{
    if(action == 0)
        return -1;

    for(int i = 0; i < (int)m_MenuItemVector.size(); i++)
    {
        if(action == m_MenuItemVector[i]->GetActionItem())
        {
            return i;
        }
    }
    return -1;
}


NAMESPACE_END_GUI
