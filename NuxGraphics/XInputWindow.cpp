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

#include "XInputWindow.h"
#include "GfxSetupX11.h"
#include "GLThread.h"

namespace nux
{

  XInputWindow::XInputWindow()
  {
    Display* d = GetThreadGLWindow()->GetX11Display();
    XSetWindowAttributes attrib;
    
    _x = -100;
    _y = -100;
    _width = 1;
    _height = 1;
    
    attrib.override_redirect = 1;
    _window = XCreateWindow (d, XDefaultRootWindow (d), _x, _y, _width, _height, 0,
                             CopyFromParent, InputOnly, CopyFromParent,
                             CWOverrideRedirect, &attrib);
                                     
    XMapRaised (d, _window);
    
    Atom data[32];
    int     i = 0;
    data[i++] = XInternAtom (d, "_NET_WM_STATE_STICKY", 0);
    data[i++] = XInternAtom (d, "_NET_WM_STATE_SKIP_TASKBAR", 0);
    data[i++] = XInternAtom (d, "_NET_WM_STATE_SKIP_PAGER", 0);
    data[i++] = XInternAtom (d, "_NET_WM_STATE_ABOVE", 0);

    XChangeProperty (d, _window, XInternAtom (d, "_NET_WM_STATE", 0),
                 XA_ATOM, 32, PropModeReplace,
                 (unsigned char *) data, i);
                 
    EnsureInputs ();

    /* terrible hack */
    g_timeout_add (5000, (GSourceFunc) &XInputWindow::EnsureInputOnTimeout, this);   
  }
  
  XInputWindow::~XInputWindow()
  {
    Display* d = GetThreadGLWindow()->GetX11Display();
    XDestroyWindow (d, _window);
  }
  
  bool
  XInputWindow::EnsureInputOnTimeout (void *data)
  {
    XInputWindow *input_window = (XInputWindow*) data;
    input_window->EnsureInputs();
    
    return false;
  }

  void XInputWindow::EnsureInputs()
  {
    Display* d = GetThreadGLWindow()->GetX11Display();
    
    XSelectInput (d, _window,
                  KeyPressMask       | 
                  KeyReleaseMask     | 
                  ButtonPressMask    | 
                  ButtonReleaseMask  |
                  EnterWindowMask    |
                  LeaveWindowMask    |
                  PointerMotionMask  |
                  ButtonMotionMask   |
                  PropertyChangeMask |
                  FocusChangeMask);
    
  }

  //! Set the position and size of the window
  void XInputWindow::SetGeometry(const Rect& geo)
  {
    SetGeometry(geo.x, geo.y, geo.width, geo.height);
  }

  //! Set the position and size of the window
  void XInputWindow::SetGeometry(int x, int y, int width, int height)
  {
    Display* d = GetThreadGLWindow()->GetX11Display();
    
    _x = x;
    _y = y;
    _width = width;
    _height = height;
    
    XMoveResizeWindow (d, _window, x, y, width, height);
    EnsureInputs ();
  }

  //! Get the window geometry.
  Rect XInputWindow::GetGeometry() const
  {
    Rect r(_x, _y, _width, _height);
    
    return r;
  }
}



