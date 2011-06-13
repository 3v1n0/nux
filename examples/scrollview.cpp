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
#include "Nux/ScrollView.h"
#include "Nux/GridHLayout.h"
#include "Nux/GridVLayout.h"
#include "Nux/TextureArea.h"
#include "Nux/Panel.h"

void ThreadWidgetInit(nux::NThread* thread, void* InitData)
{
    nux::VLayout* MainVLayout = new nux::VLayout(NUX_TRACKER_LOCATION);

    nux::GridHLayout *grid_h_layout ((new nux::GridHLayout (NUX_TRACKER_LOCATION)));
    for (int i = 0; i < 5360; i++)
    {
      nux::ColorLayer color (nux::color::RandomColor ());
      nux::TextureArea* texture_area = new nux::TextureArea ();
      texture_area->SetPaintLayer (&color);

      grid_h_layout->AddView (texture_area, 1, nux::eLeft, nux::eFull);
    }

    grid_h_layout->ForceChildrenSize (true);
    grid_h_layout->SetChildrenSize (64, 42);
    grid_h_layout->EnablePartialVisibility (true);

    grid_h_layout->SetVerticalExternalMargin (4);
    grid_h_layout->SetHorizontalExternalMargin (4);
    grid_h_layout->SetVerticalInternalMargin (4);
    grid_h_layout->SetHorizontalInternalMargin (4);


    nux::ScrollView *scroll_view = new nux::ScrollView(NUX_TRACKER_LOCATION);
    
    grid_h_layout->SetStretchFactor(1);
    scroll_view->SetLayout(grid_h_layout);

    MainVLayout->AddView(scroll_view, 1, nux::eCenter, nux::eFull);
    MainVLayout->SetContentDistribution(nux::eStackCenter);
    
    nux::GetWindowThread ()->SetLayout(MainVLayout);
    nux::ColorLayer background(nux::Color(0xFF4D4D4D));
    static_cast<nux::WindowThread*>(thread)->SetWindowBackgroundPaintLayer(&background);
}

int main(int argc, char **argv)
{
    nux::NuxInitialize(0);
    nux::WindowThread* wt = nux::CreateGUIThread(TEXT("ScrollView"), 400, 300, 0, &ThreadWidgetInit, 0);
    wt->Run(NULL);

    delete wt;
    return 0;
}
