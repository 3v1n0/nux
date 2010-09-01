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


#include <sigc++/trackable.h>
#include <sigc++/signal.h>
#include <sigc++/functors/ptr_fun.h>
#include <sigc++/functors/mem_fun.h>

#include "Nux.h"
#include "BaseArea.h"
#include "NuxGraphics/OpenGLEngine.h"
#include "WindowCompositor.h"

NAMESPACE_BEGIN_GUI

long g_FocusHandle = -1;

long gNumArea = 0;

IMPLEMENT_OBJECT_TYPE(BaseArea);

BaseArea::BaseArea()
:   BaseObject()
,   m_AreaColor(Color::Green)
{
    //m_Handle = ++s_Handle;

    m_Geometry.SetX(0);
    m_Geometry.SetY(0);
    m_Geometry.SetWidth(1);
    m_Geometry.SetHeight(1);
    m_hasKeyboardFocus = false;

    m_CaptureMouseDownAnyWhereElse = false;
    m_EnableDoubleClick = false;
    m_EnableUserKeyboardProcessing = false;
}

BaseArea::~BaseArea()
{
}

unsigned short BaseArea::getKeyState(int nVirtKey)
{
#ifdef WIN32
    if(HasKeyboardFocus())
        return GetKeyState(nVirtKey);
    else
        return 0;
#else
    return 0;
#endif 
}

void BaseArea::ForceStartFocus(int x, int y)
{
    OnStartFocus.emit();
    m_EventHandler.ForceMouseFocus(x, y, m_Geometry);
}

void BaseArea::ForceStopFocus(int x, int y)
{
    OnEndFocus.emit();
    m_EventHandler.StopMouseFocus(x, y, m_Geometry);
}

