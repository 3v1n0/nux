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
#include "Nux/VLayout.h"
#include "Nux/HLayout.h"
#include "Nux/WindowThread.h"
#include "Nux/TextEntry.h"
 
void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
    nux::VLayout* MainVLayout = new nux::VLayout(NUX_TRACKER_LOCATION);
    
    MainVLayout->SetVerticalInternalMargin(10);

    MainVLayout->SetContentDistribution(nux::eStackCenter);
    nux::GetWindowThread ()->SetLayout(MainVLayout);
    nux::ColorLayer background(nux::Color(0xFF4D4D4D));
    static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}

int main(int argc, char **argv)
{
    nux::NuxInitialize(0);
    nux::WindowThread* wt = nux::CreateGUIThread(TEXT("Text Entry"), 400, 300, 0, &ThreadWidgetInit, 0);
   
    // Test to see if the variable in GraphicsDisplay is initialized
    int is_initalized = 1;    
    nux::GetGraphicsDisplay()->PointerIsGrabbed() == false ? is_initalized = 0 : is_initalized = 1;
 
    // Dont want to make the window pop up 
    //wt->Run(NULL);

    delete wt;
    return is_initalized;
}
