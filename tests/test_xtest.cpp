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
#include "Nux/WindowThread.h"
#include "Nux/VLayout.h"
#include "Nux/Button.h"
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 

nux::TimeOutSignal *timeout_signal = NULL;
nux::Button *button = NULL;

bool Go = false;

void WaitForConfigureEvent(int x, int y, int width, int height)
{
  Go = true;
}

void TimeOutCallback(void *data)
{
  //nux::WindowThread *thread = static_cast<nux::WindowThread*>(data);
  //thread->NuxMainLoopQuit();
}

void UserInterfaceInitialization(nux::NThread* thread, void* init_data)
{ 
  nux::VLayout *main_layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  button = new nux::Button("NUX", NUX_TRACKER_LOCATION);
  button->SetLabelFontSize(76);

  main_layout->AddView(button, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_MATCHCONTENT);
  main_layout->SetVerticalInternalMargin(10);
  main_layout->SetContentDistribution(nux::MAJOR_POSITION_CENTER);

  static_cast<nux::WindowThread*>(thread)->SetLayout(main_layout);
  
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);

  //timeout_signal = new nux::TimeOutSignal();
  //timeout_signal->time_expires.connect(sigc::ptr_fun(TimeOutCallback));
  //static_cast<nux::WindowThread*>(thread)->GetTimerHandler().AddTimerHandler(6000, timeout_signal, thread, NULL);
}

class NuxAutomatedTestFramework
{
public:
  NuxAutomatedTestFramework(nux::WindowThread *window_thread);
  virtual ~NuxAutomatedTestFramework();

  void Startup();
  virtual int StartAutomatedTest();

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

  void ViewSendChar(nux::View *view, const char c, KeySym modifier);
  void ViewSendString(nux::View *view, const std::string &str, KeySym modifier);


  void SendFakeMouseEvent(int mouse_button_index, bool pressed);
  void SendFakeKeyEvent(KeySym keysym, KeySym modsym);
  void SendFakeMouseMotionEvent(int x, int y, int ms_delay);

private:
  void WindowConfigSignal(int x, int y, int width, int height);
  
  bool ready_to_start_;
  Display* display_;
  nux::WindowThread *window_thread_;
  int window_x_;
  int window_y_;
  int window_width_;
  int window_height_;
};

NuxAutomatedTestFramework::NuxAutomatedTestFramework(nux::WindowThread *window_thread)
{
  ready_to_start_ = false;
  display_ = NULL;
  window_thread_ = window_thread;
  window_x_ = 0;
  window_y_ = 0;
  window_width_ = 0;
  window_height_ = 0;
}

NuxAutomatedTestFramework::~NuxAutomatedTestFramework()
{
  XCloseDisplay(display_);
}

void NuxAutomatedTestFramework::Startup()
{
  while (Go == false)
  {
    nuxDebugMsg("Waiting to start");
    nux::SleepForMilliseconds(300);
  }

  display_ = XOpenDisplay(NULL);
  nux::Geometry rect = window_thread_->GetWindow().GetWindowGeometry();
  nuxDebugMsg("Window geometry: (%d, %d, %d, %d)", rect.x, rect.y, rect.width, rect.height);

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
    nux::Rect r = view->GetAbsoluteGeometry();
    r.OffsetPosition(window_x_ + r.width/2, window_y_ + r.height/2);
  }
  else
  {
    r = window_thread_->GetWindow().GetWindowGeometry();
    r.OffsetPosition(r.width/2, r.height/2);
  }

  SendFakeMouseMotionEvent(r.x, r.y, 1000);
  nux::SleepForMilliseconds(600);
  SendFakeMouseEvent(button, true);
  nux::SleepForMilliseconds(600);
  SendFakeMouseEvent(button, false);
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
    SendFakeMouseMotionEvent(view_center_x, view_center_y, 1000);
    nux::SleepForMilliseconds(600);
  }
  SendFakeMouseEvent(button, true);
}

void NuxAutomatedTestFramework::ViewSendMouseUp(nux::View *view, int button)
{
  //   nux::Rect r = view->GetAbsoluteGeometry();
  // r.OffsetPosition(window_x_, window_y_);

  // int view_center_x = r.x + r.width/2;
  // int view_center_y = r.y + r.height/2;

  // SendFakeMouseMotionEvent(view_center_x, view_center_y, 1000);
  // nux::SleepForMilliseconds(600);
  SendFakeMouseEvent(button, false);
}

