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


#ifndef EVENTS_H
#define EVENTS_H

#include "GLResource.h"
#include "VirtualKeyCodes.h"
//--------------------------------------------------------------key_code_e
// Keyboard codes. There's also a restricted set of codes that are most 
// probably supported on different platforms. Any platform dependent codes
// should be converted into these ones. There're only those codes are
// defined that cannot be represented as printable ASCII-characters. 
// All printable ASCII-set can be used in a regilar C/C++ manner: 
// ' ', 'A', '0' '+' and so on.
// Since the class is used for creating very simple demo-applications
// we don't need very rich possibilities here, just basic ones. 
// Actually the numeric key codes are taken from the SDL library, so,
// the implementation of the SDL support does not require any mapping.
//enum eKeyCode
//{
//    // ASCII set. Should be supported everywhere
//    key_backspace      = 8,
//    key_tab            = 9,
//    key_clear          = 12,
//    key_return         = 13,
//    key_shift          = 16,
//    key_control        = 17,
//    key_pause          = 19,
//    key_escape         = 27,
//
//    key_space          = 32,
//
//    // Keypad 
//    key_delete         = 127,
//    key_kp0            = 256,
//    key_kp1            = 257,
//    key_kp2            = 258,
//    key_kp3            = 259,
//    key_kp4            = 260,
//    key_kp5            = 261,
//    key_kp6            = 262,
//    key_kp7            = 263,
//    key_kp8            = 264,
//    key_kp9            = 265,
//    key_kp_period      = 266,
//    key_kp_divide      = 267,
//    key_kp_multiply    = 268,
//    key_kp_minus       = 269,
//    key_kp_plus        = 270,
//    key_kp_enter       = 271,
//    key_kp_equals      = 272,
//
//    // Arrow-keys and stuff
//    key_up             = 273,
//    key_down           = 274,
//    key_right          = 275,
//    key_left           = 276,
//    key_insert         = 277,
//    key_home           = 278,
//    key_end            = 279,
//    key_page_up        = 280,
//    key_page_down      = 281,
//
//    // Functional keys. You'd better avoid using
//    // f11...f15 in your applications if you want 
//    // the applications to be portable
//    key_f1             = 282,
//    key_f2             = 283,
//    key_f3             = 284,
//    key_f4             = 285,
//    key_f5             = 286,
//    key_f6             = 287,
//    key_f7             = 288,
//    key_f8             = 289,
//    key_f9             = 290,
//    key_f10            = 291,
//    key_f11            = 292,
//    key_f12            = 293,
//    key_f13            = 294,
//    key_f14            = 295,
//    key_f15            = 296,
//
//    // The possibility of using these keys is 
//    // very restricted. Actually it's guaranteed 
//    // only in win32_api and win32_sdl implementations
//    key_numlock        = 300,
//    key_capslock       = 301,
//    key_scrollock      = 302,
//
//    // Phew!
//    end_of_key_codes
//};

NAMESPACE_BEGIN_OGL

const long I_ExposureMask               = (1L<<15); 
const long I_StructureNotifyMask        = (1L<<17); 
const long I_SubstructureNotifyMask     = (1L<<19); 
/*
#define INL_BackSpace       INL_VK_BACK
#define INL_Tab             INL_VK_TAB
#define INL_Clear   	    INL_VK_CLEAR
#define INL_Enter           INL_VK_ENTER
#define INL_Shift_L         INL_VK_SHIFT
#define INL_Control_L       INL_VK_CONTROL
#define INL_Alt_L	        INL_VK_MENU
#define INL_Pause           INL_VK_PAUSE
#define INL_Caps_Lock       INL_VK_CAPITAL
#define INL_Escape          INL_VK_ESCAPE
#define INL_SpaceBar        INL_VK_SPACE
#define INL_Page_Up         INL_VK_PAGE_UP
#define INL_Page_Down       INL_VK_PAGE_DOWN
#define INL_End             INL_VK_END
#define INL_Home            INL_VK_HOME
#define INL_Left            INL_VK_LEFT
#define INL_Up              INL_VK_UP
#define INL_Right           INL_VK_RIGHT
#define INL_Down            INL_VK_DOWN
#define INL_Print	        INL_VK_SNAPSHOT
#define INL_Insert          INL_VK_INSERT
#define INL_Delete          INL_VK_DELETE
#define INL_LWin            INL_VK_LWIN
#define INL_RWin            INL_VK_RWIN
#define INL_APPS            INL_VK_APPS
#define INL_Multiply        INL_VK_MULTIPLY
#define INL_Add             INL_VK_ADD
#define INL_Subtract        INL_VK_SUBTRACT
#define INL_Decimal         INL_VK_DECIMAL
#define INL_Divide          INL_VK_DIVIDE
#define INL_Numlock         INL_VK_NUMLOCK
#define INL_Scroll          INL_VK_SCROLL
*/
#define INL_KP_ENTER        (0xff00 | INL_VK_ENTER)
#define INL_EXT_Shift_R     (0xff00 | INL_VK_SHIFT)
#define INL_EXT_Control_R   (0xff00 | INL_VK_CONTROL)
#define INL_EXT_Alt_R       (0xff00 | INL_VK_MENU)

