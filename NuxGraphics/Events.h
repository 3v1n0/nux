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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#ifndef EVENTS_H
#define EVENTS_H

#include "GLResource.h"

#if defined(NUX_OS_WINDOWS)
  #include "VirtualKeyCodes.h"
#elif defined(NUX_OS_LINUX)
  #include "VirtualKeyCodesX11.h"
#else
  #error VirtualKeyCode file not Implemented.
#endif


namespace nux
{
  const long I_ExposureMask               = (1L << 15);
  const long I_StructureNotifyMask        = (1L << 17);
  const long I_SubstructureNotifyMask     = (1L << 19);

  #define NUX_BackSpace       NUX_VK_BACK
  #define NUX_Tab             NUX_VK_TAB
  #define NUX_Clear   	    NUX_VK_CLEAR
  #define NUX_Enter           NUX_VK_ENTER
  #define NUX_Shift_L         NUX_VK_SHIFT
  #define NUX_Control_L       NUX_VK_CONTROL
  #define NUX_Alt_L	        NUX_VK_MENU
  #define NUX_Pause           NUX_VK_PAUSE
  #define NUX_Caps_Lock       NUX_VK_CAPITAL
  #define NUX_Escape          NUX_VK_ESCAPE
  #define NUX_SpaceBar        NUX_VK_SPACE
  #define NUX_Page_Up         NUX_VK_PAGE_UP
  #define NUX_Page_Down       NUX_VK_PAGE_DOWN
  #define NUX_End             NUX_VK_END
  #define NUX_Home            NUX_VK_HOME
  #define NUX_Left            NUX_VK_LEFT
  #define NUX_Up              NUX_VK_UP
  #define NUX_Right           NUX_VK_RIGHT
  #define NUX_Down            NUX_VK_DOWN
  #define NUX_Print	        NUX_VK_SNAPSHOT
  #define NUX_Insert          NUX_VK_INSERT
  #define NUX_Delete          NUX_VK_DELETE
  #define NUX_LWin            NUX_VK_LWIN
  #define NUX_RWin            NUX_VK_RWIN
  #define NUX_APPS            NUX_VK_APPS
  #define NUX_Multiply        NUX_VK_MULTIPLY
  #define NUX_Add             NUX_VK_ADD
  #define NUX_Subtract        NUX_VK_SUBTRACT
  #define NUX_Decimal         NUX_VK_DECIMAL
  #define NUX_Divide          NUX_VK_DIVIDE
  #define NUX_Numlock         NUX_VK_NUMLOCK
  #define NUX_Scroll          NUX_VK_SCROLL

#define NUX_KP_ENTER        (0xff00 | NUX_VK_ENTER)
#define NUX_EXT_Shift_R     (0xff00 | NUX_VK_SHIFT)
#define NUX_EXT_Control_R   (0xff00 | NUX_VK_CONTROL)
#define NUX_EXT_Alt_R       (0xff00 | NUX_VK_MENU)

#define NUX_KP_PAGE_UP     (0xff00 | NUX_VK_PAGE_UP)
#define NUX_KP_PAGE_DOWN   (0xff00 | NUX_VK_PAGE_DOWN)
#define NUX_KP_END         (0xff00 | NUX_VK_END)
#define NUX_KP_HOME        (0xff00 | NUX_VK_HOME)
#define NUX_KP_LEFT        (0xff00 | NUX_VK_LEFT)
#define NUX_KP_UP          (0xff00 | NUX_VK_UP)
#define NUX_KP_RIGHT       (0xff00 | NUX_VK_RIGHT)
#define NUX_KP_DOWN        (0xff00 | NUX_VK_DOWN)

#define NUX_KP_INSERT      (0xff00 | NUX_VK_INSERT)
#define NUX_KP_DELETE      (0xff00 | NUX_VK_DELETE)

#define NUX_F1     NUX_VK_F1
#define NUX_F2     NUX_VK_F2
#define NUX_F3     NUX_VK_F3
#define NUX_F4     NUX_VK_F4
#define NUX_F5     NUX_VK_F5
#define NUX_F6     NUX_VK_F6
#define NUX_F7     NUX_VK_F7
#define NUX_F8     NUX_VK_F8
#define NUX_F9     NUX_VK_F9
#define NUX_F10    NUX_VK_F10
#define NUX_F11    NUX_VK_F11
#define NUX_F12    NUX_VK_F12
#define NUX_F13    NUX_VK_F13
#define NUX_F14    NUX_VK_F14
#define NUX_F15    NUX_VK_F15
#define NUX_F16    NUX_VK_F16
#define NUX_F17    NUX_VK_F17
#define NUX_F18    NUX_VK_F18
#define NUX_F19    NUX_VK_F19
#define NUX_F20    NUX_VK_F20
#define NUX_F21    NUX_VK_F21
#define NUX_F22    NUX_VK_F22
#define NUX_F23    NUX_VK_F23
#define NUX_F24    NUX_VK_F24

#define NUX_LEFT_MOUSE	1
#define NUX_MIDDLE_MOUSE	2
#define NUX_RIGHT_MOUSE	3

// Key States. Set in e_key_modifiers.
#define NUX_STATE_SHIFT	      0x00010000
#define NUX_STATE_CAPS_LOCK	  0x00020000
#define NUX_STATE_CTRL		    0x00040000
#define NUX_STATE_ALT		      0x00080000
#define NUX_STATE_NUMLOCK	    0x00100000          // most X servers do this?
#define NUX_STATE_META		    0x00400000          // correct for XFree86
#define NUX_STATE_SCROLLLOCK	0x00800000          // correct for XFree86

// These flags describe the mouse button responsible for the mouse event.
// They are valid only for the current frame.
// Go in e_mouse_state.
#define NUX_EVENT_BUTTON1_UP        0x00001000
#define NUX_EVENT_BUTTON2_UP        0x00002000
#define NUX_EVENT_BUTTON3_UP        0x00004000
#define NUX_EVENT_BUTTON4_UP        0x00008000

#define NUX_EVENT_BUTTON1_DOWN      0x00010000
#define NUX_EVENT_BUTTON2_DOWN      0x00020000
#define NUX_EVENT_BUTTON3_DOWN      0x00040000
#define NUX_EVENT_BUTTON4_DOWN      0x00080000

#define NUX_EVENT_BUTTON1           0x00010000
#define NUX_EVENT_BUTTON2           0x00020000
#define NUX_EVENT_BUTTON3           0x00040000
#define NUX_EVENT_BUTTON4           0x00080000

#define NUX_EVENT_MOUSEWHEEL        0x00100000
// These flags describe the state of the mouse buttons.
// They persist over several frame until the mouse buttons change state.
// Go in e_mouse_state.
#define NUX_STATE_BUTTON1_DOWN      0x01000000
#define NUX_STATE_BUTTON2_DOWN      0x02000000
#define NUX_STATE_BUTTON3_DOWN      0x04000000
#define NUX_STATE_BUTTON4_DOWN      0x08000000

// These flags are set if the event is a double click.
// They are valid only for the current frame.
// Go in e_mouse_state.
#define NUX_EVENT_BUTTON1_DBLCLICK  0x10000000
#define NUX_EVENT_BUTTON2_DBLCLICK  0x20000000
#define NUX_EVENT_BUTTON3_DBLCLICK  0x40000000

#define NUX_MOUSEWHEEL_DELTA  120 // 120 correspond to one notch of the mouse wheel

