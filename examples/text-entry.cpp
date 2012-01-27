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

void TextEntrySetup(nux::NThread* thread, void* InitData)
{
    nux::VLayout* main_layout = new nux::VLayout("", NUX_TRACKER_LOCATION);

    nux::TextEntry* text_entry = new nux::TextEntry("Hello World!", NUX_TRACKER_LOCATION);

    main_layout->SetPadding(10, 10, 10, 10);

    main_layout->AddView(text_entry, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
    main_layout->SetContentDistribution(nux::MAJOR_POSITION_CENTER);
    
    nux::GetWindowThread ()->SetLayout(main_layout);
    nux::ColorLayer background(nux::Color(0xFF4D4D4D));
    static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}

int main(int argc, char** argv)
{
    nux::NuxInitialize(0);
    nux::WindowThread* wt = nux::CreateGUIThread("Text Entry", 600, 300, 0, &TextEntrySetup, 0);
    wt->Run(NULL);

    delete wt;
    return 0;
}
