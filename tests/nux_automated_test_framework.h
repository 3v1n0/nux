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

#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 

#ifndef NUX_AUTOMATED_TEST_FRAMEWORK_H
#define NUX_AUTOMATED_TEST_FRAMEWORK_H

class NuxAutomatedTestFramework
{
public:
  NuxAutomatedTestFramework(nux::WindowThread *window_thread);
  virtual ~NuxAutomatedTestFramework();

  void Startup();

  void ViewSendMouseClick(nux::View *view, int button);
  void ViewSendMouseDown(nux::View *view, int button);
  void ViewSendMouseUp(nux::View *view, int button);
  void ViewSendMouseDrag(nux::View *view, int button, int x0, int y0, int x1, int y1);
  void ViewSendMouseMotionTo(nux::View *view, int x, int y);
  void ViewSendMouseMotionToCenter(nux::View *view);
  void ViewSendMouseMotionToTopLeft(nux::View *view);
  void ViewSendMouseMotionToTopRight(nux::View *view);
  void ViewSendMouseMotionToBottomLeft(nux::View *view);
  void ViewSendMouseMotionToBottomRight(nux::View *view);

  void ViewSendChar(const char c);
  void ViewSendString(const std::string &str);
  void ViewSendKeyCombo(KeySym modsym0, KeySym modsym1, KeySym modsym2, const char c);
  void ViewSendCtrlA();
  void ViewSendDelete();
  void ViewSendBackspace();
  void ViewSendEscape();
  void ViewSendTab();
  void ViewSendReturn();


  void SendFakeMouseEvent(int mouse_button_index, bool pressed);
  void SendFakeKeyEvent(KeySym keysym, KeySym modsym);
  void SendFakeMouseMotionEvent(int x, int y, int ms_delay);

  void SetTerminateProgramWhenDone(bool terminate);
  bool WhenDoneTerminateProgram();
private:
  void WindowConfigSignal(int x, int y, int width, int height);
  
  bool ready_to_start_;
  Display* display_;
  nux::WindowThread *window_thread_;
  int window_x_;
  int window_y_;
  int window_width_;
  int window_height_;
  bool terminate_when_test_over_;

  static int mouse_motion_time_span;  // in seconds
  static int mouse_click_time_span;  // in seconds
  static int safety_border_inside_view;  // in pixels
};

#endif // NUX_AUTOMATED_TEST_FRAMEWORK_H

