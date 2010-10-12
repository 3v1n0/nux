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


#ifndef VIRTUALKEYCODES_H
#define VIRTUALKEYCODES_H

namespace nux
{

// Virtual key codes
// The following table shows the symbolic constant names, hexadecimal values, and mouse or keyboard equivalents for the virtual-key codes used by the system. The codes are listed in numeric order.
// Symbolic constant name 	                                Value (hex)            Mouse or keyboard equivalents

#define     NUX_VK_LBUTTON                                  0x01                   //   Left mouse button
#define     NUX_VK_RBUTTON                                  0x02                   //   Right mouse button
#define     NUX_VK_CANCEL                                   0x03                   //   Control-break processing
#define     NUX_VK_MBUTTON                                  0x04                   //   Middle mouse button (three-button mouse)
#define     NUX_VK_XBUTTON1                                 0x05                   //   Windows 2000/XP: X1 mouse button
#define     NUX_VK_XBUTTON2                                 0x06                   //   Windows 2000/XP: X2 mouse button
#define     NUX_VK_07                                       0x07                   //   Undefined
#define     NUX_VK_BACKSPACE                                0x08                   //   BACKSPACE key
#define     NUX_VK_TAB                                      0x09                   //   TAB key
#define     NUX_VK_0A                                       0x0A                   //   Reserved
#define     NUX_VK_0B                                       0x0B                   //   Reserved
#define     NUX_VK_CLEAR                                    0x0C                   //   CLEAR key
#define     NUX_VK_ENTER                                    0x0D                   //   ENTER key
#define     NUX_VK_0E                                       0x0E                   //   Undefined
#define     NUX_VK_0F                                       0x0F                   //   Undefined
#define     NUX_VK_SHIFT                                    0x10                   //   SHIFT key
#define     NUX_VK_CONTROL                                  0x11                   //   CTRL key
#define     NUX_VK_MENU                                     0x12                   //   ALT key
#define     NUX_VK_PAUSE                                    0x13                   //   PAUSE key
#define     NUX_VK_CAPITAL                                  0x14                   //   CAPS LOCK key
#define     NUX_VK_KANA                                     0x15                   //   IME Kana mode
#define     NUX_VK_HANGUEL                                  0x15                   //   IME Hanguel mode (maintained for compatibility; use VK_HANGUL)
#define     NUX_VK_HANGUL                                   0x15                   //   IME Hangul mode
#define     NUX_VK_16                                       0x16                   //   Undefined
#define     NUX_VK_JUNJA                                    0x17                   //   IME Junja mode
#define     NUX_VK_FINAL                                    0x18                   //   IME final mode
#define     NUX_VK_HANJA                                    0x19                   //   IME Hanja mode
#define     NUX_VK_KANJI                                    0x19                   //   IME Kanji mode
#define     NUX_VK_1A                                       0x1A                   //   Undefined
#define     NUX_VK_ESCAPE                                   0x1B                   //   ESC key
#define     NUX_VK_CONVERT                                  0x1C                   //   IME convert (Reserved for Kanji systems)
#define     NUX_VK_NONCONVERT                               0x1D                   //   IME nonconvert (Reserved for Kanji systems)
#define     NUX_VK_ACCEPT                                   0x1E                   //   IME accept (Reserved for Kanji systems)
#define     NUX_VK_MODECHANGE                               0x1F                   //   IME mode change request (Reserved for Kanji systems)
#define     NUX_VK_SPACE                                    0x20                   //   SPACEBAR
#define     NUX_VK_PAGE_UP                                  0x21                   //   PAGE UP key
#define     NUX_VK_PAGE_DOWN                                0x22                   //   PAGE DOWN key
#define     NUX_VK_END                                      0x23                   //   END key
#define     NUX_VK_HOME                                     0x24                   //   HOME key        
#define     NUX_VK_LEFT                                     0x25                   //   LEFT ARROW key
#define     NUX_VK_UP                                       0x26                   //   UP ARROW key
#define     NUX_VK_RIGHT                                    0x27                   //   RIGHT ARROW key
#define     NUX_VK_DOWN                                     0x28                   //   DOWN ARROW key
#define     NUX_VK_SELECT                                   0x29                   //   SELECT key
#define     NUX_VK_PRINT                                    0x2A                   //   PRINT key
#define     NUX_VK_EXECUTE                                  0x2B                   //   EXECUTE key
#define     NUX_VK_SNAPSHOT                                 0x2C                   //   PRINT SCREEN key for Windows 3.0 and later
#define     NUX_VK_INSERT                                   0x2D                   //   INS key
#define     NUX_VK_DELETE                                   0x2E                   //   DEL key
#define     NUX_VK_HELP                                     0x2F                   //   HELP key
#define     NUX_VK_0                                        0x30                   //   0 key
#define     NUX_VK_1                                        0x31                   //   1 key
#define     NUX_VK_2                                        0x32                   //   2 key
#define     NUX_VK_3                                        0x33                   //   3 key
#define     NUX_VK_4                                        0x34                   //   4 key
#define     NUX_VK_5                                        0x35                   //   5 key
#define     NUX_VK_6                                        0x36                   //   6 key
#define     NUX_VK_7                                        0x37                   //   7 key
#define     NUX_VK_8                                        0x38                   //   8 key
#define     NUX_VK_9                                        0x39                   //   9 key
#define     NUX_VK_3A                                       0x3A                   //   Undefined
#define     NUX_VK_3B                                       0x3B                   //   Undefined
#define     NUX_VK_3C                                       0x3C                   //   Undefined
#define     NUX_VK_3D                                       0x3D                   //   Undefined
#define     NUX_VK_3E                                       0x3E                   //   Undefined
#define     NUX_VK_3F                                       0x3F                   //   Undefined
#define     NUX_VK_40                                       0x40                   //   Undefined
#define     NUX_VK_A                                        0x41                   //   A key
#define     NUX_VK_B                                        0x42                   //   B key
#define     NUX_VK_C                                        0x43                   //   C key
#define     NUX_VK_D                                        0x44                   //   D key
#define     NUX_VK_E                                        0x45                   //   E key
#define     NUX_VK_F                                        0x46                   //   F key
#define     NUX_VK_G                                        0x47                   //   G key
#define     NUX_VK_H                                        0x48                   //   H key
#define     NUX_VK_I                                        0x49                   //   I key
#define     NUX_VK_J                                        0x4A                   //   J key
#define     NUX_VK_K                                        0x4B                   //   K key
#define     NUX_VK_L                                        0x4C                   //   L key
#define     NUX_VK_M                                        0x4D                   //   M key
#define     NUX_VK_N                                        0x4E                   //   N key
#define     NUX_VK_O                                        0x4F                   //   O key
#define     NUX_VK_P                                        0x50                   //   P key
#define     NUX_VK_Q                                        0x51                   //   Q key
#define     NUX_VK_R                                        0x52                   //   R key
#define     NUX_VK_S                                        0x53                   //   S key
#define     NUX_VK_T                                        0x54                   //   T key
#define     NUX_VK_U                                        0x55                   //   U key
#define     NUX_VK_V                                        0x56                   //   V key
#define     NUX_VK_W                                        0x57                   //   W key
#define     NUX_VK_X                                        0x58                   //   X key
#define     NUX_VK_Y                                        0x59                   //   Y key
#define     NUX_VK_Z                                        0x5A                   //   Z key
#define     NUX_VK_LWIN                                     0x5B                   //   Left Windows key (Microsoft Natural Keyboard)
#define     NUX_VK_RWIN                                     0x5C                   //   Right Windows key (Microsoft Natural Keyboard)
#define     NUX_VK_APPS                                     0x5D                   //   Applications key (Microsoft Natural Keyboard)
#define     NUX_VK_5E                                       0x5E                   //   Reserved
#define     NUX_VK_SLEEP                                    0x5F                   //   Computer Sleep key
#define     NUX_VK_NUMPAD0                                  0x60                   //   Numeric keypad 0 key
#define     NUX_VK_NUMPAD1                                  0x61                   //   Numeric keypad 1 key
#define     NUX_VK_NUMPAD2                                  0x62                   //   Numeric keypad 2 key
#define     NUX_VK_NUMPAD3                                  0x63                   //   Numeric keypad 3 key
#define     NUX_VK_NUMPAD4                                  0x64                   //   Numeric keypad 4 key
#define     NUX_VK_NUMPAD5                                  0x65                   //   Numeric keypad 5 key
#define     NUX_VK_NUMPAD6                                  0x66                   //   Numeric keypad 6 key
#define     NUX_VK_NUMPAD7                                  0x67                   //   Numeric keypad 7 key
#define     NUX_VK_NUMPAD8                                  0x68                   //   Numeric keypad 8 key
#define     NUX_VK_NUMPAD9                                  0x69                   //   Numeric keypad 9 key
#define     NUX_VK_MULTIPLY                                 0x6A                   //   Multiply key
#define     NUX_VK_ADD                                      0x6B                   //   Add key
#define     NUX_VK_SEPARATOR                                0x6C                   //   Separator key
#define     NUX_VK_SUBTRACT                                 0x6D                   //   Subtract key
#define     NUX_VK_DECIMAL                                  0x6E                   //   Decimal key
#define     NUX_VK_DIVIDE                                   0x6F                   //   Divide key
#define     NUX_VK_F1                                       0x70                   //   F1 key
#define     NUX_VK_F2                                       0x71                   //   F2 key
#define     NUX_VK_F3                                       0x72                   //   F3 key
#define     NUX_VK_F4                                       0x73                   //   F4 key
#define     NUX_VK_F5                                       0x74                   //   F5 key
#define     NUX_VK_F6                                       0x75                   //   F6 key
#define     NUX_VK_F7                                       0x76                   //   F7 key
#define     NUX_VK_F8                                       0x77                   //   F8 key
#define     NUX_VK_F9                                       0x78                   //   F9 key
#define     NUX_VK_F10                                      0x79                   //   F10 key
#define     NUX_VK_F11                                      0x7A                   //   F11 key
#define     NUX_VK_F12                                      0x7B                   //   F12 key
#define     NUX_VK_F13                                      0x7C                   //   F13 key
#define     NUX_VK_F14                                      0x7D                   //   F14 key
#define     NUX_VK_F15                                      0x7E                   //   F15 key
#define     NUX_VK_F16                                      0x7F                   //   F16 key
#define     NUX_VK_F17                                      0x80                   //   F17 key
#define     NUX_VK_F18                                      0x81                   //   F18 key
#define     NUX_VK_F19                                      0x82                   //   F19 key
#define     NUX_VK_F20                                      0x83                   //   F20 key
#define     NUX_VK_F21                                      0x84                   //   F21 key
#define     NUX_VK_F22                                      0x85                   //   F22 key
#define     NUX_VK_F23                                      0x86                   //   F23 key
#define     NUX_VK_F24                                      0x87                   //   F24 key
#define     NUX_VK_88                                       0x88                   //   Unassigned
#define     NUX_VK_89                                       0x89                   //   Unassigned
#define     NUX_VK_8A                                       0x8A                   //   Unassigned
#define     NUX_VK_8B                                       0x8B                   //   Unassigned
#define     NUX_VK_8C                                       0x8C                   //   Unassigned
#define     NUX_VK_8D                                       0x8D                   //   Unassigned
#define     NUX_VK_8E                                       0x8E                   //   Unassigned
#define     NUX_VK_8F                                       0x8F                   //   Unassigned
#define     NUX_VK_NUMLOCK                                  0x90                   //   NUM LOCK key
#define     NUX_VK_SCROLL                                   0x91                   //   SCROLL LOCK key
#define     NUX_VK_OEM_NEC_EQUAL                            0x92                   //   NEC PC-9800 kbd definitions: '=' key on numpad
#define     NUX_VK_OEM_FJ_JISHO                             0x92                   //   Fujitsu/OASYS kbd definitions: 'Dictionary' key
#define     NUX_VK_OEM_FJ_MASSHOU                           0x93                   //   Fujitsu/OASYS kbd definitions: 'Unregister word' key
#define     NUX_VK_OEM_FJ_TOUROKU                           0x94                   //   Fujitsu/OASYS kbd definitions: 'Register word' key
#define     NUX_VK_OEM_FJ_LOYA                              0x95                   //   Fujitsu/OASYS kbd definitions: 'Left OYAYUBI' key
#define     NUX_VK_OEM_FJ_ROYA                              0x96                   //   Fujitsu/OASYS kbd definitions: 'Right OYAYUBI' key
#define     NUX_VK_97                                       0x97                   //   Unassigned
#define     NUX_VK_98                                       0x98                   //   Unassigned
#define     NUX_VK_99                                       0x99                   //   Unassigned
#define     NUX_VK_9A                                       0x9A                   //   Unassigned
#define     NUX_VK_9B                                       0x9B                   //   Unassigned
#define     NUX_VK_9C                                       0x9C                   //   Unassigned
#define     NUX_VK_9D                                       0x9D                   //   Unassigned
#define     NUX_VK_9E                                       0x9E                   //   Unassigned
#define     NUX_VK_9F                                       0x9F                   //   Unassigned
#define     NUX_VK_LSHIFT                                   0xA0                   //   Left SHIFT key
#define     NUX_VK_RSHIFT                                   0xA1                   //   Right SHIFT key
#define     NUX_VK_LCONTROL                                 0xA2                   //   Left CONTROL key
#define     NUX_VK_RCONTROL                                 0xA3                   //   Right CONTROL key
#define     NUX_VK_LMENU                                    0xA4                   //   Left MENU key
#define     NUX_VK_RMENU                                    0xA5                   //   Right MENU key
#define     NUX_VK_BROWSER_BACK                             0xA6                   //   Windows 2000/XP: Browser Back key
#define     NUX_VK_BROWSER_FORWARD                          0xA7                   //   Windows 2000/XP: Browser Forward key
#define     NUX_VK_BROWSER_REFRESH                          0xA8                   //   Windows 2000/XP: Browser Refresh key
#define     NUX_VK_BROWSER_STOP                             0xA9                   //   Windows 2000/XP: Browser Stop key
#define     NUX_VK_BROWSER_SEARCH                           0xAA                   //   Windows 2000/XP: Browser Search key
#define     NUX_VK_BROWSER_FAVORITES                        0xAB                   //   Windows 2000/XP: Browser Favorites key
#define     NUX_VK_BROWSER_HOME                             0xAC                   //   Windows 2000/XP: Browser Start and Home key
#define     NUX_VK_VOLUME_MUTE                              0xAD                   //   Windows 2000/XP: Volume Mute key
#define     NUX_VK_VOLUME_DOWN                              0xAE                   //   Windows 2000/XP: Volume Down key
#define     NUX_VK_VOLUME_UP                                0xAF                   //   Windows 2000/XP: Volume Up key
#define     NUX_VK_MEDIA_NEXT_TRACK                         0xB0                   //   Windows 2000/XP: Next Track key
#define     NUX_VK_MEDIA_PREV_TRACK                         0xB1                   //   Windows 2000/XP: Previous Track key
#define     NUX_VK_MEDIA_STOP                               0xB2                   //   Windows 2000/XP: Stop Media key
#define     NUX_VK_MEDIA_PLAY_PAUSE                         0xB3                   //   Windows 2000/XP: Play/Pause Media key
#define     NUX_VK_LAUNCH_MAIL                              0xB4                   //   Windows 2000/XP: Start Mail key
#define     NUX_VK_LAUNCH_MEDIA_SELECT                      0xB5                   //   Windows 2000/XP: Select Media key
#define     NUX_VK_LAUNCH_APP1                              0xB6                   //   Windows 2000/XP: Start Application 1 key
#define     NUX_VK_LAUNCH_APP2                              0xB7                   //   Windows 2000/XP: Start Application 2 key
#define     NUX_VK_B8                                       0xB8                   //   Reserved
#define     NUX_VK_B9                                       0xB8                   //   Reserved
#define     NUX_VK_OEM_1                                    0xBA                   //   Windows 2000/XP: For the US standard keyboard, the ';:' key
#define     NUX_VK_OEM_PLUS                                 0xBB                   //   Windows 2000/XP: For any country/region, the '+' key
#define     NUX_VK_OEM_COMMA                                0xBC                   //   Windows 2000/XP: For any country/region, the ',' key
#define     NUX_VK_OEM_MINUS                                0xBD                   //   Windows 2000/XP: For any country/region, the '-' key
#define     NUX_VK_OEM_PERIOD                               0xBE                   //   Windows 2000/XP: For any country/region, the '.' key
#define     NUX_VK_OEM_2                                    0xBF                   //   Windows 2000/XP: For the US standard keyboard, the '/?' key
#define     NUX_VK_OEM_3                                    0xC0                   //   Windows 2000/XP: For the US standard keyboard, the '`~' key
#define     NUX_VK_C1                                       0xC1                   //   Reserved
#define     NUX_VK_C2                                       0xC2                   //   Reserved
#define     NUX_VK_C3                                       0xC3                   //   Reserved
#define     NUX_VK_C4                                       0xC4                   //   Reserved
#define     NUX_VK_C5                                       0xC5                   //   Reserved
#define     NUX_VK_C6                                       0xC6                   //   Reserved
#define     NUX_VK_C7                                       0xC7                   //   Reserved
#define     NUX_VK_C8                                       0xC8                   //   Reserved
#define     NUX_VK_C9                                       0xC9                   //   Reserved
#define     NUX_VK_CA                                       0xCA                   //   Reserved
#define     NUX_VK_CB                                       0xCB                   //   Reserved
#define     NUX_VK_CC                                       0xCC                   //   Reserved
#define     NUX_VK_CD                                       0xCD                   //   Reserved
#define     NUX_VK_CE                                       0xCE                   //   Reserved
#define     NUX_VK_D8                                       0xD8                   //   Unassigned
#define     NUX_VK_D9                                       0xD9                   //   Unassigned
#define     NUX_VK_DA                                       0xDA                   //   Unassigned
#define     NUX_VK_OEM_4                                    0xDB                   //   Windows 2000/XP: For the US standard keyboard, the '[{' key
#define     NUX_VK_OEM_5                                    0xDC                   //   Windows 2000/XP: For the US standard keyboard, the '\|' key
#define     NUX_VK_OEM_6                                    0xDD                   //   Windows 2000/XP: For the US standard keyboard, the ']}' key
#define     NUX_VK_OEM_7                                    0xDE                   //   Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key
#define     NUX_VK_OEM_8                                    0xDF                   //   
#define     NUX_VK_E0                                       0xE0                   //   Reserved
#define     NUX_VK_E1                                       0xE1                   //   OEM specific
#define     NUX_VK_OEM_102                                  0xE2                   //   Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
#define     NUX_VK_E3                                       0xE3                   //   OEM specific
#define     NUX_VK_E4                                       0xE4                   //   OEM specific
#define     NUX_VK_PROCESSKEY                               0xE5                   //   Windows 95/98, Windows NT 4.0, Windows 2000/XP: IME PROCESS key
#define     NUX_VK_E6                                       0xE6                   //   OEM specific
#define     NUX_VK_PACKET                                   0xE7                   //   Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. The VK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT , SendInput , WM_KEYDOWN , and WM_KEYUP
#define     NUX_VK_E8                                       0xE8                   //   Unassigned
#define     NUX_VK_OEM_RESET                                0xE9                   //   Only used by Nokia.
#define     NUX_VK_OEM_JUMP                                 0xEA                   //   Only used by Nokia.
#define     NUX_VK_OEM_PA1                                  0xEB                   //   Only used by Nokia.
#define     NUX_VK_OEM_PA2                                  0xEC                   //   Only used by Nokia.
#define     NUX_VK_OEM_PA3                                  0xED                   //   Only used by Nokia.
#define     NUX_VK_OEM_WSCTRL                               0xEE                   //   Only used by Nokia.
#define     NUX_VK_OEM_CUSEL                                0xEF                   //   Only used by Nokia.
#define     NUX_VK_OEM_ATTN                                 0xF0                   //   Only used by Nokia.
#define     NUX_VK_OEM_FINNISH                              0xF1                   //   Only used by Nokia.
#define     NUX_VK_OEM_COPY                                 0xF2                   //   Only used by Nokia.
#define     NUX_VK_OEM_AUTO                                 0xF3                   //   Only used by Nokia.
#define     NUX_VK_OEM_ENLW                                 0xF4                   //   Only used by Nokia.
#define     NUX_VK_OEM_BACKTAB                              0xF5                   //   Only used by Nokia.
#define     NUX_VK_ATTN                                     0xF6                   //   Attn key
#define     NUX_VK_CRSEL                                    0xF7                   //   CrSel key
#define     NUX_VK_EXSEL                                    0xF8                   //   ExSel key
#define     NUX_VK_EREOF                                    0xF9                   //   Erase EOF key
#define     NUX_VK_PLAY                                     0xFA                   //   Play key
#define     NUX_VK_ZOOM                                     0xFB                   //   Zoom key
#define     NUX_VK_NONAME                                   0xFC                   //   Reserved for future use.
#define     NUX_VK_PA1                                      0xFD                   //   PA1 key
#define     NUX_VK_OEM_CLEAR                                0xFE                   //   Clear key
#define     NUX_VK_FF                                       0xFF                   //   Multimedia keys. See ScanCode keys.

#define     NUX_MAX_VK      256

}

#endif // VIRTUALKEYCODES_H

