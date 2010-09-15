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

#include "NuxGraphics/GLError.h"
#include "WindowThread.h"
#include "BaseWindow.h"
#include "MenuPage.h"
#include "PaintLayer.h"
#include "Painter.h"

#include "NuxGraphics/FontTexture.h"
NAMESPACE_BEGIN_GUI

// extern TimeCounter gTimeCounter;
// float gTimeAccum = 0;

WindowCompositor::WindowCompositor()
{
    OverlayDrawingCommand       = smptr(BaseArea)(0);
    m_MouseFocusArea            = smptr(BaseArea)(0);
    m_MouseOverArea             = smptr(BaseArea)(0);
    m_PreviousMouseOverArea     = smptr(BaseArea)(0);
    m_CurrentEvent              = 0;
    m_CurrentWindow             = smptr(BaseWindow)(0);
    m_FocusAreaWindow           = smptr(BaseWindow)(0);
    m_MenuWindow                = smptr(BaseWindow)(0);
    m_OverlayWindow             = smptr(BaseWindow)(0);
    
    m_SelectedWindow = smptr(BaseWindow)(0);
    m_DrawList        = new std::list<smptr(ActiveInterfaceObject)>;
    m_EventRectList   = new std::list<Rect>;

    if(GetGraphicsThread()->GetWindow().HasFrameBufferSupport())
    {
        m_FrameBufferObject = GetThreadGLDeviceFactory()->CreateFrameBufferObject();
        // Do not leave the Fbo binded. Deactivate it.
        m_FrameBufferObject->Deactivate();
    }

    // At this stage, the size of the window may not be known yet.
    // FormatRenderTargets will be called the first time runtime gets into WindowThread::ExecutionLoop
    m_MainColorRT = GetThreadGLDeviceFactory()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8);
    m_MainDepthRT = GetThreadGLDeviceFactory()->CreateTexture(2, 2, 1, BITFMT_D24S8);

    m_MenuList = new std::list<MenuPage*>;
    m_PopupRemoved = false;
    m_MenuRemoved = false;
    m_ModalWindow = smptr(BaseWindow)(0);
    m_Background = new ColorLayer(Color(0xFF4D4D4D)); 
}

WindowCompositor::~WindowCompositor()
{
    m_WindowList.clear();
    m_DrawList->clear();
    m_EventRectList->clear();
    m_WindowToTextureMap.clear();
    m_MenuList->clear();

    NUX_SAFE_DELETE(m_DrawList);
    NUX_SAFE_DELETE(m_EventRectList);
    NUX_SAFE_DELETE(m_MenuList);
    NUX_SAFE_DELETE(m_Background);
}

bool WindowCompositor::MouseDown(Point pt)
{
    return false;
}

bool WindowCompositor::MouseMove(Point pt)
{
    return false;
}

bool WindowCompositor::MouseUp(Point pt)
{
    return true;
}

smptr(BaseWindow) WindowCompositor::GetSelectedWindow()
{
    return m_SelectedWindow;
}

WindowCompositor::RenderTargetTextures& WindowCompositor::GetWindowBuffer(smptr(BaseWindow) window)
{
    RenderTargetTextures invalid;
    RenderTargetTextures& ret = invalid;
    std::map< smptr(BaseWindow), RenderTargetTextures >::iterator it = m_WindowToTextureMap.find(window);
    if(it != m_WindowToTextureMap.end())
    {
        return (*it).second;
    }
    return ret;
}

void WindowCompositor::RegisterWindow(smptr(BaseWindow) window)
{
    if(window == 0)
        return;

    std::list<smptr(BaseWindow)>::iterator it = find(m_WindowList.begin(), m_WindowList.end(), window);
    if(it == m_WindowList.end())
    {
        m_WindowList.push_front(window);
        m_SelectedWindow = window;

        RenderTargetTextures rt;
        // Don't size the texture to the dimension of the window yet. this will be done later.
        if(GetGraphicsThread()->GetWindow().HasFrameBufferSupport())
        {
            rt.color_rt = GetThreadGLDeviceFactory()->CreateTexture(2, 2, 1, BITFMT_R8G8B8A8);
            rt.depth_rt = GetThreadGLDeviceFactory()->CreateTexture(2, 2, 1, BITFMT_D24S8);
        }
        m_WindowToTextureMap.insert( std::map< smptr(BaseWindow), RenderTargetTextures >::value_type( window, rt) );
    }
}

void WindowCompositor::UnRegisterWindow(smptr(BaseWindow) window)
{
    if(window == 0)
        return;

    std::list<smptr(BaseWindow)>::iterator it = find(m_WindowList.begin(), m_WindowList.end(), window);
    if(it != m_WindowList.end())
    {
        m_WindowList.erase(it); // @see STL for note about list.erase(it++). It is valid for lists.

        if(m_WindowList.size())
            m_SelectedWindow = (*m_WindowList.begin());

        std::map<smptr(BaseWindow), RenderTargetTextures >::iterator it2 = m_WindowToTextureMap.find(window);
        if(it2 != m_WindowToTextureMap.end())
        {
            (*it2).second.color_rt = 0;
            (*it2).second.depth_rt = 0;
            m_WindowToTextureMap.erase(it2);
        }
    }
}

long WindowCompositor::ProcessEventOnObject(IEvent &ievent, smptr(BaseObject) object, long TraverseInfo, long ProcessEventInfo)
{
    if(object == 0)
        return 0;

    long ret = 0;
//    if(GetFocusAreaWindow())
//    {
//        ievent.e_x_root = GetFocusAreaWindow()->GetBaseX();
//        ievent.e_y_root = GetFocusAreaWindow()->GetBaseY();
//    }
//    else
    {
        ievent.e_x_root = m_EventRoot.x; //GetFocusAreaWindow()->GetBaseX();
        ievent.e_y_root = m_EventRoot.y; //GetFocusAreaWindow()->GetBaseY();
    }

    if(object->Type().IsDerivedFromType(ActiveInterfaceObject::StaticObjectType))
    {
        smptr(ActiveInterfaceObject) ic(object);
        ret = ic->ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    }
    else if(object->Type().IsObjectType(BaseArea::StaticObjectType))
    {
        smptr(BaseArea) base_area(object);
        ret = base_area->OnEvent(ievent, TraverseInfo, ProcessEventInfo);
    }
    else if(object->Type().IsDerivedFromType(Layout::StaticObjectType))
    {
        smptr(Layout) layout(object);
        ret = layout->ProcessEvent(ievent, TraverseInfo, ProcessEventInfo);
    }
    else
    {
        nuxAssertMsg(0, TEXT("This should not happen"));
    }
    return ret;
}

