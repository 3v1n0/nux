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
#include "GraphicsDisplayX11.h"
#include "GLThread.h"

#define xdnd_version 5

namespace nux
{

  std::list<Window> XInputWindow::_native_windows;

  XInputWindow::XInputWindow(const char* title,
                             bool        take_focus,
                             int         override_redirect)
  {
    Display* d = GetThreadGLWindow()->GetX11Display();
    _display = d;
    XSetWindowAttributes attrib;
    
    _x = 0;
    _y = 0;
    _width = 1;
    _height = 1;
    _strutsEnabled = false;
    
    attrib.override_redirect = override_redirect;
    attrib.event_mask = KeyPressMask        |
                        KeyReleaseMask      |
                        ButtonPressMask     |
                        ButtonReleaseMask   |
                        EnterWindowMask     |
                        LeaveWindowMask     |
                        PointerMotionMask   |
                        ButtonMotionMask    |
                        PropertyChangeMask  |
                        StructureNotifyMask |
                        FocusChangeMask;

    _window = XCreateWindow (d, XDefaultRootWindow (d), _x, _y, _width, _height, 0,
                             CopyFromParent, InputOutput, CopyFromParent,
                             CWOverrideRedirect | CWEventMask, &attrib);
    
    _native_windows.push_front (_window);
    
    Atom data[32];
    int     i = 0;
    data[i++] = XInternAtom (d, "_NET_WM_STATE_STICKY", 0);
    data[i++] = XInternAtom (d, "_NET_WM_STATE_SKIP_TASKBAR", 0);
    data[i++] = XInternAtom (d, "_NET_WM_STATE_SKIP_PAGER", 0);

    XChangeProperty (d, _window, XInternAtom (d, "_NET_WM_STATE", 0),
                 XA_ATOM, 32, PropModeReplace,
                 (unsigned char *) data, i);
    
    Atom type[1];
    type[0] = XInternAtom (d, "_NET_WM_WINDOW_TYPE_DOCK", 0);
    XChangeProperty (d, _window, XInternAtom (d, "_NET_WM_WINDOW_TYPE", 0),
                     XA_ATOM, 32, PropModeReplace,
                     (unsigned char *) type, 1);

    XStoreName (d, _window, title);

    XMapRaised (d, _window);
    EnsureInputs ();

    if (take_focus)
      EnableTakeFocus ();

    EnableDnd ();
  }

  XInputWindow::~XInputWindow()
  {
    _native_windows.remove (_window);
    XDestroyWindow (_display, _window);
  }

  /* static */
  std::list<Window> XInputWindow::NativeHandleList()
  {
    return _native_windows;
  }

  void XInputWindow::SetStruts()
  {
    int screenHeight, screenWidth;
    long int data[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    screenHeight = XDisplayHeight (_display, 0);
    screenWidth = XDisplayWidth (_display, 0);    
    
    if (_width > _height)
    {
      if (_y < screenHeight / 2)
      {
        /* top */
        data[2] = _y + _height;
        data[8] = _x;
        data[9] = _x + _width - 1;
      }
      else
      {
        /* bottom */
        data[3] = (screenHeight - 1) - _y;
        data[10] = _x;
        data[11] = _x + _width - 1;
      }
    }
    else
    {
      if (_x < screenWidth / 2)
      {
        /* left */
        data[0] = _x + _width;
        data[4] = _y;
        data[5] = _y + _height - 1;
      }
      else
      {
        /* right */
        data[1] = (screenWidth - 1) - _y;
        data[6] = _y;
        data[7] = _y + _height - 1;
      }
    }
    
    XChangeProperty (_display, _window, XInternAtom (_display, "_NET_WM_STRUT_PARTIAL", 0),
                     XA_CARDINAL, 32, PropModeReplace,
                     (unsigned char *) data, 12);
  }

  void XInputWindow::UnsetStruts()
  {
    int i;
    
    int data[12];
    
    for (i = 0; i < 12; i++)
      data[i] = 0;
    
    XChangeProperty (_display, _window, XInternAtom (_display, "_NET_WM_STRUT_PARTIAL", 0),
                     XA_CARDINAL, 32, PropModeReplace,
                     (unsigned char *) data, 12);
  }

  void XInputWindow::EnableStruts(bool enable)
  {
    if (_strutsEnabled == enable)
      return;
    
    _strutsEnabled = enable;
    if (enable)
      SetStruts();
    else
      UnsetStruts();
  }

  bool XInputWindow::StrutsEnabled()
  {
    return _strutsEnabled;
  }

  void XInputWindow::EnsureInputs()
  {
    XSelectInput (_display, _window,
                  KeyPressMask        |
                  KeyReleaseMask      |
                  ButtonPressMask     |
                  ButtonReleaseMask   |
                  EnterWindowMask     |
                  LeaveWindowMask     |
                  PointerMotionMask   |
                  ButtonMotionMask    |
                  PropertyChangeMask  |
                  StructureNotifyMask |
                  FocusChangeMask);
    
  }

  void XInputWindow::EnableTakeFocus ()
  {
    Atom      wmTakeFocus = XInternAtom (_display, "WM_TAKE_FOCUS", False);
    XWMHints* wmHints     = NULL;

    wmHints = (XWMHints*) calloc (1, sizeof (XWMHints));
    wmHints->flags |= InputHint;
    wmHints->input = False;
    XSetWMHints (_display, _window, wmHints);
    free (wmHints);
    XSetWMProtocols (_display, _window, &wmTakeFocus, 1);
  }

  void XInputWindow::EnableDnd ()
  {
    int version = 5;
    XChangeProperty (_display, _window, XInternAtom (_display, "XdndAware", false),
                     XA_ATOM, 32, PropModeReplace, (unsigned char *) &version, 1);
  }
  
  void XInputWindow::DisableDnd ()
  {
    XDeleteProperty (_display, _window, XInternAtom (_display, "XdndAware", false));
  }

  //! Set the position and size of the window
  void XInputWindow::SetGeometry(const Rect& geo)
  {
    SetGeometry(geo.x, geo.y, geo.width, geo.height);
  }

  //! Set the position and size of the window
  void XInputWindow::SetGeometry(int x, int y, int width, int height)
  {
    _x = x;
    _y = y;
    _width = width;
    _height = height;
    
    XMoveResizeWindow (_display, _window, x, y, width, height);
    EnsureInputs ();
    
    if (_strutsEnabled)
      SetStruts ();
  }

  //! Get the window geometry.
  Rect XInputWindow::GetGeometry() const
  {
    Rect r(_x, _y, _width, _height);
    
    return r;
  }

  Window XInputWindow::GetWindow ()
  {
    return _window;
  }

  void XInputWindow::SetInputFocus ()
  {
    XSetInputFocus (_display, _window, RevertToParent, CurrentTime);
  }
}

