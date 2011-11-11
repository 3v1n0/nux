/*
 * Copyright 2011 Inalogic Inc.
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

#include <iostream>

#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/WindowThread.h"

void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
  nux::VLayout* layout = new nux::VLayout(NUX_TRACKER_LOCATION);
  nux::GetWindowThread()->SetLayout(layout);
}

int main(int argc, char **argv)
{
    nux::NuxInitialize(0);
    // When the GUI thread is created, it creates the GraphicsDisplay.
    nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Text Entry"),
                                                 400, 300, 0, &ThreadWidgetInit, 0);

    // Test to see if the variable in GraphicsDisplay is initialized
    int result = 1;
    if (!nux::GetGraphicsDisplay()->PointerIsGrabbed())
    {
      // Zero return code is good.
      result = 0;
    }
    else
    {
      std::cerr << "FAIL: GraphicsDisplay hasn't correctly initialized.\n";
    }

    // Dont want to make the window pop up
    //wt->Run(NULL);

    delete wt;
    return result;
}