void WindowCompositor::ProcessEvent(IEvent &ievent)
{
    long ret = 0;
    std::list<MenuPage*>::iterator menu_it;

    if(ievent.e_event == NUX_WINDOW_EXIT_FOCUS)
    {
        SetCurrentEvent(&ievent);
        if(GetMouseFocusArea().IsValid())
            ProcessEventOnObject(ievent, GetMouseFocusArea(), 0, 0);
        SetMouseFocusArea(smptr(BaseArea)(0));
        SetMouseOverArea(smptr(BaseArea)(0));
        SetCurrentEvent(0);
    }
    
    if(GetMouseFocusArea().IsValid() && ievent.e_event != NUX_MOUSE_PRESSED)
    {
        SetCurrentEvent(&ievent);
        SetCurrentWindow(GetFocusAreaWindow());
        ProcessEventOnObject(ievent, GetMouseFocusArea(), 0, 0);
        
        if(ievent.e_event == NUX_MOUSE_RELEASED)
        {
            SetMouseFocusArea(smptr(BaseArea)(0));
            // No need to set SetMouseOverArea to NULL.
            //SetMouseOverArea(0);
        }
        if((ievent.e_event == NUX_MOUSE_RELEASED))
        {
            SetWidgetDrawingOverlay(smptr(BaseArea)(0), smptr(BaseWindow)(0));
        }

        
        // Let all the menu area check the event first. Beside, they are on top of everything else.
        for(menu_it = m_MenuList->begin(); menu_it != m_MenuList->end(); menu_it++)
        {
            // The deepest menu in the menu cascade is in the front of the list.
            ret = (*menu_it)->ProcessEvent(ievent, ret, 0);
        }

        // We cannot get in here.
//         if((ievent.e_event == NUX_MOUSE_PRESSED) && m_MenuList->size())
//         {
//             bool inside = false;
//             for(menu_it = m_MenuList->begin(); menu_it != m_MenuList->end(); menu_it++)
//             {
//                 Geometry geo = (*menu_it)->GetGeometry();
//                 if(PT_IN_BOX( ievent.e_x-ievent.e_x_root, ievent.e_y-ievent.e_y_root, geo.x, geo.x + geo.width, geo.y, geo.y + geo.height ))
//                 {
//                     inside = true;
//                     break;
//                 }
//             }
// 
//             if(inside == false)
//             {
//                 (*m_MenuList->begin())->NotifyMouseDownOutsideMenuCascade(ievent.e_x-ievent.e_x_root, ievent.e_y-ievent.e_y_root);
//             }
//         }
        SetCurrentEvent(0);
        SetCurrentWindow(smptr(BaseWindow)(0));
    }
    else
    {
        SetCurrentEvent(&ievent);

        //SetMouseFocusArea(0);
        //SetMouseOverArea(0);

        if(m_MenuWindow.IsValid())
        {
            ievent.e_x_root = m_MenuWindow->GetBaseX();
            ievent.e_y_root = m_MenuWindow->GetBaseY();
        }

        // Let all the menu area check the event first. Beside, they are on top of everything else.
        for(menu_it = m_MenuList->begin(); menu_it != m_MenuList->end(); menu_it++)
        {
            // The deepest menu in the menu cascade is in the front of the list.
            ret = (*menu_it)->ProcessEvent(ievent, ret, 0);
        }

        if((ievent.e_event == NUX_MOUSE_PRESSED) && m_MenuList->size())
        {
            bool inside = false;
            for(menu_it = m_MenuList->begin(); menu_it != m_MenuList->end(); menu_it++)
            {
                Geometry geo = (*menu_it)->GetGeometry();
                if(PT_IN_BOX( ievent.e_x-ievent.e_x_root, ievent.e_y-ievent.e_y_root,
                    geo.x, geo.x + geo.width, geo.y, geo.y + geo.height ))
                {
                    inside = true;
                    break;
                }
            }

            if(inside == false)
            {
                (*m_MenuList->begin())->NotifyMouseDownOutsideMenuCascade(ievent.e_x-ievent.e_x_root, ievent.e_y-ievent.e_y_root);
            }
        }

        if(m_MenuWindow.IsValid())
        {
            ievent.e_x_root = 0;
            ievent.e_y_root = 0;
        }

        if((ievent.e_event == NUX_MOUSE_RELEASED))
        {
            SetWidgetDrawingOverlay(smptr(BaseArea)(0), smptr(BaseWindow)(0));
        }

        if((ievent.e_event == NUX_SIZE_CONFIGURATION) && m_MenuList->size())
        {
            (*m_MenuList->begin())->NotifyTerminateMenuCascade();
        }

        long ProcessEventInfo = 0;
        UBOOL MouseIsOverMenu = 0;
        if(ret & eMouseEventSolved)
        {
            // If one menu processed the event, then stop all other element from processing it.
            ProcessEventInfo = eDoNotProcess;
            MouseIsOverMenu = TRUE;
        }
        std::list<smptr(BaseWindow)>::iterator it;
        if(m_ModalWindowList.size() > 0)
        {
            SetCurrentWindow(*m_ModalWindowList.begin());
            ret = (*m_ModalWindowList.begin())->ProcessEvent(ievent, ret, ProcessEventInfo);
            SetCurrentWindow(smptr(BaseWindow)(0));
        }
        else
        {
            bool ordered = true;
            if(ievent.e_event == NUX_MOUSE_PRESSED )
            {
                // There is a possibility we might have to reorder the stack of windows.
                // Cancel the currently selected window.
                m_SelectedWindow = smptr(BaseWindow)(0);
                ordered = false;
            }

            for(it = m_WindowList.begin(); it != m_WindowList.end(); it++)
            {
                smptr(BaseWindow) window = *it;
                if((*it)->IsVisible())
                {
                    // Traverse the window from the top of the visibility stack to the bottom.
                    SetCurrentWindow(*it);
                    ret = (*it)->ProcessEvent(ievent, ret, ProcessEventInfo);
                    SetCurrentWindow(smptr(BaseWindow)(0));
                    if((ret & eMouseEventSolved) && (m_SelectedWindow == 0))
                    {
                        // The mouse event was solved in the window pointed by the iterator. 
                        // There isn't a currently selected window. Make the window pointed by the iterator the selected window.
                        ordered = false;
                        m_SelectedWindow = (*it);
                    }
                }
            }

            if((ordered == false) && (m_SelectedWindow != 0))
            {
                // Move the newly selected window at the top of the visibility stack.
                MoveWindowToFront(m_SelectedWindow);
            }

            // Check if the mouse is over a menu. If yes, do not let the main window analyze the event.
            // This avoid mouse in/out messages from widgets below the menu chain. 
            if(!MouseIsOverMenu)
            {
                // Let the main window analyze the event.
                ret = GetGraphicsThread()->ProcessEvent(ievent, ret, ProcessEventInfo) ;
            }
        }

        SetCurrentEvent(0);
    }

    CleanMenu();
//    menu_it = m_MenuList->begin();
//    while(menu_it != m_MenuList->end())
//    {
//        if((*menu_it)->isVisible() == false)
//        {
//            menu_it = m_MenuList->erase(menu_it);
//            m_MenuRemoved = true;
//        }
//        else
//            menu_it++;
//    }
}

void WindowCompositor::StartModalWindow(smptr(BaseWindow) window)
{
    if(window == 0)
        return;

    std::list<smptr(BaseWindow)>::iterator it = find(m_ModalWindowList.begin(), m_ModalWindowList.end(), window);
    if(it == m_ModalWindowList.end())
    {
        m_ModalWindowList.push_front(window);
    }
}

void WindowCompositor::StopModalWindow(smptr(BaseWindow) window)
{
    if(m_ModalWindowList.size() > 0)
    {
        if(*m_ModalWindowList.begin() == window)
            m_ModalWindowList.pop_front();
    }
}

void WindowCompositor::MoveWindowToFront(smptr(BaseWindow) window)
{
    if(window == 0)
        return;

    std::list<smptr(BaseWindow)>::iterator it = find(m_WindowList.begin(), m_WindowList.end(), window);
    if(it != m_WindowList.end())
    {
        m_WindowList.erase(it);
        m_WindowList.push_front(window);
    }
}

