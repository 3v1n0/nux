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


#ifndef VIRTUALKEYCODESX11_H
#define VIRTUALKEYCODESX11_H

namespace nux
{

// Virtual key codes
// The following table shows the symbolic constant names, hexadecimal values, and mouse or keyboard equivalents for the virtual-key codes used by the system. The codes are listed in numeric order.
// Symbolic constant name 	                                Value (hex)            Mouse or keyboard equivalents

#define     NUX_VK_BACKSPACE                                XK_BackSpace                   //   BACKSPACE key
#define     NUX_VK_TAB                                      XK_Tab                   //   TAB key
#define     NUX_VK_LEFT_TAB                                 XK_ISO_Left_Tab          //   SHIFT TAB key
#define     NUX_VK_CLEAR                                    XK_Clear                   //   CLEAR key
#define     NUX_VK_ENTER                                    XK_Return                   //   ENTER key
#define     NUX_VK_SHIFT                                    XK_Shift_L                   //   SHIFT key
#define     NUX_VK_CONTROL                                  XK_Control_L                   //   CTRL key
#define     NUX_VK_MENU                                     XK_Alt_L                   //   ALT key
#define     NUX_VK_PAUSE                                    XK_Pause                   //   PAUSE key
#define     NUX_VK_CAPITAL                                  XK_Caps_Lock                   //   CAPS LOCK key
#define     NUX_VK_ESCAPE                                   XK_Escape                   //   ESC key
#define     NUX_VK_SPACE                                    XK_space                   //   SPACEBAR
#define     NUX_VK_PAGE_UP                                  XK_Page_Up                   //   PAGE UP key
#define     NUX_VK_PAGE_DOWN                                XK_Page_Down                   //   PAGE DOWN key
#define     NUX_VK_END                                      XK_End                   //   END key
#define     NUX_VK_HOME                                     XK_Home                   //   HOME key        
#define     NUX_VK_LEFT                                     XK_Left                   //   LEFT ARROW key
#define     NUX_VK_UP                                       XK_Up                   //   UP ARROW key
#define     NUX_VK_RIGHT                                    XK_Right                   //   RIGHT ARROW key
#define     NUX_VK_DOWN                                     XK_Down                   //   DOWN ARROW key
#define     NUX_VK_PRINT                                    XK_Print                   //   PRINT key
#define     NUX_VK_EXECUTE                                  XK_Execute                   //   EXECUTE key
#define     NUX_VK_INSERT                                   XK_Insert                   //   INS key
#define     NUX_VK_DELETE                                   XK_Delete                   //   DEL key
/*#define 	NUX_VK_Multiply        							XK_Multiply
#define 	NUX_VK_Add             							XK_Aadd
#define 	NUX_VK_Subtract        							XK_Substract
#define 	NUX_VK_Decimal         							XK_Decimal
#define 	NUX_VK_Divide          							XK_Divide*/
#define     NUX_VK_0                                        XK_0                      //   0 key
#define     NUX_VK_1                                        XK_1                      //   1 key
#define     NUX_VK_2                                        XK_2                      //   2 key
#define     NUX_VK_3                                        XK_3                      //   3 key
#define     NUX_VK_4                                        XK_4                      //   4 key
#define     NUX_VK_5                                        XK_5                      //   5 key
#define     NUX_VK_6                                        XK_6                      //   6 key
#define     NUX_VK_7                                        XK_7                      //   7 key
#define     NUX_VK_8                                        XK_8                      //   8 key
#define     NUX_VK_9                                        XK_9                      //   9 key
#define     NUX_VK_A                                        XK_A                   //   A key
#define     NUX_VK_B                                        XK_B                   //   B key
#define     NUX_VK_C                                        XK_C                   //   C key
#define     NUX_VK_D                                        XK_D                   //   D key
#define     NUX_VK_E                                        XK_E                   //   E key
#define     NUX_VK_F                                        XK_F                   //   F key
#define     NUX_VK_G                                        XK_G                   //   G key
#define     NUX_VK_H                                        XK_H                   //   H key
#define     NUX_VK_I                                        XK_I                   //   I key
#define     NUX_VK_J                                        XK_J                   //   J key
#define     NUX_VK_K                                        XK_K                   //   K key
#define     NUX_VK_L                                        XK_L                   //   L key
#define     NUX_VK_M                                        XK_M                   //   M key
#define     NUX_VK_N                                        XK_N                   //   N key
#define     NUX_VK_O                                        XK_O                   //   O key
#define     NUX_VK_P                                        XK_P                   //   P key
#define     NUX_VK_Q                                        XK_Q                   //   Q key
#define     NUX_VK_R                                        XK_R                   //   R key
#define     NUX_VK_S                                        XK_S                   //   S key
#define     NUX_VK_T                                        XK_T                   //   T key
#define     NUX_VK_U                                        XK_U                   //   U key
#define     NUX_VK_V                                        XK_V                   //   V key
#define     NUX_VK_W                                        XK_W                   //   W key
#define     NUX_VK_X                                        XK_X                   //   X key
#define     NUX_VK_Y                                        XK_Y                   //   Y key
#define     NUX_VK_Z                                        XK_Z                   //   Z key
#define     NUX_VK_a                                        XK_a
#define     NUX_VK_b                                        XK_b
#define     NUX_VK_c                                        XK_c
#define     NUX_VK_d                                        XK_d
#define     NUX_VK_e                                        XK_e
#define     NUX_VK_f                                        XK_f
#define     NUX_VK_g                                        XK_g
#define     NUX_VK_h                                        XK_h
#define     NUX_VK_i                                        XK_i
#define     NUX_VK_j                                        XK_j
#define     NUX_VK_k                                        XK_k
#define     NUX_VK_l                                        XK_l
#define     NUX_VK_m                                        XK_m
#define     NUX_VK_n                                        XK_n
#define     NUX_VK_o                                        XK_o
#define     NUX_VK_p                                        XK_p
#define     NUX_VK_q                                        XK_q
#define     NUX_VK_r                                        XK_r
#define     NUX_VK_s                                        XK_s
#define     NUX_VK_t                                        XK_t
#define     NUX_VK_u                                        XK_u
#define     NUX_VK_v                                        XK_v
#define     NUX_VK_w                                        XK_w
#define     NUX_VK_x                                        XK_x
#define     NUX_VK_y                                        XK_y
#define     NUX_VK_z                                        XK_z
#define     NUX_VK_LWIN                                     XK_Super_L                   //   Left Windows key (Microsoft Natural Keyboard)
#define     NUX_VK_RWIN                                     XK_Super_R                   //   Right Windows key (Microsoft Natural Keyboard)
#define     NUX_VK_NUMPAD0                                  XK_KP_0                   //   Numeric keypad 0 key
#define     NUX_VK_NUMPAD1                                  XK_KP_1                   //   Numeric keypad 1 key
#define     NUX_VK_NUMPAD2                                  XK_KP_2                   //   Numeric keypad 2 key
#define     NUX_VK_NUMPAD3                                  XK_KP_3                   //   Numeric keypad 3 key
#define     NUX_VK_NUMPAD4                                  XK_KP_4                   //   Numeric keypad 4 key
#define     NUX_VK_NUMPAD5                                  XK_KP_5                   //   Numeric keypad 5 key
#define     NUX_VK_NUMPAD6                                  XK_KP_6                   //   Numeric keypad 6 key
#define     NUX_VK_NUMPAD7                                  XK_KP_7                   //   Numeric keypad 7 key
#define     NUX_VK_NUMPAD8                                  XK_KP_8                   //   Numeric keypad 8 key
#define     NUX_VK_NUMPAD9                                  XK_KP_9                   //   Numeric keypad 9 key
#define     NUX_KP_MULTIPLY                                 XK_KP_Multiply                   //   Multiply key
#define     NUX_KP_ADD                                      XK_KP_Add                   //   Add key
#define     NUX_KP_SEPARATOR                                XK_KP_Separator                   //   Separator key
#define     NUX_KP_SUBTRACT                                 XK_KP_Subtract                   //   Subtract key
#define     NUX_KP_DECIMAL                                  XK_KP_Decimal                   //   Decimal key
#define     NUX_KP_DIVIDE                                   XK_KP_Divide                   //   Divide key
#define 	NUX_KP_ENTER   									XK_KP_Enter
#define     NUX_VK_F1                                       XK_F1                   //   F1 key
#define     NUX_VK_F2                                       XK_F2                   //   F2 key
#define     NUX_VK_F3                                       XK_F3                   //   F3 key
#define     NUX_VK_F4                                       XK_F4                   //   F4 key
#define     NUX_VK_F5                                       XK_F5                   //   F5 key
#define     NUX_VK_F6                                       XK_F6                   //   F6 key
#define     NUX_VK_F7                                       XK_F7                   //   F7 key
#define     NUX_VK_F8                                       XK_F8                   //   F8 key
#define     NUX_VK_F9                                       XK_F9                   //   F9 key
#define     NUX_VK_F10                                      XK_F10                   //   F10 key
#define     NUX_VK_F11                                      XK_F11                   //   F11 key
#define     NUX_VK_F12                                      XK_F12                   //   F12 key
#define     NUX_VK_F13                                      XK_F13                   //   F13 key
#define     NUX_VK_F14                                      XK_F14                   //   F14 key
#define     NUX_VK_F15                                      XK_F15                   //   F15 key
#define     NUX_VK_F16                                      XK_F16                   //   F16 key
#define     NUX_VK_F17                                      XK_F17                   //   F17 key
#define     NUX_VK_F18                                      XK_F18                   //   F18 key
#define     NUX_VK_F19                                      XK_F19                   //   F19 key
#define     NUX_VK_F20                                      XK_F20                   //   F20 key
#define     NUX_VK_F21                                      XK_F21                   //   F21 key
#define     NUX_VK_F22                                      XK_F22                   //   F22 key
#define     NUX_VK_F23                                      XK_F23                   //   F23 key
#define     NUX_VK_F24                                      XK_F24                   //   F24 key
#define     NUX_VK_NUMLOCK                                  XK_Num_Lock                   //   NUM LOCK key
#define     NUX_VK_SCROLL                                   XK_Scroll_Lock                   //   SCROLL LOCK key
#define     NUX_VK_LSHIFT                                   XK_Shift_L                   //   Left SHIFT key
#define     NUX_VK_RSHIFT                                   XK_Shift_R                   //   Right SHIFT key
#define     NUX_VK_LCONTROL                                 XK_Control_L                   //   Left CONTROL key
#define     NUX_VK_RCONTROL                                 XK_Control_R                   //   Right CONTROL key
#define     NUX_VK_LMENU                                    XK_Alt_L                   //   Left MENU key
#define     NUX_VK_RMENU                                    XK_Alt_R                   //   Right MENU key
#define     NUX_VK_FF                                       /*0xFF*/                   //   Multimedia keys. See ScanCode keys.

#define     NUX_MAX_VK      256

}

#endif // VIRTUALKEYCODESX11_H

