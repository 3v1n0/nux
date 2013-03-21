/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */

#include "Nux/Nux.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include "nux_automated_test_framework.h"

#define MAX_KEYS 4 // Max keys

int NuxAutomatedTestFramework::mouse_motion_time_span = 1000; // milliseconds
int NuxAutomatedTestFramework::mouse_click_time_span = 300;   // milliseconds
int NuxAutomatedTestFramework::minimum_sleep_time = 600;      // milliseconds
int NuxAutomatedTestFramework::safety_border_inside_view = 1; // pixels

NuxAutomatedTestFramework::NuxAutomatedTestFramework(nux::WindowThread *window_thread)
 : ready_to_start_(false)
 , display_(NULL)
 , window_thread_(window_thread)
 , window_x_(0)
 , window_y_(0)
 , window_width_(0)
 , window_height_(0)
 , terminate_when_test_over_(false)
 , composition_keys_supported_(false)
{
}

NuxAutomatedTestFramework::~NuxAutomatedTestFramework()
{
  XCloseDisplay(display_);
}

void NuxAutomatedTestFramework::WarnCompositionKeysNotSupported()
{
  nuxWarningMsg("Composition keys are not supported by your server, "
                "not running the composition keys tests");
}

bool NuxAutomatedTestFramework::CompositionKeysSupported()
{
  return composition_keys_supported_;
}

void NuxAutomatedTestFramework::SetTerminateProgramWhenDone(bool terminate)
{
  terminate_when_test_over_ = terminate;
}

bool NuxAutomatedTestFramework::WhenDoneTerminateProgram()
{
  return terminate_when_test_over_;
}

void NuxAutomatedTestFramework::Startup()
{
  display_ = XOpenDisplay(NULL);
  nux::Geometry rect = window_thread_->GetGraphicsDisplay().GetWindowGeometry();
  //nuxDebugMsg("Window geometry: (%d, %d, %d, %d)", rect.x, rect.y, rect.width, rect.height);

  int min, max, per_keycode;

  // Get the min and max keycodes
  XDisplayKeycodes(display_, &min, &max);

  // Must be called with:
  // first_keycode: min,
  // keycode_count: max - min
  KeySym *syms = XGetKeyboardMapping(display_, min, max - min, &per_keycode);

  // Number of keysyms is (max - min) * keysyms_per_keycode
  for (int i = 0; i < (max - min) * per_keycode; ++i)
  {
    if (syms[i] == XK_Multi_key)
      composition_keys_supported_ = true;
  }

  XFree(syms);

  window_x_ = rect.x;
  window_y_ = rect.y;
  window_width_ = rect.width;
  window_height_ = rect.height;
}

void NuxAutomatedTestFramework::ViewSendMouseClick(nux::View *view, int button)
{
  nux::Rect r;
  if (view)
  {
    r = view->GetAbsoluteGeometry();
    r.OffsetPosition(window_x_ + r.width/2, window_y_ + r.height/2);
  }
  else
  {
    r = window_thread_->GetGraphicsDisplay().GetWindowGeometry();
    r.OffsetPosition(r.width/2, r.height/2);
  }

  SendFakeMouseMotionEvent(r.x, r.y, NuxAutomatedTestFramework::mouse_motion_time_span);
  SendFakeMouseEvent(button, true);
  nux::SleepForMilliseconds(NuxAutomatedTestFramework::mouse_click_time_span);
  SendFakeMouseEvent(button, false);

  XSync(display_, False);
  nux::SleepForMilliseconds(NuxAutomatedTestFramework::minimum_sleep_time);
}

void NuxAutomatedTestFramework::ViewSendMouseDoubleClick(nux::View *view, int button)
{
  nux::Rect r;
  if (view)
  {
    r = view->GetAbsoluteGeometry();
    r.OffsetPosition(window_x_ + r.width/2, window_y_ + r.height/2);
  }
  else
  {
    r = window_thread_->GetGraphicsDisplay().GetWindowGeometry();
    r.OffsetPosition(r.width/2, r.height/2);
  }

  // Send the mouse to the center of the view
  SendFakeMouseMotionEvent(r.x, r.y, NuxAutomatedTestFramework::mouse_motion_time_span);

  XTestFakeButtonEvent(display_, button, true,  CurrentTime);
  XTestFakeButtonEvent(display_, button, false,  CurrentTime);
  XTestFakeButtonEvent(display_, button, true,  CurrentTime);
  XTestFakeButtonEvent(display_, button, false,  CurrentTime);
  XSync(display_, False);
  nux::SleepForMilliseconds(NuxAutomatedTestFramework::minimum_sleep_time);
}