void WindowCompositor::MoveWindowToBack(smptr(BaseWindow) window)
{
    if(window == 0)
        return;

    std::list<smptr(BaseWindow)>::iterator it = find(m_WindowList.begin(), m_WindowList.end(), window);
    if(it != m_WindowList.end())
    {
        m_WindowList.erase(it);
        m_WindowList.push_back(window);
    }
}

void WindowCompositor::Draw(bool SizeConfigurationEvent, bool force_draw)
{
    if(!GetGraphicsThread()->GetWindow().isWindowMinimized())
    {
        //int w, h;
        GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, GetGraphicsThread()->GetGraphicsContext().GetWindowWidth(), GetGraphicsThread()->GetGraphicsContext().GetWindowHeight());
        GetGraphicsThread()->GetGraphicsContext().GetContextSize(m_Width, m_Height);
        GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, m_Width, m_Height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(m_Width, m_Height);


        if(GetGraphicsThread()->GetWindow().HasFrameBufferSupport())
        {
//             CHECKGL( glClearColor(0.0f, 0.0f, 1.0f, 0.0f) );
//             CHECKGL( glClearDepth(1.0f) );
//             CHECKGL( glClearStencil(0) );
//             CHECKGL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) );
//             GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, m_Width, m_Height);
//             GetGraphicsThread()->GetGraphicsContext().Push2DWindow(m_Width, m_Height);
//             //GetGraphicsThread()->GetGraphicsContext().SetEnvModeSelectColor(GL_TEXTURE0);
//             
//             glBegin(GL_QUADS);
//             {
//                 glColor4f(0, 1, 1, 1);
//                 glVertex4f(0,  0, 0, 1.0f);
//                 glVertex4f(0,  0 + 50, 0, 1.0f);
//                 glVertex4f(0 + 50,  0 + 50, 0, 1.0f);
//                 glVertex4f(0 + 50,  0, 0, 1.0f);
//             }
//             glEnd();
//             GetGraphicsThread()->GetGraphicsContext().QRP_Color(10, 10, 100, 100, Color::RoyalBlue);
            if(force_draw || SizeConfigurationEvent)
            {
                // We fall here after something dramatic has happen to the window such as a resizing. In this case
                // everything must be rendered This is very intensize and should happen rarely.
                RenderMainWindowComposition(true, true);
                //if(GetThreadGLDeviceFactory()->GetGraphicsBoardVendor() != BOARD_INTEL)
                if (true)
                {   
                    CopyTextureToCompositionRT(m_MainColorRT, 0, 0);
                    UpdatePostProcessRT();

                    DrawFloatingWindows(true, m_WindowList, false, true);
                    UpdatePostProcessRT();
                    DrawFloatingWindows(true, m_ModalWindowList, true, true);
                    UpdatePostProcessRT();

                    DrawMenu(true);
                    DrawTooltip(true);
                    DrawOverlay(true);

                    PresentBufferToScreen(m_CompositionRT, 0, 0, false, false);
                }
                else
                {
                     PresentBufferToScreen(m_MainColorRT, 0, 0, false);
                }
            }
            else if(m_PopupRemoved || m_MenuRemoved)
            {
                // A popup removed cause the whole window to be dirty (at least some part of it).
                // So exchange DrawList with a real Draw.
                RenderMainWindowComposition(false, true);
//                 if(GetThreadGLDeviceFactory()->GetGraphicsBoardVendor() != BOARD_INTEL)
                if (true)
                {   
                    CopyTextureToCompositionRT(m_MainColorRT, 0, 0);
                    UpdatePostProcessRT();

                    DrawFloatingWindows(false, m_WindowList, false, true);
                    UpdatePostProcessRT();
                    DrawFloatingWindows(false, m_ModalWindowList, true, true);
                    UpdatePostProcessRT();

                    DrawMenu(true);
                    DrawTooltip(true);
                    DrawOverlay(true);

                    PresentBufferToScreen(m_CompositionRT, 0, 0, false, false);
                }
                 else
                 {
                     PresentBufferToScreen(m_MainColorRT, 0, 0, false);
                 }
            }
            else
            {
                RenderMainWindowComposition(false, true);
//                 if(GetThreadGLDeviceFactory()->GetGraphicsBoardVendor() != BOARD_INTEL)
                if (true)
                {
                    CopyTextureToCompositionRT(m_MainColorRT, 0, 0);
                    UpdatePostProcessRT();

                    DrawFloatingWindows(false, m_WindowList, false, true);
                    UpdatePostProcessRT();
                    DrawFloatingWindows(false, m_ModalWindowList, true, true);
                    UpdatePostProcessRT();

                    DrawMenu(true);
                    DrawTooltip(true);
                    DrawOverlay(true);

                    PresentBufferToScreen(m_CompositionRT, 0, 0, false, false);
                }
                 else
                 {
                     PresentBufferToScreen(m_MainColorRT, 0, 0, false);
                 }
            }
        }
        else
        {
            if(force_draw || SizeConfigurationEvent)
            {
                gPainter.PushDrawColorLayer(GetGraphicsThread()->GetGraphicsContext(), Geometry(0, 0, m_Width, m_Height), Color(0xFF4D4D4D), true);
                RenderMainWindowComposition(true, false);

                DrawFloatingWindows(true, m_WindowList, false, false);
                DrawFloatingWindows(true, m_ModalWindowList, true, false);
                DrawMenu(true);
                DrawOverlay(true);
                DrawTooltip(true);
                gPainter.PopBackground();
            }
            else if(m_PopupRemoved || m_MenuRemoved)
            {
                gPainter.PushDrawColorLayer(GetGraphicsThread()->GetGraphicsContext(), Geometry(0, 0, m_Width, m_Height), Color(0xFF4D4D4D), true);
                RenderMainWindowComposition(true, false);

                DrawFloatingWindows(false, m_WindowList, false, false);
                DrawFloatingWindows(false, m_ModalWindowList, true, false);
                DrawMenu(true);
                DrawOverlay(true);
                DrawTooltip(true);
                gPainter.PopBackground();
            }
            else
            {
                RenderMainWindowComposition(false, false);
                DrawFloatingWindows(true, m_WindowList, false, false);
                DrawFloatingWindows(true, m_ModalWindowList, true, false);
                DrawMenu(true);
                DrawOverlay(true);
                DrawTooltip(true);
            }

            gPainter.EmptyBackgroundStack();
        }

        m_PopupRemoved = false;
        m_MenuRemoved = false;

		PageBBox page;
		page.xmin = 0;
		page.xmax = 100;
		page.ymin = 700;
		page.ymax = 730;
		page.x_margin = 0;
		page.y_margin = 0;

//         NString FPS = NString::Printf(TEXT("FPS: %3.2f"), GetGraphicsThread()->GetWindow().GetFrameRate());

        GetGraphicsThread()->GetGraphicsContext().Pop2DWindow();
//         GetGraphicsThread()->GetGraphicsContext().ResetStats();
//         GetGraphicsThread()->GetWindow().SwapBuffer();
    }
}