  typedef unsigned char uchar;
  typedef unsigned long ulong;

  struct EventToNameStruct
  {
    int event;
    const TCHAR *EventName;
  };

  enum
  {
    // events
    NUX_NO_EVENT         = 0,
    NUX_MOUSE_PRESSED,
    NUX_MOUSE_RELEASED,
    NUX_MOUSE_DOUBLECLICK,
    NUX_KEYDOWN,
    NUX_KEYUP,
    NUX_MOUSE_MOVE,
    NUX_SIZE_CONFIGURATION,
    NUX_WINDOW_CONFIGURATION,
    NUX_WINDOW_ENTER_FOCUS,
    NUX_WINDOW_EXIT_FOCUS,
    NUX_WINDOW_DIRTY,
    NUX_WINDOW_MOUSELEAVE,
    NUX_MOUSEWHEEL,
    NUX_DESTROY_WINDOW,
    NUX_TERMINATE_APP,
    NUX_TAKE_FOCUS, // ClientMessage for window with WM_TAKE_FOCUS protocol-atom set
    NUX_DND_MOVE,
    NUX_DND_DROP,
    NUX_DND_ENTER,  //!< Synthetic event generated when the dnd action enters an InputArea. This is not the event sent when the dnd action enters the window.
    NUX_DND_LEAVE,  //!< Synthetic event generated when the dnd action leaves an InputArea. This is not the event sent when the dnd action leaves the window.
    NUX_DND_ENTER_WINDOW,  //!< Emitted when the DND action goes inside (XdndLeave) a window.
    NUX_DND_LEAVE_WINDOW,   //!< Emitted when the DND action goes outside (XdndEnter) a window.
  };

