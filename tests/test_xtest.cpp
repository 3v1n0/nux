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
#include <X11/extensions/XTest.h>
#include <X11/keysym.h> 

nux::TimeOutSignal *timeout_signal = NULL;


void TimeOutCallback(void *data)
{
  //nux::WindowThread *thread = static_cast<nux::WindowThread*>(data);
  //thread->NuxMainLoopQuit();
}

void UserInterfaceInitialization(nux::NThread* thread, void* init_data)
{
 
  nux::ColorLayer background(nux::Color(0xFF4D4D4D));
  static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);

  //timeout_signal = new nux::TimeOutSignal();
  //timeout_signal->time_expires.connect(sigc::ptr_fun(TimeOutCallback));
  //static_cast<nux::WindowThread*>(thread)->GetTimerHandler().AddTimerHandler(3000, timeout_signal, thread, NULL);
}

#define NBR_KEYS 3

void SendFakeKeyEvent(Display *disp, KeySym keysym, KeySym modsym)
{
  KeyCode keycode = 0, modcode = 0;
  
  keycode = XKeysymToKeycode (disp, keysym);  
  XTestGrabControl (disp, True);  
  
  /* Generate modkey press */  
  if (modsym != 0)  
  {  
     modcode = XKeysymToKeycode(disp, modsym);  
     XTestFakeKeyEvent (disp, modcode, True, 0);  
  }  
  
  /* Generate regular key press and release */  
  XTestFakeKeyEvent (disp, keycode, True, 0);  
  XTestFakeKeyEvent (disp, keycode, False, 0);  
  
  /* Generate modkey release */  
  if (modsym != 0)  
    XTestFakeKeyEvent (disp, modcode, False, 0);  
  
  XSync (disp, False);
  XTestGrabControl (disp, False);  
} 

void SendFakeMouseEvent(Display *disp, int mouse_button_index, bool up)
{
  XTestFakeButtonEvent (disp, 3, True,  CurrentTime);
  XSync (disp, False);
}

void SendFakeMouseMotionEvent(Display *disp, int x, int y)
{
  XTestFakeMotionEvent (disp, 0, x, y, CurrentTime);  
  XSync (disp, False);
}


void SimpleThread(nux::NThread *thread, void *user_data)
{
  nux::WindowThread *wnd_thread = static_cast<nux::WindowThread*>(user_data);
  //nux::SleepForMilliseconds(300);

  Display* disp = XOpenDisplay(NULL);

  if (disp == NULL)
  {
    return; 
  }
  
  SendFakeKeyEvent(disp, XK_S, 0);
  //nux::SleepForMilliseconds(1000);
  SendFakeKeyEvent(disp, XK_A, 0);
  //nux::SleepForMilliseconds(3000);
  SendFakeKeyEvent(disp, XK_X, 0);
  nux::SleepForMilliseconds(3000);

  XCloseDisplay(disp); 

  wnd_thread->NuxMainLoopQuit();
  nuxDebugMsg("Exit system thread");
}

int main(int argc, char **argv)
{
  // // Initialize Nux subsystem
  nux::NuxInitialize (0);

  // Create a Window thread
  nux::WindowThread* wt = nux::CreateGUIThread(
    TEXT("XTest"),
    300,
    200,
    0,
    &UserInterfaceInitialization,
    0);

  // Create a system thread
  nux::SystemThread *system_thread = nux::CreateSystemThread(NULL, &SimpleThread, wt);
  
  // Start the system thread and return.
  system_thread->Start(NULL);

  // Start the window thread
  wt->Run (0);

  delete wt;
  return 0;
}
