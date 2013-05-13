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


#ifndef EVENTS_H
#define EVENTS_H

#include "GLResource.h"

#if defined(NUX_OS_WINDOWS)
  #include "VirtualKeyCodes.h"
#elif defined(USE_X11)
  #include "VirtualKeyCodesX11.h"
#elif defined(NO_X11)
  #include "VirtualKeyCodes.h"
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
  #define NUX_Clear           NUX_VK_CLEAR
  #define NUX_Enter           NUX_VK_ENTER
  #define NUX_Shift_L         NUX_VK_SHIFT
  #define NUX_Control_L       NUX_VK_CONTROL
  #define NUX_Alt_L           NUX_VK_MENU
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
  #define NUX_Print           NUX_VK_SNAPSHOT
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

#define NUX_EXT_Shift_R     (0xff00 | NUX_VK_SHIFT)
#define NUX_EXT_Control_R   (0xff00 | NUX_VK_CONTROL)
#define NUX_EXT_Alt_R       (0xff00 | NUX_VK_MENU)

#define NUX_KP_DOWN         0xFF99
#define NUX_KP_UP           0xFF97
#define NUX_KP_LEFT         0xFF96
#define NUX_KP_RIGHT        0xFF98

// #define NUX_KP_PAGE_UP     (0xff00 | NUX_VK_PAGE_UP)
// #define NUX_KP_PAGE_DOWN   (0xff00 | NUX_VK_PAGE_DOWN)
// #define NUX_KP_END         (0xff00 | NUX_VK_END)
// #define NUX_KP_HOME        (0xff00 | NUX_VK_HOME)
// #define NUX_KP_INSERT      (0xff00 | NUX_VK_INSERT)
// #define NUX_KP_DELETE      (0xff00 | NUX_VK_DELETE)

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

// These flags describe the mouse button responsible for the mouse event.
// They are valid only for the current frame.
// Go in mouse_state.
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
// Describes the event as the first press in the double click event.
#define NUX_STATE_FIRST_EVENT       0x00200000
// These flags describe the state of the mouse buttons.
// They persist over several frame until the mouse buttons change state.
// Go in mouse_state.
#define NUX_STATE_BUTTON1_DOWN      0x01000000
#define NUX_STATE_BUTTON2_DOWN      0x02000000
#define NUX_STATE_BUTTON3_DOWN      0x04000000
#define NUX_STATE_BUTTON4_DOWN      0x08000000

// These flags are set if the event is a double click.
// They are valid only for the current frame.
// Go in mouse_state.
#define NUX_EVENT_BUTTON1_DBLCLICK  0x10000000
#define NUX_EVENT_BUTTON2_DBLCLICK  0x20000000
#define NUX_EVENT_BUTTON3_DBLCLICK  0x40000000