void NuxAutomatedTestFramework::ViewSendMouseDown(nux::View *view, int button)
{
  XEvent event;
  /* Get the current pointer position */
  XQueryPointer(display_, RootWindow(display_, 0),
        &event.xbutton.root, &event.xbutton.window,
        &event.xbutton.x_root, &event.xbutton.y_root,
        &event.xbutton.x, &event.xbutton.y,
        &event.xbutton.state);

  int current_x = event.xbutton.x - window_x_;
  int current_y = event.xbutton.y - window_y_;

  nux::Rect r = view->GetAbsoluteGeometry();

  if (!r.IsInside(nux::Point(current_x, current_y)))
  {
    // The mouse pointer is not inside the view.
    // Move the mouse pointer to the center of the view.
    r.OffsetPosition(window_x_, window_y_);

    // Go to the center of the view
    int view_center_x = r.x + r.width/2;
    int view_center_y = r.y + r.height/2;
    SendFakeMouseMotionEvent(view_center_x, view_center_y, NuxAutomatedTestFramework::mouse_motion_time_span);
    nux::SleepForMilliseconds(minimum_sleep_time);
  }
  SendFakeMouseEvent(button, true);
}

void NuxAutomatedTestFramework::ViewSendMouseUp(nux::View * /* view */, int button)
{
  //   nux::Rect r = view->GetAbsoluteGeometry();
  // r.OffsetPosition(window_x_, window_y_);

  // int view_center_x = r.x + r.width/2;
  // int view_center_y = r.y + r.height/2;

  // SendFakeMouseMotionEvent(view_center_x, view_center_y, 1000);
  // nux::SleepForMilliseconds(minimum_sleep_time);
  SendFakeMouseEvent(button, false);
}

