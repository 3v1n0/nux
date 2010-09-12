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
#include "MouseHandler.h"
NAMESPACE_BEGIN_GUI
//extern bool gMouseOwned;
BaseMouseHandler* gFocusMouseHandler = 0; // put this is the GfxServer class

// static void SetMouseFocusOwner(BaseMouseHandler* ptr)
// {
//     gFocusMouseHandler = ptr;
//     if(ptr)
//         gMouseOwned = true;
//     else
//         gMouseOwned = false;
// }
// 
// static BaseMouseHandler* GetMouseFocusOwner()
// {
//     return gFocusMouseHandler;
// }

BaseMouseHandler::BaseMouseHandler()
{
    m_first_time        = true;
    m_hasMouseFocus     = false;
    m_CurrentMouseIn    = false;
}

BaseMouseHandler::~BaseMouseHandler()
{

}

unsigned int BaseMouseHandler::ProcessMouseInOut(IEvent& ievent, const Geometry & g)
{
    m_events = eSigMouseNone;
    if(ievent.e_event == NUX_NO_EVENT)
        return m_events;

    m_flag = ievent.event_key_state();

    int x, y, lo_x, hi_x, lo_y, hi_y;
    // Usually (e_x_root, e_y_root) is equal to (0, 0). In that case, (x, y) is the mouse coordinate
    // that refers to the top-left corner of the window.
    // If (e_x_root, e_y_root) is equal to the top left corner of this area in the window,
    // then (x, y) represent the coordinate of the mouse based on the top-left corner of this area.
    x = ievent.e_x - ievent.e_x_root;
    y = ievent.e_y - ievent.e_y_root;

    lo_x = g.x;
    hi_x = g.x + g.GetWidth()-1;
    lo_y = g.y;
    hi_y = g.y + g.GetHeight()-1;

    if((ievent.e_x == -1) && (ievent.e_y == -1))
    {
        // e_x == -1 and e_y == -1 are associated with some specific window events that have the mouse outside of any widget.
        // See WM_SETFOCUS, WM_KILLFOCUS, NUX_WINDOW_MOUSELEAVE
        m_CurrentMouseIn = false;
    }
    else
    {
        m_CurrentMouseIn = PT_IN_BOX( x, y, lo_x, hi_x, lo_y, hi_y );
    }
        

    if(m_first_time)
    {
        m_first_time = false;
    }
    else
    {
        if((m_PreviousMouseIn == true) && (m_CurrentMouseIn == false))
        {
            m_events |= eSigMouseLeave;
        }
        if((m_PreviousMouseIn == false) && (m_CurrentMouseIn == true))
        {
            m_events |= eSigMouseEnter;
        }
    }
    m_PreviousMouseIn = m_CurrentMouseIn;

    return m_events;
}