void WindowCompositor::DrawMenu(bool force_draw)
{
    smptr(BaseWindow) window = m_MenuWindow;
    int buffer_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    int buffer_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    if(window.IsValid())
    {
        int x = window->GetBaseX();
        int y = window->GetBaseY();
        GetGraphicsThread()->GetGraphicsContext().SetContext(x, y, buffer_width, buffer_height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(GetGraphicsThread()->GetGraphicsContext().GetWindowWidth(),
            GetGraphicsThread()->GetGraphicsContext().GetWindowHeight());
        GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    }
    else
    {
        GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, buffer_width, buffer_height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(GetGraphicsThread()->GetGraphicsContext().GetWindowWidth(),
            GetGraphicsThread()->GetGraphicsContext().GetWindowHeight());
        GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    }

    std::list<MenuPage*>::reverse_iterator rev_it_menu;
    for(rev_it_menu = m_MenuList->rbegin(); rev_it_menu != m_MenuList->rend( ); rev_it_menu++)
    {
        SetCurrentWindow(m_MenuWindow);
        (*rev_it_menu)->ProcessDraw(GetGraphicsThread()->GetGraphicsContext(), force_draw);
        SetCurrentWindow(smptr(BaseWindow)(0));
    }

    GetThreadGraphicsContext()->SetContext(0, 0, 
        GetGraphicsThread()->GetGraphicsContext().GetWindowWidth(),
        GetGraphicsThread()->GetGraphicsContext().GetWindowHeight());
}

void WindowCompositor::DrawOverlay(bool force_draw)
{
    smptr(BaseWindow) window = m_OverlayWindow;
    int buffer_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    int buffer_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    if(window.IsValid())
    {
        int x = window->GetBaseX();
        int y = window->GetBaseY();
        GetGraphicsThread()->GetGraphicsContext().SetContext(x, y, buffer_width, buffer_height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(buffer_width, buffer_height);
        GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    }
    else
        GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, buffer_width, buffer_height);

    if(OverlayDrawingCommand.IsValid())
    {
        SetCurrentWindow(m_OverlayWindow);
        OverlayDrawingCommand->OverlayDrawing(GetGraphicsThread()->GetGraphicsContext());
        SetCurrentWindow(smptr(BaseWindow)(0));
    }

    GetThreadGraphicsContext()->SetContext(0, 0, buffer_width, buffer_height);
}

void WindowCompositor::DrawTooltip(bool force_draw)
{
    smptr(BaseWindow) window = m_TooltipWindow;
    int buffer_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    int buffer_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    if(window.IsValid())
    {
        int x = window->GetBaseX();
        int y = window->GetBaseY();
        GetGraphicsThread()->GetGraphicsContext().SetContext(x, y, buffer_width, buffer_height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(buffer_width, buffer_height);
        GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    }
    else
        GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, buffer_width, buffer_height);

    if(m_TooltipText.Size())
    {
        SetCurrentWindow(m_TooltipWindow);
        
        int w = GFontBold->GetCharStringWidth(m_TooltipText.GetTCharPtr());
        int h = GFontBold->GetFontHeight();
        Geometry geo = Geometry(m_TooltipX + 10, m_TooltipY - h/2 - 2, w, h + 4);
        Geometry bkg_geo = geo;
        

        bkg_geo.OffsetSize(20, 8);
        bkg_geo.OffsetPosition(-10, -4);

        gPainter.PaintShape(GetGraphicsThread()->GetGraphicsContext(), bkg_geo, Color(0xA0000000), eSHAPE_CORNER_ROUND10, true);
        gPainter.PaintTextLineStatic(GetGraphicsThread()->GetGraphicsContext(), GFontBold, geo, m_TooltipText, Color(0xFFFFFFFF));
        SetCurrentWindow(smptr(BaseWindow)(0));
    }

    GetThreadGraphicsContext()->SetContext(0, 0, buffer_width, buffer_height);
}

void WindowCompositor::RenderWindowComposition(smptr(BaseWindow) window, bool force_draw)
{
    unsigned int window_width, window_height;
    window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();

    gPainter.EmptyBackgroundStack();
    SetCurrentWindow(window);
    window->ProcessDraw(GetGraphicsThread()->GetGraphicsContext(), force_draw || window->IsRedrawNeeded());
    SetCurrentWindow(smptr(BaseWindow)(0));
    gPainter.EmptyBackgroundStack();
}

//void WindowCompositor::DrawFloatingWindows(bool force_draw, bool UseFBO)
void WindowCompositor::DrawFloatingWindows(bool force_draw, const std::list<smptr(BaseWindow)>& WindowList, bool drawModal, bool UseFBO)
{
    GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    // Raw the windows from back to front;
    std::list<smptr(BaseWindow)>::const_reverse_iterator rev_it;
    for(rev_it = WindowList.rbegin(); rev_it != WindowList.rend(); rev_it++)
    {
        if((drawModal == false) && (*rev_it)->IsModal())
            continue;

        bool WindowNeedRedraw = false;
        if((*rev_it)->IsVisible())
        {
            RenderTargetTextures& rt = GetWindowBuffer(*rev_it);
            smptr(BaseWindow) window = *rev_it;
            WindowNeedRedraw = window->IsRedrawNeeded();

            if(rt.color_rt.IsValid() /*&& rt.depth_rt.IsValid()*/ && UseFBO)
            {
                t_s32 buffer_width = window->GetBaseWidth();
                t_s32 buffer_height = window->GetBaseHeight();
                if((rt.color_rt->GetWidth() != buffer_width) || (rt.color_rt->GetHeight() != buffer_height))
                {
                    rt.color_rt = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8);
                    rt.depth_rt = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_D24S8);
                }
                m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
                m_FrameBufferObject->SetRenderTarget( 0, rt.color_rt->GetSurfaceLevel(0) );
                m_FrameBufferObject->SetDepthSurface( rt.depth_rt->GetSurfaceLevel(0) );
                m_FrameBufferObject->Activate();
                GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, buffer_width, buffer_height);
                GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, buffer_width, buffer_height);
                GetGraphicsThread()->GetGraphicsContext().Push2DWindow(buffer_width, buffer_height);
                GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
                GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, buffer_width, buffer_height);

                CHECKGL( glClearColor(0, 0, 0, 0) );
                GLuint clear_color_buffer_bit = (force_draw || window->IsRedrawNeeded()) ? GL_COLOR_BUFFER_BIT : 0;
                CHECKGL( glClear(clear_color_buffer_bit | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) );
            }
            else
            {
                int x = window->GetBaseX();
                int y = window->GetBaseY();
                Matrix4 mat;
                mat.Translate(x, y, 0);
                //GetGraphicsThread()->GetGraphicsContext().SetContext(x, y, 0, 0);
                GetGraphicsThread()->GetGraphicsContext().Push2DWindow(GetGraphicsThread()->GetGraphicsContext().GetWindowWidth(),
                    GetGraphicsThread()->GetGraphicsContext().GetWindowHeight());

                //GetGraphicsThread()->GetGraphicsContext().Push2DModelViewMatrix(mat);
            }

            RenderWindowComposition(/*fbo,*/ window, force_draw);
            
            if(rt.color_rt.IsValid() /*&& rt.depth_rt.IsValid()*/ && UseFBO)
            {
//                GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
                m_FrameBufferObject->Deactivate();

                // Enable this to render the drop shadow under windows: not perfect yet...
                if(0/*force_draw || WindowNeedRedraw*/)
                {
                    unsigned int window_width, window_height;
                    window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
                    window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
                    GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, window_width, window_height);
                    GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
                    GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, window_width, window_height);
                    GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, window_width, window_height);
                    GetGraphicsThread()->GetGraphicsContext().Push2DWindow(window_width, window_height);

                    Geometry shadow(window->GetBaseX(), window->GetBaseY(), window->GetBaseWidth(), window->GetBaseHeight());
                    //if(window->IsVisibleSizeGrip())
                    {
                        shadow.OffsetPosition(4, 4);
                        gPainter.PaintShape(GetGraphicsThread()->GetGraphicsContext(), shadow, Color(0xFF000000), eSHAPE_CORNER_SHADOW);
                    }