long BaseArea::OnEvent(IEvent &ievent, long TraverseInfo, long ProcessEventInfo)
{ 
    smptr(BaseArea) PreviousMouseOverArea = (GetThreadWindowCompositor().m_PreviousMouseOverArea);
    smptr(BaseArea) CurrentMouseOverArea = (GetThreadWindowCompositor().m_MouseOverArea);

    gNumArea++;

    if(ProcessEventInfo & eDoNotProcess)
    {
        if(ievent.e_event == INL_MOUSE_PRESSED)
        {
            if(m_CaptureMouseDownAnyWhereElse)
                OnCaptureMouseDownAnyWhereElse.emit(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.event_mouse_state(), ievent.event_key_state());

            if(HasKeyboardFocus())
            {
                SetKeyboardFocus(false);
                OnEndFocus.emit();
            }
            return TraverseInfo;
        }

        // Even if it is eDoNotProcess, we still want to respond to mouse Enter/Leave events.
        {
        
            unsigned int mouse_signals;
            mouse_signals = m_EventHandler.ProcessMouseInOut(ievent, m_Geometry);
            if(mouse_signals & eSigMouseLeave)
            {
                OnMouseLeave.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.event_mouse_state(), ievent.event_key_state());
            }
            if(mouse_signals & eSigMouseEnter)
            {
                OnMouseEnter.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.event_mouse_state(), ievent.event_key_state());
            }
        }
        return TraverseInfo;
    }

    long ret = TraverseInfo;
    if(TraverseInfo & eMouseEventSolved)
    {
        bool hadMouseFocus = HasMouseFocus();
        m_MouseEventCurrent.MouseIn = false;
        m_EventHandler.SetMouseFocus(false);
        ret |=  eMouseEventSolved;


        if((CurrentMouseOverArea == 0) && (GetGeometry().IsPointInside(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root)))
        {
            CurrentMouseOverArea = smptr(BaseArea)(this, false);
        }

        if(ievent.e_event == INL_MOUSE_PRESSED)
        {
            if(HasKeyboardFocus())
            {
                SetKeyboardFocus(false);
                OnEndFocus.emit();
            }
        }
        
        if(ievent.e_event == INL_MOUSE_PRESSED)
        {
            if(m_CaptureMouseDownAnyWhereElse)
                OnCaptureMouseDownAnyWhereElse.emit(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.event_mouse_state(), ievent.event_key_state());
        }

        // Even if it is eMouseEventSolved, we still want to respond to mouse Enter/Leave events.
        {
            unsigned int mouse_signals;
            mouse_signals = m_EventHandler.ProcessMouseInOut(ievent, m_Geometry);

            if(mouse_signals & eSigMouseLeave)
            {
                OnMouseLeave.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.event_mouse_state(), ievent.event_key_state());
            }
            if(mouse_signals & eSigMouseEnter)
            {
                OnMouseEnter.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.event_mouse_state(), ievent.event_key_state());
            }
        }
    }
    else
    {
        unsigned int mouse_signals;
        bool hadMouseFocus = HasMouseFocus();
        mouse_signals = m_EventHandler.Process(ievent, m_Geometry);

        if(HasMouseFocus() && (GetThreadWindowCompositor().GetMouseFocusArea() == 0)) // never evince and object that has the mouse focus.
        {
            GetThreadWindowCompositor().SetMouseFocusArea(smptr(BaseArea)(this, false));
            GetThreadWindowCompositor().SetAreaEventRoot(ievent.e_x_root, ievent.e_y_root);
        }
        if(IsMouseInside())
        {
            if(CurrentMouseOverArea != this)
            {
                PreviousMouseOverArea = CurrentMouseOverArea;
//                 if(*CurrentMouseOverArea)
//                     nuxDebugMsg(TEXT("BaseArea: Mouse is inside; Setting PreviousMouseOverArea."));
//                 else
//                     nuxDebugMsg(TEXT("BaseArea: Mouse is inside; Setting PreviousMouseOverArea to 0."));
            }
            CurrentMouseOverArea = smptr(BaseArea)(this, false);
        }
        else
        {
            // The mouse is outside this widget. If the PreviousMouseOverArea == this, then call OnMouseLeave.
            if(PreviousMouseOverArea == this)
            {
                // We should also have CurrentMouseOverArea == this.
                if(CurrentMouseOverArea.IsValid()) 
                {
                    (CurrentMouseOverArea)->OnMouseLeave.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.event_mouse_state(), ievent.event_key_state());
                    //nuxDebugMsg(TEXT("BaseArea: Current MouseOver Leave"));
                }
                CurrentMouseOverArea = smptr(BaseArea)(0);
                PreviousMouseOverArea = smptr(BaseArea)(0);
            }
        }

        if(PreviousMouseOverArea != CurrentMouseOverArea)
        {
            if(PreviousMouseOverArea.IsValid())
            {
                (PreviousMouseOverArea)->OnMouseLeave.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.event_mouse_state(), ievent.event_key_state());
                // When calling OnMouseLeave.emit on a widget, we have to set the following states to false 
                (PreviousMouseOverArea)->m_EventHandler.m_CurrentMouseIn = false;
                (PreviousMouseOverArea)->m_EventHandler.m_PreviousMouseIn = false;
                //nuxDebugMsg(TEXT("BaseArea: Previous MouseOver Leave"));
            }
            if(CurrentMouseOverArea.IsValid())
            {
                (CurrentMouseOverArea)->OnMouseEnter.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.event_mouse_state(), ievent.event_key_state());
                //nuxDebugMsg(TEXT("BaseArea: Current MouseOver Enter"));
            }
            PreviousMouseOverArea = CurrentMouseOverArea;
        }

        if(ievent.e_event == INL_WINDOW_EXIT_FOCUS)
        {
            if(HasKeyboardFocus())
            {
                SetKeyboardFocus(false);
                OnEndFocus.emit();
            }
        }

        if(ievent.e_event == INL_MOUSEWHEEL)
        {
            if(HasKeyboardFocus())
            {
                OnMouseWheel.emit(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root,
                    ievent.e_wheeldelta,
                    ievent.event_mouse_state(), ievent.event_key_state());
            }
        }

        // Imagine a Toolbar. When a MouseDown happens on the toolbar, it gets the mouse focus. While the mouse
        // is being pressed, we can move it above a widget that is inside the toolbar. That widget should not claim that it has
        // resolved the mouse event because the mouse is still being owned by the Toolbar. At most, the widget
        // should be set in CurrentMouseOverArea. 
        if(m_EventHandler.MouseIn() && (GetThreadWindowCompositor().GetMouseFocusArea() == 0))
        {
            ret |=  eMouseEventSolved;
        }

        if(ievent.e_event == INL_MOUSE_PRESSED)
        {
            if(m_CaptureMouseDownAnyWhereElse)
            {
                if(hadMouseFocus && (m_EventHandler.MouseIn() == false))
                    // The area has the mouse focus and a mouse down happened outside of it.
                    OnCaptureMouseDownAnyWhereElse.emit(ievent.e_x - ievent.e_x_root, ievent.e_y - ievent.e_y_root, ievent.event_mouse_state(), ievent.event_key_state());
            }
        }

        if((mouse_signals == eSigMouseNone) && (ievent.e_event == INL_MOUSE_PRESSED))
        {
            if(HasKeyboardFocus())
            {
                SetKeyboardFocus(false);
                OnEndFocus.emit();
            }
        }
        else if((mouse_signals & eSigMouseDown) /*&& (ievent.e_event == INL_MOUSE_PRESSED)*/)
        {
            OnStartMouseFocus.emit();

            if(ievent.e_mouse_state & INL_EVENT_BUTTON1_DBLCLICK)
            {
                // Double click
                OnMouseDoubleClick.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.event_mouse_state(), ievent.event_key_state());
            }
            else
            {
                OnMouseDown.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x,
                    m_EventHandler.m_mouse_positiony - m_Geometry.y,
                    ievent.event_mouse_state(), ievent.event_key_state());
            }

            if(HasKeyboardFocus() == false)
            {
                // First mouse down
                SetKeyboardFocus(true);
                OnStartFocus.emit();
            }


            ret |=  eMouseEventSolved;
        }
        
        if(mouse_signals & eSigMouseDown)
        {
//            ret |=  eMouseEventSolved;
        }

        if((mouse_signals & eSigMouseUp))
        {
            OnEndMouseFocus.emit();
            // This is a mouse up on an area that has the mouse focus.
            // Just check that the mouse in. If the mouse is in, then it is a "Click".
            if(m_EventHandler.MouseIn())
            {
                OnMouseClick.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y, ievent.event_mouse_state(), ievent.event_key_state());
                ret |=  eMouseEventSolved;
            }
            OnMouseUp.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x,
                m_EventHandler.m_mouse_positiony - m_Geometry.y,
                ievent.event_mouse_state(), ievent.event_key_state());
        }

        if((mouse_signals & eSigMouseMove) && m_EventHandler.HasMouseFocus())
        {
            OnMouseDrag.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y,
                m_EventHandler.m_mouse_deltax, m_EventHandler.m_mouse_deltay,
                ievent.event_mouse_state(), ievent.event_key_state());
        }
        if(mouse_signals & eSigMouseMove && (!m_EventHandler.HasMouseFocus()))
        {
            OnMouseMove.emit(m_EventHandler.m_mouse_positionx - m_Geometry.x, m_EventHandler.m_mouse_positiony - m_Geometry.y,
                m_EventHandler.m_mouse_deltax, m_EventHandler.m_mouse_deltay,
                ievent.event_mouse_state(), ievent.event_key_state());
        }
    }

    if(HasKeyboardFocus() && (ievent.e_event == INL_KEYDOWN || ievent.e_event == INL_KEYUP))
    {
        OnKeyEvent.emit(
            GetGraphicsThread()->GetGraphicsContext(),
            ievent.e_event,
            ievent.event_keysym(),
            ievent.event_key_state(),
            ievent.event_text(),
            ievent.event_is_key_auto_repeat(),
            ievent.event_key_auto_repeat_count());
    }

    GetThreadWindowCompositor().m_PreviousMouseOverArea = PreviousMouseOverArea;
    GetThreadWindowCompositor().m_MouseOverArea = CurrentMouseOverArea;

    return ret;
}