#define NUX_MOUSEWHEEL_DELTA  120 // 120 correspond to one notch of the mouse wheel

  typedef unsigned char uchar;
  typedef unsigned long ulong;

  struct EventToNameStruct
  {
    int event;
    const char *EventName;
  };

  enum MouseButton
  {
    MOUSE_BUTTON_INVALID = 0,
    MOUSE_BUTTON1 = 1,
    MOUSE_BUTTON2 = 2,
    MOUSE_BUTTON3 = 3,
    MOUSE_BUTTON4 = 4,
    MOUSE_BUTTON5 = 5,
    MOUSE_BUTTON6 = 6,
    MOUSE_BUTTON7 = 7,

    // The following values are deprecated. Use the ones above.
    NUX_INVALID_MOUSE_BUTTON = MOUSE_BUTTON_INVALID,
    NUX_MOUSE_BUTTON1 = MOUSE_BUTTON1,
    NUX_MOUSE_BUTTON2 = MOUSE_BUTTON2,
    NUX_MOUSE_BUTTON3 = MOUSE_BUTTON3,
    NUX_MOUSE_BUTTON4 = MOUSE_BUTTON4,
    NUX_MOUSE_BUTTON5 = MOUSE_BUTTON5,
    NUX_MOUSE_BUTTON6 = MOUSE_BUTTON6,
    NUX_LEFT_MOUSE    = NUX_MOUSE_BUTTON1,   // Deprecated
    NUX_MIDDLE_MOUSE  = NUX_MOUSE_BUTTON2, // Deprecated
    NUX_RIGHT_MOUSE   = NUX_MOUSE_BUTTON3,  // Deprecated
  };

  enum KeyModifier
  {
    KEY_MODIFIER_SHIFT       = 0x00010000,
    KEY_MODIFIER_CAPS_LOCK   = 0x00020000,
    KEY_MODIFIER_CTRL        = 0x00040000,
    KEY_MODIFIER_ALT         = 0x00080000,
    KEY_MODIFIER_NUMLOCK     = 0x00100000,
    KEY_MODIFIER_SUPER       = 0x00200000,
    KEY_MODIFIER_SCROLLLOCK  = 0x00400000,

    // The following values are deprecated. Use the ones above.
    NUX_STATE_SHIFT       = KEY_MODIFIER_SHIFT,
    NUX_STATE_CAPS_LOCK   = KEY_MODIFIER_CAPS_LOCK,
    NUX_STATE_CTRL        = KEY_MODIFIER_CTRL,
    NUX_STATE_ALT         = KEY_MODIFIER_ALT,
    NUX_STATE_NUMLOCK     = KEY_MODIFIER_NUMLOCK,
    NUX_STATE_SUPER       = KEY_MODIFIER_SUPER,
    NUX_STATE_SCROLLLOCK  = KEY_MODIFIER_SCROLLLOCK,
  };

  enum EventType
  {
    EVENT_NONE         = 0,
    EVENT_MOUSE_DOWN,
    EVENT_MOUSE_UP,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_DOUBLECLICK,
    EVENT_MOUSE_WHEEL,
    EVENT_MOUSE_CANCEL, /*!< InputArea lost ownership over a pressed mouse. Any
                             changes caused by the previous EVENT_MOUSE_PRESS
                             should be undone. */
    EVENT_KEY_DOWN,
    EVENT_KEY_UP,
    EVENT_WINDOW_SIZE_CONFIGURATION,
    EVENT_WINDOW_MAP,
    EVENT_WINDOW_UNMAP,
    EVENT_NC_WINDOW_CONFIGURATION,  //!< Non-client event on the window borders.
    EVENT_WINDOW_ENTER_FOCUS,
    EVENT_WINDOW_EXIT_FOCUS,
    EVENT_WINDOW_DIRTY,
    EVENT_WINDOW_MOUSELEAVE,
    EVENT_DESTROY_WINDOW,
    EVENT_TERMINATE_APP,
    EVENT_TAKE_FOCUS, // ClientMessage for window with WM_TAKE_FOCUS protocol-atom set
    EVENT_DND_MOVE,
    EVENT_DND_DROP,
    EVENT_DND_ENTER,  //!< Synthetic event generated when the dnd action enters an InputArea. This is not the event sent when the dnd action enters the window.
    EVENT_DND_LEAVE,  //!< Synthetic event generated when the dnd action leaves an InputArea. This is not the event sent when the dnd action leaves the window.
    EVENT_DND_ENTER_WINDOW,  //!< Emitted when the DND action goes inside(XdndLeave) a window.
    EVENT_DND_LEAVE_WINDOW,   //!< Emitted when the DND action goes outside(XdndEnter) a window.
    EVENT_GESTURE_BEGIN, //!< Emitted when a gesture begins. Event class is GestureEvent.
    EVENT_GESTURE_UPDATE, //!< Emitted when a gesture is updated. Event class is GestureEvent.
    EVENT_GESTURE_END, //!< Emitted when a gesture ends. Event class is GestureEvent.
    EVENT_GESTURE_LOST, //!< Emitted when a gesture target loses a gesture. Event class is GestureEvent.


    // The following values are deprecated. Use the ones above.
    NUX_NO_EVENT                  = EVENT_NONE,
    NUX_MOUSE_PRESSED             = EVENT_MOUSE_DOWN,
    NUX_MOUSE_RELEASED            = EVENT_MOUSE_UP,
    NUX_MOUSE_MOVE                = EVENT_MOUSE_MOVE,
    NUX_MOUSE_DOUBLECLICK         = EVENT_MOUSE_DOUBLECLICK,
    NUX_MOUSE_WHEEL               = EVENT_MOUSE_WHEEL,
    NUX_MOUSEWHEEL                = EVENT_MOUSE_WHEEL, //!< Deprecated. Use NUX_MOUSE_WHEEL.
    NUX_KEYDOWN                   = EVENT_KEY_DOWN,
    NUX_KEYUP                     = EVENT_KEY_UP,
    NUX_SIZE_CONFIGURATION        = EVENT_WINDOW_SIZE_CONFIGURATION,
    NUX_WINDOW_MAP                = EVENT_WINDOW_MAP,
    NUX_WINDOW_UNMAP              = EVENT_WINDOW_UNMAP,
    NUX_NC_WINDOW_CONFIGURATION   = EVENT_NC_WINDOW_CONFIGURATION,
    NUX_WINDOW_ENTER_FOCUS        = EVENT_WINDOW_ENTER_FOCUS,
    NUX_WINDOW_EXIT_FOCUS         = EVENT_WINDOW_EXIT_FOCUS,
    NUX_WINDOW_DIRTY              = EVENT_WINDOW_DIRTY,
    NUX_WINDOW_MOUSELEAVE         = EVENT_WINDOW_MOUSELEAVE,
    NUX_DESTROY_WINDOW            = EVENT_DESTROY_WINDOW,
    NUX_TERMINATE_APP             = EVENT_TERMINATE_APP,
    NUX_TAKE_FOCUS                = EVENT_TAKE_FOCUS,         // ClientMessage for window with WM_TAKE_FOCUS protocol-atom set
    NUX_DND_MOVE                  = EVENT_DND_MOVE,
    NUX_DND_DROP                  = EVENT_DND_DROP,
    NUX_DND_ENTER                 = EVENT_DND_ENTER,          //!< Synthetic event generated when the dnd action enters an InputArea. This is not the event sent when the dnd action enters the window.
    NUX_DND_LEAVE                 = EVENT_DND_LEAVE,          //!< Synthetic event generated when the dnd action leaves an InputArea. This is not the event sent when the dnd action leaves the window.
    NUX_DND_ENTER_WINDOW          = EVENT_DND_ENTER_WINDOW,   //!< Emitted when the DND action goes inside(XdndLeave) a window.
    NUX_DND_LEAVE_WINDOW          = EVENT_DND_LEAVE_WINDOW,   //!< Emitted when the DND action goes outside(XdndEnter) a window.
  };

  typedef EventType NuxEventType; // NuxEventType is deprecated.

  //! Returns index of the mouse button that triggered an event.
  /*!
      Given the mouse button states of and event, returns the index of the button that
      triggered an event. The index of the left mouse button is 1 and the index for the right
      mouse button is 2. If 0 is returned, then a mouse button didn't triggered the event.

      @param button_state The mouse button states of an event.
      @return The button that triggered the event.
  */
  MouseButton GetEventButton(unsigned long button_state);

  //! Returns the state of a mouse button: pressed or released.
  /*!
      Given the mouse button states of and event, returns the state of a mouse button.
      True is the button is pressed. False otherwise.

      @param button_state The mouse button states of an event.
      @param button Button to query(1 for left mouse button).
      @return True is the button is pressed. False otherwise.
  */
  bool GetButtonState(unsigned long button_state, MouseButton button);

  //! Returns the state of a special key: CTRL, Shift, Alt, NumLock...
  /*!
      Given the key modifiers states of and event, returns the state of a key modifier.
      True is the key is pressed. False otherwise.

      @param key_modifiers_state The key modifiers states of an event.
      @param key_modifier Key modifier be query.
      @return True is the key is pressed. False otherwise.
  */
  bool GetKeyModifierState(unsigned long key_modifiers_states, KeyModifier key_modifier);

  #define NUX_EVENT_TEXT_BUFFER_SIZE 16

  //! Nux event class.
  class Event
  {
  public:
    Event();

    // Because an event is save in event_type instead of calling immediately the handling function,
    // we must clear the previous event each time before we test for new event in Gfx_OpenGLImpl::get_event.
    /*virtual*/ void Reset();

    int GetX() const;
    int GetY() const;
    int GetRootX() const;
    int GetRootY() const;
    int GetDeltaX() const;
    int GetDeltaY() const;
    unsigned long GetKeyState()  const;
    unsigned long GetMouseState() const;

    bool IsShiftDown() const;
    bool IsControlDown() const;
    bool IsCapsLockDown() const;
    bool IsAltDown() const;
    bool IsNumLockDown() const;
    bool IsScrollLockDown() const;
    bool IsSuperKeyDown() const;

    //! Returns index of the mouse button that triggered this event.
    /*!
        Returns the index of the button that triggered this event.
        The index of the left mouse button is 1 and the index for the right
        mouse button is 2. If 0 is returned, then a mouse button didn't triggered the event.

        @return The button that triggered the event.
    */
    MouseButton GetEventButton() const;

    //! Returns the state of a mouse button: pressed or released.
    /*!
        Returns the state of a mouse button.
        True is the button is pressed. False otherwise.

        @param button_index Button index to query(1 for left mouse button).
        @return True is the button is pressed. False otherwise.
    */
    bool GetButtonState(MouseButton button) const;

    //! Returns the state of a special key: CTRL, Shift, Alt, NumLock...
    /*!
        Returns the state of a key modifier.
        True is the key is pressed. False otherwise.

        @param key_modifier Key modifier be query.
        @return True is the key is pressed. False otherwise.
    */
    bool GetKeyModifierState(KeyModifier key_modifier) const;

    //! Return virtual key code of the key that has triggered the last event.
    /*!
        Return virtual key code of the key that has triggered the last event.
        @return the virtual key code.
    */
    unsigned long GetKeySym() const;
    unsigned short GetKeyRepeatCount() const;
    const char* GetText() const;

    //! Return the state of the Virtual key
    /*!
        Return the state of the Virtual key.
        @param VirtualKey virtual key code.
        @return 1 if the key is pressed, 0 if the key is released.
    */
    unsigned long GetVirtualKeyState(unsigned long VirtualKey) const;

    unsigned long VirtualKeycodeState[NUX_MAX_VK];
    int width, height;
    unsigned int ascii_code;
    int virtual_code;

    int x;
    int y;
    int x_root;
    int y_root;
    int dx;
    int dy;
    int clicks;
    int is_click;

#if defined(NUX_OS_WINDOWS)
    int win32_keycode; // Not used. Just a place holder.
    int win32_keysym;
#elif defined(USE_X11)
    Time          x11_timestamp;  //!< X11 timestamp.
    Window        x11_window;     //!< X11 window.
    unsigned int  x11_key_state;  //!< X11 key state (xevent.xkey.state).
    unsigned int  x11_keycode;    //!< X11 raw keycode.
    KeySym        x11_keysym;     //!< X11 keysym.
#endif

    int wheel_delta;              //!< Wheel delta.

    char            text[NUX_EVENT_TEXT_BUFFER_SIZE];
    char*           dtext;            //!< Dynamically allocated
    unsigned long   key_modifiers;    //!< Key modifiers. A bitwise inclusive OR of values in KeyModifier.
    unsigned long   mouse_state;
    unsigned short  key_repeat_count; //!< Number of time a key is repeated;
    EventType    type;
  };
};

#endif // EVENTS_H
