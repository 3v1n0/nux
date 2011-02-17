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
 * Authored by: Jay Taoko <jaytaoko@inalogic.com>
 *
 */


#include "Nux.h"
#include "WindowCompositor.h"
#include "PopUpWindow.h"

namespace nux
{

  PopUpWindow::PopUpWindow()
  {

    EnableTitleBar (false);
    ShowWindow (false);
    SetTopBorder (0);
    SetBorder (0);
    SetWindowSizeMatchLayout (true);
    SetMinimumSize (20, 20);
  }


  PopUpWindow::~PopUpWindow()
  {

  }

  void PopUpWindow::Show()
  {
    //m_Window.ComputeChildLayout();
    ShowWindow (true);
  }

  void PopUpWindow::Hide()
  {
    ShowWindow (false);
  }

}
