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


#ifndef VIRTUALKEYCODES_H
#define VIRTUALKEYCODES_H

namespace nux
{

// Virtual key codes
// The following table shows the symbolic constant names, hexadecimal values, and mouse or keyboard equivalents for the virtual-key codes used by the system. The codes are listed in numeric order.
// Symbolic constant name 	                                Value(hex)            Mouse or keyboard equivalents


#define NUX_VK_SPACE               0x020
#define NUX_VK_EXCLAM              0x021
#define NUX_VK_QUOTEDBL            0x022
#define NUX_VK_NUMBERSIGN          0x023
#define NUX_VK_DOLLAR              0x024
#define NUX_VK_PERCENT             0x025
#define NUX_VK_AMPERSAND           0x026
#define NUX_VK_APOSTROPHE          0x027
#define NUX_VK_QUOTERIGHT          0x027  /* deprecated */
#define NUX_VK_PARENLEFT           0x028
#define NUX_VK_PARENRIGHT          0x029
#define NUX_VK_MULTIPLY            0x02a
#define NUX_VK_ADD                 0x02b
#define NUX_VK_COMMA               0x02c
#define NUX_VK_SUBTRACT               0x02d
#define NUX_VK_DECIMAL              0x02e
#define NUX_VK_SLASH               0x02f
#define NUX_VK_0                   0x030
#define NUX_VK_1                   0x031
#define NUX_VK_2                   0x032
#define NUX_VK_3                   0x033
#define NUX_VK_4                   0x034
#define NUX_VK_5                   0x035
#define NUX_VK_6                   0x036
#define NUX_VK_7                   0x037
#define NUX_VK_8                   0x038
#define NUX_VK_9                   0x039
#define NUX_VK_COLON               0x03a
#define NUX_VK_SEMICOLON           0x03b
#define NUX_VK_LESS                0x03c
#define NUX_VK_EQUAL               0x03d
#define NUX_VK_GREATER             0x03e
#define NUX_VK_QUESTION            0x03f
#define NUX_VK_AT                  0x040
#define NUX_VK_A                   0x041
#define NUX_VK_B                   0x042
#define NUX_VK_C                   0x043
#define NUX_VK_D                   0x044
#define NUX_VK_E                   0x045
#define NUX_VK_F                   0x046
#define NUX_VK_G                   0x047
#define NUX_VK_H                   0x048
#define NUX_VK_I                   0x049
#define NUX_VK_J                   0x04a
#define NUX_VK_K                   0x04b
#define NUX_VK_L                   0x04c
#define NUX_VK_M                   0x04d
#define NUX_VK_N                   0x04e
#define NUX_VK_O                   0x04f
#define NUX_VK_P                   0x050
#define NUX_VK_Q                   0x051
#define NUX_VK_R                   0x052
#define NUX_VK_S                   0x053
#define NUX_VK_T                   0x054
#define NUX_VK_U                   0x055
#define NUX_VK_V                   0x056
#define NUX_VK_W                   0x057
#define NUX_VK_X                   0x058
#define NUX_VK_Y                   0x059
#define NUX_VK_Z                   0x05a
#define NUX_VK_BRACKETLEFT         0x05b
#define NUX_VK_BACKSLASH           0x05c
#define NUX_VK_BRACKETRIGHT        0x05d
#define NUX_VK_ASCIICIRCUM         0x05e
#define NUX_VK_UNDERSCORE          0x05f
#define NUX_VK_GRAVE               0x060
#define NUX_VK_QUOTELEFT           0x060  /* deprecated */
#define NUX_VK_a                   0x061
#define NUX_VK_b                   0x062
#define NUX_VK_c                   0x063
#define NUX_VK_d                   0x064
#define NUX_VK_e                   0x065
#define NUX_VK_f                   0x066
#define NUX_VK_g                   0x067
#define NUX_VK_h                   0x068
#define NUX_VK_i                   0x069
#define NUX_VK_j                   0x06a
#define NUX_VK_k                   0x06b
#define NUX_VK_l                   0x06c
#define NUX_VK_m                   0x06d
#define NUX_VK_n                   0x06e
#define NUX_VK_o                   0x06f
#define NUX_VK_p                   0x070
#define NUX_VK_q                   0x071
#define NUX_VK_r                   0x072
#define NUX_VK_s                   0x073
#define NUX_VK_t                   0x074
#define NUX_VK_u                   0x075
#define NUX_VK_v                   0x076
#define NUX_VK_w                   0x077
#define NUX_VK_x                   0x078
#define NUX_VK_y                   0x079
#define NUX_VK_z                   0x07a
#define NUX_VK_BRACELEFT           0x07b
#define NUX_VK_BAR                 0x07c
#define NUX_VK_BRACERIGHT          0x07d
#define NUX_VK_ASCIITILDE          0x07e

// Modifiers
#define NUX_VK_LSHIFT                                   0xA0                   //   Left SHIFT key
#define NUX_VK_RSHIFT                                   0xA1                   //   Right SHIFT key
#define NUX_VK_LCONTROL                                 0xA2                   //   Left CONTROL key
#define NUX_VK_RCONTROL                                 0xA3                   //   Right CONTROL key
// To match VK in X11 file.
#define NUX_VK_CONTROL                                  NUX_VK_LCONTROL
#define NUX_VK_CAPITAL                                  0x14                   //   CAPS LOCK key
#define NUX_VK_LSUPER                                   0x5B                   //   Left Windows key(Microsoft Natural Keyboard)
#define NUX_VK_RSUPER                                   0x5C                   //   Right Windows key(Microsoft Natural Keyboard)
// To match VK in X11 file.
#define NUX_VK_LWIN                                     NUX_VK_LSUPER
#define NUX_VK_RWIN                                     NUX_VK_RSUPER
#define NUX_VK_LALT                                     0xA4                   //   Left MENU key
#define NUX_VK_RALT                                     0xA5                   //   Right MENU key

// Cursor control and motion
#define NUX_VK_HOME                                     0x24                   //   HOME key        
#define NUX_VK_LEFT                                     0x25                   //   LEFT ARROW key
#define NUX_VK_UP                                       0x26                   //   UP ARROW key
#define NUX_VK_RIGHT                                    0x27                   //   RIGHT ARROW key
#define NUX_VK_DOWN                                     0x28                   //   DOWN ARROW key
#define NUX_VK_PAGE_UP                                  0x21                   //   PAGE UP key
#define NUX_VK_PAGE_DOWN                                0x22                   //   PAGE DOWN key
#define NUX_VK_END                                      0x23                   //   END key

// Functions
#define NUX_VK_BACKSPACE                                0x08                   //   BACKSPACE key
#define NUX_VK_ESCAPE                                   0x1B                   //   ESC key
#define NUX_VK_TAB                                      0x09                   //   TAB key
#define NUX_VK_ENTER                                    0x0D                   //   ENTER key
#define NUX_VK_NUMLOCK                                  0x90                   //   NUM LOCK key
#define NUX_VK_SCROLL                                   0x91                   //   SCROLL LOCK key
#define NUX_VK_DELETE                                   0x2E                   //   DEL key
#define NUX_VK_INSERT                                   0x2D                   //   INS key



#define NUX_VK_CANCEL                                   0x03                   //   Control-break processing
#define NUX_VK_CLEAR                                    0x0C                   //   CLEAR key
#define NUX_VK_SHIFT                                    0x10                   //   SHIFT key
#define NUX_VK_PAUSE                                    0x13                   //   PAUSE key

// #define NUX_VK_MULTIPLY                                 0x6A                   //   Multiply key
// #define NUX_VK_ADD                                      0x6B                   //   Add key
// #define NUX_VK_SEPARATOR                                0x6C                   //   Separator key
// #define NUX_VK_SUBTRACT                                 0x6D                   //   Subtract key
// #define NUX_VK_DECIMAL                                  0x6E                   //   Decimal key
// #define NUX_VK_DIVIDE                                   0x6F                   //   Divide key

#define NUX_KP_0     0xFFB0
#define NUX_KP_1     0xFFB1
#define NUX_KP_2     0xFFB2
#define NUX_KP_3     0xFFB3
#define NUX_KP_4     0xFFB4
#define NUX_KP_5     0xFFB5
#define NUX_KP_6     0xFFB6
#define NUX_KP_7     0xFFB7
#define NUX_KP_8     0xFFB8
#define NUX_KP_9     0xFFB9

#define NUX_KP_HOME        0xFF95
#define NUX_KP_LEFT        0xFF96
#define NUX_KP_UP          0xFF97
#define NUX_KP_RIGHT       0xFF98
#define NUX_KP_DOWN        0xFF99
#define NUX_KP_PRIOR       0xFF9A
#define NUX_KP_PAGE_UP     0xFF9A
#define NUX_KP_NEXT        0xFF9B
#define NUX_KP_PAGE_DOWN   0xFF9B
#define NUX_KP_END         0xFF9C
#define NUX_KP_BEGIN       0xFF9D
#define NUX_KP_INSERT      0xFF9E
#define NUX_KP_DELETE      0xFF9F
#define NUX_KP_EQUAL       0xFFBD  /* equals */
#define NUX_KP_MULTIPLY    0xFFAA
#define NUX_KP_ADD         0xFFAB
#define NUX_KP_SEPARATOR   0xFFAC  /* separator, often comma */
#define NUX_KP_SUBTRACT    0xFFAD
#define NUX_KP_DECIMAL     0xFFAE
#define NUX_KP_DIVIDE      0xFFAF

#define NUX_KP_ENTER       0xAAAA // todo
#define NUX_VK_LEFT_TAB    0xAAAB // todo

#define NUX_VK_F1          0x70                   //   F1 key
#define NUX_VK_F2          0x71                   //   F2 key
#define NUX_VK_F3          0x72                   //   F3 key
#define NUX_VK_F4          0x73                   //   F4 key
#define NUX_VK_F5          0x74                   //   F5 key
#define NUX_VK_F6          0x75                   //   F6 key
#define NUX_VK_F7          0x76                   //   F7 key
#define NUX_VK_F8          0x77                   //   F8 key
#define NUX_VK_F9          0x78                   //   F9 key
#define NUX_VK_F10         0x79                   //   F10 key
#define NUX_VK_F11         0x7A                   //   F11 key
#define NUX_VK_F12         0x7B                   //   F12 key
#define NUX_VK_F13         0x7C                   //   F13 key
#define NUX_VK_F14         0x7D                   //   F14 key
#define NUX_VK_F15         0x7E                   //   F15 key
#define NUX_VK_F16         0x7F                   //   F16 key
#define NUX_VK_F17         0x80                   //   F17 key
#define NUX_VK_F18         0x81                   //   F18 key
#define NUX_VK_F19         0x82                   //   F19 key
#define NUX_VK_F20         0x83                   //   F20 key
#define NUX_VK_F21         0x84                   //   F21 key
#define NUX_VK_F22         0x85                   //   F22 key
#define NUX_VK_F23         0x86                   //   F23 key
#define NUX_VK_F24         0x87                   //   F24 key


#define     NUX_MAX_VK      256

}

#endif // VIRTUALKEYCODES_H

