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


#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#if defined(NUX_OS_WINDOWS)
    #include "NuxGraphics/Gfx_Events.h"
#elif defined(NUX_OS_LINUX)
    #include "NuxGraphics/GfxEventsX11.h"
#endif
#include "Utils.h"
namespace nux { //NUX_NAMESPACE_BEGIN
typedef enum
{
    eSigMouseNone   = 0L,
    eSigMouseDown   = (1L<<0),
    eSigMouseUp     = (1L<<1),
    eSigMouseMove   = (1L<<2),
    eSigMouseEnter  = (1L<<3),
    eSigMouseLeave  = (1L<<4),
} eSignals;

////////////////////////////////////////////////////////////////////////////
// EventHandlers Policies
////////////////////////////////////////////////////////////////////////////

class BaseMouseHandler
{
public:
    BaseMouseHandler();
    ~BaseMouseHandler();

    unsigned int ProcessMouseInOut(IEvent& ievent, const Geometry & g);
    unsigned int Process(IEvent& ievent, const Geometry & g);
    void SetMouseFocus(bool b);

    // HasMouseFous == true mean the mouse (left button) was pressed on an area at a previous moment, 
    // and now the mouse maybe inside or outside of that area while the button(left mouse button)
    // who initiated the "button down" is still pressed.
    bool HasMouseFocus();
    bool MouseIn();
    bool ReleaseMouseFocus();

    void ForceMouseFocus(int x, int y, const Geometry & g);
    void StopMouseFocus(int x, int y, const Geometry & g);

public:
    bool m_hasMouseFocus;
    bool m_PreviousMouseIn, m_CurrentMouseIn;
    unsigned int m_events;
    unsigned int m_flag;
    
    //! Last know mouse X coordinate inside the area.
    int m_mouse_positionx;
    //! Last know mouse Y coordinate inside the area.
    int m_mouse_positiony;
    //! Mouse DeltaX coordinate inside the area.
    int m_mouse_deltax;
    //! Mouse DeltaY coordinate inside the area.
    int m_mouse_deltay;

    bool m_first_time;
};


//class ButtonEventHandler
//{
//public:
//    ButtonEventHandler(){};
//    ~ButtonEventHandler(){};
//
//    void EventHandler(BaseArea& area, IEvent& ievent, bool HasFocus = false)
//    {
//        int x, y, lo_x, hi_x, lo_y, hi_y;
//        x = ievent.x;
//        y = ievent.y;
//        
//        lo_x = area.GetPosX();
//        hi_x = area.GetPosX() + area.GetWidth();
//        lo_y = area.GetPosY();
//        hi_y = area.GetPosY() + area.GetHeight();
//
//        bool is_in = PT_IN_BOX( x, y, lo_x, hi_x, lo_y, hi_y );
//        if(!is_in && !HasFocus)
//        {
//            area.m_MouseEventCurrent.MouseIn = false;
//            return;
//        }
//        area.m_MouseEventCurrent.MouseIn = true;
//        area.m_MouseEventCurrent.PosX = area.GetPosX() - ievent.x;
//        area.m_MouseEventCurrent.PosY = area.GetPosY() - ievent.y;
//        switch(ievent.event_type)
//        {
//        case I_ButtonPress:
//            {
//                area.m_MouseEventCurrent.MouseDown = true;
//                area.m_HWMouseDown = true;
//            }
//            break;
//
//        case I_ButtonRelease:
//            {
//                area.m_MouseEventCurrent.MouseUp = true;
//                area.m_HWMouseUp = true;
//            }
//            break;
//
//        case I_EnterNotify:
//            {
//                area.m_HWMouseEnter = true;
//            }
//            break;
//
//        case I_LeaveNotify:
//            {
//                area.m_HWMouseLeave = true;
//            }
//            break;
//        }
//    }
//};

class IncrButtonEventHandler
{
public:
    IncrButtonEventHandler(){};
    ~IncrButtonEventHandler(){};
};

class DecButtonEventHandler
{
public:
    DecButtonEventHandler(){};
    ~DecButtonEventHandler(){};
};

} //NUX_NAMESPACE_END

#endif // EVENTHANDLER_H