void NuxAutomatedTestFramework::ViewSendMouseDrag(nux::View *view, int button_index, int x0, int y0, int x1, int y1)
{
  nux::Rect r0 = view->GetAbsoluteGeometry();
  nux::Rect r1 = view->GetAbsoluteGeometry();
  r0.OffsetPosition(window_x_ + x0, window_y_ + y0);
  r1.OffsetPosition(window_x_ + x1, window_y_ + y1);

  // Go to first point
  SendFakeMouseMotionEvent(r0.x, r0.y, 1000);
  nux::SleepForMilliseconds(600);
  
  // Mouse down
  ViewSendMouseDown(view, button_index);

  // Drag to second point
  SendFakeMouseMotionEvent(r1.x, r1.y, 1000);
  nux::SleepForMilliseconds(600);

  // Mouse up
  ViewSendMouseUp(view, button_index);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionTo(nux::View *view, int x, int y)
{
  nux::Rect r;
  if (view)
  {
    nux::Rect r = view->GetAbsoluteGeometry();
    r.OffsetPosition(window_x_ + x, window_y_ + y);
  }
  else
  {
    r = window_thread_->GetWindow().GetWindowGeometry();
    r.OffsetPosition(x, y);
  }

  SendFakeMouseMotionEvent(r.x, r.y, 1000);
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
    r = window_thread_->GetWindow().GetWindowGeometry();
  }

  int view_center_x = r.x + r.width/2;
  int view_center_y = r.y + r.height/2;

  SendFakeMouseMotionEvent(view_center_x, view_center_y, 1000);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionToTopLeft(nux::View *view)
{
  nux::Rect r = view->GetAbsoluteGeometry();
  r.OffsetPosition(window_x_, window_y_);

  SendFakeMouseMotionEvent(r.x+1, r.y+1, 1000);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionToTopRight(nux::View *view)
{
  nux::Rect r = view->GetAbsoluteGeometry();
  r.OffsetPosition(window_x_, window_y_);

  SendFakeMouseMotionEvent(r.x + r.width-1, r.y+1, 1000);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionToBottomLeft(nux::View *view)
{
  nux::Rect r = view->GetAbsoluteGeometry();
  r.OffsetPosition(window_x_, window_y_);

  SendFakeMouseMotionEvent(r.x+1, r.y + r.height-1, 1000);
}

void NuxAutomatedTestFramework::ViewSendMouseMotionToBottomRight(nux::View *view)
{
  nux::Rect r = view->GetAbsoluteGeometry();
  r.OffsetPosition(window_x_, window_y_);

  SendFakeMouseMotionEvent(r.x + r.width-1, r.y + r.height-1, 1000);
}

void NuxAutomatedTestFramework::ViewSendChar(nux::View *view, const char c, KeySym modifier)
{
  ViewSendMouseClick(view, 1);
  SendFakeKeyEvent(XK_A /*XKeysymToKeycode(display_, XK_A XStringToKeysym(&c))*/, modifier);
  nux::SleepForMilliseconds(600);
}

void NuxAutomatedTestFramework::ViewSendString(nux::View *view, const std::string &str, KeySym modifier)
{
  int l = str.length();
  if (l == 0)
    return;
  
  ViewSendMouseClick(view, 1);

  int i = 0;

  while (i < l)
  {
    char c = str[i++];
    SendFakeKeyEvent(XKeysymToKeycode(display_, XStringToKeysym(&c)), modifier);
    nux::SleepForMilliseconds(600);
  }
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

  XTestFakeMotionEvent(display_, XScreenNumberOfScreen(DefaultScreenOfDisplay(display_)), 0, 0, CurrentTime);
  XSync(display_, False);

  for (int i = 0; i < n_iteration; i++)
  {
    float t = ((float)i + 1.0f) / n_iteration;

    int cx = (1.0f - t) * old_x + t * x;
    int cy = (1.0f - t) * old_y + t * y;
    XTestFakeMotionEvent(display_, XScreenNumberOfScreen(DefaultScreenOfDisplay(display_)), cx, cy, CurrentTime);
    XSync(display_, False);
    //nuxDebugMsg("Iteration: %d", i);
    usleep(16*1000);
  }

  //XTestFakeMotionEvent(display_, XScreenNumberOfScreen(DefaultScreenOfDisplay(disp)), x, y, CurrentTime);  
  XSync(display_, False);
}

int NuxAutomatedTestFramework::StartAutomatedTest()
{
  ViewSendMouseMotionToCenter(0);
  ViewSendMouseClick(0, 1);

  SendFakeKeyEvent(XK_N, 0);
  SendFakeKeyEvent(XK_U, 0);
  SendFakeKeyEvent(XK_X, 0);

  //ViewSendMouseDrag(button, 1, 0, 0, 50, 50);

  //ViewSendMouseMotionTo(button, 100, 100);

  // ViewSendMouseMotionToBottomRight(button);
  // ViewSendMouseMotionToBottomLeft(button);
  // ViewSendMouseMotionToTopLeft(button);
  // ViewSendMouseMotionToTopRight(button);

  //ViewSendChar(button, 'n', 0);
}

void SimpleThread(nux::NThread *thread, void *user_data)
{
  
  nux::WindowThread *wnd_thread = static_cast<nux::WindowThread*>(user_data);
  NuxAutomatedTestFramework test(wnd_thread);
  test.Startup();
  test.StartAutomatedTest();

  //wnd_thread->NuxMainLoopQuit();
  nuxDebugMsg("Exit system thread");
}

int main(int argc, char **argv)
{
  // Initialize Nux subsystem
  nux::NuxInitialize (0);

  int xstatus = XInitThreads();
  nuxAssertMsg(xstatus > 0, "XInitThreads has failed");

  // Create a Window thread
  nux::WindowThread* wnd_thread = nux::CreateGUIThread(
    TEXT("XTest"),
    300,
    200,
    0,
    &UserInterfaceInitialization,
    0);

  // Create a system thread
  nux::SystemThread *system_thread = nux::CreateSystemThread(NULL, &SimpleThread, wnd_thread);

  wnd_thread->window_configuration.connect(sigc::ptr_fun(WaitForConfigureEvent));


  // Start the system thread and return.
  system_thread->Start(NULL);

  // Start the window thread
  wnd_thread->Run (0);

  delete wnd_thread;
  nuxDebugMsg("Exit program");
  return 0;
}