#define INL_KP_PAGE_UP     (0xff00 | INL_VK_PAGE_UP)   
#define INL_KP_PAGE_DOWN   (0xff00 | INL_VK_PAGE_DOWN)
#define INL_KP_END         (0xff00 | INL_VK_END)
#define INL_KP_HOME        (0xff00 | INL_VK_HOME)
#define INL_KP_LEFT        (0xff00 | INL_VK_LEFT)
#define INL_KP_UP          (0xff00 | INL_VK_UP)
#define INL_KP_RIGHT       (0xff00 | INL_VK_RIGHT)
#define INL_KP_DOWN        (0xff00 | INL_VK_DOWN)

#define INL_KP_INSERT      (0xff00 | INL_VK_INSERT)
#define INL_KP_DELETE      (0xff00 | INL_VK_DELETE)

#define INL_F1     INL_VK_F1     
#define INL_F2     INL_VK_F2     
#define INL_F3     INL_VK_F3     
#define INL_F4     INL_VK_F4     
#define INL_F5     INL_VK_F5     
#define INL_F6     INL_VK_F6     
#define INL_F7     INL_VK_F7     
#define INL_F8     INL_VK_F8     
#define INL_F9     INL_VK_F9     
#define INL_F10    INL_VK_F10    
#define INL_F11    INL_VK_F11    
#define INL_F12    INL_VK_F12    
#define INL_F13    INL_VK_F13    
#define INL_F14    INL_VK_F14    
#define INL_F15    INL_VK_F15    
#define INL_F16    INL_VK_F16    
#define INL_F17    INL_VK_F17    
#define INL_F18    INL_VK_F18    
#define INL_F19    INL_VK_F19    
#define INL_F20    INL_VK_F20    
#define INL_F21    INL_VK_F21    
#define INL_F22    INL_VK_F22    
#define INL_F23    INL_VK_F23    
#define INL_F24    INL_VK_F24    

#define INL_LEFT_MOUSE	1
#define INL_MIDDLE_MOUSE	2
#define INL_RIGHT_MOUSE	3

// Key States. Set in e_key_modifiers.
#define INL_STATE_SHIFT	        0x00010000
#define INL_STATE_CAPS_LOCK	    0x00020000
#define INL_STATE_CTRL		    0x00040000
#define INL_STATE_ALT		    0x00080000
#define INL_STATE_NUMLOCK	    0x00100000          // most X servers do this?
#define INL_STATE_META		    0x00400000          // correct for XFree86
#define INL_STATE_SCROLLLOCK	0x00800000          // correct for XFree86

// These flags describe the mouse button responsible for the mouse event.
// They are valid only for the current frame.
// Go in e_mouse_state.
#define INL_EVENT_BUTTON1_UP        0x00001000
#define INL_EVENT_BUTTON2_UP        0x00002000
#define INL_EVENT_BUTTON3_UP        0x00004000

#define INL_EVENT_BUTTON1_DOWN      0x00010000
#define INL_EVENT_BUTTON2_DOWN      0x00020000
#define INL_EVENT_BUTTON3_DOWN      0x00040000

#define INL_EVENT_BUTTON1           0x00010000
#define INL_EVENT_BUTTON2           0x00020000
#define INL_EVENT_BUTTON3           0x00040000

#define INL_EVENT_MOUSEWHEEL        0x00080000
// These flags describe the state of the mouse buttons.
// They persist over several frame until the mouse buttons change state.
// Go in e_mouse_state.
#define INL_STATE_BUTTON1_DOWN      0x01000000
#define INL_STATE_BUTTON2_DOWN      0x02000000
#define INL_STATE_BUTTON3_DOWN      0x04000000

// These flags are set if the event is a double click.
// They are valid only for the current frame.
// Go in e_mouse_state.
#define INL_EVENT_BUTTON1_DBLCLICK  0x10000000
#define INL_EVENT_BUTTON2_DBLCLICK  0x20000000
#define INL_EVENT_BUTTON3_DBLCLICK  0x40000000

#define eLEFT_BUTTON            INL_EVENT_BUTTON1
#define eMIDDLE_BUTTON          INL_EVENT_BUTTON2
#define eRIGHT_BUTTON           INL_EVENT_BUTTON3