//                    else
//                    {
//                        shadow.OffsetPosition(4, 4);
//                        gPainter.PaintShape(GetGraphicsThread()->GetGraphicsContext(), shadow, Color(0xFF000000), eSHAPE_CORNER_ROUND10_SHADOW);
//                    }
                }

                GetGraphicsThread()->GetGraphicsContext().GetRenderStates().SetBlend(TRUE, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                CHECKGL( glDepthMask(GL_FALSE) );
                {
                    CopyTextureToCompositionRT(rt.color_rt, window->GetBaseX(), window->GetBaseY());
                    //PresentBufferToScreen(rt.color_rt, window->GetBaseX(), window->GetBaseY(), false, false);
                }
                CHECKGL( glDepthMask(GL_TRUE) );
                GetGraphicsThread()->GetGraphicsContext().GetRenderStates().SetBlend(FALSE);
            }
            else
            {
//                int x = window->GetX();
//                int y = window->GetY();
//                Matrix4 mat;
//                mat.Translate(x, y, 0);
                GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, 0, 0);
                //GetGraphicsThread()->GetGraphicsContext().Pop2DModelViewMatrix();
            }
        }
    }

    if(UseFBO)
        m_FrameBufferObject->Deactivate();
}

// This function is meant to draw the main window and the small windows but it will not traverse the element.
// It just copies to the backbuffer what it already has in  the framebuffer objects.
void WindowCompositor::PresentRendering()
{
    m_FrameBufferObject->Deactivate();
    unsigned int window_width, window_height;
    window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    //GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().Push2DWindow(window_width, window_height);

    PresentBufferToScreen(m_MainColorRT, 0, 0, false);

    PageBBox page;
    page.xmin = 0;
    page.xmax = 100;
    page.ymin = 700;
    page.ymax = 730;
    page.x_margin = 0;
    page.y_margin = 0;

    NString FPS = NString::Printf(TEXT("FPS: %3.2f"), GetGraphicsThread()->GetFrameRate());

    GetGraphicsThread()->GetGraphicsContext().RenderColorTextLineStatic(GFontBold, page, FPS, Color(0xffff0000), true, eAlignTextLeft);     

    GetGraphicsThread()->GetGraphicsContext().Pop2DWindow();
//     GetGraphicsThread()->GetGraphicsContext().ResetStats();
//     GetGraphicsThread()->GetWindow().SwapBuffer();

//    const std::list<smptr(BaseWindow)>& W = m_WindowList;
//    std::list<smptr(BaseWindow)>::const_reverse_iterator rev_it;
//    for(rev_it = W.rbegin();
//        rev_it != W.rend();
//        rev_it++)
//    {
////        if((drawModal==false) && (*rev_it)->IsModal())
////            continue;
//
//        bool WindowNeedRedraw = false;
//        if((*rev_it)->IsVisible())
//        {
//
//            RenderTargetTextures& rt = GetWindowBuffer(*rev_it);
//            smptr(BaseWindow) window = *rev_it;
//            WindowNeedRedraw = window->IsRedrawNeeded();
//
//            if(rt.color_rt.IsValid())
//            {
//                CHECKGL( glEnable(GL_BLEND) );
//                CHECKGL( glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA) );
//                CHECKGL( glDepthMask(GL_FALSE) );
//                {
//                    PresentBufferToScreen(rt.color_rt, window->GetX(), window->GetY());
//                }
//                CHECKGL( glDepthMask(GL_TRUE) );
//                CHECKGL( glDisable(GL_BLEND) );
//            }
//        }
//    }
}

void WindowCompositor::RenderMainWindowComposition(bool force_draw, bool UseFBO)
{
    t_s32 buffer_width, buffer_height;
    if(UseFBO)
    {
        buffer_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
        buffer_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
        if((!m_MainColorRT.IsValid()) || (!m_MainDepthRT.IsValid()) || (m_MainColorRT->GetWidth() != buffer_width) || (m_MainColorRT->GetHeight() != buffer_height))
        {
            m_MainColorRT = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8);
            m_MainDepthRT = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_D24S8);
        }
        m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
        m_FrameBufferObject->SetRenderTarget( 0, m_MainColorRT->GetSurfaceLevel(0) );
        m_FrameBufferObject->SetDepthSurface( m_MainDepthRT->GetSurfaceLevel(0) );
        m_FrameBufferObject->Activate();
    }
    else
    {
        buffer_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
        buffer_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    }
    GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, buffer_width, buffer_height);
    GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, buffer_width, buffer_height);
    GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, buffer_width, buffer_height);
    GetGraphicsThread()->GetGraphicsContext().Push2DWindow(buffer_width, buffer_height);
    {
        CHECKGL( glClear(/*GL_COLOR_BUFFER_BIT |*/ GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) );
        //Begin 2D Drawing
        {
            if(force_draw)
            {
                gPainter.PushDrawLayer(GetGraphicsThread()->GetGraphicsContext(), Geometry(0, 0, buffer_width, buffer_height), m_Background);
                //gPainter.PushBackground(GetGraphicsThread()->GetGraphicsContext(), Geometry(0, 0, buffer_width, buffer_height), m_Background, true);
                
                GetGraphicsThread()->ProcessDraw(GetGraphicsThread()->GetGraphicsContext(), true);

                nuxAssert(GetGraphicsThread()->GetGraphicsContext().GetNumberOfClippingRegions() == 0);
                gPainter.PopBackground();
                gPainter.EmptyBackgroundStack();
            }
            else
            {
                gPainter.PushLayer(GetGraphicsThread()->GetGraphicsContext(), Geometry(0, 0, buffer_width, buffer_height), m_Background);
                //gPainter.PushBackground(GetGraphicsThread()->GetGraphicsContext(), Geometry(0, 0, buffer_width, buffer_height), m_Background, false);

                GetGraphicsThread()->ProcessDraw(GetGraphicsThread()->GetGraphicsContext(), false);

                nuxAssert(GetGraphicsThread()->GetGraphicsContext().GetNumberOfClippingRegions() == 0);
                gPainter.PopBackground();
                gPainter.EmptyBackgroundStack();
            }
        }
        // End 2D Drawing
    }
    if(UseFBO)
    {
        m_FrameBufferObject->Deactivate();
    }

    unsigned int window_width, window_height;
    window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().Push2DWindow(window_width, window_height);

    // Render the Buffer on the screen
//     if(UseFBO)
//     {
//         PresentBufferToScreen(m_MainColorRT, 0, 0, false);
//     }
}

void WindowCompositor::SetCompositionRT()
{
    t_s32 buffer_width, buffer_height;
    buffer_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    buffer_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    if((!m_CompositionRT.IsValid()) || (m_CompositionRT->GetWidth() != buffer_width) || (m_CompositionRT->GetHeight() != buffer_height))
    {
        m_CompositionRT = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8);
    }

    // Setup the Composition Render Target
    m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
    m_FrameBufferObject->SetRenderTarget(0, m_CompositionRT->GetSurfaceLevel(0));
    m_FrameBufferObject->SetDepthSurface(0);
    m_FrameBufferObject->Activate();
    GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, buffer_width, buffer_height);
    GetGraphicsThread()->GetGraphicsContext().Push2DWindow(buffer_width, buffer_height);
    GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, buffer_width, buffer_height);
}

