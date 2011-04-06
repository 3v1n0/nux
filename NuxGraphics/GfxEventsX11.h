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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#ifndef GFXEVENTSX11_H
#define GFXEVENTSX11_H

#include "GLResource.h"
#include "VirtualKeyCodesX11.h"
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
#define NUX_STATE_SHIFT	        0x00010000
#define NUX_STATE_CAPS_LOCK	    0x00020000
#define NUX_STATE_CTRL		    0x00040000
#define NUX_STATE_ALT		    0x00080000
#define NUX_STATE_NUMLOCK	    0x00100000          // most X servers do this?
#define NUX_STATE_META		    0x00400000          // correct for XFree86
#define NUX_STATE_SCROLLLOCK	0x00800000          // correct for XFree86

// These flags describe the mouse button responsible for the mouse event.
// They are valid only for the current frame.
// Go in e_mouse_state.
#define NUX_EVENT_BUTTON1_UP        0x00001000
#define NUX_EVENT_BUTTON2_UP        0x00002000
#define NUX_EVENT_BUTTON3_UP        0x00004000

#define NUX_EVENT_BUTTON1_DOWN      0x00010000
#define NUX_EVENT_BUTTON2_DOWN      0x00020000
#define NUX_EVENT_BUTTON3_DOWN      0x00040000

#define NUX_EVENT_BUTTON1           0x00010000
#define NUX_EVENT_BUTTON2           0x00020000
#define NUX_EVENT_BUTTON3           0x00040000

#define NUX_EVENT_MOUSEWHEEL        0x00080000
// These flags describe the state of the mouse buttons.
// They persist over several frame until the mouse buttons change state.
// Go in e_mouse_state.
#define NUX_STATE_BUTTON1_DOWN      0x01000000
#define NUX_STATE_BUTTON2_DOWN      0x02000000
#define NUX_STATE_BUTTON3_DOWN      0x04000000

// These flags are set if the event is a double click.
// They are valid only for the current frame.
// Go in e_mouse_state.
#define NUX_EVENT_BUTTON1_DBLCLICK  0x10000000
#define NUX_EVENT_BUTTON2_DBLCLICK  0x20000000
#define NUX_EVENT_BUTTON3_DBLCLICK  0x40000000

#define eLEFT_BUTTON            NUX_EVENT_BUTTON1
#define eMIDDLE_BUTTON          NUX_EVENT_BUTTON2
#define eRIGHT_BUTTON           NUX_EVENT_BUTTON3

#define eLEFT_BUTTON_DOWN       NUX_STATE_BUTTON1_DOWN
#define eMIDDLE_BUTTON_DOWN     NUX_STATE_BUTTON2_DOWN
#define eRIGHT_BUTTON_DOWN      NUX_STATE_BUTTON3_DOWN

#define NUX_WIN32_MOUSEWHEEL_DELTA  120 // 120 correspond to one notch of the mouse wheel

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
    NUX_TERMINATE_APP
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
      e_text = (char *) "";

      for (int i = 0; i < NUX_MAX_VK; i++)
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
    int event_x() const
    {
      return e_x;
    }
    int event_y() const
    {
      return e_y;
    }
    int event_x_root() const
    {
      return e_x_root;
    }
    int event_y_root() const
    {
      return e_y_root;
    }
    int event_dx() const
    {
      return e_dx;
    }
    int event_dy() const
    {
      return e_dy;
    }
    void get_mouse (int &, int &);
    unsigned long event_key_state()	const
    {
      return e_key_modifiers;
    }
    unsigned long event_mouse_state() const
    {
      return e_mouse_state;
    }

    //! Return virtual key code of the key that has triggered the last event.
    /*!
        Return virtual key code of the key that has triggered the last event.
        @return the virtual key code.
    */
    unsigned long event_keysym() const
    {
      return e_keysym;
    }
    unsigned short event_key_auto_repeat_count() const
    {
      return e_key_repeat_count;
    }
    bool event_is_key_auto_repeat() const
    {
      if (e_key_repeat_count > 1)
        return true;

      return false;
    }
    const char *event_text() const
    {
      return e_text;
    }
    // Because an event is save in e_event instead of calling immediately the handling function,
    // we must clear the previous event each time before we test for new event in Gfx_OpenGLImpl::get_event.
    void Reset()
    {
      e_event = NUX_NO_EVENT;
      e_text = (char *) "";
      e_keysym = 0;
      e_key_repeat_count = 0;
      e_wheeldelta = 0;
    }

    unsigned long VirtualKeycodeState[NUX_MAX_VK];

    //! Return the state of the Virtual key
    /*!
        Return the state of the Virtual key.
        @param VirtualKey virtual key code.
        @return 1 if the key is pressed, 0 if the key is released.
    */
    unsigned long GetVirtualKeyState (unsigned long VirtualKey) const
    {
      if (VirtualKey >= NUX_MAX_VK)
        return 0;

      if (VirtualKey <= 0)
        return 0;

      return VirtualKeycodeState[VirtualKey];
    }

    //void* Application;
  } IEvent;

}

#endif // GFXEVENTSX11_H