#define eLEFT_BUTTON_DOWN       INL_STATE_BUTTON1_DOWN
#define eMIDDLE_BUTTON_DOWN     INL_STATE_BUTTON2_DOWN
#define eRIGHT_BUTTON_DOWN      INL_STATE_BUTTON3_DOWN

#define INL_WIN32_MOUSEWHEEL_DELTA  120 // 120 correspond to one notch of the mouse wheel

typedef unsigned char uchar;
typedef unsigned long ulong;

struct EventToNameStruct {
    int event;
    const TCHAR* EventName;
};

enum 
{	// events
    INL_NO_EVENT         = 0,
    INL_MOUSE_PRESSED,
    INL_MOUSE_RELEASED,
    INL_KEYDOWN,
    INL_KEYUP,
    INL_MOUSE_MOVE,
    INL_SIZE_CONFIGURATION,
    INL_WINDOW_CONFIGURATION,
    INL_WINDOW_ENTER_FOCUS,
    INL_WINDOW_EXIT_FOCUS,
    INL_WINDOW_DIRTY,
    INL_WINDOW_MOUSELEAVE,
    INL_MOUSEWHEEL,
    INL_DESTROY_WINDOW,
    INL_TERMINATE_APP
};

typedef struct IEvent
{                              
    int width, height;
    unsigned int ascii_code;
    int virtual_code;
    bool IsLeftMouseDown;     
    bool IsRightMouseDown;
    bool IsMiddleMouseDown;
    int e_x,
        e_y,
        e_x_root,
        e_y_root,
        e_dx,
        e_dy,
        e_clicks,
        e_is_click,
        e_keysym;
    int e_wheeldelta;

    unsigned long e_key_modifiers;   // key modifiers
    unsigned short e_key_repeat_count; // number of time a key is repeated;
    unsigned long e_mouse_state;
    char    *e_text;
    int     e_length;
    unsigned long e_event;
    
    IEvent()
    {
        IsLeftMouseDown = false;
        IsRightMouseDown = false;
        IsMiddleMouseDown = false;
        e_text = (char *)"";

        for(int i = 0; i < INL_MAX_VK; i++) 
        {
            VirtualKeycodeState[i] = 0;
        }

        ascii_code = 0;
        virtual_code = 0;
        e_key_modifiers = 0;
        e_key_repeat_count = 0;
        e_mouse_state = 0;
        e_x = 0;
        e_y = 0;
        e_x_root = 0;
        e_y_root = 0;
        e_dx = 0;
        e_dy = 0;
        e_clicks = 0;
        e_is_click = 0;
        e_keysym = 0;
        e_wheeldelta = 0;

        //Application = 0;
    }
    int event_x() const      {return e_x;}
    int event_y() const      {return e_y;}
    int event_x_root() const {return e_x_root;}
    int event_y_root() const {return e_y_root;}
    int event_dx() const     {return e_dx;}
    int event_dy() const     {return e_dy;}
    void get_mouse(int &,int &);
    unsigned long event_key_state()	const   {return e_key_modifiers;}
    unsigned long event_mouse_state() const {return e_mouse_state;}

    //! Return virtual key code of the key that has triggered the last event.
    /*! 
        Return virtual key code of the key that has triggered the last event.
        @return the virtual key code.
    */
    unsigned long event_keysym() const {return e_keysym;}
    unsigned short event_key_auto_repeat_count() const {return e_key_repeat_count;}
    bool event_is_key_auto_repeat() const 
    {
        if(e_key_repeat_count > 1)
            return true;
        return false;
    }
    const char* event_text() const { return e_text; }
    // Because an event is save in e_event instead of calling immediately the handling function,
    // we must clear the previous event each time before we test for new event in Gfx_OpenGLImpl::get_event.
    void Reset()
    {
        e_event = INL_NO_EVENT;
        e_text = (char *)"";
        e_keysym = 0;
        e_key_repeat_count = 0;
        e_wheeldelta = 0;
    }

    unsigned long VirtualKeycodeState[INL_MAX_VK];

    //! Return the state of the Virtual key
    /*! 
        Return the state of the Virtual key.
        @param VirtualKey virtual key code.
        @return 1 if the key is pressed, 0 if the key is released.
    */
    unsigned long GetVirtualKeyState(unsigned long VirtualKey) const
    {
        if(VirtualKey >= INL_MAX_VK)
            return 0;

        if(VirtualKey <= 0)
            return 0;

        return VirtualKeycodeState[VirtualKey];
    }

    //void* Application;
} IEvent;

NAMESPACE_END_OGL

#endif // EVENTS_H