void BaseArea::OnDraw(GraphicsContext& GfxContext, bool force_draw)
{
    GfxContext.QRP_GLSL_Color(m_Geometry.x, m_Geometry.y, m_Geometry.GetWidth(), m_Geometry.GetHeight(), m_AreaColor);
}

void BaseArea::SetBaseString(const TCHAR* Caption)
{
    BaseObject::SetBaseString(Caption);
}

bool BaseArea::HasKeyboardFocus()
{
    return m_hasKeyboardFocus;
}

void BaseArea::SetKeyboardFocus(bool b)
{
    m_hasKeyboardFocus = b;
}

int BaseArea::GetMouseX()
{
    return m_EventHandler.m_mouse_positionx - m_Geometry.x;
}

int BaseArea::GetMouseY()
{
    return m_EventHandler.m_mouse_positiony - m_Geometry.y;
}

bool BaseArea::IsMouseInside()
{
    return m_EventHandler.MouseIn();
}

bool BaseArea::HasMouseFocus()
{
    return m_EventHandler.HasMouseFocus();
}

bool BaseArea::MouseFocusOnOtherArea()
{
    return (GetThreadWindowCompositor().GetMouseFocusArea() == 0);
}

void BaseArea::CaptureMouseDownAnyWhereElse(bool b)
{
    m_CaptureMouseDownAnyWhereElse = b;
}

bool BaseArea::IsCaptureMouseDownAnyWhereElse() const
{
    return m_CaptureMouseDownAnyWhereElse;
}

void BaseArea::EnableDoubleClick(bool b)
{
    m_EnableDoubleClick = b;
}

bool BaseArea::IsDoubleClickEnabled()
{
    return m_EnableDoubleClick;
}

void BaseArea::EnableUserKeyboardProcessing(bool b)
{
    m_EnableUserKeyboardProcessing = b;
}

bool BaseArea::IsUserKeyboardProcessingEnabled()
{
    return m_EnableUserKeyboardProcessing;
}

void BaseArea::SetAreaMousePosition(int x, int y)
{
    m_EventHandler.m_mouse_positionx = x;
    m_EventHandler.m_mouse_positiony = y;
}

NAMESPACE_END_GUI