void NuxAutomatedTestFramework::ViewSendMouseDrag(nux::View *view, int button_index, int x0, int y0, int x1, int y1)
{
  nux::Rect r0 = view->GetAbsoluteGeometry();
  nux::Rect r1 = view->GetAbsoluteGeometry();
  r0.OffsetPosition(window_x_ + x0, window_y_ + y0);
  r1.OffsetPosition(window_x_ + x1, window_y_ + y1);

  // Go to first point
  SendFakeMouseMotionEvent(r0.x, r0.y, NuxAutomatedTestFramework::mouse_motion_time_span);
  nux::SleepForMilliseconds(minimum_sleep_time);

  // Mouse down
  ViewSendMouseDown(view, button_index);

  // Drag to second point
  SendFakeMouseMotionEvent(r1.x, r1.y, NuxAutomatedTestFramework::mouse_motion_time_span);
  nux::SleepForMilliseconds(minimum_sleep_time);

  // Mouse up
  ViewSendMouseUp(view, button_index);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionTo(nux::View *view, int x, int y)
{
  nux::Rect r;
  if (view)
  {
    r = view->GetAbsoluteGeometry();
    r.OffsetPosition(window_x_ + x, window_y_ + y);
  }
  else
  {
    r = window_thread_->GetGraphicsDisplay().GetWindowGeometry();
    r.OffsetPosition(x, y);
  }

  SendFakeMouseMotionEvent(r.x, r.y, NuxAutomatedTestFramework::mouse_motion_time_span);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionToCenter(nux::View *view)
{
  nux::Rect r;
  if (view)
  {
    r = view->GetAbsoluteGeometry();
    r.OffsetPosition(window_x_, window_y_);
  }
  else
  {
    r = window_thread_->GetGraphicsDisplay().GetWindowGeometry();
  }

  int view_center_x = r.x + r.width/2;
  int view_center_y = r.y + r.height/2;

  SendFakeMouseMotionEvent(view_center_x, view_center_y, NuxAutomatedTestFramework::mouse_motion_time_span);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionToTopLeft(nux::View *view)
{
  nux::Rect r = view->GetAbsoluteGeometry();
  r.OffsetPosition(window_x_, window_y_);

  SendFakeMouseMotionEvent(r.x + safety_border_inside_view, r.y + safety_border_inside_view, NuxAutomatedTestFramework::mouse_motion_time_span);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionToTopRight(nux::View *view)
{
  nux::Rect r = view->GetAbsoluteGeometry();
  r.OffsetPosition(window_x_, window_y_);

  SendFakeMouseMotionEvent(r.x + r.width-1, r.y+safety_border_inside_view, NuxAutomatedTestFramework::mouse_motion_time_span);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionToBottomLeft(nux::View *view)
{
  nux::Rect r = view->GetAbsoluteGeometry();
  r.OffsetPosition(window_x_, window_y_);

  SendFakeMouseMotionEvent(r.x+safety_border_inside_view, r.y + r.height-1, NuxAutomatedTestFramework::mouse_motion_time_span);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionToBottomRight(nux::View *view)
{
  nux::Rect r = view->GetAbsoluteGeometry();
  r.OffsetPosition(window_x_, window_y_);

  SendFakeMouseMotionEvent(r.x + r.width-1, r.y + r.height-1, NuxAutomatedTestFramework::mouse_motion_time_span);
}

void NuxAutomatedTestFramework::ViewSendChar(const char c)
{
  KeySym modifier = 0;

  if ((c >= 'A') && (c <= 'Z'))
  {
    modifier = XK_Shift_L;
  }

  std::string s(1, c);

  if (c == ' ')
  {
    SendFakeKeyEvent(XK_space, modifier);
  }
  else
  {
    SendFakeKeyEvent(XStringToKeysym(s.c_str()), modifier);
  }

  nux::SleepForMilliseconds(300);
}

void NuxAutomatedTestFramework::ViewSendString(const std::string &str)
{
  int l = str.length();
  if (l == 0)
    return;

  int i = 0;

  while (i < l)
  {
    KeySym modifier = 0;
    char c = str[i++];

    if ((c >= 'A') && (c <= 'Z'))
    {
      modifier = XK_Shift_L;
    }

    std::string s(1, c);

    if (c == ' ')
    {
      SendFakeKeyEvent(XK_space, modifier);
    }
    else if (c == '^')
    {
      SendFakeKeyEvent(XK_asciicircum, XK_Shift_L);
    }
    else if (c == '~')
    {
      SendFakeKeyEvent(XK_asciitilde, XK_Shift_L);
    }
    else if (c == '`')
    {
      SendFakeKeyEvent(XK_asciitilde, 0);
    }
    else if (c == '=')
    {
      SendFakeKeyEvent(XK_equal, 0);
    }
    else if (c == '\"')
    {
      SendFakeKeyEvent(XK_quotedbl, XK_Shift_L);
    }
    else if (c == '!')
    {
      SendFakeKeyEvent(XK_exclam, XK_Shift_L);
    }
    else if (c == '|')
    {
      SendFakeKeyEvent(XK_bar, XK_Shift_L);
    }
    else if (c == '/')
    {
      SendFakeKeyEvent(XK_slash, 0);
    }
    else if (c == '\\')
    {
      SendFakeKeyEvent(XK_backslash, 0);
    }
    else if (c == '-')
    {
      SendFakeKeyEvent(XK_minus, 0);
    }
    else if (c == '+')
    {
      SendFakeKeyEvent(XK_plus, XK_Shift_L);
    }
    else if (c == ',')
    {
      SendFakeKeyEvent(XK_comma, 0);
    }
    else if (c == '_')
    {
      SendFakeKeyEvent(XK_underscore, XK_Shift_L);
    }
    else if (c == '<')
    {
      SendFakeKeyEvent(XK_comma, XK_Shift_L);
    }
    else if (c == '>')
    {
      SendFakeKeyEvent(XK_greater, XK_Shift_L);
    }
    else if (c == '.')
    {
      SendFakeKeyEvent(XK_period, 0);
    }
    else if (c == '?')
    {
      SendFakeKeyEvent(XK_question, XK_Shift_L);
    }
    else if (c == '\'')
    {
      SendFakeKeyEvent(XK_quoteright, 0);
    }
    else if (c == ';')
    {
      SendFakeKeyEvent(XK_semicolon, 0);
    }
    else if (c == ':')
    {
      SendFakeKeyEvent(XK_colon, XK_Shift_L);
    }
    else if (c == '%')
    {
      SendFakeKeyEvent(XK_percent, XK_Shift_L);
    }
    else if (c == '(')
    {
      SendFakeKeyEvent(XK_parenleft, XK_Shift_L);
    }
    else if (c == ')')
    {
      SendFakeKeyEvent(XK_parenright, XK_Shift_L);
    }
    else
    {
      SendFakeKeyEvent(XStringToKeysym(s.c_str()), modifier);
    }
    nux::SleepForMilliseconds(300);
  }
  XFlush(display_);
}

void NuxAutomatedTestFramework::ViewSendCompositionKeys(const std::string& str)
{
  int l = str.length();
  if (l == 0)
    return;

  SendFakeKeyEvent(XK_Multi_key, 0);
  ViewSendString(str);
}


// Send strings in the format of "key+key+key" up to 4 where the last key is not a modifier
// example "ctrl+alt+=
void NuxAutomatedTestFramework::ViewSendKeys(const std::string& str)
{
  std::string keys[MAX_KEYS];
  KeySym keys_sym[MAX_KEYS] = {0};
  size_t f_ptr = 0, s_ptr = 0;
  int i = 0;

  //Split up keys on the +
  while (s_ptr != std::string::npos && i < MAX_KEYS)
  {
    s_ptr = str.find("+", f_ptr);
    keys[i] = str.substr(f_ptr, (s_ptr-f_ptr));
    f_ptr = s_ptr+1;
    i++;
  }

  // Find what the key_sym should be for each key
  for (int i = 0; i < MAX_KEYS; i++)
  {
    if (keys[i].empty())
    {
      break;
    }
    else if (keys[i] == "ctrl")
    {
      keys_sym[i] = XK_Control_L;
    }
    else if (keys[i] == "alt")
    {
      keys_sym[i] = XK_Alt_L;
    }
    else if (keys[i] == "space")
    {
      keys_sym[i] = XK_space;
    }
    else if (keys[i] == "enter" || keys[i] == "return")
    {
      keys_sym[i] = XK_Return;
    }
    else if (keys[i] == "=")
    {
      keys_sym[i] = XK_equal;
    }
    else if (keys[i] == "[")
    {
      keys_sym[i] = XK_bracketleft;
    }
    else if (keys[i] =="shift")
    {
      keys_sym[i] = XK_Shift_L;
    }
    else
    {
      // If nothing is found try and find it...
      if (!(keys_sym[i] = XStringToKeysym(keys[i].c_str())))
        printf("Undefinded Key: %s (Add it to this list)\n", keys[i].c_str());
    }
  }
  XTestGrabControl(display_, True);
  KeyCode modcode[MAX_KEYS] = {0};

  // Press the keys from 0,1,2,3
  for (int i = 0; i < MAX_KEYS; i++)
  {
    if (keys_sym[i] != 0)
    {
      modcode[i] = XKeysymToKeycode(display_, keys_sym[i]);
      XTestFakeKeyEvent(display_, modcode[i], True, 0);
    }
  }

  // Release the keys from 3,2,1,0
  for (int i = MAX_KEYS-1; i >= 0; i--)
  {
    if (keys_sym[i] != 0)
    {
      XTestFakeKeyEvent(display_, modcode[i], False, 0);
    }
  }
  XTestGrabControl(display_, False);
  XFlush(display_);
}

void NuxAutomatedTestFramework::ViewSendKeyCombo(KeySym modsym0, KeySym modsym1, KeySym modsym2, const char c)
{
  KeyCode keycode = 0;
  KeyCode modcode0 = 0;
  KeyCode modcode1 = 0;
  KeyCode modcode2 = 0;

  if (c != 0)
  {
    printf("ViewSendKeyCombo");
    std::string s(1, c);
    keycode = XKeysymToKeycode(display_, XStringToKeysym(s.c_str()));
  }
  XTestGrabControl(display_, True);

  /* Generate modkey press */
  if (modsym0 != 0)
  {
    modcode0 = XKeysymToKeycode(display_, modsym0);
    XTestFakeKeyEvent(display_, modcode0, True, 0);
  }
  if (modsym1 != 0)
  {
    modcode1 = XKeysymToKeycode(display_, modsym1);
    XTestFakeKeyEvent(display_, modcode1, True, 0);
  }
  if (modsym2 != 0)
  {
    modcode2 = XKeysymToKeycode(display_, modsym2);
    XTestFakeKeyEvent(display_, modcode2, True, 0);
  }

  /* Generate regular key press and release */
  if (keycode)
  {
    XTestFakeKeyEvent(display_, keycode, True, 0);
    XTestFakeKeyEvent(display_, keycode, False, 0);
  }

  /* Generate modkey release */
  if (modsym0 != 0)
  {
    XTestFakeKeyEvent(display_, modcode0, False, 0);
  }
  if (modsym1 != 0)
  {
    XTestFakeKeyEvent(display_, modcode1, False, 0);
  }
  if (modsym2 != 0)
  {
    XTestFakeKeyEvent(display_, modcode2, False, 0);
  }

  XSync(display_, False);
  XTestGrabControl(display_, False);
}

void NuxAutomatedTestFramework::ViewSendCtrlA()
{
  ViewSendKeyCombo(XK_Control_L, 0, 0, 'a');
}

void NuxAutomatedTestFramework::ViewSendDelete()
{
  SendFakeKeyEvent(XK_Delete, 0);
}

void NuxAutomatedTestFramework::ViewSendBackspace()
{
  SendFakeKeyEvent(XK_BackSpace, 0);
}

void NuxAutomatedTestFramework::ViewSendEscape()
{
  SendFakeKeyEvent(XK_Escape, 0);
}

void NuxAutomatedTestFramework::ViewSendTab()
{
  SendFakeKeyEvent(XK_Tab, 0);
}

void NuxAutomatedTestFramework::ViewSendReturn()
{
  SendFakeKeyEvent(XK_Return, 0);
}

void NuxAutomatedTestFramework::ViewSendRight()
{
  SendFakeKeyEvent(XK_Right, 0);
}

void NuxAutomatedTestFramework::ViewSendLeft()
{
  SendFakeKeyEvent(XK_Left, 0);
}

void NuxAutomatedTestFramework::ViewSendUp()
{
  SendFakeKeyEvent(XK_Up, 0);
}

void NuxAutomatedTestFramework::ViewSendDown()
{
  SendFakeKeyEvent(XK_Down, 0);
}

void NuxAutomatedTestFramework::ViewSendIBusToggle()
{
  KeyCode modcode0 = 0;
  KeyCode modcode1 = 0;

  modcode0 = XKeysymToKeycode(display_, XK_Control_L);
  XTestFakeKeyEvent(display_, modcode0, True, 0);

  modcode1 = XKeysymToKeycode(display_, XK_space);
  XTestFakeKeyEvent(display_, modcode1, True, 0);

  // release
  /* Generate modkey release */
  XTestFakeKeyEvent(display_, modcode1, False, 0);
  XTestFakeKeyEvent(display_, modcode0, False, 0);
  XFlush(display_);
}

void NuxAutomatedTestFramework::PutMouseAt(int x, int y)
{
  XTestFakeMotionEvent(display_, XScreenNumberOfScreen(DefaultScreenOfDisplay(display_)), x, y, CurrentTime);
  XSync(display_, False);
}

void NuxAutomatedTestFramework::SendFakeKeyEvent(KeySym keysym, KeySym modsym)
{
  KeyCode keycode = 0;
  KeyCode modcode = 0;

  keycode = XKeysymToKeycode(display_, keysym);
  XTestGrabControl(display_, True);

  /* Generate modkey press */
  if (modsym != 0)
  {
    modcode = XKeysymToKeycode(display_, modsym);
    XTestFakeKeyEvent(display_, modcode, True, 0);
  }

  /* Generate regular key press and release */
  XTestFakeKeyEvent(display_, keycode, True, 0);
  XTestFakeKeyEvent(display_, keycode, False, 0);

  /* Generate modkey release */
  if (modsym != 0)
  {
    XTestFakeKeyEvent(display_, modcode, False, 0);
  }

  XSync(display_, False);
  XTestGrabControl(display_, False);
}

void NuxAutomatedTestFramework::SendFakeMouseEvent(int mouse_button_index, bool pressed)
{
  XTestFakeButtonEvent(display_, mouse_button_index, pressed,  CurrentTime);
  XSync(display_, False);
}

void NuxAutomatedTestFramework::SendFakeMouseMotionEvent(int x, int y, int ms_delay)
{
  XEvent event;
  /* Get the current pointer position */
  XQueryPointer(display_, RootWindow(display_, 0),
        &event.xbutton.root, &event.xbutton.window,
        &event.xbutton.x_root, &event.xbutton.y_root,
        &event.xbutton.x, &event.xbutton.y,
        &event.xbutton.state);

  int old_x = event.xbutton.x;
  int old_y = event.xbutton.y;

  int n_iteration = ms_delay / 16.0f;

  //nuxDebugMsg("n_iteration: %d", n_iteration);

  if (n_iteration < 1)
  {
    n_iteration = 1;
  }

  XSync(display_, False);

  for (int i = 0; i < n_iteration; i++)
  {
    float t = ((float)i + 1.0f) / n_iteration;

    int cx = (1.0f - t) * old_x + t * x;
    int cy = (1.0f - t) * old_y + t * y;
    XTestFakeMotionEvent(display_, XScreenNumberOfScreen(DefaultScreenOfDisplay(display_)), cx, cy, CurrentTime);
    XSync(display_, False);
    usleep(16*1000);
  }

  XTestFakeMotionEvent(display_, XScreenNumberOfScreen(DefaultScreenOfDisplay(display_)), x, y, CurrentTime);
  XSync(display_, False);
  nux::SleepForMilliseconds(NuxAutomatedTestFramework::minimum_sleep_time);
}

void NuxAutomatedTestFramework::TestReportMsg(bool b, const char* msg)
{
  if (b)
  {
    nuxOkMsg("%s: %s", msg, "Ok");
  }
  else
  {
    nuxAlertMsg("%s: %s", msg, "Failed");
  }
}