void WindowCompositor::CopyTextureToCompositionRT(TRefGL<IOpenGLTexture2D> HWTexture, int x, int y)
{
    SetCompositionRT();
    HWTexture->SetFiltering(GL_NEAREST, GL_NEAREST);
    HWTexture->BindTextureToUnit(GL_TEXTURE0);
    GetGraphicsThread()->GetGraphicsContext().DisableAllTextureMode(GL_TEXTURE1);

    int TexWidth = HWTexture->GetWidth();
    int TexHeight = HWTexture->GetHeight();

    TexCoordXForm texxform;
    texxform.FlipVCoord(true);
    texxform.uscale = 1.0f;
    texxform.vscale = 1.0f;
    texxform.uwrap = TEXWRAP_REPEAT;
    texxform.vwrap = TEXWRAP_REPEAT;
    GetGraphicsThread()->GetGraphicsContext().QRP_GLSL_1Tex(x, y, TexWidth, TexHeight, HWTexture, texxform, Color::White);
}

void WindowCompositor::PresentBufferToScreen(TRefGL<IOpenGLTexture2D> HWTexture, int x, int y, bool RenderToMainTexture, bool BluredBackground)
{
    nuxAssert(HWTexture.IsValid());
    if(HWTexture.IsNull())
        return;
    t_s32 window_width, window_height;
    window_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    window_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    

    if(RenderToMainTexture && (HWTexture != m_MainColorRT))
    {
        nuxAssert(m_MainColorRT->GetWidth() == window_width);
        nuxAssert(m_MainColorRT->GetHeight() == window_height);
        m_FrameBufferObject->FormatFrameBufferObject(window_width, window_height, BITFMT_R8G8B8A8);
        m_FrameBufferObject->SetRenderTarget( 0, m_MainColorRT->GetSurfaceLevel(0) );
        m_FrameBufferObject->SetDepthSurface( m_MainDepthRT->GetSurfaceLevel(0) );
        m_FrameBufferObject->Activate();
    }
    else
    {
        GetThreadGLDeviceFactory()->DeactivateFrameBuffer();
    }

    GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, window_width, window_height);    
    GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();
    GetGraphicsThread()->GetGraphicsContext().SetDrawClippingRegion(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, window_width, window_height);
    GetGraphicsThread()->GetGraphicsContext().Push2DWindow(window_width, window_height);
    

    // Render the MAINFBO
    {
        //-->GetGraphicsThread()->GetGraphicsContext().DrawQuadTexture

        GetGraphicsThread()->GetGraphicsContext().EnableTextureMode(GL_TEXTURE0, GL_TEXTURE_RECTANGLE_ARB);
        HWTexture->BindTextureToUnit(GL_TEXTURE0);

        int src_width, src_height;
        src_width = HWTexture->GetWidth();
        src_height = HWTexture->GetHeight();

        if(!BluredBackground)
        {
            TexCoordXForm texxform0;
            texxform0.FlipVCoord(true);
            GetGraphicsThread()->GetGraphicsContext().GetRenderStates().SetBlend(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            GetThreadGraphicsContext()->QRP_GLSL_1Tex(x, y, src_width, src_height, HWTexture, texxform0, Color::White);
            GetGraphicsThread()->GetGraphicsContext().GetRenderStates().SetBlend(false);
        }
        else
        {
            HWTexture->SetFiltering(GL_NEAREST, GL_NEAREST);
            HWTexture->BindTextureToUnit(GL_TEXTURE0);

            m_BlurTexture->SetFiltering(GL_NEAREST, GL_NEAREST);
            m_BlurTexture->BindTextureToUnit(GL_TEXTURE1);

            TexCoordXForm texxform0;
            texxform0.SetFilter(TEXFILTER_NEAREST, TEXFILTER_NEAREST);
            texxform0.FlipVCoord(true);
            TexCoordXForm texxform1;
            texxform1.SetFilter(TEXFILTER_NEAREST, TEXFILTER_NEAREST);
            texxform1.FlipVCoord(true);
            texxform1.uoffset = (float)x/(float)src_width;
            texxform1.voffset = (float)y/(float)src_height;
            texxform1.SetTexCoordType(TexCoordXForm::OFFSET_COORD);

            GetGraphicsThread()->GetGraphicsContext().QRP_GLSL_2Tex(x, y, src_width, src_height,
                HWTexture, texxform0, Color::White, 
                m_BlurTexture, texxform1, Color::White);
        }
        //GetGraphicsThread()->GetGraphicsContext().DisableAllTextureMode(GL_TEXTURE1);
        //GetGraphicsThread()->GetGraphicsContext().SetEnvModeSelectColor(GL_TEXTURE0);
    }
}

void WindowCompositor::AddToDrawList(smptr(ActiveInterfaceObject) ic)
{
    m_DrawList->push_back(ic);
    m_EventRectList->push_back(getEventRect());
}

void WindowCompositor::ClearDrawList()
{
    m_DrawList->clear();
    m_EventRectList->clear();
}

void WindowCompositor::PushEventRectangle(Rect A)
{
    Rect B;
    t_u32 stacksize = (t_u32)EventRect.size();
    int x0, y0, x1, y1;

    int window_width, window_height;
    GetGraphicsThread()->GetGraphicsContext().GetContextSize(window_width, window_height);

    if(stacksize == 0)
    {
        B = Rect(0, 0, window_width, window_height);
    }
    else
    {
        B = EventRect[stacksize-1];
    }

    //    http://www.codecomments.com/archive263-2004-12-350347.html
    //    If your rectangles are given in 2D as Top,Left,Bottom,Right coordinates, as typical for GUI programming, then it's simply:
    //        intersect.Left = max(a.Left, b.Left);
    //        intersect.Top = max(a.Top, b.Top);
    //        intersect.Right = min(a.Right, b.Right );
    //        intersect.Bottom = min(a.Bottom, b.Bottom);
    //    And the intersection is empty unless intersect.Right > intersect.Left && intersect.Bottom > intersect.Top

    x0 = Max(A.x, B.x);
    y0 = Max(A.y, B.y);
    x1 = Min(A.x + A.width, B.x + B.width);
    y1 = Min(A.y + A.height, B.y + B.height);

    if((x1 > x0) && (y1 > y0))
    {
        EventRect.push_back(Rect(x0, y0, x1 - x0, y1 - y0));
    }
    else
    {
        EventRect.push_back(Rect(0, 0, 0, 0));
    }
}
void WindowCompositor::PopEventRectangle()
{
    int window_width, window_height;
    GetGraphicsThread()->GetGraphicsContext().GetContextSize(window_width, window_height);
    EventRect.pop_back();
    t_u32 stacksize = (t_u32)EventRect.size();
    if(stacksize == 0)
    {

    }
    else
    {
        Rect B = EventRect[stacksize-1];
    }
}

void WindowCompositor::EmptyEventRegion()
{
    int window_width, window_height;
    GetGraphicsThread()->GetGraphicsContext().GetContextSize(window_width, window_height);
    EventRect.clear();
}

Rect WindowCompositor::getEventRect()
{
    t_u32 stacksize = (t_u32)EventRect.size();
    if(stacksize != 0)
    {
        return EventRect[stacksize-1];
    }
    return Rect(0, 0, 0, 0);
}