unsigned int BaseMouseHandler::Process(IEvent& ievent, const Geometry & g)
{
    m_events = eSigMouseNone;
    if(ievent.e_event == NUX_NO_EVENT)
        return m_events;

    m_flag = ievent.event_key_state();

    int x, y, lo_x, hi_x, lo_y, hi_y;
    // Normally e_x_root and e_y_root are equal to 0. Meaning, (x, y) is the mouse coordinate based on the window top-left corner.
    // If (e_x_root, e_y_root) is equal to this area top-left corner position, then (x, y) is the mouse coordinate based on this area.
    x = ievent.e_x - ievent.e_x_root;
    y = ievent.e_y - ievent.e_y_root;

    lo_x = g.x;
    hi_x = g.x + g.GetWidth()-1;
    lo_y = g.y;
    hi_y = g.y + g.GetHeight()-1;

    if((ievent.e_x == -1) && (ievent.e_y == -1))
    {
        // e_x == -1 and e_y == -1 are associated with some specific window events that have the mouse outside of any widget.
        // See WM_SETFOCUS, WM_KILLFOCUS, NUX_WINDOW_MOUSELEAVE
        m_CurrentMouseIn = false;
    }
    else
        m_CurrentMouseIn = PT_IN_BOX( x, y, lo_x, hi_x, lo_y, hi_y );


    if(m_first_time)
    {
        m_first_time = false;
    }
    else
    {
        if((m_PreviousMouseIn == true) && (m_CurrentMouseIn == false))
        {
            m_events |= eSigMouseLeave;
        }
        if((m_PreviousMouseIn == false) && (m_CurrentMouseIn == true))
        {
            m_events |= eSigMouseEnter;
        }
    }
    m_PreviousMouseIn = m_CurrentMouseIn;

    m_mouse_deltax = x - m_mouse_positionx;
    m_mouse_deltay = y - m_mouse_positiony;
    m_mouse_positionx = x;
    m_mouse_positiony = y; 

    if(m_CurrentMouseIn == false
        && HasMouseFocus() == false)
    {
        return m_events;
    }

    switch(ievent.e_event)
    {
    case NUX_MOUSE_PRESSED:
        {
            /*me.MouseDown = true;
            me.m_HWMouseDown = true;
            me.m_HWMouseUp = false;*/
            m_events |= eSigMouseDown;
            SetMouseFocus(true);
        }
        break;

    case NUX_MOUSE_RELEASED:
        {
            /*me.MouseUp = true;
            me.m_HWMouseUp = true;
            me.m_HWMouseDown = false;*/
            if(HasMouseFocus())
            {
                m_events |= eSigMouseUp;
                SetMouseFocus(false);
            }
        }
        break;

    case NUX_MOUSE_MOVE:
        {
            m_events |= eSigMouseMove;
        }
        break;
    }
    return m_events;
}

void BaseMouseHandler::SetMouseFocus(bool b)
{
    if(b)
    {
//          if(GetThreadWindowCompositor().GetMouseFocusArea()->m_EventHandler != this)
//              GetMouseFocusOwner()->ReleaseMouseFocus();
//         SetMouseFocusOwner(this);
        m_hasMouseFocus = true;
        m_CurrentMouseIn = true;
    }
    else
    {
//         if(GetMouseFocusOwner() == this)
//             SetMouseFocusOwner(0);
        m_hasMouseFocus = false;
        // If the mouse is released do not change the MouseIn status.
        if(!(m_events & eSigMouseUp))
            m_CurrentMouseIn = false;
    }
}

bool BaseMouseHandler::ReleaseMouseFocus()
{
    SetMouseFocus(false);
    return true;
}

bool BaseMouseHandler::HasMouseFocus()
{
    return m_hasMouseFocus;
}

bool BaseMouseHandler::MouseIn()
{
    return m_CurrentMouseIn;
}

void BaseMouseHandler::ForceMouseFocus(int x, int y, const Geometry & g)
{
    int lo_x, hi_x, lo_y, hi_y;
    lo_x = g.x;
    hi_x = g.x + g.GetWidth()-1;
    lo_y = g.y;
    hi_y = g.y + g.GetHeight()-1;

    bool isIn = PT_IN_BOX( x, y, lo_x, hi_x, lo_y, hi_y );
    if(isIn)
    {
        m_PreviousMouseIn = m_CurrentMouseIn = true;
    }
    else
    {
        m_PreviousMouseIn = m_CurrentMouseIn = false;
    }

    m_hasMouseFocus = true;
}

void BaseMouseHandler::StopMouseFocus(int x, int y, const Geometry & g)
{
//    int lo_x, hi_x, lo_y, hi_y;
//    lo_x = g.x;
//    hi_x = g.x + g.GetWidth();
//    lo_y = g.y;
//    hi_y = g.y + g.GetBaseHeight();
//
//    bool isIn = PT_IN_BOX( x, y, lo_x, hi_x, lo_y, hi_y );
//    if(isIn)
//    {
//        m_PreviousMouseIn = m_CurrentMouseIn = true;
//    }
//    else
//    {
//        m_PreviousMouseIn = m_CurrentMouseIn = false;
//    }

    m_hasMouseFocus = false;
}
NAMESPACE_END_GUI
