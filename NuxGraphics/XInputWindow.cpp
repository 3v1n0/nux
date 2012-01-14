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
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */

#include "XInputWindow.h"
#include "GraphicsDisplayX11.h"
#include "GLThread.h"

// Jay, what is this for?  It isn't referenced anywhere.
#define xdnd_version 5

namespace nux
{
  std::vector<Window> XInputWindow::native_windows_;

  XInputWindow::XInputWindow(const char* title,
                             bool        take_focus,
                             int         override_redirect)
    : strutsEnabled_(false)
    , display_(GetGraphicsDisplay()->GetX11Display())
    , geometry_(0, 0, 1, 1)
    , shown_(false)
    , mapped_(false)
  {
    XSetWindowAttributes attrib;

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

    window_ = XCreateWindow(display_, XDefaultRootWindow(display_),
                            geometry_.x, geometry_.y,
                            geometry_.width, geometry_.height, 0,
                            CopyFromParent, InputOutput, CopyFromParent,
                            CWOverrideRedirect | CWEventMask, &attrib);

    native_windows_.push_back(window_);

    Atom data[32];
    int     i = 0;
    data[i++] = XInternAtom(display_, "_NET_WM_STATE_STICKY", 0);
    data[i++] = XInternAtom(display_, "_NET_WM_STATE_SKIP_TASKBAR", 0);
    data[i++] = XInternAtom(display_, "_NET_WM_STATE_SKIP_PAGER", 0);

    XChangeProperty(display_, window_,
                    XInternAtom(display_, "_NET_WM_STATE", 0),
                    XA_ATOM, 32, PropModeReplace,
                    (unsigned char *) data, i);

    Atom type[1];
    type[0] = XInternAtom(display_, "_NET_WM_WINDOW_TYPE_DOCK", 0);
    XChangeProperty(display_, window_,
                    XInternAtom(display_, "_NET_WM_WINDOW_TYPE", 0),
                    XA_ATOM, 32, PropModeReplace,
                    (unsigned char *) type, 1);

    XStoreName(display_, window_, title);
    EnsureInputs();

    if (take_focus)
      EnableTakeFocus();

    EnableDnd();
  }

  XInputWindow::~XInputWindow()
  {
    native_windows_.erase(std::find(native_windows_.begin(), native_windows_.end(), window_));
    XDestroyWindow(display_, window_);
  }

  /* static */
  std::vector<Window> const& XInputWindow::NativeHandleList()
  {
    return native_windows_;
  }