void WindowCompositor::AddMenu(MenuPage* menu, smptr(BaseWindow) window, bool OverrideCurrentMenuChain)
{
    std::list<MenuPage*>::iterator it = find(m_MenuList->begin(), m_MenuList->end(), menu);
    if(it == m_MenuList->end())
    {
        // When adding a MenuPage, make sure that it is a child of the MenuPage in m_MenuList->begin().
        if(m_MenuList->size())
        {
            if(menu->GetParentMenu() != (*m_MenuList->begin()))
            {
                if(OverrideCurrentMenuChain)
                {
                    // Remove the current menu chain
                    for(it = m_MenuList->begin(); it != m_MenuList->end(); it++)
                    {
                        // Stop all pages
                        (*it)->StopMenu();
                    }
                    m_MenuList->clear();
                }
                else
                {
                    // The MenuPage argument is not a descendent of the current menu chain.
                    return;
                }
            }
        }
        m_MenuWindow = window;
        // The deepest menu is added in front of the list and tested first for events.
        m_MenuList->push_front(menu);
    }
}

// Be carefull never call this function while you are iterating through the elements of m_MenuList.
void WindowCompositor::RemoveMenu(MenuPage* menu)
{
    std::list<MenuPage*>::iterator it = find(m_MenuList->begin(), m_MenuList->end(), menu);

    if(it == m_MenuList->end())
    {
        return;
    }
    m_MenuList->erase(it);
    m_MenuRemoved = true;
    if(m_MenuList->size() == 0)
        m_MenuWindow = smptr(BaseWindow)(0);
}

void WindowCompositor::CleanMenu()
{
    if(m_MenuList->size() == 0)
        return;

    std::list<MenuPage*>::iterator menu_it = m_MenuList->begin();

    while(menu_it != m_MenuList->end())
    {
        if((*menu_it)->IsActive() == false)
        {
            menu_it = m_MenuList->erase(menu_it);
            m_MenuRemoved = true;
        }
        else
            menu_it++;
    }
    if(m_MenuList->size() == 0)
        m_MenuWindow = smptr(BaseWindow)(0);
}

void WindowCompositor::SetWidgetDrawingOverlay(smptr(BaseArea) ic, smptr(BaseWindow) OverlayWindow)
{
    OverlayDrawingCommand = ic;
    m_OverlayWindow = OverlayWindow;
}

smptr(BaseArea) WindowCompositor::GetWidgetDrawingOverlay()
{
    return OverlayDrawingCommand;
}

void WindowCompositor::SetTooltip(smptr(BaseArea) TooltipArea, const TCHAR *TooltipText, int x, int y)
{
    m_TooltipWindow = GetCurrentWindow();
    m_TooltipArea = TooltipArea;
    m_TooltipText = TooltipText;
    m_TooltipX = x;
    m_TooltipY = y;
}

void WindowCompositor::CancelTooltip()
{
    m_TooltipWindow = smptr(BaseWindow) (0);
    m_TooltipArea = smptr(BaseArea) (0);
    m_TooltipText = TEXT("");
}

bool WindowCompositor::ValidateMouseInsideTooltipArea(int x, int y)
{
    NUX_RETURN_VALUE_IF_FALSE(m_TooltipArea.IsValid(), false);
    NUX_RETURN_VALUE_IF_FALSE(m_TooltipWindow.IsValid(), false);

    Geometry geo = m_TooltipArea->GetGeometry();
    geo.OffsetPosition(m_TooltipWindow->GetBaseX(), m_TooltipWindow->GetBaseY());

    return geo.IsPointInside(x, y);    
}

bool WindowCompositor::IsTooltipActive()
{
    NUX_RETURN_VALUE_IF_FALSE(m_TooltipArea.IsValid(), false);
    return true;
}

void WindowCompositor::SetMouseFocusArea(smptr(BaseArea) area)
{
    m_MouseFocusArea = area;
    if(area == 0)
    {
        m_EventRoot.Set(0, 0);
    }
    SetFocusAreaWindow(GetCurrentWindow());
}

smptr(BaseArea) WindowCompositor::GetMouseFocusArea()
{
    return m_MouseFocusArea;
}

void WindowCompositor::SetMouseOverArea(smptr(BaseArea) area)
{
    m_MouseOverArea = area;
//     if(m_MouseOverArea)
//         nuxDebugMsg(TEXT("StackManager: Set MouseOver Area"));
//     else
//         nuxDebugMsg(TEXT("StackManager: Set MouseOver Area to 0"));
}

smptr(BaseArea) WindowCompositor::GetMouseOverArea()
{
    return m_MouseOverArea;
}

void WindowCompositor::SetPreviousMouseOverArea(smptr(BaseArea) area)
{
    m_PreviousMouseOverArea = area;
//     if(area)
//         nuxDebugMsg(TEXT("StackManager: Set Previous MouseOver Area"));
//     else
//         nuxDebugMsg(TEXT("StackManager: Set Previous MouseOver Area to 0"));
}

smptr(BaseArea) WindowCompositor::GetPreviousMouseOverArea()
{
    return m_PreviousMouseOverArea;
}

void WindowCompositor::SetBackgroundPaintLayer(AbstractPaintLayer* bkg)
{
    NUX_SAFE_DELETE(m_Background);
    m_Background = bkg->Clone();
}

void WindowCompositor::FloatingAreaConfigureNotify(int Width, int Height)
{
    std::list<smptr(BaseWindow)>::iterator it;
    for(it = m_WindowList.begin(); it != m_WindowList.end(); it++)
    {
        if((*it)->IsVisible())
        {
            (*it)->NotifyConfigurationChange(Width, Height);
        }
    }
}

void WindowCompositor::FormatRenderTargets(int width, int height)
{
    int buffer_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    int buffer_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();

    nuxAssert(buffer_width >= 1);
    nuxAssert(buffer_height >= 1);

    m_MainColorRT = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8);
    m_CompositionRT = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_R8G8B8A8);
    m_MainDepthRT = GetThreadGLDeviceFactory()->CreateTexture(buffer_width, buffer_height, 1, BITFMT_D24S8);
    
    // Clear the buffer the first time...
    m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
    m_FrameBufferObject->SetRenderTarget( 0, m_MainColorRT->GetSurfaceLevel(0) );
    m_FrameBufferObject->SetDepthSurface( m_MainDepthRT->GetSurfaceLevel(0) );
    m_FrameBufferObject->Activate();

    CHECKGL( glClearColor(0.0f, 0.0f, 0.0f, 0.0f) );
    CHECKGL( glClearDepth(1.0f) );
    CHECKGL( glClearStencil(0) );
    CHECKGL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) );
    m_FrameBufferObject->Deactivate();
    CHECKGL( glClearColor(0.0f, 0.0f, 0.0f, 0.0f) );
    CHECKGL( glClearDepth(1.0f) );
    CHECKGL( glClearStencil(0) );
    CHECKGL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT) );

    m_BlurTexture   = GetThreadGLDeviceFactory()->CreateTexture(Max(buffer_width, 1), Max(buffer_height, 1), 1, BITFMT_R8G8B8A8);
    m_FullSceneMip0 = GetThreadGLDeviceFactory()->CreateTexture(Max(buffer_width/2, 1), Max(buffer_height/2, 1), 1, BITFMT_R8G8B8A8);
    m_FullSceneMip1 = GetThreadGLDeviceFactory()->CreateTexture(Max(buffer_width/4, 1), Max(buffer_height/4, 1), 1, BITFMT_R8G8B8A8);
    m_FullSceneMip2 = GetThreadGLDeviceFactory()->CreateTexture(Max(buffer_width/8, 1), Max(buffer_height/8, 1), 1, BITFMT_R8G8B8A8);
}

