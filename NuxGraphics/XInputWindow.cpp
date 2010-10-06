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
#include "GLThread.h"

namespace nux
{

  XInputWindow::XInputWindow()
  {
    // Create the the X window here
    // The Display is provided by 
    //      Display* d = GetThreadGLWindow()->GetX11Display()
    // The Compiz window is provided by 
    //      Window w = GetThreadGLWindow()->GetWindowHandle()
    
    
  }
  
  XInputWindow::~XInputWindow()
  {

  }

  //! Set the position and size of the window
  void XInputWindow::SetGeometry(const Rect& geo)
  {

  }

  //! Set the position and size of the window
  void XInputWindow::SetGeometry(int x, int y, int width, int height)
  {

  }

  //! Get the window geometry.
  Rect XInputWindow::GetGeometry() const
  {
    Rect r(0, 0, 0, 0);
    
    return r;
  }
}