  void XInputWindow::SetStruts()
  {
    int n_info;
    XineramaScreenInfo *info = XineramaQueryScreens(display_, &n_info);
    Region             screen_region;
    Region             total_screen_region = XCreateRegion();
    Region             input_window_region = XCreateRegion();
    Region             intersection = XCreateRegion();
    XRectangle         monitor;
    XRectangle         tmp_rect;
    int largestWidth = 0, largestHeight = 0;
    int screenWidth, screenHeight;
    long int data[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    /* Find the screen that this region intersects */
    tmp_rect.x = geometry_.x;
    tmp_rect.y = geometry_.y;
    tmp_rect.width = geometry_.width;
    tmp_rect.height = geometry_.height;

    XUnionRectWithRegion(&tmp_rect, input_window_region, input_window_region);

    /* If there is no Xinerama data available just use the geometry we have */
    if (!info)
    {
      monitor = tmp_rect;
      n_info = 0;
    }

    for (int i = 0; i < n_info; i++)
    {
      tmp_rect.x = info[i].x_org;
      tmp_rect.y = info[i].y_org;
      tmp_rect.width = info[i].width;
      tmp_rect.height = info[i].height;

      screen_region = XCreateRegion();

      XUnionRectWithRegion(&tmp_rect, screen_region, screen_region);
      XUnionRegion(screen_region, total_screen_region, total_screen_region);
      XIntersectRegion(screen_region, input_window_region, intersection);

      if (!XEmptyRegion(intersection))
      {
        int width = intersection->extents.x2 - intersection->extents.x1;
        int height = intersection->extents.y2 - intersection->extents.y1;

        if ((width * height) > (largestWidth * largestHeight))
        {
          largestWidth = width;
          largestHeight = height;

          monitor.x      = info[i].x_org;
          monitor.y      = info[i].y_org;
          monitor.width  = info[i].width;
          monitor.height = info[i].height;
        }
      }

      XDestroyRegion(screen_region);
    }

    screenWidth = total_screen_region->extents.x2 - total_screen_region->extents.x1;
    screenHeight = total_screen_region->extents.y2 - total_screen_region->extents.y1;

    XDestroyRegion(input_window_region);
    XDestroyRegion(intersection);
    XDestroyRegion(total_screen_region);

    if (info)
      XFree(info);

    if (geometry_.width > geometry_.height)
    {
      if (geometry_.y - monitor.y < monitor.height / 2)
      {
        /* top */
        data[2] = geometry_.y + geometry_.height;
        data[8] = geometry_.x;
        data[9] = geometry_.x + geometry_.width - 1;
      }
      else
      {
        /* bottom */
        data[3] = (screenHeight - 1) - geometry_.y;
        data[10] = geometry_.x;
        data[11] = geometry_.x + geometry_.width - 1;
      }
    }
    else
    {
      if (geometry_.x - monitor.x < monitor.width / 2)
      {
        /* left */
        data[0] = geometry_.x + geometry_.width;
        data[4] = geometry_.y;
        data[5] = geometry_.y + geometry_.height - 1;
      }
      else
      {
        /* right */
        data[1] = (screenWidth - 1) - geometry_.y;
        data[6] = geometry_.y;
        data[7] = geometry_.y + geometry_.height - 1;
      }
    }

    XChangeProperty(display_, window_,
                    XInternAtom(display_, "_NET_WM_STRUT_PARTIAL", 0),
                    XA_CARDINAL, 32, PropModeReplace,
                    (unsigned char *) data, 12);
  }

  void XInputWindow::UnsetStruts()
  {
    XDeleteProperty(display_, window_,
                    XInternAtom(display_, "_NET_WM_STRUT_PARTIAL", 0));
  }

  void XInputWindow::EnableStruts(bool enable)
  {
    if (strutsEnabled_ == enable)
      return;

    strutsEnabled_ = enable;
    if (enable)
      SetStruts();
    else
      UnsetStruts();
  }

  bool XInputWindow::StrutsEnabled()
  {
    return strutsEnabled_;
  }

  void XInputWindow::EnsureInputs()
  {
    XSelectInput(display_, window_,
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

  void XInputWindow::EnableTakeFocus()
  {
    Atom wmTakeFocus = XInternAtom(display_, "WM_TAKE_FOCUS", False);
    XWMHints* wmHints = NULL;

    wmHints = (XWMHints*) calloc(1, sizeof(XWMHints));
    wmHints->flags |= InputHint;
    wmHints->input = False;
    XSetWMHints(display_, window_, wmHints);
    free(wmHints);
    XSetWMProtocols(display_, window_, &wmTakeFocus, 1);
  }

  void XInputWindow::EnableDnd()
  {
    int version = 5;
    XChangeProperty(display_, window_,
                    XInternAtom(display_, "XdndAware", false),
                    XA_ATOM, 32, PropModeReplace,
                    (unsigned char *) &version, 1);
  }

  void XInputWindow::DisableDnd()
  {
    XDeleteProperty(display_, window_,
                    XInternAtom(display_, "XdndAware", false));
  }

  //! Set the position and size of the window
  void XInputWindow::SetGeometry(Rect const& geo)
  {
    geometry_ = geo;

    if (shown_)
      XMoveResizeWindow(display_, window_,
                        geo.x, geo.y, geo.width, geo.height);
    EnsureInputs();

    if (strutsEnabled_)
      SetStruts();
  }

  //! Set the position and size of the window
  void XInputWindow::SetGeometry(int x, int y, int width, int height)
  {
    SetGeometry(Rect(x, y, width, height));
  }

  //! Get the window geometry.
  Rect const& XInputWindow::GetGeometry() const
  {
    return geometry_;
  }

  Window XInputWindow::GetWindow()
  {
    return window_;
  }

  void XInputWindow::SetInputFocus()
  {
    XSetInputFocus(display_, window_, RevertToParent, CurrentTime);
  }

  void XInputWindow::Hide()
  {
    XMoveResizeWindow(display_, window_,
                      -100 - geometry_.width,
                      -100 - geometry_.height,
                      geometry_.width,
                      geometry_.height);
    shown_ = false;
  }

  void XInputWindow::Show()
  {
    shown_ = true;

    if (!mapped_)
    {
      XMapRaised(display_, window_);
      mapped_ = true;

      XEvent xevent;
      while (XCheckTypedWindowEvent(display_, window_, MapNotify, &xevent));
      XSetInputFocus(display_, window_, RevertToParent, CurrentTime);

    }
    XMoveResizeWindow(display_, window_,
                      geometry_.x, geometry_.y,
                      geometry_.width, geometry_.height);
  }
}