void WindowCompositor::UpdatePostProcessRT()
{
    int buffer_width = GetGraphicsThread()->GetGraphicsContext().GetWindowWidth();
    int buffer_height = GetGraphicsThread()->GetGraphicsContext().GetWindowHeight();
    
    nuxAssert(buffer_width >= 1);
    nuxAssert(buffer_height >= 1);
    
    GetGraphicsThread()->GetGraphicsContext().EmptyClippingRegion();

    int src_width, src_height;
    int dst_width, dst_height;
    {
        src_width = buffer_width;
        src_height = buffer_height;
        dst_width = Max(buffer_width/2, 1);
        dst_height = Max(buffer_height/2, 1);

        m_FrameBufferObject->FormatFrameBufferObject(dst_width, dst_height, BITFMT_R8G8B8A8);
        m_FrameBufferObject->SetRenderTarget( 0, m_FullSceneMip0->GetSurfaceLevel(0) );
        m_FrameBufferObject->SetDepthSurface( 0 );
        m_FrameBufferObject->Activate();

        GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, dst_width, dst_height);
        GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, dst_width, dst_height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(dst_width, dst_height);

        m_CompositionRT->SetFiltering(GL_LINEAR, GL_LINEAR);

        TexCoordXForm texxform0;
        texxform0.uwrap = TEXWRAP_CLAMP;
        texxform0.vwrap = TEXWRAP_CLAMP;
        texxform0.FlipVCoord(true);
        GetThreadGraphicsContext()->QRP_GLSL_1Tex(0, 0, dst_width, dst_height, m_CompositionRT, texxform0, Color::White);
    }

    {
        src_width = Max(buffer_width/2, 1);
        src_height = Max(buffer_height/2, 1);
        dst_width = Max(buffer_width/4, 1);
        dst_height = Max(buffer_height/4, 1);

        m_FrameBufferObject->FormatFrameBufferObject(dst_width, dst_height, BITFMT_R8G8B8A8);
        m_FrameBufferObject->SetRenderTarget( 0, m_FullSceneMip1->GetSurfaceLevel(0) );
        m_FrameBufferObject->SetDepthSurface( 0 );
        m_FrameBufferObject->Activate();

        GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, dst_width, dst_height);
        GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, dst_width, dst_height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(dst_width, dst_height);


        m_FullSceneMip0->SetFiltering(GL_LINEAR, GL_LINEAR);
        TexCoordXForm texxform0;
        texxform0.uwrap = TEXWRAP_CLAMP;
        texxform0.vwrap = TEXWRAP_CLAMP;
        texxform0.FlipVCoord(true);
        GetThreadGraphicsContext()->QRP_GLSL_1Tex(0, 0, dst_width, dst_height, m_FullSceneMip0, texxform0, Color::White);
    }

    {
        src_width = Max(buffer_width/4, 1);
        src_height = Max(buffer_height/4, 1);
        dst_width = Max(buffer_width/8, 1);
        dst_height = Max(buffer_height/8, 1);

        m_FrameBufferObject->FormatFrameBufferObject(dst_width, dst_height, BITFMT_R8G8B8A8);
        m_FrameBufferObject->SetRenderTarget( 0, m_FullSceneMip2->GetSurfaceLevel(0) );
        m_FrameBufferObject->SetDepthSurface( 0 );
        m_FrameBufferObject->Activate();

        GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, dst_width, dst_height);
        GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, dst_width, dst_height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(dst_width, dst_height);


        m_FullSceneMip1->SetFiltering(GL_LINEAR, GL_LINEAR);
        TexCoordXForm texxform0;
        texxform0.uwrap = TEXWRAP_CLAMP;
        texxform0.vwrap = TEXWRAP_CLAMP;
        texxform0.FlipVCoord(true);
        GetThreadGraphicsContext()->QRP_GLSL_1Tex(0, 0, dst_width, dst_height, m_FullSceneMip1, texxform0, Color::White);
    }

    {
        src_width = buffer_width;
        src_height = buffer_height;
        dst_width = buffer_width;
        dst_height = buffer_height;

        m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
        m_FrameBufferObject->SetRenderTarget( 0, m_BlurTexture->GetSurfaceLevel(0) );
        m_FrameBufferObject->SetDepthSurface( 0 );
        m_FrameBufferObject->Activate();

        GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, dst_width, dst_height);
        GetGraphicsThread()->GetGraphicsContext().SetViewport(0, 0, dst_width, dst_height);
        GetGraphicsThread()->GetGraphicsContext().Push2DWindow(dst_width, dst_height);

        m_CompositionRT->SetFiltering(GL_LINEAR, GL_LINEAR);
        m_CompositionRT->BindTextureToUnit(GL_TEXTURE0);

        m_FullSceneMip0->SetFiltering(GL_LINEAR, GL_LINEAR);
        m_FullSceneMip0->BindTextureToUnit(GL_TEXTURE1);

        m_FullSceneMip1->SetFiltering(GL_LINEAR, GL_LINEAR);
        m_FullSceneMip1->BindTextureToUnit(GL_TEXTURE2);

        m_FullSceneMip2->SetFiltering(GL_LINEAR, GL_LINEAR);
        m_FullSceneMip2->SetWrap(GL_CLAMP, GL_CLAMP, GL_CLAMP);
        m_FullSceneMip2->BindTextureToUnit(GL_TEXTURE3);

        TexCoordXForm texxform0;
        texxform0.SetFilter(TEXFILTER_LINEAR, TEXFILTER_LINEAR);
        TexCoordXForm texxform1;
        texxform1.SetFilter(TEXFILTER_LINEAR, TEXFILTER_LINEAR);
        TexCoordXForm texxform2;
        texxform2.SetFilter(TEXFILTER_LINEAR, TEXFILTER_LINEAR);
        TexCoordXForm texxform3;
        texxform3.SetFilter(TEXFILTER_LINEAR, TEXFILTER_LINEAR);

        GetGraphicsThread()->GetGraphicsContext().QRP_GLSL_4Tex(0, 0, dst_width, dst_height,
            m_CompositionRT, texxform0, Color::White,
            m_FullSceneMip0, texxform1, Color::White,
            m_FullSceneMip1, texxform2, Color::White,
            m_FullSceneMip2, texxform3, Color::White);
    }

    GetGraphicsThread()->GetGraphicsContext().DisableAllTextureMode(GL_TEXTURE1);
    GetGraphicsThread()->GetGraphicsContext().DisableAllTextureMode(GL_TEXTURE2);
    GetGraphicsThread()->GetGraphicsContext().DisableAllTextureMode(GL_TEXTURE3);
    m_FrameBufferObject->Deactivate();
    m_CompositionRT->SetFiltering(GL_NEAREST, GL_NEAREST);

    // Restore Main Frame Buffer
    m_FrameBufferObject->FormatFrameBufferObject(buffer_width, buffer_height, BITFMT_R8G8B8A8);
    m_FrameBufferObject->SetRenderTarget(0, m_CompositionRT->GetSurfaceLevel(0));
    m_FrameBufferObject->SetDepthSurface(0);
    m_FrameBufferObject->Activate();

    GetGraphicsThread()->GetGraphicsContext().SetContext(0, 0, buffer_width, buffer_height);
    GetGraphicsThread()->GetGraphicsContext().Push2DWindow(buffer_width, buffer_height);
}

TRefGL< IOpenGLBaseTexture > WindowCompositor::GetScreenBlurTexture()
{
    return m_BlurTexture;
}

NAMESPACE_END_GUI