  unsigned long GetEventButton(unsigned long button_state);
  bool GetButtonState(unsigned long button_state, int button);
    
  #define NUX_EVENT_TEXT_BUFFER_SIZE 16

  //! Nux event class.
  class Event
  {
  public:
    Event ();

    // Because an event is save in e_event instead of calling immediately the handling function,
    // we must clear the previous event each time before we test for new event in Gfx_OpenGLImpl::get_event.
    void Reset();

    int GetX() const;
    int GetY() const;
    int GetRootX() const;
    int GetRootY() const;
    int GetDeltaX() const;
    int GetDeltaY() const;
    unsigned long GetKeyState()	const;
    unsigned long GetMouseState() const;

    /*!
        Get the button responsible for the event.
        @return The index of the button responsible for the event: 1 left mouse button, 2 middle mouse button, 3 right mouse button.
    */
    unsigned long GetEventButton() const;

    /*!
        Check if a mouse button is down.
        @param button The button whose state is to be checked.
        @return True if the button is down.
    */
    bool GetButtonState(int button) const;



    //! Return virtual key code of the key that has triggered the last event.
    /*!
        Return virtual key code of the key that has triggered the last event.
        @return the virtual key code.
    */
    unsigned long GetKeySym() const;
    unsigned short GetKeyRepeatCount() const;
    const TCHAR* GetText() const;

    //! Return the state of the Virtual key
    /*!
        Return the state of the Virtual key.
        @param VirtualKey virtual key code.
        @return 1 if the key is pressed, 0 if the key is released.
    */
    unsigned long GetVirtualKeyState (unsigned long VirtualKey) const;

    unsigned long VirtualKeycodeState[NUX_MAX_VK];
    int width, height;
    unsigned int ascii_code;
    int virtual_code;

    int e_x;
    int e_y;
    int e_x_root;
    int e_y_root;
    int e_dx;
    int e_dy;
    int e_clicks;
    int e_is_click;
    Time e_x11_timestamp;

    int e_x11_keycode;  //!< X11 raw keycode.
    int e_keysym;       //!< Key symbol.
    int e_wheeldelta;

    TCHAR           e_text [NUX_EVENT_TEXT_BUFFER_SIZE];
    unsigned long   e_key_modifiers;   // key modifiers
    unsigned long   e_mouse_state;
    unsigned short  e_key_repeat_count; // number of time a key is repeated;    
    int             e_length;
    unsigned long   e_event;
  };

  typedef Event IEvent;
}

#endif // EVENTS_H
